#ifndef LOGWORKER_H
#define LOGWORKER_H

#include "utility.h"
#include <QObject>
#include <QFile>

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

    static bool copyGameLogs;

    //Metodos
private:
    void doCopyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName);
    int readLine(QFile &file, QString &line);

public:
    void readLog();
    void resetSeek();
    void copyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName);

    static void setCopyGameLogs(bool value);

signals:
    void newLogLineRead(QString line, qint64 numLine, qint64 logSeek);
    void seekChanged(qint64 logSeek);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="LogWorker");
};

#endif // LOGWORKER_H
