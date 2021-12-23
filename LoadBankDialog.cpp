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

#include "LoadBankDialog.h"

LoadBankDialog::LoadBankDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadBankDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonCancel, SIGNAL(released()), this, SLOT(reject()));
}

LoadBankDialog::~LoadBankDialog()   {
    delete ui;
}

QString LoadBankDialog::loadBank(QString dir)    {
    ui->pushButtonAction->setText(tr("load"));
    connect(ui->pushButtonAction, SIGNAL(released()), this, SLOT(accept()));
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    this->populate(dir);
    if (this->exec()==QDialog::Accepted && ui->listWidget->currentRow() > -1)
        return ui->listWidget->currentItem()->text();
    else
        return 0;
}

void LoadBankDialog::populate(QString baseDir) {
    // ui->listWidget->addItems( Utils::get()->getBanksOnDir(baseDir) );
    ui->listWidget->addItems( Utils::get()->searchBanksOnDir(baseDir) );
}
