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
    DraftCard(QString code, bool gold);
    ~DraftCard();

//Variables
private:
    double bestQualityMatches;
    bool gold;

//Metodos
public:
    void draw(QLabel *label);
    double getBestQualityMatches();
    void setBestQualityMatch(double matchScore, bool force);
    void draw(QComboBox *comboBox);
    bool isGold();
};

#endif // DRAFTCARD_H
