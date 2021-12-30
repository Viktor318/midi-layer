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

#ifndef SEQUENCERCONTROLLER_H
#define SEQUENCERCONTROLLER_H

#include <QtWidgets/QWidget>

#include "MidiManager.h"

namespace Ui {
class SequencerController;
}

class SequencerController : public QWidget
{
    Q_OBJECT
    
public:
    explicit SequencerController(QWidget *parent = 0);
    ~SequencerController();
    
private:
    Ui::SequencerController *ui;
    int song;
    bool isInPause;
    bool isPlaying;

private slots:
    void slotStart();
    void slotStop();
    void slotPrevious();
    void slotNext();
};

#endif // SEQUENCERCONTROLLER_H
