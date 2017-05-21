#include "trackobotuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QtWidgets>


TrackobotUploader::TrackobotUploader(QObject *parent) : QObject(parent)
{
    username = password = "";
    connected = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QTimer::singleShot(1, this, SLOT(checkAccount()));
}


TrackobotUploader::~TrackobotUploader()
{
    delete networkManager;
}


bool TrackobotUploader::isConnected()
{
    return this->connected;
}


QString TrackobotUploader::getUsername()
{
    return this->username;
}


void TrackobotUploader::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    QString fullUrl = reply->url().toString();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed.");

        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New account --> Download failed.");
        }
        else if(fullUrl == TRACKOBOT_PROFILE_URL)
        {
            emit pDebug("Getting profile url failed.");
        }
        else if(fullUrl == TRACKOBOT_RESULTS_URL)
        {
            emit pDebug("Upload Results failed.");
            if(!arenaItemXlsList.isEmpty()) uploadNextXlsResult();
        }
    }
    else
    {
        if(fullUrl == TRACKOBOT_NEWUSER_URL)
        {
            emit pDebug("New account --> Download success.");
            QByteArray jsonData = reply->readAll();
            if(loadAccount(jsonData))   saveAccount();
        }
        else if(fullUrl == TRACKOBOT_PROFILE_URL)
        {
            QString profileUrl = QJsonDocument::fromJson(reply->readAll()).object().value("url").toString();
            emit pDebug("Getting profile url success. Opening: " + profileUrl);
            QDesktopServices::openUrl(QUrl(profileUrl));
        }
        else if(fullUrl == TRACKOBOT_RESULTS_URL)
        {
            emit pDebug("Upload Results success.");
            if(!arenaItemXlsList.isEmpty()) uploadNextXlsResult();
        }
    }
}


void TrackobotUploader::uploadNextXlsResult()
{
    ArenaItem arenaItem = arenaItemXlsList.takeFirst();
    GameResult gameResult = arenaItem.gameResult;
    uploadResult(gameResult, arena, QDateTime::currentSecsSinceEpoch(), arenaItem.dateTime, QJsonArray());
    QString text =  Utility::heroString2FromLogNumber(gameResult.playerHero) + " vs " +
                    Utility::heroString2FromLogNumber(gameResult.enemyHero) + " uploaded";
    emit advanceProgressBar(text);
    if(arenaItemXlsList.isEmpty())  emit showMessageProgressBar("All games uploaded");
}


void TrackobotUploader::saveAccount()
{
    QFile file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);
    if(!file.open(QIODevice::WriteOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return;
    }

    QDataStream out(&file);
    out.setVersion( QDataStream::Qt_4_8 );
    out << this->username;
    out << this->password;
    out << "https://trackobot.com";
    emit pDebug("New account " + this->username + " --> Saved.");
}


bool TrackobotUploader::loadAccount(QByteArray jsonData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonUserObject = jsonDoc.object();
    this->username = jsonUserObject.value("username").toString();
    this->password = jsonUserObject.value("password").toString();

    if(!username.isEmpty() && !password.isEmpty())
    {
        emit pDebug("New account " + this->username + " --> Loaded.");
        this->connected = true;
    }
    else
    {
        emit pDebug(jsonData + " has an invalid format.");
        this->connected = false;
    }

    return this->connected;
}


bool TrackobotUploader::loadAccount()
{
    QFile file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return false;
    }

    QDataStream in(&file);
    QString webserviceUrl;
    in.setVersion(QDataStream::Qt_4_8);
    in >> this->username;
    in >> this->password;
    in >> webserviceUrl;

    if(!username.isEmpty() && !password.isEmpty())
    {
        emit pDebug("Account " + this->username + " --> Loaded.");
        this->connected = true;
    }
    else
    {
        emit pDebug(QString(TRACKOBOT_ACCOUNT_FILE) + " file has an invalid format.");
        this->connected = false;
    }

    return this->connected;
}


void TrackobotUploader::checkAccount()
{
    QFileInfo file(Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE);

    if(file.exists())   loadAccount();
    else
    {
        emit pDebug("Account missing --> Download from: " + QString(TRACKOBOT_NEWUSER_URL));
        QNetworkRequest request(QUrl(TRACKOBOT_NEWUSER_URL));
        networkManager->post(request, "");
    }
}


QString TrackobotUploader::credentials()
{
    return "Basic " + (this->username + ":" + this->password).toLatin1().toBase64();
}


void TrackobotUploader::openTBProfile()
{
    QNetworkRequest request(QUrl(TRACKOBOT_PROFILE_URL));
    request.setRawHeader( "Authorization", credentials().toLatin1());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, "");
    emit pDebug("Getting profile url...");
}


void TrackobotUploader::uploadResult(GameResult gameResult, LoadingScreenState loadingScreen,
                                     qint64 startGameEpoch, QDateTime dateTime, QJsonArray cardHistory)
{
    QJsonObject result;
    result["coin"]          = !gameResult.isFirst;
    result["hero"]          = Utility::heroStringFromLogNumber(gameResult.playerHero).toLower();
    result["opponent"]      = Utility::heroStringFromLogNumber(gameResult.enemyHero).toLower();
    result["win"]           = gameResult.isWinner;
    result["mode"]          = Utility::getLoadingScreenToString(loadingScreen).toLower();
    result["duration"]      = QDateTime::currentSecsSinceEpoch() - startGameEpoch;
    result["added"]         = dateTime.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    result["card_history"]  = cardHistory;

    QJsonObject params;
    params[ "result" ] = result;
    QByteArray data = QJsonDocument( params ).toJson();


    QNetworkRequest request(QUrl(TRACKOBOT_RESULTS_URL));
    request.setRawHeader( "Authorization", credentials().toLatin1());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, data);
    emit pDebug("Uploading result...");
}


QString TrackobotUploader::getStringCellXls(struct st_row::st_row_data* row, int col)
{
    if(col < row->fcell || col >= row->lcell)   return "";
    struct st_cell::st_cell_data* cell = &row->cells.cell[col];
    if(cell->id != 0x0201 && cell->str!=NULL)
    {
        return QString((const char*)cell->str);
    }
    return "";
}


bool TrackobotUploader::isRowGameXls(struct st_row::st_row_data* row)
{
    return getStringCellXls(row, 2).startsWith("Game");
}


QDateTime TrackobotUploader::getRowDateXls(struct st_row::st_row_data* row)
{
    QLocale locale  = QLocale(QLocale::English, QLocale::UnitedStates);
    return locale.toDateTime(getStringCellXls(row, 0), "MMM dd, yyyy");
}


QList<ArenaItem> TrackobotUploader::extractXls(xlsWorkBook* pWB)
{
    xlsWorkSheet* pWS = xls_getWorkSheet(pWB,0);
    xls_parseWorkSheet(pWS);

    struct st_row::st_row_data* row;
    WORD t;
    QDateTime dateTime;
    QString playerHero;
    QList<ArenaItem> arenaItemList;

    for (t=0;t<=pWS->rows.lastrow;t++)
    {
        row=&pWS->rows.row[t];

        if(isRowGameXls(row))
        {
            if(!playerHero.isEmpty())
            {
                ArenaItem arenaItem;
                arenaItem.dateTime = dateTime;
                arenaItem.gameResult.playerHero = playerHero;
                arenaItem.gameResult.enemyHero = Utility::heroToLogNumber(
                            getStringCellXls(row, 3).mid(3).toLower()
                            );
                arenaItem.gameResult.isWinner = (getStringCellXls(row, 4) == "Win");
                arenaItem.gameResult.isFirst = (getStringCellXls(row, 5) == "Play First");

                if(!arenaItem.gameResult.enemyHero.isEmpty())   arenaItemList.append(arenaItem);
            }
        }
        else
        {
            dateTime = getRowDateXls(row);
            if(dateTime.isValid())
            {
                playerHero = Utility::heroToLogNumber(getStringCellXls(row, 1).toLower());
            }
            else
            {
                playerHero = "";
            }
        }
    }

    return arenaItemList;
}


void TrackobotUploader::uploadXls(QString fileName)
{
    if(!arenaItemXlsList.isEmpty())
    {
        //TODO
        return;
    }

    xlsWorkBook* pWB = xls_open(fileName.toUtf8(),"UTF-8");

    if(pWB == NULL)
    {
        //TODO
        qDebug()<<"not valid";
    }
    else
    {
        arenaItemXlsList = extractXls(pWB);
        if(arenaItemXlsList.isEmpty())  return;

        emit startProgressBar(arenaItemXlsList.count());
        uploadNextXlsResult();
    }
}




