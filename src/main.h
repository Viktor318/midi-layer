#ifndef MAIN_H
#define MAIN_H

#include <QTranslator>
#include <iostream>
#include <cstdlib>
#include <QStringList>
#include <QMap>
#include <QFontDatabase>
#include <QFile>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "config.h"
#include "Utils.h"
#include "MainWindow.h"
#include "UpdateNotifier.h"

void checkSingleInstance();
void detectAndSetRunMode(QStringList arguments);
void fatalMessageAndExit(QString dataDir);
void addFontFiles();
void showSplashScreen();

class main
{
public:
    main();
};

#endif // MAIN_H
