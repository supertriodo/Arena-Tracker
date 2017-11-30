#include "hscarddownloader.h"
#include <QtWidgets>

HSCardDownloader::HSCardDownloader(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(saveWebImage(QNetworkReply*)));
    this->fastMode = false;
}

HSCardDownloader::~HSCardDownloader()
{
    delete networkManager;
}


void HSCardDownloader::setFastMode(bool fastMode)
{
    this->fastMode = fastMode;
}


void HSCardDownloader::setSlowMode()
{
    this->fastMode = false;
}


void HSCardDownloader::forceNextDownload()
{
    if(!pendingDownloads.isEmpty())
    {
        downloadWebImage(pendingDownloads.takeFirst(), true);
        QTimer::singleShot(FORCE_NEXT_DOWNLOAD, this, SLOT(forceNextDownload()));
    }
}


void HSCardDownloader::downloadWebImage(DownloadingCard downCard, bool force)
{
    downloadWebImage(downCard.code, downCard.isHero, force);
}


void HSCardDownloader::downloadWebImage(QString code, bool isHero, bool force, bool fromHearthHead)
{
    //Already downloading
    foreach(DownloadingCard downCard, gettingWebCards.values())
    {
        if(downCard.code == code && downCard.isHero == isHero)
        {
            emit pDebug("Skip download: " + code + " - Already downloading.");
            return;
        }
    }

    //Already planned to download (Eliminamos el antiguo download en pending)
    for(int i=0; i<pendingDownloads.count(); i++)
    {
        DownloadingCard pendingCard = pendingDownloads[i];
        if(pendingCard.code == code && pendingCard.isHero == isHero)
        {
            emit pDebug("Prioritize download: " + code + " - Need for drafting.");
            pendingDownloads.removeAt(i);
            break;
        }
    }

    DownloadingCard downCard;
    downCard.code = code;
    downCard.isHero = isHero;

    if(!force && gettingWebCards.count() >= MAX_DOWNLOADS)
    {
        if(pendingDownloads.isEmpty())      QTimer::singleShot(FORCE_NEXT_DOWNLOAD, this, SLOT(forceNextDownload()));
        pendingDownloads.prepend(downCard);
        return;
    }

    QString urlString;
    if(!fromHearthHead)
    {
        urlString = AT_CARDS_URL + code + ".png";
    }
    else if(isHero)  urlString = OLD_CARDS_URL + "heroes/" + code + ".png";
    else
    {
        if(code.endsWith("_premium"))   urlString = NEW_CARDS_URL + "cards/enus/animated/" + code + ".gif";
        else                            urlString = NEW_CARDS_URL + "cards/enus/" + code + ".png";
    }

    QNetworkReply * reply = networkManager->get(QNetworkRequest(QUrl(urlString)));
    gettingWebCards[reply] = downCard;
    emit pDebug("Downloading (" + QString(fromHearthHead?"HearthHead":"GitHub") + "): " + code + " - (" + QString::number(gettingWebCards.count()) +
                ") - " + QString::number(pendingDownloads.count()));
}


void HSCardDownloader::saveWebImage(QNetworkReply * reply)
{
    reply->deleteLater();

    if(!gettingWebCards.contains(reply))    return;

    DownloadingCard downCard = gettingWebCards.take(reply);
    QString code = downCard.code;
    bool isHero = downCard.isHero;

    emit pDebug("Reply: " + code + " - (" + QString::number(gettingWebCards.count()) +
                ") - " + QString::number(pendingDownloads.count()));


    QByteArray data = reply->readAll();
    if(reply->error() != QNetworkReply::NoError)
    {
        QString fullUrl = reply->url().toString();
        if(fullUrl.startsWith(AT_CARDS_URL))
        {
            emit pDebug("Failed to download card image(GitHub): " + code + " - Trying HearthHead.", Error);
            emit pLog(tr("Web: Failed to download card image(GitHub). Trying HearthHead."));
            downloadWebImage(code, isHero, false, true);
        }
        else if(isHero)
        {
            emit pDebug("Failed to download hero card image(HearthHead): " + code, Error);
            emit pLog(tr("Web: Failed to download hero card image(HearthHead)."));
            if(!reuseOldHero(code)) downloadWebImage(code, isHero, false, true);
        }
        else
        {
            emit pDebug("Failed to download card image(HearthHead): " + code + " - Trying again.", Error);
            emit pLog(tr("Web: Failed to download card image(HearthHead). Trying again."));
            downloadWebImage(code, isHero, false, true);
        }
    }
    else if(data.isEmpty())
    {
        emit pDebug("Downloaded empty card image: " + code, Error);
        emit pLog(tr("Web: Downloaded empty card image."));
        emit missingOnWeb(code);
    }
    else
    {
        QImage webImage;
        webImage.loadFromData(data);
        if(!isHero && webImage.width()!=200)    webImage = webImage.scaledToWidth(200, Qt::SmoothTransformation);

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

    //Next download
    if(fastMode && !pendingDownloads.isEmpty() && gettingWebCards.count() < MAX_DOWNLOADS)
    {
        downloadWebImage(pendingDownloads.takeFirst());
    }

    //All cards downloaded
    if(pendingDownloads.isEmpty() && gettingWebCards.isEmpty())
    {
        emit allCardsDownloaded();
    }
}


bool HSCardDownloader::reuseOldHero(QString code)
{
    if(code.length() == 7)  return false;

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
        downloadWebImage(oldHeroCode, true);
    }
    return true;
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

