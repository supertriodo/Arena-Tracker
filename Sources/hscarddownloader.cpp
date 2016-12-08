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


void HSCardDownloader::downloadWebImage(QString code, bool isHero)
{
    foreach(DownloadingCard downCard, gettingWebCards.values())
    {
        if(downCard.code == code && downCard.isHero == isHero)
        {
            emit pDebug("Skip download: " + code + " - Already downloading.");
            return;
        }
    }

    QString urlString;
    if(isHero)  urlString = OLD_CARDS_URL + "heroes/" + code + ".png";
    else        urlString = NEW_CARDS_URL + "cards/enus/" + code + ".png";

    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(urlString)));
    DownloadingCard downCard;
    downCard.code = code;
    downCard.isHero = isHero;
    gettingWebCards[reply] = downCard;
    emit pDebug("Downloading: " + code + " - Web Cards remaining(+1): " + QString::number(gettingWebCards.count()));
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    DownloadingCard downCard = gettingWebCards[reply];
    QString code = downCard.code;
    bool isHero = downCard.isHero;
    gettingWebCards.remove(reply);
    QByteArray data = reply->readAll();
    emit pDebug("Reply: " + code + " - Web Cards remaining(-1): " + QString::number(gettingWebCards.count()));

    if(reply->error() != QNetworkReply::NoError)
    {
        if(isHero)
        {
            emit pDebug("Failed to download hero card image: " + code, Error);
            emit pLog(tr("Web: Failed to download hero card image."));
            reuseOldHero(code);
        }
        else
        {
            emit pDebug("Failed to download card image: " + code + " - Trying again.", Error);
            emit pLog(tr("Web: Failed to download card image. Trying again."));
            downloadWebImage(code);
        }
    }
    else if(data.isEmpty())
    {
        emit pDebug("Downloaded empty card image: " + code, Error);
        emit pLog(tr("Web: Downloaded empty card image."));
        emit downloaded("");
    }
    else
    {
        QImage webImage;
        webImage.loadFromData(data);
        if(!isHero)     webImage = webImage.scaledToWidth(200, Qt::SmoothTransformation);

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


void HSCardDownloader::reuseOldHero(QString code)
{
    QString oldHeroCode = code.left(7);
    QFile heroFile(Utility::hscardsPath() + "/" + oldHeroCode + ".png");

    if(heroFile.exists())
    {
        if(heroFile.copy(Utility::hscardsPath() + "/" + code + ".png"))
        {
            emit pDebug("Old hero reused: " + oldHeroCode);
            emit downloaded(code);
        }
    }
    else
    {
        emit pDebug("Old hero not found: " + oldHeroCode);
    }
}


void HSCardDownloader::setLang(QString value)
{
    if(value == "esMX")         lang = "eses";
    else if(value == "enGB" ||
            value == "jaJP" ||
            value == "plPL" ||
            value == "koKR" ||
            value == "thTH")    lang = "enus";
    else                        lang = value.toLower();
}

