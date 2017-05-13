#ifndef TRACKOBOTUPLOADER_H
#define TRACKOBOTUPLOADER_H

#include "utility.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#define TRACKOBOT_NEWUSER_URL "https://trackobot.com/users.json"

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
