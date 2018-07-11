#include "twitchhandler.h"


QString TwitchHandler::oauth;
QString TwitchHandler::channel;
QString TwitchHandler::pickTag;


void TwitchHandler::loadSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");

    TwitchHandler::oauth = settings.value("twitchOauth", QString("oauth:knicqcnfdu5sixko3ezbkr9brv58vd")).toString();
    TwitchHandler::channel = settings.value("twitchChannel", QString("#supertriodo")).toString();
    TwitchHandler::pickTag = settings.value("twitchPickTag", QString("pick")).toString();
}


void TwitchHandler::setOauth(QString oauth)
{
    TwitchHandler::oauth = oauth;

    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("twitchOauth", oauth);
}


void TwitchHandler::setChannel(QString channel)
{
    TwitchHandler::channel = channel;

    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("twitchChannel", channel);
}


void TwitchHandler::setPickTag(QString pickTag)
{
    TwitchHandler::pickTag = pickTag;

    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("twitchPickTag", pickTag);
}


TwitchHandler::TwitchHandler(QObject *parent, bool testingConnection) : QObject(parent)
{
    reset();
    this->testingConnection = testingConnection;
    if(testingConnection)   QTimer::singleShot(10000, this, SLOT(endTestConnection()));

    connect(&twitchSocket, SIGNAL(connected()),
                this, SLOT(connected()));
    twitchSocket.open(QUrl("wss://irc-ws.chat.twitch.tv:443"));
}


TwitchHandler::~TwitchHandler()
{
    twitchSocket.close();
}


void TwitchHandler::endTestConnection()
{
    if(testingConnection)
    {
        emit connectionOk(false);
    }
}


void TwitchHandler::reset()
{
    participants.clear();
    for(int i = 0; i < 3; i++)  votes[i]=0;
}


void TwitchHandler::connected()
{
    qDebug()<<"CONNECTED!!!";
    connect(&twitchSocket, SIGNAL(textMessageReceived(QString)),
                this, SLOT(textMessageReceived(QString)));
    twitchSocket.sendTextMessage("PASS " + TwitchHandler::oauth);
    twitchSocket.sendTextMessage(QStringLiteral("NICK atbot"));
    twitchSocket.sendTextMessage("JOIN " + TwitchHandler::channel);

    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/membership"));
    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/tags"));
    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/commands"));

    twitchSocket.sendTextMessage("PRIVMSG " + TwitchHandler::channel + " :Bot conectado!!!");
}


void TwitchHandler::textMessageReceived(QString message)
{
    //TESTING CONNECTION
    if(testingConnection)
    {
        if(message.contains(":Welcome, GLHF!"))
        {
            this->testingConnection = false;
            emit connectionOk();
            return;
        }
    }

    qDebug()<<message;

    //PING
    if(message.contains("PING :tmi.twitch.tv"))
    {
        twitchSocket.sendTextMessage(QStringLiteral("PONG :tmi.twitch.tv"));
        qDebug()<<"PONG sent!";
    }

    //PICK TAG
    else if(message.contains(QRegularExpression("\\:(\\w+)!\\w*@\\w*\\.tmi\\.twitch\\.tv PRIVMSG " + TwitchHandler::channel +
                                           " :!" + TwitchHandler::pickTag + "([1-3])\\r\\n"), &match))
    {
        QString username = match.captured(1);
        int pick = match.captured(2).toInt() - 1;

        if(!participants.contains(username))
        {
            participants.append(username);
            votes[pick]++;
            emit voteUpdate(votes[pick], pick);
        }
    }
}
