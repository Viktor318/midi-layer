/*
    Midi Layer
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

#ifndef __LOG_H__
#define __LOG_H__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QCoreApplication>
#include <QFile>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QTime>
#include <QDate>
#include <QTextStream>

#include "config.h"

#define __LV( x )	(x>__MAX_VERBOSITY?true:false)

class LogFile: public QThread {
    Q_OBJECT

private:
    QMutex			mutex;
    QWaitCondition	logWait;
    QFile			logFile;
    volatile bool	logReady;
    volatile bool	stopLog;
    QStringList		logData;

private:
    void 	run(void);
    QString	timeStamp(void);
    QString time( void );

public:
    LogFile( QString fileName );
    ~LogFile( void );

    void write( bool supress , QString str );
    void write( bool supress , QStringList list );
    void stop (void);
    bool isReady( void );

};


#endif // __LOG_H__
