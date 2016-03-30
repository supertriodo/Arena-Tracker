#ifndef DRAWCARD_H
#define DRAWCARD_H

#include "deckcard.h"


class DrawCard : public DeckCard
{
public:
    DrawCard(QString code);
    ~DrawCard();

//Metodos
public:
    void draw();
};

#endif // DRAWCARD_H
