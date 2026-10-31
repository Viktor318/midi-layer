/*
    Midi Layer
    Copyright (C) 2012-2014, António José Soares Maia <maia.ajs@gmail.com>

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

#include "MidiChannelMonitor.h"

MidiChannelMonitor::MidiChannelMonitor(QWidget *parent, direction portDirection)
    : QWidget(parent)   {

    this->hBoxChannels = new QHBoxLayout();
    this->hBoxChannels->setContentsMargins(0, 0, 0, 0);
    this->hBoxChannels->setSpacing(0);

    this->slotLed = new Led(this,16,11);
    if (portDirection == In)    {
        this->slotLed->setColor("#FF0");
        this->slotLed->setText(QString(tr(" IN")));
    } else {
        this->slotLed->setColor("#6F6");
        this->slotLed->setText(QString(tr("OUT")));
    }

    this->hBoxChannels->addWidget(this->slotLed);

    for (int m=0; m<16; m++)    {

        this->led[m] = new Led(this, 16, 11);
        this->led[m]->setColor(QColor(0,0,255));
        this->led[m]->setColorOff(QColor(191,191,191));
        this->led[m]->setTextColor(QColor(255,255,255,191));

        if (m<9)
            this->led[m]->setText("0"+QString::number(m+1));
        else
            this->led[m]->setText(QString::number(m+1));

        this->hBoxChannels->addWidget(led[m]);
    }

    this->hBoxChannels->setContentsMargins(4, 4, 4, 4);
    this->setLayout(this->hBoxChannels);
}

MidiChannelMonitor::~MidiChannelMonitor()   {
    delete slotLed;
    for (int m=0; m<16; m++)
        delete led[m];
    delete hBoxChannels;    
}

void MidiChannelMonitor::flash(unsigned char channel)   {
    if (channel<16)
        this->led[channel]->flash();
    else
        this->slotLed->flash();
}
