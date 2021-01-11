#ifndef TWITCHHANDLER_H
#define TWITCHHANDLER_H

#include "utility.h"
#include <QObject>
#include <QtWebSockets/QtWebSockets>

#define MAX_ATTEMPTS 3
#define ATTEMP_DELAY 5000


class TwitchHandler : public QObject
{
    Q_OBJECT

//Constructor
public:
    TwitchHandler(QObject *parent);
    ~TwitchHandler();

//Variables
private:
    static QString oauth, channel, pickTag;
    static bool wellConfigured, active;

    QWebSocket twitchSocket;
    bool connectionOk_;
    QRegularExpressionMatch match;
    QMap<QString, int> participants;
    int votes[3];
    int attempts;


//Metodos
public:
    static bool loadSettings();
    static void setOauth(QString oauth);
    static void setChannel(QString channel);
    static void setPickTag(QString pickTag);
    static void setWellConfigured(bool wellConfigured);
    static QString getOauth();
    static QString getChannel();
    static QString getPickTag();
    static void setActive(bool active);
    static bool isActive();
    static bool isWellConfigured();
    void reset();
    bool isConnectionOk();
    void sendMessage(QString msg);

signals:
    void connectionOk(bool ok=true);
    void voteUpdate(int vote1, int vote2, int vote3, QString username);
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="TwitchHandler");

public slots:

private slots:
    void textMessageReceived(QString message);
    void connected();
    void endTestConnection();
};

#endif // TWITCHHANDLER_H
