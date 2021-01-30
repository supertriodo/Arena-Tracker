#include "popularcard.h"
#include <QtWidgets>

PopularCard::PopularCard(QString code, QString manaText) : DeckCard(code)
{
    this->manaText = manaText;
}

PopularCard::~PopularCard()
{

}


void PopularCard::draw()
{
    QPixmap canvas = DeckCard::draw(1, false, BLACK, manaText);
    this->listItem->setIcon(QIcon(canvas));
}

