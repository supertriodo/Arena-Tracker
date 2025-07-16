#include "winratesdownloader.h"
#include "Widgets/scorebutton.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include "qnetworkreply.h"
#include <QtWidgets>

WinratesDownloader::WinratesDownloader(QObject *parent) : QObject(parent)
{
    hsrPickratesMap = nullptr;
    hsrWRMap = nullptr;
    hsrSamplesMap = nullptr;
    hsrPlayedWRMap = nullptr;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

WinratesDownloader::~WinratesDownloader()
{
    delete networkManager;

    //Delete HSR maps
    if(hsrPickratesMap != nullptr)        delete[] hsrPickratesMap;
    if(hsrWRMap != nullptr) delete[] hsrWRMap;
    if(hsrSamplesMap != nullptr)    delete[] hsrSamplesMap;
    if(hsrPlayedWRMap != nullptr)   delete[] hsrPlayedWRMap;
}


void WinratesDownloader::waitFinishThreads()
{
    if(futureProcessHSRCardsPickrates.isRunning())          futureProcessHSRCardsPickrates.waitForFinished();
    if(futureProcessHSRCardsIncludedWinrates.isRunning())   futureProcessHSRCardsIncludedWinrates.waitForFinished();
    if(futureProcessHSRCardsIncludedDecks.isRunning())      futureProcessHSRCardsIncludedDecks.waitForFinished();
    if(futureProcessHSRCardsPlayedWinrates.isRunning())     futureProcessHSRCardsPlayedWinrates.waitForFinished();
}


void WinratesDownloader::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    QString fullUrl = reply->url().toString();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed. Retrying...");

        if(fullUrl == HSR_CARDS_14DAYS)
        {
            emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_EXP));
            networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_EXP)));
        }
        else if(fullUrl == HSR_CARDS_EXP)
        {
            emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_PATCH));
            networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_PATCH)));
        }
        else if(fullUrl == HSR_CARDS_PATCH)
        {
            localHSRCards();
        }
        else if(fullUrl == HSR_HEROES_WINRATE)
        {
            localHSRHeroesWinrate();
        }
        else
        {
            networkManager->get(QNetworkRequest(reply->url()));
        }
    }
    else
    {
        //HSR Heroes Winrate
        if(fullUrl == HSR_HEROES_WINRATE)
        {
            emit pDebug("Heroes winrate --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/HSRheroes.json");
            processHSRHeroesWinrate(QJsonDocument::fromJson(jsonData).object());
        }
        //HSR Cards Pickrate/Winrate
        else if(fullUrl == HSR_CARDS_PATCH || fullUrl == HSR_CARDS_EXP || fullUrl == HSR_CARDS_14DAYS)
        {
            emit pDebug("HSR cards --> Download Success from: " + fullUrl);
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/HSRcards.json");
            startProcessHSRCards(QJsonDocument::fromJson(jsonData).object());
        }
    }
}


void WinratesDownloader::showHSRdataProgressBar()
{
    HSRdataThreads--;
    emit advanceProgressBar(HSRdataThreads);
    if(HSRdataThreads == 0) emit showMessageProgressBar("HSR data ready");
}


void WinratesDownloader::initHSRHeroesWinrate()
{
    QFileInfo fi(Utility::extraPath() + "/HSRheroes.json");
    if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
    {
        localHSRHeroesWinrate();
    }
    else
    {
        emit pDebug("Heroes winrate --> Download from: " + QString(HSR_HEROES_WINRATE));
        networkManager->get(QNetworkRequest(QUrl(HSR_HEROES_WINRATE)));
    }
}


void WinratesDownloader::localHSRHeroesWinrate()
{
    emit pDebug("Heroes winrate --> Use local HSRheroes.json");

    QFile file(Utility::extraPath() + "/HSRheroes.json");
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("ERROR: Failed to open HSRheroes.json");
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    processHSRHeroesWinrate(QJsonDocument::fromJson(jsonData).object());
}


void WinratesDownloader::processHSRHeroesWinrate(const QJsonObject &jsonObject)
{
    float heroScores[NUM_HEROS];
    QJsonObject data = jsonObject.value("series").toObject().value("data").toObject();

    for(const QString &key: (const QStringList)data.keys())
    {
        for(const QJsonValue &gameWinrate: (const QJsonArray)data.value(key).toArray())
        {
            QJsonObject gameWinrateObject = gameWinrate.toObject();
            if(gameWinrateObject.value("game_type").toInt() == 3)
            {
                int classOrder = Utility::className2classOrder(key);
                if(classOrder!=-1)
                {
                    heroScores[classOrder] = round(gameWinrateObject.value("win_rate").toDouble() * 10)/10.0;
                }
            }
        }
    }

    ScoreButton::setHeroScores(heroScores);
}


void WinratesDownloader::initHSRCards()
{
    connect(&futureProcessHSRCardsPickrates, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (Pickrates) --> Thread end.");
                this->hsrPickratesMap = futureProcessHSRCardsPickrates.result();
                emit readyHSRPickratesMap(hsrPickratesMap);
                showHSRdataProgressBar();
            }
            );

    connect(&futureProcessHSRCardsIncludedWinrates, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (IncludedWinrate) --> Thread end.");
                this->hsrWRMap = futureProcessHSRCardsIncludedWinrates.result();
                emit readyHSRWRMap(hsrWRMap);
                showHSRdataProgressBar();
            }
            );

    connect(&futureProcessHSRCardsIncludedDecks, &QFutureWatcher<QMap<QString, int> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (TimesPlayed) --> Thread end.");
                this->hsrSamplesMap = futureProcessHSRCardsIncludedDecks.result();
                emit readyHSRSamplesMap(hsrSamplesMap);
                showHSRdataProgressBar();
            }
            );

    connect(&futureProcessHSRCardsPlayedWinrates, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (PlayedWinrate) --> Thread end.");
                this->hsrPlayedWRMap = futureProcessHSRCardsPlayedWinrates.result();
                emit readyHSRPlayedWRMap(hsrPlayedWRMap);
                showHSRdataProgressBar();
            }
            );

    QFileInfo fi(Utility::extraPath() + "/HSRcards.json");
    if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
    {
        localHSRCards();
    }
    else
    {
        emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_14DAYS));
        networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_14DAYS)));
    }
}


void WinratesDownloader::localHSRCards()
{
    emit pDebug("HSR cards --> Use local HSRcards.json");

    QFile file(Utility::extraPath() + "/HSRcards.json");
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("ERROR: Failed to open HSRcards.json");
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    startProcessHSRCards(QJsonDocument::fromJson(jsonData).object());
}


void WinratesDownloader::processHSRCardClassDouble(const QJsonArray &jsonArray, const QString &tag, QMap<QString, float> &cardsMap, bool trunk)
{
    for(const QJsonValue &card: jsonArray)
    {
        QJsonObject cardObject = card.toObject();
        QString code = Utility::getCodeFromCardAttribute("dbfId", cardObject.value("dbf_id"));
        double value = cardObject.value(tag).toDouble();
        if(trunk)   value = round(value * 10)/10.0;
        cardsMap[code] = static_cast<float>(value);
    }
}


void WinratesDownloader::processHSRCardClassInt(const QJsonArray &jsonArray, const QString &tag, QMap<QString, int> &cardsMap)
{
    for(const QJsonValue &card: jsonArray)
    {
        QJsonObject cardObject = card.toObject();
        QString code = Utility::getCodeFromCardAttribute("dbfId", cardObject.value("dbf_id"));
        cardsMap[code] = cardObject.value(tag).toInt();
    }
}


void WinratesDownloader::startProcessHSRCards(const QJsonObject &jsonObject)
{
    if(futureProcessHSRCardsPickrates.isRunning() || futureProcessHSRCardsIncludedWinrates.isRunning() ||
        futureProcessHSRCardsIncludedDecks.isRunning() || futureProcessHSRCardsPlayedWinrates.isRunning())   return;

    const QJsonObject &data = jsonObject.value("series").toObject().value("data").toObject();

    QFuture<QMap<QString, float> *> future1 = QtConcurrent::run([this,data]()->QMap<QString, float> *{
        QMap<QString, float> * map = new QMap<QString, float>[NUM_HEROS];
        //--------------------------------------------------------
        //----NEW HERO CLASS
        //--------------------------------------------------------
        processHSRCardClassDouble(data.value("DEATHKNIGHT").toArray(), "included_popularity", map[DEATHKNIGHT]);
        processHSRCardClassDouble(data.value("DEMONHUNTER").toArray(), "included_popularity", map[DEMONHUNTER]);
        processHSRCardClassDouble(data.value("DRUID").toArray(), "included_popularity", map[DRUID]);
        processHSRCardClassDouble(data.value("HUNTER").toArray(), "included_popularity", map[HUNTER]);
        processHSRCardClassDouble(data.value("MAGE").toArray(), "included_popularity", map[MAGE]);
        processHSRCardClassDouble(data.value("PALADIN").toArray(), "included_popularity", map[PALADIN]);
        processHSRCardClassDouble(data.value("PRIEST").toArray(), "included_popularity", map[PRIEST]);
        processHSRCardClassDouble(data.value("ROGUE").toArray(), "included_popularity", map[ROGUE]);
        processHSRCardClassDouble(data.value("SHAMAN").toArray(), "included_popularity", map[SHAMAN]);
        processHSRCardClassDouble(data.value("WARLOCK").toArray(), "included_popularity", map[WARLOCK]);
        processHSRCardClassDouble(data.value("WARRIOR").toArray(), "included_popularity", map[WARRIOR]);
        return map;
    });
    futureProcessHSRCardsPickrates.setFuture(future1);

    QFuture<QMap<QString, float> *> future2 = QtConcurrent::run([this,data]()->QMap<QString, float> *{
        QMap<QString, float> * map = new QMap<QString, float>[NUM_HEROS];
        processHSRCardClassDouble(data.value("DEATHKNIGHT").toArray(), "included_winrate", map[DEATHKNIGHT], true);
        processHSRCardClassDouble(data.value("DEMONHUNTER").toArray(), "included_winrate", map[DEMONHUNTER], true);
        processHSRCardClassDouble(data.value("DRUID").toArray(), "included_winrate", map[DRUID], true);
        processHSRCardClassDouble(data.value("HUNTER").toArray(), "included_winrate", map[HUNTER], true);
        processHSRCardClassDouble(data.value("MAGE").toArray(), "included_winrate", map[MAGE], true);
        processHSRCardClassDouble(data.value("PALADIN").toArray(), "included_winrate", map[PALADIN], true);
        processHSRCardClassDouble(data.value("PRIEST").toArray(), "included_winrate", map[PRIEST], true);
        processHSRCardClassDouble(data.value("ROGUE").toArray(), "included_winrate", map[ROGUE], true);
        processHSRCardClassDouble(data.value("SHAMAN").toArray(), "included_winrate", map[SHAMAN], true);
        processHSRCardClassDouble(data.value("WARLOCK").toArray(), "included_winrate", map[WARLOCK], true);
        processHSRCardClassDouble(data.value("WARRIOR").toArray(), "included_winrate", map[WARRIOR], true);
        return map;
    });
    futureProcessHSRCardsIncludedWinrates.setFuture(future2);
    QFuture<QMap<QString, int> *> future3 = QtConcurrent::run([this,data]()->QMap<QString, int> *{
        QMap<QString, int> * map = new QMap<QString, int>[NUM_HEROS];
        processHSRCardClassInt(data.value("DEATHKNIGHT").toArray(), "times_played", map[DEATHKNIGHT]);
        processHSRCardClassInt(data.value("DEMONHUNTER").toArray(), "times_played", map[DEMONHUNTER]);
        processHSRCardClassInt(data.value("DRUID").toArray(), "times_played", map[DRUID]);
        processHSRCardClassInt(data.value("HUNTER").toArray(), "times_played", map[HUNTER]);
        processHSRCardClassInt(data.value("MAGE").toArray(), "times_played", map[MAGE]);
        processHSRCardClassInt(data.value("PALADIN").toArray(), "times_played", map[PALADIN]);
        processHSRCardClassInt(data.value("PRIEST").toArray(), "times_played", map[PRIEST]);
        processHSRCardClassInt(data.value("ROGUE").toArray(), "times_played", map[ROGUE]);
        processHSRCardClassInt(data.value("SHAMAN").toArray(), "times_played", map[SHAMAN]);
        processHSRCardClassInt(data.value("WARLOCK").toArray(), "times_played", map[WARLOCK]);
        processHSRCardClassInt(data.value("WARRIOR").toArray(), "times_played", map[WARRIOR]);
        return map;
        });
    futureProcessHSRCardsIncludedDecks.setFuture(future3);
    QFuture<QMap<QString, float> *> future4 = QtConcurrent::run([this,data]()->QMap<QString, float> *{
        QMap<QString, float> * map = new QMap<QString, float>[NUM_HEROS];
        processHSRCardClassDouble(data.value("DEATHKNIGHT").toArray(), "winrate_when_played", map[DEATHKNIGHT], true);
        processHSRCardClassDouble(data.value("DEMONHUNTER").toArray(), "winrate_when_played", map[DEMONHUNTER], true);
        processHSRCardClassDouble(data.value("DRUID").toArray(), "winrate_when_played", map[DRUID], true);
        processHSRCardClassDouble(data.value("HUNTER").toArray(), "winrate_when_played", map[HUNTER], true);
        processHSRCardClassDouble(data.value("MAGE").toArray(), "winrate_when_played", map[MAGE], true);
        processHSRCardClassDouble(data.value("PALADIN").toArray(), "winrate_when_played", map[PALADIN], true);
        processHSRCardClassDouble(data.value("PRIEST").toArray(), "winrate_when_played", map[PRIEST], true);
        processHSRCardClassDouble(data.value("ROGUE").toArray(), "winrate_when_played", map[ROGUE], true);
        processHSRCardClassDouble(data.value("SHAMAN").toArray(), "winrate_when_played", map[SHAMAN], true);
        processHSRCardClassDouble(data.value("WARLOCK").toArray(), "winrate_when_played", map[WARLOCK], true);
        processHSRCardClassDouble(data.value("WARRIOR").toArray(), "winrate_when_played", map[WARRIOR], true);
        return map;
    });
    futureProcessHSRCardsPlayedWinrates.setFuture(future4);

    HSRdataThreads = 4;
    emit startProgressBar(HSRdataThreads, "Building HSR data...");
}



