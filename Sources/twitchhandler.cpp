#include "twitchhandler.h"


QString TwitchHandler::oauth;
QString TwitchHandler::channel;
QString TwitchHandler::pickTag;
bool TwitchHandler::wellConfigured;
bool TwitchHandler::active;


bool TwitchHandler::loadSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");

    TwitchHandler::oauth = settings.value("twitchOauth", QString("oauth:xxxxxxxxxx")).toString();
    TwitchHandler::channel = settings.value("twitchChannel", QString("")).toString();
    TwitchHandler::pickTag = settings.value("twitchPickTag", QString("!pick")).toString();

    TwitchHandler::wellConfigured = false;

    return !(TwitchHandler::oauth.isEmpty() || TwitchHandler::channel.isEmpty());
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


QString TwitchHandler::getOauth()
{
    return TwitchHandler::oauth;
}


QString TwitchHandler::getChannel()
{
    return TwitchHandler::channel;
}


QString TwitchHandler::getPickTag()
{
    return TwitchHandler::pickTag;
}


void TwitchHandler::setActive(bool active)
{
    TwitchHandler::active = active;
}


bool TwitchHandler::isActive()
{
    return TwitchHandler::active;
}


bool TwitchHandler::isWellConfigured()
{
    return TwitchHandler::wellConfigured;
}


TwitchHandler::TwitchHandler(QObject *parent) : QObject(parent)
{
    reset();
    this->connectionOk_ = false;
    QTimer::singleShot(10000, this, SLOT(endTestConnection()));

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
    if(!connectionOk_)
    {
        TwitchHandler::wellConfigured = false;
        emit connectionOk(false);
    }
}


void TwitchHandler::reset()
{
    participants.clear();
    for(int i = 0; i < 3; i++)  votes[i]=0;
    emit pDebug("\nReset");
}


void TwitchHandler::connected()
{
    connect(&twitchSocket, SIGNAL(textMessageReceived(QString)),
                this, SLOT(textMessageReceived(QString)));
    twitchSocket.sendTextMessage("PASS " + TwitchHandler::oauth);
    twitchSocket.sendTextMessage(QStringLiteral("NICK atbot"));
    twitchSocket.sendTextMessage("JOIN " + TwitchHandler::channel);

    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/membership"));
    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/tags"));
    twitchSocket.sendTextMessage(QStringLiteral("CAP REQ :twitch.tv/commands"));
}


void TwitchHandler::sendMessage(QString msg)
{
    twitchSocket.sendTextMessage("PRIVMSG " + TwitchHandler::channel + " :" + msg);
}


bool TwitchHandler::isConnectionOk()
{
    return this->connectionOk_;
}


void TwitchHandler::textMessageReceived(QString message)
{
    //TESTING CONNECTION
    if(message.contains(":Welcome, GLHF!"))
    {
        TwitchHandler::wellConfigured = true;
        this->connectionOk_ = true;
        emit connectionOk();
        return;
    }

    //PING
    if(message.contains("PING :tmi.twitch.tv"))
    {
        twitchSocket.sendTextMessage(QStringLiteral("PONG :tmi.twitch.tv"));
    }

    //PICK TAG
    else if(message.contains(QRegularExpression("\\:(\\w+)!\\w*@\\w*\\.tmi\\.twitch\\.tv PRIVMSG " + TwitchHandler::channel +
                                           " :" + TwitchHandler::pickTag + "([1-3])\\r\\n"), &match))
    {
        emit pDebug(match.captured(0).trimmed());

        QString username = match.captured(1);
        int pick = match.captured(2).toInt() - 1;

        QString debugStr = username + " pick " + QString::number(pick+1);

        //Eliminamos su voto anterior
        if(participants.contains(username))
        {
            int oldPick = participants[username];

            if(oldPick>=0 && oldPick<3)
            {
                votes[oldPick]--;
            }

            debugStr += " from " + QString::number(oldPick+1);
        }

        participants[username] = pick;
        votes[pick]++;

        emit pDebug(debugStr);
        emit pDebug("Participants(" + QString::number(participants.count()) + "): " +
                    QString::number(votes[0]) + " - " + QString::number(votes[1]) + " - " + QString::number(votes[2]));

        emit voteUpdate(votes[0], votes[1], votes[2], username);
    }
}
