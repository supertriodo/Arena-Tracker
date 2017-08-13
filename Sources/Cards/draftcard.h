#ifndef DRAFTCARD_H
#define DRAFTCARD_H

#include "deckcard.h"
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>


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
    double getSumQualityMatches();
    void setBestQualityMatch(double matchScore);
    void draw(QComboBox *comboBox);
};

#endif // DRAFTCARD_H
