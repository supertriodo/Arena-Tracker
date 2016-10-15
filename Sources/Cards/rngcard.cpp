#include "rngcard.h"
#include <QtWidgets>

RngCard::RngCard(QString code) : DeckCard(code)
{

}

RngCard::~RngCard()
{

}


void RngCard::draw()
{
    QPixmap canvas = drawCustomCard(this->code, "RNG:");

    this->listItem->setIcon(QIcon(canvas));
}
