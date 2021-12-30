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


#include "MasterStrip.h"
#include "ui_MasterStrip.h"


MasterStrip::MasterStrip(QWidget *parent, QList<ElementStrip *> * strips) :
    QWidget(parent),
    ui(new Ui::MasterStrip)
{
    ui->setupUi(this);

    VertLabel * vertLabel = new VertLabel(APP_NAME, this);
    ui->labelLayout->addWidget(vertLabel);

    this->strips = strips;
    this->stripsCount = this->strips->count();

    QFont fontElementStrip;
    Utils::get()->configElementStripFont(&fontElementStrip);
    ui->labelTranspose->setFont(fontElementStrip);

    this->buildConnections();
    this->volumeReference = ui->vsVolume->value();

    QFont fontLabelStrip;
    Utils::get()->configElementStripBigFont(&fontLabelStrip);
    ui->label->setFont(fontLabelStrip);

    QString volumeSliderStyle = ".QSlider::groove { background: transparent; background-repeat:no-repeat; background-image: url(:/images/slider_v_140.png); height: 140px; width: 28px; }"
            " .QSlider::handle:vertical {background-repeat:no-repeat; height: 35px; margin: 0 0 0 6px; background-image: url(:/images/slider_cap_17x35.png); }"
            " .QSlider::handle:vertical:disabled {background-repeat:no-repeat; background-image: url(:/images/slider_cap_17x35_disabled.png);}";

    ui->vsVolume->setStyleSheet(volumeSliderStyle);

    // ToolTips
    ui->sbTranspose->setToolTip(QString("Relative Transpose"));
    ui->vsVolume->setToolTip(QString("Relative Volume"));
}


MasterStrip::~MasterStrip()
{
    delete ui;
}


void MasterStrip::buildConnections() {

    connect(ui->vsVolume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumePercentChanged(int)));
    for (int i=0; i<stripsCount; i++)    {
        connect(ui->sbTranspose, SIGNAL(valueChanged(int)), this->strips->at(i), SLOT(slotTranposeRelativeChanged(int)));
        connect(ui->buttonEnable, SIGNAL(clicked(bool)), this->strips->at(i), SLOT(slotEnabled(bool)));
    }

    connect(ui->buttonEnableInvert, SIGNAL(clicked()), this, SLOT(slotEnabledInvert()));
    connect(ui->buttonRevert, SIGNAL(clicked()), this, SLOT(slotRevert()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(slotSave()));
}


void MasterStrip::slotVolumePercentChanged(int value)   {
    int percent;
    if (this->volumeReference>0)    {
        percent = 100*value/this->volumeReference;
    } else  {
        percent = value;
    }
    for (int i=0; i<this->stripsCount; i++)    {
        this->strips->at(i)->slotVolumePercentChanged(percent);
    }
}


void MasterStrip::slotRevert()   {

    QProgressDialog * progressDialog = new QProgressDialog("Restore elements", NULL, 0, this->stripsCount, this);
    if (this->stripsCount>1)    {
        progressDialog->show();
        progressDialog->setWindowModality(Qt::WindowModal);
    }

    for (int i=0; i<this->stripsCount; i++)    {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        this->strips->at(i)->slotRevertForMaster();
        progressDialog->setValue(i+1);
    }
    emit analizeConflicts();
    progressDialog->close();
    ui->vsVolume->setValue(this->volumeReference);
}


void MasterStrip::slotSave()   {

    QProgressDialog * progressDialog = new QProgressDialog("Saving elements", NULL, 0, this->stripsCount, this);
    if (this->stripsCount>1)    {
        progressDialog->show();
        progressDialog->setWindowModality(Qt::WindowModal);
    }

    for (int i=0; i<this->stripsCount; i++)    {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        this->strips->at(i)->slotSave();
        progressDialog->setValue(i+1);
    }
    this->volumeReference = ui->vsVolume->value();
}


void MasterStrip::slotEnabledInvert()   {
    for (int i=0; i<this->stripsCount; i++)    {
        this->strips->at(i)->slotEnabledInvert();
    }
    emit analizeConflicts();
}

