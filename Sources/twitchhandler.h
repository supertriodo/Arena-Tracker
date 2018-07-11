#ifndef TWITCHHANDLER_H
#define TWITCHHANDLER_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>


class TwitchHandler : public QObject
{
    Q_OBJECT

//Constructor
public:
    TwitchHandler(QObject *parent, bool testingConnection=true);
    ~TwitchHandler();

//Variables
private:
    static QString oauth, channel, pickTag;

    QWebSocket twitchSocket;
    bool testingConnection;
    QRegularExpressionMatch match;
    QList<QString> participants;
    int votes[3];


//Metodos
public:
    static void loadSettings();
    static void setOauth(QString oauth);
    static void setChannel(QString channel);
    static void setPickTag(QString pickTag);
    void reset();

signals:
    void connectionOk(bool ok=true);
    void voteUpdate(int votes, int pick);

public slots:

private slots:
    void textMessageReceived(QString message);
    void connected();
    void endTestConnection();
};

#endif // TWITCHHANDLER_H
