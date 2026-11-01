/*
    MidiLayer - A simple and intuitive tool to improve live performances
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

#ifndef MIDIMONITOR_H
#define MIDIMONITOR_H

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QDialog>
#include <QtWidgets/QScrollBar>
#include <QAction>

#include "RtMidi/RtMidi.h"
#include "RtMidi/RtError.h"

#include "MidiManager.h"

#include "ui_MidiMonitor.h"

#include "mididefs.h"
#include "controllers.h"
#include "NoteUtil.h"
#include "ChannelMessage.h"


#ifdef QT_DEBUG
#include <QAction>
#endif

namespace Ui {
    class MidiMonitor;
}

class MidiMonitor : public QDialog, private Ui::MidiMonitor
{
    Q_OBJECT

public:
    MidiMonitor(QWidget *parent = 0);
    int exec();

signals:
    
public slots:

    void slotReceivedChannelMessage (ChannelMessage channelMessage);
    void slotSentChannelMessage (ChannelMessage channelMessage);

private:
    Ui::MidiMonitor *ui;
    QAction *actionClose;
    void showEvent(QShowEvent * event);
    void hideEvent(QHideEvent * event);     // Windows fork: closeEvent helyett (lásd a .cpp-ben)
    void print(QString text, QColor * color, bool wasReceived);
    void printRaw(ChannelMessage * channelMessage, QColor * color, bool wasReceived);
    QString getNoteName(unsigned char noteCode);

    void parseChannelMessage (ChannelMessage * channelMessage, bool wasReceived);

    QString noteOnStr;
    QColor noteOnColor;
    QString noteOffStr;
    QColor noteOffColor;
    QString polyAftertouchStr;
    QColor polyAftertouchColor;
    QString controlChangeStr;
    QColor controlChangeColor;
    QString programChangeStr;
    QColor programChangeColor;
    QString channelAftertouchStr;
    QColor channelAftertouchColor;
    QString pitchbendStr;
    QColor pitchbendColor;
    QString otherEventStr;
    QColor otherEventColor;
    QAction * actionDelete;
    QAction * actionLeft;
    QAction * actionRight;

#ifdef QT_DEBUG
    QAction * action1;
    QAction * action2;
    QAction * action3;
    QAction * action4;
    int debugProgramChange;
#endif


private slots:

    void checkAllEvents();
    void slotDisplayMode(int);
    void slotKeyLeft();
    void slotKeyRight();

#ifdef QT_DEBUG
    void slotAction_1();
    void slotAction_2();
    void slotAction_3();
    void slotAction_4();
#endif


};

#endif // MIDIMONITOR_H
