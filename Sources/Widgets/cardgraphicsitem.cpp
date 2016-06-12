#include "cardgraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>


CardGraphicsItem::CardGraphicsItem(QString code, int id)
{
    this->code = code;
    this->id = id;
    this->played = false;
}


CardGraphicsItem::CardGraphicsItem(CardGraphicsItem *copy)
{
    this->code = copy->code;
    this->id = copy->id;
    this->played = copy->played;
    this->setPos(copy->pos());
}


int CardGraphicsItem::getId()
{
    return this->id;
}


void CardGraphicsItem::setPlayed()
{
    this->played = true;
    prepareGeometryChange();
    update();
}


void CardGraphicsItem::checkDownloadedCode(QString code)
{
    if(this->code == code)  update();
}


QRectF CardGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, (played?-30-7:0)-HEIGHT/2, WIDTH, (played?7:0)+HEIGHT);
}


void CardGraphicsItem::setZonePos(bool friendly, int pos, int cardsZone, int viewWidth)
{
    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    const int hCard = HEIGHT;
    viewWidth -= WIDTH - viewWidth/cardsZone;
    const int wCard = std::min(175/*166*/, viewWidth/cardsZone);
    int x = wCard*(pos - (cardsZone-1)/2.0);
    int y = friendly?hMinion + hHero + hCard/2:-hMinion - hHero - hCard/2;
    this->setPos(x, y);
}


void CardGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(played)  painter->drawPixmap(-WIDTH/2, (played?-30:0)-HEIGHT/2-7, QPixmap(":/Images/bgCardGlow.png"));
    painter->drawPixmap(-WIDTH/2, (played?-30:0)-HEIGHT/2, QPixmap(Utility::hscardsPath() + "/" + code + ".png"), 5, 34, WIDTH, HEIGHT);
}
