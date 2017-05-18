#ifndef TRACKOBOTUPLOADER_H
#define TRACKOBOTUPLOADER_H

#include "utility.h"
#include "gamewatcher.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#define TRACKOBOT_NEWUSER_URL "https://trackobot.com/users.json"
#define TRACKOBOT_RESULTS_URL "https://trackobot.com/profile/results.json"
#define TRACKOBOT_PROFILE_URL "https://trackobot.com/one_time_auth.json"
#define TRACKOBOT_ACCOUNT_FILE "Account.track-o-bot"

class TrackobotUploader : public QObject
{
    Q_OBJECT
public:
    TrackobotUploader(QObject *parent);
    ~TrackobotUploader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QString username, password;
    bool connected;


//Metodos
private:
    bool loadAccount();
    void saveAccount();
    bool loadAccount(QByteArray jsonData);
    QString credentials();

public:
    bool isConnected();
    void openTBProfile();
    void uploadResult(GameResult gameResult, LoadingScreenState loadingScreen, qint64 startGameEpoch, QJsonArray cardHistory);
    QString getUsername();

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="TrackobotUploader");

public slots:

private slots:
    void checkAccount();
    void replyFinished(QNetworkReply *reply);
};

#endif // TRACKOBOTUPLOADER_H
