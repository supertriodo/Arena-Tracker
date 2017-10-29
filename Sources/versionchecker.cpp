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

        QString latestVersion;
        if(patreonVersion)
        {
            latestVersion = QJsonDocument::fromJson(reply->readAll()).object().value("versionPatreon").toString();
            emit pDebug("Installed AT Patreon: " + VERSION + " - Latest AT Patreon: " + latestVersion);
        }
        else
        {
            latestVersion = QJsonDocument::fromJson(reply->readAll()).object().value("versionFree").toString();
            emit pDebug("Installed AT Free: " + VERSION + " - Latest AT Free: " + latestVersion);
        }

        if(VERSION >= latestVersion)
        {
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
            QPushButton *button1 = msgBox.addButton("Open web", QMessageBox::ActionRole);
            msgBox.addButton("Exit", QMessageBox::ActionRole);

            msgBox.exec();

            if(msgBox.clickedButton() == button1)
            {
                if(patreonVersion)
                {
                    QDesktopServices::openUrl(QUrl("https://www.patreon.com/triodo/posts"));
                }
                else
                {
                    QDesktopServices::openUrl(QUrl("https://github.com/supertriodo/Arena-Tracker/releases/latest"));
                }
            }
            ((QMainWindow*)this->parent())->close();
        }
    }
}


//Old update version dialog
//void VersionChecker::replyFinished(QNetworkReply *reply)
//{
//    reply->deleteLater();
//    this->deleteLater();

//    emit pLog("Settings: Arena Tracker " + VERSION);
//    emit pDebug("Arena Tracker " + VERSION);

//    QString target = reply->rawHeader("Location");
//    QRegularExpressionMatch match;
//    if(target.contains(QRegularExpression(
//        "^https://github.com/supertriodo/Arena-Tracker/releases/tag/(.*)$"), &match))
//    {
//        QString latestVersion = match.captured(1);

//        QSettings settings("Arena Tracker", "Arena Tracker");
//        QString remindedVersion = settings.value("version", "").toString();

//        emit pDebug("VERSION: " + VERSION + " - RemindedVersion: " + remindedVersion + " - LatestVersion: " + latestVersion);

//        if(remindedVersion.isEmpty())
//        {
//            remindedVersion = VERSION;
//        }

//        if(remindedVersion != latestVersion)
//        {
//            if(VERSION == latestVersion)
//            {
//                settings.setValue("version", VERSION);
//                emit pLog("Settings: Arena Tracker is up-to-date.");
//                emit pDebug("Arena Tracker is up-to-date.");
//            }
//            else
//            {
//                emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
//                emit pDebug("Arena Tracker " + latestVersion + " is available for download.");

//                QMessageBox msgBox((QMainWindow*)this->parent());
//                msgBox.setText("Arena Tracker " + latestVersion + " is available for download.");
//                msgBox.setWindowTitle(tr("New version"));
//                msgBox.setIcon(QMessageBox::Information);
//                QPushButton *button1 = msgBox.addButton("Open in web", QMessageBox::ActionRole);
//                QPushButton *button2 = msgBox.addButton("Remind me later", QMessageBox::ActionRole);
//                QPushButton *button3 = msgBox.addButton("Don't remind me", QMessageBox::ActionRole);

//                msgBox.exec();

//                if(msgBox.clickedButton() == button1)
//                {
//                    QDesktopServices::openUrl(QUrl(target));
//                }
//                else if(msgBox.clickedButton() == button2)
//                {
//                }
//                else if(msgBox.clickedButton() == button3)
//                {
//                    settings.setValue("version", latestVersion);
//                }
//            }
//        }
//        else if(VERSION != latestVersion)
//        {
//            emit pLog("Settings: Arena Tracker " + latestVersion + " is available for download.");
//            emit pDebug("Arena Tracker " + latestVersion + " is available for download.");
//        }
//        else
//        {
//            emit pLog("Settings: Arena Tracker is up-to-date.");
//            emit pDebug("Arena Tracker is up-to-date.");
//        }
//    }
//}
