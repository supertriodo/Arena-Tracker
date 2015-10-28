#include "secretcard.h"
#include "../utility.h"
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
        QPixmap canvas = DeckCard::draw(1, true);

        this->treeItem->setIcon(0, QIcon(canvas));
        this->treeItem->setToolTip(0, tooltip());
    }
    else
    {
        QPixmap canvas(CARD_SIZE);
        QPainter painter;
        painter.begin(&canvas);
            painter.fillRect(canvas.rect(), Qt::black);
            painter.drawPixmap(0,0,QPixmap(":Images/handCard1.png"));

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
}
