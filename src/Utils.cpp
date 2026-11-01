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


#include "Utils.h"


Utils * Utils::m_Utils;
QMutex Utils::mutex;

Utils* Utils::get() {
    if( !m_Utils )
    {
        mutex.lock();
        if( !m_Utils )
            m_Utils = new Utils;
        mutex.unlock();
    }

    return m_Utils;
}


void Utils::Drop()  {
    mutex.lock();
    delete m_Utils;
    m_Utils = 0;
    mutex.unlock();
}


void Utils::setMainWindowReady(bool ready)  {
    this->mainWindowReady = ready;
}


bool Utils::isMainWindowReady() {
    return this->mainWindowReady;
}


QString Utils::getCloneName(QString name, QStringList existingNames) {

    if (existingNames.indexOf(name,0)==-1)
        return name;

    QString radical = "";

    // Verify if 'name' is a clone
    QRegularExpression regexp("(.*)\\s\\((\\d+)\\)");
    QRegularExpressionMatch match = regexp.match(name);
    int maxIndex=1;
    if (match.hasMatch())  {
        radical = match.captured(1);
        maxIndex = match.captured(2).toInt(0,10);
        if (maxIndex<1)
            maxIndex=1;
    } else {
        radical=name;
    }

    while (existingNames.contains( QString("%1 (%2)").arg(radical).arg(++maxIndex)));
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString ("getCloneName result: %1 (%2)").arg(radical).arg(maxIndex));
    return QString ("%1 (%2)").arg(radical).arg(maxIndex);
}


bool Utils::copyDir(const QString source, const QString destination, const bool override) {

    QDir directory(source);
    bool error = false;

    if (!directory.exists()) {
        if (ISLOGGABLE_INFO) LOG_ERROR("copyDir: source directory ("+ source +") does not exist");
        return false;
    }

    QStringList dirs = directory.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QStringList files = directory.entryList(QDir::Files | QDir::Hidden);

    QList<QString>::iterator d,f;

    QDir temp(destination);
    temp.mkpath(temp.path());

    for (d = dirs.begin(); d != dirs.end(); ++d) {
        if ((*d) == "." || (*d) == "..") {
            continue;
        }

        if (!QFileInfo(directory.path() + "/" + (*d)).isDir()) {
            continue;
        }

        QDir temp(destination + "/" + (*d));
        temp.mkpath(temp.path());

        if (!copyDir(directory.path() + "/" + (*d), destination + "/" + (*d), override)) {
            error = true;
        }
    }

    for (f = files.begin(); f != files.end(); ++f) {
        QFile tempFile(directory.path() + "/" + (*f));


        if (QFileInfo(directory.path() + "/" + (*f)).isDir()) {
            continue;
        }

        QFile destFile(destination + "/" + directory.relativeFilePath(tempFile.fileName()));

        if (destFile.exists() && override) {
            destFile.remove();
        }

        if (!tempFile.copy(destination + "/" + directory.relativeFilePath(tempFile.fileName()))) {
            if (ISLOGGABLE_ERROR) LOG_ERROR("copyDir: could not copy file "+ tempFile.fileName() + " to " +destination + "/" + directory.relativeFilePath(tempFile.fileName()));
            error = true;

        }
    }
    return !error;
}


/*!
   Delete a directory along with all of its contents.
   \param dirName Path of directory to remove.
   \return true on success; false on error.
*/
bool Utils::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}


QStringList Utils::getBanksOnDir(QString baseDir) {

    QStringList dirList;
    QDir banksDir = QDir( baseDir );
    QStringList bankDirNameFilter;
    bankDirNameFilter.append( QString("*") );
    QStringList dirsOnFolder = banksDir.entryList(bankDirNameFilter, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable );
    // Find a dirs with bank file definitions
    for (int i=0; i<dirsOnFolder.count(); i++)  {
        QString completeBankFileName = baseDir + "/" + dirsOnFolder.at(i) + "/" + dirsOnFolder.at(i) + APP_DATA_BANK_EXTENSION;
        QFile file( completeBankFileName );
        if ( file.exists() )	{
            dirList << dirsOnFolder.at(i);
        }
    }
    return dirList;
}


QStringList Utils::searchBanksOnDir(QString baseDir) {

    QStringList dirList;
    // Scan available preset files
    QDir banksDir = QDir( baseDir );
    QStringList bankDirNameFilter;
    bankDirNameFilter.append( QString("*") );
    QStringList dirsOnFolder = banksDir.entryList(bankDirNameFilter, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable );

    // Find a dirs with bank file definitions
    for (int i=0; i<dirsOnFolder.count(); i++)  {
        bool canAddBank = false;
        QString completeBankFileName = baseDir + "/" + dirsOnFolder.at(i) + "/" + dirsOnFolder.at(i) + APP_DATA_BANK_EXTENSION;
        QFile file( completeBankFileName );
        if ( file.exists() )	{
            canAddBank = true;
        }
        else  {
            QDir presetsDir = QDir( baseDir + "/" + dirsOnFolder.at(i) );
            QStringList presetsDirNameFilter;
            presetsDirNameFilter.append( QString("*") );
            QStringList presetsOnFolder = presetsDir.entryList(presetsDirNameFilter, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable );
            for (int j=0; j<presetsOnFolder.count(); j++)   {
                QString completePresetFileName = baseDir + "/" + dirsOnFolder.at(i) + "/" + presetsOnFolder.at(j)+ "/" + presetsOnFolder.at(j) + APP_DATA_PRESET_EXTENSION;
                QFile file( completePresetFileName );
                if ( file.exists() )	{
                    canAddBank = true;
                    break;
                } else  {
                    QDir elementsDir = QDir( baseDir + "/" + dirsOnFolder.at(i)  + "/" + presetsOnFolder.at(j) );
                    QStringList elementsDirNameFilter;
                    elementsDirNameFilter.append( QString("*%1").arg(APP_DATA_ELEMENT_EXTENSION) );
                    QStringList elementsOnFolder = elementsDir.entryList(elementsDirNameFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable );
                    if (elementsOnFolder.count()>0) {
                        canAddBank = true;
                    }
                }
            }
        }
        if (canAddBank) {
            dirList << dirsOnFolder.at(i);
        }
    }
    return dirList;
}


void Utils::setPortableMode(bool portableMode)   {
    this->portableMode = portableMode;
}


bool Utils::isPortableMode(void)   {
    return this->portableMode;
}


void Utils::setDataDir(QString dataDir)   {
    this->dataDir = dataDir;
}


QString Utils::getDataDir(void)   {
    return this->dataDir;
}


void Utils::setPreferencesDir(QString preferencesDir)   {
    this->preferencesDir = preferencesDir;
}


QString Utils::getPreferencesDir(void)   {
    return this->preferencesDir;
}


void Utils::configDisplayFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("LCDDotPT"));
#ifdef __MACOSX_CORE__
    font->setPointSize(16);
#else
    font->setPointSize(12);
#endif
    //    font->setBold(false);
    //    font->setWeight(50);
    //    font->setKerning(true);
    font->setStyleStrategy(QFont::PreferDefault);
}


void Utils::configBigSpinValueFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("LED Digital 7"));
#ifdef __MACOSX_CORE__
    font->setPointSize(20);
#else
    font->setPointSize(18);
#endif
    font->setStyleStrategy(QFont::PreferDefault);
}


void Utils::configSpinValueFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("LED Digital 7"));
#ifdef __MACOSX_CORE__
    font->setPointSize(16);
#else
    font->setPointSize(14);
#endif
    font->setStyleStrategy(QFont::PreferDefault);
}


void Utils::configElementStripFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("Economica"));
#ifdef __MACOSX_CORE__
    font->setPointSize(14);
#else
    font->setPointSize(10);
#endif
    font->setStyleStrategy(QFont::PreferDefault);
}


void Utils::configElementStripBigFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("Economica"));
#ifdef __MACOSX_CORE__
    font->setPointSize(18);
#else
    font->setPointSize(14);
#endif
    font->setStyleStrategy(QFont::PreferDefault);
}


void Utils::configElementStripSpinFont(QFont * font)   {
    font->setFamily(QString::fromUtf8("LED Digital 7"));
#ifdef __MACOSX_CORE__
    font->setPointSize(15);
#else
    font->setPointSize(11);
#endif
    font->setStyleStrategy(QFont::PreferDefault);
}


// Windows fork: a bank-, preset- és elemnév elejéről és végéről levágja a szóközöket és a
// pontokat. A névből mappa- és fájlnév lesz: a Windows a név végéről csendben elhagyja a
// pontot és a szóközt (így a név és a valódi mappa eltérne), Linuxon és macOS-en pedig a
// ponttal kezdődő név rejtett lenne. Ezért ezeket a getNameRegexValidator sem engedi; a
// felhasználónak viszont nem egyértelmű ez a szabály, így a program maga teszi rendbe a nevet.
// A név belsejében a szóköz és a pont továbbra is megengedett.
QString Utils::cleanName(QString name)  {
    int start = 0;
    int end = name.length();
    while (start < end && (name.at(start).isSpace() || name.at(start) == QChar('.')))
        start++;
    while (end > start && (name.at(end-1).isSpace() || name.at(end-1) == QChar('.')))
        end--;
    return name.mid(start, end - start);
}


QRegularExpression Utils::getNameRegexValidator()  {
    QRegularExpression nameValidator("^(([a-zA-Z]:|\\\\)\\\\)?(((\\.)|(\\.\\.)|([^\\\\/:\\*\\?""\\|<>\\. ](([^\\\\/:\\*\\?""\\|<>\\. ])|([^\\\\/:\\*\\?""\\|<>]*[^\\\\/:\\*\\?""\\|<>\\. ]))?))\\\\)*[^\\\\/:\\*\\?""\\|<>\\. ](([^\\\\/:\\*\\?""\\|<>\\. ])|([^\\\\/:\\*\\?""\\|<>]*[^\\\\/:\\*\\?""\\|<>\\. ]))?$");
    return nameValidator;
}


QString Utils::buildElementTabIconPath(bool isActive, bool isEdited)  {
    QString sufixOnOff ="";
    if (isActive)  {
        sufixOnOff = "on";
    } else  {
        sufixOnOff = "off";
    }

    QString sufixEdited ="";
    if (isEdited)  {
        sufixEdited = "_edited";
    }

    return QString(":/images/tabIcon_%1%2.png").arg(sufixOnOff).arg(sufixEdited);
}


bool Utils::renameFile(QString oldName, QString newName)    {
    if ( !QFile::rename(oldName, newName) )  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Can not rename file '"+ oldName +"' to '"+ newName+ "'");
        return false;
    }
    return true;
}


bool Utils::renameDir(QString oldName, QString newName)    {
    QDir dir;
    if ( !dir.rename(oldName, newName) )  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Can not rename directory '"+ oldName +"' to '"+ newName+ "'");
        return false;
    }
    return true;
}


QStringList * Utils::getControllersList() {
    if (controllersList.size()==0)  {
        controllersList    << "Bank Select" \
                           << "Modulation Wheel" \
                           << "Breath Controller" \
                           << "(undefined)" \
                           << "Foot Controller" \
                           << "Portamento Time" \
                           << "Data Entry" \
                           << "Volume" \
                           << "Balance" \
                           << "(undefined)" \
                           << "Pan" \
                           << "Expression" \
                           << "Effect Control 1" \
                           << "Effect Control 2" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "General Purpose #1" \
                           << "General Purpose #2" \
                           << "General Purpose #3" \
                           << "General Purpose #4" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "(not documented)" \
                           << "Hold 1 (Sustain)" \
                           << "Portamento On/Off" \
                           << "Sostenuto" \
                           << "Soft Pedal" \
                           << "Legato Footswitch" \
                           << "Hold 2" \
                           << "Sound Contr. 1 / Sound Variation" \
                           << "Sound Contr. 2 / Timbre Content" \
                           << "Sound Contr. 3 / Release Time" \
                           << "Sound Contr. 4 / Attack Time" \
                           << "Sound Contr. 5 / Brightness" \
                           << "Sound Contr. 6" \
                           << "Sound Contr. 7" \
                           << "Sound Contr. 8" \
                           << "Sound Contr. 9" \
                           << "Sound Contr. 10" \
                           << "General Purpose #5" \
                           << "General Purpose #6" \
                           << "General Purpose #7" \
                           << "General Purpose #8" \
                           << "Portamento Control" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "Effect 1 / External Effects Depth" \
                           << "Effect 2 / Tremelo Depth" \
                           << "Effect 3 / Chorus Depth" \
                           << "Effect 4 / Detune (Celeste)" \
                           << "Effect 5 / Phaser Depth" \
                           << "Data Increment" \
                           << "Data Decrement" \
                           << "Non-Registered Parameter LSB" \
                           << "Non-Registered Parameter MSB" \
                           << "Registered Parameter LSB" \
                           << "Registered Parameter MSB" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "(undefined)" \
                           << "All Sound Off" \
                           << "Reset All Controllers" \
                           << "Local Control On/Off" \
                           << "All Notes Off" \
                           << "Omni Mode Off (all notes off)" \
                           << "Omni Mode On (all notes off)" \
                           << "Mono Mode On (all notes off)" \
                           << "Poly Mode On (all notes off)";
    }
    return &controllersList;
}


QString Utils::getLockFileCompletePath()    {
    return QDir::tempPath()+"/"+APP_LOCK_FILE_NAME;
}


bool Utils::checkLockFile() {
    QFileInfo checkFile(getLockFileCompletePath());
    return checkFile.exists() && checkFile.isFile();
}


// Windows fork: az egyetlen futó példány biztosítása QLockFile-lal. Az eredeti megoldás egy
// egyszerű PID-fájlt hozott létre, és indításkor csak azt nézte, hogy létezik-e. Összeomlás vagy
// áramszünet után a fájl ott maradt, és a program nem indult el, amíg kézzel ki nem töröltük
// (vagy --remove-lock-kal el nem indítottuk). A QLockFile felismeri az elárvult zárolást (a
// fájlban szereplő folyamat már nem fut), és magától eltávolítja.
bool Utils::tryLockSingleInstance()    {
    if (lockFile == nullptr)  {
        lockFile = new QLockFile( getLockFileCompletePath() );
        // csak akkor elavult, ha a zároló folyamat már nem fut (az életkor nem számít)
        lockFile->setStaleLockTime(0);
    }
    if (lockFile->isLocked() || lockFile->tryLock(0))
        return true;

    if (lockFile->error() == QLockFile::LockFailedError)   {
        qint64 pid = 0;
        QString hostName, appName;
        lockFile->getLockInfo(&pid, &hostName, &appName);
        if (ISLOGGABLE_WARN) LOG_WARN(QString("Another instance is running (pid %1, lock file '%2')").arg(pid).arg(getLockFileCompletePath()));
        return false;
    }

    // Egyéb hiba (pl. nem írható az ideiglenes mappa): ne akadályozza az indítást
    if (ISLOGGABLE_WARN) LOG_WARN(QString("Could not create lock file '%1' (error %2), starting without single instance check")
                                  .arg(getLockFileCompletePath()).arg(lockFile->error()));
    return true;
}


bool Utils::deleteLockFile()    {
    // Saját zárolás feloldása
    if (lockFile != nullptr && lockFile->isLocked())  {
        lockFile->unlock();
        return true;
    }
    // --remove-lock: egy idegen zárolófájl törlése (Windowson nem sikerül, amíg a másik példány fut)
    return QFile::remove( getLockFileCompletePath() );
}


#ifdef QT_DEBUG
QString Utils::toString(QStringList stringList)  {
    QString string = QString("");
    bool first=true;
    for (int i=0; i<stringList.count(); i++)  {
        if (first)
            first=false;
        else
            string.append(", ");
        string.append(stringList.at(i));
    }
    return "["+string+"]";
}
#endif
