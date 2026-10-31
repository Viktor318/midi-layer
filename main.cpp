/*
    Midi Layer
    Copyright (C) 2012-2013, António José Soares Maia <maia.ajs@gmail.com>

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

#include <QScreen>
#include <QStyleFactory>
#include "main.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Qt 6: a Windows 11-es alapstílus szélesebb fel/le gombokat rajzol a számmezőkre,
    // és a régi (Qt 5-re tervezett) elrendezésben levágja a számokat. A Fusion stílus
    // minden platformon egyforma, és jól illeszkedik az eredeti felülethez.
    // A "-style <név>" parancssori argumentummal továbbra is felülbírálható.
    bool styleGivenOnCommandLine = false;
    for (int i = 1; i < argc; ++i) {
        if (QString::fromLocal8Bit(argv[i]).startsWith("-style")) {
            styleGivenOnCommandLine = true;
            break;
        }
    }
    if (!styleGivenOnCommandLine)
        app.setStyle(QStyleFactory::create("Fusion"));

    // Qt Translation
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Translation
    QTranslator myappTranslator;
    myappTranslator.load("midiLayer_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    app.setOrganizationName(APP_ORGANIZATION);
    app.setOrganizationDomain("http://sourceforge.net/projects/midi-layer/");
    app.setApplicationName(QString::fromLocal8Bit(APP_NAME));
    app.setApplicationVersion(QString::fromLocal8Bit(APP_VERSION)+"."+QString::fromLocal8Bit(BUILD_REVISION));
    app.setWindowIcon( QIcon( ":/images/icon64.png" ) );

    detectAndSetRunMode(app.arguments());

    checkSingleInstance();

    if (ISLOGGABLE_INFO) LOG_INFO("# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #");
    if (ISLOGGABLE_INFO) LOG_INFO("Start application from "+ QCoreApplication::applicationDirPath());

#ifdef QT_DEBUG
    if (ISLOGGABLE_INFO) LOG_INFO("Running a debug build");
#else
    if (ISLOGGABLE_INFO) LOG_INFO("Running a release build");
#endif

    if (ISLOGGABLE_INFO) LOG_INFO(QString("Translator: %1%2").arg("midiLayer_").arg(QLocale::system().name()));

    // Adding fonts to database
    addFontFiles();

    Utils::get()->setMainWindowReady(false);
    MainWindow mainWindow(&app);

    // Show SplashScreen
    showSplashScreen(&mainWindow);

    mainWindow.show();
    int returnValue = app.exec();

    return returnValue;
}

void checkSingleInstance()  {

    // check if file exists and if yes: Is it really a file and no directory?
    if (Utils::get()->checkLockFile()) {

        QMessageBox::StandardButton reply;

        reply = QMessageBox::critical(
                    NULL,
                    QString("%1 %2").arg(APP_NAME).arg(APP_VERSION),
                    QString("Another instance of %1 is running. If you experienced a abnormal application exit, you must remove the file \"%2\" manualy or reboot you device.").arg(APP_NAME).arg(Utils::get()->getLockFileCompletePath()));

        if (reply == QMessageBox::Ok) {
            exit(1);
        }

    } else {
        // Create lock file with pid content
        Utils::get()->createLockFile();
    }
}

void detectAndSetRunMode(QStringList arguments)  {

    QString homeDataDir = QDir::homePath()+"/.midiLayer/" + APP_DATA_DIR;
    QString portableDataDir = QCoreApplication::applicationDirPath() + "/" + APP_DATA_DIR;
    QDir dir;

    bool forcedMode =false;
    Utils::get()->setPortableMode(false);

    if ( arguments.size() == 2)   {
        //if (arguments.at(1) == "--portable" )	{
        //    if (ISLOGGABLE_INFO) LOG_INFO("Forced to run in portable mode.");
        //    if (dir.mkpath(portableDataDir))  {
        //        Utils::get()->setPortableMode(true);
        //        forcedMode =true;
        //    } else  {
        //        fatalMessageAndExit(portableDataDir);
        //    }
        // }
        // else
        if (arguments.at(1) == "--non-portable")   {
            if (ISLOGGABLE_INFO) LOG_INFO("Forced to run in non-portable mode.");
            if (dir.mkpath(homeDataDir))  {
                forcedMode =true;
            } else  {
                fatalMessageAndExit(homeDataDir);
            }
        }
        else if (arguments.at(1) == "--remove-lock")   {
            if (Utils::get()->checkLockFile())  {
                Utils::get()->deleteLockFile();
                QString infoMessage = QString( QObject::tr("Lock removed!\nNow already can start the application normally.") );
                QMessageBox::information(0, qApp->applicationName(), infoMessage);
            } else  {
                QString infoMessage = QString( QObject::tr("No lock file founded!") );
                QMessageBox::information(0, qApp->applicationName(), infoMessage);
            }
            qApp->exit(0);
            exit(0);
        }
        else if (arguments.at(1) == "--list-interfaces")   {
            QTextStream(stdout) << "MIDI in interfaces:" << "\n";
            QList<QString> inPorts = MidiManager::get()->getAvailableInPorts().keys();
            for (int i = 0; i < inPorts.count(); i++) {
                QTextStream(stdout) << "-" << inPorts.at(i) << Qt::endl;
            }
            QTextStream(stdout) << "MIDI out interfaces:" << "\n";
            QList<QString> outPorts = MidiManager::get()->getAvailableOutPorts().keys();
            for (int i = 0; i < outPorts.count(); i++) {
                QTextStream(stdout) << "-" << outPorts.at(i) << Qt::endl;
            }
            qApp->exit(0);
            exit(0);
        }
        else if (arguments.at(1) == "-v" || arguments.at(1) == "--version")   {
            QTextStream(stdout) << QString("%1.%2").arg(APP_VERSION).arg(BUILD_REVISION) << Qt::endl;
            qApp->exit(0);
            exit(0);
        }
        else if (arguments.at(1) == "-h" || arguments.at(1) == "--help")   {
            QString helpLine0 = QString("\n  %1 %2 help:\n\n").arg(APP_NAME).arg(APP_VERSION);
            // QString helpLine1 = QString("  --portable           Forced to run in portable mode. Use data files from dir\n                       \"./%3\" of binary location.\n\n").arg(APP_DATA_DIR);
            QString helpLine2 = QString("  --non-portable       Forced to run in non portable mode. Use data files from\n                       OS user data.\n\n");
            QString helpLine3 = QString("  --remove-lock        Remove application lock file\n                       \"%1\".\n                       Used when you experienced a abnormal application exit.\n\n").arg(Utils::get()->getLockFileCompletePath());
            QString helpLine4 = QString("  --list-interfaces    List all MIDI interfaces detected and exit.\n\n");
            QString helpLine5 = QString("  -v, --version        Output version information and exit.\n\n");
            QString helpLine6 = QString("  -h, --help           Display this help and exit.\n\n");
            QTextStream(stdout) << helpLine0 << helpLine2 << helpLine3 << helpLine4 << helpLine5 << helpLine6;
            qApp->exit(0);
            exit(0);
        }
        else
            LOG_ERROR("Unreconized option \""+ arguments.at(1) +"\"");
    }

    // Autodetect mode (portable/non-portable)
    if (!forcedMode) {
        if (dir.exists(portableDataDir))   {
            Utils::get()->setPortableMode(true);
        } else  {
            if (!dir.exists(homeDataDir) && !dir.mkpath(homeDataDir))   {
                fatalMessageAndExit(homeDataDir);
            }
        }
    }

    // Set variable dirs info
    if (Utils::get()->isPortableMode()) {
        Utils::get()->setDataDir(portableDataDir);
    } else  {
        Utils::get()->setDataDir(homeDataDir);
    }
    QFileInfo fi(Utils::get()->getDataDir());
    Utils::get()->setPreferencesDir(fi.absoluteDir().path());

}

void fatalMessageAndExit(QString dataDir)  {
    QString fatalMessage ="Could not create data dir at \""+dataDir+"\". Please verify path permitions and try again.";
    LOG_FATAL(fatalMessage);
    QMessageBox::critical(0, qApp->applicationName(), fatalMessage);
    exit(-1);
}

void addFontFiles() {

    // List of font to used (Must be on fonts folder).
    QStringList fontFileNames;
    fontFileNames << "LCDDotPT.ttf" << "led_digital_7.ttf" << "Economica-Regular-OTF.otf";

    // Adding fonts to database
    QString fontFile;

    for (int f=0; f<fontFileNames.length(); f++)    {
        fontFile = Utils::get()->getPreferencesDir() + "/" + APP_FONTS_DIR + "/" + fontFileNames.at(f);
        if ( QFile::exists(fontFile) )   {
            QFontDatabase::addApplicationFont ( fontFile );
        } else  {
            if (ISLOGGABLE_INFO) LOG_ERROR(QString("Can not found font file '%1'. Will use default font.").arg(fontFile));
        }
    }
}

void showSplashScreen(MainWindow * mainWindow) {

    // Show SplashScreen
    QPixmap pixmap(":/images/splashScreen.png");
    QSplashScreen * splash = new QSplashScreen(pixmap);   // Qt 6: a szülő ablakot váró konstruktor megszűnt

    QTimer * timer = new QTimer();
    timer->setInterval(5000);
    timer->setSingleShot(true);
    QObject::connect(timer, SIGNAL(timeout()), splash, SLOT(close()));
    timer->start();

    QRect srect(0, 0, splash->width(), splash->height());
    splash->move(QGuiApplication::primaryScreen()->availableGeometry().center() - srect.center());
    splash->show();
    splash->showMessage ( QString( "<br><br><br><br><span style=\"color: #CCC;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Version %1 - Copyright (C) 2012-2015, %3</span>")
                          .arg( QApplication::applicationVersion(),
                                QApplication::organizationName() ), Qt::AlignLeft | Qt::AlignBottom, QColor(Qt::blue) );
}

