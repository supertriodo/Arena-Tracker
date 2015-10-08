#ifndef LOGLOADER_H
#define LOGLOADER_H

#include "utility.h"
#include "logworker.h"
#include <QObject>
#include <QTextStream>

#define MIN_UPDATE_TIME 500
#define MAX_UPDATE_TIME 2000
#define UPDATE_TIME_STEP 500

class LogLoader : public QObject
{
    Q_OBJECT

//Constructor
public:
    LogLoader(QObject *parent);
    ~LogLoader();

//Variables
private:
    QString logPath, logConfig;
    qint64 logSize;
    LogWorker *logWorker;
    bool firstRun;
    int updateTime, maxUpdateTime;

//Metodos
private:
    qint64 getLogFileSize();
    bool isLogReset();
    void checkFirstRun();
    void createFileWatcher();
    void readSettings();
    void readLogPath();
    void readLogConfigPath();
    QString createDefaultLogConfig();
    void checkLogConfig();
    void checkLogConfigOption(QString option, QString &data, QTextStream &stream);
    void workerFinished();
    void setMaxUpdateTime(int value);

public:
    void init(qint64 &logSize);
    QString getLogConfigPath();

//Signals
signals:
    void seekChanged(qint64 logSeek);
    void logConfigSet();
    void synchronized();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogLoader");

    //LogWorker signal reemit
    void newLogLineRead(QString line, qint64 numLine);


//Slots
private slots:
    void updateSeek(qint64 logSeek);

    //LogWorker signal reemit
    void emitNewLogLineRead(QString line, qint64 numLine);

public slots:
    void sendLogWorker();
    void setUpdateTimeMax();
    void setUpdateTimeMin();
};

#endif // LOGLOADER_H
