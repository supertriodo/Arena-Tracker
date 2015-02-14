#ifndef HSCARDDOWNLOADER_H
#define HSCARDDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

#define CARDS_URL "http://wow.zamimg.com/images/hearthstone/cards/enus/medium/"


class HSCardDownloader : public QObject
{
    Q_OBJECT
public:
    explicit HSCardDownloader(QObject *parent = 0);
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
    void sendLog(QString line);

public slots:
    void saveWebImage(QNetworkReply * reply);
};

#endif // HSCARDDOWNLOADER_H
