#include "trackobotuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QtWidgets>

TrackobotUploader::TrackobotUploader(QObject *parent) : QObject(parent)
{
    username = password = "";
    connected = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QTimer::singleShot(1, this, SLOT(checkAccount()));
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

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed.");

        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New account --> Download failed.");
        }
        else if(fullUrl == TRACKOBOT_PROFILE_URL)
        {
            emit pDebug("Getting profile url failed.");
        }
        else if(fullUrl == TRACKOBOT_RESULTS_URL)
        {
            emit pDebug("Upload Results failed.");
        }
    }
    else
    {
        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New account --> Download success.");
            QByteArray jsonData = reply->readAll();
            if(loadAccount(jsonData))   saveAccount();
        }
        else if(fullUrl == TRACKOBOT_PROFILE_URL)
        {
            QString profileUrl = QJsonDocument::fromJson(reply->readAll()).object().value("url").toString();
            emit pDebug("Getting profile url success. Opening: " + profileUrl);
            QDesktopServices::openUrl(QUrl(profileUrl));
        }
        else if(fullUrl == TRACKOBOT_RESULTS_URL)
        {
            qDebug()<<reply->readAll();
            emit pDebug("Upload Results success.");
        }
    }
}


void TrackobotUploader::saveAccount()
{
    QFile file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);
    if(!file.open(QIODevice::WriteOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return;
    }

    QDataStream out(&file);
    out.setVersion( QDataStream::Qt_4_8 );
    out << this->username;
    out << this->password;
    out << "https://trackobot.com";
    emit pDebug("New account " + this->username + " --> Saved.");
}


bool TrackobotUploader::loadAccount(QByteArray jsonData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonUserObject = jsonDoc.object();
    this->username = jsonUserObject.value("username").toString();
    this->password = jsonUserObject.value("password").toString();

    if(!username.isEmpty() && !password.isEmpty())
    {
        emit pDebug("New account " + this->username + " --> Loaded.");
        this->connected = true;
    }
    else
    {
        emit pDebug(jsonData + " has an invalid format.");
        this->connected = false;
    }

    return this->connected;
}


bool TrackobotUploader::loadAccount()
{
    QFile file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return false;
    }

    QDataStream in(&file);
    QString webserviceUrl;
    in.setVersion(QDataStream::Qt_4_8);
    in >> this->username;
    in >> this->password;
    in >> webserviceUrl;

    if(!username.isEmpty() && !password.isEmpty())
    {
        emit pDebug("Account " + this->username + " --> Loaded.");
        this->connected = true;
    }
    else
    {
        emit pDebug(QString(TRACKOBOT_ACCOUNT_FILE) + " file has an invalid format.");
        this->connected = false;
    }

    return this->connected;
}


void TrackobotUploader::checkAccount()
{
    QFileInfo file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);

    if(file.exists())   loadAccount();
    else
    {
        emit pDebug("Account missing --> Download from: " + QString(TRACKOBOT_NEWUSER_URL));
        QNetworkRequest request(QUrl(TRACKOBOT_NEWUSER_URL));
        networkManager->post(request, "");
    }
}


QString TrackobotUploader::credentials()
{
    return "Basic " + (this->username + ":" + this->password).toLatin1().toBase64();
}


void TrackobotUploader::openTBProfile()
{
    QNetworkRequest request(QUrl(TRACKOBOT_PROFILE_URL));
    request.setRawHeader( "Authorization", credentials().toLatin1());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, "");
    emit pDebug("Getting profile url...");
}


void TrackobotUploader::uploadResult(GameResult gameResult, LoadingScreenState loadingScreen, qint64 startGameEpoch)
{
    QJsonObject result;
    result[ "coin" ]     = !gameResult.isFirst;//true;//( order == ORDER_SECOND );*
    result[ "hero" ]     = Utility::heroStringFromLogNumber(gameResult.playerHero).toLower();//"priest";//CLASS_NAMES[ hero ];*
    result[ "opponent" ] = Utility::heroStringFromLogNumber(gameResult.enemyHero).toLower();//"mage";//CLASS_NAMES[ opponent ];*
    result[ "win" ]      = gameResult.isWinner;//true;//( outcome == OUTCOME_VICTORY );*
    result[ "mode" ]     = Utility::getLoadingScreenString(loadingScreen).toLower();//modeString(loadingScreen);//"arena";//MODE_NAMES[ mode ];*
    result[ "duration" ] = QDateTime::currentSecsSinceEpoch() - startGameEpoch;//300;//duration;
    result[ "added" ]    = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);

//    QJsonArray card_history;
//    QJsonObject item;
//    item[ "turn" ] = 1;
//    item[ "player" ] = "me";
//    item[ "card_id" ] = MANA_BIND;
//    card_history.append(item);

//    item[ "turn" ] = 1;
//    item[ "player" ] = "opponent";
//    item[ "card_id" ] = SPREADING_MADNESS;
//    card_history.append(item);
//    result[ "card_history" ] = card_history;

    QJsonObject params;
    params[ "result" ] = result;
    QByteArray data = QJsonDocument( params ).toJson();


    QNetworkRequest request(QUrl(TRACKOBOT_RESULTS_URL));
    request.setRawHeader( "Authorization", credentials().toLatin1());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, data);
    emit pDebug("Uploading result...");
}


















