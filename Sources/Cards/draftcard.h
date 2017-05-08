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
private:
    double sumQualityMatches;

//Metodos
public:
    void draw(QLabel *label);
    void addQualityMatch(double matchScore);
    double getSumQualityMatches();
    void setQualityMatch(double matchScore);
};

#endif // DRAFTCARD_H
