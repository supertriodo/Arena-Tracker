#include "versionchecker.h"
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

VersionChecker::VersionChecker(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    networkManager->get(QNetworkRequest(QUrl(VERSION_URL)));

    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runVersion = settings.value("runVersion", VERSION).toString();
    newVersion = (runVersion != VERSION);
    settings.setValue("runVersion", VERSION);
    qApp->setApplicationVersion(VERSION);

    //ArenaTracker.new run - WIN 10 updater
    connect(&futureNewAppReplace, SIGNAL(finished()), this, SLOT(finishNewAppReplace()));
    if(isNewApp())    startNewAppReplace();
    removeOldNewVersion();
}


VersionChecker::~VersionChecker()
{
    delete networkManager;
}


void VersionChecker::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit advanceProgressBar(static_cast<int>(bytesTotal - bytesReceived), "Downloading " + latestVersion + "...");
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
            emit startProgressBar(1, "Downloading " + latestVersion + "...");
            QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(location)));
            connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                    this, SLOT(downloadProgress(qint64,qint64)));
        }

        //Check version
        else if(fullUrl == VERSION_URL)
        {
            checkUpdate(QJsonDocument::fromJson(reply->readAll()).object());
        }

        //New version downloaded
        else
        {
            emit showMessageProgressBar(latestVersion + " downloaded");
            saveRestart(reply->readAll());
        }
    }
}


void VersionChecker::checkUpdate(const QJsonObject &versionJsonObject)
{
    //AppImage version se baja siempre en el primer run para que el usuario ejecute el
    //AppImage de ~/Arena Tracker
    //Al reiniciar se crearan los shortcut
#ifdef Q_OS_LINUX
    #ifdef APPIMAGE
    QFile appFile(Utility::dataPath() + "/ArenaTracker.Linux.AppImage");
    if(!appFile.exists())
    {
        downloadLatestVersion(versionJsonObject);
        return;
    }
    #endif
#endif

    QJsonArray versionArray = versionJsonObject.value("versionFree").toArray();
    QStringList allowedVersions;
    for(QJsonValue value: versionArray)
    {
        allowedVersions.append(value.toString());
    }
    this->latestVersion = allowedVersions.isEmpty()?"":allowedVersions.last();

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

        QMessageBox msgBox(static_cast<QMainWindow*>(this->parent()));
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
            static_cast<QMainWindow*>(this->parent())->close();
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

            QMessageBox msgBox(static_cast<QMainWindow*>(this->parent()));
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

    if(newVersion && VERSION == latestVersion)  showVersionLog(versionJsonObject.value("log").toString());
}


void VersionChecker::showVersionLog(QString changesLog)
{
    QMessageBox msgBox(static_cast<QMainWindow*>(this->parent()));
    msgBox.setText(changesLog);
    msgBox.setWindowTitle(VERSION + " changes");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.addButton(QMessageBox::Ok);

    msgBox.exec();
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

#ifdef Q_OS_LINUX
    #ifdef APPIMAGE
        saveRestartAppImage(data);
        return;
    #endif
#endif

    QString runningBinaryName = QCoreApplication::applicationFilePath().split("/").last();
    QString runningBinaryPath = Utility::appPath() + "/" + runningBinaryName;

    QFile appFile(runningBinaryPath);
    QFile::Permissions permissions = appFile.permissions();

    emit pDebug(runningBinaryPath + " rename " + Utility::dataPath() + "/ArenaTracker.old");

    if(!appFile.rename(Utility::dataPath() + "/ArenaTracker.old"))
    {
        emit pDebug("Rename failed. Using ArenaTracker.new workaround for WIN10.");

        QString dataBinaryPath = Utility::dataPath() + "/" + runningBinaryName;

        Utility::dumpOnFile(data, Utility::dataPath() + "/binaryTemp.zip");
        Utility::unZip(Utility::dataPath() + "/binaryTemp.zip", Utility::dataPath());
        QFile zipFile(Utility::dataPath() + "/binaryTemp.zip");
        zipFile.remove();

        emit pDebug("Extract ArenaTracker on " + Utility::dataPath());

        QFile::setPermissions(dataBinaryPath, permissions);

        QFile appDataFile(dataBinaryPath);
        appDataFile.copy(Utility::dataPath() + "/ArenaTracker.new");

        emit pDebug("Copy downloaded ArenaTracker on " + Utility::dataPath() + "/ArenaTracker.new");

        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("runningBinaryPath", runningBinaryPath);

        emit pDebug("Start ArenaTracker.new...");

        QProcess::startDetached(Utility::dataPath() + "/ArenaTracker.new", qApp->arguments());
        static_cast<QMainWindow*>(this->parent())->close();
    }
    else
    {
        emit pDebug("Rename success.");

        Utility::dumpOnFile(data, Utility::dataPath() + "/binaryTemp.zip");
        Utility::unZip(Utility::dataPath() + "/binaryTemp.zip", Utility::appPath());
        QFile zipFile(Utility::dataPath() + "/binaryTemp.zip");
        zipFile.remove();

        emit pDebug("Extract ArenaTracker on " + Utility::appPath());

        QFile::setPermissions(runningBinaryPath, permissions);

        emit pDebug("Start downloaded ArenaTracker...");

        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        static_cast<QMainWindow*>(this->parent())->close();
    }
}


void VersionChecker::saveRestartAppImage(const QByteArray &data)
{
    QString runningBinaryName = "ArenaTracker.Linux.AppImage";
    QString runningBinaryPath = Utility::dataPath() + "/" + runningBinaryName;

    QFile appFile(runningBinaryPath);
    if(appFile.exists())    appFile.rename(Utility::dataPath() + "/ArenaTracker.old");

    Utility::dumpOnFile(data, runningBinaryPath);
    QFile::setPermissions(runningBinaryPath, QFileDevice::ExeOther|QFileDevice::ReadOther|
                          QFileDevice::ExeGroup|QFileDevice::ReadGroup|
                          QFileDevice::ExeUser|QFileDevice::WriteUser|QFileDevice::ReadUser|
                          QFileDevice::ExeOwner|QFileDevice::WriteOwner|QFileDevice::ReadOwner);

    QProcess::startDetached(runningBinaryPath, qApp->arguments());
    static_cast<QMainWindow*>(this->parent())->close();
}


void VersionChecker::removeOldNewVersion()
{
    QFile appOld(Utility::dataPath() + "/ArenaTracker.old");
    if(appOld.exists())
    {
        qDebug() << Utility::dataPath() + "/ArenaTracker.old" << "removed.";
        appOld.remove();
    }

    QFile appNew(Utility::dataPath() + "/ArenaTracker.new");
    if(appNew.exists())
    {
        qDebug() << Utility::dataPath() + "/ArenaTracker.new" << "removed.";
        appNew.remove();
    }
}


bool VersionChecker::isNewApp()
{
    QString runningBinaryName = QCoreApplication::applicationFilePath().split("/").last();
    return runningBinaryName == "ArenaTracker.new";
}


void VersionChecker::newAppReplace()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runningBinaryPath = settings.value("runningBinaryPath", "").toString();
    QString runningBinaryName = runningBinaryPath.split("/").last();
    QString dataBinaryPath = Utility::dataPath() + "/" + runningBinaryName;

    qDebug() << "ArenaTracker.new running...";

    QFile appFile(runningBinaryPath);
    if(!appFile.exists())   return;
    while(!appFile.remove())    QThread::sleep(1);

    qDebug() << runningBinaryPath << "removed.";

    QFile appDataFile(dataBinaryPath);
    while(!appDataFile.rename(runningBinaryPath))    QThread::sleep(1);

    qDebug() << dataBinaryPath << "renamed to" << runningBinaryPath;
}


void VersionChecker::startNewAppReplace()
{
    if(!futureNewAppReplace.isRunning()) futureNewAppReplace.setFuture(QtConcurrent::run(this, &VersionChecker::newAppReplace));
}
void VersionChecker::finishNewAppReplace()
{
    emit pDebug("ArenaTracker replaced by downloaded version.");
    emit showMessageProgressBar(VERSION + " ready to use");
}
