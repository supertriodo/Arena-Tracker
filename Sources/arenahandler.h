#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "Widgets/ui_extended.h"
#include "gamewatcher.h"
#include "deckhandler.h"
#include "planhandler.h"
#include <QObject>
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QDate>

#define ARENA_YELLOW            QColor(255, 255, 61)
#define ARENA_GREEN             QColor(61, 255, 61)
#define ARENA_RED               QColor(255, 61, 61)
#define ARENA_BLUE              QColor(122, 122, 255)
#define ARENA_WRONG             Qt::darkGray
#define NUM_REGIONS             5
#define NUM_BEST_ARENAS         30

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
    QTreeWidgetItem *arenaCurrent;
    QTreeWidgetItem *rankedTreeItem[NUM_HEROS];
    QTreeWidgetItem *adventureTreeItem, *tavernBrawlTreeItem, *friendlyTreeItem, *casualTreeItem;
    QTreeWidgetItem *winrateTreeItem;
    QTreeWidgetItem *winrateClassTreeItem[NUM_HEROS];
    QTreeWidgetItem *best30TreeItem;
    QTreeWidgetItem *best30RegionTreeItem[NUM_REGIONS];
    QString arenaCurrentHero;
    bool mouseInApp;
    bool premium;
    Transparency transparency;
    QJsonObject statsJson;
    QString statsJsonFile;
    //Connection QTreeWidgetItem -- key in ArenaTrackerStats.json (date/"current"). Se usa para editar la tabla.
    QMap<QTreeWidgetItem *, QString> arenaStatLink;
    QRegularExpressionMatch *match;
    //Se usa para que itemChanged solo actue cuando el usuario modifica un campo, no cuando el codigo lo hace.
    bool editingColumnText;
    int lastRegion;
    DraftMethod draftMethodAvgScore;
    QFutureWatcher<void> futureProcessArenas2Stats;



//Metodos
private:
    void completeUI();
    void createArenaTreeWidget();
    void createArenaStatsTreeWidget();
    void createComboBoxArenaRegion();
    void createComboBoxArenaStatsJson();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    void setRowColor(QTreeWidgetItem *item, int region=-1);
    void setColorWrongArena(QTreeWidgetItem *item);
    bool isCompleteArena(int wins, int losses);
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, int wins=0, int losses=0, int avgHA=0, float avgHSR=0,
                                        bool isArena=false, bool insertPos1=false);
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen, int avgHA, float avgHSR);
    QTreeWidgetItem *showArena(QString hero, QString title="", int wins=0, int losses=0, int avgHA=0, float avgHSR=0,
                               bool isArenaNewEmpty=false);
    void newArenaStat(QString hero, int wins=0, int losses=0, QTreeWidgetItem *item=nullptr);
    void newArenaGameStat(GameResult gameResult);
    void saveStatsJsonFile();
    QString getColumnText(QTreeWidgetItem *item, int col);
    void setColumnText(QTreeWidgetItem *item, int col, const QString &text);
    void setColumnIcon(QTreeWidgetItem *item, int col, const QIcon &aicon);
    void itemChangedDate(QTreeWidgetItem *item, int column);
    void itemChangedHero(QTreeWidgetItem *item, int column);
    void itemChangedWL(QTreeWidgetItem *item, int column);
    QString getUniqueDate(QString date);
    void completeButtons();
    void showArenaStatsTreeWidget();
    void hideArenaStatsTreeWidget();
    void setJsonExtra(QString key, QString value);
    QString getJsonExtra(QString key);
    QString getJsonExtraRegion(int i);
    int getRegionTreeItemIndex(QTreeWidgetItem *item);
    void loadRegionNames();
    void processArenas2Stats();
    void showArenas2StatsClass(int classRuns[], int classWins[], int classLost[]);
    void showArenas2StatsBest30(int best30Runs[], int best30BestWins[], QString best30Start[], QString best30End[]);
    void startProcessArenas2Stats();
    void checkNewPeriod();
    QString isNewPeriod(const QDateTime &leftD, const QDateTime &rightD);
    void setCurrentStatsJson();
    void setNullTreeItems();
    ScoreSource scoreSourceFromDraftMethod(DraftMethod draftMethod);
    void loadSelectedStatsJsonFile();
    bool processWinratesFromFile(int classRuns[], int classWins[], int classLost[], QJsonObject &jsonObject, const QDate &rotationDate);
    bool jsonObjectFromFile(QJsonObject &jsonObject, const QString &statsFile);
    void processWinratesFromDir(const QDate &rotationDate);
    void loadStatsJsonFile(const QString &statsFile="ArenaTrackerStats.json");

public:
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setTheme();
    void setDraftMethodAvgScore(DraftMethod draftMethodAvgScore);
    void processPlayerWinrates();

signals:
    void showPremiumDialog();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher/MainWindow
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, int avgHA=0, float avgHSR=0);

    //DraftHandler
    void newArena(QString hero);
    void setCurrentAvgScore(int avgHA, float avgHSR, QString heroLog);

    //MainWindow
    void setPremium(bool premium, bool load=true);

private slots:
    void itemChanged(QTreeWidgetItem *item, int column);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void regionChanged(int index);
    void itemSelectionChanged();
    void arenaNewEmpty();
    void arenaDelete();
    void toggleArenaStatsTreeWidget();
    void finishShowArenaStatsTreeWidget();
    void finishHideArenaTreeWidget();
    void finishHideArenaStatsTreeWidget();
    void finishShowArenaTreeWidget();
    void statItemDoubleClicked(QTreeWidgetItem *item, int column);
    void statItemChanged(QTreeWidgetItem *item, int column);
    void deselectRow();
    void arenaStatsJsonChanged(int index);
};

#endif // ARENAHANDLER_H
