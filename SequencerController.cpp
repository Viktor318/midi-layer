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

#include "SequencerController.h"
#include "ui_SequencerController.h"

SequencerController::SequencerController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SequencerController)
{
    ui->setupUi(this);
    song = 0;
    isInPause = false;
    isPlaying = false;

    connect(ui->buttonPlay, SIGNAL(pressed()), this, SLOT(slotStart()));
    connect(ui->buttonStop, SIGNAL(pressed()), this, SLOT(slotStop()));
    connect(ui->buttonPrev, SIGNAL(pressed()), this, SLOT(slotPrevious()));
    connect(ui->buttonNext, SIGNAL(pressed()), this, SLOT(slotNext()));
}

SequencerController::~SequencerController() {
    delete ui;
}

void SequencerController::slotStart()   {

    if (!isPlaying)   {
        if (!isInPause)  {
            MidiManager::get()->sendSeqStart();
        }
        else    {
            MidiManager::get()->sendSeqContinue();
        }
        isPlaying = true;
    } else  {
        MidiManager::get()->sendSeqStop();
        isInPause = true;
        isPlaying = false;
    }

}

void SequencerController::slotStop()   {
    MidiManager::get()->sendSeqStop();
    isInPause = false;
    isPlaying = false;
    ui->buttonPlay->setChecked(false);
}

void SequencerController::slotPrevious()   {
    if (--song<0) {
        song = 127;
    }
    MidiManager::get()->sendSeqSelectSong(song);
    isInPause = false;
    isPlaying = false;
}

void SequencerController::slotNext()   {
    if (++song>127) {
        song = 0;
    }
    MidiManager::get()->sendSeqSelectSong(song);
    isInPause = false;
    isPlaying = false;
}
