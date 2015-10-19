#include "deckcard.h"
#include "utility.h"
#include <QtWidgets>


QMap<QString, QJsonObject> * DeckCard::cardsJson;


DeckCard::DeckCard(QString code)
{
    setCode(code);
    listItem = NULL;
    total = remaining = 1;
}


DeckCard::~DeckCard()
{

}


void DeckCard::setCode(QString code)
{
    this->code = code;

    if(!code.isEmpty())
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


QString DeckCard::tooltip()
{
    return "<html><img src=./HSCards/" + this->code + ".png/></html>";
}


void DeckCard::draw(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip(tooltip());
}

void DeckCard::drawGreyed(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(
                            CARD_SIZE, QIcon::Disabled, QIcon::On)));
    this->listItem->setToolTip(tooltip());
}

QPixmap DeckCard::draw(uint total, bool drawRarity, QColor nameColor, int cardHeight)
{
    QFont font("Belwe Bd BT");

    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")               source = QRectF(63,18,100,25);
        else if(type==QString("Minion"))    source = QRectF(48,72,100,25);
        else                                source = QRectF(48,98,100,25);
        if(total > 1)                       target = QRectF(100,6,100,25);
        else                                target = QRectF(113,6,100,25);
        painter.drawPixmap(target, QPixmap(Utility::appPath() + "/HSCards/" + ((name=="unknown")?name:code) + ".png"), source);

        //Background and #cards
        if(type==QString("Minion"))         painter.setPen(QPen(WHITE));
        else if (type==QString("Spell"))    painter.setPen(QPen(YELLOW));
        else                                painter.setPen(QPen(ORANGE));

        if(total == 1 && rarity != "Legendary")  painter.drawPixmap(0,0,QPixmap(":Images/bgCard1.png"));
        else
        {
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard2.png"));

            if(total > 1)
            {
                font.setPointSize(16);
                painter.setFont(font);
                painter.drawText(QRectF(190,6,26,24), Qt::AlignCenter, QString::number(total));
            }
            else
            {
                font.setPointSize(20);
                painter.setFont(font);
                painter.drawText(QRectF(190,9,26,24), Qt::AlignCenter, "*");
            }
        }

        //Name and mana
        font.setPointSize(10);
        painter.setFont(font);
        if(name == "unknown")
        {
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(35,7,174,23), Qt::AlignVCenter, "Unknown");
        }
        else
        {
            if(drawRarity)              painter.setPen(QPen(getRarityColor()));
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
            if(drawRarity)                      painter.setPen(QPen(getRarityColor()));
            else if(nameColor!=BLACK)           painter.setPen(QPen(nameColor));
            else if(type==QString("Minion"))    painter.setPen(QPen(WHITE));
            else if (type==QString("Spell"))    painter.setPen(QPen(YELLOW));
            else                                painter.setPen(QPen(ORANGE));
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


QColor DeckCard::getRarityColor()
{
    if(rarity == "Free")               return WHITE;
    else if(rarity == "Common")        return WHITE;
    else if(rarity == "Rare")          return BLUE;
    else if(rarity == "Epic")          return VIOLET;
    else if(rarity == "Legendary")     return ORANGE;
    else                                return BLACK;
}


QString DeckCard::getCode()
{
    return code;
}


QString DeckCard::getType()
{
    return type;
}


QString DeckCard::getName()
{
    return name;
}


int DeckCard::getCost()
{
    return cost;
}


void DeckCard::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    DeckCard::cardsJson = cardsJson;
}
