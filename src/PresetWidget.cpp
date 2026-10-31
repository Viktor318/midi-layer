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


#include "PresetWidget.h"


PresetWidget::PresetWidget(QWidget *parent) :
    QWidget(parent)
{
    this->parent = parent;
    vMainLayout = new QVBoxLayout(this);
    vMainLayout->setSpacing(8);

    QFont font;
    Utils::get()->configDisplayFont(&font);

    this->edited = false;

    nameEdit = new QLineEdit(this);
    nameEdit->setObjectName("name");
    nameEdit->setMinimumWidth(326);
    nameEdit->setMinimumHeight(29);
    nameEdit->setMaximumHeight(29);
    nameEdit->setFont(font);
    nameEdit->setStyleSheet(QString::fromUtf8("background-color: #9bb6d8; color: #000000; padding-left:4px;"));
    nameEdit->setMaxLength(48);
    nameEdit->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
    nameEdit->setValidator(new QRegularExpressionValidator(Utils::get()->getNameRegexValidator(), this));
    nameEdit->setText("");
    nameEdit->setToolTip("Preset name. Click to edit.");
    presetName = "";

    pcLabel = new QLabel(this);
    pcLabel->setStyleSheet(QString("margin: 6px 0 0 0;"));
    pcLabel->setFont(font);
    pcLabel->setText("---");
    pcLabel->setToolTip("Program Change Value Mapping (1-128)");

    buttonRevert = new QPushButton(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/revert.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonRevert->setIcon(icon2);
    buttonRevert->setIconSize(QSize(24, 28));
    buttonRevert->setChecked(false);
    buttonRevert->setToolTip("revert preset");
    buttonRevert->setFixedSize(28, 28);     // Windows fork: ugyanakkora, mint az elem soráé (Element.ui)
    buttonSave = new QPushButton(this);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonSave->setIcon(icon3);
    buttonSave->setIconSize(QSize(24, 28));
    buttonSave->setChecked(false);
    buttonSave->setToolTip("save preset");
    buttonSave->setFixedSize(28, 28);       // Windows fork: ugyanakkora, mint az elem soráé (Element.ui)
    this->setEdited(false);

    buttonOverview = new QPushButton(this);
    buttonOverview->setToolTip("Preset overview");

    QIcon buttonOverviewIcon;
    buttonOverviewIcon.addFile(QStringLiteral(":/images/mixer.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonOverview->setIcon(buttonOverviewIcon);
    buttonOverview->setIconSize(QSize(16, 23));

    buttonRemove = buildActionButton("remove selected element", ":/images/button_remove.png");
    buttonClone = buildActionButton("duplicate selected element", ":/images/button_clone.png");
    buttonAdd = buildActionButton("add new element", ":/images/button_add.png");

    QHBoxLayout * hBoxHeader = new QHBoxLayout();
    hBoxHeader->setSpacing(4);
    hBoxHeader->addSpacing(6);
    // Windows fork: a programszám, a névmező és a visszavonás / mentés gomb nyújtás (stretch)
    // nélkül, szorosan egymás után következik, ugyanazokkal a térközökkel, mint az elem sorában
    // (Element.ui: név, 10 px, visszavonás, 7 px, mentés). Az eredeti elrendezésben ezek is
    // kaptak a szabad helyből, ezért a gombok a névmezőtől távol, a rendelkezésre álló hely
    // közepén jelentek meg. A szabad helyet most egyedül az utánuk következő nyújtás veszi fel.
    hBoxHeader->addWidget(pcLabel, 0, Qt::AlignLeft);
    hBoxHeader->addSpacing(5);
    hBoxHeader->addWidget(nameEdit, 0, Qt::AlignLeft);
    hBoxHeader->addSpacing(10 - hBoxHeader->spacing());
    hBoxHeader->addWidget(buttonRevert, 0, Qt::AlignVCenter);
    hBoxHeader->addSpacing(7 - hBoxHeader->spacing());
    hBoxHeader->addWidget(buttonSave, 0, Qt::AlignVCenter);
    hBoxHeader->addStretch(24);
    // Windows fork: az eredeti szerző adományozásra felhívó gombja helyett (a cím már nem
    // aktuális) a fork visszafogott, kattintható felirata.
    forkButton = buildForkButton();
    hBoxHeader->addWidget(forkButton, 10, Qt::AlignRight);
    hBoxHeader->addSpacing(4);
    hBoxHeader->addWidget(buttonOverview, 10, Qt::AlignRight);
    hBoxHeader->addSpacing(4);
    hBoxHeader->addWidget(buttonRemove, 5, Qt::AlignRight);
    hBoxHeader->addWidget(buttonClone, 5, Qt::AlignRight);
    hBoxHeader->addWidget(buttonAdd, 5, Qt::AlignRight);

    tabElement = new ElementsTab();
    tabElement->setUsesScrollButtons(true);

    connect (tabElement, SIGNAL(tabMoved(int,int)), this, SLOT( slotSetEdited()) );

    vMainLayout->addLayout(hBoxHeader);
    vMainLayout->addWidget(tabElement);

    connect (this->nameEdit, SIGNAL(editingFinished()), this, SLOT( slotNameChanged() ) );
    // Windows fork: a mentés gomb már gépelés közben megjelenik, nem csak a mező elhagyásakor
    connect (this->nameEdit, SIGNAL(textEdited(QString)), this, SLOT( slotNameEdited(QString) ) );
    this->nameEdit->installEventFilter(this);

    connect (buttonRevert, SIGNAL(released()), this, SLOT( slotRevert() ) );
    connect (buttonSave, SIGNAL(released()), this, SLOT( slotSave() ) );
    connect (buttonOverview, SIGNAL(released()), this, SLOT( slotPresetOverview() ) );

    connect (buttonRemove, SIGNAL(released()), this, SLOT( slotDeleteElement() ) );
    connect (buttonClone, SIGNAL(released()), this, SLOT( slotDuplicateElement() ) );
    connect (buttonAdd, SIGNAL(released()), this, SLOT( slotAddElement() ) );

    this->active = false;
    this->setVisible(false);

    this->slotMetronomeBpm(120, false);
    this->slotMetronomeVolume(0, false);

}


PresetWidget::~PresetWidget()   {
    if (ISLOGGABLE_TRACE) LOG_TRACE("unloading preset \""+ this->nameEdit->text()+"\"");

    for (int i=0; i<elements.count();i++)   {
        ElementWidget * element = elements.takeFirst();
        delete element;
        // ElementWidgetPool::get()->push(element);
    }
}


void PresetWidget::setBankName(QString bankName)  {
    this->bankName = bankName;
}


void PresetWidget::setName(QString presetName)  {
    this->presetName = presetName;
    this->nameEdit->setText(this->presetName);
}


void PresetWidget::setPC(int programChangeValue)  {
    this->programChangeValue = programChangeValue;
    this->pcLabel->setText(QString("%1").arg(programChangeValue, 3, 10, QLatin1Char('0')));
}


QString PresetWidget::getName()  {
    return this->presetName;
}


bool PresetWidget::load(QString bankName, QString presetName)   {

    if (ISLOGGABLE_INFO) LOG_INFO("loading preset \"" + bankName + "/" + presetName+"\"");

    if (presetName.isEmpty())    {
        this->presetName = "";
        return false;
    }

    this->bankName = bankName;
    this->presetName = presetName;
    this->nameEdit->setText( presetName );

    QString fileName = presetName + APP_DATA_PRESET_EXTENSION;
    QString completePresetFileName = Utils::get()->getDataDir() + "/" + bankName + "/" + presetName + "/" + fileName;

    // Scan available element files
    QDir elementsDir = QDir( Utils::get()->getDataDir() + "/" + bankName + "/" + presetName);
    QStringList elementDirNameFilter;
    elementDirNameFilter.append( QString("*%1").arg(APP_DATA_ELEMENT_EXTENSION) );
    QStringList elementFilesOnFolder = elementsDir.entryList(elementDirNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );

    QStringList elementsListForLoad;
    QStringList presetFilesNotFound;
    QString selectedElement = "";

    // Scan bank definitions file
    QFile file( completePresetFileName );
    if ( !file.exists() )	{
        // Preset file don't exits!
        presetFilesNotFound << presetName;

        QString message = QString( "Can not find preset file '%1'. Will proced with default settings and load all elements available in folder.").arg(bankName + "/" + presetName+"/"+fileName);
        if (ISLOGGABLE_WARN) LOG_WARN(message);

        for (int i=0; i<elementFilesOnFolder.count(); i++)  {
            QString elementFounded = elementFilesOnFolder.at(i);
            elementFounded.replace(QRegularExpression(QRegularExpression::escape(APP_DATA_ELEMENT_EXTENSION)+"$"),"");
            if (!elementFounded.isEmpty())
                elementsListForLoad.append(elementFounded);
        }

    } else  {
        QSettings settings(completePresetFileName,QSettings::IniFormat,this);
        settings.beginGroup("Preset");

        elementsListForLoad = settings.value("elementsList").toString().split(APP_OBJECT_NAME_SEPARATOR);
        selectedElement = settings.value("selectedElement").toString();
        notes = settings.value("notes","").toString();
        settings.endGroup();

        settings.beginGroup("Metronome");
        this->metronomeActive = settings.value("active", false).toBool();
        this->metronomeBpm = settings.value("bmp",120).toInt();
        this->metronomeVolume = settings.value("volume",0).toInt();
        settings.endGroup();

        // Load elements not listed in preset definitions
        for (int f=0; f<elementFilesOnFolder.count(); f++)   {
            QString elementOnFolder = elementFilesOnFolder.at(f);
            elementOnFolder.replace(QRegularExpression(QRegularExpression::escape(APP_DATA_ELEMENT_EXTENSION)+"$"),"");
            if (!elementsListForLoad.contains(elementOnFolder))    {
                elementsListForLoad.append(elementOnFolder);
                if (ISLOGGABLE_TRACE) LOG_TRACE("added founded extra element:"+ elementOnFolder);
            }
        }
    }

    for (int p=0; p<elementsListForLoad.count(); p++)   {
        ElementWidget * element = new ElementWidget(this->parent, this);
        if (element->load(bankName, presetName, elementsListForLoad.at(p)))  {
            QIcon icon(Utils::get()->buildElementTabIconPath(element->isActive(), element->isEdited()));
            tabElement->addTab( element, icon, element->getName() );
            this->updateTabIcon(element->getName());
        }
        else    {
            delete element;
            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("ERROR loading element '%1'").arg(bankName + "/" + presetName + "/" + elementsListForLoad.at(p) + APP_DATA_ELEMENT_EXTENSION));
        }
    }

    if (!selectedElement.isEmpty())
        tabElement->setCurrentIndex( this->getElementIndex( selectedElement ) );

    // Prevents empty presets (human edited files).
    this->createInitElementIfNecessary();

    this->resizeTabWidget();

    // if ( !notes.isEmpty() )  {
    /// TODO: Support read 'welcome message' from URI
    //        presetCommon->setShortDescription(shortDescription);
    // }

    return true;
}


void PresetWidget::slotRevert()  {
    // Remove elements from tab only (memory)
    for (int i=tabElement->count()-1; i > -1 ; i--)  {
        ElementWidget * element = (ElementWidget *) tabElement->widget(i);
        tabElement->removeTab(i);
        delete element;
    }
    this->load(this->bankName, this->presetName);
    this->setEdited(false);
}


void PresetWidget::savePreset() {
    this->slotSave();
}


void PresetWidget::updateTabIcon(QString elementName)  {

    int elementIndex = getElementIndex(elementName);
    ElementWidget * element = (ElementWidget *) tabElement->widget(elementIndex);

    if (element != NULL) {
        QIcon icon(Utils::get()->buildElementTabIconPath(element->isActive(), element->isEdited()));
        tabElement->setTabIcon(elementIndex, icon);
    }
}


void PresetWidget::setSelectedElement(QString elementName) {
    int elementIndex=this->getElementIndex(elementName);
    this->tabElement->setCurrentIndex(elementIndex);
}


void PresetWidget::slotSave()   {

    if (presetName.isEmpty())   {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Preset name is not defined");
        return;
    }

    if (ISLOGGABLE_INFO) LOG_INFO( "saving preset \"" + bankName + "/" + presetName+"\"");

    QString fileName = presetName + APP_DATA_PRESET_EXTENSION;
    QString completePresetFileName = Utils::get()->getDataDir() + "/" + bankName + "/" + presetName + "/" + fileName;
    QSettings settings(completePresetFileName,QSettings::IniFormat,this);

    settings.beginGroup("Preset");
    QStringList elementsListForSave;
    for (int i=0; i<tabElement->count(); i++)    {
        ElementWidget * element = (ElementWidget *) tabElement->widget(i);
        elementsListForSave.append( element->getName() );
    }
    settings.setValue( "elementsList", elementsListForSave.join(APP_OBJECT_NAME_SEPARATOR) );
    // settings.setValue( "notes", presetCommon->getShortDescription() );
    settings.setValue( "selectedElement", this->getCurrentElementName() );
    settings.endGroup();

    settings.beginGroup("Metronome");
    settings.setValue("active", this->metronomeActive );
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Will save BPM = %1").arg(this->metronomeBpm));
    settings.setValue("bmp", this->metronomeBpm );
    settings.setValue("volume", this->metronomeVolume );
    settings.endGroup();

    this ->setEdited(false);
    if (ISLOGGABLE_TRACE) LOG_TRACE("End saving preset \"" + bankName + "/" + presetName + "\"");
}


void PresetWidget::slotSelected()    {
    for (int i=0; i<tabElement->count(); i++)    {
        ElementWidget * activeElement;
        activeElement = (ElementWidget *) tabElement->widget(i);
        activeElement->selectElement();
        connect(activeElement, SIGNAL(nameChanged(QString,QString)), this, SLOT(slotElementNameChanged(QString,QString)));
    }
    this->setVisible(true);
    this->active = true;
}


void PresetWidget::slotUnselected()    {
    for (int i=0; i<tabElement->count(); i++)    {
        ElementWidget * activeElement;
        activeElement = (ElementWidget *) tabElement->widget(i);
        disconnect(activeElement, SIGNAL(nameChanged(QString,QString)), this, SLOT(slotElementNameChanged(QString,QString)));
        activeElement->unselectElement();
    }
    this->setVisible(false);
    this->active = false;

    // All notes off
    //MidiManager::get()->allNotesOff();
    // Reset all controllers
    //MidiManager::get()->resetAllControllers();
}


void PresetWidget::setActive(bool active) {
    this->active = active;
}


bool PresetWidget::isActive() {
    return this->active;
}


QPushButton * PresetWidget::buildActionButton(QString toolTip, QString resourceName)   {
    QPushButton * button = new QPushButton(this);
    button->setMaximumHeight(20);
    button->setMaximumWidth(20);
    // button->setCursor(QCursor(Qt::PointingHandCursor));
    QIcon icon;
    icon.addFile(resourceName, QSize(), QIcon::Normal, QIcon::On);
    button->setIcon(icon);
    button->setIconSize(QSize(16, 16));
    button->setToolTip(toolTip);
    return button;
}


LinkButton * PresetWidget::buildForkButton()   {
    LinkButton * button = new LinkButton(this);
    button->setMaximumHeight(26);
    button->setCursor(QCursor(Qt::PointingHandCursor));
    button->setText(FORK_TEXT);
    button->setStyleSheet("background-color: transparent; color: #808080; border: none;");
    button->setToolTip(QString("%1 a GitHubon: %2").arg(APP_NAME).arg(FORK_URL));
    button->setUrl(FORK_URL);
    return button;
}


// Windows fork: a névmező gépelés közbeni változása. Csak megjelöli a presetet szerkesztettként
// (megjelenik a mentés gomb); maga az átnevezés továbbra is a mező elhagyásakor történik
// (slotNameChanged). Ha a mentés gombra kattintunk, a mező előbb elveszti a fókuszt, így az
// átnevezés a mentés előtt lefut.
void PresetWidget::slotNameEdited(const QString & text)   {
    // akkor is, ha a név (még) érvénytelen, pl. a végén szóköz van - lásd eventFilter
    if (text != this->presetName) {
        this->setEdited(true);
    }
}


// Windows fork: a névmezőhöz tartozó érvényesítő (getNameRegexValidator) nem enged a név
// elején és végén szóközt és pontot, mert a névből mappa- és fájlnév lesz. Ilyen
// "félkész" szövegnél a QLineEdit a mező elhagyásakor / Enterre nem küld editingFinished
// jelet, így az átnevezés eredetileg csendben elmaradt, a hibás szöveg pedig a mezőben maradt.
// Most a mező elhagyásakor és Enterre ezeket levágjuk (Utils::cleanName), a helyes nevet
// visszaírjuk a mezőbe, és elvégezzük az átnevezést. Ha a név így sem érvényes (pl. üres
// maradt, vagy tiltott karaktert tartalmaz), a mentett név áll vissza.
bool PresetWidget::eventFilter(QObject * watched, QEvent * event)  {
    if ( watched == this->nameEdit && ( event->type() == QEvent::FocusOut
        || ( event->type() == QEvent::KeyPress
             && ( static_cast<QKeyEvent *>(event)->key() == Qt::Key_Return
                  || static_cast<QKeyEvent *>(event)->key() == Qt::Key_Enter ) ) )
         && !this->nameEdit->hasAcceptableInput() )  {
        QString trimmedName = Utils::get()->cleanName(this->nameEdit->text());
        this->nameEdit->setText(trimmedName);
        if (this->nameEdit->hasAcceptableInput())  {
            // a levágott név érvényes: átnevezés (az editingFinished ilyenkor nem biztos,
            // hogy jön; a slotNameChanged ismételt hívása ártalmatlan)
            this->slotNameChanged();
        } else  {
            this->nameEdit->setText(this->presetName);
        }
    }
    return QWidget::eventFilter(watched, event);
}


void PresetWidget::slotNameChanged()    {

    QString oldName = this->presetName;
    QString newName = this->nameEdit->text();

    if (newName == oldName) {
        this->tabElement->setFocus();
        return;
    }

    QFile file( buildPresetPath(newName) );
    if ( file.exists() )	{
        disconnect (this->nameEdit, SIGNAL(editingFinished()), this, SLOT( slotNameChanged() ) );
        QString message = QString( "Preset '%1' already exists.").arg(newName);
        QMessageBox::critical(0, APP_NAME, message);
        this->nameEdit->setText(this->getCloneName(newName));
        connect (this->nameEdit, SIGNAL(editingFinished()), this, SLOT( slotNameChanged() ) );
        return;
    }

    Utils::get()->renameFile( buildPresetPath(oldName), buildPresetPath( newName) );
    Utils::get()->renameFile( buildPresetPath(newName)+"/"+oldName+APP_DATA_PRESET_EXTENSION, buildPresetPath( newName)+"/"+newName+APP_DATA_PRESET_EXTENSION );
    this->presetName = newName;

    // Change elements presetName
    for (int i=0; i< tabElement->count();i++)    {
        ((ElementWidget *) tabElement->widget(i))->setPresetName(newName);
    }

    // Emit signal to presetListWidget
    emit nameChanged(oldName, newName);
    this->tabElement->setFocus();
    this->setEdited(true);

}


QString PresetWidget::buildPresetPath(QString presetName)  {
    QString completeDirName = QString("%1/%2/%3")
            .arg(Utils::get()->getDataDir())
            .arg(this->bankName)
            .arg(presetName);
    return completeDirName;
}


QString PresetWidget::buildPresetFileName(QString presetName)  {
    QString completeFileName = QString("%1/%2%3")
            .arg(buildPresetPath(presetName))
            .arg(presetName)
            .arg(APP_DATA_PRESET_EXTENSION) ;
    return completeFileName;
}


void PresetWidget::slotDeleteElement()  {

    QString message = QString( "Remove element '%1'?").arg(tabElement->tabText(tabElement->currentIndex()) );
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel )
        return;

    ElementWidget * element = (ElementWidget *) tabElement->widget(tabElement->currentIndex());
    tabElement->removeTab(tabElement->currentIndex());
    element->remove();
    delete element;
    this->createInitElementIfNecessary();
    this->setEdited(true);
    this->savePreset();

}


void PresetWidget::slotDuplicateElement()  {

    QString currentElementName = tabElement->tabText(tabElement->currentIndex());
    ElementWidget * element = new ElementWidget(this->parent, this);
    if ( element->load(bankName, presetName, currentElementName) )  {
        element->initialize(bankName, presetName, this->getElementCloneName(currentElementName));
        this->putElementInTab(element);
        // save on add new element
        element->slotSave();
        this->updateTabIcon(element->getName());
    }
    this->savePreset();
}


void PresetWidget::slotAddElement()  {

    if (ISLOGGABLE_INFO) LOG_INFO("Adding new element");
    ElementWidget * element = new ElementWidget(this->parent, this);
    element->initialize( bankName, presetName, this->getElementCloneName("init element"));
    this->putElementInTab(element);
    element->slotSave();
    this->savePreset();
}


void PresetWidget::newElementFrom(QString elementCompleteName) {

    if (ISLOGGABLE_INFO) LOG_INFO("Creating new element from \"" + elementCompleteName +"\"");

    QFileInfo pathInfo( elementCompleteName );
    QString elementName = pathInfo.fileName();
    this->addElement( this->getElementCloneName(elementName), elementCompleteName);
}


void PresetWidget::addElement(QString elementName, QString elementFile) {

    QStringList pathParts = elementFile.split("/");
    int pathPartsSize = pathParts.count();

    QString fromBank = pathParts.at(pathPartsSize-3);
    QString fromPreset = pathParts.at(pathPartsSize-2);
    QString fromElement = pathParts.at(pathPartsSize-1);

    ElementWidget * element = new ElementWidget(this->parent, this);
    if ( element->loadFrom(fromBank, fromPreset, fromElement, bankName, presetName, elementName) )  {
        this->putElementInTab(element);
        element->slotSave();
        this->slotSave();
    }
}


void PresetWidget::putElementInTab(ElementWidget * element) {
    QIcon icon(Utils::get()->buildElementTabIconPath(element->isActive(), element->isEdited()));
    tabElement->insertTab( tabElement->currentIndex()+1, element, icon, element->getName());
    tabElement->setCurrentIndex(tabElement->currentIndex()+1);
    element->focusToName();
    element->selectElement();
}


QString PresetWidget::getCurrentElementName()    {
    if (tabElement==0 || tabElement->count()<1)
        return "";
    QString elementName = ((ElementWidget *) tabElement->widget(tabElement->currentIndex()))->getName();
    if (ISLOGGABLE_TRACE) LOG_TRACE("Current element name \""+elementName+"\"");
    return elementName;
}


void PresetWidget::updateSelectedTabName(QString tabName)  {
    tabElement->setTabText(tabElement->currentIndex(), tabName);
}


QStringList PresetWidget::getElementNames() {
    QStringList elementsNames;
    for (int i=0; i<tabElement->count(); i++)    {
        elementsNames << tabElement->tabText(i);
    }
    return elementsNames;
}


int PresetWidget::getElementIndex(QString elementName) {
    int index = 0;
    for (int i=0; i<tabElement->count(); i++)    {
        if (tabElement->tabText(i) == elementName)  {
            index = i;
            break;
        }
    }
    return index;
}


QString PresetWidget::getCloneName(QString pretendedName)   {

    QDir dir( Utils::get()->getDataDir() + "/" + bankName);
    QStringList bankDirFilter;
    bankDirFilter.append("*");
    QStringList dirsInBankFolder = dir.entryList(bankDirFilter, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable );

    QStringList existingPresets;
    QStringList presetNameFilter;
    presetNameFilter.append( QString("*%1").arg(APP_DATA_ELEMENT_EXTENSION) );
    for (int i=0; i< dirsInBankFolder.count(); i++)  {
        dir.cd(dirsInBankFolder.at(i));
        QStringList elementsFilesOnFolder = dir.entryList(presetNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );
        if (elementsFilesOnFolder.count()>0)
            existingPresets << dirsInBankFolder.at(i);
    }
    existingPresets.replaceInStrings(QRegularExpression(QRegularExpression::escape(APP_DATA_PRESET_EXTENSION)+"$"),"");
    return Utils::get()->getCloneName(pretendedName, existingPresets);
}


QString PresetWidget::getElementCloneName(QString pretendedName)   {
    return Utils::get()->getCloneName(pretendedName, this->getElementNames());
}


void PresetWidget::remove()   {

    QString presetName = this->nameEdit->text();
    if (ISLOGGABLE_INFO) LOG_INFO("Removing preset \"" + presetName +"\"");

    QString trashPath = Utils::get()->getPreferencesDir() + "/" + APP_TRASH_DIR;
    if (ISLOGGABLE_TRACE) LOG_TRACE("Moving \""+presetName+"\" to trash (\""+trashPath+"\").");

    // Remove child elements
    for (int i=tabElement->count()-1; i > -1 ; i--)  {
        ElementWidget * element = (ElementWidget *) tabElement->widget(i);
        tabElement->removeTab(i);
        element->remove();
        delete element;
    }

    // Remove preset

    QString presetDir = this->buildPresetPath(this->presetName);
    QString trashDir = trashPath+"/"+this->presetName;

    bool copySuccess = Utils::get()->copyDir(presetDir, trashDir, true);

    if (copySuccess)    {
        Utils::get()->removeDir( presetDir );
    } else  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Error moving \"" +presetName + "\" to trash dir.");
    }

}


bool PresetWidget::createInitElementIfNecessary()   {
    if (tabElement->count()<1)   {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Creating default init element...");
        slotAddElement();
        resizeTabWidget();
        return true;
    }
    return false;
}


void PresetWidget::resizeTabWidget()    {
    if (tabElement->count()>0)   {
        tabElement->setMinimumWidth(955);
        tabElement->setMinimumHeight(680);
    }
}


QString PresetWidget::getNotes()   {
    return this->notes;
}


void PresetWidget::slotMetronomeBpm(int bpm, bool markEdited) {
    // if (this != 0)  {
        this->metronomeBpm = bpm;
        if (markEdited) {
            this->setEdited(true);
        }
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Preset BMP set to %1").arg(bpm));
    // }
}


void PresetWidget::slotMetronomeActive(bool active, bool markEdited)   {
    // if (this != 0) {
        this->metronomeActive = active;
        if (markEdited) {
            this->setEdited(true);
        }
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Preset metronome active is %1").arg(active));
    // }
}


void PresetWidget::slotMetronomeVolume(int volume, bool markEdited)   {
    // if (this != 0) {
        this->metronomeVolume = volume;
        if (markEdited) {
            this->setEdited(true);
        }
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Preset metronome volume set to %1").arg(volume));
    // }
}


void PresetWidget::slotMetronomeSettings(bool active, int bpm, int volume, bool markEdited)   {
    LOG_TRACE("Set metronome settings - active");
    this->slotMetronomeActive(active, false);
    LOG_TRACE("Set metronome settings - bpm");
    this->slotMetronomeBpm(bpm, false);
    if (volume>-1)
        this->slotMetronomeVolume(volume, false);
    if (markEdited) {
        this->setEdited(true);
    }
}


int PresetWidget::getMetronomeBpm(void) {
    return this->metronomeBpm;
}


bool PresetWidget::isMetronomeActive(void)   {
    return this->metronomeActive;
}


int PresetWidget::getMetronomeVolume(void)  {
    return this->metronomeVolume;
}


void PresetWidget::slotElementNameChanged(QString oldName, QString newName) {
    for (int i=0; i<tabElement->count(); i++)    {
        if (tabElement->tabText(i) == oldName)  {
            tabElement->setTabText(i, newName);
            return;
        }
    }
}


void PresetWidget::slotSetEdited()  {
    this->setEdited(true);
}


void PresetWidget::setEdited(bool edited)  {

    this->edited = edited;
    if (edited) {
        this->buttonRevert->setStyleSheet("background-color:#DD4444; color: black; border-radius:4px;");
        this->buttonSave->setStyleSheet("background-color:#44DD44;  color: black; border-radius:4px;");
    } else  {
        this->buttonRevert->setStyleSheet("background-color: transparent; border-radius:4px;");
        this->buttonSave->setStyleSheet("background-color: transparent; border-radius:4px;");
    }
    this->buttonRevert->setEnabled(edited);
    this->buttonSave->setEnabled(edited);
    this->buttonRevert->setVisible(edited);
    this->buttonSave->setVisible(edited);
}


void PresetWidget::slotPresetOverview()  {
    PresetOverview * presetOverview = new PresetOverview(this, this->tabElement);
    presetOverview->setWindowTitle(QString("%1 - %2").arg(APP_NAME, this->getName()));
    presetOverview->move(qApp->activeWindow()->geometry().center() - presetOverview->geometry().center());
    presetOverview->exec();
}


void PresetWidget::popup(QString message)  {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

