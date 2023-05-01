#include "draftcard.h"
#include "../utility.h"
#include <QtWidgets>

DraftCard::DraftCard() : DraftCard("", false)
{
}

DraftCard::DraftCard(QString code) : DraftCard(code, false)
{
}

DraftCard::DraftCard(QString code, bool gold) : DeckCard(code)
{
    this->bestQualityMatches = 1;
    this->gold = gold;
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


bool DraftCard::isGold()
{
    return gold;
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
