#ifndef HSCARDDOWNLOADER_H
#define HSCARDDOWNLOADER_H

#include "utility.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

#define OLD_CARDS_URL QString("http://wow.zamimg.com/images/hearthstone/")
#define NEW_CARDS_URL QString("http://media.services.zam.com/v1/media/byName/hs/")


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


//Metodos
private:
    void reuseOldHero(QString code);

public:
    void downloadWebImage(QString code, bool isHero=false);
    void setLang(QString value);

signals:
    void downloaded(QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="HSCardDownloader");

public slots:
    void saveWebImage(QNetworkReply * reply);
};

#endif // HSCARDDOWNLOADER_H
