#ifndef PLANHANDLER_H
#define PLANHANDLER_H

#include "Widgets/ui_extended.h"
#include "Widgets/miniongraphicsitem.h"
#include "Widgets/herographicsitem.h"
#include "utility.h"
#include <QObject>


class TagChange
{
public:
    int id;
    bool friendly;
    QString tag, value;
};

class Board
{
public:
    QList<MinionGraphicsItem *> playerMinions, enemyMinions;
    HeroGraphicsItem * playerHero = NULL;
    HeroGraphicsItem * enemyHero = NULL;
    bool playerTurn;
    int numTurn = 0;//0 --> nowBoard
};

class PlanHandler : public QObject
{
    Q_OBJECT
public:
    PlanHandler(QObject *parent, Ui::Extended *ui);
    ~PlanHandler();

//Variables
private:
    Ui::Extended *ui;
    QList<TagChange> pendingTagChanges;
    Board *nowBoard;
    Board *viewBoard;
    QList<Board *> turnBoards;
    int firstStoredTurn;// 0 unset
    MinionGraphicsItem * lastMinionAdded;
    bool inGame;
    bool mouseInApp;
    Transparency transparency;

//Metodos:
private:
    void updateTransparency();
    void updateZoneSpots(bool friendly);
    QList<MinionGraphicsItem *> *getMinionList(bool friendly);
    int findMinionPos(QList<MinionGraphicsItem *> *minionsList, int id);
    MinionGraphicsItem *findMinion(bool friendly, int id);
    void addMinion(bool friendly, QString code, int id, int pos);
    void addMinion(bool friendly, MinionGraphicsItem *minion, int pos);
    void updateMinionPos(bool friendly, int id, int pos);
    void removeMinion(bool friendly, int id);
    void addTagChange(int id, bool friendly, QString tag, QString value);
    void stealMinion(bool friendly, int id, int pos);
    MinionGraphicsItem *takeMinion(bool friendly, int id);
    void addHero(bool friendly, QString code, int id);
    void removeHero(Board *board, bool friendly);
    void resetBoard(Board *board);
    void updateButtons();
    void loadViewBoard();
    void completeUI();

public:
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    void redrawDownloadedCardImage(QString code);
    void reset();

signals:
    void checkCardImage(QString code, bool isHero);
//    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="PlanHandler");

public slots:
    void playerMinionZonePlayAdd(QString code, int id, int pos);
    void enemyMinionZonePlayAdd(QString code, int id, int pos);
    void playerMinionZonePlaySteal(int id, int pos);
    void enemyMinionZonePlaySteal(int id, int pos);
    void playerMinionZonePlayRemove(int id);
    void enemyMinionZonePlayRemove(int id);
    void playerMinionPosChange(int id, int pos);
    void enemyMinionPosChange(int id, int pos);
    void playerMinionTagChange(int id, QString tag, QString value);
    void enemyMinionTagChange(int id, QString tag, QString value);
    void playerHeroZonePlayAdd(QString code, int id);
    void enemyHeroZonePlayAdd(QString code, int id);
    void newTurn(bool playerTurn, int numTurn);
    void lockPlanInterface();
    void unlockPlanInterface();

private slots:
    void checkPendingTagChanges();
    void showNextTurn();
    void showPrevTurn();
};

#endif // PLANHANDLER_H
