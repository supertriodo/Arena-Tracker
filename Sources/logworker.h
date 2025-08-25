#ifndef LOGWORKER_H
#define LOGWORKER_H

#include "utility.h"
#include <QObject>
#include <QFile>

class LogWorker : public QObject
{
    Q_OBJECT
public:
    LogWorker(QObject *parent, const QString &logsDirPath, const QString &logComponent);
    ~LogWorker();

//Variables
private:
    qint64 logSeek, logNumLine, logSize;
    QString logPath, logComponentString;
    LogComponent logComponent;


//Metodos
private:
    int readLine(QFile &file, QString &utf8Line);
    void reset();
    void initLogComponent(QString logComponentString);

public:
    void readLog();

    static void setCopyGameLogs(bool value);

signals:
    void newLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogWorker");
};

#endif // LOGWORKER_H
