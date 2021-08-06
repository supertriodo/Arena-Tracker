#ifndef LOGLOADER_H
#define LOGLOADER_H

#include "utility.h"
#include "logworker.h"
#include <QObject>
#include <QTextStream>

#define MIN_UPDATE_TIME 500
#define MAX_UPDATE_TIME 2000
#define UPDATE_TIME_STEP 500


class DataLog
{
public:
    LogComponent logComponent;
    QString line;
    qint64 numLine;
    qint64 logSeek;
};


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
    QMap<QString, LogWorker *>logWorkerMap;
    QList<QString> logComponentList;
    int updateTime, maxUpdateTime;
    bool sortLogs;
    QMap<qint64,DataLog> dataLogs;
    QRegularExpressionMatch *match;

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
    void addToDataLogs(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);
    void processDataLogs();
    QString findLinuxLogs(QString pattern);

public:
    bool init();
    QString getLogConfigPath();
    QString getLogsDirPath();

//Signals
signals:
    void synchronized();
    void logReset();
    void logConfigSet();
    void showMessageProgressBar(QString text);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogLoader");

    //LogWorker signal reemit
    void newLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);


//Slots
private slots:
    //LogWorker signal reemit
    void emitNewLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);

    void sendLogWorker();
    void sendLogWorkerFirstRun();

public slots:
    void setUpdateTimeMax();
    void setUpdateTimeMin();
};

#endif // LOGLOADER_H
