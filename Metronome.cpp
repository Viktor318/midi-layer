#include "Metronome.h"
#include "ui_Metronome.h"

Metronome::Metronome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Metronome)
{
    ui->setupUi(this);
    this->isRunning=false;

    #ifdef QT_PHONON_LIB
    // Search for personalized click sound
    QString userClickSound = Utils::get()->getPreferencesDir() + "/" + "metronome.wav";
    QFile userSoundFile( userClickSound );
    clickMedia = new Phonon::MediaObject(this);
    // Decide what sound will be used
    if ( userSoundFile.exists() )	{
        clickFile = userClickSound;
    } else  {
        clickFile = ":/sounds/metronomeClick.wav";
    }
    clickAudioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    phononPath = Phonon::createPath(clickMedia, clickAudioOutput);

    clickAudioOutput->setVolume((qreal)1);

    // Loads sound to cache. Prepare for play imediatly.
    clickMedia->setCurrentSource(Phonon::MediaSource(clickFile));
    clickMedia->pause();
    clickMedia->clear();
    #endif

    // Big LED
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->setSpacing(0);
    ui->frame->setLayout(horizontalLayout);
    led = new Led(this, 1200, 138);
    led->setFlashDuration(75);
    led->setGradient(false);
    led->setColor(QColor(50,255,50,255));
    led->setColorOff(QColor(0,0,0,255));
    horizontalLayout->addWidget(led,0,Qt::AlignCenter);

    QFont butFont;
    butFont.setFamily("Ubuntu Condensed");
    butFont.setPixelSize(14);

    // Foreground
    ui->buttonOnOff->setToolTip(tr("On/Off <b>SPACE</b>"));
    ui->buttonOnOff->setText(tr("OFF"));
    ui->buttonOnOff->setFont(butFont);
    ui->buttonOnOff->setStyleSheet("color: rgb(0,0,0);");

    ui->buttonTapTempo->setToolTip(tr("Tap tempo <b>T</b>"));
    ui->buttonTapTempo->setText(tr("TAP"));
    ui->buttonTapTempo->setStyleSheet("color: rgb(0,0,0);");
    ui->buttonTapTempo->setFont(butFont);

    ui->dialTempo->setToolTip(tr("Tempo [<b>+</b>/<b>-</b>]"));

    // UDP metronome
    this->udpSocketSend = new QUdpSocket(this);
    // udpSocketGet  = new QUdpSocket(this);

    QString broadcastAddress = this->obtainBroadcastIp();
    this->bcast = new QHostAddress(broadcastAddress);

    this->localHostName = this->obtainHostName();

    this->udpSocketSend->connectToHost(*bcast,65432);
    // udpSocketGet->bind(*host, udpSocketSend->localPort());
    // connect(udpSocketGet,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));


#ifdef QT_PHONON_LIB
    ui->dialVolume->setToolTip(tr("Volume"));
    connect(ui->dialVolume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
#else
    ui->dialVolume->setVisible(false);
#endif

    this->timer = new QTimer();
    this->timer->setTimerType(Qt::PreciseTimer);

    connect(ui->buttonOnOff, SIGNAL(pressed()),this, SLOT(slotStartStopPressed()));
    connect(ui->buttonTapTempo, SIGNAL(pressed()),this, SLOT(slotTapTempo()));
    connect(ui->dialTempo, SIGNAL(valueChanged(int)), this, SLOT(setBpm(int)));

    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));

    this->tapTempoTimer = new QTimer();
    this->tapTempoTimer->setInterval(2000);
    this->tapTempoTimer->setSingleShot(true);
    connect(tapTempoTimer, SIGNAL(timeout()), this, SLOT(slotTapTempoTimeOut()));

    tapTempoAction = new QAction(tr("&Tap Tempo"),this);
    tapTempoAction->setShortcut(QString("T"));
    this->addAction(tapTempoAction);
    connect(tapTempoAction, SIGNAL(triggered()), this, SLOT(slotTapTempo()));

    startStopAction = new QAction(tr("&Metronome On/Off"), this);
    startStopAction->setShortcut(QString("Space"));
    startStopAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    this->addAction(startStopAction);
    connect(startStopAction, SIGNAL(triggered()), this, SLOT(slotStartStop()));

    incrementAction = new QAction(this);
    incrementAction->setShortcut(QString("+"));
    this->addAction(incrementAction);
    connect(incrementAction, SIGNAL(triggered()), this, SLOT(slotIncrement()));

    decrementAction = new QAction(this);
    decrementAction->setShortcut(QString("-"));
    this->addAction(decrementAction);
    connect(decrementAction, SIGNAL(triggered()), this, SLOT(slotDecrement()));

    this->nextBeatTime=120;
    this->slotSetBpm(120);

    udpOrdinal = 0;
    // this->timer->start();
}

Metronome::~Metronome() {

    if (Preferences::get()->getSendMetronomeUdp())  {
        QByteArray datagram;
        datagram.append(QString("|%1|%2|%3|%4|")
                        .arg( QDateTime::currentMSecsSinceEpoch() )
                        .arg(ui->dialTempo->sliderPosition())
                        .arg("0")
                        .arg(this->udpOrdinal++).toUtf8());
        udpSocketSend->write(datagram);
        udpSocketSend->write(datagram);
    }

#ifdef QT_PHONON_LIB
    clickMedia->clear();
    phononPath.disconnect();
    clickMedia->deleteLater();
    clickAudioOutput->deleteLater();
    // phonon objects
    delete clickMedia;
    delete clickAudioOutput;
#endif

    delete led;
    // timers
    delete timer;
    delete tapTempoTimer;
    // Actions
    delete tapTempoAction;
    delete startStopAction;
    delete incrementAction;
    delete decrementAction;
    delete horizontalLayout;
    delete ui;
}

void Metronome::slotStartStopPressed() {
    setActive(!this->isRunning);
}

void Metronome::slotSetMetronome(bool isActive, int bpm ,int /*volume*/, QString currentPresetName)   {

    this->slotSetBpm(bpm);
    this->currentPresetName = currentPresetName;

    // Force resync on change preset (send UDP)
    if (this->isRunning && isActive)    {
        this->slotSetActive(false);
    }
    this->slotSetActive(isActive);
#ifdef QT_PHONON_LIB
    this->slotSetVolume(volume);
#endif

}

void Metronome::emitMetronomeInfo() {
    int volume = -1; // not setted
#ifdef QT_PHONON_LIB
    volume = ui->dialVolume->value();
#endif
    emit changed(this->isRunning, this->getBpm(), volume);
}

void Metronome::setActive(bool active) {
//    emit activated(active);
    this->refreshActive(active);
    LOG_TRACE("Will emit metronome info...");
    this->emitMetronomeInfo();
}

void Metronome::slotSetActive(bool active) {
    this->refreshActive(active);
    ui->buttonOnOff->setDown(active);
}

void Metronome::refreshActive(bool active)  {
    //if (active != this->timer->isActive())  {
        if (active) {
            this->timer->start();
            this->led->flash();
            LOG_TRACE("Send metronome UDP info");
            this->sendUDP();
#ifdef QT_PHONON_LIB
            this->playClickSound();
#endif
            ui->buttonOnOff->setText(tr("ON"));
            ui->buttonOnOff->setStyleSheet("color: rgb(64,224, 64);");            
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Metronome set to ON"));
        }
        else    {
            // this->timer->stop();
            this->sendUDP();
            ui->buttonOnOff->setText(tr("OFF"));
            ui->buttonOnOff->setStyleSheet("color: rgb(0,0, 0);");            
            if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Metronome set to OFF"));
        }
        this->isRunning = active;
    //}
}

bool Metronome::isActive() {
    return this->isRunning;
}

void Metronome::setBpm(int bpm) {
//    emit bpmChanged(bpm);
    this->emitMetronomeInfo();
    refreshBpm(bpm);
}

void Metronome::slotSetBpm(int bpm) {
    if (ISLOGGABLE_TRACE) LOG_TRACE("####");
    ui->dialTempo->setTracking(false);
    ui->dialTempo->setSliderPosition(bpm);
    ui->dialTempo->setTracking(true);
    // Use dial for limit minimum and maximum value
    this->refreshBpm(ui->dialTempo->sliderPosition());
}

void Metronome::refreshBpm(int bpm)    {
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("ENTER Metronome BMP refresh to %1").arg(bpm));
    ui->lcdNumber->display(bpm);
    this->nextBeatTime = round( 60000/bpm );
    this->setToolTip(QString(tr("Metronome<br>beat = %1ms")).arg(this->nextBeatTime));
    if (!this->tapTempoTimer->isActive())
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Metronome BMP refresh to %1").arg(bpm));
}

int Metronome::getBpm()  {
    return ui->dialTempo->value();
}

void Metronome::slotTimeOut()   {
    this->timer->setInterval(this->nextBeatTime);
    if (this->isRunning)     {
        this->led->flash();
#ifdef QT_PHONON_LIB
        this->playClickSound();
#endif
    }
    this->sendUDP();
}

void Metronome::sendUDP() {
    #ifdef SEND_UDP_METRONOME
    if (Preferences::get()->getSendMetronomeUdp())  {
        //      AppId|milliseconds|bmp|on/off|ordinal|presetName
        QByteArray datagram;
        datagram.append(QString("%1|%2|%3|%4|%5|%6")
                        .arg( this->localHostName )
                        .arg( QDateTime::currentMSecsSinceEpoch() )
                        .arg( ui->dialTempo->sliderPosition() )
                        .arg( this->isRunning?"1":"0" )
                        .arg( this->udpOrdinal++ )
                        .arg( this->currentPresetName).toUtf8());
        udpSocketSend->write(datagram);
        udpSocketSend->write(datagram);
//        LOG_TRACE(QString("Metronome datagram: %1").arg(datagram.toStdString()));
    }
    #endif
}

void Metronome::slotStartStop() {
    bool newState = !this->isRunning;
    this->setActive(newState);
    ui->buttonOnOff->setDown(newState);
}

void Metronome::slotTapTempoTimeOut()   {
    ui->buttonTapTempo->setStyleSheet("color: rgb(0, 0, 0);");
    if (ISLOGGABLE_INFO) LOG_INFO(QString("Metronome BMP set to %1").arg(ui->dialTempo->sliderPosition()));
}

void Metronome::slotTapTempo()  {

    if (!tapTempoTimer->isActive())  {
        tapTempoRecords.clear();
        tapTempoTimer->start();
        // tapTempoStopwatch.start();
        ui->buttonTapTempo->setStyleSheet("color: rgb(255, 64, 64);");
    }
    else    {
        if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("### slotTapTempo TAP ENabled"));
        tapTempoRecords.append(tapTempoStopwatch.currentTime());
        tapTempoTimer->start();
        this->led->flash();
    }

    int samples = tapTempoRecords.count();
    if (samples>1)  {
        int sum = 0;
        for(int i=1; i<samples; i++)    {
            sum += tapTempoRecords.at(i).msecsSinceStartOfDay()-tapTempoRecords.at(i-1).msecsSinceStartOfDay();
        }
        if (sum>0)  {
            ui->dialTempo->setSliderPosition((int) 60000*samples/sum);
        }
    }
}

void Metronome::slotIncrement()    {
    ui->dialTempo->setValue(this->getBpm()+1);
}

void Metronome::slotDecrement()    {
    ui->dialTempo->setValue(this->getBpm()-1);
}

#ifdef QT_PHONON_LIB
void Metronome::setVolume(int volumePercent)  {
//    emit volumeChanged(volumePercent);
    this->emitMetronomeInfo();
    this->refreshVolume(volumePercent);
}

void Metronome::slotSetVolume(int volumePercent) {
    ui->dialVolume->setTracking(false);
    ui->dialVolume->setSliderPosition(volumePercent);
    ui->dialVolume->setTracking(true);
    // Use dial for limit minimum and maximum value
    this->refreshVolume(ui->dialVolume->sliderPosition());
}

void Metronome::refreshVolume(int volumePercent)    {
    ui->dialVolume->setToolTip(QString(tr("Volume %1%")).arg(volumePercent));
    this->clickAudioOutput->setVolume((qreal) volumePercent/100);
}

void Metronome::playClickSound()   {

    if (ui->dialVolume->sliderPosition()==0)
        return;

    switch (clickMedia->state()) {
    case Phonon::PlayingState:
        clickMedia->pause();
        clickMedia->seek(0);
        clickMedia->play();
        break;
    case Phonon::PausedState:
        clickMedia->seek(0);
        clickMedia->play();
        break;
    case Phonon::StoppedState:
        clickMedia->setCurrentSource(Phonon::MediaSource(clickFile));
        clickMedia->play();
        break;
    case Phonon::ErrorState:
        if (ISLOGGABLE_ERROR) LOG_ERROR(clickMedia->errorString());
        clickMedia->stop();
        clickMedia->setCurrentSource(Phonon::MediaSource(clickFile));
        clickMedia->play();
        break;
    case Phonon::LoadingState: //Quiet GCC
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Metronome sound is loading... (could not play it yet)");
        break;
    case Phonon::BufferingState:
        if (ISLOGGABLE_DEBUG) LOG_DEBUG("Metronome sound is buffering... (could not play it yet)");
        break;
    }
}

void Metronome::slotPhononStateLog()  {

    QString state = "Undefined";
    switch (clickMedia->state()) {
    case Phonon::PlayingState:
        state = "PlayingState";
        break;
    case Phonon::PausedState:
        state = "PausedState";
        break;
    case Phonon::StoppedState:
        state = "StoppedState";
        break;
    case Phonon::ErrorState:
        state = "ErrorState";
        if (ISLOGGABLE_ERROR) LOG_ERROR(clickMedia->errorString());
        break;
    case Phonon::LoadingState: //Quiet GCC
        state = "LoadingState";
        break;
    case Phonon::BufferingState:
        state = "BufferingState";
        break;
    }
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Phonon state: %1").arg(state));
}
#endif

QString Metronome::obtainBroadcastIp()  {

    // QRegExp rx("^([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.).*$");
    // if (Preferences::get()->getMetronomeUdpNetwork().contains(rx))  {
    //     QString broadcastIp = rx.cap(1)+"255";
    //     if (ISLOGGABLE_INFO) LOG_INFO(QString("Configured broadcast IP: " + broadcastIp));
    //     return broadcastIp;
    // }

    // if (QString::fromLatin1(BUILD_OS) == "Linux")    {
    //     QProcess *proc = new QProcess();
    //     proc->start("./midiLayer.ip");
    //     proc->waitForStarted(500); //blocking, return bool
    //     proc->waitForFinished(500); //blocking, return bool

    //     QByteArray out = proc->readAllStandardOutput();
    //     QString outStr = QString::fromLatin1(out);

    //     if (outStr.contains(rx))   {
      //       QString broadcastIp = rx.cap(1)+"255";
     //        if (ISLOGGABLE_INFO) LOG_INFO(QString("Autodetect broadcast IP: " + broadcastIp));
     //        return broadcastIp;
    //     }
    // }
    if (ISLOGGABLE_INFO) LOG_INFO(QString("Boadcast address not set, will use 255.255.255.255"));
    return "255.255.255.255";
}

QString Metronome::obtainHostName()  {
    QString configHostName = Preferences::get()->getMetronomeUdpHostName();
    if (!configHostName.isNull() && !configHostName.isEmpty())  {
        return configHostName;
    }
    QString hostName = QHostInfo::localHostName();
    Preferences::get()->setMetronomeUdpHostName(hostName);
    return hostName;
}
