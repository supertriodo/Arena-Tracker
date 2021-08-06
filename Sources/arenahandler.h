#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "deckhandler.h"
#include "planhandler.h"
#include <QObject>
#include <QTreeWidgetItem>

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


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    QColor getRowColor(QTreeWidgetItem *item);
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, bool addAtStart);
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void showArena(QString hero);
    void redrawRow(QTreeWidgetItem *item);
    void redrawAllRows();

public:
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme();
    void linkDraftLogToArenaCurrent(QString logFileName);
    QString getArenaCurrentDraftLog();

signals:
    void showPremiumDialog();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    bool newArena(QString hero);

    //MainWindow
    void setPremium(bool premium);

private slots:
    void openUserGuide();
};

#endif // ARENAHANDLER_H
