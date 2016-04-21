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
    QString logsDirPath, logConfig;
    QList<LogWorker *>logWorkerList;
    QList<QString> logComponentList;
    int updateTime, maxUpdateTime;

//Metodos
private:
    void createFileWatcher();
    bool readSettings();
    bool readLogsDirPath();
    bool readLogConfigPath();
    QString createDefaultLogConfig();
    bool checkLogConfig();
    bool checkLogConfigOption(QString option, QString &data, QTextStream &stream);
    void setMaxUpdateTime(int value);
    void createLogWorkers();
    void createLogWorker(QString logComponent);

public:
    bool init();
    QString getLogConfigPath();

//Signals
signals:
    void logReset();
    void logConfigSet();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogLoader");

    //LogWorker signal reemit
    void newLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);


//Slots
private slots:
    //LogWorker signal reemit
    void emitNewLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);

public slots:
    void sendLogWorker();
    void setUpdateTimeMax();
    void setUpdateTimeMin();
    void copyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName);
};

#endif // LOGLOADER_H
