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

#ifndef LINKBUTTON_H
#define LINKBUTTON_H

#include <QDesktopServices>
#include <QUrl>
#include <QPushButton>

class LinkButton : public QPushButton
{
    Q_OBJECT
public:
    explicit LinkButton(QWidget *parent = 0);

    void setUrl(QString url);

private:
    QString url;

signals:

public slots:

private slots:

    void slotOpenUrl();

};

#endif // LINKBUTTON_H
