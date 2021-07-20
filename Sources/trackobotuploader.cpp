#include "trackobotuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QtWidgets>


TrackobotUploader::TrackobotUploader(QObject *parent) : QObject(parent)
{
    username = password = "";
    connectSuccess = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}


TrackobotUploader::~TrackobotUploader()
{
    delete networkManager;
}


bool TrackobotUploader::isConnected()
{
    return this->connectSuccess;
}


QString TrackobotUploader::getUsername()
{
    return this->username;
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
            importAccount(jsonData);
        }
        else if(fullUrl == TRACKOBOT_PROFILE_URL)
        {
            QString profileUrl = QJsonDocument::fromJson(reply->readAll()).object().value("url").toString();
            emit pDebug("Getting profile url success. Opening: " + profileUrl);
            QDesktopServices::openUrl(QUrl(profileUrl));
        }
        else if(fullUrl == TRACKOBOT_RESULTS_URL)
        {
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

    saveSettings();

    emit pDebug("New account " + this->username + " --> Saved.");
    emit showMessageProgressBar("New track-o-bot account");
}


void TrackobotUploader::saveSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("tbUsername", this->username);
    settings.setValue("tbPassword", this->password);
}


bool TrackobotUploader::loadAccount(QByteArray jsonData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonUserObject = jsonDoc.object();
    QString username, password;
    username = jsonUserObject.value("username").toString();
    password = jsonUserObject.value("password").toString();

    if(!username.isEmpty() && !password.isEmpty())
    {
        this->username = username;
        this->password = password;
        this->connectSuccess = true;
        emit pDebug("New account " + this->username + " --> Loaded.");
        emit connected(username, password);
        return true;
    }
    else
    {
        this->connectSuccess = false;
        emit pDebug(jsonData + " has an invalid format.");
        emit disconnected();
        return false;
    }
}


QString TrackobotUploader::getRandomString(int rslength)
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-");

   QString randomString;
   for(int i=0; i<rslength; i++)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}


void TrackobotUploader::createFakeAccount()
{
    emit pDebug("New fake account --> Created.");
    loadAccount(getRandomString(20), getRandomString(10));
    saveAccount();
}


void TrackobotUploader::importAccount(QString fileName)
{
    if(loadAccount(fileName))   saveAccount();
}


void TrackobotUploader::importAccount(QByteArray jsonData)
{
    if(loadAccount(jsonData))   saveAccount();
}


void TrackobotUploader::loadAccount(QString username, QString password)
{
    this->username = username;
    this->password = password;
    this->connectSuccess = true;
    emit pDebug("Account " + this->username + " --> Loaded.");
    emit connected(username, password);
}


bool TrackobotUploader::loadAccount(QString fileName)
{
    if(fileName.isEmpty())  fileName = Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return false;
    }

    QDataStream in(&file);
    QString username, password, webserviceUrl;
    in.setVersion(QDataStream::Qt_4_8);
    in >> username;
    in >> password;
    in >> webserviceUrl;

    if(!username.isEmpty() && !password.isEmpty())
    {
        loadAccount(username, password);
        return true;
    }
    else
    {
        this->connectSuccess = false;
        emit pDebug(fileName + " file has an invalid format.");
        emit showMessageProgressBar("Invalid track-o-bot account");
        emit disconnected();
        return false;
    }
}


void TrackobotUploader::checkAccount()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString username = settings.value("tbUsername", "").toString();
    QString password = settings.value("tbPassword", "").toString();
    QFileInfo file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);

    if(!username.isEmpty() && !password.isEmpty())
    {
        loadAccount(username, password);
        if(!file.exists())  saveAccount();
    }
    else
    {
        if(file.exists())
        {
            if(loadAccount())   saveSettings();
        }
        else
        {
            emit pDebug("No account");
//            emit pDebug("Account missing --> Download from: " + QString(TRACKOBOT_NEWUSER_URL));
//            QNetworkRequest request(QUrl(TRACKOBOT_NEWUSER_URL));
//            networkManager->post(request, "");
        }
    }
}


QString TrackobotUploader::credentials()
{
    return "Basic " + (this->username + ":" + this->password).toLatin1().toBase64();
}


void TrackobotUploader::openTBProfile()
{
//    QNetworkRequest request(QUrl(TRACKOBOT_PROFILE_URL));
//    request.setRawHeader( "Authorization", credentials().toLatin1());
//    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//    networkManager->post(request, "");
//    emit pDebug("Getting profile url...");
}


//Trackobot no acepta "demonhunter"
void TrackobotUploader::uploadResult(GameResult gameResult, LoadingScreenState loadingScreen,
                                     qint64 startGameEpoch, QDateTime dateTime, QJsonArray cardHistory)
{
    Q_UNUSED(gameResult);
    Q_UNUSED(loadingScreen);
    Q_UNUSED(startGameEpoch);
    Q_UNUSED(dateTime);
    Q_UNUSED(cardHistory);

//    QJsonObject result;
//    result["coin"]          = !gameResult.isFirst;
//    result["hero"]          = Utility::classLogNumber2classLName(gameResult.playerHero);
//    result["opponent"]      = Utility::classLogNumber2classLName(gameResult.enemyHero);
//    result["win"]           = gameResult.isWinner;
//    result["mode"]          = Utility::getLoadingScreenToString(loadingScreen).toLower();
//    result["duration"]      = QDateTime::currentMSecsSinceEpoch()/1000 - startGameEpoch;
//    result["added"]         = dateTime.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
//    result["card_history"]  = cardHistory;

//    QJsonObject params;
//    params[ "result" ] = result;
//    QByteArray data = QJsonDocument( params ).toJson();


//    QNetworkRequest request(QUrl(TRACKOBOT_RESULTS_URL));
//    request.setRawHeader( "Authorization", credentials().toLatin1());
//    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//    networkManager->post(request, data);
//    emit pDebug("Uploading result...");

//    QString text =  Utility::classLogNumber2classULName(gameResult.playerHero) + " vs " +
//                    Utility::classLogNumber2classULName(gameResult.enemyHero) + " uploaded";
//    emit showMessageProgressBar(text);
}





