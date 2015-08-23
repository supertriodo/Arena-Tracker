#ifndef LOGWORKER_H
#define LOGWORKER_H

#include "utility.h"
#include <QObject>

class LogWorker : public QObject
{
    Q_OBJECT
public:
    LogWorker(QObject *parent, const QString &logPath);
    ~LogWorker();

//Variables
private:
    qint64 logSeek, logNumLine;
    QString logPath;

//Metodos
public:
    void readLog();
    void resetSeek();

signals:
    void newLogLineRead(QString line, qint64 numLine);
    void seekChanged(qint64 logSeek);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogWorker");
};

#endif // LOGWORKER_H
