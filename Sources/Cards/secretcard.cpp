#include "secretcard.h"
#include "../utility.h"
#include "../themehandler.h"
#include <QtWidgets>

SecretCard::SecretCard() : DeckCard("")
{
    this->manaText = "";
}

SecretCard::SecretCard(QString code) : DeckCard(code)
{
    this->manaText = "";
}

SecretCard::SecretCard(QString code, QString manaText) : DeckCard(code)
{
    this->manaText = manaText;
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
        canvas = DeckCard::draw(1, true, BLACK, true, manaText);
    }
    else
    {
        canvas = QPixmap(CARD_SIZE);

        painter.begin(&canvas);
            painter.fillRect(canvas.rect(), Qt::black);
            painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardFile()));
        painter.end();
    }

    if(this->listItem != nullptr)
    {
        if(remaining == 0)  this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(canvas.size(), QIcon::Disabled, QIcon::On)));
        else                this->listItem->setIcon(QIcon(canvas));
    }
}
