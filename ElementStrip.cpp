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

#include "ElementStrip.h"
#include "ui_ElementStrip.h"
#include "ElementWidget.h"

ElementStrip::ElementStrip(QWidget *parent, ElementWidget * element) :
    QWidget(parent),
    ui(new Ui::ElementStrip)
{
    ui->setupUi(this);
    this->element = element;

    lastVolPercent=100;

    ui->buttonRemove->setVisible(false);

    QFont fontElementStrip;
    Utils::get()->configElementStripFont(&fontElementStrip);
    ui->labelIn->setFont(fontElementStrip);
    ui->labelOut->setFont(fontElementStrip);
    ui->labelBank->setFont(fontElementStrip);
    ui->labelProgramChange->setFont(fontElementStrip);
    ui->labelTranspose->setFont(fontElementStrip);
    ui->cbReset->setFont(fontElementStrip);
    ui->cbSysex->setFont(fontElementStrip);
    ui->label->setFont(fontElementStrip);

    this->loadValuesFromElement();
    this->buidToolTips();
    this->buildAndAsignLeds();

    this->referenceTranspose = this->element->ui->dialTranspose->value();
    this->referenceVolume = this->element->ui->vsVolume->value();

    this->buildConnections();
}

ElementStrip::~ElementStrip()
{
    delete ledIn;
    delete ledOut;
    delete ui;
}

void ElementStrip::loadValuesFromElement()  {

    this->setToolTip(this->element->getName());

    ui->label->setText(this->element->getName());
    ui->sbSlotIn->setMaximum(MIDI_SLOTS);
    ui->sbSlotIn->setValue(this->element->ui->cbPortIn->currentIndex()+1);
    ui->sbMidiIn->setValue(this->element->ui->sbMidiInChannel->value());

    ui->sbSlotOut->setValue(this->element->ui->cbPortOut->currentIndex()+1);
    ui->sbMidiOut->setValue(this->element->ui->sbMidiOutChannel->value());

    ui->sbTranspose->setValue(this->element->ui->dialTranspose->value());

    ui->sbBankMsb->setValue(this->element->ui->dialMSB->value());
    ui->sbBankMsb->setEnabled(this->element->ui->dialMSB->isEnabled());
    ui->sbBankLsb->setValue(this->element->ui->dialLSB->value());
    ui->sbBankLsb->setEnabled(this->element->ui->dialLSB->isEnabled());

    ui->sbProgramChange->setValue(this->element->ui->dialProgram->value());
    ui->sbProgramChange->setEnabled(this->element->ui->dialProgram->isEnabled());

    ui->cbReset->setChecked(this->element->ui->cbResetChannelOnUnselect->isChecked());
    ui->cbSysex->setChecked(this->element->ui->groupBoxSendSystemExclusive->isChecked());

    ui->buttonEnable->setChecked(this->element->ui->buttonEnable->isChecked());
    ui->vsVolume->setValue(this->element->ui->vsVolume->value());
    ui->vsVolume->setEnabled(this->element->ui->groupBoxVolume->isChecked());
}

void ElementStrip::buidToolTips()   {
    // ToolTips
    ui->sbSlotIn->setToolTip(QString("Slot IN - %1").arg(this->element->getName()));
    ui->sbSlotOut->setToolTip(QString("Slot OUT - %1").arg(this->element->getName()));
    ui->sbMidiIn->setToolTip(QString("Channel IN - %1").arg(this->element->getName()));
    ui->sbMidiOut->setToolTip(QString("Channel OUT - %1").arg(this->element->getName()));
    ui->sbBankMsb->setToolTip(QString("Bank MSB - %1").arg(this->element->getName()));
    ui->sbBankLsb->setToolTip(QString("Bank LSB - %1").arg(this->element->getName()));
    ui->sbProgramChange->setToolTip(QString("Program Change - %1").arg(this->element->getName()));
    ui->sbTranspose->setToolTip(QString("Transpose - %1").arg(this->element->getName()));
    ui->cbReset->setToolTip(QString("Reset channel on unselect - %1").arg(this->element->getName()));
    ui->cbSysex->setToolTip(QString("Send sysex on select and unselect - %1").arg(this->element->getName()));
    ui->vsVolume->setToolTip(QString("Volume (%1) - %2").arg(this->element->ui->vsVolume->value()).arg(this->element->getName()));
}

void ElementStrip::buildConnections() {

    // Connections
    connect(ui->sbSlotIn, SIGNAL(valueChanged(int)), this, SLOT(slotSlotIn(int)));
    connect(ui->sbSlotOut, SIGNAL(valueChanged(int)), this, SLOT(slotSlotOut(int)));

    connect(ui->sbMidiIn, SIGNAL(valueChanged(int)), this, SLOT(slotMidiIn(int)));
    connect(ui->sbMidiOut, SIGNAL(valueChanged(int)), this, SLOT(slotMidiOut(int)));

    connect(ui->sbTranspose, SIGNAL(valueChanged(int)), this, SLOT(slotTranspose(int)));

    connect(ui->sbBankMsb, SIGNAL(valueChanged(int)), this, SLOT(slotBankMsb(int)));
    connect(ui->sbBankLsb, SIGNAL(valueChanged(int)), this, SLOT(slotBankLsb(int)));

    connect(ui->sbProgramChange, SIGNAL(valueChanged(int)), this, SLOT(slotProgramChange(int)));

    connect(ui->cbReset, SIGNAL(clicked(bool)), this, SLOT(slotReset(bool)));
    connect(ui->cbSysex, SIGNAL(clicked(bool)), this, SLOT(slotSysex(bool)));

    connect(ui->buttonEnable, SIGNAL(clicked(bool)), this, SLOT(slotEnabled(bool)));
    connect(ui->vsVolume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumeChanged(int)));

    connect(ui->buttonRevert, SIGNAL(clicked()), this, SLOT(slotRevert()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(slotSave()));

    this->setEdited(this->element->isEdited());

    connect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT(slotInFlashes(ChannelMessage)) );
    connect(MidiManager::get(), SIGNAL(flashLedMonitor(int,unsigned char)), this, SLOT(slotOutFlashes(int,unsigned char)) );

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowContextMenu(QPoint)));

}

void ElementStrip::mouseReleaseEvent ( QMouseEvent * event ) {
    if (event->button() == Qt::LeftButton)   {
        this->element->slotSetSelected();
    }
}

void ElementStrip::slotShowContextMenu(const QPoint &pos) {

    QMenu contextMenu("Context menu", this);

    // set a bold font for caption item
    QAction actionFakeCaption(this->element->ui->lName->text() , this);
    QFont font = actionFakeCaption.font();
    font.setBold(true);
    actionFakeCaption.setFont(font);
    contextMenu.addAction(&actionFakeCaption);

    QAction actionBankChange("bank change", this);
    actionBankChange.setCheckable(true);
    actionBankChange.setChecked(ui->sbBankLsb->isEnabled());
    connect(&actionBankChange, SIGNAL(triggered()), this, SLOT(slotEnableBankChange()));
    contextMenu.addAction(&actionBankChange);

    QAction actionProgramChange("program change", this);
    actionProgramChange.setCheckable(true);
    actionProgramChange.setChecked(ui->sbProgramChange->isEnabled());
    connect(&actionProgramChange, SIGNAL(triggered()), this, SLOT(slotEnableProgramChange()));
    contextMenu.addAction(&actionProgramChange);

    QAction actionVolumeChange("volume change", this);
    actionVolumeChange.setCheckable(true);
    actionVolumeChange.setChecked(ui->vsVolume->isEnabled());
    connect(&actionVolumeChange, SIGNAL(triggered()), this, SLOT(slotEnableVolumeChange()));
    contextMenu.addAction(&actionVolumeChange);

    contextMenu.exec(this->mapToGlobal(pos));

}

void ElementStrip::slotSlotIn(int value)   {
    this->element->ui->cbPortIn->setCurrentIndex(value-1);
    this->slotMarkEdited();
}

void ElementStrip::slotSlotOut(int value)   {
    this->element->ui->cbPortOut->setCurrentIndex(value-1);
    this->slotMarkEdited();
    emit analizeConflicts();
}

void ElementStrip::slotMidiIn(int value)   {
    this->element->ui->sbMidiInChannel->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotMidiOut(int value)   {
    this->element->ui->sbMidiOutChannel->setValue(value);
    this->slotMarkEdited();
    emit analizeConflicts();
}

void ElementStrip::slotBankMsb(int value)   {
    this->element->ui->dialMSB->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotBankLsb(int value)   {
    this->element->ui->dialLSB->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotProgramChange(int value)   {
    this->element->ui->dialProgram->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotTranspose(int value)   {
    this->element->ui->dialTranspose->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotEnabled()   {
    this->element->ui->buttonEnable->setChecked(ui->buttonEnable->isChecked());
    this->element->slotMarkEdited();
    this->slotMarkEdited();
    emit analizeConflicts();
}

void ElementStrip::slotReset(bool enabled)   {
    if (this->element->ui->cbResetChannelOnUnselect->isChecked() != enabled)  {
        ui->cbReset->setChecked(enabled);
        this->element->ui->cbResetChannelOnUnselect->setChecked(enabled);
        this->element->slotMarkEdited();
        this->slotMarkEdited();
        emit analizeConflicts();
    }
}

void ElementStrip::slotSysex(bool enabled)   {
    if (this->element->ui->groupBoxSendSystemExclusive->isChecked() != enabled)  {
        ui->cbSysex->setChecked(enabled);
        this->element->ui->groupBoxSendSystemExclusive->setChecked(enabled);
        this->element->slotMarkEdited();
        this->slotMarkEdited();
        emit analizeConflicts();
    }
}

void ElementStrip::slotEnabled(bool enabled)   {
    if (this->element->ui->buttonEnable->isChecked() != enabled)  {
        ui->buttonEnable->setChecked(enabled);
        this->element->ui->buttonEnable->setChecked(enabled);
        this->element->slotMarkEdited();
        this->slotMarkEdited();
        emit analizeConflicts();
    }
}

void ElementStrip::slotEnabledInvert()   {
    bool inverted = !this->element->ui->buttonEnable->isChecked();
    ui->buttonEnable->setChecked(inverted);
    this->element->ui->buttonEnable->setChecked(inverted);
    this->element->slotMarkEdited();
    this->slotMarkEdited();
}

void ElementStrip::slotVolumeChanged(int value)   {
    this->element->ui->vsVolume->setValue(value);
    ui->vsVolume->setToolTip(QString("Volume (%1) - %2").arg(this->element->ui->vsVolume->value()).arg(this->element->getName()));
    this->slotMarkEdited();
    this->referenceVolume = ui->vsVolume->value()*100/lastVolPercent;
}

void ElementStrip::slotVolumePercentChanged(int percent)   {

    int volume = this->referenceVolume*percent/100;
    if (volume > 127) {
        volume = 127;
    } else if (volume < 0)  {
        volume = 0;
    }
    if (volume != this->element->ui->vsVolume->value() && this->element->ui->vsVolume->isEnabled()) {
        this->element->ui->vsVolume->setValue(volume);
        disconnect(ui->vsVolume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumeChanged(int)));
        ui->vsVolume->setValue(volume);
        connect(ui->vsVolume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumeChanged(int)));
        ui->vsVolume->setToolTip(QString("Volume (%1) - %2").arg(this->element->ui->vsVolume->value()).arg(this->element->getName()));
        this->slotMarkEdited();
    }
    lastVolPercent = percent;
}

void ElementStrip::slotTranposeRelativeChanged(int relativeValue)   {
    int value = this->referenceTranspose+relativeValue;
    this->element->ui->dialTranspose->setValue(value);
    ui->sbTranspose->setValue(value);
    this->slotMarkEdited();
}

void ElementStrip::slotMarkEdited()   {
    this->setEdited(true);
}

void ElementStrip::setEdited(bool edited)  {

    if (edited) {
        ui->buttonRevert->setStyleSheet("background-color:#DD4444; color: black; border-radius:3px;");
        ui->buttonSave->setStyleSheet("background-color:#44DD44;  color: black; border-radius:3px;");
    } else  {
        ui->buttonRevert->setStyleSheet("background-color: transparent; border-radius:3px;");
        ui->buttonSave->setStyleSheet("background-color: transparent; border-radius:3px;");
    }
    ui->buttonRevert->setEnabled(edited);
    ui->buttonSave->setEnabled(edited);
    ui->buttonRevert->setVisible(edited);
    ui->buttonSave->setVisible(edited);
}

void ElementStrip::slotEnableBankChange()  {
    ui->sbBankMsb->setEnabled(!ui->sbBankMsb->isEnabled());
    ui->sbBankLsb->setEnabled(ui->sbBankMsb->isEnabled());
    this->element->ui->groupBoxBankSelect->setChecked(ui->sbBankMsb->isEnabled());
    this->element->slotMarkEdited();
    this->setEdited(true);
    emit analizeConflicts();
}

void ElementStrip::slotEnableProgramChange()  {
    ui->sbProgramChange->setEnabled(!ui->sbProgramChange->isEnabled());
    this->element->ui->groupBoxProgramChange->setChecked(ui->sbProgramChange->isEnabled());
    this->element->slotMarkEdited();
    this->setEdited(true);
    emit analizeConflicts();
}

void ElementStrip::slotEnableVolumeChange()  {
    ui->vsVolume->setEnabled(!ui->vsVolume->isEnabled());
    this->element->ui->groupBoxVolume->setChecked(ui->vsVolume->isEnabled());
    this->element->slotMarkEdited();
    this->setEdited(true);
    emit analizeConflicts();
}

void ElementStrip::slotRevert()   {
    this->element->slotRevert();
    this->loadValuesFromElement();
    this->setEdited(false);
    this->referenceVolume = this->element->ui->vsVolume->value();
    emit analizeConflicts();
}

void ElementStrip::slotRevertForMaster()   {
    if (this->element->isEdited())  {
        this->element->slotRevert();
        this->loadValuesFromElement();
        this->setEdited(false);
        this->referenceVolume = this->element->ui->vsVolume->value();
    }
}

void ElementStrip::slotSave()   {
    this->element->slotSave();
    this->setEdited(false);
    this->referenceVolume = this->element->ui->vsVolume->value();
}

void ElementStrip::buildAndAsignLeds() {

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
}

void ElementStrip::slotInFlashes(ChannelMessage channelMessage)   {
    if (channelMessage.getSlotNumber()+1 == ui->sbSlotIn->value() && channelMessage.getChannel()+1 == ui->sbMidiIn->value())    {
        ledIn->flash();
    }
}

void ElementStrip::slotOutFlashes(int slot, unsigned char channel)   {
    if (slot+1 == ui->sbSlotOut->value() && channel+1 == ui->sbMidiOut->value())    {
        ledOut->flash();
    }
}

void ElementStrip::markBankConflict(bool conflicted)   {
    if (conflicted)    {
        this->ui->sbBankMsb->setStyleSheet("color: #FF2020;");
        this->ui->sbBankLsb->setStyleSheet("color: #FF2020;");
    } else  {
        this->ui->sbBankMsb->setStyleSheet("color: auto;");
        this->ui->sbBankLsb->setStyleSheet("color: auto;");
    }
}

void ElementStrip::markProgramConflict(bool conflicted)   {
    if (conflicted)    {
        this->ui->sbProgramChange->setStyleSheet("color: #FF2020;");
    } else  {
        this->ui->sbProgramChange->setStyleSheet("color: auto;");
    }
}

void ElementStrip::markResetConflict(bool conflicted)   {
    if (conflicted)    {
        this->ui->cbReset->setStyleSheet("color: #FF2020;");
    } else  {
        this->ui->cbReset->setStyleSheet("color: auto;");
    }
}

void ElementStrip::markSysexConflict(bool conflicted)   {
    if (conflicted)    {
        this->ui->cbSysex->setStyleSheet("color: #FF2020;");
    } else  {
        this->ui->cbSysex->setStyleSheet("color: auto;");
    }
}

void ElementStrip::markVolumeConflict(bool conflicted)   {
    if (conflicted)    {
        this->ui->volumeBackground->setStyleSheet("background-color: #EE2020; border-radius: 6px;");
    } else  {
        this->ui->volumeBackground->setStyleSheet("background-color: auto;");
    }
}


