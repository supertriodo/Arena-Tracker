#include "handcard.h"
#include <QtWidgets>


HandCard::HandCard(QMap<QString, QJsonObject> *cardsJson) : DeckCard(cardsJson)
{
    id = turn = 0;
    special = false;
}


HandCard::~HandCard()
{

}


void HandCard::draw()
{
    if(this->code != "")
    {
        DeckCard::draw();
    }
    else
    {
        QFont font("Belwe Bd BT");
        QPixmap canvas(CARD_SIZE);
        QPainter painter;
        painter.begin(&canvas);
            painter.fillRect(canvas.rect(), Qt::black);
            painter.drawPixmap(0,0,QPixmap(this->special?":Images/handCard2.png":":Images/handCard1.png"));

            font.setPointSize(18);
            font.setBold(true);
            painter.setFont(font);
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(154,6,40,22), Qt::AlignCenter, "T"+QString::number((this->turn+1)/2));

            font.setPointSize(16);
            font.setBold(false);
            painter.setFont(font);
            painter.setPen(QPen(WHITE));
            painter.drawText(QRectF(155,6,40,22), Qt::AlignCenter, "T"+QString::number((this->turn+1)/2));
        painter.end();

        this->listItem->setIcon(QIcon(canvas));
    }
}
