#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include "utility.h"
#include <QNetworkAccessManager>

#define VERSION QString("v6.12")
#define VERSION_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Version/version.json"


class VersionChecker : public QObject
{
    Q_OBJECT
public:
    VersionChecker(QObject *parent);
    ~VersionChecker();

private:
    QNetworkAccessManager * networkManager;
    void checkUpdate(const QJsonObject &versionJsonObject);
    void downloadLatestVersion(const QJsonObject &versionJsonObject);
    void saveRestart(const QByteArray &data);
    void removeOldVersion();
    void saveRestartAppImage(const QByteArray &data);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="VersionChecker");

public slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // VERSIONCHECKER_H
