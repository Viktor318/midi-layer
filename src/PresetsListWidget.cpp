/*
    MidiLayer
    Copyright (C) 2012-2022, António José Soares Maia <maia.ajs@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/


#include "PresetsListWidget.h"


PresetsListWidget::PresetsListWidget(QWidget *parent, QWidget * centralWidget) :
    QWidget(parent)
{
    this->parentWidget = parent;
    this->centralWidget = centralWidget;

    currentPreset = new PresetWidget(this->centralWidget);
    currentPreset->setEnabled(false);

    Utils::get()->configDisplayFont(&listFont);

    dockLayout = new QVBoxLayout();
    dockLayout->setSpacing(8);

    presetList = new QListWidget();
    presetList->setFont(listFont);
    presetList->setStyleSheet(QString::fromUtf8("background-color: #25303d; color: #FFFFFF;"));
    presetList->setDragDropMode(QAbstractItemView::NoDragDrop);
    presetList->setDefaultDropAction(Qt::TargetMoveAction);

    bankNameEdit = new QLineEdit(this);
    QFont font;
    Utils::get()->configDisplayFont(&font);
    bankNameEdit->setFont(font);
    bankNameEdit->setStyleSheet(QString::fromUtf8("background-color: #8aa1be; color: #000000; padding-left:4px;"));
    bankNameEdit->setMinimumHeight(29);
    bankNameEdit->setMaximumHeight(29);
    bankNameEdit->setMaximumWidth(1024);
    bankNameEdit->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    bankNameEdit->setValidator(new QRegularExpressionValidator(Utils::get()->getNameRegexValidator(), this));

    buttonLock = buildCheckableButton("lock/unlock presets order", ":/images/stock_lock_open_16.png", ":/images/stock_lock_16.png");
    buttonLock->setChecked(true);

    buttonRemove = buildActionButton("remove selected preset", ":/images/button_remove.png");
    buttonClone = buildActionButton("duplicate selected preset", ":/images/button_clone.png");
    buttonAdd = buildActionButton("add new preset", ":/images/button_add.png");

    sizeSlider = new QSlider(this);
    sizeSlider->setMaximum(31);
    sizeSlider->setMinimum(8);
    sizeSlider->setOrientation(Qt::Horizontal);

    toolsLayout = new QHBoxLayout();
    toolsLayout->addWidget(buttonLock, 5, Qt::AlignLeft);
    toolsLayout->addWidget(sizeSlider, 20, Qt::AlignHCenter);
    toolsLayout->addStretch(16);
    toolsLayout->addWidget(buttonRemove, 0, Qt::AlignRight);
    toolsLayout->addWidget(buttonClone, 0, Qt::AlignRight);
    toolsLayout->addWidget(buttonAdd, 0, Qt::AlignRight);

    dockLayout->addWidget(bankNameEdit);
    dockLayout->addLayout(toolsLayout);
    dockLayout->addWidget(presetList);
    // toolsLayout->addSpacing(12);

    this->setLayout(dockLayout);

    this->setTabOrder(presetList, bankNameEdit);
    this->setTabOrder(bankNameEdit, buttonLock);
    this->setTabOrder(buttonLock, buttonRemove);
    this->setTabOrder(buttonRemove, buttonClone);
    this->setTabOrder(buttonClone, buttonAdd);

    connect (buttonLock, SIGNAL(toggled(bool)), this, SLOT( slotPreservePresetOrder(bool)) );
    connect (buttonRemove, SIGNAL(released()), this, SLOT( slotDeletePreset() ) );
    connect (buttonClone, SIGNAL(released()), this, SLOT( slotDuplicatePreset() ) );
    connect (buttonAdd, SIGNAL(released()), this, SLOT( slotAddPreset() ) );
    connect (bankNameEdit, SIGNAL(editingFinished()), this, SLOT( slotBankNameChanged() ) );

    // SIGNAL OF SELECTED PRESET
    connect(presetList, SIGNAL(currentRowChanged(int)), this, SLOT(slotChangePreset(int)) );
    connect(MidiManager::get(), SIGNAL(receivedProgramChange(int)), this, SLOT(slotChangePresetByProgramChange(int)) );

    connect(sizeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotListFontSize(int)) );

    this->sizeSlider->setValue(16);
}


PresetsListWidget::~PresetsListWidget() {
}


void PresetsListWidget::slotPreservePresetOrder(bool preserve)    {
    if (preserve)
        presetList->setDragDropMode(QAbstractItemView::NoDragDrop);
    else
        presetList->setDragDropMode(QAbstractItemView::InternalMove);
}


void PresetsListWidget::slotChangePresetByProgramChange(int programChangeValue)    {
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Changing preset by program change...");
    presetList->setCurrentRow(programChangeValue);
}


void PresetsListWidget::slotChangePreset(int selectedRow)    {
    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Slot change preset in row %1").arg(selectedRow));
    this->presetList->setCurrentRow(selectedRow);
    this->changeToSelectedPreset();
}


void PresetsListWidget::changeToSelectedPreset()    {

    if (currentPreset && currentPreset->isActive())  {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Unselecting preset: '%1'").arg(currentPreset->getName()));
        // this->currentPreset->save();
    }
    this->unselectPreset();

    if (presetList->currentRow()>-1)    {
        QString selected = presetList->currentItem()->text();
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Selecting preset: '%1'").arg(selected));
        this->selectPreset(selected);
    } else  {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("No preset selected");
    }
}


void PresetsListWidget::unselectPreset()    {
    if (currentPreset)  {
        currentPreset->slotUnselected();
        disconnect(currentPreset, SIGNAL(nameChanged(QString,QString)),
                   this, SLOT(slotPresetNameChanged(QString,QString)));

    } else  {
        if (ISLOGGABLE_WARN) LOG_WARN("Preset is NULL");
    }
}


void PresetsListWidget::selectPreset(QString selectedPreset)  {

    if (currentPreset!=NULL)
        delete currentPreset;
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Select preset \"" + selectedPreset+"\"" + QString(" %1").arg(this->presetList->currentIndex().row()+1));
    currentPreset = new PresetWidget(this->centralWidget);
    QString bank = this->bankName;
    currentPreset->load(bank, selectedPreset);
    currentPreset->slotSelected();
    currentPreset->setPC(this->presetList->currentIndex().row()+1);
    this->sendInfoToMetronome();
    connect(currentPreset, SIGNAL(nameChanged(QString,QString)), this, SLOT(slotPresetNameChanged(QString,QString)));
    lastPresetSelected = selectedPreset;
}


void PresetsListWidget::sendInfoToMetronome() {
    emit metronomeChanged(currentPreset->isMetronomeActive(), currentPreset->getMetronomeBpm(), currentPreset->getMetronomeVolume(), currentPreset->getName() );
}


void PresetsListWidget::addToList(QString presetName, QString toolTip)  {
    QListWidgetItem * item = new QListWidgetItem();
    item->setText(presetName);
    item->setToolTip(toolTip);
    item->setData(Qt::UserRole, QVariant::fromValue(presetList->count()+1));
    // item->setFlags (item->flags () | Qt::ItemIsEditable);
    presetList->addItem(item);
}


void PresetsListWidget::insertToList( int row, QString presetName, QString /*toolTip*/)  {
    QListWidgetItem * item = new QListWidgetItem();
    item->setText(presetName);
    item->setToolTip(QString("%1").arg(presetName));
    item->setData(Qt::UserRole, QVariant::fromValue(row));
    // item->setFlags (item->flags () | Qt::ItemIsEditable);
    presetList->insertItem(row, item);
}


void PresetsListWidget::sort()  {
    if (ISLOGGABLE_TRACE) LOG_TRACE("sort invoked");
    for (int i=0;i<presetList->count();i++) {
        for (int j=presetList->count()-1;j>i;j--) {
            if (presetList->item(j)->data(Qt::UserRole).toInt() < presetList->item(j-1)->data(Qt::UserRole).toInt())  {
                QListWidgetItem * tmpItem = presetList->takeItem(j-1);
                presetList->addItem(tmpItem);
            }
        }
    }
}


void PresetsListWidget::setPreserveOrder(bool preserveOrder)    {
    this->buttonLock->setChecked(preserveOrder);
    this->slotPreservePresetOrder(preserveOrder);
}


bool PresetsListWidget::loadBank(QString bankName) {

    if (ISLOGGABLE_INFO) LOG_INFO("loading bank \"" + bankName+"\"");

    if (bankName.isEmpty())
        return false;

    this->bankName=bankName;
    this->bankNameEdit->setText(bankName);

    QString fileName = bankName + APP_DATA_BANK_EXTENSION;
    QString completeBankFileName = Utils::get()->getDataDir() + "/" + bankName + "/" + fileName;

    // Scan available preset files
    presetsDir = new QDir( Utils::get()->getDataDir() + "/" + bankName);
    QStringList presetDirNameFilter;
    presetDirNameFilter.append( QString("*") );
    QStringList presetsDirsOnFolder = presetsDir->entryList(presetDirNameFilter, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable );

    QStringList presetsListForLoad;
    QStringList slotsAlias;
    QString presetSelected;

    // Scan bank definitions file
    QFile file( completeBankFileName );
    if ( !file.exists() )	{
        // Bank file don't exits!
        QString message = QString( "Can not find bank file '%1'. Will proced with default settings and load all presets in folder.").arg(fileName);
        QMessageBox::warning(0, APP_NAME, message);
        setPreserveOrder( false );
        // Load all presets in dir with order given by "presetsDir->entryList"
        presetsListForLoad = presetsDirsOnFolder;

    } else  {
        QSettings settings(completeBankFileName,QSettings::IniFormat,this);
        settings.beginGroup("Bank");
        setPreserveOrder( settings.value("presetsOrderLocked", true).toBool() );
        presetsListForLoad = settings.value("presetsList").toString().split(APP_OBJECT_NAME_SEPARATOR);
        presetSelected = settings.value("presetSelected").toString();
        this->infoMessage = settings.value("infoMessage").toString();
        this->sizeSlider->setValue(settings.value("fontSize", 16).toInt());
        settings.endGroup();
        settings.beginGroup("Slots");
        slotsAlias = settings.value("alias").toString().split(APP_OBJECT_NAME_SEPARATOR);
        settings.endGroup();

        /// TODO: Load from preferences if folder presets not listed in bank definitions will be load too
        // Load presets not listed in bank definitions
        for (int f=0; f<presetsDirsOnFolder.count(); f++)   {
            QString presetDirOnFolder = presetsDirsOnFolder.at(f);
            if (!presetsListForLoad.contains(presetDirOnFolder))    {
                presetsListForLoad.append(presetDirOnFolder);
                if (ISLOGGABLE_TRACE) LOG_TRACE("added founded extra preset: "+ presetDirOnFolder);
            }
        }
    }

    QStringList validPresets;
    QStringList invalidPresets;

    for (int p=0; p<presetsListForLoad.count(); p++)   {
        QString presetName = presetsListForLoad.at(p);
        QDir presetDir( Utils::get()->getDataDir() + "/" + bankName + "/" + presetName);
        if (presetDir.exists()) {
            bool presetFolderIsValid = false;
            // If preset dir have at last one element, consider preset folder valid
            QStringList elementNameFilter;
            elementNameFilter.append( QString("*%1").arg(APP_DATA_ELEMENT_EXTENSION) );
            QStringList elementsFilesOnFolder = presetDir.entryList(elementNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );
            if (!elementsFilesOnFolder.isEmpty())   {
                presetFolderIsValid = true;
            } else  {
                // If preset dir have at last one preset, consider preset folder valid
                QStringList presetNameFilter;
                presetNameFilter.append( QString("*%1").arg(APP_DATA_PRESET_EXTENSION) );
                QStringList presetFilesOnFolder = presetDir.entryList(presetNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );
                if (!presetFilesOnFolder.isEmpty())   {
                    presetFolderIsValid = true;
                }
            }
            // If preset folder is considered valid, add preset to list
            if (presetFolderIsValid)    {
                validPresets.append(presetName);
            } else  {
                if (ISLOGGABLE_ERROR) LOG_ERROR("preset folder \""+presetName+ "\" discarded. Is not valid preset folder");
            }
        } else  {
            invalidPresets.append(presetName);
        }
    }

    if (!invalidPresets.isEmpty())  {
        // Preset dir listed on bank definitions don't exists
        QString message = QString( "Can not find preset%1:<br><br>&nbsp;&nbsp;&nbsp;&nbsp;- %2<br><br>defined on '%3'")
                .arg(invalidPresets.count()>1?"s":"")
                .arg(invalidPresets.join("<br>&nbsp;&nbsp;&nbsp;&nbsp;- "))
                .arg(bankName);
        if (ISLOGGABLE_WARN) LOG_WARN(message);
        QMessageBox::critical(0, APP_NAME, message);
    }

    // LOAD PRESETS
    // Finally, fill preset widget list and load presets
    for (int p=0; p<validPresets.count(); p++)   {
        this->addToList(validPresets.at(p), validPresets.at(p));
    }

//    if (!this->infoMessage.isNull() && !this->infoMessage.isEmpty())  {
//        /// TODO: Support read 'welcome message' from URI
//        QMessageBox::information(0, APP_NAME, this->infoMessage);
//    }

    if (ISLOGGABLE_TRACE) LOG_TRACE("Selecting bank active saved preset");

    if (validPresets.contains( presetSelected) )    {
        this->setSelectedByName( presetSelected );
    }

    this->updateTooltip();
    this->presetList->setFocus();

    return true;
}


bool PresetsListWidget::newBank(QString bankName) {

    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Creating new bank \"" + bankName + "\"");

    if (bankName.isEmpty())
        return false;

    this->bankName=bankName;
    this->bankNameEdit->setText(bankName);
    this->updateTooltip();
    this->saveBankOnly();

    return true;
}


void PresetsListWidget::saveBankOnly() {

    if (this->bankName.isEmpty())
        return;

    if (ISLOGGABLE_DEBUG) LOG_DEBUG("saving bank \"" + this->bankName + "\"");

    QString fileName = this->bankName + APP_DATA_BANK_EXTENSION;
    QString bankPath = Utils::get()->getDataDir() + "/" + this->bankName;

    /// TODO: Manage error making dir
    QDir dir;
    dir.mkpath(bankPath);

    QString completeBankFileName = bankPath + "/" + fileName;

    QSettings settings(completeBankFileName,QSettings::IniFormat,this);
    settings.beginGroup("Bank");
    settings.setValue("presetsOrderLocked", this->buttonLock->isChecked());

    QStringList presetsListForSave;
    for (int i=0; i<this->presetList->count(); i++) {
        presetsListForSave.append( presetList->item(i)->text() );
    }
    settings.setValue("presetsList", presetsListForSave.join(APP_OBJECT_NAME_SEPARATOR) );
    QString selectPresetName = "";

    if (presetList->currentItem())
        selectPresetName = presetList->currentItem()->text();

    settings.setValue("presetSelected", (presetList->count()>0? selectPresetName :"") );
    settings.setValue("infoMessage", this->infoMessage );
    settings.setValue("fontSize", this->sizeSlider->value() );
    settings.endGroup();
    settings.beginGroup("Slots");
    QStringList slotsAlias;
    for (int i=0; i< MIDI_SLOTS; i++)   {
        slotsAlias.append(MidiManager::get()->getInPortNameAtSlot(i));
    }
    settings.setValue("alias", slotsAlias.join(APP_OBJECT_NAME_SEPARATOR) );
    settings.endGroup();
    if (ISLOGGABLE_TRACE) LOG_TRACE("End saving bank \""+this->bankName+"\"");
}


void PresetsListWidget::setSelectedByName(QString selectedPreset)  {
    if (ISLOGGABLE_TRACE) LOG_TRACE("Select preset by name  \""+selectedPreset+"\"");
    int selectedIndex = -1;
    this->lastPresetSelected = QString();
    for (int i=0; i<this->presetList->count(); i++) {
        if (this->presetList->item(i)->text() == selectedPreset)    {
            selectedIndex = i;
            this->lastPresetSelected = selectedPreset;
            break;
        }
    }

    this->presetList->setCurrentRow(selectedIndex);
}


void PresetsListWidget::saveActiveBank()   {
    // Save presets
    if (this->currentPreset->isActive())
        this->currentPreset->slotSave();
    // Save bank
    this->saveBankOnly();
}


void PresetsListWidget::slotPresetNameChanged(QString oldName, QString newName)    {

    for (int i=0; i<this->presetList->count(); i++) {
        if (this->presetList->item(i)->text() == oldName)    {
            this->presetList->item(i)->setText(newName);
            this->presetList->item(i)->setToolTip(newName);
            this->currentPreset->setName(newName);
            this->sendInfoToMetronome();
            return;
        }
    }
}


void PresetsListWidget::slotDeletePreset()  {

    if (this->presetList->count()>0)    {
        this->currentPreset->setActive(false);
        this->deletePreset(this->presetList->currentItem()->text(), true);
        this->unselectPreset();
        this->changeToSelectedPreset();
    }
}


void PresetsListWidget::slotCopyPresetToClipboard()  {
    clipboardPresetName = this->getCurrentPresetPath();
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Copying preset \""+clipboardPresetName+"\" to clipboard");
}


void PresetsListWidget::slotPastePresetFromClipboard()   {
    if (!clipboardPresetName.isNull() && !clipboardPresetName.isEmpty())
        this->newPresetFrom(clipboardPresetName);
    else    {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Preset clipboard is empty.");
    }
}


void PresetsListWidget::slotCopyElementToClipboard()  {
    clipboardElementName = this->getCurrentPresetPath()+"/"+currentPreset->getCurrentElementName();
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Copying element \""+clipboardElementName+"\" to clipboard");
}


void PresetsListWidget::slotPasteElementFromClipboard()   {
    if (!clipboardElementName.isNull() && !clipboardElementName.isEmpty())  {
        currentPreset->newElementFrom(clipboardElementName);
    }
    else    {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Element clipboard is empty.");
    }
}


void PresetsListWidget::deletePreset(QString presetName, bool showConfirm)  {

    QString message = QString( "Remove preset '%1'?").arg(presetName);

    if (showConfirm)    {
        QMessageBox msgBox;
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel )
            return;
    }

    currentPreset->remove();

    QListWidgetItem * item = new QListWidgetItem();
    item = this->presetList->takeItem( this->getPresetListRow(currentPreset->getName()) );
    this->presetList->removeItemWidget(item);
}


void PresetsListWidget::unloadBank()  {

    QStringList presetsOnBank = this->getPresetsNamesList();

    for (int i =0; i<presetsOnBank.count(); i++)    {
        this->unloadPreset(presetsOnBank.at(i));
    }
}


void PresetsListWidget::unloadPreset(QString presetName)  {

    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString( "Unload preset \"%1\"").arg(presetName));

    QListWidgetItem * item = new QListWidgetItem();
    item = this->presetList->takeItem( this->getPresetListRow(presetName) );
    this->presetList->removeItemWidget(item);

}


int PresetsListWidget::getPresetListRow(QString presetName)   {
    int presetListRow = -1;
    for (int i =0; i<this->presetList->count(); i++)    {
        if (this->presetList->item(i)->text() == presetName)  {
            presetListRow = i;
            break;
        }
    }
    return presetListRow;
}


void PresetsListWidget::slotDuplicatePreset()  {

    if (this->presetList->currentRow()<0)
        return;

    QString currentPresetName= this->getCurrentPresetPath();

    this->newPresetFrom(currentPresetName);
}


QString PresetsListWidget::getCurrentPresetPath()  {

    if (this->presetList->currentRow()<0)
        return "";
    return this->buildBankPath(this->bankName)+"/"+this->presetList->currentItem()->text();
}


void PresetsListWidget::newPresetFrom(QString presetCompleteName) {

    if (ISLOGGABLE_INFO) LOG_INFO("Creating new preset from \"" + presetCompleteName +"\"");
    //currentPreset->save();

    QFileInfo pathInfo( presetCompleteName );
    QString presetName = pathInfo.fileName();

    QString bankDir = this->buildBankPath(this->bankName);
    QString newPresetName = Utils::get()->getCloneName(presetName, this->getPresetsNamesList());

    QString toDir = bankDir+"/"+ newPresetName;
    if (!Utils::get()->copyDir(presetCompleteName, toDir, true ) )  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Creating preset (copyDir operation from \""+ presetCompleteName +"\" to \""+ toDir+ "\")");
        return;
    }

    if (newPresetName != presetName)    {
        QString renameFrom = bankDir+"/"+ newPresetName +"/"+presetName+APP_DATA_PRESET_EXTENSION;
        QString renameTo = bankDir+"/"+ newPresetName +"/"+newPresetName+APP_DATA_PRESET_EXTENSION;
        Utils::get()->renameFile(renameFrom,renameTo);
    }
    
    int row = this->presetList->currentRow();
    this->insertToList(row+1, newPresetName, newPresetName);
    this->presetList->setCurrentRow(row+1);
    this->changeToSelectedPreset();
    
}


void PresetsListWidget::slotAddPreset()  {

    if (ISLOGGABLE_INFO) LOG_INFO("Adding new preset");

    int presetsCount = this->presetList->count();
    int row = -1;

    if (presetsCount > 0)    {
        row = this->presetList->currentRow();
        // Force save actual preset
        //currentPreset->save();
    }

    QString newPresetName = Utils::get()->getCloneName("init preset", this->getPresetsNamesList());

    // Generate preset
    PresetWidget * newPreset = new PresetWidget(this->centralWidget);
    newPreset->setBankName(this->bankName);
    newPreset->setName(newPresetName);
    newPreset->createInitElementIfNecessary();
    newPreset->slotSave();
    delete newPreset;

    this->insertToList(row+1, newPresetName, newPresetName);
    this->presetList->setCurrentRow(row+1);
    this->changeToSelectedPreset();
}


void PresetsListWidget::slotBankNameChanged()   {

    QString oldName = this->bankName;
    QString newName = this->bankNameEdit->text();

    if (newName == oldName)
        return;

    if (ISLOGGABLE_TRACE) LOG_TRACE("Required change bank name from \""+oldName +"\" to \""+ newName+"\"");

    QString newDirName = buildBankPath(newName);
    QFile file( newDirName );
    if ( file.exists() )	{
        QString message = QString( "Bank'%1' already exists.").arg(newName);
        QMessageBox::critical(0, APP_NAME, message);
        this->changeBankNameEditProgramaticly(this->getCloneName(newName));
        return;
    }

    QString oldDirName = buildBankPath(oldName);
    bool renameDir = Utils::get()->renameDir( oldDirName, newDirName );
    if (!renameDir) {
       this->changeBankNameEditProgramaticly(oldName);
       if (ISLOGGABLE_ERROR) LOG_ERROR("Could not change dir name \""+oldDirName +"\" to \""+ newDirName+"\"");
       return;
    }

    bool renameFile = Utils::get()->renameFile( newDirName+"/"+oldName+APP_DATA_BANK_EXTENSION, newDirName+"/"+newName+APP_DATA_BANK_EXTENSION );
    if (!renameFile) {
        // Replace dir name
       Utils::get()->renameFile(newDirName,oldDirName);
       this->changeBankNameEditProgramaticly(oldName);
       if (ISLOGGABLE_ERROR) LOG_ERROR("Could not change file name \""+oldName+APP_DATA_BANK_EXTENSION +"\" to \""+ newName+APP_DATA_BANK_EXTENSION+"\"");
       return;
    }

    this->bankName = newName;
    this->updateTooltip();
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Change bank name from \""+oldName +"\" to \""+ newName+"\"");
    emit bankNameChanged(oldName, newName);
    this->presetList->setFocus();
}


void PresetsListWidget::changeBankNameEditProgramaticly(QString newName)    {
    disconnect(bankNameEdit, SIGNAL(editingFinished()), this, SLOT( slotBankNameChanged() ) );
    this->bankNameEdit->setText(newName);
    this->updateTooltip();
    connect(bankNameEdit, SIGNAL(editingFinished()), this, SLOT( slotBankNameChanged() ) );
}


QString PresetsListWidget::buildBankPath(QString bankName)  {
    QString completePathForBank = QString("%1/%2")
            .arg(Utils::get()->getDataDir())
            .arg(bankName);
    return completePathForBank;
}


QString PresetsListWidget::getCloneName(QString pretendedName)   {
    return Utils::get()->getCloneName( pretendedName, Utils::get()->getBanksOnDir( Utils::get()->getDataDir() ) );
}


void PresetsListWidget::slotMetronomeActive(bool active)   {
    currentPreset->slotMetronomeActive(active, true);
}


void PresetsListWidget::slotMetronomeBpm(int bpm)   {
    currentPreset->slotMetronomeBpm(bpm, true);
}


void PresetsListWidget::slotMetronomeVolume(int volume)   {
    currentPreset->slotMetronomeVolume(volume, true);
}


void PresetsListWidget::slotMetronomeChanged(bool active, int bpm, int volume)   {
    currentPreset->slotMetronomeSettings( active, bpm, volume, true);
}


void PresetsListWidget::slotAddElement()   {
    currentPreset->slotAddElement();
}


void PresetsListWidget::slotDuplicateElement()   {
    currentPreset->slotDuplicateElement();
}


void PresetsListWidget::slotDeleteElement()   {
    currentPreset->slotDeleteElement();
}


QPushButton * PresetsListWidget::buildActionButton(QString toolTip, QString resourceName)   {

    QPushButton * button = new QPushButton(this);
    button->setMaximumHeight(20);
    button->setMaximumWidth(20);
    QIcon icon;
    icon.addFile(resourceName, QSize(), QIcon::Normal, QIcon::On);
    button->setIcon(icon);
    button->setIconSize(QSize(16, 16));
    button->setToolTip(toolTip);
    return button;
}


QPushButton * PresetsListWidget::buildCheckableButton(QString toolTip, QString resourceName, QString resourceClickedName)   {
    QPushButton * button = new QPushButton(this);
    button->setMaximumHeight(20);
    button->setMaximumWidth(20);
    button->setCheckable(true);
    QIcon icon;
    icon.addFile(resourceName, QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(resourceClickedName, QSize(), QIcon::Normal, QIcon::On);
    button->setIcon(icon);
    button->setIconSize(QSize(16, 16));
    button->setToolTip(toolTip);
    return button;
}


QStringList PresetsListWidget::getPresetsNamesList()    {
    QStringList existingPresets;
    for (int p=0; p<this->presetList->count(); p++) {
        existingPresets.append(this->presetList->item(p)->text());
    }
    return existingPresets;
}


void PresetsListWidget::slotListFontSize(int listFontSize)  {
    this->listFont.setPixelSize(listFontSize);
    this->presetList->setFont(this->listFont);
    this->sizeSlider->setToolTip(QString("list font size (%1)").arg(listFontSize));
}


int PresetsListWidget::getPresetListFontSize() {
    return listFont.pixelSize();
}


void PresetsListWidget::updateTooltip() {
    if (!this->infoMessage.isNull() && !this->infoMessage.isEmpty())
        this->setToolTip(this->infoMessage);
    else
        this->setToolTip(Utils::get()->getDataDir() + "/" + this->bankName);
}


