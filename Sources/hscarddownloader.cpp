#include "hscarddownloader.h"
#include <QtWidgets>

HSCardDownloader::HSCardDownloader(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(saveWebImage(QNetworkReply*)));
}

HSCardDownloader::~HSCardDownloader()
{
    delete networkManager;
}


void HSCardDownloader::downloadWebImage(QString code)
{
    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(QString(CARDS_URL + code + ".png"))));
    gettingWebCards[reply] = code;
    qDebug() << "HSCardDownloader: Web Cards pendientes(+1): " << gettingWebCards.count();
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    QString code = gettingWebCards[reply];

    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "HSCardDownloader: " << "ERROR: Fallo al descargar: " << code;
        emit sendLog(tr("Web: Failed to download card image."));
        return;
    }
    QImage webImage;
    webImage.loadFromData(reply->readAll());

    if(!webImage.save("./HSCards/" + code + ".png", "png"))
    {
        qDebug() << "HSCardDownloader: " << "ERROR: Fallo al guardar en disco: " << code;
        emit sendLog(tr("File: ERROR:Saving card image to disk. Make sure HSCards dir is in the same place as the exe."));
        return;
    }

    emit downloaded(code);
    gettingWebCards.remove(reply);
    qDebug() << "HSCardDownloader: Web Cards pendientes(-1): " << gettingWebCards.count();
    emit sendLog(tr("Web: New card image downloaded."));
}
