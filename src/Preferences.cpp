#include "Preferences.h"


//QAtomicPointer<Preferences> Preferences::m_preferences;
Preferences * Preferences::m_preferences;
QMutex Preferences::mutex;


Preferences* Preferences::get() {
    if( !m_preferences )
    {
        mutex.lock();
        if( !m_preferences )     {
            m_preferences = new Preferences;
        }
        mutex.unlock();
    }
    return m_preferences;
}


void Preferences::Drop()  {
    mutex.lock();
    delete m_preferences;
    m_preferences = 0;
    mutex.unlock();
}


void Preferences::setConfirmOnExit(bool confirmOnExit) {
    this->confirmOnExit = confirmOnExit;
}


bool Preferences::getConfirmOnExit()    {
    return this->confirmOnExit;
}


void Preferences::setCheckForUpdates(bool checkForUpdates) {
    this->checkForUpdates = checkForUpdates;
}


bool Preferences::getCheckForUpdates()    {
    return this->checkForUpdates;
}


void Preferences::setSaveWindowsState(bool saveWindowsState) {
    this->saveWindowsState = saveWindowsState;
}


bool Preferences::getSaveWindowsState()    {
    return this->saveWindowsState;
}


void Preferences::setAutoShowHideMonitors(bool autoShowHideMonitors) {
    this->autoShowHideMonitors = autoShowHideMonitors;
}

bool Preferences::getAutoShowHideMonitors()    {
    return this->autoShowHideMonitors;
}


void Preferences::setSendMetronomeUdp(bool sendMetronomeUdp)    {
    this->sendMetronomeUdp = sendMetronomeUdp;
}

bool Preferences::getSendMetronomeUdp() {
    return this->sendMetronomeUdp;
}


void Preferences::setMetronomeUdpHostName(QString metronomeUdpHostName)    {
    this->metronomeUdpHostName = metronomeUdpHostName;
}


QString Preferences::getMetronomeUdpHostName() {
    return this->metronomeUdpHostName;
}


void Preferences::setMetronomeUdpNetwork(QString metronomeUdpNetwork)    {
    this->metronomeUdpNetwork = metronomeUdpNetwork;
}


QString Preferences::getMetronomeUdpNetwork() {
    return this->metronomeUdpNetwork;
}
