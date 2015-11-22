#include "draftcard.h"
#include "../utility.h"
#include <QtWidgets>

DraftCard::DraftCard() : DeckCard("")
{

}

DraftCard::DraftCard(QString code) : DeckCard(code)
{

}

DraftCard::~DraftCard()
{

}


void DraftCard::draw()
{
    QPixmap canvas = DeckCard::draw(1, false);

    this->radioItem->setIcon(QIcon(canvas));
}
