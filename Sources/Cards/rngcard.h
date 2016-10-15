#ifndef RNGCARD_H
#define RNGCARD_H

#include "deckcard.h"


class RngCard : public DeckCard
{
public:
    RngCard(QString code);
    ~RngCard();

//Metodos
public:
    void draw();
};

#endif // RNGCARD_H
