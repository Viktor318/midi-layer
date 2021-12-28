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

#include "MainWindow.h"


MainWindow::MainWindow(QApplication */*parent*/)
{
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("Starting main window...");

    setupUi(this);

    #ifndef QT_DEBUG
    menubar->removeAction(menuDebug->menuAction());
    #endif

    setTitleWithBankName(0);

    this->setSizePolicy(QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ));

    centralWidget = new QWidget(this);
    centralWidget->setSizePolicy(QSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed ));
    centralWidget->setMinimumSize(975,720);
    this->setCentralWidget(centralWidget);

    midiSetup = new MidiSetup(this);
    midiSetup->setSizeGripEnabled(false);

    preferences = new PreferencesDialog(this);
    midiMonitor = new MidiMonitor(this);

    // Presets Dock
    presetsWidget = new PresetsListWidget(this, centralWidget);
    presetDock = new QDockWidget(tr("Bank"), this);
    presetDock->setObjectName("presetDock");
    presetDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    presetDock->setWidget(presetsWidget);
    presetDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    presetDock->setMinimumSize(300,300);
    this->addDockWidget(Qt::LeftDockWidgetArea, presetDock);

    sequencerController = new SequencerController(this);
    sequencerControllerDock = new QDockWidget(tr("Sequencer Transport"), this);
    sequencerControllerDock->setObjectName("sequencerControllerDock");
    sequencerControllerDock->setWidget(sequencerController);
    sequencerControllerDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    this->addDockWidget(Qt::TopDockWidgetArea, sequencerControllerDock);

    metronomeDock = new QDockWidget(tr("Metronome"), this);
    metronomeDock->setObjectName("metronomeDock");
    metronomeDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    this->addDockWidget(Qt::TopDockWidgetArea, metronomeDock);

    panic = new PanicPanel(this);
    panicDock = new QDockWidget(tr("Panic"), this);
    panicDock->setObjectName("panicDock");
    panicDock->setWidget(panic);
    panicDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    this->addDockWidget(Qt::LeftDockWidgetArea, panicDock);

    this->buildMidiInMonitorDocks();
    this->buildMidiOutMonitorDocks();
    this->buildWindowsMenu();

    metronome = new Metronome(this);
    metronomeDock->setWidget(metronome);

    this->loadSettings();
    this->buildConnections();

    presetsWidget->loadBank(activeBankName);
    this->setTitleWithBankName(activeBankName);

#ifdef QT_DEBUG
    debugCount = 0;
#endif

    this->slotUpdateMonitorsNames();

    Utils::get()->setMainWindowReady(true);

    UpdateNotifier * updateNotifier = new UpdateNotifier(this);
    updateNotifier->start();

}


MainWindow::~MainWindow()      {

    if (ISLOGGABLE_INFO) LOG_INFO("Saving active bank...");
    this->saveActiveBank();
    if (ISLOGGABLE_INFO) LOG_INFO("Saving settings...");
    this->saveSettings();

    delete metronome;

    if (ISLOGGABLE_INFO) LOG_INFO("End application.");
}


void MainWindow::buildMidiInMonitorDocks(void)  {
    for (int m=0; m<MIDI_SLOTS; m++)    {
        midiInMonitors.append(new MidiChannelMonitor(this, MidiChannelMonitor::In));;

        QDockWidget * monitorActivityIn = new QDockWidget(this);
        monitorActivityIn->setObjectName(QString("monitorActivityIn%1").arg(m+1));
        monitorActivityIn->setWidget(midiInMonitors.at(m));
        monitorActivityIn->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
        this->addDockWidget(Qt::TopDockWidgetArea, monitorActivityIn);
        midiInMonitorDock.append(monitorActivityIn);
    }
}


void MainWindow::buildMidiOutMonitorDocks(void)  {
    for (int m=0; m<MIDI_SLOTS; m++)    {
        midiOutMonitors.append(new MidiChannelMonitor(this, MidiChannelMonitor::Out));

        QDockWidget * monitorActivityOut = new QDockWidget(this);
        monitorActivityOut->setObjectName(QString("monitorActivityOut%1").arg(m+1));
        monitorActivityOut->setWidget(midiOutMonitors.at(m));
        monitorActivityOut->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
        this->addDockWidget(Qt::TopDockWidgetArea, monitorActivityOut);
        midiOutMonitorDock.append(monitorActivityOut);
    }
}


void MainWindow::buildWindowsMenu(void)  {

    this->buildWindowsDockActionSync(presetDock,"");
    this->buildWindowsDockActionSync(metronomeDock,"");
    this->buildWindowsDockActionSync(sequencerControllerDock,"");
    this->buildWindowsDockActionSync(panicDock,"");

    menuWindows->addSeparator ();

    actionsBeforeMonitorsCount = menuWindows->actions().count();
    for (int s=0; s<MIDI_SLOTS; s++)   {
        this->buildWindowsDockActionSync(midiInMonitorDock.at(s), QString("Ctrl+%1").arg(s+1));
    }

    for (int s=0; s<MIDI_SLOTS; s++)   {
        this->buildWindowsDockActionSync(midiOutMonitorDock.at(s),QString("Shift+%1").arg(s+1));
    }

}


void MainWindow::buildWindowsDockActionSync(QDockWidget * dock, QString shortCut)  {
    QAction * action = new QAction(this);
    action->setCheckable(true);

    if (!shortCut.isEmpty())    {
        action->setShortcut(QKeySequence(shortCut));
    }

    action->setText(dock->windowTitle());
    connect(action, SIGNAL(triggered(bool)), dock, SLOT(setVisible(bool)));
    connect(dock, SIGNAL(visibilityChanged(bool)), action, SLOT(setChecked(bool)));
    action->setChecked(dock->isVisible());
    menuWindows->addAction(action);
}


void MainWindow::buildConnections(void)  {

    connect(actionPreferences, SIGNAL(triggered()), this, SLOT( slotOpenPreferences()) );
    connect(actionSetup,   SIGNAL(triggered()), this, SLOT( slotOpenMidiSetup()) );
    connect(actionExit,    SIGNAL(triggered()), this, SLOT( exitApplication()) );
    connect(actionMonitor, SIGNAL(triggered()), this, SLOT( slotOpenMonitor()) );
    connect(actionAbout,   SIGNAL(triggered()), this, SLOT( about()) );
    connect(actionAboutQt, SIGNAL(triggered()), this, SLOT( aboutQt()) );

    connect(actionPanic, SIGNAL(triggered()), MidiManager::get(), SLOT( slotPanic() ) );
    connect(midiSetup, SIGNAL(accepted()), this, SLOT(slotUpdateMonitorsNames()));
    connect(MidiManager::get(), SIGNAL(flashLedMonitor(int,unsigned char)), this, SLOT(slotOutFlashes(int,unsigned char)) );
    connect(MidiManager::get(), SIGNAL(receivedChannelMessage(ChannelMessage)), this, SLOT(slotInFlashes(ChannelMessage)) );

    connect(actionOpenBank, SIGNAL(triggered()), this, SLOT(slotOpenBank()));
    connect(actionNewBank, SIGNAL(triggered()), this, SLOT(slotNewBank()));
    connect(actionCloneBank, SIGNAL(triggered()), this, SLOT(slotCloneBank()));
    connect(presetsWidget, SIGNAL(bankNameChanged(QString,QString)), this, SLOT(slotBankNameChanged(QString,QString)) );

    connect(actionAddPreset, SIGNAL(triggered()), presetsWidget, SLOT(slotAddPreset()));
    connect(actionDuplicatePreset, SIGNAL(triggered()), presetsWidget, SLOT(slotDuplicatePreset()));
    connect(actionCopyPreset, SIGNAL(triggered()), presetsWidget, SLOT(slotCopyPresetToClipboard()));
    connect(actionPastePreset, SIGNAL(triggered()), presetsWidget, SLOT(slotPastePresetFromClipboard()));
    connect(actionRemovePreset, SIGNAL(triggered()), presetsWidget, SLOT(slotDeletePreset()));

    connect(actionAddElement, SIGNAL(triggered()), presetsWidget, SLOT(slotAddElement()));
    connect(actionDuplicateElement, SIGNAL(triggered()), presetsWidget, SLOT(slotDuplicateElement()));
    connect(actionCopyElement, SIGNAL(triggered()), presetsWidget, SLOT(slotCopyElementToClipboard()));
    connect(actionPasteElement, SIGNAL(triggered()), presetsWidget, SLOT(slotPasteElementFromClipboard()));
    connect(actionRemoveElement, SIGNAL(triggered()), presetsWidget, SLOT(slotDeleteElement()));

    connect(metronome, SIGNAL( changed(bool,int,int) ), presetsWidget, SLOT( slotMetronomeChanged(bool,int,int) ));
    connect(presetsWidget, SIGNAL(metronomeChanged(bool,int,int,QString)), metronome, SLOT(slotSetMetronome(bool,int,int,QString)));

    #ifdef QT_DEBUG
    connect(actionAction_1, SIGNAL(triggered()),this,SLOT(slotAction_1()));
    connect(actionAction_2, SIGNAL(triggered()),this,SLOT(slotAction_2()));
    connect(actionAction_3, SIGNAL(triggered()),this,SLOT(slotAction_3()));
    #endif
}


void MainWindow::slotInFlashes(ChannelMessage channelMessage)  {
    midiInMonitors.at(channelMessage.getSlotNumber())->flash(channelMessage.getChannel());
}


void MainWindow::slotOutFlashes(int slotIndex, unsigned char midiChannel) {
    midiOutMonitors.at(slotIndex)->flash(midiChannel);
}


void MainWindow::slotOpenMonitor(void)   {
    midiMonitor->exec();
}


void MainWindow::saveSettings(void) {

    QString preferencesFile = Utils::get()->getPreferencesDir() + "/" + APP_SETTINGS_FILE;
    if (ISLOGGABLE_DEBUG) LOG_DEBUG( "saving preferences to '"+preferencesFile+"'" );

    QSettings settings( preferencesFile, QSettings::IniFormat,this);
    settings.beginGroup("Application");
    settings.setValue("lastBank", activeBankName);
    settings.endGroup();

    settings.beginGroup("Preferences");
    settings.setValue("confirmOnExit", Preferences::get()->getConfirmOnExit() );
    settings.setValue("saveDocks", Preferences::get()->getSaveWindowsState() );
    settings.setValue("autoShowHideMonitors", Preferences::get()->getAutoShowHideMonitors() );
    settings.setValue("checkForUpdates", Preferences::get()->getCheckForUpdates() );
    settings.setValue("presetListFontSize", presetsWidget->getPresetListFontSize() );
    settings.endGroup();

    settings.beginGroup("Monitor");
    settings.setValue("geometry", midiMonitor->geometry());
    settings.endGroup();

    settings.beginGroup("Midi");
    for (int m=0; m<MIDI_SLOTS; m++)    {
        settings.setValue(QString("inPort%1").arg(m+1), MidiManager::get()->getInPortNameAtSlot(m) );
        settings.setValue(QString("outPort%1").arg(m+1), MidiManager::get()->getOutPortNameAtSlot(m) );
    }
    settings.setValue("programChangeSlot", MidiManager::get()->getProgramChangeSlot()+1);
    settings.setValue("programChangeChannel", MidiManager::get()->getProgramChangeChannel()+1);
    settings.setValue("sequencerControllerSlot", MidiManager::get()->getSequencerControllerSlot()+1);
    settings.endGroup();

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", this->geometry());
    if (Preferences::get()->getSaveWindowsState())  {
        if (ISLOGGABLE_TRACE) LOG_TRACE("Saving windows state");
        settings.setValue("windowState", saveState());
    }
    settings.endGroup();

    settings.beginGroup("Metronome");
    settings.setValue("sendMetronomeUdp", Preferences::get()->getSendMetronomeUdp());
    settings.setValue("metronomeUdpHostName", Preferences::get()->getMetronomeUdpHostName());
    // settings.setValue("metronomeUdpNetwork", Preferences::get()->getMetronomeUdpNetwork());
    settings.endGroup();

    if (ISLOGGABLE_TRACE) LOG_TRACE("saving preferences END");
}


void MainWindow::loadSettings(void) {

    QString preferencesFile = Utils::get()->getPreferencesDir() + "/" + APP_SETTINGS_FILE;
    if (ISLOGGABLE_DEBUG) LOG_DEBUG( "loading preferences from '"+preferencesFile+"'" );

    QFile file( preferencesFile );
    if (!file.exists() )	{
        this->setDefaultWidgetSettings();

    } else {
        QSettings settings(preferencesFile,QSettings::IniFormat,this);
        settings.beginGroup("Application");
        activeBankName = settings.value("lastBank", "").toString();
        settings.endGroup();

        settings.beginGroup("Monitor");
        midiMonitor->setGeometry(settings.value("geometry", QRect(100,100, 488, 464)).toRect());
        settings.endGroup();

        settings.beginGroup("Midi");
        for (int m=0; m<MIDI_SLOTS; m++)    {
            MidiManager::get()->setMidiInSelected(m,  settings.value(QString("inPort%1").arg(m+1)).toString());
            MidiManager::get()->setMidiOutSelected(m, settings.value(QString("outPort%1").arg(m+1)).toString());
        }
        MidiManager::get()->setProgramChangeSlot(settings.value("programChangeSlot", 1).toInt()-1);
        MidiManager::get()->setProgramChangeChannel(settings.value("programChangeChannel", 1).toInt()-1);
        MidiManager::get()->setSequencerControllerSlot(settings.value("sequencerControllerSlot", 1).toInt()-1);
        settings.endGroup();

        settings.beginGroup("MainWindow");
        this->setGeometry(settings.value("geometry", QRect((1320-qApp->screens().at(0)->geometry().width())/2, (847-qApp->screens().at(0)->geometry().height())/2, 1320, 847)).toRect());
        if (settings.allKeys().contains("windowState")) {
            this->restoreState(settings.value("windowState", QByteArray("\0\0\0\xff\0\0\0\0\xfd\0\0\0\x3\0\0\0\0\0\0\x1\x4\0\0\x1\xf0\xfc\x2\0\0\0\x5\xfb\0\0\0\x14\0p\0r\0\x65\0s\0\x65\0t\0\x44\0o\0\x63\0k\x1\0\0\0\\\0\0\x1\xf0\0\0\0\x96\0\xff\xff\xff\xfb\0\0\0.\0s\0\x65\0q\0u\0\x65\0n\0\x63\0\x65\0r\0\x43\0o\0n\0t\0r\0o\0l\0l\0\x65\0r\0\x44\0o\0\x63\0k\0\0\0\x2\x19\0\0\0\x33\0\0\0\x33\0\0\0\x33\xfb\0\0\0&\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0O\0u\0t\0\x32\x2\0\0\x3\f\0\0\0\0\0\0\x1\xf4\0\0\0\xa5\xfb\0\0\0&\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0O\0u\0t\0\x34\x2\0\0\x1\0\0\0\0\xbe\0\0\x4\0\0\0\0\xf0\xfb\0\0\0\x12\0p\0\x61\0n\0i\0\x63\0\x44\0o\0\x63\0k\0\0\0\x1\xda\0\0\0<\0\0\0\0\0\0\0\0\0\0\0\x2\0\0\x4\"\0\0\0\x42\xfc\x1\0\0\0\x6\xfb\0\0\0$\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0I\0n\0\x31\x1\0\0\0\0\0\0\x1v\0\0\x1\"\0\xff\xff\xff\xfb\0\0\0&\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0O\0u\0t\0\x31\x1\0\0\x1|\0\0\x1\x9c\0\0\x1\"\0\xff\xff\xff\xfb\0\0\0\x1a\0m\0\x65\0t\0r\0o\0n\0o\0m\0\x65\0\x44\0o\0\x63\0k\x1\0\0\x3\x1e\0\0\x1\x4\0\0\x1\x4\0\xff\xff\xff\xfb\0\0\0$\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0I\0n\0\x33\x2\0\0\0O\0\0\x2Q\0\0\x1$\0\0\0\x44\xfb\0\0\0$\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0I\0n\0\x34\x2\0\0\x3\xe2\0\0\0\xcb\0\0\x1\x1e\0\0\0?\xfb\0\0\0&\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0O\0u\0t\0\x33\x2\0\0\x3\xa2\0\0\x1\x10\0\0\x1^\0\0\0?\0\0\0\x3\0\0\x4\"\0\0\0\x42\xfc\x1\0\0\0\x1\xfb\0\0\0$\0m\0o\0n\0i\0t\0o\0r\0\x41\0\x63\0t\0i\0v\0i\0t\0y\0I\0n\0\x32\0\0\0\0\0\0\0\x2=\0\0\x1\"\0\xff\xff\xff\0\0\x3\x18\0\0\x1\xf0\0\0\0\x4\0\0\0\x4\0\0\0\b\0\0\0\b\xfc\0\0\0\0")).toByteArray());
        }
        settings.endGroup();

        settings.beginGroup("Preferences");
        Preferences::get()->setCheckForUpdates(settings.value("checkForUpdates",true).toBool());
        Preferences::get()->setConfirmOnExit(settings.value("confirmOnExit",true).toBool());
        Preferences::get()->setSaveWindowsState(settings.value("saveDocks",true).toBool());
        Preferences::get()->setAutoShowHideMonitors(settings.value("autoShowHideMonitors",true).toBool());
        presetsWidget->slotListFontSize( settings.value("presetListFontSize", 16).toInt() );
        settings.endGroup();

        settings.beginGroup("Metronome");
        Preferences::get()->setSendMetronomeUdp(settings.value("sendMetronomeUdp", false).toBool());
        Preferences::get()->setMetronomeUdpHostName(settings.value("metronomeUdpHostName", QString::fromLatin1(APP_NAME)).toString());
        Preferences::get()->setMetronomeUdpNetwork(settings.value("metronomeUdpNetwork", "").toString());
        settings.endGroup();
    }

    if (ISLOGGABLE_TRACE) LOG_TRACE( "loading preferences END" );
}


void MainWindow::saveActiveBank()   {
    presetsWidget->saveActiveBank();
}


void MainWindow::slotOpenPreferences()  {
    preferences->exec();
}


void MainWindow::slotOpenMidiSetup()    {
    midiSetup->exec();
}


void MainWindow::slotUpdateMonitorsNames()   {

    for (int m=0; m<MIDI_SLOTS; m++)    {
        QString label = QString(tr("IN %1 - %2")).arg(m+1).arg(MidiManager::get()->getInPortNameAtSlot(m));
        this->midiInMonitorDock.at(m)->setWindowTitle(label);
        if (Preferences::get()->getAutoShowHideMonitors())  {
            this->midiInMonitorDock.at(m)->setVisible( !(MidiManager::get()->getInPortNameAtSlot(m).isEmpty() || QString::compare(MidiManager::get()->getInPortNameAtSlot(m), "[not defined]", Qt::CaseSensitive)) == 0 );
        }
        // Refresh Menu "Windows" monitors Names
        menuWindows->actions().at(actionsBeforeMonitorsCount+m)->setText(this->midiInMonitorDock.at(m)->windowTitle());
    }

    for (int m=0; m<MIDI_SLOTS; m++)    {
        QString label = QString(tr("OUT %1 - %2")).arg(m+1).arg(MidiManager::get()->getOutPortNameAtSlot(m));
        this->midiOutMonitorDock.at(m)->setWindowTitle(label);
        if (Preferences::get()->getAutoShowHideMonitors())  {
            this->midiOutMonitorDock.at(m)->setVisible(!(MidiManager::get()->getOutPortNameAtSlot(m).isEmpty() || QString::compare(MidiManager::get()->getOutPortNameAtSlot(m), "[not defined]", Qt::CaseSensitive) == 0 ));
        }
        // Refresh Menu "Windows" monitors Names
        menuWindows->actions().at(actionsBeforeMonitorsCount+MIDI_SLOTS+m)->setText(this->midiOutMonitorDock.at(m)->windowTitle());
    }
}


void MainWindow::slotOpenBank(void)
{
    LoadBankDialog * openBank = new LoadBankDialog();
    openBank->setWindowTitle(tr("Open Bank"));
    QString bankName = openBank->loadBank(Utils::get()->getDataDir());
    this->openBank(bankName);
    delete openBank;
}


void MainWindow::openBank(QString bankName)
{
    if ( bankName.isNull() || bankName.isEmpty() )    {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Invalid name for bank open");
        return;
    }
    this->saveActiveBank();
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("loading bank '"+bankName+"'");
    presetsWidget->slotChangePreset(-1);
    presetsWidget->unloadBank();
    setTitleWithBankName(bankName);
    presetsWidget->loadBank(bankName);
    presetDock->setToolTip(bankName);
    this->activeBankName = bankName;
}


void MainWindow::slotNewBank(void)
{
    this->saveActiveBank();
    QString bankName = Utils::get()->getCloneName("new bank", Utils::get()->getBanksOnDir( Utils::get()->getDataDir() ) );
    if (ISLOGGABLE_DEBUG) LOG_DEBUG("new bank '"+bankName+"'");
    presetsWidget->slotChangePreset(-1);
    presetsWidget->unloadBank();
    setTitleWithBankName(bankName);
    presetsWidget->newBank(bankName);
    presetDock->setToolTip(bankName);
    this->activeBankName = bankName;
}


void MainWindow::slotCloneBank(void)
{
    this->saveActiveBank();
    QString bankName = Utils::get()->getCloneName(this->activeBankName, Utils::get()->getBanksOnDir( Utils::get()->getDataDir() ) );

    if (ISLOGGABLE_DEBUG) LOG_DEBUG("clone bank '"+bankName+"'");

    if ( bankName.isNull() )    {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Could not create clone '" + bankName +"'"+ ". Invalid name.");
    }

    // Clone Bank
    QString fromBankDir = Utils::get()->getDataDir() + "/" + this->activeBankName;
    QString toBankDir = Utils::get()->getDataDir() + "/" + bankName;

    bool copySuccess = Utils::get()->copyDir(fromBankDir, toBankDir, true);

    if (!copySuccess)   {
        /// TODO: Warning user
        if (ISLOGGABLE_ERROR) LOG_ERROR("Could not create clone '" + toBankDir +"'");
        return;
    }

    bool renameResult = Utils::get()->renameFile( Utils::get()->getDataDir() + "/" + bankName + "/" + this->activeBankName + APP_DATA_BANK_EXTENSION ,
                                       Utils::get()->getDataDir() + "/" + bankName + "/" + bankName + APP_DATA_BANK_EXTENSION );

    if (!renameResult)   {
        /// TODO: Warning user
        if (ISLOGGABLE_ERROR) LOG_ERROR("Could not create clone '" + toBankDir +"'");
        /// TODO remove dir
        return;
    }
    this->openBank(bankName);
}


void MainWindow::about(void)
{
    AboutDialog * about = new AboutDialog(this);
    about->exec();
}


void MainWindow::aboutQt(void)
{
    QMessageBox::aboutQt ( this, tr("About Qt") );
}


void MainWindow::exitApplication(void)  {
    this->close();
}


void MainWindow::closeEvent(QCloseEvent *event) {

    if (Preferences::get()->getConfirmOnExit()) {
        int confirm = QMessageBox::question(this, tr("Exit program"),
                                        tr("\nAre you sure do you want exit %1?").arg( QApplication::applicationName() ),
                                        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

        if ( confirm == QMessageBox::Cancel )	{
            event->ignore();
            return;
        }
    }

    Utils::get()->deleteLockFile();
    event->accept();

}


void MainWindow::slotBankNameChanged(QString /*oldBankName*/, QString newBankName)  {
    this->activeBankName = newBankName;
    this->setTitleWithBankName(newBankName);
}


void MainWindow::setTitleWithBankName(QString bankName) {

    QString windowTitle = "";

    if (!bankName.isNull() && !bankName.isEmpty())
        windowTitle += bankName + " - ";

    windowTitle+=QApplication::applicationName();

    if (Utils::get()->isPortableMode()) {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Running in portable mode.");
    }
    else    {
        windowTitle += " (non portable)";
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Running in normal mode (non-portable).");
    }
    setWindowTitle(windowTitle);
}


#ifdef QT_DEBUG
void MainWindow::slotAction_1() {

    ChannelMessage  channelMessage1 = ChannelMessage( 0, STATUS_NOTEON, 0, 20 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);

    ChannelMessage  channelMessage2 = ChannelMessage( 0, STATUS_NOTEON, 2, 62 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage2);

    ChannelMessage  channelMessage3 = ChannelMessage( 0, STATUS_NOTEON, 4, 102 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage3);

//    PresetOverview *presetOverview = new PresetOverview(this);
//    presetOverview->exec();

    MidiManager::get()->toString();
}


void MainWindow::slotAction_2() {

    ChannelMessage  channelMessage1 = ChannelMessage( 1, STATUS_NOTEON, 1, 20 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);

    ChannelMessage  channelMessage2 = ChannelMessage( 1, STATUS_NOTEON, 3, 62 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage2);

    ChannelMessage  channelMessage3 = ChannelMessage( 1, STATUS_NOTEON, 5, 102 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage3);

    MidiManager::get()->toString();

}


void MainWindow::slotAction_3() {

    ChannelMessage  channelMessage1 = ChannelMessage( 0, STATUS_PRGCHG, 0, debugCount++%5 & MASK_SAFETY, 100 & MASK_SAFETY);
    MidiManager::get()->relayMessage(channelMessage1);
}
#endif


void MainWindow::setDefaultWidgetSettings()  {

    // Set widgets default config (no config file)
    this->setGeometry(0, 0, 1320, 847);
    this->move(qApp->screens().at(0)->geometry().center()-this->geometry().center());

    presetDock->setVisible(true);
    metronomeDock->setVisible(false);
    panicDock->setVisible(false);
    sequencerControllerDock->setVisible(false);
    for (int s=0; s<MIDI_SLOTS; s++)   {
        midiInMonitorDock.at(s)->setVisible(false);
    }

    for (int s=0; s<MIDI_SLOTS; s++)   {
        midiOutMonitorDock.at(s)->setVisible(false);
    }

    Preferences::get()->setCheckForUpdates(true);
    Preferences::get()->setConfirmOnExit(true);
    Preferences::get()->setSaveWindowsState(true);
    Preferences::get()->setAutoShowHideMonitors(false);
    presetsWidget->slotListFontSize(16);

    Preferences::get()->setSendMetronomeUdp(false);
    Preferences::get()->setMetronomeUdpHostName(QString::fromLatin1(APP_NAME));
    Preferences::get()->setMetronomeUdpNetwork("");
}
