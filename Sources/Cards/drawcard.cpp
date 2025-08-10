#include "drawcard.h"

DrawCard::DrawCard(QString code) : DeckCard(code)
{

}

DrawCard::~DrawCard()
{

}


void DrawCard::draw()
{
    QPixmap canvas = DeckCard::draw(1, false);

    this->listItem->setIcon(QIcon(canvas));
}
