#include "winratesdownloader.h"
#include "Widgets/scorebutton.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include "qnetworkreply.h"
#include <QtWidgets>

WinratesDownloader::WinratesDownloader(QObject *parent) : QObject(parent)
{
    hsrdataPickratesThreads = hsrdataWRThreads = hsrdataSamplesThreads = hsrdataPlayedThreads = fireDataThreads = 0;
    hsrPickratesMap = new QMap<QString, float>[NUM_HEROS];
    hsrWRMap = new QMap<QString, float>[NUM_HEROS];
    hsrSamplesMap = new QMap<QString, int>[NUM_HEROS];
    hsrPlayedWRMap = new QMap<QString, float>[NUM_HEROS];

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
    for(int i=0; i<NUM_HEROS; i++)
    {
        if(futureHSRPickrates[i].isRunning())   futureHSRPickrates[i].waitForFinished();
        if(futureHSRWR[i].isRunning())          futureHSRWR[i].waitForFinished();
        if(futureHSRSamples[i].isRunning())     futureHSRSamples[i].waitForFinished();
        if(futureHSRPlayedWR[i].isRunning())    futureHSRPlayedWR[i].waitForFinished();
        if(futureFire[i].isRunning())           futureFire[i].waitForFinished();
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


int WinratesDownloader::runningThreads()
{
    return hsrdataPickratesThreads + hsrdataWRThreads + hsrdataSamplesThreads + hsrdataPlayedThreads + fireDataThreads;
}


void WinratesDownloader::showDataProgressBar()
{
    int numThreads = runningThreads();
    emit advanceProgressBar(numThreads);
    if(numThreads == 0) emit showMessageProgressBar("WR data ready");
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
    hsrdataPickratesThreads = hsrdataWRThreads = hsrdataSamplesThreads = hsrdataPlayedThreads = fireDataThreads = NUM_HEROS;
    emit startProgressBar(runningThreads(), "Building WR data...");

    initHSRCards();
    initFireCards();
}


void WinratesDownloader::initHSRCards()
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        connect(&futureHSRPickrates[i], &QFutureWatcher<QMap<QString, float>>::finished, this,[this,i]()
        {
            emit pDebug("HSR cards (Pickrates: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
            this->hsrPickratesMap[i] = futureHSRPickrates[i].result();

            hsrdataPickratesThreads--;
            if(hsrdataPickratesThreads == 0)
            {
                emit pDebug("HSR cards (Pickrates) ready.");
                emit readyHSRPickratesMap(hsrPickratesMap);
            }
            showDataProgressBar();
        });

        connect(&futureHSRWR[i], &QFutureWatcher<QMap<QString, float>>::finished, this,[this,i]()
        {
            emit pDebug("HSR cards (IncludedWinrate: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
            this->hsrWRMap[i] = futureHSRWR[i].result();

            hsrdataWRThreads--;
            if(hsrdataWRThreads == 0)
            {
                emit pDebug("HSR cards (IncludedWinrate) ready.");
                emit readyHSRWRMap(hsrWRMap);
            }
            showDataProgressBar();
        });

        connect(&futureHSRSamples[i], &QFutureWatcher<QMap<QString, int>>::finished, this,[this,i]()
        {
            emit pDebug("HSR cards (TimesPlayed: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
            this->hsrSamplesMap[i] = futureHSRSamples[i].result();

            hsrdataSamplesThreads--;
            if(hsrdataSamplesThreads == 0)
            {
                emit pDebug("HSR cards (TimesPlayed) ready.");
                emit readyHSRSamplesMap(hsrSamplesMap);
            }
            showDataProgressBar();
        });

        connect(&futureHSRPlayedWR[i], &QFutureWatcher<QMap<QString, float>>::finished, this,[this,i]()
        {
            emit pDebug("HSR cards (PlayedWinrate: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
            this->hsrPlayedWRMap[i] = futureHSRPlayedWR[i].result();

            hsrdataPlayedThreads--;
            if(hsrdataPlayedThreads == 0)
            {
                emit pDebug("HSR cards (PlayedWinrate) ready.");
                emit readyHSRPlayedWRMap(hsrPlayedWRMap);
            }
            showDataProgressBar();
        });
    }


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
    for(int i=0; i<NUM_HEROS; i++)
    {
        if(futureHSRPickrates[i].isRunning() || futureHSRWR[i].isRunning() ||
            futureHSRSamples[i].isRunning() || futureHSRPlayedWR[i].isRunning())   return;

        const QString hero = Utility::classEnum2classUName((CardClass)i);
        const QJsonArray &data = jsonObject.value("series").toObject().value("data").toObject().value(hero).toArray();

        QFuture<QMap<QString, float>> future1 = QtConcurrent::run([this,data]()->QMap<QString, float>{
            QMap<QString, float> map;
            processHSRCardClassDouble(data, "included_popularity", map);
            return map;
        });
        futureHSRPickrates[i].setFuture(future1);

        QFuture<QMap<QString, float>> future2 = QtConcurrent::run([this,data]()->QMap<QString, float>{
            QMap<QString, float> map;
            processHSRCardClassDouble(data, "included_winrate", map, true);
            return map;
        });
        futureHSRWR[i].setFuture(future2);
        QFuture<QMap<QString, int>> future3 = QtConcurrent::run([this,data]()->QMap<QString, int>{
            QMap<QString, int> map;
            processHSRCardClassInt(data, "times_played", map);
            return map;
            });
        futureHSRSamples[i].setFuture(future3);
        QFuture<QMap<QString, float>> future4 = QtConcurrent::run([this,data]()->QMap<QString, float>{
            QMap<QString, float> map;
            processHSRCardClassDouble(data, "winrate_when_played", map, true);
            return map;
        });
        futureHSRPlayedWR[i].setFuture(future4);
    }
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



