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


#include "UpdateNotifier.h"


UpdateNotifier::UpdateNotifier(QObject *parent):
    QObject(parent)
{ 
    QString fileName = "lastVersionInfo.xml";
    QString timeStr = QDateTime::currentDateTime().toString();

    urlList.append(new QUrl(QString("http://downloads.sourceforge.net/project/midi-layer/%1?r=&ts=%2&use_mirror=heanet").arg(fileName, timeStr)));
    urlList.append(new QUrl(QString("http://downloads.sourceforge.net/project/midi-layer/%1?r=&ts=%2&use_mirror=cznic").arg(fileName, timeStr)));
    urlList.append(new QUrl(QString("http://downloads.sourceforge.net/project/midi-layer/%1?r=&ts=%2&use_mirror=kent").arg(fileName, timeStr)));
    urlList.append(new QUrl(QString("http://downloads.sourceforge.net/project/midi-layer/%1?r=&ts=%2&use_mirror=garr").arg(fileName, timeStr)));

    this->urlIndex = 0;
    this->connectSignalsAndSlots();
}


void UpdateNotifier::connectSignalsAndSlots()    {
    connect(&mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));
}


UpdateNotifier::~UpdateNotifier() {
    int q=urlList.size();
    for (int i=q; i>0; i--)   {
        delete urlList.at(i-1);
    }
}


void UpdateNotifier::start()   {

    if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - Starting update notifier...");
    if (!Preferences::get()->getCheckForUpdates())  {
        if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - User prefer not be informed for updates");
        return;
    }
    this->invokeNextUrl();
}


void UpdateNotifier::invokeNextUrl()    {
    QUrl * url = this->getNextUrl();
    if (url != NULL)    {
        if (ISLOGGABLE_TRACE) LOG_TRACE(QString("UpdateNotifier request URL[%1]: [%2]").arg( this->urlIndex ).arg( url->toString() ));
        QNetworkRequest request = QNetworkRequest(*url);
        request.setRawHeader("User-Agent", QString("%1 %2 updates check").arg(APP_NAME, APP_VERSION).toLatin1());
        mNetworkManager.get(request);
    }
}


QUrl * UpdateNotifier::getNextUrl()   {
    if (this->urlIndex<urlList.size())  {
        return urlList.at(this->urlIndex++);
    } else  {
        return NULL;
    }
}


void UpdateNotifier::onNetworkReply(QNetworkReply* reply)
{

    if(reply->error() == QNetworkReply::NoError)    {

        QString replyString = QString::fromUtf8(reply->readAll());
        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if ( httpstatuscode == 302 || httpstatuscode == 303 || httpstatuscode == 307 ||  reply->rawHeaderList().contains("Location") )    {
            if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - HTTP REDIRECTING to ") + reply->header(QNetworkRequest::LocationHeader).toString());
            QNetworkRequest req(reply->header(QNetworkRequest::LocationHeader).toString());
            mNetworkManager.get(req);
            return;
        }
        else if (httpstatuscode == 200 )  {
            // if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - HTTP OK"));
            // if (ISLOGGABLE_DEBUG) LOG_DEBUG(QString("Check updates - Downloaded last version information file\n----------------------------------------\n%1----------------------------------------").arg(replyString));
            this->checkUpdate(replyString);
        }
        else    {
            if (ISLOGGABLE_ERROR) LOG_ERROR(QString("Check updates - Unexpected HTTP response: \"%1\"").arg(replyString));
        }
        reply->deleteLater();
    }
    else    {
        reply->deleteLater();
        if (ISLOGGABLE_ERROR) LOG_ERROR(QString("Check updates - reply error.") );
        this->invokeNextUrl();
    }

}


void UpdateNotifier::checkUpdate(QString versionInfo)   {

    QDomDocument doc("checkUpdateReply");
    if (!(doc.setContent(versionInfo)) || versionInfo.isEmpty()) {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Check updates - Fail to parse file content.");
        return;
    }

    QDomElement root = doc.documentElement();

    if (root.tagName() != "midiLayer")  {
        if (ISLOGGABLE_ERROR) LOG_ERROR("Check updates - XML root not valid.");
        return;
    }
    if (ISLOGGABLE_TRACE) LOG_TRACE("Check updates - Version info file content is ok");
    QString buildTag=QString("%1_%2").arg(BUILD_OS, BUILD_ARCH);
    if (ISLOGGABLE_TRACE) LOG_TRACE(QString("Check updates - Checking available version for %1...").arg(buildTag));
    QDomNode osName = root.firstChild();
    while(!osName.isNull()) {
        QDomElement e = osName.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName() == buildTag) {
                QString availableVersion = e.attribute("version","0.0.0-0");
                if (this->hasNewVersion( splitVersionNumbers(QString("%1.%2").arg(APP_VERSION, BUILD_REVISION)), \
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


QList<int> UpdateNotifier::splitVersionNumbers(QString versionStr)  {

    QList<int> version;
    // Split version info
    QRegularExpression regexp("\\s*(\\d+)\\.(\\d+)\\.(\\d+)(.*)\\s*");
    QRegularExpressionMatch match = regexp.match(versionStr);
    if (match.hasMatch() && match.capturedLength()>=4) {
        version.append(match.captured(1).toInt(0,10));
        version.append(match.captured(2).toInt(0,10));
        version.append(match.captured(3).toInt(0,10));
        if (match.capturedLength()==5)   {
            QString revisionStr = match.captured(4);
            revisionStr.remove(".").remove("-");
            version.append(revisionStr.toInt(0,10));
        }
    }
    return version;
}


bool UpdateNotifier::hasNewVersion(QList<int> runningVersion, QList<int> lastVersion)  {

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


void UpdateNotifier::showInfo(QString availableVersion)  {
    QMessageBox msgBox(0);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Update available"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString(tr("New version available: %1.<br>Download it at <a href='%2'>%2</a><br><br>You can disable check updates on preferences.")) \
                   .arg(availableVersion, qApp->organizationDomain()));
    msgBox.exec();
}
