#include "handcard.h"
#include <QtWidgets>


HandCard::HandCard(QString code) : DeckCard(code)
{
    id = turn = 0;
    special = false;
}


HandCard::~HandCard()
{

}


//Reusamos cost/type/name/rarity para createdByCode
void HandCard::setCreatedByCode(QString code)
{
    this->createdByCode = code;

    if(!this->code.isEmpty()) return;

    if(!createdByCode.isEmpty())
    {
        cost = (*cardsJson)[code].value("cost").toInt();
        type = (*cardsJson)[code].value("type").toString();
        name = (*cardsJson)[code].value("name").toString();
        rarity = (*cardsJson)[code].value("rarity").toString();
    }
    else
    {
        cost = -1;
        type = "Minion";
        name = "unknown";
        rarity = "";
    }
}


QString HandCard::getCreatedByCode()
{
    return this->createdByCode;
}


void HandCard::draw()
{
    if(!this->code.isEmpty())
    {
        DeckCard::draw();
    }
    else if(!this->createdByCode.isEmpty())
    {
        drawCreatedByHandCard();
    }
    else
    {
        drawDefaultHandCard();
    }
}


void HandCard::drawCreatedByHandCard()
{
    QFont font("Belwe Bd BT");

    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Card
        QRectF target = QRectF(113,6,100,25);;
        QRectF source;
        if(type==QString("Minion"))         source = QRectF(48,72,100,25);
        else                                source = QRectF(48,98,100,25);
        painter.drawPixmap(target, QPixmap(Utility::appPath() + "/HSCards/" + createdByCode + ".png"), source);

        //Background
        painter.drawPixmap(0,0,QPixmap(":Images/handCard3.png"));

        //Name
        font.setPointSize(12);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(QPen(WHITE));
        painter.drawText(QRectF(9,7,25,23), Qt::AlignVCenter, "BY:");

        font.setPointSize(11);
        font.setBold(false);
        painter.setFont(font);
        painter.setPen(QPen(BLACK));
        painter.drawText(QRectF(10,7,25,23), Qt::AlignVCenter, "BY:");

        if(type==QString("Minion"))         painter.setPen(QPen(WHITE));
        else if (type==QString("Spell"))    painter.setPen(QPen(YELLOW));
        else                                painter.setPen(QPen(ORANGE));
        font.setPointSize(10);
        font.setBold(false);
        painter.setFont(font);
        painter.drawText(QRectF(35,7,154,23), Qt::AlignVCenter, name);
    painter.end();

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip("<html><img src=./HSCards/" + this->createdByCode + ".png/></html>");
}


void HandCard::drawDefaultHandCard()
{
    QFont font("Belwe Bd BT");
    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
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
