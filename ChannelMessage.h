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

#ifndef CHANNELMESSAGE_H
#define CHANNELMESSAGE_H

#include "mididefs.h"

class ChannelMessage
{

public:

    ChannelMessage();
    ChannelMessage(int slotNumber, unsigned char status, unsigned char channel, unsigned char byte1);
    ChannelMessage(int slotNumber, unsigned char status, unsigned char channel, unsigned char byte1, unsigned char byte2);
    virtual ~ChannelMessage();

    void setSlotNumber(int slotNumber);
    void setStatus(unsigned char type);
    void setChannel(unsigned char channel);
    void setByte1(unsigned char byte1);
    void setByte2(unsigned char byte2);

    int getSlotNumber(void);
    unsigned char getStatus(void);
    unsigned char getChannel(void);
    unsigned char getByte1(void);
    unsigned char getByte2(void);

private:

    int slotNumber;
    unsigned char status;
    unsigned char channel;
    unsigned char byte1;
    unsigned char byte2;

};

#endif // CHANNELMESSAGE_H
