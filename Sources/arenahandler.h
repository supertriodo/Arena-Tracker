#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "deckhandler.h"
#include "utility.h"
#include "trackobotuploader.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QTreeWidgetItem>

#define Z2H_UPLOAD_DRAFT_URL    "https://www.zerotoheroes.com/api/hearthstone/upload/draft/"
#define Z2H_UPLOAD_GAME_URL     "https://www.zerotoheroes.com/api/hearthstone/upload/game/"
#define Z2H_VIEW_REPLAY_URL     "https://www.zerotoheroes.com/r/hearthstone/"
#define USER_GUIDE_URL          "https://arena-tracker.gitbooks.io/arena-tracker-documentation/content/en/"

class ArenaHandler : public QObject
{
    Q_OBJECT
public:
    ArenaHandler(QObject *parent, DeckHandler *deckHandler, TrackobotUploader *trackobotUploader, PlanHandler *planHandler, Ui::Extended *ui);
    ~ArenaHandler();

//Variables
private:
    TrackobotUploader *trackobotUploader;
    DeckHandler *deckHandler;
    PlanHandler *planHandler;
    Ui::Extended *ui;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent;
    QTreeWidgetItem *rankedTreeItem[9];
    QTreeWidgetItem *adventureTreeItem, *tavernBrawlTreeItem, *friendlyTreeItem, *casualTreeItem;
    QString arenaCurrentHero;
    QMap<QTreeWidgetItem *, QString> replayLogsMap;
    QTreeWidgetItem *lastReplayUploaded;
    bool mouseInApp;
    Transparency transparency;
    QNetworkAccessManager *networkManager;


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    QColor getRowColor(QTreeWidgetItem *item);
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, bool addAtEnd);
    QString compressLog(QString logFileName);
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void showArena(QString hero);
    void completeRewardsUI();
    bool isOnZ2H(QString &logFileName, QRegularExpressionMatch &match);
    void redrawRow(QTreeWidgetItem *item);
    void redrawAllRows();

public:
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme();
    void deselectRow();
    void linkDraftLogToArenaCurrent(QString logFileName);
    QString getArenaCurrentDraftLog();
    QTreeWidgetItem *showGameResultLog(const QString &logFileName);
    void showArenaLog(const QString &logFileName);
    void clearAllGames();

signals:
    void showMessageProgressBar(QString text);
    void showPremiumDialog();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName, qint64 startGameEpoch);
    bool newArena(QString hero);
    void showRewards();

    //NetworkManager
    void replyFinished(QNetworkReply *reply);

    //MainWindow
    void setPremium(bool premium);

private slots:
    void hideRewards();
    void changedRow(QTreeWidgetItem *current);
    void replayLog();
    void openTBProfile();
    void openUserGuide();
};

#endif // ARENAHANDLER_H
