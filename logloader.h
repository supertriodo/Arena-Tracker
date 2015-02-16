#ifndef LOGLOADER_H
#define LOGLOADER_H

#include "logworker.h"
#include "gamewatcher.h"
#include <QObject>
#include <QTextStream>

class LogLoader : public QObject
{
    Q_OBJECT

//Constructor
public:
    LogLoader(QObject *parent);
    ~LogLoader();

//Variables
private:
    QString logPath;
    qint64 logSize;
    GameWatcher *gameWatcher;
    LogWorker *logWorker;
    bool firstRun;
    int updateTime;

//Metodos
private:
    qint64 getLogFileSize();
    bool isLogReset();
    void checkFirstRun();
    void createFileWatcher();
    void readSettings();
    void readLogPath();
    void readLogConfigPath();
    void checkLogConfig(QString logConfig);
    void checkLogConfigOption(QString option, QString &data, QTextStream &stream);
    void workerFinished();

public:
    void init(qint64 &logSize);
    void setDeckRead();

//Signals
signals:
    void seekChanged(qint64 logSeek);
    void sendLog(QString line);
    void synchronized();

    //GameWatcher signals reemit
    void newGameResult(GameResult gameResult);
    void newArena(QString hero);
    void newArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void arenaRewardsComplete();
    void newDeckCard(QString card);
    void startGame();
    void endGame();
    void playerCardDraw(QString code);
    void enemyCardDraw(int id, int turn, bool special, QString code);
    void enemyCardPlayed(int id, QString code);
    void lastHandCardIsCoin();

//Slots
private slots:
    void updateSeek(qint64 logSeek);
    void processLogLine(QString line);
    void sendLogWorker();

    //GameWatcher signals reemit
    void emitNewGameResult(GameResult gameResult);
    void emitNewArena(QString hero);
    void emitNewArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void emitArenaRewardsComplete();
    void emitNewDeckCard(QString card);
    void emitSendLog(QString line);
    void emitStartGame();
    void emitEndGame();
    void emitPlayerCardDraw(QString code);
    void emitEnemyCardDraw(int id, int turn, bool special, QString code);
    void emitEnemyCardPlayed(int id, QString code);
    void emitlastHandCardIsCoin();
};

#endif // LOGLOADER_H
