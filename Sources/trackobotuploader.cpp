#include "trackobotuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QtWidgets>


TrackobotUploader::TrackobotUploader(QObject *parent) : QObject(parent)
{
    username = password = "";
    connectSuccess = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}


TrackobotUploader::~TrackobotUploader()
{
    delete networkManager;
}


bool TrackobotUploader::isConnected()
{
    return this->connectSuccess;
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
            importAccount(jsonData);
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
    QString text =  Utility::classLogNumber2classULName(gameResult.playerHero) + " vs " +
                    Utility::classLogNumber2classULName(gameResult.enemyHero) + " uploaded";
    emit advanceProgressBar(arenaItemXlsList.count(), text);
    uploadResult(gameResult, arena, QDateTime::currentMSecsSinceEpoch()/1000, arenaItem.dateTime, QJsonArray());
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
    emit showMessageProgressBar("New track-o-bot account");
}


bool TrackobotUploader::loadAccount(QByteArray jsonData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonUserObject = jsonDoc.object();
    QString username, password;
    username = jsonUserObject.value("username").toString();
    password = jsonUserObject.value("password").toString();

    if(!username.isEmpty() && !password.isEmpty())
    {
        this->username = username;
        this->password = password;
        this->connectSuccess = true;
        emit pDebug("New account " + this->username + " --> Loaded.");
        emit connected(username, password);
        return true;
    }
    else
    {
        this->connectSuccess = false;
        emit pDebug(jsonData + " has an invalid format.");
        emit disconnected();
        return false;
    }
}


void TrackobotUploader::importAccount(QString fileName)
{
    if(loadAccount(fileName))   saveAccount();
}


void TrackobotUploader::importAccount(QByteArray jsonData)
{
    if(loadAccount(jsonData))   saveAccount();
}


bool TrackobotUploader::loadAccount(QString fileName)
{
    if(fileName.isEmpty())  fileName = Utility::dataPath() + "/" + TRACKOBOT_ACCOUNT_FILE;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open " + QString(TRACKOBOT_ACCOUNT_FILE) + " file.");
        return false;
    }

    QDataStream in(&file);
    QString username, password, webserviceUrl;
    in.setVersion(QDataStream::Qt_4_8);
    in >> username;
    in >> password;
    in >> webserviceUrl;

    if(!username.isEmpty() && !password.isEmpty())
    {
        this->username = username;
        this->password = password;
        this->connectSuccess = true;
        emit pDebug("Account " + this->username + " --> Loaded.");
        emit connected(username, password);
        return true;
    }
    else
    {
        this->connectSuccess = false;
        emit pDebug(fileName + " file has an invalid format.");
        emit showMessageProgressBar("Invalid track-o-bot account");
        emit disconnected();
        return false;
    }
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


//TODO verificar trackobot acepta "demonhunter"
void TrackobotUploader::uploadResult(GameResult gameResult, LoadingScreenState loadingScreen,
                                     qint64 startGameEpoch, QDateTime dateTime, QJsonArray cardHistory)
{
    QJsonObject result;
    result["coin"]          = !gameResult.isFirst;
    result["hero"]          = Utility::classLogNumber2classLName(gameResult.playerHero);
    result["opponent"]      = Utility::classLogNumber2classLName(gameResult.enemyHero);
    result["win"]           = gameResult.isWinner;
    result["mode"]          = Utility::getLoadingScreenToString(loadingScreen).toLower();
    result["duration"]      = QDateTime::currentMSecsSinceEpoch()/1000 - startGameEpoch;
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

    QString text =  Utility::classLogNumber2classULName(gameResult.playerHero) + " vs " +
                    Utility::classLogNumber2classULName(gameResult.enemyHero) + " uploaded";
    if(arenaItemXlsList.isEmpty())  emit showMessageProgressBar(text);
}


QString TrackobotUploader::getStringCellXls(struct st_row::st_row_data* row, int col)
{
    if(col < row->fcell || col >= row->lcell)   return "";
    struct st_cell::st_cell_data* cell = &row->cells.cell[col];
    if(cell->id != 0x0201 && cell->str != nullptr)
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
                arenaItem.gameResult.enemyHero = Utility::className2classLogNumber(
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
                playerHero = Utility::className2classLogNumber(getStringCellXls(row, 1).toLower());
            }
            else
            {
                playerHero = "";
            }
        }
    }

    xls_close_WS(pWS);
    emit pDebug("Extracted " + QString::number(arenaItemList.count()) + " games from the XLS file.");

    return arenaItemList;
}


void TrackobotUploader::uploadXls(QString fileName)
{
    if(!arenaItemXlsList.isEmpty())
    {
        emit pDebug("Trying to upload a new Xls file while still busy with the last one.", Warning);
        return;
    }

    xlsWorkBook* pWB = xls_open(fileName.toUtf8(),"UTF-8");

    if(pWB == nullptr)
    {
        emit pDebug("Xls invalid file: " + fileName, Warning);
        emit showMessageProgressBar("XLS invalid");
    }
    else
    {
        arenaItemXlsList = extractXls(pWB);
        xls_close_WB(pWB);
        if(arenaItemXlsList.isEmpty())  return;

        emit startProgressBar(arenaItemXlsList.count(), "");
        uploadNextXlsResult();
    }
}




