#include "synergyweightcard.h"
#include "../Synergies/mechaniccounter.h"

SynergyWeightCard::SynergyWeightCard(QString code, int draw, int toYourHand, int discover) : SynergyCard(code)
{
    this->draw = draw;
    this->toYourHand = toYourHand;
    this->discover = discover;
    this->correctedMana = Utility::getCorrectedCardMana(code, cost);
}


float SynergyWeightCard::processWeightMana(float drawMultiplier)
{
    weightMana = correctedMana +
                 (draw * drawMultiplier) +
                 ((toYourHand + discover) * 4);

    int cost = Utility::getCardAttribute(code, "cost").toInt();

    QMap<QString, QString> keyMap = MechanicCounter::getMapKeySynergies();
    bool showSynergy = false;
    bool showSynergyDetails = false;
    QString text = "";
    QString detailText = "";

    if(cost != correctedMana)
    {
        text += QStringLiteral("%1->%2").arg(cost).arg(correctedMana);
        showSynergy = true;
    }
    else
    {
        text += QStringLiteral("%1").arg(cost);
    }

    if(draw > 0)
    {
        text += QStringLiteral(" + %1x%2").arg(draw).arg(drawMultiplier, 0, 'f', 1);

        if(showSynergyDetails)  detailText += " / ";
        detailText += QStringLiteral("%1 %2").arg(draw).arg(keyMap["draw"]);
        showSynergyDetails = true;
    }
    if(toYourHand > 0)
    {
        text += QStringLiteral(" + %1x4").arg(toYourHand);

        if(showSynergyDetails)  detailText += " / ";
        detailText += QStringLiteral("%1 %2").arg(toYourHand).arg(keyMap["toYourHand"]);
        showSynergyDetails = true;
    }
    if(discover > 0)
    {
        text += QStringLiteral(" + %1x4").arg(discover);

        if(showSynergyDetails)  detailText += " / ";
        detailText += QStringLiteral("%1 %2").arg(discover).arg(keyMap["discover"]);
        showSynergyDetails = true;
    }

    if(showSynergy || showSynergyDetails)
    {
        synergyTag = text;
        if(showSynergyDetails)
        {
            synergyTag += QStringLiteral("  (%1)").arg(detailText);
        }
    }
    this->cost = weightMana;
    return weightMana;
}

















