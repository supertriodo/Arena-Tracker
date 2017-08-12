#include "draftcard.h"
#include "../utility.h"
#include <QtWidgets>

DraftCard::DraftCard() : DraftCard("")
{

}

DraftCard::DraftCard(QString code) : DeckCard(code)
{
    this->sumQualityMatches = 1;
}

DraftCard::~DraftCard()
{

}


void DraftCard::setBestQualityMatch(double matchScore)
{
    this->sumQualityMatches = std::min(matchScore,sumQualityMatches);
}


double DraftCard::getSumQualityMatches()
{
    return this->sumQualityMatches;
}


void DraftCard::draw(QLabel *label)
{
    QPixmap canvas = DeckCard::draw(1, false);

    label->setPixmap(canvas);
}
