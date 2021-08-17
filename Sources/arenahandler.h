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
#define ARENA_YELLOW            QColor(255, 255, 61)
#define ARENA_GREEN             QColor(61, 255, 61)
#define ARENA_RED               QColor(255, 61, 61)
#define ARENA_BLUE              QColor(122, 122, 255)

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
    QString arenaCurrentHero;
    QString arenaCurrentDraftFile;
    bool mouseInApp;
    Transparency transparency;
    QJsonObject statsJson;
    //Connection QTreeWidgetItem -- key in ArenaTrackerStats.json (date/"current"). Se usa para editar la tabla.
    QMap<QTreeWidgetItem *, QString> arenaStatLink;
    QRegularExpressionMatch *match;
    //Se usa para que itemChanged solo actue cuando el usuario modifica un campo, no cuando el codigo lo hace.
    bool editingColumnText;
    int lastRegion;


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void createComboBoxArenaRegion();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    void setRowColor(QTreeWidgetItem *item);
    QColor getRowColor(QTreeWidgetItem *item);
    QTreeWidgetItem *createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen);
    void updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem);
    QTreeWidgetItem *createTopLevelItem(QString title, QString hero, bool addAtStart, int wins=0, int losses=0, bool isArena=false);
    QTreeWidgetItem *showGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void showArena(QString hero, QString title="", int wins=0, int losses=0);
    void redrawRow(QTreeWidgetItem *item);
    void newArenaStat(QString hero, int wins=0, int losses=0);
    void newArenaGameStat(GameResult gameResult);
    void saveStatsJsonFile();
    void setColumnText(QTreeWidgetItem *item, int col, const QString &text);
    void setColumnIcon(QTreeWidgetItem *item, int col, const QIcon &aicon);
    void itemChangedDate(QTreeWidgetItem *item, int column);
    void itemChangedHero(QTreeWidgetItem *item, int column);
    void itemChangedWL(QTreeWidgetItem *item, int column);

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
    void itemChanged(QTreeWidgetItem *item, int column);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void regionChanged(int index);
    void itemSelectionChanged();
};

#endif // ARENAHANDLER_H
