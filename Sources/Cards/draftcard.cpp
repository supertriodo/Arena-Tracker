#include "draftcard.h"
#include "../utility.h"
#include <QtWidgets>

DraftCard::DraftCard() : DraftCard("")
{

}

DraftCard::DraftCard(QString code) : DeckCard(code)
{
    this->bestQualityMatches = 1;
}

DraftCard::~DraftCard()
{

}


void DraftCard::setBestQualityMatch(double matchScore, bool force)
{
    if(force)   this->bestQualityMatches = matchScore;
    else        this->bestQualityMatches = std::min(matchScore,bestQualityMatches);
}


double DraftCard::getBestQualityMatches()
{
    return this->bestQualityMatches;
}


void DraftCard::draw(QLabel *label)
{
    QPixmap canvas = DeckCard::draw(1, false);

    label->setPixmap(canvas);
}


void DraftCard::draw(QComboBox *comboBox)
{
    QPixmap canvas = DeckCard::draw(1, false);

    comboBox->addItem(canvas,"");
}
