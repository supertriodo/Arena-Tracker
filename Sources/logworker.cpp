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


