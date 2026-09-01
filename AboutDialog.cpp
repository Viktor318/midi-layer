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

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QString(tr("About - %1 %2.%3")).arg(APP_NAME).arg(APP_VERSION).arg(BUILD_REVISION));

    QTextCursor textCursor = ui->textEditAbout->textCursor();
    textCursor.movePosition(QTextCursor::Start);

    ui->textEditAbout->insertHtml(QString("<p align=\"center\" style=\"margin-left:5px; margin-right:5px; text-indent:0px;\"><br><span style=\" font-size:large; font-weight:600;\">")+ \
                                  APP_NAME + QString(" %1.%2</span><span style=\" font-size:small; font-weight:600;\"><br>").arg(APP_VERSION).arg(BUILD_REVISION) + \
                                  QString(tr("A MIDI tool to improve live performances"))+QString("</span><br></p>"));

    QString buildInfoHtml;
    buildInfoHtml.append("<br><br><table align=\"center\" style=\"border: solid 1px grey;\">");
    buildInfoHtml.append(this->buildLine(tr("Application Name"), APP_NAME));
    buildInfoHtml.append(this->buildLine(tr("Application Version"), APP_VERSION));
    buildInfoHtml.append(this->buildLine(tr("Application Revision"), BUILD_REVISION));
    buildInfoHtml.append(this->buildLine(tr("Application Organization"), APP_ORGANIZATION));

    buildInfoHtml.append(this->buildLine(tr("Operative System"), BUILD_OS ));
    buildInfoHtml.append(this->buildLine(tr("Arch"), BUILD_ARCH ) );

    buildInfoHtml.append(this->buildLine(tr("Qt Version"), QT_VERSION_STR));
    buildInfoHtml.append(this->buildLine(tr("RtMidi"), RTMIDI_VERSION));

    buildInfoHtml.append(this->buildLine(tr("Settings file"), APP_SETTINGS_FILE));
    buildInfoHtml.append(this->buildLine(tr("Data dir"), APP_DATA_DIR));
    buildInfoHtml.append(this->buildLine(tr("Trash dir"),QString("./%1").arg(APP_TRASH_DIR) ));
    buildInfoHtml.append(this->buildLine(tr("Bank file extension"), APP_DATA_BANK_EXTENSION));
    buildInfoHtml.append(this->buildLine(tr("Preset file extension"), APP_DATA_PRESET_EXTENSION));
    buildInfoHtml.append(this->buildLine(tr("Element file extension"), APP_DATA_ELEMENT_EXTENSION));
    buildInfoHtml.append(this->buildLine(tr("Lock file"), Utils::get()->getLockFileCompletePath()));
    // buildInfoHtml.append(this->buildLine(tr("MIDI slot #"), MIDI_SLOTS));
    buildInfoHtml.append(this->buildLine(tr("Log file"), QString("%1/%2").arg(QDir::tempPath()).arg(__LOG_FILE__)));

    buildInfoHtml.append("</table><br>");
    ui->textEditBuldInfo->append(buildInfoHtml);
    QTextCursor cursor = ui->textEditBuldInfo->textCursor();
    cursor.setPosition(QTextCursor::Start);
    ui->textEditBuldInfo->setTextCursor(cursor);

}

AboutDialog::~AboutDialog()
{
    delete ui;
}


QString AboutDialog::buildLine(QString label, QString value) {
    return QString("<tr><td align=\"right\" style=\"color: gray;\">%1: </td><td>%2</td></tr>").arg(label).arg(value);
}

QString AboutDialog::buildLine(QString label, int value) {
    return this->buildLine(label, QString("%1").arg(value));
}
