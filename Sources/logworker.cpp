#include "logworker.h"
#include <QtWidgets>


LogWorker::LogWorker(QObject *parent, const QString &logsDirPath, const QString &logComponentString) : QObject(parent)
{
    this->logSeek = 0;
    this->logNumLine = 0;
    this->logComponentString = logComponentString;
    this->logPath = logsDirPath + "/" + logComponentString + ".log";
    this->logSize = QFileInfo (logPath).size();
    initLogComponent(logComponentString);
}

LogWorker::~LogWorker()
{

}


void LogWorker::initLogComponent(QString logComponentString)
{
    if(logComponentString == "LoadingScreen")
    {
        this->logComponent = logLoadingScreen;
    }
    else if(logComponentString == "Power")
    {
        this->logComponent = logPower;
    }
    else if(logComponentString == "Zone")
    {
        this->logComponent = logZone;
    }
    else if(logComponentString == "Arena")
    {
        this->logComponent = logArena;
    }
    else if(logComponentString == "Asset")
    {
        this->logComponent = logAsset;
    }
    else
    {
        this->logComponent = logInvalid;
    }
}


void LogWorker::reset()
{
    logSeek = 0;
    logNumLine = 0;
    logSize = 0;
}


bool LogWorker::isLogReset()
{
    qint64 newSize = QFileInfo (logPath).size();

    if(newSize < logSize)
    {
        //Log se ha reiniciado
        emit pDebug("Log " + logComponentString + " reset. FileSize: " + QString::number(newSize) + " < " + QString::number(logSize));
        emit logReset();
        reset();
        return true;
    }
    else
    {
        logSize = newSize;
        return false;
    }
}


int LogWorker::readLine(QFile &file, QString &utf8Line)
{
    char c;
    int lineLength = 0;
    QByteArray line;

    while(true)
    {
        if(!file.getChar(&c))    return -1;
        line.append(c);
        lineLength++;
        if(c == '\n')
        {
            utf8Line = QString::fromUtf8(line);
            return lineLength;
        }
    }
}


void LogWorker::readLog()
{
    isLogReset();

    QFile logFile(logPath);
    if(!logFile.exists())
    {
        if(this->logComponent == logLoadingScreen)  emit pDebug("Missing log LoadingScreen.", DebugLevel::Warning);
        return;
    }
    if(!logFile.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open existing log " + this->logComponentString, DebugLevel::Error);
        return;
    }

    logFile.seek(logSeek);

    QString line;
    int lineLenght;

    while((lineLenght = readLine(logFile, line)) > 0)
    {
        emit newLogLineRead(logComponent, line, ++logNumLine, logSeek);
        logSeek += lineLenght;
    }

    logFile.close();
}


void LogWorker::copyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName)
{
    QFileInfo dir(Utility::hscardsPath());
    if(!dir.exists())
    {
        emit pDebug("Cannot copy game Log. HSCards dir doesn't exist.");
        return;
    }

    dir = QFileInfo(Utility::gameslogPath());
    if(!dir.exists())
    {
        emit pDebug("Cannot copy game Log. GamesLog dir doesn't exist.");
        return;
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

    doCopyGameLog(logSeekCreate, logSeekWon, fileName);
}


void LogWorker::doCopyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName)
{
    emit pDebug("Start copy GameLog: " + fileName + ": " + QString::number(logSeekCreate) + " - " + QString::number(logSeekWon));

    QFile logFile(logPath);
    if(!logFile.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open log...", DebugLevel::Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    QFile logGame(Utility::gameslogPath() + "/" + fileName);
    if(!logGame.open(QIODevice::WriteOnly))
    {
        emit pDebug("Cannot create game log file...", DebugLevel::Error);
        emit pLog(tr("Log: ERROR:Cannot create game log file..."));
        return;
    }

    QTextStream out(&logGame);
    logFile.seek(logSeekCreate);

    char line[2048];
    qint64 lineLenght;
    qint64 gameLogSeek = logSeekCreate;

    while((gameLogSeek <= logSeekWon) && ((lineLenght = logFile.readLine(line, sizeof(line))) > 0))
    {
        out << QString::fromUtf8(line);
        gameLogSeek += lineLenght;
    }

    logFile.close();
    logGame.close();

    if(gameLogSeek > logSeekWon)   emit pDebug("End copy GameLog: Success");
    else                            emit pDebug("End copy GameLog: Reached EOF before WON");
}



