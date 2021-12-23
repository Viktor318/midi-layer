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

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    ui->autoShowHideMonitors->setVisible(false);

    connect(this, SIGNAL( accepted() ), this, SLOT( slotApplySetup() ) );
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::showEvent ( QShowEvent * /*event*/ )    {
    ui->cbCheckUpdates->setChecked( Preferences::get()->getCheckForUpdates() );
    ui->cbConfirmAtExit->setChecked( Preferences::get()->getConfirmOnExit() );
    ui->cbSaveDocks->setChecked( Preferences::get()->getSaveWindowsState() );
    ui->autoShowHideMonitors->setChecked( Preferences::get()->getAutoShowHideMonitors() );
}

void PreferencesDialog::slotApplySetup() {
    if (ISLOGGABLE_INFO) LOG_INFO("Applying setup...");
    Preferences::get()->setCheckForUpdates( ui->cbCheckUpdates->isChecked() );
    Preferences::get()->setConfirmOnExit( ui->cbConfirmAtExit->isChecked() );
    Preferences::get()->setSaveWindowsState( ui->cbSaveDocks->isChecked() );
    Preferences::get()->setAutoShowHideMonitors(ui->autoShowHideMonitors->isChecked() );
}
