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

//Metodos
public:
    void draw() Q_DECL_OVERRIDE;
};

#endif // DRAFTCARD_H
