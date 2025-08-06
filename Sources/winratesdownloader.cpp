#include "winratesdownloader.h"
#include "Widgets/scorebutton.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include "qfileinfo.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

WinratesDownloader::WinratesDownloader(QObject *parent) : QObject(parent)
{
    hsrdataPickratesThreads = hsrdataWRThreads = hsrdataSamplesThreads = hsrdataPlayedThreads = hsrdataBundlesThreads = fireDataThreads = 0;
    hsrPickratesMap = new QMap<QString, float>[NUM_HEROS];
    hsrWRMap = new QMap<QString, float>[NUM_HEROS];
    hsrSamplesMap = new QMap<QString, int>[NUM_HEROS];
    hsrPlayedWRMap = new QMap<QString, float>[NUM_HEROS];
    hsrBundlesMap = new QMap<QString, QStringList>[NUM_HEROS];

    fireWRMap = new QMap<QString, float>[NUM_HEROS];
    fireSamplesMap = new QMap<QString, int>[NUM_HEROS];

    dbfIdMap = new QMap<int, QString>;

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
    if(hsrBundlesMap != nullptr)    delete[] hsrBundlesMap;

    //Delete Fire maps
    if(fireWRMap != nullptr)        delete[] fireWRMap;
    if(fireSamplesMap != nullptr)   delete[] fireSamplesMap;

    deleteDbfIdMap();//Ya deberia estar borrado, solo por seguridad.
}


void WinratesDownloader::waitFinishThreads()
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        if(futureHSRPickrates[i].isRunning())   futureHSRPickrates[i].waitForFinished();
        if(futureHSRWR[i].isRunning())          futureHSRWR[i].waitForFinished();
        if(futureHSRSamples[i].isRunning())     futureHSRSamples[i].waitForFinished();
        if(futureHSRPlayedWR[i].isRunning())    futureHSRPlayedWR[i].waitForFinished();
        if(futureHSRBundles[i].isRunning())     futureHSRBundles[i].waitForFinished();
        if(futureFire[i].isRunning())           futureFire[i].waitForFinished();
    }
}


int WinratesDownloader::url2classOrder(QString url)
{
    int classOrder = -1;
    static const auto re = QRegularExpression(QString(FIRE_CARDS_URL) + "(\\w+)\\.gz\\.json");
    if(url.contains(re, match))
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

        if(fullUrl.startsWith(FIRE_CARDS_URL))
        {
            int classOrder = url2classOrder(fullUrl);
            if(classOrder == -1)    emit pDebug("ERROR: Fail retrieving class from url:" + fullUrl);
            else                    localFireCards(classOrder);
        }
        else if(fullUrl == HSR_CARDS_14DAYS_URL)
        {
            emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_EXP_URL));
            networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_EXP_URL)));
        }
        else if(fullUrl == HSR_CARDS_EXP_URL)
        {
            emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_PATCH_URL));
            networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_PATCH_URL)));
        }
        else if(fullUrl == HSR_CARDS_PATCH_URL)
        {
            localHSRCards();
        }
        else if(fullUrl == HSR_HEROES_WINRATE_URL)
        {
            localHSRHeroesWinrate();
        }
        else if(fullUrl == HSR_BUNDLES_URL)
        {
            localHSRBundles();
        }
        else
        {
            emit pDebug(reply->url().toString() + " --> Failed.");
            // networkManager->get(QNetworkRequest(reply->url()));
        }
    }
    else
    {
        //Fire Cards Winrate/Samples
        if(fullUrl.startsWith(FIRE_CARDS_URL))
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
        else if(fullUrl == HSR_HEROES_WINRATE_URL)
        {
            emit pDebug("Heroes winrate --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/" + HSR_HEROES_FILE);
            processHSRHeroesWinrate(QJsonDocument::fromJson(jsonData).object());
        }
        //HSR Cards Pickrate/Winrate
        else if(fullUrl == HSR_CARDS_PATCH_URL || fullUrl == HSR_CARDS_EXP_URL || fullUrl == HSR_CARDS_14DAYS_URL)
        {
            emit pDebug("HSR cards --> Download Success from: " + fullUrl);
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/" + HSR_CARDS_FILE);
            startProcessHSRCards(QJsonDocument::fromJson(jsonData).object());
        }
        //HSR Bundles
        else if(fullUrl == HSR_BUNDLES_URL)
        {
            emit pDebug("HSR bundles --> Download Success from: " + fullUrl);
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/" + HSR_BUNDLES_FILE);
            startProcessHSRBundles(QJsonDocument::fromJson(jsonData).object());
        }
    }
}


int WinratesDownloader::runningThreads()
{
    return hsrdataPickratesThreads + hsrdataWRThreads + hsrdataSamplesThreads + hsrdataPlayedThreads + hsrdataBundlesThreads + fireDataThreads;
}


//Show threads progres
//If all threads done, show message and delete dbfIdMap
void WinratesDownloader::showDataProgressBar()
{
    int numThreads = runningThreads();
    emit advanceProgressBar(numThreads);
    if(numThreads == 0)
    {
        emit showMessageProgressBar("WR data ready");
        deleteDbfIdMap();
    }
}


void WinratesDownloader::initHSRHeroesWinrate()
{
    QFileInfo fi(Utility::extraPath() + "/" + HSR_HEROES_FILE);
    if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
    {
        localHSRHeroesWinrate();
    }
    else
    {
        emit pDebug("Heroes winrate --> Download from: " + QString(HSR_HEROES_WINRATE_URL));
        networkManager->get(QNetworkRequest(QUrl(HSR_HEROES_WINRATE_URL)));
    }
}


void WinratesDownloader::localHSRHeroesWinrate()
{
    emit pDebug(QStringLiteral("Heroes winrate --> Use local %1").arg(HSR_HEROES_FILE));

    QFile file(Utility::extraPath() + "/" + HSR_HEROES_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug(QStringLiteral("ERROR: Failed to open %1").arg(HSR_HEROES_FILE));
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
    hsrdataPickratesThreads = hsrdataWRThreads = hsrdataSamplesThreads = hsrdataPlayedThreads = hsrdataBundlesThreads = fireDataThreads = NUM_HEROS;
    emit startProgressBar(runningThreads(), "Building WR data...");

    initHSRBundles();
    initFireCards();

    Utility::buildDbfIdMap(dbfIdMap);
    initHSRCards();
}


void WinratesDownloader::initHSRBundles()
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        connect(&futureHSRBundles[i], &QFutureWatcher<QMap<QString, QStringList>>::finished, this,[this,i]()
                {
                    // emit pDebug("HSR bundles (" + Utility::classOrder2classLName(i) + ") --> Thread end.");
                    this->hsrBundlesMap[i] = futureHSRBundles[i].result();

                    hsrdataBundlesThreads--;
                    if(hsrdataBundlesThreads == 0)
                    {
                        emit pDebug("HSR bundles ready.");
                        emit readyHSRBundlesMap(hsrBundlesMap);
                    }
                    showDataProgressBar();
                });
    }


    QFileInfo fi(Utility::extraPath() + "/" + HSR_BUNDLES_FILE);
    if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
    {
        localHSRBundles();
    }
    else
    {
        emit pDebug("HSR bundles --> Download from: " + QString(HSR_BUNDLES_URL));
        networkManager->get(QNetworkRequest(QUrl(HSR_BUNDLES_URL)));
    }
}


void WinratesDownloader::localHSRBundles()
{
    emit pDebug(QStringLiteral("HSR bundles --> Use local %1").arg(HSR_BUNDLES_FILE));

    QFile file(Utility::extraPath() + "/" + HSR_BUNDLES_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug(QStringLiteral("ERROR: Failed to open %1").arg(HSR_BUNDLES_FILE));
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    startProcessHSRBundles(QJsonDocument::fromJson(jsonData).object());
}


void WinratesDownloader::startProcessHSRBundles(const QJsonObject &jsonObject)
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        if(futureHSRBundles[i].isRunning()) return;

        const QString hero = Utility::classEnum2classUName((CardClass)i);
        const QJsonArray &data = jsonObject.value("data").toObject().value(hero).toArray();

        QFuture<QMap<QString, QStringList>> future = QtConcurrent::run([data]()->QMap<QString, QStringList>{
            QMap<QString, QStringList> map;

            for(const QJsonValue &bundleV: data)
            {
                QJsonObject bundleObject = bundleV.toObject();
                QString codeMain = bundleObject.value("package_key_card_id").toString();
                QStringList codesList;

                const auto &codeArray = bundleObject.value("package_card_ids").toArray();
                for(const QJsonValue &codeV: codeArray)
                {
                    codesList << codeV.toString();

                }
                map.insert(codeMain, codesList);
            }
            return map;
        });
        futureHSRBundles[i].setFuture(future);
    }
}


void WinratesDownloader::deleteDbfIdMap()
{
    if(dbfIdMap != nullptr)
    {
        delete dbfIdMap;
        dbfIdMap = nullptr;
    }
}


void WinratesDownloader::initHSRCards()
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        connect(&futureHSRPickrates[i], &QFutureWatcher<QMap<QString, float>>::finished, this,[this,i]()
        {
            // emit pDebug("HSR cards (Pickrates: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
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
            // emit pDebug("HSR cards (IncludedWinrate: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
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
            // emit pDebug("HSR cards (TimesPlayed: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
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
            // emit pDebug("HSR cards (PlayedWinrate: " + Utility::classOrder2classLName(i) + ") --> Thread end.");
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


    QFileInfo fi(Utility::extraPath() + "/" + HSR_CARDS_FILE);
    if(fi.exists() && (fi.lastModified().addDays(1)>QDateTime::currentDateTime()))
    {
        localHSRCards();
    }
    else
    {
        emit pDebug("HSR cards --> Download from: " + QString(HSR_CARDS_14DAYS_URL));
        networkManager->get(QNetworkRequest(QUrl(HSR_CARDS_14DAYS_URL)));
    }
}


void WinratesDownloader::localHSRCards()
{
    emit pDebug(QStringLiteral("HSR cards --> Use local %1").arg(HSR_CARDS_FILE));

    QFile file(Utility::extraPath() + "/" + HSR_CARDS_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug(QStringLiteral("ERROR: Failed to open %1").arg(HSR_CARDS_FILE));
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
        int dbfId = cardObject.value("dbf_id").toInt();
        QString code = dbfIdMap->value(dbfId);
        double value = cardObject.value(tag).toDouble();
        if(trunk)   value = round(value * 10)/10.0;
        cardsMap.insert(code, static_cast<float>(value));
    }
}


void WinratesDownloader::processHSRCardClassInt(const QJsonArray &jsonArray, const QString &tag, QMap<QString, int> &cardsMap)
{
    for(const QJsonValue &card: jsonArray)
    {
        QJsonObject cardObject = card.toObject();
        int dbfId = cardObject.value("dbf_id").toInt();
        QString code = dbfIdMap->value(dbfId);
        int value = cardObject.value(tag).toInt();
        cardsMap.insert(code, value);
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
            // emit pDebug("Fire cards (" + Utility::classOrder2classLName(i) + ") --> Thread end.");
            FireData fireData = futureFire[i].result();
            this->fireWRMap[i] = fireData.fireWRMap;
            this->fireSamplesMap[i] = fireData.fireSamplesMap;
            fireDataThreads--;
            if(fireDataThreads == 0)
            {
                emit pDebug("Fire cards (WR/Samples) ready.");
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
            QString url = FIRE_CARDS_URL + hero + ".gz.json";
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
            fireData.fireSamplesMap.insert(code, samples);
            fireData.fireWRMap.insert(code, wr);
        }
        return fireData;
    });
    futureFire[classOrder].setFuture(future);
}



