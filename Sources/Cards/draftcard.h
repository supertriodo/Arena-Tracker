#ifndef DRAFTCARD_H
#define DRAFTCARD_H

#include "deckcard.h"
#include <QRadioButton>
#include <QLabel>


class DraftCard : public DeckCard
{
public:
    DraftCard();
    DraftCard(QString code);
    ~DraftCard();

//Variables
public:
    QLabel *scoreLFitem, *scoreHAitem, *cardItem;
    double tierScore;

//Metodos
public:
    void draw();
};

#endif // DRAFTCARD_H
