#ifndef GAMEWATCHER_H
#define GAMEWATCHER_H

#include <QObject>


class GameResult
{
public:
    bool isFirst, isWinner;
    QString playerHero, enemyHero;
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
                     inGameState, inRewards, readingDeck };

//Variables
private:
    QString playerTag;
    GameState gameState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    int playerID;
    bool arenaMode;
    bool deckRead;
    QRegularExpressionMatch *match;
    //TurnReal avanza a turn cuando robamos carta, nos aseguramos de que animaciones atrasadas
    //no aparezcan como parte del nuevo turno
    int turn, turnReal;
    bool mulliganEnemyDone;

//Metodos
private:
    void createGameResult();
    void processPower(QString &line);
    void processPowerInGame(QString &line);
    void processZone(QString &line);
    QString askPlayerTag(QString &playerName1, QString &playerName2);
    void advanceTurn(bool playerDraw);

public:
    void processLogLine(QString &line);
    void reset();    
    void setDeckRead();

signals:
    void newGameResult(GameResult gameResult);
    void newArena(QString hero);
    void newArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void arenaRewardsComplete();
    void newDeckCard(QString card);
    void sendLog(QString line);
    void startGame();
    void endGame();
    void playerCardDraw(QString code);
    void enemyCardDraw(int id, int turn=0, bool special=false, QString code="");
    void enemyCardPlayed(int id, QString code="");
    void lastHandCardIsCoin();

public slots:
};

#endif // GAMEWATCHER_H
