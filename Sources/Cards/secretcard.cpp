#include "secretcard.h"
#include "../utility.h"
#include <QtWidgets>

SecretCard::SecretCard() : DeckCard("")
{
    treeItem = NULL;
    hero = INVALID_CLASS;
}

SecretCard::SecretCard(QString code) : DeckCard(code)
{
    treeItem = NULL;
    hero = INVALID_CLASS;
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
        canvas = DeckCard::draw(1, true, BLACK, false);
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
        case MAGE:
        painter.drawPixmap(5,4,QPixmap(":Images/secretMage.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretMage.png"));
        break;

        case HUNTER:
        painter.drawPixmap(5,4,QPixmap(":Images/secretHunter.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretHunter.png"));
        break;

        case PALADIN:
        painter.drawPixmap(5,4,QPixmap(":Images/secretPaladin.png"));
        painter.drawPixmap(186,4,QPixmap(":Images/secretPaladin.png"));
        break;

        default:
        break;
    }
    painter.end();

    if(this->listItem != NULL)
    {
        this->listItem->setIcon(QIcon(resizeCardHeight(canvas)));
    }
    if(this->treeItem != NULL)
    {
        this->treeItem->setIcon(0, QIcon(resizeCardHeight(canvas)));
    }
}
