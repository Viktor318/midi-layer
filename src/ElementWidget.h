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

#ifndef ELEMENTUI_H
#define ELEMENTUI_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMessageBox>
#include <QDir>
#include <QFile>
#include <QEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QStringList>

#include "PresetInterface.h"

#include "config.h"
#include "mididefs.h"
#include "Logger.h"
#include "ui_Element.h"
#include "controllers.h"
#include "NoteUtil.h"
#include "MidiManager.h"
#include "Utils.h"
#include "Led.h"

#include "ChannelMessage.h"
#include "Knob.h"
#include "Preferences.h"

namespace Ui {
class ElementUI;
}

class ElementWidget : public QWidget
{
    Q_OBJECT
    
public:

    explicit ElementWidget(QWidget *parent = 0, PresetInterface * presetInterface = 0);
    virtual ~ElementWidget();

    void initialize(QString bankName, QString presetName, QString elementName);
    QString getName();

    bool load(QString bankName, QString presetName, QString elementName);
    void setPresetName(QString presetName);
    void setBankName(QString bankName);     // Windows fork
    bool remove();     // Windows fork: false, ha nem sikerült lomtárba tenni (ilyenkor nincs törlés)
    void focusToName();

    void selectElement();
    void unselectElement();
    void sendSelectMessages();      // Windows fork: a kiválasztáskor küldött beállító üzenetek
    bool loadFrom(QString fromBankName, QString fromPresetName, QString fromElementName, QString bankName, QString presetName, QString elementName);
    bool isEdited();
    bool isActive();

    Ui::ElementUI *ui;

protected:
    // Windows fork: a névmező fókuszvesztésének figyelése (érvénytelen név visszaállítása)
    bool eventFilter(QObject * watched, QEvent * event) override;

private:

    PresetInterface * presetInterface;
    QTabWidget * tabWidget;

    bool edited;
    bool savePresetOnSave;
    bool guiConnectionsDone = false;    // Windows fork: a setupGuiConnections csak egyszer fusson le

    Led * ledIn;
    Led * ledOut;

    bool active;
    QString elementName;
    QString presetName;
    QString bankName;

    QSet<int> controllersSelected;
    std::vector<unsigned char> systemExclusiveOnSelect;
    std::vector<unsigned char> systemExclusiveOnUnselect;

    void setupGuiConnections();
    void initialSysExValidation();
    void systemExclusiveTextParser(QString systemExclusiveText, std::vector<unsigned char> * buffer);
    unsigned char hexText2uchar(QString hexText);
    unsigned char hexChar2uchar(QChar hexChar);
    void showSysExValidity(QLineEdit * field, const std::vector<unsigned char> & data);   // Windows fork
    bool validateSysExclusive(std::vector<unsigned char> systemExclusiveData);
    QString renderSystemExclusiveForSave( QString systemExclusiveOnGUI );
    QString renderControllersForSave();
    void setControllersFromString(QString controllersStr);
    QString getCloneName(QString pretendedName);

    void processNoteOn(ChannelMessage channelMessage);
    void processNoteOff(ChannelMessage channelMessage);
    void processAftertouch(ChannelMessage channelMessage);
    void processChannelAftertouch(ChannelMessage channelMessage);
    void processControlChange(ChannelMessage channelMessage);
    void processPitchBend(ChannelMessage channelMessage);
    Led * buildEventLed(QString colorStr, int width, int height);
    void buildAndAsignLeds();
    void setEdited(bool edited);
    bool renameIfNedded();
    bool isNameConflict(QString newName);     // Windows fork
    QString getDirPath();

    Led * ledNoteYes;
    Led * ledNoteNo;
    Led * ledAftertouchYes;
    Led * ledAftertouchNo;
    Led * ledPitchBendYes;
    Led * ledPitchBendNo;
    Led * ledExprAsVol;
    Led * ledVolAsVol;
    Led * ledCCAsVol;
    Led * ledCCAsVol2;
    Led * ledRangeLow;
    Led * ledRangeMatch;
    Led * ledRangeHigh;
    Led * ledVelocityLow;
    Led * ledVelocityMatch;
    Led * ledVelocityHigh;
    Led * ledCCLow;
    Led * ledCCMatch;
    Led * ledCCHigh;
    Led * ledControllerYes;
    Led * ledControllerNo;
    Led * ledTransposeOutOfBounds;

private slots:

    void slotControllerSelected(QListWidgetItem * item );
    void slotRangeDownMoved(int value);
    void slotRangeUpMoved(int value);

    void slotNoteHighMoved(int value);
    void slotNoteLowMoved(int value);

    void slotCCHighMoved(int value);
    void slotCCLowMoved(int value);

    void slotSelectAllControllers();
    void slotSelectNoneControllers();
    void slotSelectInvertControllers();

    void slotNameChanged();
    void slotNameEdited(const QString & text);     // Windows fork
    void slotMidiOutChanged();

    void slotFindController(QString findText);
    void slotViewAllControllers();
    void slotViewSelectedControllers();
    void slotSystemExclusiveOnSelect(QString systemExclusiveText);
    void slotSystemExclusiveOnUnselect(QString systemExclusiveText);

    void slotSendSEOnSelect();
    void slotSendSEOnUnselect();

    void slotVolumeChanged(int value);
    void slotBankChanged();
    void slotProgramChanged(int value);

    QString buildElementPath(QString elementName);

    void slotRefreshSlots(int,int);

    void slotOctaveDown();
    void slotOctaveUp();

    void deleyedSelectElement();

public slots:

    void slotRevert();
    void slotSave();
    void slotMarkEdited();
    void slotProcessMidiMessage(ChannelMessage channelMessage);
    void slotSetSelected();

signals:

    void nameChanged(QString, QString);

};

#endif // ELEMENTUI_H
