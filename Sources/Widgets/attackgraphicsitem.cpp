#include "attackgraphicsitem.h"
#include "miniongraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>

AttackGraphicsItem::AttackGraphicsItem()
{
    this->setPos(0, 0);
}


bool AttackGraphicsItem::isFriendly()
{
    return this->friendly;
}


void AttackGraphicsItem::setPointPos(bool isFrom, bool isHero, bool friendly, int pos, int minionsZone)
{
    if(isFrom)  this->friendly = friendly;

    if(isHero)
    {
        const int hMinion = MinionGraphicsItem::HEIGHT-5;
        int x = 0;
        int y = friendly?hMinion + HeroGraphicsItem::HEIGHT/2:-hMinion - HeroGraphicsItem::HEIGHT/2;
        if(isFrom)  from = QPoint(x, y);
        else        to = QPoint(x, y);
    }
    else
    {
        const int hMinion = MinionGraphicsItem::HEIGHT-5;
        const int wMinion = MinionGraphicsItem::WIDTH-5;
        int x = wMinion*(pos - (minionsZone-1)/2.0);
        int y = friendly?hMinion/2:-hMinion/2;
        if(isFrom)  from = QPoint(x, y);
        else        to = QPoint(x, y);
    }
}


QRectF AttackGraphicsItem::boundingRect() const
{
    int left = std::min(from.x(), to.x());
    int top = std::min(from.y(), to.y());
    int width = std::abs(to.x()-from.x());
    int height = std::abs(to.y()-from.y());
    return QRectF(left, top, width, height);
}


void AttackGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QPen pen(RED);
    pen.setWidth(10);
    painter->setPen(pen);
    painter->drawLine(from, to);
}





















