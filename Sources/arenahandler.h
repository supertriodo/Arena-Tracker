#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "deckhandler.h"
#include "planhandler.h"
#include <QObject>
#include <QTreeWidgetItem>
#include <QJsonObject>

#define USER_GUIDE_URL          "https://triodo.gitbook.io/arena-tracker-documentation/en"

class ArenaHandler : public QObject
{
    Q_OBJECT
public:
    ArenaHandler(QObject *parent, DeckHandler *deckHandler, PlanHandler *planHandler, Ui::Extended *ui);
    ~ArenaHandler();

//Variables
private:
    DeckHandler *deckHandler;
    PlanHandler *planHandler;
    Ui::Extended *ui;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent;
    QTreeWidgetItem *rankedTreeItem[NUM_HEROS];
    QTreeWidgetItem *adventureTreeItem, *tavernBrawlTreeItem, *friendlyTreeItem, *casualTreeItem;
    QString arenaCurrentHero;
    QString arenaCurrentDraftFile;
    bool mouseInApp;
    Transparency transparency;
    QJsonObject statsJson;


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    QColor getRowColor(QTreeWidgetItem *item);
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, bool addAtStart, int wins=0, int losses=0);
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void showArena(QString hero, QString title="Arena", int wins=0, int losses=0);
    void redrawRow(QTreeWidgetItem *item);
    void redrawAllRows();
    void newArenaStat(QString hero, int wins=0, int losses=0);
    void newArenaGameStat(GameResult gameResult);
    void saveStatsJsonFile();

public:
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme();
    void linkDraftLogToArenaCurrent(QString logFileName);
    QString getArenaCurrentDraftLog();
    void loadStatsJsonFile();

signals:
    void showPremiumDialog();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void newArena(QString hero);

    //MainWindow
    void setPremium(bool premium);

private slots:
    void openUserGuide();
};

#endif // ARENAHANDLER_H
