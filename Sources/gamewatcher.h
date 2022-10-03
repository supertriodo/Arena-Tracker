#ifndef GAMEWATCHER_H
#define GAMEWATCHER_H

#include <QObject>
#include <QString>
#include "Cards/secretcard.h"
#include "utility.h"


class GameResult
{
public:
    bool isFirst, isWinner;
    QString playerHero, enemyHero, enemyName;
};


class ArenaRewards
{
public:
    ArenaRewards(){gold=0; dust=0; packs=0; plainCards=0; goldCards=0;}
    int gold, dust, packs, plainCards, goldCards;
};


class ArenaResult
{
public:
    QString playerHero;
    QList<GameResult> gameResultList;
    ArenaRewards arenaRewards;
};


class ShowEntity
{
public:
    int id;
    bool isPlayer;
    bool trackAllTags;//No lo usamos
};



class GameWatcher : public QObject
{
    Q_OBJECT
public:
    GameWatcher(QObject *parent = nullptr);
    ~GameWatcher();

private:
    enum PowerState { noGame, heroType1State, heroPower1State, heroType2State, mulliganState, inGameState };
    enum ArenaState { noDeckRead, deckRead, readingDeck };

//Variables
private:
    QString playerTag;
    PowerState powerState;
    ArenaState arenaState;
    LoadingScreenState loadingScreenState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    int playerID;
    CardClass secretHero;
    int enemyMinions;
    int playerMinions;
    bool isPlayerTurn;
    QRegularExpressionMatch *match;
    //TurnReal avanza a turn cuando robamos carta, nos aseguramos de que animaciones atrasadas
    //no aparezcan como parte del nuevo turno
    int turn, turnReal;
    bool mulliganEnemyDone, mulliganPlayerDone;
    bool spectating, tied;
    ShowEntity lastShowEntity;
    qint64 startGameEpoch;



//Metodos
private:
    void createGameResult();
    void processLoadingScreen(QString &line, qint64 numLine);
    void processAsset(QString &line, qint64 numLine);
    void processArena(QString &line, qint64 numLine);
    void processPower(QString &line, qint64 numLine);
    void processPowerHero(QString &line, qint64 numLine);
    void processPowerMulligan(QString &line, qint64 numLine);
    void processPowerInGame(QString &line, qint64 numLine);
    void processZone(QString &line, qint64 numLine);
    bool advanceTurn(bool playerDraw);
    void startReadingDeck();
    void endReadingDeck();
    bool isHeroPower(QString code);
    bool isCheatingCard(const QString &zoneFrom);

public:
    void reset();
    LoadingScreenState getLoadingScreen();

signals:
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void newArena(QString hero);
    void continueDraft();
    void arenaChoosingHeroe();
    void inRewards();
    void newDeckCard(QString card);
    void startGame();
    void endGame(bool playerWon=false, bool playerUnknown=true);
    void enemyHero(QString hero);
    void enterArena();
    void leaveArena();
    void playerCardDraw(QString code, int id);
    void playerReturnToDeck(QString code, int id=0);
    void enemyKnownCardDraw(int id, QString code);
    void enemyCardDraw(int id, int turn=0, bool special=false, QString code="");
    void playerCardToHand(int id, QString code, int turn);
    void enemyCardPlayed(int id, QString code="", bool discard=false);
    void playerCardPlayed(int id, QString code, bool discard, bool isPlayerTurn);
    void lastHandCardIsCoin();
    void enemySecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState);
    void playerSecretPlayed(int id, QString code);
    void enemySecretStolen(int id, QString code, LoadingScreenState loadingScreenState);
    void playerSecretStolen(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSecretRevealed(int id, QString code);
    void playerSpellPlayed(QString code);
    void playerMinionPlayed(QString code, int id, int playerMinions);
    void playerMinionZonePlayAdd(QString code, int id, int pos);
    void enemyMinionZonePlayAdd(QString code, int id, int pos);
    void playerMinionZonePlayAddTriggered(QString code, int id, int pos);
    void enemyMinionZonePlayAddTriggered(QString code, int id, int pos);
    void playerMinionZonePlayRemove(int id);
    void enemyMinionZonePlayRemove(int id);
    void playerMinionZonePlaySteal(int id, int pos);
    void enemyMinionZonePlaySteal(int id, int pos);
    void playerHeroZonePlayAdd(QString code, int id);
    void enemyHeroZonePlayAdd(QString code, int id);
    void playerHeroPowerZonePlayAdd(QString code, int id);
    void enemyHeroPowerZonePlayAdd(QString code, int id);
    void playerWeaponZonePlayAdd(QString code, int id);
    void enemyWeaponZonePlayAdd(QString code, int id);
    void playerWeaponZonePlayRemove(int id);
    void enemyWeaponZonePlayRemove(int id);
    void playerMinionPosChange(int id, int pos);
    void enemyMinionPosChange(int id, int pos);
    void playerBoardTagChange(int id, QString code, QString tag, QString value);
    void enemyBoardTagChange(int id, QString code, QString tag, QString value);
    void unknownTagChange(QString tag, QString value);
    void playerTagChange(QString tag, QString value);
    void enemyTagChange(QString tag, QString value);
    void playerCardCodeChange(int id, QString newCode);
    void minionCodeChange(bool friendly, int id, QString newCode);
    void playerMinionGraveyard(int id, QString code);
    void enemyMinionGraveyard(int id, QString code, bool isPlayerTurn, int enemyMinions);
    void playerWeaponGraveyard(int id, QString code);
    void enemyWeaponGraveyard(int id, QString code);
    void playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions, int id1, int id2);
    void zonePlayAttack(QString code, int id1, int id2);
    void playerSpellObjHeroPlayed();
    void playerSpellObjMinionPlayed();
    void playerBattlecryObjHeroPlayed();
    void playerCardObjPlayed(QString code, int id1, int id2);
    void enemyCardObjPlayed(QString code, int id1, int id2);
    void playerHeroPower();
    void clearDrawList();
    void newTurn(bool isPlayerTurn, int numTurn, int enemyMinions);
    void logTurn();
    void needResetDeck();
    void heroDraftDeck(QString hero="");
    void activeDraftDeck();
    void pickCard(QString code);
    void specialCardTrigger(QString code, QString blockType, int id1, int id2);
    void arenaDeckRead();
    void buffHandCard(int id);
    void coinIdFound(int id);
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel=Normal, QString file="GameWatcher");

public slots:
    void processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);
    void setDeckRead(bool value=true);
};

#endif // GAMEWATCHER_H
