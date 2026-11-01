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

#ifndef PRESETWIDGET_H
#define PRESETWIDGET_H

#include <QtWidgets/QWidget>
//#include <QtWidgets/QTabWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QFile>
#include <QEvent>
#include <QKeyEvent>

#include "PresetInterface.h"

#include "Logger.h"
#include "config.h"
#include "controllers.h"
#include "NoteUtil.h"
#include "ElementWidget.h"
#include "LinkButton.h"
#include "ElementsTab.h"
#include "PresetOverview.h"

class PresetWidget : public QWidget, public PresetInterface {
    Q_OBJECT


public:

    explicit PresetWidget(QWidget *parent = 0);
    virtual ~PresetWidget();
    virtual void savePreset();
    virtual void updateSelectedTabName(QString tabName);
    virtual void updateTabIcon(QString elementName);
    virtual void setSelectedElement(QString elementName);

    void setBankName(QString bankName);
    void setName(QString presetName);
    void setPC(int programChangeValue);
    QString getName();

    bool load(QString presetPath, QString presetName);

    void setActive(bool active);
    bool isActive();

    void remove();
    bool createInitElementIfNecessary();
    QString getNotes();
    QString getCurrentElementName();
    void newElementFrom(QString elementCompleteName);

    int getMetronomeBpm(void);
    bool isMetronomeActive(void);
    int getMetronomeVolume(void);

    void popup(QString message);


protected:
    // Windows fork: a névmező fókuszvesztésének figyelése (érvénytelen név visszaállítása)
    bool eventFilter(QObject * watched, QEvent * event) override;

private:

    QWidget * parent;

    QVBoxLayout * vMainLayout;
    ElementsTab * tabElement;
    QLineEdit * nameEdit;
    QLabel * pcLabel;
    QPushButton * buttonAdd;
    QPushButton * buttonClone;
    QPushButton * buttonRemove;
    QList <ElementWidget *> elements;

    QString bankName;
    QString presetName;
    QString clipboardElementName;
    QString notes;
    bool active;
    bool metronomeActive;
    int programChangeValue;
    int metronomeBpm;
    int metronomeVolume;

    QPushButton *buttonRevert;
    QPushButton *buttonSave;

    QPushButton * buttonOverview;

    QWidget * presetLabelTag;

    QPushButton * buildActionButton(QString toolTip, QString resourceName);
    QString buildPresetPath(QString presetName);
    QString buildPresetFileName(QString presetName);
    QString getCloneName(QString pretendedName);
    QString getElementCloneName(QString pretendedName);
    QStringList getElementNames();
    void addElement(QString elementName, QString elementFile);
    int getElementIndex(QString elementName);
    void resizeTabWidget();
    void putElementInTab(ElementWidget * element);
    void setEdited(bool edited);
    bool edited;

    LinkButton * forkButton;
    LinkButton * buildForkButton();

private slots:

    void slotNameChanged();
    void slotNameEdited(const QString & text);     // Windows fork
    void slotRevert();
    void slotElementNameChanged(QString oldName, QString newName);
    void slotSetEdited();
    void slotPresetOverview();

public slots:

    void slotDeleteElement();
    void slotAddElement();
    void slotSelected();
    void slotUnselected();
    void slotDuplicateElement();
    void slotMetronomeActive(bool active, bool markEdited);
    void slotMetronomeBpm(int bpm, bool markEdited);
    void slotMetronomeVolume(int volume, bool markEdited);
    void slotMetronomeSettings(bool active, int bpm, int volume, bool markEdited);
    void slotSave();

signals:

    void nameChanged(QString oldName, QString newName);
    void reverted();

};

#endif // PRESETWIDGET_H
