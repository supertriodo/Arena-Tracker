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

    QString fullUrl = reply->url().toString();
    QString noParamsUrl = fullUrl.split("?").first();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed.");

        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New user settings --> Download failed.");
        }
        else if(noParamsUrl == TRACKOBOT_RESULTS_URL)
        {
            emit pDebug("Upload Results failed.");
            tryConnect();
        }
        else if(noParamsUrl == TRACKOBOT_LOGIN_URL)
        {
            emit pDebug("Login failed.");
        }
    }
    else
    {
        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New user settings --> Download success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::dataPath() + "/TrackobotUser.json");
            loadUserSettings();
        }
        else if(noParamsUrl == TRACKOBOT_RESULTS_URL)
        {
            emit pDebug("Upload Results success.");
        }
        else if(noParamsUrl == TRACKOBOT_LOGIN_URL)
        {
            this->connected = true;
            emit pDebug("Login success.");
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
    tryConnect();
}


void TrackobotUploader::tryConnect()
{
    delete networkManager->cookieJar();
    networkManager->setCookieJar(new QNetworkCookieJar());
    networkManager->get(QNetworkRequest(QUrl(TRACKOBOT_LOGIN_URL + QString("?username=") + this->username +
                                             QString("&token=") + this->token)));

    this->connected = false;
    emit pDebug("Checking connection...");
}




















