/*
    MidiLayer
    Copyright (C) 2012-2015, António José Soares Maia <maia.ajs@gmail.com>

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


#include "LinkButton.h"


LinkButton::LinkButton(QWidget *parent) :
    QPushButton(parent)
{
    this->setFlat(true);
    connect(this, SIGNAL(clicked()), this, SLOT(slotOpenUrl()));
}


void LinkButton::setUrl(QString url) {
    this->url = url;
}


void LinkButton::slotOpenUrl() {
    QDesktopServices::openUrl(QUrl(this->url));
}
