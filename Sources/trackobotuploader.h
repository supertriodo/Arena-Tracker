#ifndef TRACKOBOTUPLOADER_H
#define TRACKOBOTUPLOADER_H

#include "utility.h"
#include "gamewatcher.h"
#include "LibXls/xls.h"
#include "Widgets/ui_extended.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>


#define TRACKOBOT_NEWUSER_URL "https://trackobot.com/users.json"
#define TRACKOBOT_RESULTS_URL "https://trackobot.com/profile/results.json"
#define TRACKOBOT_PROFILE_URL "https://trackobot.com/one_time_auth.json"
#define TRACKOBOT_ACCOUNT_FILE "Account.track-o-bot"

using namespace xls;


class ArenaItem
{
public:
    QDateTime dateTime;
    GameResult gameResult;
};

class TrackobotUploader : public QObject
{
    Q_OBJECT
public:
    TrackobotUploader(QObject *parent);
    ~TrackobotUploader();

//Variables
private:
    QNetworkAccessManager *networkManager;
    QString username, password;
    bool connected;
    QList<ArenaItem> arenaItemXlsList;


//Metodos
private:
    bool loadAccount(QString fileName="");
    void saveAccount();
    bool loadAccount(QByteArray jsonData);
    QString credentials();
    QString getStringCellXls(st_row::st_row_data *row, int col);
    bool isRowGameXls(st_row::st_row_data *row);
    QDateTime getRowDateXls(st_row::st_row_data *row);
    QList<ArenaItem> extractXls(xlsWorkBook *pWB);
    void uploadNextXlsResult();

public:
    bool isConnected();
    void openTBProfile();
    void uploadResult(GameResult gameResult, LoadingScreenState loadingScreen, qint64 startGameEpoch, QDateTime dateTime, QJsonArray cardHistory);
    QString getUsername();
    void uploadXls(QString fileName);
    void importAccount(QString fileName);

signals:
    void advanceProgressBar(int remaining, QString text);
    void startProgressBar(int maximum, QString text);
    void showMessageProgressBar(QString text);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="TrackobotUploader");

public slots:

private slots:
    void checkAccount();
    void replyFinished(QNetworkReply *reply);
};

#endif // TRACKOBOTUPLOADER_H
