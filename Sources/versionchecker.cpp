#include "versionchecker.h"
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtWidgets>

VersionChecker::VersionChecker(QObject *parent, bool patreonVersion) : QObject(parent)
{
    this->patreonVersion = patreonVersion;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    networkManager->get(QNetworkRequest(QUrl(VERSION_URL + QString("/version.json"))));

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
        this->deleteLater();

        QJsonArray versionArray = QJsonDocument::fromJson("{\"versionPatreon\": [],\"versionFree\": [\"v6.01\",\"v6.1\"]}"
                    /*reply->readAll()*/).object().value("versionFree").toArray();
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
            QPushButton *button1 = msgBox.addButton("Open web", QMessageBox::ActionRole);
            msgBox.addButton("Exit", QMessageBox::ActionRole);

            msgBox.exec();

            if(msgBox.clickedButton() == button1)
            {
                QDesktopServices::openUrl(QUrl("https://github.com/supertriodo/Arena-Tracker/releases/latest"));
            }
            ((QMainWindow*)this->parent())->close();
        }
        else if(remindedVersion != latestVersion)
        {
            if(VERSION == latestVersion)
            {
                settings.setValue("version", VERSION);
                emit pLog("Settings: Arena Tracker is up-to-date.");
                emit pDebug("Arena Tracker is up-to-date.");
            }
            else
            {
                emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
                emit pDebug("Arena Tracker " + latestVersion + " is available for download.");

                QMessageBox msgBox((QMainWindow*)this->parent());
                msgBox.setText("Arena Tracker " + latestVersion + " is available for download.");
                msgBox.setWindowTitle(tr("New version"));
                msgBox.setIcon(QMessageBox::Information);
                QPushButton *button1 = msgBox.addButton("Open in web", QMessageBox::ActionRole);
                QPushButton *button2 = msgBox.addButton("Remind me later", QMessageBox::ActionRole);
                QPushButton *button3 = msgBox.addButton("Don't remind me", QMessageBox::ActionRole);

                msgBox.exec();

                if(msgBox.clickedButton() == button1)
                {
                    QDesktopServices::openUrl(QUrl("https://github.com/supertriodo/Arena-Tracker/releases/latest"));
                }
                else if(msgBox.clickedButton() == button2)
                {
                }
                else if(msgBox.clickedButton() == button3)
                {
                    settings.setValue("version", latestVersion);
                }
            }
        }
        else if(VERSION != latestVersion)
        {
            emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
            emit pDebug("Arena Tracker " + latestVersion + " is available for download.");
        }
        else
        {
            emit pLog("Settings: Arena Tracker is up-to-date.");
            emit pDebug("Arena Tracker is up-to-date.");
        }
    }
}

