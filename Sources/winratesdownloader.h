#ifndef WINRATESDOWNLOADER_H
#define WINRATESDOWNLOADER_H

#include "Sources/utility.h"
#include "qfuturewatcher.h"
#include <QObject>
#include <QNetworkAccessManager>

#define HSR_HEROES_WINRATE_URL "https://hsreplay.net/api/v1/arena/classes_stats/"
#define HSR_CARDS_PATCH_URL "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=CURRENT_PATCH"
#define HSR_CARDS_EXP_URL "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=CURRENT_EXPANSION"
#define HSR_CARDS_14DAYS_URL "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=LAST_14_DAYS"
#define HSR_BUNDLES_URL "https://hsreplay.net/api/v1/arena/card_packages/free/?format=json"
#define FIRE_CARDS_URL "https://static.zerotoheroes.com/api/arena/stats/cards/arena-underground/last-patch/"

#define HSR_HEROES_FILE "HSRheroes.json"
#define HSR_CARDS_FILE "HSRcards.json"
#define HSR_BUNDLES_FILE "HSRbundles.json"


class FireData
{
public:
    QMap<QString, float> fireWRMap;
    QMap<QString, int> fireSamplesMap;
};


class WinratesDownloader : public QObject
{
    Q_OBJECT
public:
    WinratesDownloader(QObject *parent);
    ~WinratesDownloader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QRegularExpressionMatch *match;
    QFutureWatcher<QMap<QString, float>> futureHSRPickrates[NUM_HEROS];
    QFutureWatcher<QMap<QString, float>> futureHSRWR[NUM_HEROS];
    QFutureWatcher<QMap<QString, int>> futureHSRSamples[NUM_HEROS];
    QFutureWatcher<QMap<QString, float>> futureHSRPlayedWR[NUM_HEROS];
    QFutureWatcher<QMap<QString, QStringList>> futureHSRBundles[NUM_HEROS];
    QFutureWatcher<FireData> futureFire[NUM_HEROS];
    int hsrdataPickratesThreads, hsrdataWRThreads, hsrdataSamplesThreads, hsrdataPlayedThreads, hsrdataBundlesThreads, fireDataThreads;
    QMap<QString, float> *hsrPickratesMap;
    QMap<QString, float> *hsrWRMap;
    QMap<QString, int> *hsrSamplesMap;
    QMap<QString, float> *hsrPlayedWRMap;
    QMap<QString, QStringList> *hsrBundlesMap;
    QMap<QString, float> *fireWRMap;
    QMap<QString, int> *fireSamplesMap;
    QMap<int, QString> *dbfIdMap;


//Metodos
private:
    void initHSRCards();
    void initHSRBundles();
    void initFireCards();
    void localHSRHeroesWinrate();
    void localHSRCards();
    void localHSRBundles();
    void localFireCards(const int classOrder);
    void startProcessHSRCards(const QJsonObject &jsonObject);
    void startProcessHSRBundles(const QJsonObject &jsonObject);
    void startProcessFireCards(const QJsonObject &jsonObject, const int classOrder);
    void processHSRCardClassDouble(const QJsonArray &jsonArray, const QString &tag, QMap<QString, float> &cardsMap, bool trunk=false);
    void processHSRCardClassInt(const QJsonArray &jsonArray, const QString &tag, QMap<QString, int> &cardsMap);
    void processHSRHeroesWinrate(const QJsonObject &jsonObject);
    void showDataProgressBar();
    int url2classOrder(QString url);
    int runningThreads();
    void deleteDbfIdMap();

public:
    void initWRCards();
    void initHSRHeroesWinrate();
    void waitFinishThreads();

signals:
    void startProgressBar(int maximum, QString text);
    void advanceProgressBar(int remaining, QString text="");
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="WinratesDownloader");
    void readyHSRPickratesMap(QMap<QString, float> *hsrPickratesMap);
    void readyHSRWRMap(QMap<QString, float> *hsrWRMap);
    void readyHSRSamplesMap(QMap<QString, int> *hsrSamplesMap);
    void readyHSRPlayedWRMap(QMap<QString, float> *hsrPlayedWRMap);
    void readyHSRBundlesMap(QMap<QString, QStringList> *hsrBundlesMap);
    void readyFireWRMap(QMap<QString, float> *fireWRMap);
    void readyFireSamplesMap(QMap<QString, int> *fireSamplesMap);

private slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // WINRATESDOWNLOADER_H
