#ifndef LOGLOADER_H
#define LOGLOADER_H

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
    void checkLogConfig(QString logConfig);
    void checkLogConfigOption(QString option, QString &data, QTextStream &stream);
    void workerFinished();

public:
    void init(qint64 &logSize);

//Signals
signals:
    void seekChanged(qint64 logSeek);
    void sendLog(QString line);
    void synchronized();

    //LogWorker signal reemit
    void newLogLineRead(QString line);


//Slots
private slots:
    void updateSeek(qint64 logSeek);
    void sendLogWorker();

    //LogWorker signal reemit
    void emitNewLogLineRead(QString line);
    void emitSendLog(QString line);
};

#endif // LOGLOADER_H
