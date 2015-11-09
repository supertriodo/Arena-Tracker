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
    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(QString(CARDS_URL + lang + "/medium/" + code + ".png"))));
    gettingWebCards[reply] = code;
    emit pDebug("Web Cards remaining(+1): " + QString::number(gettingWebCards.count()));
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    QString code = gettingWebCards[reply];

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug("Failed to download card image: " + code, Error);
        emit pLog(tr("Web: Failed to download card image."));
        return;
    }
    QImage webImage;
    webImage.loadFromData(reply->readAll());

    if(!webImage.save(Utility::appPath() + "/HSCards/" + code + ".png", "png"))
    {
        emit pDebug("Failed to save card image to disk: " + code, Error);
        emit pLog(tr("File: ERROR:Saving card image to disk. Make sure HSCards dir is in the same place as the exe."));
        return;
    }

    emit downloaded(code);
    gettingWebCards.remove(reply);
    emit pDebug("Web Cards remaining(-1): " + QString::number(gettingWebCards.count()));
    emit pLog(tr("Web: New card image downloaded."));
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

