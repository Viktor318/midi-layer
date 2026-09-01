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

#include "ChannelMessage.h"

ChannelMessage::ChannelMessage()    {
    this->slotNumber = 0;
    this->status = 0;
    this->channel = 0;
    this->byte1 = 0;
    this->byte2 = 0;
}

ChannelMessage::ChannelMessage(int slotNumber, unsigned char status, unsigned char channel, unsigned char byte1)    {
    this->slotNumber = slotNumber;
    this->status = status & MASK_STATUS;
    this->channel = channel & MASK_CHANNEL;
    this->byte1 = byte1 & MASK_SAFETY;
    this->byte2 = 0;
}

ChannelMessage::ChannelMessage(int slotNumber, unsigned char status, unsigned char channel, unsigned char byte1, unsigned char byte2) {
    this->slotNumber = slotNumber;
    this->status = status & MASK_STATUS;
    this->channel = channel & MASK_CHANNEL;
    this->byte1 = byte1 & MASK_SAFETY;
    this->byte2 = byte2 & MASK_SAFETY;
}

ChannelMessage::~ChannelMessage()    {
}

void ChannelMessage::setSlotNumber(int slotNumber)  {
    this->slotNumber = slotNumber;
}

void ChannelMessage::setStatus(unsigned char status)    {
    this->status = status & MASK_STATUS;
}

void ChannelMessage::setChannel(unsigned char channel)  {
    this->channel = channel & MASK_CHANNEL;
}

void ChannelMessage::setByte1(unsigned char byte1)  {
    this->byte1 = byte1 & MASK_SAFETY;
}

void ChannelMessage::setByte2(unsigned char byte2)  {
    this->byte2 = byte2 & MASK_SAFETY;
}

int ChannelMessage::getSlotNumber(void)  {
    return this->slotNumber;
}

unsigned char ChannelMessage::getStatus(void) {
    return this->status;
}

unsigned char ChannelMessage::getChannel(void)  {
    return this->channel;
}

unsigned char ChannelMessage::getByte1(void)    {
    return this->byte1;
}

unsigned char ChannelMessage::getByte2(void)    {
    return this->byte2;
}
