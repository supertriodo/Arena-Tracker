#include "draftcard.h"

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
    QPixmap canvas = DeckCard::draw(this->code, 1);

    this->radioItem->setIcon(QIcon(canvas));
    this->radioItem->setToolTip("<html><img src=./HSCards/" + this->code + ".png/></html>");
}
