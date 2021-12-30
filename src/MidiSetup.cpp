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


#include "MidiSetup.h"
#include "ui_MidiSetup.h"


MidiSetup::MidiSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MidiSetup)
{
    ui->setupUi(this);

    for (int m=0; m<MIDI_SLOTS; m++)    {

        QComboBox * comboIn = new QComboBox(this);
        comboIn->setMinimumHeight(24);
        midiInCombos.insert(m, comboIn);

        QComboBox * comboOut = new QComboBox(this);
        comboOut->setMinimumHeight(24);
        midiOutCombos.insert(m, comboOut);

        QHBoxLayout * slotHBox = new QHBoxLayout();
        slotHBox->setSpacing(24);

        QLabel * slotLabel = new QLabel(QString(tr("SLOT %1")).arg(m+1),this);
        slotLabel->setAlignment(Qt::AlignCenter);
        slotHBox->addWidget(slotLabel, 1);
        slotHBox->addWidget(midiInCombos.at(m),4);
        slotHBox->addWidget(midiOutCombos.at(m),4);
        ui->slotsLayout->insertLayout(m+1,slotHBox,0);
    }

    ui->programChangeSlot->setMaximum(MIDI_SLOTS);
    ui->sequencerControllerSlot->setMaximum(MIDI_SLOTS);

    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);

    connect(ui->refreshButton, SIGNAL( clicked() ), this, SLOT( slotRefresh() ) );
    connect(this, SIGNAL( accepted() ), this, SLOT( slotApplySetup() ) );

}


void MidiSetup::slotRefresh()   {
    if (ISLOGGABLE_TRACE) LOG_TRACE("Refresh MIDI interfaces");

    MidiManager::get()->refresh();
    this->refreshCombos(false);
}


void MidiSetup::refreshCombos(bool fromMidiSettings) {

    for (int i=0; i< MIDI_SLOTS; i++)   {

        QString previousInterfaceName;

        if (fromMidiSettings)
            previousInterfaceName = MidiManager::get()->getInPortNameAtSlot(i);
        else
            previousInterfaceName = midiInCombos.at(i)->currentText();
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Previous IN selection at %1:%2").arg(i).arg(previousInterfaceName));
        midiInCombos.at(i)->clear();
        midiInCombos.at(i)->addItem("");
        midiInCombos.at(i)->addItems(MidiManager::get()->getInPorts());
        this->selectInComboByText(midiInCombos.at(i), previousInterfaceName);

        if (fromMidiSettings)
            previousInterfaceName = MidiManager::get()->getOutPortNameAtSlot(i);
        else
            previousInterfaceName = midiOutCombos.at(i)->currentText();
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Previous OUT selection at %1:%2").arg(i).arg(previousInterfaceName));
        midiOutCombos.at(i)->clear();
        midiOutCombos.at(i)->addItem("");
        midiOutCombos.at(i)->addItems(MidiManager::get()->getOutPorts());
        this->selectInComboByText(midiOutCombos.at(i), previousInterfaceName);
    }

}


void MidiSetup::showEvent ( QShowEvent * /*event*/ )    {
    MidiManager::get()->refresh();
    this->refreshCombos(true);
    ui->programChangeSlot->setValue( MidiManager::get()->getProgramChangeSlot()+1 );
    ui->programChangeChannel->setValue( MidiManager::get()->getProgramChangeChannel()+1 );
    ui->sequencerControllerSlot->setValue( MidiManager::get()->getSequencerControllerSlot()+1 );
    ui->autoShowHideMonitors->setChecked(Preferences::get()->getAutoShowHideMonitors());
}


void MidiSetup::selectInComboByText(QComboBox * comboBox, QString text)  {
    for (int j=0; j<comboBox->count(); j++)    {
        if (text == comboBox->itemText(j))   {
            comboBox->setCurrentIndex(j);
            break;
        }
    }
}


void MidiSetup::slotApplySetup() {
    if (ISLOGGABLE_INFO) LOG_INFO("Applying setup...");
    for (int i=0; i<MIDI_SLOTS; i++)    {
        MidiManager::get()->setMidiInSelected(i, midiInCombos.at(i)->currentText() );
        MidiManager::get()->setMidiOutSelected(i, midiOutCombos.at(i)->currentText() );
    }
    MidiManager::get()->setProgramChangeSlot( ui->programChangeSlot->value()-1 );
    MidiManager::get()->setProgramChangeChannel( ui->programChangeChannel->value()-1 );
    MidiManager::get()->setSequencerControllerSlot( ui->sequencerControllerSlot->value()-1 );

    Preferences::get()->setAutoShowHideMonitors(ui->autoShowHideMonitors->isChecked() );
}


MidiSetup::~MidiSetup() {
    delete ui;
}
