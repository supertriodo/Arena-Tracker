#ifndef PLANHANDLER_H
#define PLANHANDLER_H

#include "Widgets/ui_extended.h"
#include "Widgets/miniongraphicsitem.h"
#include "Widgets/herographicsitem.h"
#include "Widgets/arrowgraphicsitem.h"
#include "Cards/handcard.h"
#include "utility.h"
#include <QObject>


class TagChange
{
public:
    int id;
    bool friendly;
    QString tag, value;
};


class ArmorRemover
{
public:
    int idAddon, idHero;
};


class Board
{
public:
    QList<MinionGraphicsItem *> playerMinions, enemyMinions;
    HeroGraphicsItem * playerHero = NULL;
    HeroGraphicsItem * enemyHero = NULL;
    QList<ArrowGraphicsItem *> arrows;
    QList<QString> playerHandList;
    QList<HandCard> enemyHandList;
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
    qint64 lastMinionAddedTime=0;
    int lastTriggerId;//-1 --> no trigger       //Used to create reinforcements. Guarda ultimo trigger o power con obj
    Addon lastPowerAddon;//Id=-1 --> no power   //Used to create addons. Guarda ultimo trigger, power o fatigue
    qint64 lastPowerTime=0;
    ArmorRemover lastArmorRemoverIds;//idAddon=-1 -->None      //Evita doble damage en heroe (armadura y damage)
    bool inGame;
    bool mouseInApp;
    Transparency transparency;

//Metodos:
private:
    void updateTransparency();
    void updateZoneSpots(bool friendly, Board *board=NULL);
    QList<MinionGraphicsItem *> *getMinionList(bool friendly, Board *board=NULL);
    HeroGraphicsItem *getHero(bool friendly, Board *board=NULL);
    int findMinionPos(QList<MinionGraphicsItem *> *minionsList, int id);
    MinionGraphicsItem *findMinion(bool friendly, int id, Board *board=NULL);
    void addMinion(bool friendly, QString code, int id, int pos);
    void addMinion(bool friendly, MinionGraphicsItem *minion, int pos);
    void addMinionTriggered(bool friendly, QString code, int id, int pos);
    void copyMinionToLastTurn(bool friendly, MinionGraphicsItem *minion, int idCreator=-1);
    void addReinforceToLastTurn(MinionGraphicsItem *parent, MinionGraphicsItem *child, Board *board);
    void updateMinionPos(bool friendly, int id, int pos);
    void removeMinion(bool friendly, int id);
    void addTagChange(int id, bool friendly, QString tag, QString value);
    void stealMinion(bool friendly, int id, int pos);
    MinionGraphicsItem *takeMinion(bool friendly, int id, bool stolen=false);
    void addHero(bool friendly, QString code, int id);
    void removeHero(Board *board, bool friendly);
    void resetBoard(Board *board);
    void updateButtons();
    void loadViewBoard();
    void completeUI();
    bool findAttackPoint(ArrowGraphicsItem *attack, bool isFrom, int id, Board *board);
    bool appendAttack(ArrowGraphicsItem *attack, Board *board);
    void addAddonToLastTurn(QString code, int id1, int id2, Addon::AddonType type, int number=1);
    void addWeaponAddonToLastTurn(bool friendly, QString code, int id);
    void addAddon(MinionGraphicsItem *minion, QString code, int id, Addon::AddonType type, int number=1);
    void addHeroDeadToLastTurn(bool playerWon);
    bool isLastPowerAddonValid(QString tag, QString value, int idTarget, bool friendly, bool isHero, bool healing);
    bool isLastMinionAddedValid();
    bool isLastTriggerValid(QString code);    
    bool areThereAuras(bool friendly);    
    bool isAddonMinionValid(QString code);
    bool isAddonHeroValid(QString code);
    bool isAddonCommonValid(QString code);    
    SecretHero cardclassToSecrethero(CardClass cardClass);    
    void checkAtkHealthChange(MinionGraphicsItem *minion, bool friendly, QString tag, QString value);

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
    void playerMinionZonePlayAddTriggered(QString code, int id, int pos);
    void enemyMinionZonePlayAddTriggered(QString code, int id, int pos);
    void playerMinionZonePlaySteal(int id, int pos);
    void enemyMinionZonePlaySteal(int id, int pos);
    void playerMinionZonePlayRemove(int id);
    void enemyMinionZonePlayRemove(int id);
    void playerMinionPosChange(int id, int pos);
    void enemyMinionPosChange(int id, int pos);
    void playerMinionTagChange(int id, QString code, QString tag, QString value);
    void enemyMinionTagChange(int id, QString code, QString tag, QString value);
    void playerHeroZonePlayAdd(QString code, int id);
    void enemyHeroZonePlayAdd(QString code, int id);
    void playerWeaponZonePlayAdd(QString code, int id);
    void enemyWeaponZonePlayAdd(QString code, int id);
    void zonePlayAttack(QString code, int id1, int id2);
    void playerCardObjPlayed(QString code, int id1, int id2);
    void enemyCardObjPlayed(QString code, int id1, int id2);
    void newTurn(bool playerTurn, int numTurn);
    void setLastTriggerId(QString code, QString blockType, int id, int idTarget);
    void lockPlanInterface();
    void unlockPlanInterface();
    void resetLastPowerAddon();
    void endGame(bool playerWon);
    void playerSecretPlayed(int id, QString code);
    void enemySecretPlayed(int id, SecretHero secretHero);
    void playerSecretRevealed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSecretStolen(int id, QString code);
    void enemySecretStolen(int id, QString code);

private slots:
    void checkPendingTagChanges();
    void showNextTurn();
    void showPrevTurn();
};

#endif // PLANHANDLER_H
