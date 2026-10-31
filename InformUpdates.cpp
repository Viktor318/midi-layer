/*
    Midi Layer
    Copyright (C) 2012-2014, António José Soares Maia <maia.ajs@gmail.com>

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

#include <QRegularExpression>
#include "InformUpdates.h"

InformUpdates::InformUpdates(QObject *parent):
    QObject(parent)
{
    if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - Starting update notifier...");

    if (!Preferences::get()->getCheckForUpdates())  {
        if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - User prefer not be informed for updates");
        return;
    }

    mNetworkManager = new QNetworkAccessManager();
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));
//   QUrl url = "http://downloads.sourceforge.net/project/midi-layer/lastVersionInfo.xml?r=&ts="+QString::number(QDateTime::currentDateTime().toTime_t())+"&use_mirror=switch";
    QUrl url = QString("http://netcologne.dl.sourceforge.net/project/midi-layer/lastVersionInfo.xml");
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("InformUpdates request URL: [%1]").arg( url.toString() ));
    mNetworkManager->get(QNetworkRequest(url));

}

InformUpdates::~InformUpdates() {
}

void InformUpdates::onNetworkReply(QNetworkReply* reply)
{

    QString replyString = QString::fromUtf8(reply->readAll().data());

    if(reply->error() == QNetworkReply::NoError)    {

        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if ( httpstatuscode == 302 || httpstatuscode == 303 || httpstatuscode == 307 ||  reply->rawHeaderList().contains("Location") )    {
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - HTTP REDIRECTING to ") + reply->header(QNetworkRequest::LocationHeader).toString());
            QNetworkRequest req(reply->header(QNetworkRequest::LocationHeader).toString());
            mNetworkManager->get(req);
            return;
        }
        else if (httpstatuscode == 200 )  {
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - HTTP OK"));
            if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - Downloaded last version information file\n----------------------------------------\n" + replyString + "----------------------------------------");
            this->checkUpdate(replyString);
        }
        else    {
            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("Check updates - Unexpected HTTP response: %1").arg(replyString));
        }
    }
    else    {
        if (ISLOGGABLE_ERROR) LOG_ERROR("InformUpdates reply error: "+reply->errorString());
    }
    reply->deleteLater();
}

//bool InformUpdates::saveToDisk(const QString &filename, QIODevice *data)
//{
//    QFile file(filename);
//    if (!file.open(QIODevice::WriteOnly)) {
//        if (ISLOGGABLE_ERROR) LOG_ERROR(QString("Could not open %1 for writing: %2").arg(filename).arg(file.errorString()) );
//        return false;
//    }

//    file.write(data->readAll());
//    file.close();

//    return true;
//}

void InformUpdates::checkUpdate(QString versionInfo)   {

    QDomDocument doc("mydocument");
    if (!(doc.setContent(versionInfo)) || versionInfo.isEmpty()) {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Check updates: Fail to check version info file content.");
        return;
    }

    QDomElement root = doc.documentElement();

    if (root.tagName() != "midiLayer")  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Check updates - XML root not valid.");
        return;
    }
    if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - Version info file content is ok");
    QString buildTag=QString("%1_%2").arg(BUILD_OS).arg(BUILD_ARCH);
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - Checking available version for %1...").arg(buildTag));
    QDomNode osName = root.firstChild();
    while(!osName.isNull()) {
        QDomElement e = osName.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName() == buildTag) {
                QString availableVersion = e.attribute("version","0.0.0-0");
                if (this->hasNewVersion( splitVersionNumbers(QString("%1.%2").arg(APP_VERSION).arg(BUILD_REVISION)), \
                                         splitVersionNumbers(availableVersion)) )  {
                    if (ISLOGGABLE_INFO) LOG_INFO("Check updates - New version available: '"+availableVersion+"'");
                    this->showInfo(availableVersion);
                } else  {
                    if (ISLOGGABLE_INFO) LOG_INFO("Check updates - No new version available.");
                }
            }
        }
        osName = osName.nextSibling();
    }
}


QList<int> InformUpdates::splitVersionNumbers(QString versionStr)  {

    QList<int> version;
    // Split version info
    // Qt 6: QRegExp -> QRegularExpression (találat nélkül üres listát ad, így a méretvizsgálat kiszűri)
    QRegularExpression regexp("\\s*(\\d+)\\.(\\d+)\\.(\\d+)(.*)\\s*");
    QStringList captureList = regexp.match(versionStr).capturedTexts();

    if (captureList.size()>=4)  {
        version.append(captureList.at(1).toInt(0,10));
        version.append(captureList.at(2).toInt(0,10));
        version.append(captureList.at(3).toInt(0,10));
        if (captureList.size()==5)   {
            QString revisionStr = captureList.at(4);
            revisionStr.remove(".").remove("-");
            version.append(revisionStr.toInt(0,10));
        }
    }
    return version;
}

bool InformUpdates::hasNewVersion(QList<int> runningVersion, QList<int> lastVersion)  {

    QString rv = "";
    for (int r=0; r<runningVersion.count();r++) {
        rv = rv+QString("%1-").arg(runningVersion.at(r));
    }

    QString lv = "";
    for (int r=0; r<lastVersion.count();r++) {
        lv = lv+QString("%1-").arg(lastVersion.at(r));
    }

    int limit = qMin(runningVersion.size(), lastVersion.size());
    bool newVersionAvailable= false;
    for (int n=0; n<limit;n++) {
        if (runningVersion.at(n)<lastVersion.at(n)) {
            newVersionAvailable = true;
            break;
        }
        if (runningVersion.at(n)>lastVersion.at(n))  {
            break;
        }
    }
    return newVersionAvailable;
}

void InformUpdates::showInfo(QString availableVersion)  {
    QMessageBox msgBox(0);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Update available"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString(tr("New version available: %1.<br>Download it at <a href='%2'>%2</a><br><br>You can disable check updates on preferences.")) \
                   .arg(availableVersion).arg(qApp->organizationDomain()));
    msgBox.exec();
}
