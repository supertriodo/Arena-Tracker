#include "winratesdownloader.h"
#include "Widgets/scorebutton.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include "qnetworkreply.h"
#include <QtWidgets>

WinratesDownloader::WinratesDownloader(QObject *parent) : QObject(parent)
{
    HSRdataThreads = fireDataThreads = 0;
    hsrPickratesMap = nullptr;
    hsrWRMap = nullptr;
    hsrSamplesMap = nullptr;
    hsrPlayedWRMap = nullptr;

    fireWRMap = new QMap<QString, float>[NUM_HEROS];
    fireSamplesMap = new QMap<QString, int>[NUM_HEROS];

    match = new QRegularExpressionMatch();

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

WinratesDownloader::~WinratesDownloader()
{
    delete networkManager;
    delete match;

    //Delete HSR maps
    if(hsrPickratesMap != nullptr)  delete[] hsrPickratesMap;
    if(hsrWRMap != nullptr)         delete[] hsrWRMap;
    if(hsrSamplesMap != nullptr)    delete[] hsrSamplesMap;
    if(hsrPlayedWRMap != nullptr)   delete[] hsrPlayedWRMap;

    //Delete Fire maps
    if(fireWRMap != nullptr)        delete[] fireWRMap;
    if(fireSamplesMap != nullptr)   delete[] fireSamplesMap;
}


void WinratesDownloader::waitFinishThreads()
{
    if(futureHSRPickrates.isRunning())  futureHSRPickrates.waitForFinished();
    if(futureHSRWR.isRunning())         futureHSRWR.waitForFinished();
    if(futureHSRSamples.isRunning())    futureHSRSamples.waitForFinished();
    if(futureHSRPlayedWR.isRunning())   futureHSRPlayedWR.waitForFinished();

    for(int i=0; i<NUM_HEROS; i++)
    {
        if(futureFire[i].isRunning())   futureFire[i].waitForFinished();
    }
}


int WinratesDownloader::url2classOrder(QString url)
{
    int classOrder = -1;
    if(url.contains(QRegularExpression(QString(FIRE_CARDS) + "(\\w+)\\.gz\\.json"), match))
    {
        QString hero = match->captured(1);
        classOrder = Utility::className2classOrder(hero);
    }
    return classOrder;
}


void WinratesDownloader::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    QString fullUrl = reply->url().toString();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed.");

        if(fullUrl.startsWith(FIRE_CARDS))
        {
            int classOrder = url2classOrder(fullUrl);
            if(classOrder == -1)    emit pDebug("ERROR: Fail retrieving class from url:" + fullUrl);
            else                    localFireCards(classOrder);
        }
        else if(fullUrl == HSR_CARDS_14DAYS)
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
            emit pDebug(reply->url().toString() + " --> Failed. Retrying...");
            networkManager->get(QNetworkRequest(reply->url()));
        }
    }
    else
    {
        //Fire Cards Winrate/Samples
        if(fullUrl.startsWith(FIRE_CARDS))
        {
            emit pDebug("Fire cards --> Download Success from: " + fullUrl);

            int classOrder = url2classOrder(fullUrl);
            if(classOrder == -1)
            {
                emit pDebug("ERROR: Fail retrieving class from url:" + fullUrl);
                return;
            }

            QString filename = "fireCards" + QString::number(classOrder) + ".json";
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/" + filename);
            startProcessFireCards(QJsonDocument::fromJson(jsonData).object(), classOrder);
        }
        //HSR Heroes Winrate
        else if(fullUrl == HSR_HEROES_WINRATE)
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


void WinratesDownloader::showDataProgressBar()
{
    emit advanceProgressBar(HSRdataThreads + fireDataThreads);
    if((HSRdataThreads + fireDataThreads) == 0) emit showMessageProgressBar("WR data ready");
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


void WinratesDownloader::initWRCards()
{
    HSRdataThreads = 4;
    fireDataThreads = NUM_HEROS;
    emit startProgressBar(HSRdataThreads + fireDataThreads, "Building WR data...");

    initHSRCards();
    initFireCards();
}


void WinratesDownloader::initHSRCards()
{
    connect(&futureHSRPickrates, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (Pickrates) --> Thread end.");
                this->hsrPickratesMap = futureHSRPickrates.result();
                emit readyHSRPickratesMap(hsrPickratesMap);
                HSRdataThreads--;
                showDataProgressBar();
            }
            );

    connect(&futureHSRWR, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (IncludedWinrate) --> Thread end.");
                this->hsrWRMap = futureHSRWR.result();
                emit readyHSRWRMap(hsrWRMap);
                HSRdataThreads--;
                showDataProgressBar();
            }
            );

    connect(&futureHSRSamples, &QFutureWatcher<QMap<QString, int> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (TimesPlayed) --> Thread end.");
                this->hsrSamplesMap = futureHSRSamples.result();
                emit readyHSRSamplesMap(hsrSamplesMap);
                HSRdataThreads--;
                showDataProgressBar();
            }
            );

    connect(&futureHSRPlayedWR, &QFutureWatcher<QMap<QString, float> *>::finished, this,
            [this]()
            {
                emit pDebug("HSR cards (PlayedWinrate) --> Thread end.");
                this->hsrPlayedWRMap = futureHSRPlayedWR.result();
                emit readyHSRPlayedWRMap(hsrPlayedWRMap);
                HSRdataThreads--;
                showDataProgressBar();
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
    if(futureHSRPickrates.isRunning() || futureHSRWR.isRunning() ||
        futureHSRSamples.isRunning() || futureHSRPlayedWR.isRunning())   return;

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
    futureHSRPickrates.setFuture(future1);

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
    futureHSRWR.setFuture(future2);
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
    futureHSRSamples.setFuture(future3);
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
    futureHSRPlayedWR.setFuture(future4);
}


void WinratesDownloader::initFireCards()
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        connect(&futureFire[i], &QFutureWatcher<FireData>::finished, this,[this,i]()
        {
            emit pDebug("Fire cards (" + Utility::classOrder2classLName(i) + ") --> Thread end.");
            FireData fireData = futureFire[i].result();
            this->fireWRMap[i] = fireData.fireWRMap;
            this->fireSamplesMap[i] = fireData.fireSamplesMap;
            fireDataThreads--;
            if(fireDataThreads == 0)
            {
                emit pDebug("Fire maps ready.");
                emit readyFireWRMap(fireWRMap);
                emit readyFireSamplesMap(fireSamplesMap);
            }
            showDataProgressBar();
        });


        QString hero = Utility::classOrder2classLName(i);
        QString filename = "fireCards" + QString::number(i) + ".json";
        QFileInfo fi(Utility::extraPath() + "/" + filename);
        if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
        {
            localFireCards(i);
        }
        else
        {
            QString url = FIRE_CARDS + hero + ".gz.json";
            emit pDebug("Fire cards --> Download from: " + QString(url));
            networkManager->get(QNetworkRequest(QUrl(url)));
        }
    }
}


void WinratesDownloader::localFireCards(const int classOrder)
{
    QString filename = "fireCards" + QString::number(classOrder) + ".json";
    emit pDebug("Fire cards --> Use local " + filename);

    QFile file(Utility::extraPath() + "/" + filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("ERROR: Failed to open " + filename);
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    startProcessFireCards(QJsonDocument::fromJson(jsonData).object(), classOrder);
}


void WinratesDownloader::startProcessFireCards(const QJsonObject &jsonObject, const int classOrder)
{
    if(futureFire[classOrder].isRunning())  return;

    const QJsonArray &data = jsonObject.value("stats").toArray();

    QFuture<FireData> future = QtConcurrent::run([data]()->FireData{
        FireData fireData;

        for(const QJsonValue &card: data)
        {
            QJsonObject cardObject = card.toObject();
            QString code = cardObject.value("cardId").toString();
            QJsonObject cardStatsObject = cardObject.value("stats").toObject();
            int samples = cardStatsObject.value("decksWithCard").toInt();
            int wins = cardStatsObject.value("decksWithCardThenWin").toInt();
            float wr = round((wins/(float)samples) * 1000)/10.0;
            fireData.fireSamplesMap[code] = samples;
            fireData.fireWRMap[code] = wr;
        }
        return fireData;
    });
    futureFire[classOrder].setFuture(future);
}














