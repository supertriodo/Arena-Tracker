#ifndef HANDCARD_H
#define HANDCARD_H

#include "deckcard.h"
#include "../utility.h"

class HandCard : public DeckCard
{
public:
    HandCard(QString code);
    ~HandCard();

//Variables
public:
    int turn;


//Metodos
private:
    void drawDefaultHandCard();

public:
    void draw();
};
#endif // HANDCARD_H
