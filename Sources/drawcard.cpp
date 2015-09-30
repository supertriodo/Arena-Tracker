#include "drawcard.h"
#include "utility.h"
#include <QtWidgets>

DrawCard::DrawCard(QString code) : DeckCard(code)
{

}

DrawCard::~DrawCard()
{

}


void DrawCard::draw()
{
    QPixmap canvas = DeckCard::draw(1, false, ORANGE);

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip("<html><img src=" + Utility::appPath() + "/HSCards/" + this->code + ".png/></html>");
}
