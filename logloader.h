#ifndef LOGLOADER_H
#define LOGLOADER_H

#include "logworker.h"
#include "gamewatcher.h"
#include <QObject>
#include <QFileSystemWatcher>
#include <QTextStream>


class LogLoader : public QObject
{
    Q_OBJECT

//Constructor
public:
    LogLoader(QObject *parent, qint64 &logSize);
    ~LogLoader();

//Variables
private:
    QString logPath;
    qint64 logSize;
    QFileSystemWatcher * fileWatcher;
    GameWatcher *gameWatcher;
    LogWorker *logWorker;
    bool workerRunning;
    bool firstRun;

//Metodos
private:
    qint64 getLogFileSize();
    void waitLogExists();
    bool isLogReset();
    void checkFirstRun();
    void createFileWatcher();
    void readSettings();
    void checkLogConfig(QString logConfig);
    void checkLogConfigOption(QString option, QString &data, QTextStream &stream);

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

//Slots
private slots:
    void prepareLogWorker(QString path);
    void sendLogWorker();
    void setWorkerFinished();
    void updateSeek(qint64 logSeek);
    void processLogLine(QString line);

    //GameWatcher signals reemit
    void emitNewGameResult(GameResult gameResult);
    void emitNewArena(QString hero);
    void emitNewArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void emitArenaRewardsComplete();
};

#endif // LOGLOADER_H
