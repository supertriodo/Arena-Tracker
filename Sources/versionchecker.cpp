#include "versionchecker.h"
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtWidgets>

VersionChecker::VersionChecker(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    networkManager->get(QNetworkRequest(QUrl(VERSION_URL)));
    removeOldVersion();

//    qApp->setApplicationVersion(VERSION);//Eliminar runVersion

    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("runVersion", VERSION);
}


VersionChecker::~VersionChecker()
{
    delete networkManager;
}


void VersionChecker::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    emit pLog("Settings: Arena Tracker " + VERSION);

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed. Retrying...");
        networkManager->get(QNetworkRequest(reply->url()));
    }
    else
    {
        QString fullUrl = reply->url().toString();

        //Redirect
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
        {
            QByteArray location = reply->rawHeader("Location");
            emit pDebug("Redirect to --> " + location);
            networkManager->get(QNetworkRequest(QUrl(location)));
        }

        //Check version
        else if(fullUrl == VERSION_URL)
        {
            checkUpdate(QJsonDocument::fromJson(reply->readAll()).object());
        }

        //New version downloaded
        else
        {
            saveRestart(reply->readAll());
        }
    }
}


void VersionChecker::checkUpdate(const QJsonObject &versionJsonObject)
{
    QJsonArray versionArray = versionJsonObject.value("versionFree").toArray();
    QStringList allowedVersions;
    for(QJsonValue value: versionArray)
    {
        allowedVersions.append(value.toString());
    }
    QString latestVersion = allowedVersions.isEmpty()?"":allowedVersions.last();

    QSettings settings("Arena Tracker", "Arena Tracker");
    QString remindedVersion = settings.value("version", "").toString();

    emit pDebug("VERSION: " + VERSION + " - RemindedVersion: " + remindedVersion +
                " - LatestVersion: " + latestVersion + " - AllowedVersions: " + allowedVersions.join(","));

    if(remindedVersion.isEmpty())
    {
        remindedVersion = VERSION;
    }


    if(!allowedVersions.contains(VERSION))
    {
        emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
        emit pDebug("Arena Tracker " + latestVersion + " is available for download.");

        QMessageBox msgBox((QMainWindow*)this->parent());
        msgBox.setText("Arena Tracker " + latestVersion + " is available for download.");
        msgBox.setWindowTitle(tr("New version"));
        msgBox.setIcon(QMessageBox::Information);
        QPushButton *button1 = msgBox.addButton("Update", QMessageBox::ActionRole);
        msgBox.addButton("Exit", QMessageBox::ActionRole);

        msgBox.exec();

        if(msgBox.clickedButton() == button1)
        {
            downloadLatestVersion(versionJsonObject);
        }
        else
        {
            ((QMainWindow*)this->parent())->close();
        }
    }
    else if(remindedVersion != latestVersion)
    {
        if(VERSION == latestVersion)
        {
            settings.setValue("version", VERSION);
            emit pLog("Settings: Arena Tracker is up-to-date.");
            emit pDebug("Arena Tracker is up-to-date.");
            this->deleteLater();
        }
        else
        {
            emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
            emit pDebug("Arena Tracker " + latestVersion + " is available for download.");

            QMessageBox msgBox((QMainWindow*)this->parent());
            msgBox.setText("Arena Tracker " + latestVersion + " is available for download.");
            msgBox.setWindowTitle(tr("New version"));
            msgBox.setIcon(QMessageBox::Information);
            QPushButton *button1 = msgBox.addButton("Update", QMessageBox::ActionRole);
            QPushButton *button2 = msgBox.addButton("Remind me later", QMessageBox::ActionRole);
            QPushButton *button3 = msgBox.addButton("Don't remind me", QMessageBox::ActionRole);

            msgBox.exec();

            if(msgBox.clickedButton() == button1)
            {
                downloadLatestVersion(versionJsonObject);
            }
            else if(msgBox.clickedButton() == button2)
            {
                this->deleteLater();
            }
            else if(msgBox.clickedButton() == button3)
            {
                settings.setValue("version", latestVersion);
                this->deleteLater();
            }
        }
    }
    else if(VERSION != latestVersion)
    {
        emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
        emit pDebug("Arena Tracker " + latestVersion + " is available for download.");
        this->deleteLater();
    }
    else
    {
        emit pLog("Settings: Arena Tracker is up-to-date.");
        emit pDebug("Arena Tracker is up-to-date.");
        this->deleteLater();
    }
}


void VersionChecker::downloadLatestVersion(const QJsonObject &versionJsonObject)
{
    QString binaryUrl = "";

#ifdef Q_OS_WIN
    binaryUrl = versionJsonObject.value("windowsUrl").toString();
#endif

#ifdef Q_OS_MAC
        binaryUrl = versionJsonObject.value("macUrl").toString();
#endif

#ifdef Q_OS_LINUX
    #ifdef APPIMAGE
        binaryUrl = versionJsonObject.value("linuxAppUrl").toString();
    #else
        binaryUrl = versionJsonObject.value("linuxStaticUrl").toString();
    #endif
#endif

    if(!binaryUrl.isEmpty())
    {
        emit pDebug("New binary --> Download from: " + binaryUrl);
        networkManager->get(QNetworkRequest(QUrl(binaryUrl)));
    }
    else                        this->deleteLater();
}


void VersionChecker::saveRestart(const QByteArray &data)
{
    emit pDebug("New binary --> Download Success.");

    QString runningBinaryName = QCoreApplication::applicationFilePath().split("/").last();
    QString runningBinaryPath = Utility::appPath() + "/" + runningBinaryName;

    QFile appFile(runningBinaryPath);
    QFile::Permissions permissions = appFile.permissions();
    appFile.rename(Utility::dataPath() + "/ArenaTracker.old");

    Utility::dumpOnFile(data, Utility::dataPath() + "/binaryTemp.zip");
    Utility::unZip(Utility::dataPath() + "/binaryTemp.zip", Utility::appPath());
    QFile zipFile(Utility::dataPath() + "/binaryTemp.zip");
    zipFile.remove();

    QFile::setPermissions(runningBinaryPath, permissions);

    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    ((QMainWindow *)this->parent())->close();
}


void VersionChecker::removeOldVersion()
{
    QFile appOld(Utility::dataPath() + "/ArenaTracker.old");
    if(appOld.exists()) appOld.remove();
}
