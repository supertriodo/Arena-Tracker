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
    if(gettingWebCards.values().contains(code))
    {
        emit pDebug("Skip download: " + code + " - Already downloading.");
        return;
    }

    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(QString(CARDS_URL + lang + "/medium/" + code + ".png"))));
    gettingWebCards[reply] = code;
    emit pDebug("Downloading: " + code + " - Web Cards remaining(+1): " + QString::number(gettingWebCards.count()));
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    QString code = gettingWebCards[reply];
    gettingWebCards.remove(reply);
    emit pDebug("Reply: " + code + " - Web Cards remaining(-1): " + QString::number(gettingWebCards.count()));

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug("Failed to download card image: " + code + " - Trying again.", Error);
        emit pLog(tr("Web: Failed to download card image. Trying again."));
        downloadWebImage(code);
    }
    else
    {
        QImage webImage;
        webImage.loadFromData(reply->readAll());

        if(!webImage.save(Utility::hscardsPath() + "/" + code + ".png", "png"))
        {
            emit pDebug("Failed to save card image to disk: " + code, Error);
            emit pLog(tr("File: ERROR:Saving card image to disk."));
        }
        else
        {
            emit pDebug("Card downloaded: " + code);
            emit downloaded(code);
        }
    }
}


void HSCardDownloader::setLang(QString value)
{
    if(value == "esMX")         lang = "eses";
    else if(value == "enGB" ||
            value == "jaJP" ||
            value == "plPL" ||
            value == "koKR")    lang = "enus";
    else                        lang = value.toLower();
}

