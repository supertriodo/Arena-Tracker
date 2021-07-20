#ifndef TRACKOBOTUPLOADER_H
#define TRACKOBOTUPLOADER_H

#include "utility.h"
#include "gamewatcher.h"
#include "Widgets/ui_extended.h"
#include <QDateTime>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>


#define TRACKOBOT_NEWUSER_URL "https://trackobot.com/users.json"
#define TRACKOBOT_RESULTS_URL "https://trackobot.com/profile/results.json"
#define TRACKOBOT_PROFILE_URL "https://trackobot.com/one_time_auth.json"
#define TRACKOBOT_ACCOUNT_FILE "Account.track-o-bot"


class ArenaItem
{
public:
    QDateTime dateTime;
    GameResult gameResult;
};

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
    bool connectSuccess;


//Metodos
private:
    bool loadAccount(QString fileName="");
    void saveAccount();
    bool loadAccount(QByteArray jsonData);
    QString credentials();
    void saveSettings();
    void loadAccount(QString username, QString password);
    QString getRandomString(int rslength);

public:
    void checkAccount();
    bool isConnected();
    void openTBProfile();
    void uploadResult(GameResult gameResult, LoadingScreenState loadingScreen, qint64 startGameEpoch, QDateTime dateTime, QJsonArray cardHistory);
    QString getUsername();
    void importAccount(QString fileName);
    void createFakeAccount();

signals:
    void advanceProgressBar(int remaining, QString text);
    void startProgressBar(int maximum, QString text);
    void showMessageProgressBar(QString text);
    void connected(QString username, QString password);
    void disconnected();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="TrackobotUploader");

public slots:
    void importAccount(QByteArray jsonData);

private slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // TRACKOBOTUPLOADER_H
