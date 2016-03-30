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
    QRadioButton *radioItem;
    QLabel *scoreItem;
    double score, tierScore;

//Metodos
public:
    void draw();
};

#endif // DRAFTCARD_H
