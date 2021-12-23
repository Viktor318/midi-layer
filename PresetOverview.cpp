/*
    Midi Layer
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

#include "PresetOverview.h"

PresetOverview::PresetOverview(QWidget *parent, ElementsTab * elementsTab) :
    QDialog(parent)
{

    this->tabElement = elementsTab;

    this->setGeometry(QRect(100,100,80,560));
    this->hBoxElementsStrip = new QHBoxLayout();
    this->hBoxElementsStrip->setSpacing(0);

    this->addSeparator();

    int i;
    for (i=0; i<elementsTab->count(); i++)    {
        ElementStrip * strip = new ElementStrip(this, elementsTab->element(i));
        connect (strip, SIGNAL(analizeConflicts()), this, SLOT( slotElementConflictAnalizer() ) );
        strips.append(strip);
        hBoxElementsStrip->addWidget(strips.at(i));
        this->addSeparator();
    }

    // Add Master Strip
    MasterStrip * masterStrip = new MasterStrip(this, &strips);
    connect (masterStrip, SIGNAL(analizeConflicts()), this, SLOT( slotElementConflictAnalizer() ) );
    hBoxElementsStrip->addWidget(masterStrip);
    this->addSeparator();

    setLayout(hBoxElementsStrip);
#ifdef __MACOSX_CORE__
    this->setMinimumWidth(68*i+68+16);
    this->setMaximumWidth(68*i+68+16);
    this->setMinimumHeight(630);
    this->setMaximumWidth(640);
#else
    this->setMinimumWidth(62*i+62+16);
    this->setMaximumWidth(62*i+62+16);
    this->setMinimumHeight(610);
    this->setMaximumWidth(610);
#endif

    this->setSizeGripEnabled(false);
    this->setWindowFlags(Qt::Window);
    this->slotElementConflictAnalizer();

    this->setAcceptDrops(true);
}

void PresetOverview::addSeparator() {
    QWidget *verticalLineWidget = new QWidget;
    verticalLineWidget->setFixedWidth(1);
    verticalLineWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    verticalLineWidget->setStyleSheet(QString("background-color: #c0c0c0;"));
    hBoxElementsStrip->addWidget(verticalLineWidget);
}

void PresetOverview::slotElementConflictAnalizer()  {

    QTime iniTime = QTime::currentTime();

    // Reset conflicts marks
    for (int s=0; s<strips.count(); s++)    {
        strips.at(s)->markBankConflict(false);
        strips.at(s)->markProgramConflict(false);
        strips.at(s)->markVolumeConflict(false);
        strips.at(s)->markResetConflict(false);
        strips.at(s)->markSysexConflict(false);
    }

    QMap<int, QMap<int, QList< QList<int> > > > mem;

    int pBank=0;
    int pProgram=1;
    int pVolume=2;
    int pSysex=3;
    int pReset=4;

    for (int i=0; i<tabElement->count(); i++)    {

        // Map slot
        //        + Map channel
        //                    + List
        //                         + List bank
        //                         + List program
        //                         + List volume
        //                         + List sysex

        ElementWidget * element = (ElementWidget *) tabElement->widget(i);
        if (element->isActive())    {

            int slot = element->ui->cbPortOut->currentIndex();
            if (!mem.keys().contains(slot)) {
                QMap<int, QList< QList<int> > > channelLevel;
                mem.insert(slot, channelLevel);
            }

            int channel = element->ui->sbMidiOutChannel->value();
            if (!mem.value(slot).keys().contains(channel))   {
                QList< QList<int> > parametersLevel;
                QList<int> bankList;
                QList<int> programList;
                QList<int> volumeList;
                QList<int> sysexList;
                QList<int> resetList;
                parametersLevel.append(bankList);    // bank
                parametersLevel.append(programList); // program
                parametersLevel.append(volumeList);  // volume
                parametersLevel.append(sysexList);   // sysex
                parametersLevel.append(resetList);   // reset
                mem[slot].insert(channel, parametersLevel);
            }

            if (element->ui->groupBoxBankSelect->isChecked())   {
                mem[slot][channel][pBank].append(i);
            }

            if (element->ui->groupBoxProgramChange->isChecked())   {
                mem[slot][channel][pProgram].append(i);
            }

            if (element->ui->groupBoxVolume->isChecked())   {
                mem[slot][channel][pVolume].append(i);
            }

            if (element->ui->groupBoxSendSystemExclusive->isChecked() &&
                    (    !element->ui->systemExclusiveOnSelect->text().isEmpty()
                         || !element->ui->systemExclusiveOnUnselect->text().isEmpty()  ) )    {
                mem[slot][channel][pSysex].append(i);
            }

            if (element->ui->cbResetChannelOnUnselect->isChecked())   {
                mem[slot][channel][pReset].append(i);
            }

        }
    }

    for( QMap<int, QMap<int, QList< QList<int> > > >::iterator itSlot = mem.begin(); itSlot != mem.end(); ++itSlot)  {
        for( QMap<int, QList< QList<int> > >::iterator itChannel = itSlot.value().begin(); itChannel != itSlot.value().end(); ++itChannel)  {
            if (itChannel.value().at(pBank).count()>1)   {
                for (int j=0; j<itChannel.value().at(pBank).count(); j++)    {
                    int conflictIndex = itChannel.value()[pBank][j];
                    strips.at( conflictIndex )->markBankConflict(true);
                }
            }

            if (itChannel.value().at(pProgram).count()>1)   {
                for (int j=0; j<itChannel.value().at(pProgram).count(); j++)    {
                    int conflictIndex = itChannel.value()[pProgram][j];
                    strips.at( conflictIndex )->markProgramConflict(true);
                }
            }

            if (itChannel.value().at(pVolume).count()>1)   {
                for (int j=0; j<itChannel.value().at(pVolume).count(); j++)    {
                    int conflictIndex = itChannel.value()[pVolume][j];
                    strips.at( conflictIndex )->markVolumeConflict(true);
                }
            }

            if (itChannel.value().at(pReset).count()>1)   {
                for (int j=0; j<itChannel.value().at(pReset).count(); j++)    {
                    int conflictIndex = itChannel.value()[pReset][j];
                    strips.at( conflictIndex )->markResetConflict(true);
                }
            }

            if (itChannel.value().at(pSysex).count()>1)   {
                for (int j=0; j<itChannel.value().at(pSysex).count(); j++)    {
                    int conflictIndex = itChannel.value()[pSysex][j];
                    strips.at( conflictIndex )->markSysexConflict(true);
                }
            }
        }
    }

    LOG_DEBUG(QString("Elements conflict analizer spent %1 msecs").arg(QTime::currentTime().msecsSinceStartOfDay()-iniTime.msecsSinceStartOfDay()));

}



