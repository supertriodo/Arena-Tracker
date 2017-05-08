#include "draftcard.h"
#include "../utility.h"
#include <QtWidgets>

DraftCard::DraftCard() : DraftCard("")
{

}

DraftCard::DraftCard(QString code) : DeckCard(code)
{
    this->sumQualityMatches = 0;
}

DraftCard::~DraftCard()
{

}


void DraftCard::addQualityMatch(double matchScore)
{
    this->sumQualityMatches += matchScore;
}


void DraftCard::setQualityMatch(double matchScore)
{
    this->sumQualityMatches = matchScore;
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
