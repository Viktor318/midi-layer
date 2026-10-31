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


#include "MidiManager.h"


static void midiCallback( double /*deltatime*/,
                          std::vector< unsigned char > *message,
                          void *userData )
{

    unsigned char status = message->at(0) & MASK_STATUS;
//    unsigned char byte1 = 0;
    unsigned char byte2 = 0;

    int *slotIndexPtr = static_cast<int*>(userData);
    int slot = *slotIndexPtr;

    // Is a channel message
    if ( status < 0xF0 )    {
        switch( status ) {
//        case STATUS_PRGCHG:
//        case STATUS_CHANAFT:
//            byte1 = message->at(1);
//            break;
        case STATUS_NOTEOFF:
        case STATUS_NOTEON:
        case STATUS_BENDER:
        case STATUS_POLYAFT:
        case STATUS_CTLCHG:
//            byte1 = message->at(1);
            byte2 = message->at(2);
//            break;
//        default:
//            /// All other channel messages
//            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("### Received a unrecognized channel message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
        }
//        if (slot>-1 && slot < MIDI_SLOTS)    {
        MidiManager::get()->relayMessage( ChannelMessage( slot, status, message->at(0) & MASK_CHANNEL, message->at(1), byte2 ) );
//        } else {
//            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("Received message from invalid slot index '%1").arg(slot)) ;
//        }

    }
    // Is a common message
    else    {
        switch( status ) {
        case STATUS_SYSEX_START:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_SYSEX_START common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_MTCFRAME:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_MTCFRAME common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_SEQPOS:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_SEQPOS common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_SONG:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_SONG common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_TUNEREQ:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_TUNEREQ common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_TEMPO:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_TEMPO common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_START:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_START common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_CONTINUE:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_CONTINUE common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_STOP:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_STOP common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_ACTSENSE:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_ACTSENSE common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        case STATUS_RESET:
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### Received a STATUS_RESET common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
            /// TODO: Implement me
            break;
        default:
            // All other messages
            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("### Received a unrecognized common message! STATUS byte is '%1").arg(message->at(0), 2, 16)) ;
        }
    }

}


MidiManager * MidiManager::m_manager;
QMutex MidiManager::mutex;


MidiManager* MidiManager::get() {
    if( !m_manager )
    {
        mutex.lock();
        if( !m_manager )     {
            m_manager = new MidiManager;
            m_manager->init();
        }
        mutex.unlock();
    }
    return m_manager;
}


void MidiManager::Drop()  {
    mutex.lock();
    delete m_manager;
    m_manager = 0;
    mutex.unlock();
}


void MidiManager::init()  {

    LOG_INFO("Initializing MIDI...");

    this->active =true;
    panicTimer = new QTimer();    
    panicTimer->setSingleShot(true);
    panicTimer->setInterval(400);
    connect(panicTimer, SIGNAL(timeout()), this, SLOT(activate()));

    // Need register a <T> for use in Signals & Slots
    qRegisterMetaType<ChannelMessage>("ChannelMessage");

    // Windows fork: a billentyű- és pedál-felengedések a nyilvántartás alapján mennek ki.
    // FONTOS: a jelet az RtMidi saját szálán bocsátjuk ki, a feldolgozás pedig sorba
    // állítva, a felület szálán fut - ugyanabban a sorban, mint az elemek feldolgozása.
    // Így egy felengedés soha nem előzheti meg a hozzá tartozó lenyomás bejegyzését.
    connect(this, SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT(slotReleaseTrackedNotes(ChannelMessage)));

    for (int i=0; i<MIDI_SLOTS; i++)    {

        midiInList.append(0);
        portInInterfaceIndex.append(-1);
        portInOpened.append(false);

        midiOutList.append(0);
        portOutInterfaceIndex.append(-1);

        // Used for store callback slot index as user data
        portSlotIndex[i]=i;

    }

    // Port Index and Channel for program change
    presetProgramChangeSlot = 0;
    presetProgramChangeChannel = 0;
    sequencerControllerSlot = 0;
    metronomeBpm = 120;
    metronomeActive = false;

    emitSentChannelMessage = false;

    // LOG_DEBUG("Initializing MIDI. In port.");
    this->midiInDriver = new RtMidiIn(STR_VIRTUAL_IN);
    // LOG_DEBUG("Initializing MIDI. Out port.");
    this->midiOutDriver = new RtMidiOut(STR_VIRTUAL_OUT);

    // this->midiInDriver->openVirtualPort();
    // this->midiOutDriver->openVirtualPort();

    // Windows fork: a Windows MM MIDI API nem tud virtuális portot nyitni (az RtMidi ilyenkor
    // csak egy figyelmeztetést ír ki portonként), ezért Windowson a virtuális portokat
    // létre sem hozzuk. A virtualInPorts/virtualOutPorts listát máshol nem használja a program.
#ifndef __WINDOWS_MM__
    QString vInPrefixName = QString(STR_VIRTUAL_IN);
    QString vOutPrefixName = QString(STR_VIRTUAL_OUT);

    for ( int i=0; i<VIRTUAL_PORTS; i++ ) {
        // LOG_DEBUG(QString("Initializing MIDI. Virtual port %1.").arg(i+1));
        virtualInPorts.append(new RtMidiIn(QString("%1%2").arg(vInPrefixName).arg(i+1).toStdString()));
        virtualInPorts.at(i)->openVirtualPort();

        virtualOutPorts.append(new RtMidiOut(QString("%1%2").arg(vOutPrefixName).arg(i+1).toStdString()));
        virtualOutPorts.at(i)->openVirtualPort();
    }
#endif

    LOG_INFO("Initializing MIDI. Completed.");
    this->refresh();
}


void MidiManager::refresh() {

    portsIn.clear();
    portsIn = getAvailableInPorts().keys();

    portsOut.clear();
    portsOut = getAvailableOutPorts().keys();

}


QMap<QString,int> MidiManager::getAvailableInPorts()  {

    QString portName;
    QMap<QString,int> availableInPorts;
    // RtMidiIn * probeMidiIn = new RtMidiIn();
    try {
        int nPorts = (int) this->midiInDriver->getPortCount();
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("There are %1 sources available at MIDI input").arg(nPorts));
        for ( int i=0; i<nPorts; i++ ) {
            // portName = QString::fromStdString(this->midiInDriver->getPortName(i)).remove(QRegularExpression("[^a-zA-Z\\d\\s]"));
            portName = QString::fromStdString(this->midiInDriver->getPortName(i));
            if ( !isExcludedMidiPort( portName ) )  {
                // portName.replace(QRegExp(":[0-9]+$"),"");
                availableInPorts.insert(portName,i);
            }
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("%1-'%2'").arg(i).arg(portName));
        };
    }
    catch ( RtError &error ) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
    return availableInPorts;
}


QMap<QString,int> MidiManager::getAvailableOutPorts()  {

    QString portName;
    QMap<QString,int> availableOutPorts;

    // RtMidiOut * probeMidiOut = new RtMidiOut();
    try {
        int nPorts = (int) this->midiOutDriver->getPortCount();
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("There are %1 sources available at MIDI output").arg(nPorts));
        for ( int i=0; i<nPorts; i++ ) {

            // portName = QString::fromStdString(this->midiOutDriver->getPortName(i)).remove(QRegularExpression("[^a-zA-Z\\d\\s]"));
            portName = QString::fromStdString(this->midiOutDriver->getPortName(i));
            if ( !isExcludedMidiPort( portName) )   {
                // portName.replace(QRegExp(":[0-9]+$"),"");
                availableOutPorts.insert(portName,i);
            }
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("%1-'%2'").arg(i).arg(portName));
        }

    }
    catch (RtError &error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
    return availableOutPorts;
}


QStringList MidiManager::getInPorts()  {
    return portsIn;
}


QStringList MidiManager::getOutPorts()  {
    return portsOut;
}


int MidiManager::getInPortsCount()  {
    return portsIn.count();
}


int MidiManager::getOutPortsCount()  {
    return portsOut.count();
}


void MidiManager::setMidiInSelected(int slotIndex, QString portName)   {

    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Required config IN slot %1 with interface %2").arg(slotIndex).arg(portName));
    QMap<QString,int> availableInPorts = getAvailableInPorts();
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Required '%1'' to SLOT%2 available IN ports %3").arg(portName).arg(slotIndex).arg(Utils::get()->toString(availableInPorts.keys())) );

    try {
        closeAndDeleteInPortAtSlot(slotIndex);
        if ( !portName.trimmed().isEmpty() && availableInPorts.contains(portName) )   {
            midiInList.replace(slotIndex, new RtMidiIn(portName.toStdString()));
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Will open IN '%1' at SLOT%2").arg(portName).arg(slotIndex) );
            midiInList.at(slotIndex)->openPort(availableInPorts.value(portName));
            midiInList.at(slotIndex)->ignoreTypes(false, false, false);
            midiInList.at(slotIndex)->setCallback( &midiCallback, &portSlotIndex[slotIndex] );
            portInInterfaceIndex.replace(slotIndex, availableInPorts.value(portName));
            if (ISLOGGABLE_INFO) LOG_INFO(QString("MIDI IN at slot %1 is assigned to interface %2").arg(slotIndex).arg(portName));
        }
        else  {
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("No port IN selected for slot %1").arg(slotIndex));
        }
        emit slotChanged(0, slotIndex);
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
}


void MidiManager::closeAndDeleteInPortAtSlot(int slotIndex)   {
    try {
        if ( midiInList.at(slotIndex) )   {
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("=I=Closing IN interface %1")
                                            .arg(QString::fromStdString( midiInList.at(slotIndex)->getPortName())) );
            midiInList.at(slotIndex)->cancelCallback();
            midiInList.at(slotIndex)->closePort();
            delete midiInList.at(slotIndex);
        }
        portInInterfaceIndex.replace(slotIndex, -1);
        midiInList.replace(slotIndex,0);
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
}


void MidiManager::setMidiOutSelected(int slotIndex, QString portName)   {

    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Required config OUT slot %1 with interface %2").arg(slotIndex).arg(portName));
    QMap<QString,int> availableOutPorts = getAvailableOutPorts();

    try {
        closeAndDeleteOutPortAtSlot(slotIndex);
        if ( !portName.trimmed().isEmpty() && availableOutPorts.contains(portName) )   {
            midiOutList.replace(slotIndex, new RtMidiOut(portName.toStdString() ));
            midiOutList.at(slotIndex)->openPort(availableOutPorts.value(portName));
            portOutInterfaceIndex.replace(slotIndex, availableOutPorts.value(portName));
            if (ISLOGGABLE_INFO) LOG_INFO(QString("MIDI OUT at slot %1 is assigned to interface %2").arg(slotIndex).arg(portName));            
        }
        else  {
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("No port OUT selected for slot %1").arg(slotIndex));
        }
        emit slotChanged(1, slotIndex);
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
}


void MidiManager::closeAndDeleteOutPortAtSlot(int slotIndex)   {
    try {
        if (midiOutList.at(slotIndex) )  {
            midiOutList.at(slotIndex)->closePort();
            delete midiOutList.at(slotIndex);
        }
        portOutInterfaceIndex.replace(slotIndex, -1);
        midiOutList.replace(slotIndex,0);
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
}


bool MidiManager::isExcludedMidiPort(QString portName) {

    QStringList excludedMidiPorts;
    excludedMidiPorts << "Midi Through";
    bool containsExcluded = false;
    for (int i=0; i<excludedMidiPorts.count(); i++) {
        if (portName.startsWith(excludedMidiPorts.at(i)))   {
            containsExcluded = true;
            break;
        }
    }
    return containsExcluded || this->isAlreadyOpenPort(portName);
}


bool MidiManager::isAlreadyOpenPort(QString portName)    {
    QRegularExpression regexp(".*(:\\d:)");
    return (regexp.match(portName).hasMatch());
}


QString MidiManager::getInPortNameAtSlot(int slotIndex)  {
    try {
        if(portInInterfaceIndex.at(slotIndex)>-1)
            return QString::fromStdString( midiInList.at(slotIndex)->getPortName(portInInterfaceIndex.at(slotIndex)) ).toLatin1();
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
    return "[not defined]";
}


QString MidiManager::getOutPortNameAtSlot(int slotIndex)  {
    try {
        if(portOutInterfaceIndex.at(slotIndex)>-1)
            return QString::fromStdString( midiOutList.at(slotIndex)->getPortName(portOutInterfaceIndex.at(slotIndex)) ).toLatin1();
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
    return "[not defined]";
}


QString MidiManager::toString()  {

    try  {
        for (int i= 0; i< MIDI_SLOTS; i++)   {
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString( "IN name:'%1', index %2, is open %3")
                                            .arg( QString::fromStdString( midiInList.at(i)?midiInList.at(i)->getPortName():"(NONE)") )
                                            .arg(portInInterfaceIndex.at(i))
                                            .arg(midiInList.at(i)?true:false ) );
        }
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }

    try {
        for (int i= 0; i< MIDI_SLOTS; i++)   {
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("OUT name:'%1', index %2, is open %3")
                                            .arg( QString::fromStdString( midiOutList.at(i)?midiOutList.at(i)->getPortName():"(NONE)") )
                                            .arg(portOutInterfaceIndex.at(i))
                                            .arg(midiOutList.at(i)?true:false) );
        }
    }
    catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString::fromStdString(error.getMessage()));
    }
    return "";
}


void MidiManager::relayMessage(ChannelMessage channelMessage)   {

    if (this->active)    {
        // Filter the ProgramChange to MidiLyer change preset functionality.
        if ( channelMessage.getStatus() == STATUS_PRGCHG &&
             (presetProgramChangeSlot==-1 || channelMessage.getSlotNumber() == presetProgramChangeSlot) &&
             (presetProgramChangeChannel==-1 || channelMessage.getChannel() == presetProgramChangeChannel) ) {
            emit receivedProgramChange( channelMessage.getByte1() );
            // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("##### Program change emit -> '%1'").arg(channelMessage.getByte1()) );
        } else  {
            emit receivedChannelMessage(channelMessage);
        }
    }
}


void MidiManager::activate()    {
    if (ISLOGGABLE_INFO) LOG_INFO("Layer engine input is ON!");
    this->active = true;
}


void MidiManager::desactivate()    {
    if (this->active)
        if (ISLOGGABLE_INFO) LOG_INFO("Layer engine input is OFF!");
    this->active = false;
}


// ####### Send channel messages

void MidiManager::sendNoteOn(int slotNumber, int channel, const int midiNote, const int vel)    {

    ChannelMessage channelMessage(slotNumber, STATUS_NOTEON, channel, midiNote, vel);
    this->sendChannelMessage(&channelMessage);
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendNoteOn( %1, %2, %3, %4)").arg(slotNumber).arg(channel).arg(midiNote).arg(vel));
}


void MidiManager::sendNoteOff(int slotNumber, int channel, const int midiNote, const int vel)   {

    ChannelMessage channelMessage(slotNumber, STATUS_NOTEOFF, channel, midiNote, vel);
    this->sendChannelMessage(&channelMessage);
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendNoteOff( %1, %2, %3, %4)").arg(slotNumber).arg(channel).arg(midiNote).arg(vel));
}


void MidiManager::sendController(int slotNumber, int channel, const int controller, const int value)    {

    ChannelMessage channelMessage(slotNumber, STATUS_CTLCHG, channel, controller, value);
    this->sendChannelMessage(&channelMessage);
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendController( %1, %2, %3, %4)").arg(slotNumber).arg(channel).arg(controller).arg(value));
}


void MidiManager::sendProgramChange(int slotNumber, int channel, const int program) {

    ChannelMessage channelMessage(slotNumber, STATUS_PRGCHG, channel, program);
    this->sendChannelMessage(&channelMessage);
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendProgramChange( %1, %2, %3)").arg(slotNumber).arg(channel).arg(program));

}


void MidiManager::sendBankChange(int slotNumber, int channel, const unsigned char msb, const unsigned char lsb)    {

    sendController(slotNumber, channel, CTL_MSB, msb);
    sendController(slotNumber, channel, CTL_LSB, lsb);
}


void MidiManager::sendPolyKeyPress(int slotNumber, int channel, const int note, const int value)    {

    ChannelMessage channelMessage(slotNumber, STATUS_POLYAFT, channel, note, value);
    this->sendChannelMessage(&channelMessage);
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendPolyKeyPress( %1, %2, %3, %4)").arg(slotNumber).arg(channel).arg(note).arg(value));
}


void MidiManager::sendChanKeyPress(int slotNumber, int channel, const int value)    {

    ChannelMessage channelMessage(slotNumber, STATUS_CHANAFT, channel, value);
    this->sendChannelMessage(&channelMessage);
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendChanKeyPress( %1, %2, %3)").arg(slotNumber).arg(channel).arg(value));
}


void MidiManager::sendBender(int slotNumber, int channel, const int value)
{
    int v = value + BENDER_MID; // v >= 0, v <= 16384
    unsigned char lsb  = static_cast<unsigned char>(CALC_LSB(v));
    unsigned char msb  = static_cast<unsigned char>(CALC_MSB(v));
    // Program: 0xE0 + channel, lsb, msb
    sendBender(slotNumber, channel, lsb, msb);
}


void MidiManager::sendBender(int slotNumber, int channel, const unsigned char lsb, const unsigned char msb)
{
    ChannelMessage channelMessage(slotNumber, STATUS_BENDER, channel, lsb, msb);
    this->sendChannelMessage(&channelMessage);
    //if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendBender( %1, %2, %3, %4)").arg(slotNumber).arg(channel).arg(lsb).arg(msb));
}


void MidiManager::sendSysex(int slotNumber, std::vector<unsigned char> *message)  {
    // DEBUG
    #ifdef QT_DEBUG
    if (ISLOGGABLE_TRACE)   {
        QString debugMesg;
        for (unsigned int n = 0; n < message->size(); n++)
            if (n==0)
                debugMesg.append(QString("%1").arg(message->at(n), 2,16));
            else
                debugMesg.append(QString(" %1").arg(message->at(n), 2,16));
        LOG_TRACE(QString("send system exclusive at port %1. Data: %2").arg(slotNumber).arg(debugMesg) );
    }
    #endif

    this->sendMessageWrapper( slotNumber, message );
    #ifndef NO_CHANNEL_MONITOR
        emit flashLedMonitor( slotNumber, 16 );
    #endif //NO_CHANNEL_MONITOR

}


void MidiManager::sendChannelMessage(ChannelMessage * channelMessage)    {

    unsigned char channel = channelMessage->getChannel();
    std::vector<unsigned char> message;

    message.push_back(channelMessage->getStatus() + (channel & MASK_CHANNEL));
    message.push_back(channelMessage->getByte1() & MASK_SAFETY);
    // Program change and Aftertouch only needs 1 data byte
    switch (channelMessage->getStatus())   {
    case STATUS_NOTEON:
    case STATUS_NOTEOFF:
    case STATUS_POLYAFT:
    case STATUS_CTLCHG:
    case STATUS_BENDER:
        message.push_back(channelMessage->getByte2() & MASK_SAFETY);
    }

    this->sendMessageWrapper( channelMessage->getSlotNumber(), &message );

    if (emitSentChannelMessage) {
        emit sentChannelMessage(*channelMessage);
    }
#ifndef NO_CHANNEL_MONITOR
    emit flashLedMonitor( channelMessage->getSlotNumber(), channelMessage->getChannel() );
#endif //NO_CHANNEL_MONITOR
    // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendChannelMessage( slot %1, status %2, channel %3, kkk %4, vvv %5)").arg(channelMessage.getSlotNumber()).arg(channelMessage.getStatus()).arg(channelMessage.getChannel()+1).arg(channelMessage.getByte1()).arg(channelMessage.getByte2()));
}


void MidiManager::setEmitSentChannelMessage(bool emitSentChannelMessage)    {
    this->emitSentChannelMessage = emitSentChannelMessage;
}


void MidiManager::sendSeqStart()    {
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Sequence start");

    std::vector<unsigned char> message;
    message.push_back(STATUS_START);
    this->sendSequencerMessage(&message);
}


void MidiManager::sendSeqContinue()    {
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Sequence continue");

    std::vector<unsigned char> message;
    message.push_back(STATUS_CONTINUE);
    this->sendSequencerMessage(&message);
}


void MidiManager::sendSeqStop()    {
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Sequence stop");

    std::vector<unsigned char> message;
    message.push_back(STATUS_STOP);
    this->sendSequencerMessage(&message);
}


void MidiManager::sendSeqPosition(int position)    {
    if (ISLOGGABLE_INFO) LOG_INFO(QString("Send sequencer position %1").arg(position) );

    unsigned char lsb  = static_cast<unsigned char>(CALC_LSB(position));
    unsigned char msb  = static_cast<unsigned char>(CALC_MSB(position));

    std::vector<unsigned char> message;
    message.push_back(STATUS_SEQPOS);
    message.push_back(lsb & MASK_SAFETY);
    message.push_back(msb & MASK_SAFETY);
    this->sendSequencerMessage(&message);
}


void MidiManager::sendSeqSelectSong(int songId)    {
    if (ISLOGGABLE_INFO) LOG_INFO(QString("Send select song %1").arg(songId) );

    std::vector<unsigned char> message;
    message.push_back(STATUS_SONG);
    /// TODO: APPLY SAFTY MASK
    message.push_back(songId);
    this->sendSequencerMessage(&message);
}


void MidiManager::sendSequencerMessage(std::vector<unsigned char> *message) {
    if (sequencerControllerSlot<0)  {
        for (int s=0; s<MIDI_SLOTS; s++) {
            this->sendMessageWrapper( s, message );
            #ifndef NO_CHANNEL_MONITOR
            emit flashLedMonitor( s, 17 );
            #endif //NO_CHANNEL_MONITOR
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendSequencerMessage( %1, message)").arg(s));
        }
    } else  {
        this->sendMessageWrapper( sequencerControllerSlot, message );
        #ifndef NO_CHANNEL_MONITOR
        emit flashLedMonitor( sequencerControllerSlot, 17 );
        #endif //NO_CHANNEL_MONITOR
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("MidiManager->sendSequencerMessage( %1, message)").arg(sequencerControllerSlot));
    }
}


void MidiManager::sendMessageWrapper(int slotIndex, std::vector<unsigned char> * message) {

    try {
        if (midiOutList.at(slotIndex))    {
            midiOutList.at(slotIndex)->sendMessage(message);
#ifdef QT_DEBUG
            // DEBUG
            QString debugMessage;
            for (unsigned int n = 0; n < message->size(); n++)
                if (n==0)
                    debugMessage.append(QString("%1").arg(message->at(n), 2, 16));
                else
                    debugMessage.append(QString(" %1").arg(message->at(n), 2, 16));
            // if (ISLOGGABLE_TRACE) LOG_TRACE("Send Message: " + debugMessage);
#endif
        }
    } catch (RtError& error) {
        error.printMessage();
        LOG_ERROR(QString("ERROR sendMessageWrapper: %1").arg(QString::fromStdString(error.getMessage())) );
    }
}


void MidiManager::slotPanic()   {
    if (ISLOGGABLE_INFO) LOG_INFO("Send PANIC!");
    this->desactivate();
    // Windows fork: a pánik minden hangot leállít, a nyilvántartás így érvényét veszti
    this->clearTrackedNotes();
    for (int slotIndex=0; slotIndex<MIDI_SLOTS; slotIndex++)    {
        for (int channel=0; channel<16; channel++)    {
            allNotesOff(slotIndex, channel);
            resetAllControllers(slotIndex, channel);
            allSoundOff(slotIndex, channel);
        }
    }
    panicTimer->start();
}


void MidiManager::allSoundOff(int slotIndex, int channel) {   
    sendController(slotIndex, channel, CTL_ALL_SOUND_OFF, 0);
    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("All sound off at slot '%1', channel'%2'").arg(getInPortNameAtSlot(slotIndex)).arg(channel));
}


void MidiManager::resetAllControllers(int slotIndex, int channel) {    
    sendController(slotIndex, channel, CTL_RESET_ALL_CTL, 0);
    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Reset all controllers at slot '%1', channel'%2'").arg(getInPortNameAtSlot(slotIndex)).arg(channel));
}


void MidiManager::resetAllControllers(int slotIndex) {
    if (midiOutList.at(slotIndex))      {
        for (int channel=0; channel<16; channel++)
            resetAllControllers(slotIndex, channel);
    }
}


void MidiManager::resetAllControllers() {
    for (int slot=0; slot<MIDI_SLOTS; slot++)    {
        this->resetAllControllers(slot);
    }
}


// ####### Windows fork: megszólaltatott hangok és kitartó pedál nyilvántartása

void MidiManager::registerSoundingNote(int inSlot, int inChannel, int inNote, int outSlot, int outChannel, int outNote)    {
    OutTarget target = { outSlot, outChannel, outNote };
    QList<OutTarget> & targets = soundingNotes[trackKey(inSlot, inChannel, inNote)];
    if (!targets.contains(target))
        targets.append(target);
}


void MidiManager::registerSustainOn(int inSlot, int inChannel, int outSlot, int outChannel)    {
    OutTarget target = { outSlot, outChannel, 0 };
    QList<OutTarget> & targets = sustainedOutputs[trackKey(inSlot, inChannel, 0)];
    if (!targets.contains(target))
        targets.append(target);
}


void MidiManager::slotReleaseTrackedNotes(ChannelMessage channelMessage)   {

    unsigned char status = channelMessage.getStatus();

    // Billentyű felengedése: Note Off, vagy 0-s velocityjű Note On (a régi MIDI-szokás szerint)
    if ( status == STATUS_NOTEOFF || (status == STATUS_NOTEON && channelMessage.getByte2() == 0) )  {
        int key = trackKey(channelMessage.getSlotNumber(), channelMessage.getChannel(), channelMessage.getByte1());
        QList<OutTarget> targets = soundingNotes.take(key);
        for (const OutTarget & target : targets)   {
            sendNoteOff(target.slot, target.channel, target.note, channelMessage.getByte2());
        }
        return;
    }

    // Kitartó pedál (CC64) felengedése: 64 alatti érték
    if ( status == STATUS_CTLCHG && channelMessage.getByte1() == CTL_SUSTAIN && channelMessage.getByte2() < 64 )  {
        int key = trackKey(channelMessage.getSlotNumber(), channelMessage.getChannel(), 0);
        QList<OutTarget> targets = sustainedOutputs.take(key);
        for (const OutTarget & target : targets)   {
            sendController(target.slot, target.channel, CTL_SUSTAIN, 0);
        }
    }
}


void MidiManager::clearTrackedNotes()   {
    soundingNotes.clear();
    sustainedOutputs.clear();
}


void MidiManager::allNotesOff(int slotIndex, int channel) {    
    sendController(slotIndex, channel, CTL_ALL_NOTES_OFF, 0);
    if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("All notes off at slot '%1', channel'%2'").arg(getInPortNameAtSlot(slotIndex)).arg(channel));
}


void MidiManager::allNotesOff(int slotIndex) {
    if (midiOutList.at(slotIndex))      {
        for (int channel=0; channel<16; channel++)
            allNotesOff(slotIndex, channel);
    }
}


void MidiManager::allNotesOff() {
    for (int slotIndex=0; slotIndex<MIDI_SLOTS; slotIndex++)    {
        this->allNotesOff(slotIndex);
    }
}


void MidiManager::setProgramChangeSlot(int slotIndex)    {
    presetProgramChangeSlot = slotIndex;
}


int MidiManager::getProgramChangeSlot()    {
    return presetProgramChangeSlot ;
}


void MidiManager::setProgramChangeChannel(int channelIndex)    {
    presetProgramChangeChannel = channelIndex;
}


int MidiManager::getProgramChangeChannel()    {
    return presetProgramChangeChannel ;
}


void MidiManager::setSequencerControllerSlot(int slotIndex)    {
    this->sequencerControllerSlot = slotIndex;
}


int MidiManager::getSequencerControllerSlot()    {
    return this->sequencerControllerSlot;
}


void MidiManager::slotMetronomeActive(bool active)   {
    this->metronomeActive = active;
}


void MidiManager::slotMetronomeBpm(int bpm) {
    this->metronomeBpm = bpm;
}


void MidiManager::slotMetronomeVolume(int volume) {
    this->metronomeVolume = volume;
}

