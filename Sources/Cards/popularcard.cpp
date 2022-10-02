#include "popularcard.h"
#include <QtWidgets>

PopularCard::PopularCard(QString code, QString manaText, QStringList mechanics) : DeckCard(code)
{
    this->manaText = manaText;
    this->mechanics = mechanics;
}

PopularCard::~PopularCard()
{
    this->mechanics.clear();
}


void PopularCard::draw()
{
    QPixmap canvas = DeckCard::draw(1, false, BLACK, manaText, 0, mechanics);
    this->listItem->setIcon(QIcon(canvas));
}

