#include "secretcard.h"
#include "../utility.h"
#include <QtWidgets>

SecretCard::SecretCard() : SecretCard("")
{
}

SecretCard::SecretCard(QString code) : DeckCard(code)
{
    treeItem = NULL;
    hero = unknown;
}

SecretCard::~SecretCard()
{

}


void SecretCard::draw()
{
    QPixmap canvas;
    QPainter painter;

    if(this->code != "")
    {
        canvas = DeckCard::draw(1, true);
    }
    else
    {
        canvas = QPixmap(CARD_SIZE);

        painter.begin(&canvas);
            painter.fillRect(canvas.rect(), Qt::black);
            painter.drawPixmap(0,0,QPixmap(":Images/handCard1.png"));
        painter.end();
    }

    //Pinta ? de color
    painter.begin(&canvas);
    switch(hero)
    {
        case mage:
        painter.drawPixmap(5,4,QPixmap(":Images/secretMage.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretMage.png"));
        break;

        case hunter:
        painter.drawPixmap(5,4,QPixmap(":Images/secretHunter.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretHunter.png"));
        break;

        case paladin:
        painter.drawPixmap(5,4,QPixmap(":Images/secretPaladin.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretPaladin.png"));
        break;

        case unknown:
        break;
    }
    painter.end();

    this->treeItem->setIcon(0, QIcon(canvas));
}
