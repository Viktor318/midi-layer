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

#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include <QAtomicPointer>
#include <QMutex>
#include <QString>
#include <QList>
#include <QMap>
#include <QHash>
#include <QStringList>
#include <QTimer>

#include "RtMidi/RtMidi.h"
#include "RtMidi/RtError.h"

#include "Logger.h"
#include "config.h"
#include "mididefs.h"
#include "ChannelMessage.h"

class MidiManager: public QObject
{
    Q_OBJECT


public:

    void forwardMidiEvent(ChannelMessage channelMessage);
    static MidiManager * get();
    RtMidiIn * midiInDriver;
    RtMidiOut * midiOutDriver;
    QList<RtMidiIn*> virtualInPorts;
    QList<RtMidiOut*> virtualOutPorts;
    static void Drop();
    void refresh();
    QStringList getInPorts();
    QStringList getOutPorts();
    int getInPortsCount();
    void setMidiOutSelected(int slotIndex, QString portName);
    int getOutPortsCount();
    void setMidiInSelected(int slotIndex, QString portName);
    QString getInPortNameAtSlot(int slotIndex);
    QString getOutPortNameAtSlot(int slotIndex);
    QString toString();
    bool metronomeActive;
    int metronomeBpm;
    int metronomeVolume;

    QMap<QString,int> getAvailableInPorts();
    QMap<QString,int> getAvailableOutPorts();

    void thruToChannel(unsigned char channel, ChannelMessage channelMessage);
    void changeChannel(ChannelMessage * channelMessage, unsigned char channel);
    void sendChannelMessage(ChannelMessage * channelMessage);
    void sendController(int portNumber, int channel, const int controller, const int value);
    void sendBankChange(int portNumber, int channel, const unsigned char msb, const unsigned char lsb);
    void sendNoteOn(int portNumber, int channel, const int midiNote, const int vel);
    void sendNoteOff(int portNumber, int channel, const int midiNote, const int vel);
    void sendProgramChange(int portNumber, int channel, const int program);
    void sendBender(int portNumber, int channel, const unsigned char msb, const unsigned char lsb);
    void sendBender(int portNumber, int channel, const int value);
    void sendPolyKeyPress(int portNumber, int channel, const int note, const int value);
    void sendChanKeyPress(int portNumber, int channel, const int value);
    void sendSysex(int portNumber, std::vector<unsigned char> *message);
    void allNotesOff(int slotIndex, int channel);
    void allNotesOff(int portNumber );
    void allNotesOff();
    void resetAllControllers(int slotIndex, int channel);
    void resetAllControllers(int portNumber);
    void resetAllControllers();
    void allSoundOff(int slotIndex, int channel);
    void sendSeqStart();
    void sendSeqContinue();
    void sendSeqStop();
    void sendSequencerMessage(std::vector<unsigned char> *message);
    void sendSeqPosition(int position);
    void sendSeqSelectSong(int songId);

    void setProgramChangeSlot(int portIndex);
    int getProgramChangeSlot();
    void setProgramChangeChannel(int channelIndex);
    int getProgramChangeChannel();
    void setSequencerControllerSlot(int slotIndex);
    int getSequencerControllerSlot();
    void setEmitSentChannelMessage(bool emitSentChannelMessage);

    // Windows fork: a megszólaltatott hangok és a lenyomott kitartó pedál nyilvántartása.
    // Az elemek a kiküldés után bejegyzik, hogy melyik bemenet (slot + csatorna + billentyű)
    // hová szólaltatott meg hangot, illetve hová küldött pedál-lenyomást. A billentyű vagy
    // a pedál felengedését a MidiManager ennek alapján pontosan oda küldi, ahová a lenyomás
    // ment - az aktuális preset és elemek beállításaitól (tartomány, transzponálás, kimeneti
    // slot / csatorna, be-/kikapcsolt állapot) függetlenül. Így preset-váltáskor sem
    // ragadnak be hangok.
    void registerSoundingNote(int inSlot, int inChannel, int inNote, int outSlot, int outChannel, int outNote);
    void registerSustainOn(int inSlot, int inChannel, int outSlot, int outChannel);


public slots:

    void slotPanic();
    void relayMessage(ChannelMessage channelMessage);
    void slotMetronomeBpm(int bpm);
    void slotMetronomeActive(bool active);
    void slotMetronomeVolume(int volume);


private slots:

    void activate();
    void desactivate();
    void slotReleaseTrackedNotes(ChannelMessage channelMessage);


private:

    //static QAtomicPointer<MidiManager> m_manager;
    static MidiManager * m_manager;
    static QMutex mutex;

    void init();

    QStringList portsIn;    
    QList<RtMidiIn *> midiInList;
    QList<int> portInInterfaceIndex;
    int portSlotIndex[MIDI_SLOTS];
    QList<bool> portInOpened;
    QStringList portsOut;
    QList<RtMidiOut*> midiOutList;
    QList<int> portOutInterfaceIndex;
    QList<bool> portOutOpened;

    int presetProgramChangeSlot;
    int presetProgramChangeChannel;

    int sequencerControllerSlot;

    bool active;
    QTimer * panicTimer;

    bool isExcludedMidiPort(QString portName);
    bool isAlreadyOpenPort(QString portName);
    void closeAndDeleteInPortAtSlot(int slotIndex);
    void closeAndDeleteOutPortAtSlot(int slotIndex);
    void sendByte(int slot, unsigned char byte) ;
    void sendMessageWrapper(int portNumber, std::vector<unsigned char> *message);

//    QString virtualMidiInName;
//    QString virtualMidiOutName;

//    RtMidiIn * virtualMidiIn;
//    RtMidiOut * virtualMidiOut;

    bool emitSentChannelMessage;

    // Windows fork: a hang- és pedál-nyilvántartás (lásd registerSoundingNote)
    struct OutTarget {
        int slot;
        int channel;
        int note;       // a pedálnál nem használt
        bool operator==(const OutTarget & other) const {
            return slot == other.slot && channel == other.channel && note == other.note;
        }
    };
    QHash<int, QList<OutTarget>> soundingNotes;     // kulcs: bemeneti slot + csatorna + billentyű
    QHash<int, QList<OutTarget>> sustainedOutputs;  // kulcs: bemeneti slot + csatorna
    static int trackKey(int slot, int channel, int note) { return (slot << 11) | ((channel & 0x0F) << 7) | (note & 0x7F); }
    void clearTrackedNotes();


signals:

    void flashLedMonitor(int portNumber, unsigned char channel);
    void receivedChannelMessage(ChannelMessage channelMessage);
    void sentChannelMessage(ChannelMessage channelMessage);
    void receivedProgramChange(int programChangeValue);
    void slotChanged(int inOrOut, int slotIndex);

};

#endif // MIDIMANAGER_H

