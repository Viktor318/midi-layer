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

#include "ElasticLabel.h"

/**
 * @brief The ElasticLabel class Simple rectangle Led widget
 */
ElasticLabel::ElasticLabel(QWidget *parent, int width, int height) :
    QWidget(parent)
{
    this->w = width;
    this->h = height;
    this->setMinimumSize(width, height);

    // Default colors
    this->textColor="#CCCCCC";

    this->text="";
    this->textFont = font();

    // Default font
    this->textFont.setFamily("Ubuntu");
    this->textOffsetX = 0;
    this->textOffsetY = -2;
    this->textX = 0;
    this->textY = 0;
    this->fontSize = 10;
    this->textFont.setPixelSize(this->fontSize);
    this->setFont(this->textFont);

}

ElasticLabel::~ElasticLabel() {
}

/**
 * @brief ElasticLabel::setText Sets the text
 * @param text ElasticLabel text
 */
void ElasticLabel::setText(QString text)  {
    this->text = text;
    QFontMetrics fontMetrics(this->textFont);
    this->labelBoundingRect = fontMetrics.boundingRect(text);
    this->repaint();
}

/**
 * @brief ElasticLabel::setTextColor Sets the text color
 * @param color text color
 */
void ElasticLabel::setTextColor(QColor color)    {
    this->textColor=color;
    this->repaint();
}

/**
 * @brief ElasticLabel::setTextFontSize Sets text font size
 * @param fontSize size in pixels
 */
void ElasticLabel::setTextFontSize(int fontSize)    {
    this->fontSize=fontSize;
    this->textFont.setPixelSize(fontSize);
    QFontMetrics fontMetrics(textFont);
    this->labelBoundingRect = fontMetrics.boundingRect(text);
}

/**
 * @brief ElasticLabel::setTextOffset Offset coords to adjust text position
 * @param textOffsetX x axis offset
 * @param textOffsetY y axis offset
 */
void ElasticLabel::setTextOffset(int textOffsetX, int textOffsetY) {
    this->textOffsetX = textOffsetX;
    this->textOffsetY = textOffsetY;
}

/**
 * @brief ElasticLabel::paintEvent Render widget
 */
void ElasticLabel::paintEvent(QPaintEvent * /* event */) {
    QPainter painter(this);

    if (!text.isEmpty() ) {

        QFontMetrics fontMetrics(this->textFont);
        this->labelBoundingRect = fontMetrics.boundingRect(text);

        int size;
        size = this->min(this->height(), 2*this->width()/this->text.length());
        if (size<8) {
            size = 8;
        }

        this->textFont.setPixelSize(size);

        this->textX = (int) ((float)(this->width() -this->labelBoundingRect.width())  * 0.45f);
        this->textY = (int) ((float)(this->height()+this->labelBoundingRect.height()) * 0.4f);

        this->setFont(this->textFont);
    }

    if (!text.isEmpty()) {
        painter.setBrush(this->textColor);
        painter.drawText( this->textX, this->textY, this->text);
    }
}

int ElasticLabel::min(int a, int b)  {
    if (a<b)
        return a;
    return b;
}
