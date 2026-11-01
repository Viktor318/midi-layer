/*
    MidiLayer
    Copyright (C) 2012-2022, António José Soares Maia <maia.ajs@gmail.com>

    For this file, the following copyright notice is also applicable:
    Copyright (C) 2009-2013, Antonio Manuel Silva Oliveira. All rights
    reserved.

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

#include "LogFile.h"
#include <QFileInfo>

// Windows fork: a napló legfeljebb ekkora lehet; e fölött a program átnevezi ".old" végűre (a
// korábbi ".old" törlődik), és új naplót kezd. Így a két fájl együtt legfeljebb kb. 10 MB.
// Eredetileg a napló korlátlanul nőtt, pl. kihúzott MIDI-eszköznél minden üzenet egy hibasort írt.
#define LOG_MAX_SIZE (5 * 1024 * 1024)


LogFile::LogFile( QString fileName ) {
    logFile.setFileName( fileName );
    // Windows fork: túl nagy napló átnevezése már indításkor
    if ( QFileInfo(fileName).size() > LOG_MAX_SIZE )   {
        QFile::remove( fileName + ".old" );
        QFile::rename( fileName, fileName + ".old" );
    }
    logReady 	= logFile.open( QIODevice::Append | QIODevice::Text );
    stopLog		= false;
    logData.clear();
    this->start();
}


LogFile::~LogFile( void ) {

}


bool LogFile::isReady( void ) {
    return logReady;
}


void LogFile::write( bool supress , QString str ) {

    if( supress )
        return;

    mutex.lock();
    if ( !logReady ) {
        mutex.unlock();
        return;
    }

    logData << timeStamp() + str << "\n";
    // logData << time() + str << "\n";
    if (logData.size()==1 )
        logWait.wakeAll();

    mutex.unlock();
}


void LogFile::write( bool supress , QStringList list ) {

    QString str;

    if( supress )
        return;

    str.clear();
    while ( !list.isEmpty() ) {
        str.append( list.takeFirst() );
    }

    mutex.lock();
    if ( !logReady ) {
        mutex.unlock();
        return;
    }
    logData << timeStamp() + str << "\n";

    if( logData.size()==1 )
        logWait.wakeAll();
    mutex.unlock();
}


void LogFile::stop( void ) {
    mutex.lock();
    stopLog		= true;
    logReady	= false;
    mutex.unlock();
    wait();
}


void LogFile::run (void) {
    QString	str;
    bool	logEmpty;

    while( !stopLog ) {

        while( 1 ) {
            mutex.lock();
            logEmpty	= logData.isEmpty();
            mutex.unlock();

            if ( logEmpty )
                break;

            mutex.lock();
            str	= logData.takeFirst();
            mutex.unlock();
            QCoreApplication::processEvents();
            // Windows fork: UTF-8, hogy az ékezetes eszköz- és presetnevek se legyenek '?' a naplóban
            logFile.write( str.toUtf8() );
            this->rotateIfTooLarge();     // Windows fork
        }
        logFile.flush();
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        msleep(200);
    }
    logFile.close();
    return;
}


// Windows fork: futás közben is figyeljük a méretet (csak a naplózó szál írja a fájlt)
void LogFile::rotateIfTooLarge( void ) {
    if ( !logFile.isOpen() || logFile.size() <= LOG_MAX_SIZE )
        return;
    QString fileName = logFile.fileName();
    logFile.close();
    QFile::remove( fileName + ".old" );
    QFile::rename( fileName, fileName + ".old" );
    logFile.open( QIODevice::Append | QIODevice::Text );
}


QString LogFile::timeStamp( void ) {
    QDate date 			= QDate::currentDate();
    QTime time 			= QTime::currentTime();
    QString date_format = "yyyy-MM-dd";
    QString time_format = "hh:mm:ss.zzz";
    QString qstr_time 	= time.toString(time_format);
    QString qstr_date 	= date.toString(date_format);
    return qstr_date + " " + qstr_time + " ";
}


QString LogFile::time( void ) {
#ifdef __WINDOWS_MM__
    return this->timeStamp();
#else
    QTime time = QTime::currentTime();
    return QString::number(time.msecsSinceStartOfDay());
#endif
}
