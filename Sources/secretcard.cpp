#include "secretcard.h"
#include <QtWidgets>

SecretCard::SecretCard() : DeckCard("")
{
    treeItem = NULL;
}

SecretCard::SecretCard(QString code) : DeckCard(code)
{
    treeItem = NULL;
}

SecretCard::~SecretCard()
{

}


void SecretCard::draw()
{
    if(this->code != "")
    {
        QPixmap canvas = DeckCard::draw(this->code, 1);

        this->treeItem->setIcon(0, QIcon(canvas));
        this->treeItem->setToolTip(0, "<html><img src=./HSCards/" + this->code + ".png/></html>");
    }
    else
    {
        QPixmap canvas(CARD_SIZE);
        QPainter painter;
        painter.begin(&canvas);
            painter.fillRect(canvas.rect(), Qt::black);
            painter.drawPixmap(0,0,QPixmap(":Images/handCard1.png"));
        painter.end();

        this->treeItem->setIcon(0, QIcon(canvas));
    }
}
