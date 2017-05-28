#ifndef HSCARDDOWNLOADER_H
#define HSCARDDOWNLOADER_H

#include "utility.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

#define OLD_CARDS_URL QString("http://wow.zamimg.com/images/hearthstone/")
#define NEW_CARDS_URL QString("http://media.services.zam.com/v1/media/byName/hs/")
#define MAX_DOWNLOADS 10
#define FORCE_NEXT_DOWNLOAD 500


class DownloadingCard
{
public:
    QString code = "";
    bool isHero = false;
};


class HSCardDownloader : public QObject
{
    Q_OBJECT
public:
    HSCardDownloader(QObject *parent);
    ~HSCardDownloader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QMap<QNetworkReply *, DownloadingCard> gettingWebCards;
    QString lang;
    QList<DownloadingCard> pendingDownloads;


//Metodos
private:
    void reuseOldHero(QString code);
    void downloadWebImage(DownloadingCard downCard, bool force=false);

public:
    void downloadWebImage(QString code, bool isHero=false, bool force=false);
    void setLang(QString value);

signals:
    void downloaded(QString code);
    void missingOnWeb(QString code);
    void allCardsDownloaded();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="HSCardDownloader");

public slots:
    void saveWebImage(QNetworkReply * reply);

private slots:
    void forceNextDownload();
};

#endif // HSCARDDOWNLOADER_H
