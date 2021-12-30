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


#include "VertLabel.h"


VertLabel::VertLabel(QString txt, QWidget *parent): QLabel(parent)
{
    text = txt;
    QFont font("sans",24);
    QFontMetrics fm(font);

    int pixelsWide = fm.horizontalAdvance(txt);
    int pixelsHigh = fm.height();
    this->resize(pixelsHigh,pixelsWide);

}


void VertLabel::paintEvent(QPaintEvent *)
{

    QFont font("sans",24);
    QPainter painter(this);
    painter.setFont(font);
    painter.setPen(Qt::gray);
    painter.rotate(90);
    painter.drawText(QPoint(0,-10), text);

}


void VertLabel::changeText(QString txt)
{
    text = txt;
    update();
}
