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
    void draw() Q_DECL_OVERRIDE;
};

#endif // DRAWCARD_H
