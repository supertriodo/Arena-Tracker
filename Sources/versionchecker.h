#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include "utility.h"
#include <QNetworkAccessManager>
#include <QFutureWatcher>

#define VERSION QString("v7.2")
#define VERSION_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Version/version.json"


class VersionChecker : public QObject
{
    Q_OBJECT
public:
    VersionChecker(QObject *parent);
    ~VersionChecker();

//Variables
private:
    QNetworkAccessManager * networkManager;
    QString latestVersion;
    bool newVersion;
    QFutureWatcher<void> futureNewAppReplace;

//Metodos
private:
    void checkUpdate(const QJsonObject &versionJsonObject);
    void downloadLatestVersion(const QJsonObject &versionJsonObject);
    void saveRestart(const QByteArray &data);
    void removeOldNewVersion();
    void saveRestartAppImage(const QByteArray &data);
    void showVersionLog(QString changesLog);
    bool isNewApp();
    void newAppReplace();
    void saveRestartOld(const QByteArray &data);
    void saveRestartNew(const QByteArray &data);

signals:
    void startProgressBar(int maximum, QString text);
    void advanceProgressBar(int remaining, QString text="");
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="VersionChecker");

public slots:
    void replyFinished(QNetworkReply *reply);

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void startNewAppReplace();
    void finishNewAppReplace();
};

#endif // VERSIONCHECKER_H
