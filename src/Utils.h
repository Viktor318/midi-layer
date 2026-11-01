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

#ifndef UTILS_H
#define UTILS_H

#include <QAtomicPointer>
#include <QMutex>
#include <QCoreApplication>
#include <QString>
#include <QRegularExpression>
#include <QStringList>
#include <QList>
#include <QFont>
#include <QChar>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QLockFile>
#include "config.h"
#include "Logger.h"

class Utils {

    static Utils * m_Utils;
    static QMutex mutex;

public:

    static Utils* get();
    static void Drop();

    void setMainWindowReady(bool ready);
    bool isMainWindowReady();

    QString getCloneName(QString name, QStringList existingNames);
    bool renameFile(QString oldName, QString newName);
    bool renameDir(QString oldName, QString newName);
    bool copyDir(const QString source, const QString destination, const bool override);
    bool removeDir(const QString &dirName);
    // Windows fork: fájl vagy mappa áthelyezése a rendszer lomtárába, tartalékként a saját lomtárba
    bool moveToTrash(QString path, QString bankName, QString relativeName);
    QString getFallbackTrashDir();
    // Search bank dirs
    QStringList getBanksOnDir(QString baseDir);
    // Search bank dirs and files and potencial dirs (dirs structure without bank settings).
    QStringList searchBanksOnDir(QString baseDir);

    void setPortableMode(bool portableMode);
    bool isPortableMode(void);

    void setDataDir(QString dataDir);
    QString getDataDir(void);

    void setPreferencesDir(QString preferencesDir);
    QString getPreferencesDir(void);
    void configDisplayFont(QFont * font);
    void configBigSpinValueFont(QFont * font);
    void configSpinValueFont(QFont * font);
    void configElementStripFont(QFont * font);
    void configElementStripSpinFont(QFont * font);
    void configElementStripBigFont(QFont * font);

    QRegularExpression getNameRegexValidator();
    QString cleanName(QString name);    // Windows fork: szóközök és pontok levágása a név elejéről és végéről

    QString buildElementTabIconPath(bool isActive, bool isEdited);

    QStringList controllersList;
    QStringList * getControllersList();

    QString getLockFileCompletePath();
    bool checkLockFile();
    bool tryLockSingleInstance();   // Windows fork: false, ha egy másik példány fut
    bool deleteLockFile();

#ifdef QT_DEBUG
    QString toString(QStringList stringList);
#endif

private:

    bool mainWindowReady;
    bool portableMode;
    QString dataDir;
    QString preferencesDir;
    QLockFile * lockFile = nullptr;     // Windows fork: a futó példány zárolása

};

#endif // UTILS_H
