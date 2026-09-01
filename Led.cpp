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

#include "Led.h"

int Led::lastTextLenght = 0;
int Led::lastTextSize = 0;
int Led::lastTextX = 0;
int Led::lastTextY = 0;
int Led::lastWidth = 0;
int Led::lastHeight = 0;

/**
 * @brief The Led class Simple rectangle Led widget
 */
Led::Led(QWidget *parent, int width, int height) :
    QWidget(parent)
{
    this->w = width;
    this->h = height;
    this->setMinimumSize(width, height);

    this->enabled = false;
    this->gradient = true;

    this->timer = new QTimer();
    this->timer->setSingleShot(true);
    this->timer->setInterval(100);

    // Default colors
    this->setColor(QColor("#FF0000"));
    this->textColor="#FFFFFF";

    this->text="";
    this->textFont = font();

    // Default font
    this->textFont.setFamily("Sans Serif");
    this->textOffsetX = 0;
    this->textOffsetY = -2;
    this->textX = 0;
    this->textY = 0;
    this->fontSize = 10;
    this->textFont.setPixelSize(this->fontSize);
    this->setFont(this->textFont);
    this->connect(this->timer, SIGNAL(timeout()), this, SLOT(turnOff()));

}

Led::~Led() {
    delete timer;
}

/**
 * @brief Led::setColor Sets the status ON color LED
 * @param color State ON LED color
 */
void Led::setColor(QColor color)    {
    this->color = color;
    this->borderColor = this->color.darker(600);
    this->buildBrushes();
    this->repaint();
}

void Led::buildBrushes()    {
    if (this->gradient) {
        this->onBrush = QBrush(buidGradient( this->color ));
        this->offBrush = QBrush(buidGradient(this->color.darker(300)));
    } else  {
        this->onBrush = QBrush(this->color);
        this->offBrush = QBrush(this->offBrush);
    }
}

/**
 * @brief Led::setText Sets the LED text
 * @param text LED text
 */
void Led::setText(QString text)  {
    this->text = text;
    QFontMetrics fontMetrics(this->textFont);
    this->labelBoundingRect = fontMetrics.boundingRect(text);
    this->update();
}

/**
 * @brief Led::setTextColor Sets the LED text color
 * @param color text color
 */
void Led::setTextColor(QColor color)    {
    this->textColor=color;
}

/**
 * @brief Led::setTextFontSize Sets LED text font size
 * @param fontSize size in pixels
 */
void Led::setTextFontSize(int fontSize)    {
    this->fontSize=fontSize;
    this->textFont.setPixelSize(fontSize);
    QFontMetrics fontMetrics(textFont);
    this->labelBoundingRect = fontMetrics.boundingRect(text);
}

/**
 * @brief Led::setTextOffset Offset coords to adjust text position
 * @param textOffsetX x axis offset
 * @param textOffsetY y axis offset
 */
void Led::setTextOffset(int textOffsetX, int textOffsetY) {
    this->textOffsetX = textOffsetX;
    this->textOffsetY = textOffsetY;
}

/**
 * @brief Led::flash flash LED (see setFlashInterval)
 */
void Led::flash() {
    this->timer->start();
    this->enabled = true;
    this->update();
}

/**
 * @brief Led::setFlashInterval Sets the time of LED flash.
 * @param interval time of flash in milliseconds
 */
void Led::setFlashDuration(int interval) {
    this->timer->setInterval(interval);
}

/**
 * @brief Led::setGradient LED will filled with a gradient aspect (more real)
 * @param useGradient defines if LED is filled with gradient
 */
void Led::setGradient(bool useGradient) {
    this->gradient = useGradient;
    this->buildBrushes();
}

/**
 * @brief Led::buidGradient builds a gradient using color base.
 * @param color color base for gradient
 * @return the bilded gradient
 */
QLinearGradient Led::buidGradient(QColor color)  {
    QLinearGradient gradient(0, 0, 0, this->height());
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, color.darker(150));
    return gradient;
}

/**
 * @brief Led::turnOff Sets LED to off state
 */
void Led::turnOff() {
    this->enabled = false;
    this->update();
}

/**
* @brief Led::setColorOff Sets the status OFF color LED. If not defined will
*  be automaticly calculated.
* @param color Off state LED color
*/
void Led::setColorOff(QColor color)    {
   if (this->gradient)
       this->offBrush = QBrush(buidGradient(color));
   else
       this->offBrush = QBrush(color);
   this->buildBrushes();
   this->repaint();
}

//void Led::setMaxSize(int width, int height)    {
//    this->width = width;
//    this->height = height;
//}

void Led::paintEvent(QPaintEvent * /* event */) {
    QPainter painter(this);

    this->buildBrushes();

    if (!text.isEmpty() ) {

        int size;

        if (    Led::lastTextLenght != this->text.length() ||
                Led::lastHeight != this->height() ||
                (this->width() > Led::lastWidth+2 || this->width() < Led::lastWidth-2 )
                ) {

            QFontMetrics fontMetrics(this->textFont);
            this->labelBoundingRect = fontMetrics.boundingRect(text);

            int height;
            height = (int) this->height()*0.8f;

            int width;
            width = (int) this->width()*0.6f;

            size = this->min(height, (int) (2.2*width/this->text.length()) );

            if (size<6) {
                size = 6;
            }

            this->textX = (int) ((float)(this->width() -this->labelBoundingRect.width())  * 0.45f);
            this->textY = (int) ((float)(this->height()+this->labelBoundingRect.height()) * 0.46f);

            Led::lastHeight = this->height();
            Led::lastWidth = this->width();
            Led::lastTextLenght = this->text.length();
            Led::lastTextSize = size;
            Led::lastTextX = this->textX;
            Led::lastTextY = this->textY;

        } else  {
            size = Led::lastTextSize;
            this->textX = Led::lastTextX;
            this->textY = Led::lastTextY;
        }

        this->textFont.setPixelSize(size);
        this->setFont(this->textFont);
    }

    this->enabled ? painter.setBrush(this->onBrush) : painter.setBrush(this->offBrush);

    painter.setPen(this->borderColor);
    painter.drawRect(0, 0, this->width()-1, this->height()-1);

    if (!text.isEmpty()) {
        painter.setPen(this->textColor);
        painter.drawText( this->textX, this->textY, this->text);
    }
}

int Led::min(int a, int b)  {
    if (a<b)
        return a;
    return b;
}
