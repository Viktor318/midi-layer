/*
    MidiLayer
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

#ifndef MIDICHANNELMONITOR_H
#define MIDICHANNELMONITOR_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>

#include "Utils.h"
#include "Led.h"

class MidiChannelMonitor : public QWidget
{
    Q_OBJECT
    Q_ENUMS (direction)


public:
    enum direction {In=0,Out};
    MidiChannelMonitor(QWidget *parent = 0, direction portDirection=In);
    virtual ~MidiChannelMonitor();


private:
    QHBoxLayout * hBoxTitle;
    QHBoxLayout * hBoxChannels;    
    Led * led[16];
    Led * slotLed;

public slots:
    void flash(unsigned char channel);

};

#endif // MIDICHANNELMONITOR_H
