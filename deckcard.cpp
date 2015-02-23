#include "deckcard.h"
#include <QtWidgets>



DeckCard::DeckCard(QMap<QString, QJsonObject> *cardsJson)
{
    listItem = NULL;
    code = "";
    cost = 0;
    total = remaining = 1;
    this->cardsJson = cardsJson;
}


DeckCard::~DeckCard()
{

}


void DeckCard::draw()
{
    draw(true);
}
void DeckCard::draw(bool drawTotal)
{
    draw(this->listItem, this->code, drawTotal?this->total:this->remaining);
}
void DeckCard::draw(QListWidgetItem * item, QString code, uint total)
{
    QString type = (*cardsJson)[code].value("type").toString();
    QString name = (*cardsJson)[code].value("name").toString();
    int cost = (*cardsJson)[code].value("cost").toInt();

    if(code=="")
    {
        code = "unknown";
        type = "Minion";
    }
    QFont font("Belwe Bd BT");

    QPixmap canvas(CARD_SIZE);
    QPainter painter;
    painter.begin(&canvas);
        //Card
        painter.fillRect(canvas.rect(), Qt::black);
        QRectF target;
        QRectF source;
        if(code == "unknown")               source = QRectF(63,18,100,25);
        else if(type==QString("Minion"))    source = QRectF(48,72,100,25);
        else                                source = QRectF(48,98,100,25);
        if(total > 1)                       target = QRectF(100,6,100,25);
        else                                target = QRectF(113,6,100,25);
        if(type!=QString("Minion"))     painter.setPen(QPen(YELLOW));
        else                            painter.setPen(QPen(WHITE));
        painter.drawPixmap(target, QPixmap("./HSCards/" + code + ".png"), source);

        //Background and #cards
        if(total == 1)  painter.drawPixmap(0,0,QPixmap(":Images/bgCard1.png"));
        else
        {
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard2.png"));

            font.setPointSize(16);
            painter.setFont(font);
            painter.drawText(QRectF(190,6,26,24), Qt::AlignCenter, QString::number(total));
        }

        //Name
        font.setPointSize(10);
        painter.setFont(font);
        if(name=="")
        {
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(35,7,174,23), Qt::AlignVCenter, "Unknown");
        }
        else
        {
            painter.drawText(QRectF(35,7,174,23), Qt::AlignVCenter, name);

            //Mana cost
            int manaSize = cost>9?6:cost;
            font.setPointSize(14+manaSize);
            font.setBold(true);
            painter.setFont(font);
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(0,6,26,24), Qt::AlignCenter, QString::number(cost));
            font.setPointSize(12+manaSize);
            font.setBold(false);
            painter.setFont(font);
            if(type!=QString("Minion"))     painter.setPen(QPen(YELLOW));
            else                            painter.setPen(QPen(WHITE));
            painter.drawText(QRectF(1,6,26,24), Qt::AlignCenter, QString::number(cost));
        }
    painter.end();

    item->setIcon(QIcon(canvas));
    item->setToolTip("<html><img src=./HSCards/" + code + ".png/></html>");
}
