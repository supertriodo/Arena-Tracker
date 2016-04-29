#ifndef GAMEWATCHER_H
#define GAMEWATCHER_H

#include <QObject>
#include <QString>
#include "Cards/secretcard.h"
#include "utility.h"

#define MAD_SCIENTIST "FP1_004"

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



class GameWatcher : public QObject
{
    Q_OBJECT
public:
    GameWatcher(QObject *parent = 0);
    ~GameWatcher();

private:
    enum PowerState { noGame, heroType1State, heroType2State, playerName1State, playerName2State, inGameState };
    enum ArenaState { noDeckRead, deckRead, readingDeck };

//Variables
private:
    QString playerTag;
    PowerState powerState;
    ArenaState arenaState;
    LoadingScreenState loadingScreenState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    int playerID;
    SecretHero secretHero;
    int enemyMinions, enemyMinionsAliveForAvenge; //Avenge control
    int playerMinions;
    bool isPlayerTurn;
    QRegularExpressionMatch *match;
    //TurnReal avanza a turn cuando robamos carta, nos aseguramos de que animaciones atrasadas
    //no aparezcan como parte del nuevo turno
    int turn, turnReal;
    bool mulliganEnemyDone;
    bool synchronized;
    qint64 logSeekCreate;



//Metodos
private:
    void createGameResult();
    void processLoadingScreen(QString &line, qint64 numLine);
    void processArena(QString &line, qint64 numLine);
    void processPower(QString &line, qint64 numLine, qint64 logSeek);
    void processPowerInGame(QString &line, qint64 numLine, qint64 logSeek);
    void processZone(QString &line, qint64 numLine);
    void advanceTurn(bool playerDraw);
    SecretHero getSecretHero(QString playerHero, QString enemyHero);
    void startReadingDeck();
    void endReadingDeck();
    bool isHeroPower(QString code);
    void createGameLog(qint64 logSeekWon);


public:
    void reset();
    void setSynchronized();
    LoadingScreenState getLoadingScreen();

signals:
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
    void newArena(QString hero);
    void inRewards();
    void newDeckCard(QString card);
    void startGame();
    void endGame();
    void enemyHero(QString hero);
    void enterArena();
    void leaveArena();
    void playerCardDraw(QString code);
    void enemyKnownCardDraw(QString code);
    void enemyCardDraw(int id, int turn=0, bool special=false, QString code="");
    void enemyCardPlayed(int id, QString code="");
    void lastHandCardIsCoin();
    void enemySecretPlayed(int id, SecretHero hero);
    void enemySecretStealed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSpellPlayed();
    void playerMinionPlayed(int playerMinions);
    void enemyMinionDead(QString code);
    void playerAttack(bool isHeroFrom, bool isHeroTo);
    void playerSpellObjPlayed();
    void playerHeroPower();
    void avengeTested();
    void cSpiritTested();
    void playerTurnStart();
    void needResetDeck();
    void activeDraftDeck();
    void pickCard(QString code);
    void specialCardTrigger(QString code, QString subType);
    void gameLogComplete(qint64 logSeekCreate, qint64 logSeekWon, QString fileName);
    void pLog(QString line);
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel=Normal, QString file="GameWatcher");

private slots:
    void checkAvenge();

public slots:
    void processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);
    void setDeckRead(bool value=true);
};

#endif // GAMEWATCHER_H
