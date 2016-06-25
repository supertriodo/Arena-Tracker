#include "cardgraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>


CardGraphicsItem::CardGraphicsItem( int id, QString code, QString createdByCode, int turn)
{
    this->code = code;
    this->createdByCode = createdByCode;
    this->id = id;
    this->played = this->discard = this->draw = false;
    this->heightShow = HEIGHT;
    this->turn = turn;
}


CardGraphicsItem::CardGraphicsItem(CardGraphicsItem *copy)
{
    this->code = copy->code;
    this->createdByCode = copy->createdByCode;
    this->id = copy->id;
    this->played = copy->played;
    this->discard = copy->discard;
    this->draw = copy->draw;
    this->heightShow = copy->heightShow;
    this->turn = copy->turn;
    this->setPos(copy->pos());
}


int CardGraphicsItem::getId()
{
    return this->id;
}


void CardGraphicsItem::setCode(QString code)
{
    this->code = code;
    update();
}


void CardGraphicsItem::setPlayed(bool played)
{
    this->played = played;
    prepareGeometryChange();
    update();
}


void CardGraphicsItem::setDiscard()
{
    this->discard = true;
    prepareGeometryChange();
    update();
}


void CardGraphicsItem::setDraw()
{
    this->draw = true;
    update();
}


void CardGraphicsItem::checkDownloadedCode(QString code)
{
    if(this->code == code || this->createdByCode == code)  update();
}


QRectF CardGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -heightShow/2+(played||discard?-CARD_LIFT:0), WIDTH, heightShow+(played||discard?CARD_LIFT:0));
}


void CardGraphicsItem::setZonePos(bool friendly, int pos, int cardsZone, int viewWidth, int cardHeightShow)
{
    if(cardHeightShow > HEIGHT)     cardHeightShow = HEIGHT;
    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    this->heightShow = cardHeightShow;
    viewWidth -= WIDTH+8 - viewWidth/cardsZone;
    const int wCard = std::min(175, viewWidth/cardsZone);
    int x = wCard*(pos - (cardsZone-1)/2.0);
    int y = friendly?hMinion + hHero + heightShow/2:-hMinion - hHero - heightShow/2;
    this->setPos(x, y);
}


void CardGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(played)          painter->drawPixmap(-WIDTH/2, -heightShow/2-CARD_LIFT, QPixmap(":/Images/bgCardGlow.png"), 0, 0, 190, heightShow+CARD_LIFT);
    else if(discard)    painter->drawPixmap(-WIDTH/2, -heightShow/2-CARD_LIFT, QPixmap(":/Images/bgCardDiscard.png"), 0, 0, 190, heightShow+CARD_LIFT);
    if(draw)            painter->drawPixmap(-WIDTH/2, -heightShow/2, QPixmap(":/Images/bgCardDraw.png"), 0, 0, 190, heightShow);

    if(!code.isEmpty())
    {
        painter->drawPixmap(-WIDTH/2, -heightShow/2+((played||discard)&&!draw?-CARD_LIFT:0),
                            QPixmap(Utility::hscardsPath() + "/" + code + ".png"), 5, 34, WIDTH,
                            heightShow+((played||discard)&&!draw?CARD_LIFT:0));
    }

    else if(!createdByCode.isEmpty())
    {
        painter->drawPixmap(-48, -heightShow/2+24+((played||discard)&&!draw?-CARD_LIFT:0),
                            QPixmap(Utility::hscardsPath() + "/" + createdByCode + ".png"), 49, 60, 101, 66);
        painter->drawPixmap(-81, -heightShow/2+15+((played||discard)&&!draw?-CARD_LIFT:0),
                            QPixmap(":/Images/bgCardCreatedBy.png"), 0, 0, 168,
                            heightShow-15+((played||discard)&&!draw?CARD_LIFT:0));
    }

    else
    {
        painter->drawPixmap(-81, -heightShow/2+15+((played||discard)&&!draw?-CARD_LIFT:0),
                            QPixmap(":/Images/bgCardUnknown.png"), 0, 0, 168,
                            heightShow-15+((played||discard)&&!draw?CARD_LIFT:0));

        //Turn
        QFont font("Belwe Bd BT");
        font.setPixelSize(40);
        font.setBold(true);
        font.setKerning(true);
    #ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    #else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    #endif
        painter->setFont(font);
        QPen pen(BLACK);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(WHITE);
        QString text = "T"+QString::number((this->turn+1)/2);
        QFontMetrics fm(font);
        int textWide = fm.width(text);
        int textHigh = fm.height();
        QPainterPath path;
        path.addText(-35 - textWide/2, -heightShow/2 + 71 + +((played||discard)&&!draw?-CARD_LIFT:0) + textHigh/4, font, text);
        painter->drawPath(path);
    }



}
