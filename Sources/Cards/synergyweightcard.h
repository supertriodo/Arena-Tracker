#ifndef SYNERGYWEIGHTCARD_H
#define SYNERGYWEIGHTCARD_H

#include "synergycard.h"

class SynergyWeightCard : public SynergyCard
{
public:
    SynergyWeightCard(QString code, int draw, int toYourHand, int discover);

//Variables
private:
    int draw, toYourHand, discover;
    int correctedMana;
    float weightMana;

//Metodos
public:
    float processWeightMana(float drawMultiplier);
};

#endif // SYNERGYWEIGHTCARD_H
