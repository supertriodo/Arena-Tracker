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
    while(!logFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "LogWorker: "<< "Esperando a log accesible...";
        QThread::sleep(1);
    }

    logFile->seek(logSeek);

    char line[1024];
    int lineLenght;

    while((lineLenght = logFile->readLine(line, sizeof(line))) > 0)
    {
        if((line[lineLenght-1] != '\n') &&
                lineLenght<((int)sizeof(line)-1))
        {
            qDebug() << "LogWorker: "<< "Leida linea a medias.";
            emit synchronized();
            return;
        }

        emit newLogLineRead(QString(line));

        logSeek += lineLenght + 1;
        emit seekChanged(logSeek);
    }

    logFile->close();
    delete logFile;
    emit synchronized();
}


void LogWorker::resetSeek()
{
    logSeek = 0;
}



















//    QTextStream *in = new QTextStream(logFile);
//    in->seek(logSeek);

//    QString line = in->readLine();
//    while(!in->atEnd())
//    {
//        emit newLogLineRead(line);

//        logSeek += line.length() + 2;
//        emit seekChanged(logSeek);

//        line = in->readLine();
//    }
