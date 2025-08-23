#include "synergyweightcard.h"

SynergyWeightCard::SynergyWeightCard(QString code, int draw, int toYourHand, int discover) : SynergyCard(code)
{
    this->draw = draw;
    this->toYourHand = toYourHand;
    this->discover = discover;

    int cost = Utility::getCardAttribute(code, "cost").toInt();
    this->correctedMana = Utility::getCorrectedCardMana(code, cost);
}


float SynergyWeightCard::processWeightMana(float drawMultiplier)
{
    weightMana = correctedMana +
                 (draw * drawMultiplier) +
                 ((toYourHand + discover) * 4);
    return weightMana;
}
