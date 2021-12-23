/*
    Midi Layer
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

#ifndef MSETUP_H
#define MSETUP_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>

#include <QList>
#include <typeinfo>

#include <QStringList>
#include <QString>

#include "RtMidi/RtMidi.h"
#include "RtMidi/RtError.h"

#include "config.h"
#include "Logger.h"
#include "MidiManager.h"
#include "Preferences.h"

namespace Ui {
class MidiSetup;
}

class MidiSetup : public QDialog
{
    Q_OBJECT
    
public:
    explicit MidiSetup(QWidget *parent = 0);
    ~MidiSetup();
    
private:
    Ui::MidiSetup *ui;

    QList<QComboBox*> midiInCombos;
    QList<QComboBox*> midiOutCombos;

    void selectInComboByText(QComboBox * comboBox, QString text);
    virtual void showEvent ( QShowEvent * event );
    void refreshCombos(bool fromMidiSettings);

private slots:
    void slotRefresh();
    void slotApplySetup();
};

#endif // MSETUP_H
