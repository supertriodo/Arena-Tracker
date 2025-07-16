#ifndef WINRATESDOWNLOADER_H
#define WINRATESDOWNLOADER_H

#include "Sources/utility.h"
#include "qfuturewatcher.h"
#include <QObject>
#include <QNetworkAccessManager>

#define HSR_HEROES_WINRATE "https://hsreplay.net/api/v1/analytics/query/player_class_performance_summary/"
#define HSR_CARDS_PATCH "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=CURRENT_PATCH"
#define HSR_CARDS_EXP "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=CURRENT_EXPANSION"
#define HSR_CARDS_14DAYS "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=UNDERGROUND_ARENA&TimeRange=LAST_14_DAYS"


class WinratesDownloader : public QObject
{
    Q_OBJECT
public:
    WinratesDownloader(QObject *parent);
    ~WinratesDownloader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsPickrates;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsIncludedWinrates;
    QFutureWatcher<QMap<QString, int> *> futureProcessHSRCardsIncludedDecks;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsPlayedWinrates;
    int HSRdataThreads;
    QMap<QString, float> *hsrPickratesMap;
    QMap<QString, float> *hsrWRMap;
    QMap<QString, int> *hsrSamplesMap;
    QMap<QString, float> *hsrPlayedWRMap;


//Metodos
private:
    void localHSRHeroesWinrate();
    void processHSRHeroesWinrate(const QJsonObject &jsonObject);
    void localHSRCards();
    void startProcessHSRCards(const QJsonObject &jsonObject);
    void processHSRCardClassDouble(const QJsonArray &jsonArray, const QString &tag, QMap<QString, float> &cardsMap, bool trunk=false);
    void processHSRCardClassInt(const QJsonArray &jsonArray, const QString &tag, QMap<QString, int> &cardsMap);
    void showHSRdataProgressBar();

public:
    void initHSRHeroesWinrate();
    void initHSRCards();
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

private slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // WINRATESDOWNLOADER_H
