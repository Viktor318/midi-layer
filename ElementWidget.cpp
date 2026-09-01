/*
    Midi Layer
    Copyright (C) 2012-2013, António José Soares Maia <maia.ajs@gmail.com>

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

#include "ElementWidget.h"

ElementWidget::ElementWidget(QWidget *parent, PresetInterface * presetInterface) :
    QWidget(parent),
    ui(new Ui::ElementUI)
{
    this->presetInterface = presetInterface;
    this->savePresetOnSave = false;

    ui->setupUi(this);

    // Setup customized fonts
    QFont fontElemName;
    Utils::get()->configDisplayFont(&fontElemName);
    this->ui->lName->setFont(fontElemName);

    QFont fontBigSpinValues;
    Utils::get()->configBigSpinValueFont(&fontBigSpinValues);
    this->ui->dialProgram->setFont(fontBigSpinValues);
    this->ui->dialTranspose->setFont(fontBigSpinValues);
    this->ui->lcdVolume->setFont(fontBigSpinValues);
    this->ui->dialLSB->setFont(fontBigSpinValues);
    this->ui->dialMSB->setFont(fontBigSpinValues);

    QFont fontSpinValues;
    Utils::get()->configSpinValueFont(&fontSpinValues);
    this->ui->sbMidiInChannel->setFont(fontSpinValues);
    this->ui->sbMidiOutChannel->setFont(fontSpinValues);
    this->ui->sbVelocityHigh->setFont(fontSpinValues);
    this->ui->sbVelocityLow->setFont(fontSpinValues);
    this->ui->sbCCHigh->setFont(fontSpinValues);
    this->ui->sbCCLow->setFont(fontSpinValues);

    this->ui->lName->setValidator(new QRegExpValidator(Utils::get()->getNameRegexValidator(), this));

    for (int m=0; m<MIDI_SLOTS; m++)    {
        this->ui->cbPortIn->addItem(QString("%1 - %2").arg(m+1).arg(MidiManager::get()->getInPortNameAtSlot(m)),m);
        this->ui->cbPortOut->addItem(QString("%1 - %2").arg(m+1).arg(MidiManager::get()->getOutPortNameAtSlot(m)),m);
    }

    for (int n=0; n < 128 ; n++ )   {
        // Control Change list fill
        QListWidgetItem *item = new QListWidgetItem;
        QString controllerStr = QString("[%1] %2").arg(n,3,10).arg( Utils::get()->getControllersList()->at(n));
        item->setData( Qt::DisplayRole, controllerStr );
        item->setData( Qt::CheckStateRole, Qt::Unchecked );
        ui->listCtrlAvaiable->addItem( item );
        ui->cbController->addItem(controllerStr);
        ui->cbControllerAsVol->addItem(controllerStr);
        ui->cbControllerAsVol2->addItem(controllerStr);
    }
    ui->cbControllerAsVol->setCurrentIndex(7);
    ui->cbControllerAsVol2->setCurrentIndex(11);

    QString hsRangeUpStyle = "QSlider::handle:horizontal {background-repeat:no-repeat; background-image: url(:/images/kb_up_handle.png); } QSlider::handle:horizontal:disabled {background-repeat:no-repeat; background-image: url(:/images/kb_up_handle_disabled.png); }";
    QString hsRangeDownStyle = "QSlider::handle:horizontal {background-repeat:no-repeat; background-image: url(:/images/kb_down_handle.png); } QSlider::handle:horizontal:disabled {background-repeat:no-repeat; background-image: url(:/images/kb_down_handle_disabled.png);}";
    ui->hsRangeUp->setStyleSheet(hsRangeUpStyle);
    ui->hsRangeDown->setStyleSheet(hsRangeDownStyle);

    this->buildAndAsignLeds();

    this->ui->listCtrlAvaiable->setAlternatingRowColors(true);

    this->active = false;
    this->setEdited(false);

    connect(MidiManager::get(), SIGNAL(slotChanged(int,int)), this, SLOT(slotRefreshSlots(int,int)));
}

/**
 * @brief ElementWidget::setNew Initialize the element for usage. Sets all heritage names and connections.
 * @param bankName bank name
 * @param presetName preset name
 * @param elementName element name
 * @return initialized element
 */
void ElementWidget::initialize(QString bankName, QString presetName, QString elementName) {
    this->bankName=bankName;
    this->presetName=presetName;
    this->elementName=elementName;
    this->ui->lName->setText(elementName);
    this->setupGuiConnections();
    this->setVisible(false);
    this->setEdited(true);
    //this->slotMarkEdited();
}

ElementWidget::~ElementWidget()
{
    if (ISLOGGABLE_TRACE) LOG_TRACE("unload element \""+ this->ui->lName->text()+"\"");

    while (ui->listCtrlAvaiable->count()>0) {
        delete ui->listCtrlAvaiable->takeItem(0);
    }

    delete ledIn;
    delete ledOut;
    delete ledNoteYes;
    delete ledNoteNo;
    delete ledAftertouchYes;
    delete ledAftertouchNo;
    delete ledPitchBendYes;
    delete ledPitchBendNo;
    delete ledCCAsVol;
    delete ledCCAsVol2;
    delete ledRangeLow;
    delete ledRangeMatch;
    delete ledRangeHigh;
    delete ledVelocityLow;
    delete ledVelocityMatch;
    delete ledVelocityHigh;
    delete ledCCLow;
    delete ledCCMatch;
    delete ledCCHigh;
    delete ledControllerYes;
    delete ledControllerNo;
    delete ui;
}

void ElementWidget::setupGuiConnections()  {

    connect(ui->buttonEnable, SIGNAL(released()), this, SLOT(slotMarkEdited()));
    connect(ui->cbResetChannelOnUnselect, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->lName, SIGNAL(editingFinished()), this, SLOT(slotNameChanged()) );
    connect(this->ui->buttonRevert, SIGNAL(released()), this, SLOT(slotRevert()));
    connect(this->ui->buttonSave, SIGNAL(released()), this, SLOT(slotSave()));

    // slots and channels
    connect(ui->cbPortIn, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotMarkEdited()));
    connect(ui->cbPortOut, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotMidiOutChanged()));
    connect(ui->sbMidiInChannel, SIGNAL(valueChanged(QString)), this, SLOT(slotMarkEdited()));
    connect(ui->sbMidiOutChannel, SIGNAL(valueChanged(QString)), this, SLOT(slotMidiOutChanged()));

    // Bank Change
    connect(ui->groupBoxBankSelect, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->dialMSB, SIGNAL(valueChanged(int)), this, SLOT(slotBankChanged()));
    connect(ui->dialLSB, SIGNAL(valueChanged(int)), this, SLOT(slotBankChanged()));

    // Program Change
    connect(ui->groupBoxProgramChange, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->dialProgram, SIGNAL(valueChanged(int)), this, SLOT(slotProgramChanged(int)));

    // Transpose
    connect(ui->dialTranspose, SIGNAL(valueChanged(int)), this, SLOT(slotMarkEdited()));
    connect(ui->buttonOctaveDown, SIGNAL(released()), this, SLOT(slotOctaveDown()));
    connect(ui->buttonOctaveUp, SIGNAL(released()), this, SLOT(slotOctaveUp()));

    // Volume
    connect(ui->groupBoxVolume, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->vsVolume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumeChanged(int)));

    // event type filter
    connect(ui->cbNotes, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbAftertouch, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbPitchBend, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));

    // controller transforation
    connect(ui->cbCCAsVolume, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbControllerAsVol, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMarkEdited()));
    connect(ui->cbCCAsVolume2, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbControllerAsVol2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMarkEdited()));

    // Keyboard Range
    connect(ui->groupBoxNoteRange, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->hsRangeDown, SIGNAL(valueChanged(int)), this, SLOT(slotRangeDownMoved(int)) );
    connect(ui->hsRangeUp, SIGNAL(valueChanged(int)), this, SLOT(slotRangeUpMoved(int)) );

    // Controllers tools
    connect(ui->groupBoxControllers, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->listCtrlAvaiable, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotControllerSelected(QListWidgetItem*)) );
    connect(ui->buttonCtrlAll, SIGNAL(released()), this, SLOT(slotSelectAllControllers()) );
    connect(ui->buttonCtrlInvert, SIGNAL(released()), this, SLOT(slotSelectInvertControllers()) );
    connect(ui->buttonCtrlNone, SIGNAL(released()), this, SLOT(slotSelectNoneControllers()) );
    connect(ui->findController, SIGNAL(textChanged(QString)), this, SLOT(slotFindController(QString)));
    connect(ui->buttonCtrlViewAll, SIGNAL(released()), this, SLOT(slotViewAllControllers()) );
    connect(ui->buttonCtrlViewSelected, SIGNAL(released()), this, SLOT(slotViewSelectedControllers()) );

    // Note Velocity
    connect(ui->groupBoxVelocityLimit, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbLimitIfHigh, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbLimitIfLow, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->hsVelocityHigh, SIGNAL(valueChanged(int)), this, SLOT(slotNoteHighMoved(int)) );
    connect(ui->hsVelocityLow, SIGNAL(valueChanged(int)), this, SLOT(slotNoteLowMoved(int)) );
    connect(ui->sbVelocityHigh, SIGNAL(valueChanged(int)), this, SLOT(slotNoteHighMoved(int)) );
    connect(ui->sbVelocityLow, SIGNAL(valueChanged(int)), this, SLOT(slotNoteLowMoved(int)) );

    // Volume Limit
    connect(ui->groupBoxCCLimit, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbCCLimitIfHigh, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->cbCCLimitIfLow, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->hsCCHigh, SIGNAL(valueChanged(int)), this, SLOT(slotCCHighMoved(int)) );
    connect(ui->hsCCLow, SIGNAL(valueChanged(int)), this, SLOT(slotCCLowMoved(int)) );
    connect(ui->sbCCHigh, SIGNAL(valueChanged(int)), this, SLOT(slotCCHighMoved(int)) );
    connect(ui->sbCCLow, SIGNAL(valueChanged(int)), this, SLOT(slotCCLowMoved(int)) );

    // SysEx
    connect(ui->groupBoxSendSystemExclusive, SIGNAL(clicked()), this, SLOT(slotMarkEdited()));
    connect(ui->systemExclusiveOnSelect, SIGNAL(textChanged(QString)), this, SLOT(slotSystemExclusiveOnSelect(QString)));
    connect(ui->systemExclusiveOnUnselect, SIGNAL(textChanged(QString)), this, SLOT(slotSystemExclusiveOnUnselect(QString)));
    connect(ui->buttonSendSEOnSelect, SIGNAL(released()), this, SLOT(slotSendSEOnSelect()));
    connect(ui->buttonSendSEOnUnselect, SIGNAL(released()), this, SLOT(slotSendSEOnUnselect()));

}

void ElementWidget::selectElement()   {
    if (!this->active)  {
        if (ui->buttonEnable->isChecked())  {

            // Send system exclusive
            if ( ui->groupBoxSendSystemExclusive->isChecked() && this->systemExclusiveOnSelect.size()>0 ) {
                MidiManager::get()->sendSysex( ui->cbPortOut->currentIndex(), &this->systemExclusiveOnSelect );
                // Set a delay before send Bank Change and Program Change
                // Prevents some equipment problems
                QTimer::singleShot(50,this,SLOT(deleyedSelectElement()));
            } else  {
                this->deleyedSelectElement();
            }
        }

        connect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT(slotProcessMidiMessage(ChannelMessage)));

        this->active = true;

        if (ISLOGGABLE_TRACE) LOG_TRACE("Turn element ON  \""+ bankName+"/"+presetName+"/"+elementName+"\"");
    }
}

void ElementWidget::deleyedSelectElement()  {

    // Send Bank Select
    if (ui->groupBoxBankSelect->isChecked()) {
        MidiManager::get()->sendBankChange(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1, ui->dialMSB->value(), ui->dialLSB->value());
        ledOut->flash();
    }

    // Send Program Change
    if (ui->groupBoxProgramChange->isChecked()) {
        MidiManager::get()->sendProgramChange(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,ui->dialProgram->value()-1);
        ledOut->flash();
    }

    // Send initial Volume
    if (ui->groupBoxVolume->isChecked())    {
        MidiManager::get()->sendController(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,CTL_VOLUME,ui->vsVolume->value());
        ledOut->flash();
    }

}

void ElementWidget::unselectElement()   {
    if (this->active)   {

        // Disconect from received channel messages
        disconnect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT(slotProcessMidiMessage(ChannelMessage)));

        if (ui->buttonEnable->isChecked())  {

            // Send all notes off and all controllers off
            if (ui->cbResetChannelOnUnselect->isChecked()) {
                MidiManager::get()->allNotesOff(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1);
                MidiManager::get()->resetAllControllers(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1);
            }

            // Send system exclusive
            if ( ui->groupBoxSendSystemExclusive->isChecked() && this->systemExclusiveOnUnselect.size()>0) {
                MidiManager::get()->sendSysex( ui->cbPortOut->currentIndex(), &this->systemExclusiveOnUnselect );
            }

        }

        this->active = false;
        if (ISLOGGABLE_TRACE) LOG_TRACE("Turn element OFF \""+ presetName +"/"+ elementName + "\"");
    }
}

void ElementWidget::initialSysExValidation()  {
    slotSystemExclusiveOnSelect(ui->systemExclusiveOnSelect->text());
    slotSystemExclusiveOnUnselect(ui->systemExclusiveOnUnselect->text());
}

QString ElementWidget::renderSystemExclusiveForSave( QString systemExclusiveOnGUI ) {
    QString renderedStr = "";
    QStringList list = systemExclusiveOnGUI.split(" ", QString::SkipEmptyParts);
    bool first = true;
    for (int i=0; i<list.size(); i++)    {
        if (first)
            first = false;
        else
            renderedStr.append(" ");
        renderedStr.append(list.at(i));
    }
    return renderedStr;
}

QString ElementWidget::renderControllersForSave() {
    QString renderedStr = "";

    bool first = true;
    for (int i=0; i<ui->listCtrlAvaiable->count(); i++) {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        if ( item->checkState()==Qt::Checked )    {
            if (first)
                first = false;
            else
                renderedStr.append(",");
            renderedStr.append(QString("%1").arg(i));
        }
    }
    return renderedStr;
}

void ElementWidget::setControllersFromString(QString controllersStr) {
    QStringList list = controllersStr.split(",", QString::SkipEmptyParts);
    for (int i=0; i< list.size(); i++)  {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(list.at(i).toInt());
        item->setCheckState( Qt::Checked );
        controllersSelected.insert(list.at(i).toInt());
    }
}

QString ElementWidget::getName()   {
    return ui->lName->text();
}

void ElementWidget::slotRangeDownMoved(int valueInv)   {
    int value = 127-valueInv;
    if (ui->hsRangeUp->value()<value) {
        ui->hsRangeUp->setValue(value);
    }
    ui->kbDown->setText(NoteUtil::get()->getNoteName(value));
    this->slotMarkEdited();
}

void ElementWidget::slotRangeUpMoved(int value)   {
    if ( (127-ui->hsRangeDown->value()) >value) {
        ui->hsRangeDown->setValue(127-value);
    }
    ui->kbUp->setText(NoteUtil::get()->getNoteName(value));
    this->slotMarkEdited();
}

void ElementWidget::slotControllerSelected(QListWidgetItem * item )   {
    if ( item->checkState()==Qt::Checked )    {
        controllersSelected.insert(ui->listCtrlAvaiable->row(item));
    }
    else if (item->checkState()==Qt::Unchecked) {
        controllersSelected.remove(ui->listCtrlAvaiable->row(item));
    }
    this->slotMarkEdited();
}

void ElementWidget::slotSelectAllControllers()    {
    for (int i=0; i<ui->listCtrlAvaiable->count(); i++) {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        if ( item->checkState()==Qt::Unchecked )    {
            item->setData( Qt::CheckStateRole, Qt::Checked );
        }
    }
    this->slotMarkEdited();
}

void ElementWidget::slotSelectNoneControllers()    {
    for (int i=0; i<ui->listCtrlAvaiable->count(); i++) {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        if ( item->checkState()==Qt::Checked )    {
            item->setData( Qt::CheckStateRole, Qt::Unchecked );
        }
    }
    this->slotMarkEdited();
}

void ElementWidget::slotSelectInvertControllers()    {
    for (int i=0; i<ui->listCtrlAvaiable->count(); i++) {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);

        if ( item->checkState()==Qt::Checked )    {
            item->setData( Qt::CheckStateRole, Qt::Unchecked );
        }
        else {
            item->setData( Qt::CheckStateRole, Qt::Checked );
        }
    }
    this->slotMarkEdited();
}

void ElementWidget::slotFindController(QString findText)   {

    QList<QListWidgetItem*> searchResultList = ui->listCtrlAvaiable->findItems(findText, Qt::MatchContains);

    for (int i=0; i<ui->listCtrlAvaiable->count();i++)  {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        item->setHidden(!searchResultList.contains(item));
    }
}

void ElementWidget::slotViewAllControllers() {
    for (int i=0; i<ui->listCtrlAvaiable->count();i++)  {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        item->setHidden(false);
    }
}

void ElementWidget::slotViewSelectedControllers() {
    for (int i=0; i<ui->listCtrlAvaiable->count();i++)  {
        QListWidgetItem *item = ui->listCtrlAvaiable->item(i);
        item->setHidden(item->checkState()==Qt::Unchecked);
    }
}

void ElementWidget::slotSystemExclusiveOnSelect(QString systemExclusiveText)  {
    this->systemExclusiveTextParser(systemExclusiveText, &systemExclusiveOnSelect);
    ui->buttonSendSEOnSelect->setEnabled( this->validateSysExclusive(this->systemExclusiveOnSelect) );
    slotMarkEdited();
}

void ElementWidget::slotSystemExclusiveOnUnselect(QString systemExclusiveText)    {
    this->systemExclusiveTextParser(systemExclusiveText, &systemExclusiveOnUnselect);
    ui->buttonSendSEOnUnselect->setEnabled( this->validateSysExclusive(this->systemExclusiveOnUnselect) );
    slotMarkEdited();
}

void ElementWidget::slotSendSEOnSelect()    {
    if (ui->buttonEnable->isChecked() && this->systemExclusiveOnSelect.size()>0)    {
        MidiManager::get()->sendSysex( ui->cbPortOut->currentIndex(), &this->systemExclusiveOnSelect );
    }
}

void ElementWidget::slotSendSEOnUnselect()    {
    if (ui->buttonEnable->isChecked() && this->systemExclusiveOnUnselect.size()>0)
        MidiManager::get()->sendSysex( ui->cbPortOut->currentIndex(), &this->systemExclusiveOnUnselect );
}

void ElementWidget::slotRevert()  {

    if (!this->isEdited())  {
        return;
    }

    this->load(this->bankName, this->presetName, this->elementName);
}

void ElementWidget::slotSave()  {

    if (!this->isEdited())  {
        return;
    }

    this->renameIfNedded();

    QString elementName = ui->lName->text();

    if (ui == 0 || ui->lName->text().isEmpty())    {
        /// TODO: User message
        if (ISLOGGABLE_WARN) LOG_WARN("ERROR element name is empty. Is not suposed. Can not save element. Data will be lost");
        return;
    }

    if (ISLOGGABLE_INFO) LOG_INFO ( "saving element \"" + bankName + "/" + presetName+"/"+ elementName +"\"");

    QString completeFileName = Utils::get()->getDataDir() + "/" + bankName + "/" + presetName + "/" + elementName + APP_DATA_ELEMENT_EXTENSION ;
    QFile file( completeFileName );
    if ( file.exists() )	{
        // File exits!
        QString message = QString( "Element file '%1' will be overwrited.").arg(bankName + "/" + presetName + "/" + elementName + APP_DATA_ELEMENT_EXTENSION );
        //        QMessageBox::critical(0, APP_NAME, message + " TODO: Implement confirmation");
        /// TODO: Implement confirmation
    }

    QSettings settings(completeFileName,QSettings::IniFormat,this);
    settings.beginGroup("Element");
    settings.setValue( "Enabled", ui->buttonEnable->isChecked() );
    settings.setValue( "resetChannelOnUnselect", ui->cbResetChannelOnUnselect->isChecked() );
    settings.endGroup();

    settings.beginGroup("Route");
    settings.setValue("InPortSlot", ui->cbPortIn->currentIndex()+1 );
    settings.setValue("InChannel", ui->sbMidiInChannel->value());
    settings.setValue("OutPortSlot", ui->cbPortOut ->currentIndex()+1 );
    settings.setValue("OutChannel", ui->sbMidiOutChannel->value());
    settings.endGroup();

    settings.beginGroup("BankSelect");
    settings.setValue("Enabled", ui->groupBoxBankSelect->isChecked() );
    settings.setValue("MSB", ui->dialMSB->value());
    settings.setValue("LSB", ui->dialLSB->value());
    settings.endGroup();

    settings.beginGroup("Program");
    settings.setValue("Enabled", ui->groupBoxProgramChange->isChecked() );
    settings.setValue("ProgramChange", ui->dialProgram->value());
    settings.endGroup();

    settings.beginGroup("Volume");
    settings.setValue("Enabled", ui->groupBoxVolume->isChecked() );
    settings.setValue("Volume", ui->vsVolume->value());
    settings.endGroup();

    settings.beginGroup("NoteRange");
    settings.setValue("Enabled", ui->groupBoxNoteRange->isChecked() );
    settings.setValue("Low", 127-ui->hsRangeDown->value() );
    settings.setValue("High", ui->hsRangeUp->value() );
    settings.endGroup();

    settings.beginGroup("EventFilter");
    settings.setValue("Enabled", ui->groupBoxEventFilter->isChecked() );
    settings.setValue("Notes", ui->cbNotes->isChecked() );
    settings.setValue("Aftertouch", ui->cbAftertouch->isChecked() );
    settings.setValue("PitchBend", ui->cbPitchBend->isChecked() );
    settings.endGroup();

    settings.beginGroup("EventFilter");
    settings.setValue("Enabled", ui->groupBoxEventFilter->isChecked() );
    settings.setValue("Aftertouch", ui->cbAftertouch->isChecked() );
    settings.setValue("PitchBend", ui->cbPitchBend->isChecked() );
    settings.endGroup();

    settings.beginGroup("Transpose");
    settings.setValue("Enabled", ui->groupBoxTranspose->isChecked() );
    settings.setValue("Transpose", ui->dialTranspose->value() );
    settings.endGroup();

    settings.beginGroup("VelocityLimit");
    settings.setValue("Enabled", ui->groupBoxVelocityLimit->isChecked() );
    settings.setValue("Low", ui->hsVelocityLow->value() );
    settings.setValue("LowLimiter", ui->cbLimitIfLow->isChecked() );
    settings.setValue("High", ui->hsVelocityHigh->value() );
    settings.setValue("HighLimiter", ui->cbLimitIfHigh->isChecked() );
    settings.endGroup();

    settings.beginGroup("ControllersFilter");
    settings.setValue("Enabled", ui->groupBoxControllers->isChecked() );
    settings.setValue("SelectedCotrollers", renderControllersForSave() );
    settings.setValue("ControllerAsVol1", ui->cbCCAsVolume->isChecked() );
    settings.setValue("ControllerAsVol1CC", ui->cbControllerAsVol->currentIndex() );
    settings.setValue("ControllerAsVol2", ui->cbCCAsVolume2->isChecked() );
    settings.setValue("ControllerAsVol2CC", ui->cbControllerAsVol2->currentIndex() );
    settings.endGroup();

    settings.beginGroup("ControlChangeLimit");
    settings.setValue("ControlChange", ui->cbController->currentIndex() );
    settings.setValue("Enabled", ui->groupBoxCCLimit->isChecked() );
    settings.setValue("Low", ui->hsCCLow->value() );
    settings.setValue("LowLimiter", ui->cbCCLimitIfLow->isChecked() );
    settings.setValue("High", ui->hsCCHigh->value() );
    settings.setValue("HighLimiter", ui->cbCCLimitIfHigh->isChecked() );
    settings.endGroup();

    settings.beginGroup("SendSystemExclusive");
    settings.setValue("Enabled", ui->groupBoxSendSystemExclusive->isChecked() );
    settings.setValue("OnSelect", renderSystemExclusiveForSave(ui->systemExclusiveOnSelect->text() ) );
    settings.setValue("OnUnselect", renderSystemExclusiveForSave(ui->systemExclusiveOnUnselect->text() ) );
    settings.endGroup();

    if (ISLOGGABLE_TRACE) LOG_TRACE("End saving element " + bankName + "/" + presetName + "/" + elementName + APP_DATA_ELEMENT_EXTENSION);

    if (this->savePresetOnSave) {
        this->presetInterface->updateSelectedTabName(elementName);
        this->presetInterface->savePreset();
        this->savePresetOnSave = false;
    }

    this->setEdited(false);
}

void ElementWidget::slotNameChanged() {

    QString oldName = this->elementName;
    QString newName = ui->lName->text();

    this->ui->buttonEnable->setFocus();

    if (newName == oldName) {
        return;
    }

    this->slotMarkEdited();
    this->savePresetOnSave = true;

    return;
}

void ElementWidget::slotNoteHighMoved(int value) {
    ui->sbVelocityHigh->setValue(value);
    ui->hsVelocityHigh->setValue(value);
    if (value < ui->sbVelocityLow->value())    {
        ui->sbVelocityLow->setValue(value);
        ui->hsVelocityLow->setValue(value);
    }
    this->slotMarkEdited();
}

void ElementWidget::slotNoteLowMoved(int value) {
    ui->sbVelocityLow->setValue(value);
    ui->hsVelocityLow->setValue(value);
    if (value > ui->sbVelocityHigh->value())    {
        ui->sbVelocityHigh->setValue(value);
        ui->hsVelocityHigh->setValue(value);
    }
    this->slotMarkEdited();
}

void ElementWidget::slotCCHighMoved(int value) {
    ui->sbCCHigh->setValue(value);
    ui->hsCCHigh->setValue(value);
    if (value < ui->sbCCLow->value())    {
        ui->sbCCLow->setValue(value);
        ui->hsCCLow->setValue(value);
    }
    this->slotMarkEdited();
}

void ElementWidget::slotCCLowMoved(int value) {
    ui->sbCCLow->setValue(value);
    ui->hsCCLow->setValue(value);
    if (value > ui->sbCCHigh->value())    {
        ui->sbCCHigh->setValue(value);
        ui->hsCCHigh->setValue(value);
    }
    this->slotMarkEdited();
}

void ElementWidget::slotProcessMidiMessage(ChannelMessage channelMessage)  {

    // Element Enabled, Slot Index and MIDI channel
    if ( ui->buttonEnable->isChecked() &&  channelMessage.getSlotNumber() == ui->cbPortIn->currentIndex() && channelMessage.getChannel() == ui->sbMidiInChannel->value()-1 )    {
        ledIn->flash();
        // Filter message type
        switch (channelMessage.getStatus())   {
        case STATUS_NOTEON:
            processNoteOn(channelMessage);
            break;
        case STATUS_NOTEOFF:
            processNoteOff(channelMessage);
            break;
        case STATUS_POLYAFT:
            processAftertouch(channelMessage);
            break;
        case STATUS_CHANAFT:
            processChannelAftertouch(channelMessage);
            break;
        case STATUS_CTLCHG:
            processControlChange(channelMessage);
            break;
        case STATUS_BENDER:
            processPitchBend(channelMessage);
            break;
            // Is catched by preset change
        case STATUS_PRGCHG:
            /// TODO implement option bypass program change
            //            // Filter Program change
            //            if (ui->cbProgramChange->isChecked())    {
            //                MidiManager::get()->sendProgramChange( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,channelMessage.getByte1());
            //                outLed->flash();
            //            }
            break;
            //        case CTL_ALL_SOUND_OFF:
            //        case CTL_ALL_NOTES_OFF:
            //        case CTL_RESET_ALL_CTL:
        default:
            channelMessage.setSlotNumber(ui->cbPortOut->currentIndex());
            channelMessage.setChannel(ui->sbMidiOutChannel->value()-1);
            MidiManager::get()->sendChannelMessage(&channelMessage);
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("%1 sendChannelMessage (%2)...").arg( ui->lName->text()).arg(channelMessage.getStatus()));
            ledOut->flash();
        }
    }
}

void ElementWidget::setPresetName(QString presetName)   {
    this->presetName = presetName;
}

void ElementWidget::slotVolumeChanged(int value)    {
    if (ui->buttonEnable->isChecked())  {
        MidiManager::get()->sendController(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,CTL_VOLUME,value);
        ledOut->flash();
    }
    this->slotMarkEdited();
}

void ElementWidget::slotBankChanged()    {
    if (ui->buttonEnable->isChecked())  {
        MidiManager::get()->sendBankChange(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1, ui->dialMSB->value(), ui->dialLSB->value());
        ledOut->flash();
    }
    this->slotMarkEdited();
}

void ElementWidget::slotProgramChanged(int value)    {
    if (ui->buttonEnable->isChecked())  {
        MidiManager::get()->sendProgramChange(ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,value-1);
        ledOut->flash();
    }
    this->slotMarkEdited();
}

void ElementWidget::slotRefreshSlots(int inOrOut, int slotIndex)   {

    if (inOrOut==0) {
        this->ui->cbPortIn->setItemText(slotIndex, QString("%1 - %2").arg(slotIndex+1).arg(MidiManager::get()->getInPortNameAtSlot(slotIndex)));
    } else  {
        this->ui->cbPortOut->setItemText(slotIndex, QString("%1 - %2").arg(slotIndex+1).arg(MidiManager::get()->getOutPortNameAtSlot(slotIndex)));
    }
}

void ElementWidget::slotOctaveDown()    {
    this->ui->dialTranspose->setValue( this->ui->dialTranspose->value()-12 );
    this->slotMarkEdited();
}

void ElementWidget::slotOctaveUp()    {
    this->ui->dialTranspose->setValue( this->ui->dialTranspose->value()+12 );
    this->slotMarkEdited();
}

void ElementWidget::slotMarkEdited()    {
    this->setEdited(true);
}

void ElementWidget::slotMidiOutChanged() {
    this->slotMarkEdited();
}

void ElementWidget::remove()   {

    // next IF is only necessary for prevents error in human presets edition
    if (!this->elementName.isNull() || !this->elementName.isEmpty())    {
        if (ISLOGGABLE_INFO) LOG_INFO("Removing element \"" + this->elementName +"\"");
        QString trashPath = Utils::get()->getPreferencesDir() + "/" + APP_TRASH_DIR;

        /// TODO: Manage error making dir
        QDir dir;
        dir.mkpath(QString("%1/%2").arg(APP_TRASH_DIR).arg(this->presetName));

        /// TODO: Manage move fail  (bool result = )
        QFile::copy( buildElementPath(this->elementName), trashPath+"/"+this->presetName+"/"+this->elementName+APP_DATA_ELEMENT_EXTENSION );
        QFile::remove( buildElementPath(this->elementName) );
    }
}

void ElementWidget::focusToName()   {
    this->ui->lName->setFocus();
    this->ui->lName->setCursorPosition(ui->lName->text().length());
}

QString ElementWidget::getCloneName(QString pretendedName)   {

    QDir presetDir( this->getDirPath() );
    QStringList elementNameFilter;
    elementNameFilter.append( QString("*%1").arg(APP_DATA_ELEMENT_EXTENSION) );
    QStringList elementsFilesOnFolder = presetDir.entryList(elementNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );
    elementsFilesOnFolder.replaceInStrings(QRegExp(QRegExp::escape(APP_DATA_ELEMENT_EXTENSION)+"$"),"");
    return Utils::get()->getCloneName(pretendedName, elementsFilesOnFolder);
}

QString ElementWidget::getDirPath() {
    return Utils::get()->getDataDir() + "/" + bankName + "/" + presetName;
}

void ElementWidget::processNoteOn(ChannelMessage channelMessage) {

    // Filter Note event and Note range
    if (ui->cbNotes->isChecked()) {

        // Test Old MIDI Note Off implementation
        if (channelMessage.getByte2() == 0) {
            this->processNoteOff(channelMessage);
            return;
        }

        // Learn Velocity High
        if (this->ui->buttonVelocityRangeHi->isChecked())    {
            this->ui->hsVelocityHigh->setValue(channelMessage.getByte2());
            this->ui->buttonVelocityRangeHi->setChecked(false);
        }

        // Learn Velocity Low
        if (this->ui->buttonVelocityRangeLow->isChecked())    {
            this->ui->hsVelocityLow->setValue(channelMessage.getByte2());
            this->ui->buttonVelocityRangeLow->setChecked(false);
        }

        // Learn Note High
        if (this->ui->buttonNoteHi->isChecked())    {
            this->ui->hsRangeUp->setValue(channelMessage.getByte1());
            this->ui->buttonNoteHi->setChecked(false);
        }

        // Learn Note Low
        if (this->ui->buttonNoteLow->isChecked())    {
            this->ui->hsRangeDown->setValue(127-channelMessage.getByte1());
            this->ui->buttonNoteLow->setChecked(false);
        }

        // Process Note Range
        bool validRange = true;
        if ( ui->groupBoxNoteRange->isChecked() )    {
            if (channelMessage.getByte1() < 127-ui->hsRangeDown->value()) {
                validRange = false;
                ledRangeLow->flash();
            } else if (channelMessage.getByte1() > ui->hsRangeUp->value())    {
                validRange = false;
                ledRangeHigh->flash();
            }
            if (validRange)
                ledRangeMatch->flash();
        }

        if ( validRange )   {

            bool validVelocity = true;

            // Filter Note velocity
            if (ui->groupBoxVelocityLimit->isChecked()) {

                // High velocity Limiter
                if ( channelMessage.getByte2() > ui->hsVelocityHigh->value() )  {
                    if (ui->cbLimitIfHigh->isChecked() ) {
                        channelMessage.setByte2( ui->hsVelocityHigh->value() );
                    } else  {
                        validVelocity = false;
                    }
                    ledVelocityHigh->flash();
                    // Low velocity Limiter
                } else if ( channelMessage.getByte2() < ui->hsVelocityLow->value() )  {
                    if (ui->cbLimitIfLow->isChecked() ) {
                        channelMessage.setByte2( ui->hsVelocityLow->value() );
                    } else  {
                        validVelocity = false;
                    }
                    ledVelocityLow->flash();
                }
                if ( validVelocity )    {
                    ledVelocityMatch->flash();
                }
            }

            if ( validVelocity ) {
                // Apply Element transpose
                int note = (int) channelMessage.getByte1()+ (int) ui->dialTranspose->value();
                // Note number is inbound
                if (note>-1 && note <128)   {
                    channelMessage.setByte1( channelMessage.getByte1()+ui->dialTranspose->value() );
                    // Apply Channel translation and send note
                    MidiManager::get()->sendNoteOn( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1, (unsigned char) note, channelMessage.getByte2());
                    ledOut->flash();
                    if (ISLOGGABLE_TRACE) LOG_TRACE( ui->lName->text()+" sendNoteOn..."+ channelMessage.getByte2());
                }
                else    {
                    this->ledTransposeOutOfBounds->flash();
                }
            }
        }
        ledNoteYes->flash();
    } else  {
        ledNoteNo->flash();
    }
}

void ElementWidget::processNoteOff(ChannelMessage channelMessage) {

    // Filter Note event and Note range
    if (ui->cbNotes->isChecked()) {
        // Process Note Range
        bool validRange = true;
        if ( ui->groupBoxNoteRange->isChecked() )    {
            if (channelMessage.getByte1() < 127-ui->hsRangeDown->value()) {
                validRange = false;
                ledRangeLow->flash();
            } else if (channelMessage.getByte1() > ui->hsRangeUp->value())    {
                validRange = false;
                ledRangeHigh->flash();
            }
            if ( validRange )
                ledRangeMatch->flash();
        }

        if ( validRange )   {

            // Apply Element transpose
            channelMessage.setByte1( channelMessage.getByte1()+ui->dialTranspose->value() );
            // Apply Channel translation and send note
            MidiManager::get()->sendNoteOff( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1, channelMessage.getByte1(), channelMessage.getByte2());
            ledOut->flash();
        }
        ledNoteYes->flash();
    } else  {
        ledNoteNo->flash();
    }
}

void ElementWidget::processAftertouch(ChannelMessage channelMessage) {
    // Filter Aftertouch MAPPED IN POLY
    if (ui->cbAftertouch->isChecked())    {
        MidiManager::get()->sendPolyKeyPress( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,channelMessage.getByte1(), channelMessage.getByte2());
        if (ISLOGGABLE_TRACE) LOG_TRACE( ui->lName->text()+" sendPolyKeyPress...");
        ledOut->flash();
        ledAftertouchYes->flash();
    } else  {
        ledAftertouchNo->flash();
    }
}

void ElementWidget::processChannelAftertouch(ChannelMessage channelMessage)  {
    // Filter Channel Aftertouch
    if (ui->cbAftertouch->isChecked())    {
        MidiManager::get()->sendChanKeyPress( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,channelMessage.getByte1());
        if (ISLOGGABLE_TRACE) LOG_TRACE( ui->lName->text()+" sendChanKeyPress...");
        ledOut->flash();
        ledAftertouchYes->flash();
    } else  {
        ledAftertouchNo->flash();
    }
}

void ElementWidget::processControlChange(ChannelMessage channelMessage)  {

    bool controllerAuthorized = false;

    // Learn Volume High
    if (this->ui->buttonCCRangeHi->isChecked())    {
        this->ui->hsCCHigh->setValue(channelMessage.getByte2());
        this->ui->buttonCCRangeHi->setChecked(false);
    }

    // Learn Volume Low
    if (this->ui->buttonCCRangeLow->isChecked())    {
        this->ui->hsCCLow->setValue(channelMessage.getByte2());
        this->ui->buttonCCRangeLow->setChecked(false);
    }

    // Filter Control change
    if (!ui->groupBoxControllers->isChecked() || (controllerAuthorized = controllersSelected.contains( channelMessage.getByte1())  ) ) {

//        // Volume as subVolume transformation
//        if ( channelMessage.getByte1()==CTL_VOLUME && ui->cbVolumeSubVolume->isChecked())   {
//            channelMessage.setByte2(ui->vsVolume->value()*channelMessage.getByte2()/127);
//            ledVolAsVol->flash();
//        }

//        // Expression as subVolume transformation
//        if ( channelMessage.getByte1()==CTL_EXPRESSION && ui->cbExpressionSubVolume->isChecked())   {
//            channelMessage.setByte1(CTL_VOLUME);
//            channelMessage.setByte2(ui->vsVolume->value()*channelMessage.getByte2()/127);
//            ledExprAsVol->flash();
//        }

        // Control Change as subVolume transformation
        if ( ui->cbCCAsVolume->isChecked() && channelMessage.getByte1()==ui->cbControllerAsVol->currentIndex())   {
            channelMessage.setByte1(CTL_VOLUME);
            channelMessage.setByte2(ui->vsVolume->value()*channelMessage.getByte2()/127);
            ledCCAsVol->flash();
        }

        // Control Change as subVolume2 transformation
        if ( ui->cbCCAsVolume2->isChecked() && channelMessage.getByte1()==ui->cbControllerAsVol2->currentIndex())   {
            channelMessage.setByte1(CTL_VOLUME);
            channelMessage.setByte2(ui->vsVolume->value()*channelMessage.getByte2()/127);
            ledCCAsVol2->flash();
        }

        bool validControlChange = true;
        // Filter Control Change
        if ( ui->groupBoxCCLimit->isChecked() && channelMessage.getByte1()==ui->cbController->currentIndex())  {

            // High volume Limiter
            if ( channelMessage.getByte2() > ui->hsCCHigh->value() )  {
                if (ui->cbCCLimitIfHigh->isChecked() ) {
                    channelMessage.setByte2( ui->hsCCHigh->value() );
                } else  {
                    validControlChange = false;
                }
                ledCCHigh->flash();
                // Low volume Limiter
            } else if ( channelMessage.getByte2() < ui->hsCCLow->value() )  {
                if (ui->cbCCLimitIfLow->isChecked() ) {
                    channelMessage.setByte2( ui->hsCCLow->value() );
                } else  {
                    validControlChange = false;
                }
                ledCCLow->flash();
            }
            if ( validControlChange )    {
                ledCCMatch->flash();
            }
        }
        if (validControlChange) {
            MidiManager::get()->sendController( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,channelMessage.getByte1(), channelMessage.getByte2());
            ledOut->flash();
        }
    }
    if (ui->groupBoxControllers->isChecked())   {
        if ( controllerAuthorized )  {
            ledControllerYes->flash();
        } else  {
            ledControllerNo->flash();
        }
    }
}

void ElementWidget::processPitchBend(ChannelMessage channelMessage)  {
    // Filter Pitch bend
    if (ui->cbPitchBend->isChecked())    {
        MidiManager::get()->sendBender( ui->cbPortOut->currentIndex(), ui->sbMidiOutChannel->value()-1,channelMessage.getByte1(), channelMessage.getByte2());
        if (ISLOGGABLE_TRACE) LOG_TRACE( ui->lName->text()+" sendBender...");
        ledOut->flash();
        ledPitchBendYes->flash();
    } else {
        ledPitchBendNo->flash();
    }
}

bool ElementWidget::renameIfNedded()    {
    if (this->elementName != ui->lName->text())   {
        if (!this->elementName.isEmpty())    {
            // Rename file
            Utils::get()->renameFile( this->getDirPath() +"/"+this->elementName+APP_DATA_ELEMENT_EXTENSION, this->getDirPath() +"/"+ui->lName->text()+APP_DATA_ELEMENT_EXTENSION);
        }
        // Emit signal to preset tab
        emit nameChanged(this->elementName, ui->lName->text());
        this->elementName = ui->lName->text();
        return true;
    }
    return false;
}

QString ElementWidget::buildElementPath(QString elementName)  {
    QString completeFileName = QString("%1/%2%3").arg(Utils::get()->getDataDir()).arg(bankName + "/" + presetName + "/" + elementName).arg(APP_DATA_ELEMENT_EXTENSION) ;
    return completeFileName;
}

Led * ElementWidget::buildEventLed(QString colorStr, int width, int height) {
    Led * led = new Led(this, width, height);
    led->setColor(colorStr);
    led->setColorOff(_LED_COLOR_OFF_);
    return led;
}

void ElementWidget::buildAndAsignLeds() {

    this->ledIn = new Led(this,15,15);
    this->ledIn->setColor(_LED_COLOR_IN_);
    this->ledIn->setColorOff(_LED_COLOR_OFF_);
    this->ledIn->setToolTip(tr("receiving data from selected interface and MIDI channel"));
    this->ledOut = new Led(this,15,15);
    this->ledOut->setColor(_LED_COLOR_ACCEPT_);
    this->ledOut->setColorOff(_LED_COLOR_OFF_);
    this->ledOut->setToolTip(tr("transmitting data to selected interface and MIDI channel"));

    this->ui->layoutInLed->addWidget(this->ledIn);
    this->ui->layoutOutLed->addWidget(this->ledOut);

    this->ledNoteYes = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 5);
    this->ledNoteYes->setToolTip(tr("receiving and transmitting note data"));
    this->ui->layoutLedNoteYes->addWidget(this->ledNoteYes);

    this->ledNoteNo = this->buildEventLed(_LED_COLOR_REJECT_, 10, 5);
    this->ledNoteNo->setToolTip(tr("blocking note data"));
    this->ui->layoutLedNoteNo->addWidget(this->ledNoteNo);

    this->ledAftertouchYes = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 5);
    this->ledAftertouchYes->setToolTip(tr("receiving and transmitting aftertouch data"));
    this->ui->layoutLedAftertouchYes->addWidget(this->ledAftertouchYes);

    this->ledAftertouchNo = this->buildEventLed(_LED_COLOR_REJECT_, 10, 5);
    this->ledAftertouchNo->setToolTip(tr("blocking aftertouch data"));
    this->ui->layoutLedAftertouchNo->addWidget(this->ledAftertouchNo);

    this->ledPitchBendYes = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 5);
    this->ledPitchBendYes->setToolTip(tr("receiving and transmitting pitch bend data"));
    this->ui->layoutLedPitchBendYes->addWidget(this->ledPitchBendYes);

    this->ledPitchBendNo = this->buildEventLed(_LED_COLOR_REJECT_, 10, 5);
    this->ledPitchBendNo->setToolTip(tr("blocking pitch bend data"));
    this->ui->layoutLedPitchBendNo->addWidget(this->ledPitchBendNo);

    this->ledCCAsVol = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledCCAsVol->setToolTip(tr("converting received control change to sub-volume"));
    this->ui->layoutLedCCAsVol->addWidget(this->ledCCAsVol);

    this->ledCCAsVol2 = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledCCAsVol2->setToolTip(tr("converting received control change to sub-volume"));
    this->ui->layoutLedCCAsVol2->addWidget(this->ledCCAsVol2);

    this->ledTransposeOutOfBounds = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledTransposeOutOfBounds->setToolTip(tr("notes are transposed to out of range values"));
    this->ui->layoutLedTransposeOutOfBounds->addWidget(this->ledTransposeOutOfBounds);

    this->ledRangeLow = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledRangeLow->setToolTip(tr("notes are below range"));
    this->ui->layoutLedRangeLow->addWidget(this->ledRangeLow);

    this->ledRangeMatch = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledRangeMatch->setToolTip(tr("notes are in range"));
    this->ui->layoutLedRangeMatch->addWidget(this->ledRangeMatch);

    this->ledRangeHigh = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledRangeHigh->setToolTip(tr("notes are above the range"));
    this->ui->layoutLedRangeHigh->addWidget(this->ledRangeHigh);

    this->ledVelocityLow = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledVelocityLow->setToolTip(tr("notes velocity are below range"));
    this->ui->layoutLedVelocityLow->addWidget(this->ledVelocityLow);

    this->ledVelocityMatch = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledVelocityMatch->setToolTip(tr("notes velocity are in range"));
    this->ui->layoutLedVelocityMatch->addWidget(this->ledVelocityMatch);

    this->ledVelocityHigh = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledVelocityHigh->setToolTip(tr("notes velocity are above the range"));
    this->ui->layoutLedVelocityHigh->addWidget(this->ledVelocityHigh);

    this->ledCCLow = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledCCLow->setToolTip(tr("control change are below range"));
    this->ui->layoutLedCCLow->addWidget(this->ledCCLow);

    this->ledCCMatch = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledCCMatch->setToolTip(tr("control change are in range"));
    this->ui->layoutLedCCMatch->addWidget(this->ledCCMatch);

    this->ledCCHigh = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledCCHigh->setToolTip(tr("control change are above the range"));
    this->ui->layoutLedCCHigh->addWidget(this->ledCCHigh);

    this->ledControllerYes = this->buildEventLed(_LED_COLOR_ACCEPT_, 10, 10);
    this->ledControllerYes->setToolTip(tr("receiving and transmitting selected control changed"));
    this->ui->layoutLedControllerYes->addWidget(this->ledControllerYes);

    this->ledControllerNo = this->buildEventLed(_LED_COLOR_REJECT_, 10, 10);
    this->ledControllerNo->setToolTip(tr("blocking not selected control changed"));
    this->ui->layoutLedControllerNo->addWidget(this->ledControllerNo);

}

bool ElementWidget::validateSysExclusive(std::vector<unsigned char> systemExclusiveData) {
    bool isValid=false;
    if (systemExclusiveData.size()>2)   {
        if (systemExclusiveData.at(0)==0xF0 && systemExclusiveData.at(systemExclusiveData.size()-1)==0xF7)
            isValid = true;
    }
    return isValid;
}

bool ElementWidget::loadFrom(QString fromBankName, QString fromPresetName, QString fromElementName, QString bankName, QString presetName, QString elementName) {

    if (this->load( fromBankName, fromPresetName, fromElementName)) {
        this->ui->lName->setText(elementName);
        this->bankName=bankName;
        this->presetName=presetName;
        this->elementName=elementName;
        this->slotSave();
        return true;
    }
    return false;
}

bool ElementWidget::load(QString bankName, QString presetName, QString elementName) {

    if (elementName == 0 || elementName.isEmpty())    {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Loading element fails. No name asigned for load.");
        return false;
    }

    QString completeFileName = Utils::get()->getDataDir()+"/"+bankName+"/"+presetName+"/"+elementName+APP_DATA_ELEMENT_EXTENSION;
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Loading element \""+completeFileName+"\"");

    QFile file( completeFileName );
    if ( !file.exists() )	{
        // File don't exits!
        QString message = QString( "Can not find element file '%1'.").arg(completeFileName);
        if (ISLOGGABLE_ERROR) LOG_ERROR(message);
        // QMessageBox::critical(0, APP_NAME, message);
        return false;
    }

    QSettings settings(completeFileName,QSettings::IniFormat,this);
    settings.beginGroup("Element");
    ui->buttonEnable->setChecked( settings.value("Enabled", false).toBool() );
    ui->cbResetChannelOnUnselect->setChecked( settings.value("resetChannelOnUnselect", false).toBool() );
    settings.endGroup();

    settings.beginGroup("Route");
    int port = settings.value("InPortSlot", "1").toInt();
    if (port>0 && port<=MIDI_SLOTS)
        ui->cbPortIn->setCurrentIndex( port-1 );
    else
        ui->cbPortIn->setCurrentIndex( 0 );
    ui->sbMidiInChannel->setValue( settings.value("InChannel", 1).toInt() );
    port = settings.value("OutPortSlot", "1").toInt();
    if (port>0 && port<=MIDI_SLOTS)
        ui->cbPortOut->setCurrentIndex( port-1 );
    else
        ui->cbPortOut->setCurrentIndex( 0 );

    ui->sbMidiOutChannel->setValue( settings.value("OutChannel", 1).toInt() );
    settings.endGroup();

    settings.beginGroup("BankSelect");
    ui->groupBoxBankSelect->setChecked( settings.value("Enabled", false).toBool() );
    ui->dialMSB->setValue( settings.value("MSB", 0).toInt() );
    ui->dialLSB->setValue( settings.value("LSB", 0).toInt() );
    settings.endGroup();

    settings.beginGroup("Program");
    ui->groupBoxProgramChange->setChecked( settings.value("Enabled", false).toBool() );
    ui->dialProgram->setValue( settings.value("ProgramChange", 1).toInt() );
    settings.endGroup();

    settings.beginGroup("Volume");
    ui->groupBoxVolume->setChecked( settings.value("Enabled", false).toBool() );
    ui->vsVolume->setValue( settings.value("Volume", 96).toInt() );
    settings.endGroup();

    settings.beginGroup("NoteRange");
    ui->groupBoxNoteRange->setChecked( settings.value("Enabled", false).toBool() );
    ui->hsRangeDown->setValue( 127-settings.value("Low", 0).toInt() );
    slotRangeDownMoved( 127-settings.value("Low", 0).toInt() );
    ui->hsRangeUp->setValue( settings.value("High", 127).toInt() );
    slotRangeUpMoved( settings.value("High", 127).toInt() );
    settings.endGroup();

    settings.beginGroup("EventFilter");
    ui->groupBoxEventFilter->setChecked( settings.value("Enabled", false).toBool() );
    ui->cbNotes->setChecked( settings.value("Notes", false).toBool() );
    ui->cbAftertouch->setChecked( settings.value("Aftertouch", false).toBool() );
    ui->cbPitchBend->setChecked( settings.value("PitchBend", false).toBool() );
    settings.endGroup();

    settings.beginGroup("Transpose");
    ui->groupBoxTranspose->setChecked( settings.value("Enabled", false).toBool() );
    ui->dialTranspose->setValue( settings.value("Transpose", 0).toInt() );
    settings.endGroup();

    settings.beginGroup("VelocityLimit");
    ui->groupBoxVelocityLimit->setChecked( settings.value("Enabled", false).toBool() );
    slotNoteLowMoved(settings.value("Low", 1).toInt());
    ui->cbLimitIfLow->setChecked( settings.value("LowLimiter", false).toBool() );
    slotNoteHighMoved( settings.value("High", 127).toInt() );
    ui->cbLimitIfHigh->setChecked( settings.value("HighLimiter", false).toBool() );
    settings.endGroup();

    settings.beginGroup("ControlChangeLimit");
    ui->cbController->setCurrentIndex( settings.value("ControlChange", 0).toInt() );
    ui->groupBoxCCLimit->setChecked( settings.value("Enabled", false).toBool() );
    slotCCLowMoved(settings.value("Low", 0).toInt());
    ui->cbCCLimitIfLow->setChecked( settings.value("LowLimiter", false).toBool() );
    slotCCHighMoved( settings.value("High", 127).toInt() );
    ui->cbCCLimitIfHigh->setChecked( settings.value("HighLimiter", false).toBool() );
    settings.endGroup();

    settings.beginGroup("SendSystemExclusive");
    ui->groupBoxSendSystemExclusive->setChecked( settings.value("Enabled", false).toBool() );
    ui->systemExclusiveOnSelect->setText( settings.value("OnSelect", "").toString() );
    ui->systemExclusiveOnSelect->setCursorPosition(0);
    ui->systemExclusiveOnUnselect->setText( settings.value("OnUnselect", "").toString() );
    ui->systemExclusiveOnUnselect->setCursorPosition(0);
    settings.endGroup();

    settings.beginGroup("ControllersFilter");
    ui->groupBoxControllers->setChecked( settings.value("Enabled", false).toBool() );
    setControllersFromString(  settings.value("SelectedCotrollers", "").toString() );
    ui->cbControllerAsVol->setCurrentIndex( settings.value("ControllerAsVol1CC", 7).toInt() );
    ui->cbControllerAsVol2->setCurrentIndex( settings.value("ControllerAsVol2CC", 11).toInt() );
    ui->cbCCAsVolume ->setChecked( settings.value("VolumeAsSubVolume", false).toBool() );    // Retrocompatibility
    ui->cbCCAsVolume2->setChecked( settings.value("ExpressionAsSubVolume", false).toBool() );   // Retrocompatibility
    ui->cbCCAsVolume ->setChecked( settings.value("ControllerAsVol1", false).toBool() || ui->cbCCAsVolume ->isChecked() );
    ui->cbCCAsVolume2->setChecked( settings.value("ControllerAsVol2", false).toBool() || ui->cbCCAsVolume2->isChecked() );
    settings.endGroup();

    if (controllersSelected.count()>0)  {
        slotViewSelectedControllers();
    }

    if (ISLOGGABLE_TRACE) LOG_TRACE("End loading element " +elementName + APP_DATA_ELEMENT_EXTENSION);

    this->setupGuiConnections();
    this->initialSysExValidation();

    this->ui->lName->setText(elementName);
    this->elementName = elementName;
    this->presetName = presetName;
    this->bankName = bankName;

    this->setEdited(false);

    return true;
}

void ElementWidget::setEdited(bool edited)  {

    this->edited = edited;
    if (edited) {
        this->ui->buttonRevert->setStyleSheet("background-color:#DD4444; color: black; border-radius:4px;");
        this->ui->buttonSave->setStyleSheet("background-color:#44DD44;  color: black; border-radius:4px;");
        this->presetInterface->updateTabIcon(this->getName());
    } else  {
        this->ui->buttonRevert->setStyleSheet("background-color: transparent; border-radius:4px;");
        this->ui->buttonSave->setStyleSheet("background-color: transparent; border-radius:4px;");
        this->presetInterface->updateTabIcon(this->getName());
    }
    this->ui->buttonRevert->setEnabled(edited);
    this->ui->buttonSave->setEnabled(edited);
    this->ui->buttonRevert->setVisible(edited);
    this->ui->buttonSave->setVisible(edited);
}

bool ElementWidget::isEdited()  {
    return this->edited;
}

bool ElementWidget::isActive()  {
    return this->ui->buttonEnable->isChecked();
}

void ElementWidget::slotSetSelected()   {
    this->presetInterface->setSelectedElement(this->elementName);
}

void ElementWidget::systemExclusiveTextParser(QString systemExclusiveText, std::vector<unsigned char> * buffer)  {

    buffer->clear();
    QStringList textPiece = systemExclusiveText.split(' ',QString::SkipEmptyParts);
    for (int i=0; i<textPiece.count();i++)  {
        if (textPiece.at(i).length()==2 )  {
            buffer->push_back( this->hexText2uchar( textPiece.at(i)) );
        }
    }
}

unsigned char ElementWidget::hexText2uchar(QString hexText) {
    unsigned char theByte = 0x00;
    if (hexText.size()==2)  {
        theByte += hexChar2uchar(hexText.at(1));
        theByte += hexChar2uchar(hexText.at(0)) * 16;
    }
    return theByte;
}

unsigned char ElementWidget::hexChar2uchar(QChar hexChar)   {
    unsigned char theChar = 0x00;
    if (hexChar.isDigit())
        theChar += (hexChar.toLatin1()-48);
    else
        theChar += (hexChar.toLatin1()-55);
    return theChar;
}
