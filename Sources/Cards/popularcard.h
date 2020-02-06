#ifndef POPULARCARD_H
#define POPULARCARD_H

#include "deckcard.h"


class PopularCard : public DeckCard
{
public:
    PopularCard(QString code, QString manaText);
    ~PopularCard();

//Variables
public:
    QString manaText;

//Metodos
public:
    void draw();
};

#endif // POPULARCARD_H
