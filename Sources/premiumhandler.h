#ifndef PREMIUMHANDLER_H
#define PREMIUMHANDLER_H

#include "utility.h"
#include <QObject>

class PremiumHandler : public QObject
{
    Q_OBJECT
public:
    PremiumHandler(QObject *parent);
    ~PremiumHandler();

public:
    void unlockPremium(QString email);

public slots:
    void checkPremium(QString username="", QString password="");

signals:
    void setPremium(bool premium);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="PremiumHandler");
};

#endif // PREMIUMHANDLER_H
