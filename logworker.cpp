#include "logworker.h"
#include <QFile>
#include <QDebug>
#include <QThread>

LogWorker::LogWorker(QObject *parent, const QString &logPath) : QObject(parent)
{
    this->logSeek = 0;
    this->logPath = logPath;
}

LogWorker::~LogWorker()
{

}


void LogWorker::readLog()
{
    QFile *logFile = new QFile(logPath);
    if(!logFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "LogWorker: "<< "Esperando a log accesible...";
        delete logFile;
        return;
    }

    logFile->seek(logSeek);

    char line[2048];
    int lineLenght;

    while((lineLenght = logFile->readLine(line, sizeof(line))) > 0)
    {
        if((line[lineLenght-1] != '\n') &&
                lineLenght<((int)sizeof(line)-1))
        {
            qDebug() << "LogWorker: "<< "Leida linea a medias.";
            return;
        }

        emit newLogLineRead(QString(line));

        logSeek += lineLenght + 1;
        emit seekChanged(logSeek);
    }

    logFile->close();
    delete logFile;
}


void LogWorker::resetSeek()
{
    logSeek = 0;
}

