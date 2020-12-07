#include "premiumhandler.h"
#include "LibSmtp/smtp.h"
#include <QNetworkReply>
#include <QtWidgets>

PremiumHandler::PremiumHandler(QObject *parent) : QObject(parent)
{
}


PremiumHandler::~PremiumHandler()
{
}


void PremiumHandler::checkPremium(QString username, QString password)
{
}


void PremiumHandler::unlockPremium(QString email)
{
}


void PremiumHandler::checkTwitchSent()
{
}
