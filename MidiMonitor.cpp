/*
    Midi Layer - A simple and intuitive tool to improve live performances
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

#include "MidiMonitor.h"

MidiMonitor::MidiMonitor(QWidget *parent)
    : QDialog(parent)   {

    setupUi(this);
    this->hide();
    this->setWindowModality ( Qt::NonModal );
    this->setModal(false);

    noteOnStr = "[%1] ch: %2, note on:  %3, velocity: %4";
    noteOnColor = QColor("#2D2");
    noteOffStr = "[%1] ch: %2, note off: %3, velocity: %4";
    noteOffColor = QColor("#0A0");
    polyAftertouchStr = "[%1] ch: %2, note aftertouch: %3, value: %4";
    polyAftertouchColor = QColor("#E00");
    controlChangeStr = "[%1] ch: %2, %3, value: %4";
    controlChangeColor = QColor("#22E");
    programChangeStr = "[%1] ch: %2, program change: %3";
    programChangeColor = QColor("#DD2");
    channelAftertouchStr = "[%1] ch: %2, channel aftertouch: %3";
    channelAftertouchColor = QColor("#E00");
    pitchbendStr = "[%1] ch: %2, pitch bend: %3";
    pitchbendColor = QColor("#D91");
    otherEventStr = "[%1] ch: %2, value kk: %3, value vv: %4";
    otherEventColor = QColor("#CCC");

    monitorIn->document()->setMaximumBlockCount(256);
    monitorOut->document()->setMaximumBlockCount(256);

    monitorIn->ensureCursorVisible();
    monitorOut->ensureCursorVisible();

    cbChannel->addItem("all channels");
    for (int c=0; c<16; c++)    {
        cbChannel->addItem(QString("channel %1").arg(c+1));
    }

    actionClose = new QAction(this);
    actionClose->setShortcut(QKeySequence("Ctrl+M"));
    addAction(actionClose);
    connect(actionClose, SIGNAL( triggered() ), this, SLOT(close()));

    actionDelete = new QAction(this);
    actionDelete->setShortcut(QKeySequence::Delete);
    addAction(actionDelete);
    connect(actionDelete, SIGNAL( triggered() ), monitorIn, SLOT(clear()));
    connect(actionDelete, SIGNAL( triggered() ), monitorOut, SLOT(clear()));

    actionLeft = new QAction(this);
    actionLeft->setShortcut(QKeySequence("Left"));
    addAction(actionLeft);
    connect(actionLeft, SIGNAL( triggered() ), this, SLOT(slotKeyLeft()));

    actionRight = new QAction(this);
    actionRight->setShortcut(QKeySequence("Right"));
    addAction(actionRight);
    connect(actionRight, SIGNAL( triggered() ), this, SLOT(slotKeyRight()));

    connect(buttonCheckAll, SIGNAL( released() ), this, SLOT( checkAllEvents()) );
    connect(buttonClear, SIGNAL( released() ), monitorIn, SLOT(clear()));
    connect(buttonClear, SIGNAL( released() ), monitorOut, SLOT(clear()));
    connect(hsDisplayMode, SIGNAL(valueChanged(int)), this, SLOT(slotDisplayMode(int)));

    #ifdef QT_DEBUG

    debugProgramChange = 0;

    action1 = new QAction(this);
    action1->setShortcut(QKeySequence("1"));
    addAction(action1);
    connect(action1, SIGNAL(triggered()), this, SLOT(slotAction_1()));

    action2 = new QAction(this);
    action2->setShortcut(QKeySequence("2"));
    addAction(action2);
    connect(action2, SIGNAL(triggered()), this, SLOT(slotAction_2()));

    action3 = new QAction(this);
    action3->setShortcut(QKeySequence("3"));
    addAction(action3);
    connect(action3, SIGNAL(triggered()), this, SLOT(slotAction_3()));

    action4 = new QAction(this);
    action4->setShortcut(QKeySequence("4"));
    addAction(action4);
    connect(action4, SIGNAL(triggered()), this, SLOT(slotAction_4()));

    #endif
}

//MidiMonitor::~MidiMonitor() {
//}

int MidiMonitor::exec()    {
    this->setWindowModality(Qt::NonModal);
    this->hide();
    this->show();
    return 0;
}

void MidiMonitor::slotReceivedChannelMessage (ChannelMessage channelMessage)  {
    this->parseChannelMessage (&channelMessage, true);
}

void MidiMonitor::slotSentChannelMessage (ChannelMessage channelMessage)  {
    this->parseChannelMessage (&channelMessage, false);
}

/// TODO must be optimized...
void MidiMonitor::parseChannelMessage (ChannelMessage * channelMessage, bool isInput)  {

    QString slotDescription = QString("%1-%2").arg(channelMessage->getSlotNumber()+1).arg(MidiManager::get()->getInPortNameAtSlot(channelMessage->getSlotNumber()));

    int channel = channelMessage->getChannel()+1;

    // Filter MIDI channel
    if (cbChannel->currentIndex()>0 && cbChannel->currentIndex()!=channel)  {
        return;
    }

    bool rawMode = !cbInterpreted->isChecked();

    switch (channelMessage->getStatus())   {
    case STATUS_NOTEON:
        if ( cbNote->isChecked() ) {
            if (rawMode)
                printRaw(channelMessage, (channelMessage->getByte2()==0?&noteOffColor:&noteOnColor), isInput );
            else
                print(noteOnStr.arg(slotDescription).arg(channel, 2).arg(NoteUtil::get()->getNoteName(channelMessage->getByte1()),3).arg(channelMessage->getByte2(),3), (channelMessage->getByte2()==0?&noteOffColor:&noteOnColor), isInput);
        }
        break;
    case STATUS_NOTEOFF:
        if ( cbNote->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &noteOffColor, isInput );
            else
                print(noteOffStr.arg(slotDescription).arg(channel,2).arg(NoteUtil::get()->getNoteName(channelMessage->getByte1()),3).arg(channelMessage->getByte2(),3), &noteOffColor, isInput);
        }
        break;
    case STATUS_POLYAFT:
        if ( cbAftertouch->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &polyAftertouchColor, isInput );
            else
                print(polyAftertouchStr.arg(slotDescription).arg(channel,2).arg(channelMessage->getByte1(),3).arg(channelMessage->getByte2(),3), &polyAftertouchColor, isInput);
        }
    case STATUS_CTLCHG:
        if ( cbControlChange->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &controlChangeColor, isInput );
            else    {
                QString controlerAlias = QString("[%1] %2").arg(channelMessage->getByte1(),3).arg(Utils::get()->getControllersList()->at(channelMessage->getByte1()));
                print(controlChangeStr.arg(slotDescription).arg(channel,2).arg(controlerAlias).arg(channelMessage->getByte2(),3), &controlChangeColor, isInput);
            }
        }
        break;
    case STATUS_PRGCHG:
        if ( cbProgramChange->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &programChangeColor, isInput );
            else
                print(programChangeStr.arg(slotDescription).arg(channel,2).arg(channelMessage->getByte1()), &programChangeColor, isInput);
        }
        break;
    case STATUS_CHANAFT:
        if ( cbAftertouch->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &channelAftertouchColor, isInput);
            else
                print(channelAftertouchStr.arg(slotDescription).arg(channel,2).arg(channelMessage->getByte1()), &channelAftertouchColor, isInput);
        }
        break;
    case STATUS_BENDER:
        if ( cbPitchBend->isChecked()) {
            if (rawMode)
                printRaw(channelMessage, &pitchbendColor, isInput );
            else    {
                int value = channelMessage->getByte2()*0x80 - BENDER_MID + channelMessage->getByte1();
                print(pitchbendStr.arg(slotDescription).arg(channel,2).arg(value,5), &pitchbendColor, isInput);
            }
        }
        break;
    default:
        if (rawMode)
            printRaw(channelMessage, &otherEventColor, isInput );
        else
            print(otherEventStr.arg(slotDescription).arg(channel,2).arg(channelMessage->getByte1(),3).arg(channelMessage->getByte2(),3), &otherEventColor, isInput);
    }
}

void MidiMonitor::print(QString text, QColor * color, bool isInput)   {
    if (isInput)    {
        monitorIn->setTextColor(*color);
        monitorIn->append(text);
    } else  {
        monitorOut->setTextColor(*color);
        monitorOut->append(text);
    }
}

void MidiMonitor::printRaw(ChannelMessage * channelMessage, QColor * color, bool isInput)     {

    unsigned char channel = channelMessage->getChannel();
    QString slotNumberSrt = QString("%1").arg(channelMessage->getSlotNumber()+1);

    QString text = QString("[%1-%2] %3, %4 %5")\
            .arg( slotNumberSrt )\
            .arg( MidiManager::get()->getInPortNameAtSlot(channelMessage->getSlotNumber()) )\
            .arg( (channelMessage->getStatus() + (channel & MASK_CHANNEL)), 2, 16)\
            .arg( (channelMessage->getByte1() & MASK_SAFETY), 2, 16)\
            .arg( (channelMessage->getByte2() & MASK_SAFETY), 2, 16);
    this->print(text, color, isInput);
}

void MidiMonitor::showEvent(QShowEvent * event)   {

    MidiManager::get()->setEmitSentChannelMessage(true);

    connect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT( slotReceivedChannelMessage(ChannelMessage)) );
    connect(MidiManager::get(), SIGNAL(sentChannelMessage(ChannelMessage)), this, SLOT( slotSentChannelMessage(ChannelMessage)) );
    event->accept();
    if (ISLOGGABLE_INFO) LOG_INFO("Monitor opened");
}

void MidiMonitor::closeEvent(QCloseEvent * event)    {

    MidiManager::get()->setEmitSentChannelMessage(false);

    disconnect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT( slotReceivedChannelMessage(ChannelMessage)) );
    disconnect(MidiManager::get(), SIGNAL(sentChannelMessage(ChannelMessage)), this, SLOT( slotSentChannelMessage(ChannelMessage)) );
    event->accept();
    if (ISLOGGABLE_INFO) LOG_INFO("Monitor closed");
}

void MidiMonitor::checkAllEvents() {
    cbNote->setChecked(true);
    cbProgramChange->setChecked(true);
    cbControlChange->setChecked(true);
    cbSystemMessage->setChecked(true);
    cbAftertouch->setChecked(true);
    cbPitchBend->setChecked(true);
}

void MidiMonitor::slotDisplayMode(int displayMode)  {

    switch(displayMode) {
    case -1:
        groupBoxMonitorIn->setVisible(true);
        groupBoxMonitorOut->setVisible(false);
        break;
    case 1:
        groupBoxMonitorIn->setVisible(false);
        groupBoxMonitorOut->setVisible(true);
        break;
    default:
        groupBoxMonitorIn->setVisible(true);
        groupBoxMonitorOut->setVisible(true);
    }

    QScrollBar *sb;
    sb = monitorIn->verticalScrollBar();
    sb->setValue(sb->maximum());
    sb = monitorOut->verticalScrollBar();
    sb->setValue(sb->maximum());

}

void MidiMonitor::slotKeyLeft() {
    hsDisplayMode->setValue(hsDisplayMode->value()-1);
}

void MidiMonitor::slotKeyRight() {
    hsDisplayMode->setValue(hsDisplayMode->value()+1);
}


#ifdef QT_DEBUG
void MidiMonitor::slotAction_1() {

    ChannelMessage  channelMessage1 = ChannelMessage( 0, STATUS_NOTEON, 0, 20 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);

    ChannelMessage  channelMessage2 = ChannelMessage( 0, STATUS_NOTEON, 2, 62 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage2);

    ChannelMessage  channelMessage3 = ChannelMessage( 0, STATUS_NOTEON, 4, 102 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage3);

    MidiManager::get()->toString();
}

void MidiMonitor::slotAction_2() {

    ChannelMessage  channelMessage1 = ChannelMessage( 1, STATUS_NOTEON, 1, 20 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);

    ChannelMessage  channelMessage2 = ChannelMessage( 1, STATUS_NOTEON, 3, 62 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage2);

    ChannelMessage  channelMessage3 = ChannelMessage( 1, STATUS_NOTEON, 5, 102 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage3);

    MidiManager::get()->toString();

}

void MidiMonitor::slotAction_3() {

    ChannelMessage  channelMessage1 = ChannelMessage( 0, STATUS_PRGCHG, 0, --debugProgramChange & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);
}

void MidiMonitor::slotAction_4() {

    ChannelMessage  channelMessage1 = ChannelMessage( 0, STATUS_PRGCHG, 0, ++debugProgramChange & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);
}

#endif

