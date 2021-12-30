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

#ifndef ELEMENTSTAB_H
#define ELEMENTSTAB_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTabBar>

#include "Logger.h"
#include "ElementWidget.h"

class ElementsTab : public QTabWidget
{
    Q_OBJECT

public:
    explicit ElementsTab(QWidget *parent = 0);
    ElementWidget * element(int index);

private:

signals:
    void tabMoved(int from, int to);

public slots:
    void slotTabMoved(int from, int to);
};

#endif // ELEMENTSTAB_H
