#include "drawcard.h"

DrawCard::DrawCard(QString code) : DeckCard(code)
{

}

DrawCard::~DrawCard()
{

}


void DrawCard::draw()
{
    QPixmap canvas = DeckCard::draw(this->code, 1, false, ORANGE);

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip("<html><img src=./HSCards/" + this->code + ".png/></html>");
}
