#ifndef LOGWORKER_H
#define LOGWORKER_H

#include <QObject>

class LogWorker : public QObject
{
    Q_OBJECT
public:
    LogWorker(QObject *parent, const QString &logPath);
    ~LogWorker();

//Variables
private:
    qint64 logSeek;
    QString logPath;

//Metodos
public:
    void readLog();
    void resetSeek();

signals:
    void newLogLineRead(QString line);
    void seekChanged(qint64 logSeek);
    void synchronized();
};

#endif // LOGWORKER_H
