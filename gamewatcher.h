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
    enum GameState { noGame, heroType1State, heroType2State, playerName1State, playerName2State, winnerState, inRewards };

//Variables
private:
    QString playerTag;
    GameState gameState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    bool arenaMode;
    QRegularExpressionMatch *match;

//Metodos
private:
    void createGameResult();
    void processGameLine(QString &line);
    QString askPlayerTag(QString &playerName1, QString &playerName2);

public:
    void processLogLine(QString &line);

signals:
    void newGameResult(GameResult gameResult);
    void newArena(QString hero);
    void newArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void arenaRewardsComplete();
    void sendLog(QString line);

public slots:
};

#endif // GAMEWATCHER_H
