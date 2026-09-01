#ifndef METRONOMEWIDGET_H
#define METRONOMEWIDGET_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenu>
#include <QTranslator>
#include <QList>
#include <QFont>
#include <cmath>

#include "Led.h"
#include "Logger.h"
#include "Knob.h"
#include "Preferences.h"

#ifdef QT_PHONON_LIB
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#endif

// UDP
#include <QHostAddress>
#include <QUdpSocket>
#include <QDateTime>
#include <QHostInfo>
#include <QProcess>

namespace Ui {
class Metronome;
}

class Metronome : public QDialog
{
    Q_OBJECT
    
public:
    explicit Metronome(QWidget *parent = 0);
    ~Metronome();

    bool isActive();
    int getBpm();
    
private:
    Ui::Metronome *ui;
    bool isRunning;

    QHBoxLayout * horizontalLayout;
    Led * led;
    QTimer * timer;
    QTimer * tapTempoTimer;
    QAction * tapTempoAction;
    QAction * startStopAction;
    QAction * incrementAction;
    QAction * decrementAction;
    int tapTempoCount;
    int nextBeatTime;
    QList<int> tapTempoRecords;
    QTime tapTempoStopwatch;
    QString localHostName;
    QString currentPresetName;

    // UDP
    QUdpSocket * udpSocketSend;
    // QUdpSocket * udpSocketGet;
    // QHostAddress * host;
    QHostAddress * bcast;

    #ifdef QT_PHONON_LIB
    Phonon::MediaObject *clickMedia;
    Phonon::AudioOutput *clickAudioOutput;
    Phonon::Path phononPath;
    QString clickFile;
    void refreshVolume(int volumePercent);
    #endif

    void refreshActive(bool active);
    void refreshBpm(int bpm);
    void playClickSound();
    void emitMetronomeInfo();
    void sendUDP();
    int udpOrdinal;
    QString obtainBroadcastIp();
    QString obtainHostName();


private slots:

    void setBpm(int bpm);
    void slotStartStopPressed();
    void setActive(bool active);
    void slotTimeOut();
    void slotTapTempo();
    void slotStartStop();
    void slotIncrement();
    void slotDecrement();
    void slotTapTempoTimeOut();
//    void slotShowContextMenu(const QPoint& pos);
    #ifdef QT_PHONON_LIB
    void setVolume(int volumePercent);
    void slotPhononStateLog();
    #endif

    // UDP
    //void readPendingDatagrams();

public slots:
    void slotSetActive(bool active);
    void slotSetBpm(int bpm);
    #ifdef QT_PHONON_LIB
    void slotSetVolume(int volumePercent);
    #endif
    void slotSetMetronome(bool isActive, int bpm ,int volume, QString currentPresetName = "");

signals:

    void changed(bool isActive, int bpm, int volume);
};

#endif // METRONOMEWIDGET_H
