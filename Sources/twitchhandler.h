#ifndef TWITCHHANDLER_H
#define TWITCHHANDLER_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>


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
    QList<QString> participants;
    int votes[3];


//Metodos
public:
    static bool loadSettings();
    static void setOauth(QString oauth);
    static void setChannel(QString channel);
    static void setPickTag(QString pickTag);
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
    void voteUpdate(int vote1, int vote2, int vote3);

public slots:

private slots:
    void textMessageReceived(QString message);
    void connected();
    void endTestConnection();
};

#endif // TWITCHHANDLER_H
