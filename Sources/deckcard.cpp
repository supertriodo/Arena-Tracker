#include "deckcard.h"
#include <QtWidgets>


QMap<QString, QJsonObject> * DeckCard::cardsJson;


DeckCard::DeckCard(QString code)
{
    this->code = code;

    if(!code.isEmpty()) cost = (*cardsJson)[code].value("cost").toInt();
    else                cost = -1;

    listItem = NULL;
    total = remaining = 1;
}


DeckCard::~DeckCard()
{

}


void DeckCard::draw(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(this->code, drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip("<html><img src=./HSCards/" + this->code + ".png/></html>");
}

void DeckCard::drawGreyed(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(this->code, drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(
                            CARD_SIZE, QIcon::Disabled, QIcon::On)));
    this->listItem->setToolTip("<html><img src=./HSCards/" + this->code + ".png/></html>");
}

QPixmap DeckCard::draw(QString code, uint total, bool drawRarity, QColor nameColor, int cardHeight)
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

        //Name and mana
        font.setPointSize(10);
        painter.setFont(font);
        if(name=="")
        {
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(35,7,174,23), Qt::AlignVCenter, "Unknown");
        }
        else
        {
            if(drawRarity)              painter.setPen(QPen(getRarityColor(code)));
            else if(nameColor!=BLACK)   painter.setPen(QPen(nameColor));
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
            if(drawRarity)                      painter.setPen(QPen(getRarityColor(code)));
            else if(nameColor!=BLACK)           painter.setPen(QPen(nameColor));
            else if(type!=QString("Minion"))    painter.setPen(QPen(YELLOW));
            else                                painter.setPen(QPen(WHITE));
            painter.drawText(QRectF(1,6,26,24), Qt::AlignCenter, QString::number(cost));
        }
    painter.end();

    //Adapt to size
    if(cardHeight<35)
    {
        if(cardHeight==30)
        {
            return canvas.copy(0,0+3,218,35-5);
        }
        else
        {
            canvas = canvas.copy(0,0+6,218,35-10);

            if(cardHeight<25)   canvas = canvas.scaled(QSize(218,cardHeight));
            return canvas;
        }
    }

    return canvas;
}


QColor DeckCard::getRarityColor(QString code)
{
    QString rarityS = (*cardsJson)[code].value("rarity").toString();

    if(rarityS == "Free")               return WHITE;
    else if(rarityS == "Common")        return WHITE;
    else if(rarityS == "Rare")          return BLUE;
    else if(rarityS == "Epic")          return VIOLET;
    else if(rarityS == "Legendary")     return ORANGE;
    else                                return BLACK;
}


void DeckCard::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    DeckCard::cardsJson = cardsJson;
}
