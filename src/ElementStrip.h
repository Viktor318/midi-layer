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

#ifndef ELEMENTSTRIP_H
#define ELEMENTSTRIP_H

#include <QWidget>
#include <QMenu>
#include <QMouseEvent>

#include "Utils.h"
#include "config.h"
#include "ElementWidget.h"
#include "MidiManager.h"
#include "ChannelMessage.h"

namespace Ui {
class ElementStrip;
}

class ElementStrip : public QWidget
{
    Q_OBJECT

public:

    explicit ElementStrip(QWidget *parent = 0, ElementWidget * element = 0);
    ~ElementStrip();
    void mouseReleaseEvent ( QMouseEvent * event );

    void markBankConflict(bool conflicted);
    void markProgramConflict(bool conflicted);
    void markVolumeConflict(bool conflicted);
    void markResetConflict(bool conflicted);
    void markSysexConflict(bool conflicted);

private:

    Ui::ElementStrip *ui;
    ElementWidget * element;

    Led * ledIn;
    Led * ledOut;

    void loadValuesFromElement();
    void buidToolTips();
    void buildConnections();
    void buildAndAsignLeds();
    int referenceVolume;
    int referenceTranspose;
    int lastVolPercent;

private slots:

    void slotSlotIn(int value);
    void slotSlotOut(int value);

    void slotMidiIn(int value);
    void slotMidiOut(int value);

    void slotBankMsb(int value);
    void slotBankLsb(int value);

    void slotProgramChange(int value);
    void slotTranspose(int value);
    void slotVolumeChanged(int value);
    void slotEnabled();

    void slotMarkEdited();
    void setEdited(bool edited);

    void slotEnableBankChange();
    void slotEnableProgramChange();
    void slotEnableVolumeChange();

    void slotReset(bool enabled);
    void slotSysex(bool enabled);

    void slotInFlashes(ChannelMessage channelMessage);
    void slotOutFlashes(int slot, unsigned char channel);

    void slotShowContextMenu(const QPoint &pos);

public slots:

    void slotVolumePercentChanged(int value);
    void slotTranposeRelativeChanged(int relativeValue);

    void slotEnabled(bool enabled);
    void slotEnabledInvert();

    void slotRevert();
    void slotRevertForMaster();
    void slotSave();

signals:

    void analizeConflicts();

};

#endif // ELEMENTSTRIP_H
