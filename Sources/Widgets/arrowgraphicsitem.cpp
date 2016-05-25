#include "arrowgraphicsitem.h"
#include "miniongraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>

ArrowGraphicsItem::ArrowGraphicsItem(bool attack)
{
    this->attack = attack;
    this->setPos(0, 0);
    this->setZValue(1);
}


bool ArrowGraphicsItem::isFriendly()
{
    return this->friendly;
}


void ArrowGraphicsItem::setEnd(bool isFrom, MinionGraphicsItem *item)
{
    if(isFrom)
    {
        from = item;
        this->friendly = item->isFriendly();
    }
    else
    {
        to = item;
    }
}


QRectF ArrowGraphicsItem::boundingRect() const
{
    QPointF from = this->from->pos();
    QPointF to = this->to->pos();
    float left = std::min(from.x(), to.x());
    float top = std::min(from.y(), to.y());
    float width = std::abs(to.x()-from.x());
    float height = std::abs(to.y()-from.y());
    if(width == 0)
    {
        left -= 25;
        width = 50;
    }
    if(height == 0)
    {
        top -= 25;
        height = 50;
    }
    return QRectF(left, top, width, height);
}


void ArrowGraphicsItem::prepareGeometryChange()
{
    QGraphicsItem::prepareGeometryChange();
}


void ArrowGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QLine line(this->from->pos().toPoint(), this->to->pos().toPoint());
    QVector2D vector(line.dx(), line.dy());
    QPixmap pixmap(":Images/arrow"+QString(attack?"Attack":"Reinforce")+".png");
    const int margen = attack?115:80;
    const int arrowHeight = vector.length()-margen;
    const int arrowWide = std::max(20, 50 - arrowHeight/10);
    pixmap = pixmap.scaled(QSize(arrowHeight,arrowWide), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    qreal rot = qAtan2(vector.y(), vector.x());
    QPointF origin;
    QPixmap rotImage = Utility::getTransformedImage(pixmap, line.p1(), QPoint(-margen/2,arrowWide/2), rot, origin);
    painter->drawPixmap(origin, rotImage);
}


























