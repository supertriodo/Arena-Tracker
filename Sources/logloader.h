#ifndef LOGLOADER_H
#define LOGLOADER_H

#include "utility.h"
#include "logworker.h"
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
    QString createDefaultLogConfig();
    void checkLogConfig(QString logConfig);
    void checkLogConfigOption(QString option, QString &data, QTextStream &stream);
    void workerFinished();

public:
    void init(qint64 &logSize);

//Signals
signals:
    void seekChanged(qint64 logSeek);
    void synchronized();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogLoader");

    //LogWorker signal reemit
    void newLogLineRead(QString line, qint64 numLine);


//Slots
private slots:
    void updateSeek(qint64 logSeek);
    void sendLogWorker();

    //LogWorker signal reemit
    void emitNewLogLineRead(QString line, qint64 numLine);
};

#endif // LOGLOADER_H
