#ifndef HSCARDDOWNLOADER_H
#define HSCARDDOWNLOADER_H

#include "utility.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

#define CARDS_URL "http://wow.zamimg.com/images/hearthstone/cards/enus/medium/"


class HSCardDownloader : public QObject
{
    Q_OBJECT
public:
    HSCardDownloader(QObject *parent);
    ~HSCardDownloader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QMap<QNetworkReply *, QString> gettingWebCards;


//Metodos
private:

public:
    void downloadWebImage(QString code);

signals:
    void downloaded(QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="HSCardDownloader");

public slots:
    void saveWebImage(QNetworkReply * reply);
};

#endif // HSCARDDOWNLOADER_H
