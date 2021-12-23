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

#ifndef DIALOGLOADBANK_H
#define DIALOGLOADBANK_H

#include <QtWidgets/QDialog>
#include <QStringList>
#include <QDir>
#include <QFile>

#include "config.h"
#include "Logger.h"
#include "Utils.h"
#include "ui_LoadBankDialog.h"

namespace Ui {
    class LoadBankDialog;
}

class LoadBankDialog : public QDialog, private Ui::LoadBankDialog
{
    Q_OBJECT
    
public:
    explicit LoadBankDialog(QWidget *parent = 0);
    ~LoadBankDialog();
    
    QString loadBank(QString dir);

private:
    Ui::LoadBankDialog *ui;
    void populate(QString baseDir);
};

#endif // DIALOGLOADBANK_H
