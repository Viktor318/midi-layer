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

#ifndef UpdateNotifier_H
#define UpdateNotifier_H

#include <QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtXml/QDomDocument>
#include <QList>

#include "config.h"
#include "Logger.h"
#include "Preferences.h"

class UpdateNotifier : public QObject
{
    Q_OBJECT


public:
    UpdateNotifier(QObject *parent = 0);
    virtual ~UpdateNotifier();
    void start();

private:
    void connectSignalsAndSlots();
    void invokeNextUrl();
    QUrl * getNextUrl();
    QNetworkAccessManager mNetworkManager;
    void checkUpdate(QString versionInfo);
    QList<QUrl*> urlList;
    QList<int> splitVersionNumbers(QString versionStr);
    bool hasNewVersion(QList<int> runningVersion, QList<int> lastVersion);
    void showInfo(QString availableVersion);
    int urlIndex;

private slots:
    void onNetworkReply(QNetworkReply*);
};

#endif // UpdateNotifier_H
