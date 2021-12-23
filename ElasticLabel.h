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

#ifndef ELASTICLABEL_H
#define ELASTICLABEL_H

#include <QtWidgets/QWidget>
#include <QPalette>
#include <QColor>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <QFontMetrics>
#include <QList>

#include "Utils.h"

class ElasticLabel : public QWidget
{
    Q_OBJECT

public:
    ElasticLabel(QWidget *parent = 0, int w=18, int h=10);
    virtual ~ElasticLabel();
    void setText(QString text);
    void setTextColor(QColor color);
    void setTextFontSize(int fontSize);
    void setTextOffset(int textOffsetX, int textOffsetY);

protected:
    void paintEvent(QPaintEvent *);

private:

    int w, h;
    QString text;
    int textX, textY;
    QColor textColor;
    QFont textFont;
    int fontSize;
    int textOffsetX;
    int textOffsetY;
    QRect labelBoundingRect;

    int min(int a, int b);

};

#endif // ELASTICLABEL_H
