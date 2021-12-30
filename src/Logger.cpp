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


#include "Logger.h"


/* TODO: Implement recognition of env var MIDILAYER_LOG_LEVEL */

char const *sevText[7]  = {
    ""        ,
    "TRACE"   ,
    "DEBUG"   ,
    "INFO "   ,
    "WARN "   ,
    "ERROR"   ,
    "FATAL"
};


std::unique_ptr<Logger> Logger::m_pInstance;


Logger::Logger() : m_logger(QDir::tempPath() + "/"+__LOG_FILE__) {
}


Logger::~Logger() {
    m_logger.stop();
}


Logger *Logger::getInstance(void) {
    if ( 0==m_pInstance.get() ) {
        try {
            m_pInstance.reset(new Logger);
        }
        catch(...) {
            std::cerr << "Unhandled exception! Log will not work!" << std::endl;
        }
    }
    return m_pInstance.get();
}


void Logger::log( Severity level, QString file, int line, QString func, QString text ) {
    if ( level >= __LOG_LEVEL__ ) {
        QString msg;
        #ifdef QT_DEBUG
            msg = QString("[%1] %2 %3 <%4> | %5").arg(sevText[level], file).arg(line).arg(func, text);
            qDebug() << QString("[%1]").arg(sevText[level]) << text;
        #else
            msg = QString("[%1] %2").arg(sevText[level], text);
        #endif
        m_logger.write(false , msg);
    }
}


bool Logger::isLoggable(Severity level)  {
    return ( level >= __LOG_LEVEL__ );
}
