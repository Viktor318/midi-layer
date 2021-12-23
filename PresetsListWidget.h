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

#ifndef PRESETSLISTWIDGET_H
#define PRESETSLISTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QCheckBox>
#include <QDir>
#include <QFile>
#include <QValidator>

#include "Logger.h"
#include "config.h"
#include "PresetWidget.h"
#include "Utils.h"


class PresetsListWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PresetsListWidget(QWidget *parent = 0, QWidget * centralWidget = 0);
    ~PresetsListWidget();
    void addToList( QString presetName, QString toolTip);
    void insertToList( int row, QString presetName, QString toolTip);
    void setPreserveOrder(bool preserveOrder);
    void setSelectedByName(QString selectedPreset);

    bool loadBank(QString bankName);
    bool newBank(QString bankName);
    void saveBankOnly();
    void saveActiveBank();
    void deletePreset(QString presetName, bool showConfirm);
    void unloadBank();
    void unloadPreset(QString presetName);
    QString getCurrentPresetPath();
    void sort(); // Not used
    int getPresetListFontSize();

private:

    QWidget * parentWidget;
    QWidget * centralWidget;
    QDir * presetsDir;

    PresetWidget * currentPreset;
    QVBoxLayout * dockLayout;
    QHBoxLayout * toolsLayout;
    QListWidget * presetList;
    QString lastPresetSelected;
    QString clipboardPresetName;
    QString clipboardElementName;
    QString infoMessage;

    QString bankName;
    QLineEdit * bankNameEdit;
    QPushButton * buttonLock;
    QPushButton * buttonAdd;
    QPushButton * buttonClone;
    QPushButton * buttonRemove;

    QSlider * sizeSlider;
    QFont listFont;

    QWidget * bankLabelTag;

    QPushButton * buildActionButton(QString toolTip, QString resourceName);
    QPushButton * buildCheckableButton(QString toolTip, QString resourceName, QString resourceClickedName);
    int getPresetListRow(QString presetName);
    QString buildBankPath(QString bankName);
    QString getCloneName(QString pretendedName);
    void changeBankNameEditProgramaticly(QString newName);
    void newPresetFrom(QString presetCompleteName);

    void unselectPreset();
    void selectPreset(QString presetName);
    void changeToSelectedPreset();
    QStringList getPresetsNamesList();
    void sendInfoToMetronome();
    void updateTooltip();

signals:

    void bankNameChanged(QString oldName, QString newName);
    void metronomeChanged(bool active, int bmp ,int volume, QString presetName);

public slots:

    void slotPresetNameChanged(QString oldName, QString newName);
    void slotChangePreset(int newRow);
    void slotMetronomeActive(bool active);
    void slotMetronomeBpm(int bpm);
    void slotMetronomeVolume(int volume);
    void slotMetronomeChanged(bool active, int bpm, int volume);
    void slotListFontSize(int);

private slots:

    void slotChangePresetByProgramChange(int programChangeValue);
    void slotPreservePresetOrder(bool preserve);
    void slotBankNameChanged();
    void slotDuplicatePreset();
    void slotDuplicateElement();
    void slotCopyPresetToClipboard();
    void slotCopyElementToClipboard();
    void slotPastePresetFromClipboard();
    void slotPasteElementFromClipboard();
    void slotAddPreset();
    void slotAddElement();
    void slotDeletePreset();
    void slotDeleteElement();

};

#endif // PRESETSLISTWIDGET_H
