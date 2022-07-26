#ifndef PLANHANDLER_H
#define PLANHANDLER_H

#include "Widgets/ui_extended.h"
#include "Widgets/GraphicItems/miniongraphicsitem.h"
#include "Widgets/GraphicItems/herographicsitem.h"
#include "Widgets/GraphicItems/weapongraphicsitem.h"
#include "Widgets/GraphicItems/heropowergraphicsitem.h"
#include "Widgets/GraphicItems/arrowgraphicsitem.h"
#include "Widgets/GraphicItems/cardgraphicsitem.h"
#include "Widgets/GraphicItems/graphicsitemsender.h"
#include "Cards/handcard.h"
#include "utility.h"
#include <QObject>
#include <QFutureWatcher>


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


class DeadProbs
{
public:
    QList<float> dp;
    bool playerIn, onlyMinions;
};


class Board
{
public:
    QList<MinionGraphicsItem *> playerMinions, enemyMinions;
    HeroGraphicsItem * playerHero = nullptr;
    HeroGraphicsItem * enemyHero = nullptr;
    HeroPowerGraphicsItem * playerHeroPower = nullptr;
    HeroPowerGraphicsItem * enemyHeroPower = nullptr;
    WeaponGraphicsItem * playerWeapon = nullptr;
    WeaponGraphicsItem * enemyWeapon = nullptr;
    QList<ArrowGraphicsItem *> arrows;
    QList<CardGraphicsItem *> playerHandList;
    QList<CardGraphicsItem *> enemyHandList;
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
    QMap<int,TagChange> pendingTagChanges;
    Board *nowBoard;
    Board *viewBoard;
    Board *futureBoard;
    QList<Board *> turnBoards;
    int firstStoredTurn;// 0 unset
    MinionGraphicsItem * lastMinionAdded;
    MinionGraphicsItem * selectedMinion;
    QString selectedCode;
    qint64 lastMinionAddedTime=0;
    int lastTriggerId;//-1 --> no trigger       //Used to create reinforcements. Guarda ultimo trigger o power con obj
    Addon lastPowerAddon;//Id=-1 --> no power   //Used to create addons. Guarda ultimo trigger, power o fatigue
    qint64 lastPowerTime=0;
    ArmorRemover lastArmorRemoverIds;//idAddon=-1 -->None      //Evita doble damage en heroe (armadura y damage)
    bool sizePlan;
    bool inGame;
    bool mouseInApp;
    Transparency transparency;
    GraphicsItemSender *graphicsItemSender;
    QFutureWatcher<DeadProbs> futureBombs;
    bool abortFutureBombs;
    QPushButton *planPatreonButton;

//Metodos:
private:
    void updateTransparency();
    void updateMinionZoneSpots(bool friendly, Board *board = nullptr);
    QList<MinionGraphicsItem *> *getMinionList(bool friendly, Board *board = nullptr);
    HeroGraphicsItem *getHero(bool friendly, Board *board = nullptr);
    int findMinionPos(QList<MinionGraphicsItem *> *minionsList, int id);
    MinionGraphicsItem *findMinion(bool friendly, int id, Board *board = nullptr);
    void addMinion(bool friendly, QString code, int id, int pos);
    void addMinion(bool friendly, MinionGraphicsItem *minion, int pos);
    void addMinionTriggered(bool friendly, QString code, int id, int pos);
    void copyMinionToLastTurn(bool friendly, MinionGraphicsItem *minion, int idCreator=-1);
    void addReinforceToLastTurn(MinionGraphicsItem *parent, MinionGraphicsItem *child, Board *board);
    void updateMinionPos(bool friendly, int id, int pos);
    void removeMinion(bool friendly, int id);
    void addBoardTagChange(int id, bool friendly, QString tag, QString value);
    void addCardTagChange(const TagChange &tagChange, CardGraphicsItem *card);
    void addMinionTagChange(const TagChange &tagChange, MinionGraphicsItem *minion);
    void addHeroTagChange(const TagChange &tagChange);
    void addHeroPowerTagChange(const TagChange &tagChange);
    void addWeaponTagChange(const TagChange &tagChange);
    void addAddonTagChange(const TagChange &tagChange, bool healing, bool isDead, bool isHero);
    void stealMinion(bool friendly, int id, int pos);
    MinionGraphicsItem *takeMinion(bool friendly, int id, bool stolen=false);
    void addHero(bool friendly, QString code, int id);
    void removeHero(bool friendly, Board *board = nullptr);
    void resetBoard(Board *board);
    void updateTurnLabel();
    void loadViewBoard();
    void completeUI();
    bool findAttackPoint(ArrowGraphicsItem *attack, bool isFrom, int id, Board *board);
    bool appendAttack(ArrowGraphicsItem *attack, Board *board);
    void addAddonToLastTurn(QString code, int id1, int id2, Addon::AddonType type, int number=1);
    void addAddon(MinionGraphicsItem *minion, QString code, int id, Addon::AddonType type, int number=1);
    void addHeroDeadToLastTurn(bool playerWon);
    bool isLastPowerAddonValid(QString tag, QString value, int idTarget, bool friendly, bool isHero, bool healing);
    bool isLastMinionAddedValid();
    bool isLastTriggerValid(const QString &code);
    bool areThereAuras(bool friendly);    
    bool isAddonMinionValid(const QString &code);
    bool isAddonHeroValid(const QString &code);
    bool isAddonCommonValid(const QString &code);
    void checkAtkHealthChange(MinionGraphicsItem *minion, bool friendly, QString tag, QString value);    
    void updateCardZoneSpots(bool friendly, Board *board = nullptr);
    int findCardPos(QList<CardGraphicsItem *> *cardsList, int id);
    QList<CardGraphicsItem *> *getHandList(bool friendly, Board *board = nullptr);
    CardGraphicsItem *findCard(bool friendly, int id, Board *board = nullptr);
    CardGraphicsItem *cardDraw(bool friendly, int id, QString code, QString createdByCode, int turn);
    void cardPlayed(bool friendly, int id, QString code, bool discard);
    void revealEnemyCardPrevTurns(int id, QString code);    
    void addWeapon(bool friendly, QString code, int id);
    void removeWeapon(bool friendly, int id=-1, Board *board = nullptr);
    void killWeaponLastTurn(bool friendly, int id);
    void removeHeroPower(bool friendly, Board *board);
    void addHeroPower(bool friendly, QString code, int id);
    void updateMinionsAttack(bool friendly, Board *board = nullptr);
    void fixTurn1Card();    
    void fixLastEchoCard();
    void addTagChange(bool friendly, QString tag, QString value);
    bool getWinner();
    void cardTagChangePrevTurn(int id, bool friendly, QString tag, QString value);
    bool updateInPendingTagChange(int id, QString tag, QString value);
    void createGraphicsItemSender();
    QMap<QString, float> bomb(QMap<QString, float> &oldStates, int missileDamage);
    QList<int> decodeBombState(QString state);
    QString encodeBombState(QList<int> targets);
    DeadProbs bombDeads(QList<int> targets, bool playerIn, bool onlyMinions, int missiles, int missileDamage);
    bool isCardBomb(QString code, bool &playerIn, bool &onlyMinions, int &missiles, int &missileDamage);
    int numRaceOnBoard(CardRace cardRace);
    int flamewakersOnBoard();
    void updateTurnSliderRange();
    Board *copyBoard(Board *origBoard, int numTurn = 0, bool copySecretCodes=false);
    void deleteFutureBoard();
    void createFutureBoard();
    void updateMinionFromCard(MinionGraphicsItem *minion);
    int getPotentialMana(Board *board);
    void showManaPlayableCards(Board *board);
    int getPotentialManaNextTurn();
    void showManaPlayableCardsNextTurn();
    void showManaPlayableCardsAuto();

public:
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    void redrawDownloadedCardImage(QString code);
    void reset();
    bool resetSizePlan();
    bool isSizePlan();
    bool isCardBomb(QString code);
    QJsonArray getJsonCardHistory();
    void setTheme();
    bool isMinionOnBoard(bool friendly, int id, Board *board = nullptr);

    //SecretsHandler
    bool isReckoningTested(bool isHeroTo, int id1, int id2);
    bool isEnemyHeroHealthChanged();

signals:
    void checkCardImage(QString code, bool isHero);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void secretEntered(int id, QRect &rectCard, int maxTop, int maxBottom);
    void cardLeave();
    void needMainWindowFade(bool fade);
    void heroTotalAttackChange(bool friendly, int totalAttack, int totalMaxAttack);
    void showPremiumDialog();
    void swapSize(bool customSize);
    void playerAllManaSpent();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="PlanHandler");

private slots:
    void updateViewCardZoneSpots();
    void setDeadProbs();
    void showSliderTurn(int turn);
    void showFirstTurn();
    void showPrevTurn();
    void showNextTurn();
    void cardPress(CardGraphicsItem *card, Qt::MouseButton mouseButton);
    void heroPowerPress(HeroPowerGraphicsItem *heroPower, Qt::MouseButton mouseButton);
    void minionPress(MinionGraphicsItem *minion, Qt::MouseButton mouseButton);
    void heroPress(HeroGraphicsItem *hero, Qt::MouseButton mouseButton);
    void showCardTooltip(QString code, QRect rectCard, int maxTop, int maxBottom);
    void minionWheel(MinionGraphicsItem *minion, bool up);
    void heroWheel(HeroGraphicsItem *hero, bool up);

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
    void playerBoardTagChange(int id, QString code, QString tag, QString value);
    void enemyBoardTagChange(int id, QString code, QString tag, QString value);
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
    void endGame(bool playerWon, bool playerUnknown);
    void playerSecretPlayed(int id, QString code);
    void enemySecretPlayed(int id, CardClass secretHero);
    void playerSecretRevealed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSecretStolen(int id, QString code);
    void enemySecretStolen(int id, QString code);
    void playerCardDraw(int id, QString code, int turn);
    void enemyCardDraw(int id, QString code, QString createdByCode, int turn);
    void playerCardPlayed(int id, QString code, bool discard);
    void enemyCardPlayed(int id, QString code, bool discard);
    void playerWeaponZonePlayRemove(int id);
    void enemyWeaponZonePlayRemove(int id);
    void playerHeroPowerZonePlayAdd(QString code, int id);
    void enemyHeroPowerZonePlayAdd(QString code, int id);
    void playerTagChange(QString tag, QString value);
    void enemyTagChange(QString tag, QString value);
    void unknownTagChange(QString tag, QString value);
    void resizePlan();
    void resetDeadProbs();
    void checkBomb(QString code);
    void showLastTurn();
    void enemyCardBuff(int id, int buffAttack, int buffHealth);
    void enemyIsolatedSecret(int id, QString code);
    void playerCardCodeChange(int id, QString newCode);
    void revealEnemyCard(int id, QString code);
    void setPremium(bool premium);
    void minionCodeChange(bool friendly, int id, QString newCode);
};

#endif // PLANHANDLER_H
