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

#include "NoteUtil.h"

// QAtomicPointer<NoteUtil> NoteUtil::m_NoteUtil;
NoteUtil * NoteUtil::m_NoteUtil;
QMutex NoteUtil::mutex;

NoteUtil* NoteUtil::get() {

    if( ! m_NoteUtil )
    {
        mutex.lock();
        if( ! m_NoteUtil )
            m_NoteUtil = new NoteUtil;
        mutex.unlock();
    }
    return m_NoteUtil;
}

void NoteUtil::Drop()  {
    mutex.lock();
    delete m_NoteUtil;
    m_NoteUtil = 0;
    mutex.unlock();
}

QStringList NoteUtil::noteNameMap() {
    QStringList list;
    list << "C " << "C#" << "D " << "Eb" << "E " << "F " << "F#" << "G " << "G#" << "A " << "Bb" << "B ";
    return list;
}

QString NoteUtil::getNoteName(int noteCode)    {
    return noteNameMap().at( noteCode%12 ) + QString::number( (int) ((int)noteCode)/12 -2  );
}
