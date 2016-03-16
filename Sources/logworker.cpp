#include "logworker.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

LogWorker::LogWorker(QObject *parent, const QString &logPath) : QObject(parent)
{
    this->logSeek = 0;
    this->logNumLine = 0;
    this->logPath = logPath;
}

LogWorker::~LogWorker()
{

}


void LogWorker::readLog()
{
    QFile logFile(logPath);
    if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Cannot open log...", Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    logFile.seek(logSeek);

    char line[2048];
    int lineLenght;

    while((lineLenght = logFile.readLine(line, sizeof(line))) > 0)
    {
        if((line[lineLenght-1] != '\n') &&
                lineLenght<((int)sizeof(line)-1))
        {
            //Leida linea a medias.
            return;
        }

        emit newLogLineRead(QString(line), ++logNumLine, logSeek);

        logSeek += lineLenght + 1;
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
    if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Cannot open log...", Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    QFile logGame(Utility::appPath() + "/HSCards/GamesLog/" + fileName);
    if(!logGame.open(QIODevice::WriteOnly | QIODevice::Text))
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
        gameLogSeek += lineLenght + 1;
    }

    logFile.close();
    logGame.close();

    if(gameLogSeek > logSeekWon)   emit pDebug("End copy GameLog: Success");
    else                            emit pDebug("End copy GameLog: Reached EOF before WON");
}



