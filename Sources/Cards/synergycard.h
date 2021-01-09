#ifndef SYNERGYCARD_H
#define SYNERGYCARD_H

#include "deckcard.h"


class SynergyCard : public DeckCard
{
public:
    SynergyCard(QString code);
    ~SynergyCard();

//Variables
private:
    static int synergyWidth;

//Metodos
public:
    void draw();

    static void setSynergyWidth(int value);
};

#endif // SYNERGYCARD_H
