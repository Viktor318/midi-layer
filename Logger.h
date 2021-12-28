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

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <memory>
#include <QString>
#include <QDir>
#include <QtDebug>

#include "config.h"
#include "LogFile.h"
#include "Utils.h"

/** \brief Wrapper to manage the debug log.
 * \details This class is a simple wrapper aroud log classes to allow easy
 *          logging, showing file name, line number and method information.
 *          Only one instance of this class is used through the whole
 *          application. The log instance is accesses using a static member
 *          method or a few useful macros.
 */
class Logger
{
    private:
        static std::unique_ptr<Logger>   m_pInstance;
        LogFile                 m_logger;
        /** \brief Private constructor.
         * \details The constructor is made private to allow only one instance
         *          of this class per process.
         */
        Logger();

    public:
        ~Logger();
        /** \brief Severity of log messages.
          \details  This classifies log messages according to their severity.
                    Trace messages are the most verbose, hance have the least
                    severity of all. Fatal messages, on the other hand, are the
                    most severe and more likely to be written into the
                    log.
                    */
        typedef enum {
            Trace       = 1 ,
            Debug       = 2 ,
            Info        = 3 ,
            Warn        = 4 ,
            Error       = 5 ,
            Fatal       = 6
        } Severity;
        static Logger *getInstance(void);

        void log( Severity level , QString file , int line , QString func , QString text );
        bool isLoggable(Severity level);

};

/* The log facilities are meant to be used with these macros. */
/* using logging with the class is not recommended, but should still work... */

#define logger (Logger::getInstance())
#define LOG_TRACE(msg)  (logger->log(Logger::Trace, __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))
#define LOG_DEBUG(msg)  (logger->log(Logger::Debug, __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))
#define LOG_INFO(msg)   (logger->log(Logger::Info , __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))
#define LOG_WARN(msg)   (logger->log(Logger::Warn , __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))
#define LOG_ERROR(msg)  (logger->log(Logger::Error, __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))
#define LOG_FATAL(msg)  (logger->log(Logger::Fatal, __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , (msg) ))

#define ISLOGGABLE_TRACE (logger->isLoggable(Logger::Trace))
#define ISLOGGABLE_DEBUG (logger->isLoggable(Logger::Debug))
#define ISLOGGABLE_INFO  (logger->isLoggable(Logger::Info))
#define ISLOGGABLE_WARN  (logger->isLoggable(Logger::Warn))
#define ISLOGGABLE_ERROR (logger->isLoggable(Logger::Error))


#endif // LOGGER_H

