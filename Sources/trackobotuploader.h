#ifndef TRACKOBOTUPLOADER_H
#define TRACKOBOTUPLOADER_H

#include "utility.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#define TRACKOBOT_NEWUSER_URL "https://trackobot.com/users.json"
#define TRACKOBOT_LOGIN_URL "https://trackobot.com/profile/history.json"
#define TRACKOBOT_RESULTS_URL "https://trackobot.com/profile/results.json"
#define TRACKOBOT_PROFILE_URL "https://trackobot.com/one_time_auth.json"

class TrackobotUploader : public QObject
{
    Q_OBJECT
public:
    TrackobotUploader(QObject *parent);
    ~TrackobotUploader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QString username, token;
    bool connected;


//Metodos
private:
    void loadUserSettings();
    void tryConnect();

public:
    bool isConnected();

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="TrackobotUploader");

public slots:

private slots:
    void checkUserSettings();
    void replyFinished(QNetworkReply *reply);
};

#endif // TRACKOBOTUPLOADER_H
