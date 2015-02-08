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


void HSCardDownloader::downloadWebImage(DeckCard &deckCard)
{
    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(QString(CARDS_URL + deckCard.code + ".png"))));
    gettingWebCards[reply] = deckCard;
    qDebug() << "1 mas. Web Cards pendientes: " << gettingWebCards.count();
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    DeckCard deckCard = gettingWebCards[reply];

    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "MainWindow: " << "ERROR: Fallo al descargar: " << deckCard.code;
        emit sendLog(tr("Web: No internet access to Hearthhead."));
        return;
    }
    QImage webImage;
    webImage.loadFromData(reply->readAll());

    if(!webImage.save("./HSCards/" + deckCard.code + ".png", "png"))
    {
        qDebug() << "MainWindow: " << "ERROR: Fallo al guardar en disco: " << deckCard.code;
        emit sendLog(tr("File: ERROR saving image card to disk. Did you remove HSCards dir?"));
        return;
    }

    emit downloaded(deckCard);
    gettingWebCards.remove(reply);
    qDebug() << "1 menos. Web Cards pendientes: " << gettingWebCards.count();
    emit sendLog(tr("Web: New card image downloaded."));
}
