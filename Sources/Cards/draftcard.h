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
    double bestQualityMatches;

//Metodos
public:
    void draw(QLabel *label);
    double getSumQualityMatches();
    void setBestQualityMatch(double matchScore, bool force);
    void draw(QComboBox *comboBox);
};

#endif // DRAFTCARD_H
