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

#ifndef MIDILAYER_H
#define MIDILAYER_H

#include <QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMessageBox>
#include <QtGui/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMessageBox>
#include <QSettings>
#include <QFont>
#include <QVariant>
#include <QList>
#include <QCloseEvent>
#include <QShowEvent>
#include <QString>
#include <QCloseEvent>

#include "RtMidi/RtMidi.h"
#include "RtMidi/RtError.h"

#include "UpdateNotifier.h"
#include "AboutDialog.h"
#include "Logger.h"
#include "mididefs.h"
#include "ChannelMessage.h"
#include "MidiManager.h"
#include "MidiChannelMonitor.h"
#include "MidiSetup.h"
#include "MidiMonitor.h"
#include "ui_MainWindow.h"
#include "ElementWidget.h"
#include "PresetWidget.h"
#include "PresetsListWidget.h"
#include "MidiManager.h"
#include "LoadBankDialog.h"
#include "Metronome.h"
#include "SequencerController.h"
#include "Preferences.h"
#include "PreferencesDialog.h"
#include "PanicPanel.h"
#include "PresetOverview.h"

namespace Ui {
    class MainWindow;
}

class MainWindow: public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication *qapp);
    virtual ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void setTitleWithBankName(QString bankName);

signals:

    void event_noteoff(int channel, int note);
    void event_noteon(int channel, int note);
    void event_polykeypress(int channel, int note, int value);
    void event_controlchange(int channel, int control, int value);
    void event_programchange(int channel, int value);
    void event_chankeypress(int channel, int value);
    void event_pitchwheel(int channel, int value);

private:

    Ui::MainWindow *ui;

    MidiSetup *midiSetup;
    MidiMonitor *midiMonitor;
    PreferencesDialog *preferences;

    QList<MidiChannelMonitor *> midiInMonitors;
    QList<MidiChannelMonitor *> midiOutMonitors;
    QWidget * centralWidget;
    QList<int> currentMidiInIndex;
    QList<int> currentMidiOutIndex;
    QList<bool> isMidiInActive;

    void openPort();
    void closePort();

    QApplication *application;
    QList<QDockWidget *> midiInMonitorDock;
    QList<QDockWidget *> midiOutMonitorDock;

    QDockWidget * metronomeDock;
    Metronome * metronome;

    QDockWidget * sequencerControllerDock;
    SequencerController * sequencerController;

    QDockWidget * panicDock;
    PanicPanel * panic;

    void setDefaultWidgetSettings();
    void showSplashScreen();
    void buildMidiInMonitorDocks();
    void buildMidiOutMonitorDocks();
    void buildWindowsMenu();
    void buildWindowsDockActionSync(QDockWidget * dock, QString shortCut);
    void buildConnections();

    void saveSettings();
    void loadSettings();
    void saveActiveBank();
    void openBank(QString bankName);

    QDockWidget * presetDock;
    PresetsListWidget * presetsWidget;
    QString activeBankName;
    int actionsBeforeMonitorsCount;

    QList<ElementWidget*> pool;

#ifdef QT_DEBUG
    int debugCount;
#endif


private slots:

    void slotUpdateMonitorsNames();
    void slotOpenMidiSetup();
    void slotOpenPreferences();
    void exitApplication();
    void about(void);
    void aboutQt(void);

    void slotOpenMonitor(void);
    void slotInFlashes(ChannelMessage channelMessage);
    void slotOutFlashes(int slotIndex, unsigned char midiChannel);
    void slotOpenBank(void);
    void slotNewBank(void);
    void slotBankNameChanged(QString /*oldName*/, QString newName);
    void slotCloneBank(void);

public slots:
    #ifdef QT_DEBUG
    void slotAction_1();
    void slotAction_2();
    void slotAction_3();
    #endif
};

#endif // MIDILAYER_H

