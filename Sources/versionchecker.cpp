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

    networkManager->get(QNetworkRequest(QUrl("https://github.com/supertriodo/Arena-Tracker/releases/latest")));

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
    this->deleteLater();

    emit pLog("Settings: Arena Tracker " + VERSION);
    emit pDebug("Arena Tracker " + VERSION);

    QString target = reply->rawHeader("Location");
    QRegularExpressionMatch match;
    if(target.contains(QRegularExpression(
        "^https://github.com/supertriodo/Arena-Tracker/releases/tag/(.*)$"), &match))
    {
        QString latestVersion = match.captured(1);

        QSettings settings("Arena Tracker", "Arena Tracker");
        QString remindedVersion = settings.value("version", "").toString();

        emit pDebug("VERSION: " + VERSION + " - RemindedVersion: " + remindedVersion + " - LatestVersion: " + latestVersion);

        if(remindedVersion.isEmpty())
        {
            remindedVersion = VERSION;
        }

        if(remindedVersion != latestVersion)
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
                    QDesktopServices::openUrl(QUrl(target));
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
