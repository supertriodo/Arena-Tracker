#include "logworker.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

bool LogWorker::copyGameLogs;

LogWorker::LogWorker(QObject *parent, const QString &logPath) : QObject(parent)
{
    this->logSeek = 0;
    this->logNumLine = 0;
    this->logPath = logPath;
}

LogWorker::~LogWorker()
{

}


int LogWorker::readLine(QFile &file, QString &line)
{
    char c;
    int lineLength = 0;
    line = "";

    while(true)
    {
        if(!file.getChar(&c))    return -1;
        line.append(c);
        lineLength++;
        if(c == '\n')    return lineLength;
    }
}


void LogWorker::readLog()
{
    QFile logFile(logPath);
    if(!logFile.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open log...", Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    logFile.seek(logSeek);

    QString line;
    int lineLenght;

    while((lineLenght = readLine(logFile, line)) > 0)
    {
        emit newLogLineRead(QString(line), ++logNumLine, logSeek);
        logSeek += lineLenght;
        emit seekChanged(logSeek);
    }

    logFile.close();
}


void LogWorker::resetSeek()
{
    logSeek = 0;
    logNumLine = 0;
}


void LogWorker::copyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName)
{
    if(!copyGameLogs)
    {
        emit pDebug("Game log copy disabled.");
        return;
    }

    QFileInfo dir(Utility::appPath() + "/HSCards");
    if(!dir.exists())
    {
        emit pDebug("Cannot copy game Log. HSCards dir doesn't exist.");
        return;
    }

    dir = QFileInfo(Utility::appPath() + "/HSCards/GamesLog");
    if(!dir.exists())
    {
        QDir().mkdir(Utility::appPath() + "/HSCards/GamesLog");
        emit pDebug("GamesLog dir created.");
    }

    if(logSeekCreate < 0)
    {
        emit pDebug("logSeekCreate < 0");
        return;
    }
    if(logSeekCreate > logSeekWon)
    {
        emit pDebug("logSeekCreate > logSeekWon");
        return;
    }

//    QFuture<void> future = QtConcurrent::run(this, &LogWorker::doCopyGameLog, logSeekCreate, logSeekWon, fileName);
    doCopyGameLog(logSeekCreate, logSeekWon, fileName);
}


void LogWorker::doCopyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName)
{
    emit pDebug("Start copy GameLog: " + fileName + ": " + QString::number(logSeekCreate) + " - " + QString::number(logSeekWon));

    QFile logFile(logPath);
    if(!logFile.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open log...", Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    QFile logGame(Utility::appPath() + "/HSCards/GamesLog/" + fileName);
    if(!logGame.open(QIODevice::WriteOnly))
    {
        emit pDebug("Cannot create game log file...", Error);
        emit pLog(tr("Log: ERROR:Cannot create game log file..."));
        return;
    }

    QTextStream s1(&logFile);
    QTextStream s2(&logGame);
    s1.seek(logSeekCreate);

    char line[2048];
    int lineLenght;
    qint64 gameLogSeek = logSeekCreate;

    while((gameLogSeek <= logSeekWon) && ((lineLenght = logFile.readLine(line, sizeof(line))) > 0))
    {
        s2 << line;
        gameLogSeek += lineLenght;
    }

    logFile.close();
    logGame.close();

    if(gameLogSeek > logSeekWon)   emit pDebug("End copy GameLog: Success");
    else                            emit pDebug("End copy GameLog: Reached EOF before WON");
}


void LogWorker::setCopyGameLogs(bool value)
{
    copyGameLogs = false;//value;//Desactivado
}



