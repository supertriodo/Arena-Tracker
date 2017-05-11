#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include "utility.h"
#include <QNetworkAccessManager>

#define VERSION QString("v4.51")


class VersionChecker : public QObject
{
    Q_OBJECT
public:
    VersionChecker(QObject *parent);
    ~VersionChecker();

private:
    QNetworkAccessManager * networkManager;

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="VersionChecker");

public slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // VERSIONCHECKER_H
