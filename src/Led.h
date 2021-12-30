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

#ifndef LED_H
#define LED_H

#include <QtWidgets/QWidget>
#include <QPalette>
#include <QColor>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <QFontMetrics>
#include <QList>

#include "Utils.h"

class Led : public QWidget
{
    Q_OBJECT

public:
    Led(QWidget *parent = 0, int w=18, int h=10);
    virtual ~Led();
    void setColor(QColor color);
    void setColorOff(QColor color);
    void buildBrushes();
    void setText(QString text);
    void setTextColor(QColor color);
    void setTextFontSize(int fontSize);
    void setFlashDuration(int interval);
    void setTextOffset(int textOffsetX, int textOffsetY);
    void setGradient(bool state);

public slots:
    void flash();

private slots:
    void turnOff();

protected:
    void paintEvent(QPaintEvent *);

private:
    QTimer * timer;

    bool enabled;
    bool gradient;
    int w, h;
    QColor color;
    QColor borderColor;
    QBrush onBrush;
    QBrush offBrush;
    QString text;
    int textX, textY;
    QColor textColor;
    QFont textFont;
    int fontSize;
    int textOffsetX;
    int textOffsetY;
    QRect labelBoundingRect;

    QLinearGradient buidGradient(QColor color);
    int min(int a, int b);

    static int lastTextLenght;
    static int lastTextSize;
    static int lastWidth;
    static int lastHeight;
    static int lastTextX;
    static int lastTextY;

};

#endif // LED_H
