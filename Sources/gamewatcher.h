#ifndef GAMEWATCHER_H
#define GAMEWATCHER_H

#include <QObject>
#include "secretcard.h"
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
    enum GameState { noGame, heroType1State, heroType2State, playerName1State, playerName2State,
                     inGameState, /*inRewards, */readingDeck };

//Variables
private:
    QString playerTag;
    GameState gameState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    int playerID;
    SecretHero secretHero;
    int enemyMinions, enemyMinionsAliveForAvenge; //Avenge control
    bool isPlayerTurn;
    bool arenaMode;
    bool deckRead;
    QRegularExpressionMatch *match;
    //TurnReal avanza a turn cuando robamos carta, nos aseguramos de que animaciones atrasadas
    //no aparezcan como parte del nuevo turno
    int turn, turnReal;
    bool mulliganEnemyDone;
    bool synchronized;

    static QMap<QString, QJsonObject> *cardsJson;


//Metodos
private:
    void createGameResult();
    void processPower(QString &line, qint64 numLine);
    void processPowerInGame(QString &line, qint64 numLine);
    void processZone(QString &line, qint64 numLine);
    QString askPlayerTag(QString &playerName1, QString &playerName2);
    void advanceTurn(bool playerDraw);
    SecretHero getSecretHero(QString playerHero, QString enemyHero);
    void startReadingDeck();
    void endReadingDeck();
    bool findClasp(QString &line);

public:
    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);

public:
    void reset();
    void setSynchronized();

signals:
    void newGameResult(GameResult gameResult, bool arenaMatch);
    void newArena(QString hero);
//    void newArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
//    void arenaRewardsComplete();
    void inRewards();
    void newDeckCard(QString card);
    void startGame();
    void endGame();
    void playerCardDraw(QString code);
    void enemyCardDraw(int id, int turn=0, bool special=false, QString code="");
    void enemyCardPlayed(int id, QString code="");
    void lastHandCardIsCoin();
    void enemySecretPlayed(int id, SecretHero hero);
    void enemySecretStealed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSpellPlayed();
    void playerMinionPlayed();
    void enemyMinionDead();
    void playerAttack(bool isHeroFrom, bool isHeroTo);
    void playerSpellObjPlayed();
    void avengeTested();
    void cSpiritTested();
    void playerTurnStart();
    void enterArena();
    void leaveArena();
    void beginReadingDeck();
    void activeDraftDeck();
    void pickCard(QString code);
    void pLog(QString line);
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel=Normal, QString file="GameWatcher");

private slots:
    void checkAvenge();

public slots:
    void processLogLine(QString line, qint64 numLine);
    void setDeckRead(bool value=true);
};

#endif // GAMEWATCHER_H
