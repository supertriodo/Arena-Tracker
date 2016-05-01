#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "webuploader.h"
#include "deckhandler.h"
#include "utility.h"
#include <QObject>
#include <QTreeWidgetItem>

class ArenaHandler : public QObject
{
    Q_OBJECT
public:
    ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::Extended *ui);
    ~ArenaHandler();

//Variables
private:
    WebUploader *webUploader;
    DeckHandler *deckHandler;
    Ui::Extended *ui;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent, *arenaPrevious;
    QTreeWidgetItem *constructedTreeItem[9];
    QTreeWidgetItem *adventureTreeItem, *tavernBrawlTreeItem;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    QList<GameResult> arenaPreviousGameList; //Se usa en removeDuplicateArena
    QMap<QTreeWidgetItem *, QString> replayLogsMap;
    bool noArena;
    bool mouseInApp;
    Transparency transparency;
    Theme theme;


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

public:
    void setWebUploader(WebUploader *webUploader);
    bool isNoArena();
    void currentArenaToWhite();
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme(Theme theme);
    void deselectRow();
    void linkLogToDraft(QString logFileName);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName);
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

private slots:
    void refresh();
    void openDonateWeb();
    void hideRewards();
    void uploadRewards();
    void openAMWeb();
    void changedRow(QTreeWidgetItem *current);
    void replayLog();
};

#endif // ARENAHANDLER_H
