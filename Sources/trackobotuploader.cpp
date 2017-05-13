#include "trackobotuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QtWidgets>

TrackobotUploader::TrackobotUploader(QObject *parent) : QObject(parent)
{
    username = token = "";
    connected = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QTimer::singleShot(1, this, SLOT(checkUserSettings()));
}


TrackobotUploader::~TrackobotUploader()
{
    delete networkManager;
}


bool TrackobotUploader::isConnected()
{
    return this->connected;
}


void TrackobotUploader::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed. Retrying...");
//        networkManager->get(QNetworkRequest(reply->url()));
    }
    else
    {
        QString fullUrl = reply->url().toString();
        QString endUrl = fullUrl.split("/").last();

        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New user settings --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::dataPath() + "/TrackobotUser.json");
            loadUserSettings();
        }
    }
}


void TrackobotUploader::checkUserSettings()
{
    QFileInfo file(Utility::dataPath() + "/TrackobotUser.json");

    if(file.exists())   loadUserSettings();
    else
    {
        emit pDebug("User settings missing --> Download from: " + QString(TRACKOBOT_NEWUSER_URL));
        networkManager->post(QNetworkRequest(QUrl(TRACKOBOT_NEWUSER_URL)), "");
    }
}


void TrackobotUploader::loadUserSettings()
{
    QFile file(Utility::dataPath() + "/TrackobotUser.json");
    if(!file.exists())
    {
        emit pDebug("ERROR: TrackobotUser.json doesn't exists.");
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("ERROR: Failed to open TrackobotUser.json");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonObject jsonUserObject = jsonDoc.object();
    this->username = jsonUserObject.value("username").toString();
    this->token = jsonUserObject.value("password").toString();
    emit pDebug("User settings loaded.");
}























