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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QAtomicPointer>
#include <QMutex>

#include "Logger.h"

class Preferences
{
public:

    static Preferences * get();
    static void Drop();

    void setConfirmOnExit(bool confirmOnExit);
    bool getConfirmOnExit();
    void setCheckForUpdates(bool checkForUpdates);
    bool getCheckForUpdates();
    void setSaveWindowsState(bool saveWindowsState);
    bool getSaveWindowsState();
    void setAutoShowHideMonitors(bool autoShowHideMonitors);
    bool getAutoShowHideMonitors();
    void setAutoSavePresets(bool autoSavePresets);
    bool getAutoSavePresets();
    void setAutoSaveElements(bool autoSaveElements);
    bool getAutoSaveElements();
    void resetBankEdit();
    void setSendMetronomeUdp(bool sendMetronomeUdp);
    bool getSendMetronomeUdp();
    void setMetronomeUdpHostName(QString metronomeUdpHostName);
    QString getMetronomeUdpHostName();
    void setMetronomeUdpNetwork(QString metronomeUdpNetwork);
    QString getMetronomeUdpNetwork();


private:

    //static QAtomicPointer<Preferences> m_preferences;
    static Preferences * m_preferences;
    static QMutex mutex;

    bool confirmOnExit;
    bool checkForUpdates;
    bool saveWindowsState;
    bool autoShowHideMonitors;
    bool autoSavePresets;
    bool autoSaveElements;

    // Send UDP metronome beats
    bool sendMetronomeUdp;
    QString metronomeUdpHostName;
    QString metronomeUdpNetwork;
};

#endif // PREFERENCES_H
