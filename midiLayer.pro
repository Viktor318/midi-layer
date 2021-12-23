#
#    Midi Layer - A simple and intuitive tool to improve live performances
#    Copyright (C) 2012-2022, António José Soares Maia <maia.ajs@gmail.com>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; If not, see <http://www.gnu.org/licenses/>.
#

VERSION=1.1.0-alfa

#unix    {
#    VERSION=$$system(echo $(date +%Y\-%m\-%d\_%H\:%M))
#}

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = MidiLayer

DESTDIR += $$TARGET

CONFIG += qt
#CONFIG += console

# Update Notifier and Metronome UDP
CONFIG += network

#CONFIG += debug
CONFIG += release
#CONFIG += debug_and_release
#CONFIG += build_all

# Trace, Debug, Info, Warn, Error, Fatal
DEFINES += __LOG_LEVEL__=Info
DEFINES += __LOG_FILE__=\\\"MidiLayer.log\\\"    # in temp dir

debug   {
    #DEFINES += __RTMIDI_DEBUG__
    #DEFINES += QT_DEBUG
}

unix:!macx    {
    DEFINES += BUILD_REVISION=\\\"$$system(svn info | grep 'Revision:'| cut -b11- )\\\"
    DEFINES += RTMIDI_VERSION=\\\"$$system(./getRtMidiVersion.sh)\\\"
}

win32   {
    DEFINES += BUILD_REVISION=\\\"1\\\"
    DEFINES += RTMIDI_VERSION=\\\"1.0.5\\\"
}

macx   {
    DEFINES += BUILD_REVISION=\\\"1\\\"
    DEFINES += RTMIDI_VERSION=\\\"1.0.5\\\"
}

QT += widgets
QT += network
QT += xml
#QT += phonon

HEADERS += main.h \
    config.h \
    MidiMonitor.h \
    mididefs.h \
    ChannelMessage.h \
    controllers.h \
    NoteUtil.h \
    MainWindow.h \
    PresetsListWidget.h \
    PresetWidget.h \
    ElementWidget.h \
    Utils.h \
    Led.h \
    ElasticLabel.h \
    MidiManager.h \
    MidiChannelMonitor.h\
    LogFile.h \
    Logger.h \
    Knob.h \
    LoadBankDialog.h \
    SequencerController.h \
    AboutDialog.h \
    Metronome.h \
    MidiSetup.h \
    PreferencesDialog.h \
    Preferences.h \
    PanicPanel.h \
    LinkButton.h \
    UpdateNotifier.h \
    ElementsTab.h \
    PresetInterface.h \
    PresetOverview.h \
    ElementStrip.h \
    MasterStrip.h \
    VertLabel.h

INCLUDEPATH += RtMidi

SOURCES += main.cpp \
    AboutDialog.cpp \
    ChannelMessage.cpp \
    ElasticLabel.cpp \
    ElementWidget.cpp \
    Knob.cpp \
    Led.cpp \
    LinkButton.cpp \
    LoadBankDialog.cpp \
    LogFile.cpp \
    Logger.cpp \
    MainWindow.cpp \
    Metronome.cpp \
    MidiChannelMonitor.cpp \
    MidiManager.cpp \
    MidiMonitor.cpp \
    MidiSetup.cpp \
    NoteUtil.cpp \
    PanicPanel.cpp \
    Preferences.cpp \
    PreferencesDialog.cpp \
    PresetsListWidget.cpp \
    PresetWidget.cpp \
    SequencerController.cpp \
    UpdateNotifier.cpp \
    Utils.cpp \
    ElementsTab.cpp \
    PresetOverview.cpp \
    ElementStrip.cpp \
    MasterStrip.cpp \
    VertLabel.cpp

SOURCES += RtMidi/RtMidi.cpp

FORMS += \
    AboutDialog.ui \
    Element.ui \
    LoadBankDialog.ui \
    MainWindow.ui \
    Metronome.ui \
    MidiMonitor.ui \
    MidiSetup.ui \
    PanicPanel.ui \
    PreferencesDialog.ui \
    SequencerController.ui \
    ElementStrip.ui \
    MasterStrip.ui

unix:!macx {
    message(RtMidi will use linux alsaseq.)
    # alsa:
    LIBS+=-lasound
    LIBS += -lpthread
    DEFINES+=__LINUX_ALSA__
    DEFINES+=__LINUX_ALSASEQ__
    # jack;
    #LIBS+=-ljack
    #DEFINES+=__LINUX_JACK__
    DEFINES+=__UNIX_JACK__
    DEFINES += BUILD_OS=\\\"Linux\\\"
}

macx {
    message(RtMidi will use macosx core.)
    DEFINES+=__MACOSX_CORE__
    DEFINES+=__UNIX_JACK__
    LIBS+=-framework CoreMidi -framework CoreAudio -framework CoreFoundation
    ICON = dist/MidiLayer.icns
    DEFINES += BUILD_OS=\\\"Mac\\\"
    CONFIG += app_bundle
}

win32 {
    message(RtMidi will use windows mm.)
    DEFINES+=__WINDOWS_MM__
    LIBS+=-lwinmm
    # Resources for windows (application icon)
    RC_FILE = winresources.rc
    DEFINES += BUILD_OS=\\\"Windows\\\"
}

message($$QMAKE_HOST.arch)
DEFINES += BUILD_ARCH=\\\"$$QMAKE_HOST.arch\\\"

RESOURCES += resources.qrc

#No translations yet
#TRANSLATIONS = midiLayer_pt_PT.ts

OTHER_FILES += \
    dist/readme.txt \
    dist/midiLayer.ini \
    dist/midiLayer.ip
