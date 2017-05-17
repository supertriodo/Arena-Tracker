#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "webuploader.h"
#include "deckhandler.h"
#include "utility.h"
#include "trackobotuploader.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QTreeWidgetItem>

class ArenaHandler : public QObject
{
    Q_OBJECT
public:
    ArenaHandler(QObject *parent, DeckHandler *deckHandler, TrackobotUploader *trackobotUploader, PlanHandler *planHandler, Ui::Extended *ui);
    ~ArenaHandler();

//Variables
private:
    WebUploader *webUploader;
    TrackobotUploader *trackobotUploader;
    DeckHandler *deckHandler;
    PlanHandler *planHandler;
    Ui::Extended *ui;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent, *arenaPrevious;
    QTreeWidgetItem *constructedTreeItem[9];
    QTreeWidgetItem *adventureTreeItem, *tavernBrawlTreeItem, *friendlyTreeItem;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    QList<GameResult> arenaPreviousGameList; //Se usa en removeDuplicateArena
    QMap<QTreeWidgetItem *, QString> replayLogsMap;
    QTreeWidgetItem *lastReplayUploaded;
    bool noArena;
    bool mouseInApp;
    Transparency transparency;
    Theme theme;
    QNetworkAccessManager *networkManager;
    bool connectedAM;


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    QColor getRowColor(QTreeWidgetItem *item);
    bool isRowOk(QTreeWidgetItem *item);
    void allToWhite();
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, bool addAtEnd);
    QString compressLog(QString logFileName);

public:
    void setWebUploader(WebUploader *webUploader);
    bool isNoArena();
    void currentArenaToWhite();
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme(Theme theme);
    void deselectRow();
    void linkLogToDraft(QString logFileName);
    QString getArenaCurrentGameLog();
    void setConnectedAM(bool value);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName, qint64 startGameEpoch);
    bool newArena(QString hero);
    void showRewards();

    //WebUploader
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void showArena(QString hero);
    void reshowGameResult(GameResult gameResult);
    void reshowArena(QString hero);
    void showNoArena();
    void enableRefreshButton(bool enable=true);
    void syncArenaCurrent();
    void removeDuplicateArena();
    void linkLogsToWebGames();

    //NetworkManager
    void replyFinished(QNetworkReply *reply);

private slots:
    void refresh();
    void openDonateWeb();
    void hideRewards();
    void uploadRewards();
    void changedRow(QTreeWidgetItem *current);
    void replayLog();
    void openTBProfile();
};

#endif // ARENAHANDLER_H
