#include "arrowgraphicsitem.h"
#include "miniongraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>

ArrowGraphicsItem::ArrowGraphicsItem(ArrowType arrowType)
{
    this->arrowType = arrowType;
    this->numAttacks = 1;
    this->setPos(0, 0);

    if(arrowType == reinforcement)  this->setZValue(1);
    else                            this->setZValue(2);
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

        if(item->isHero())
        {
            this->arrowType = heroAttack;
            this->setZValue(3);
        }
    }
    else
    {
        to = item;
    }
}


MinionGraphicsItem * ArrowGraphicsItem::getEnd(bool isFrom)
{
    if(isFrom)  return this->from;
    else        return this->to;
}


ArrowGraphicsItem::ArrowType ArrowGraphicsItem::getArrowType()
{
    return this->arrowType;
}


void ArrowGraphicsItem::increaseNumAttacks()
{
    this->numAttacks++;
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


QPixmap ArrowGraphicsItem::getArrowPixmap(int &margen)
{
    switch(arrowType)
    {
        case heroAttack:
            margen = 120;
            return QPixmap(":Images/arrowHeroAttack.png");
        case reinforcement:
            margen = 80;
            return QPixmap(":Images/arrowReinforce.png");
        default:
            margen = 115;
            return QPixmap (":Images/arrowAttack.png");
    }
}


void ArrowGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QLine line(this->from->pos().toPoint(), this->to->pos().toPoint());
    QVector2D vector(line.dx(), line.dy());
    int margen;
    QPixmap pixmap = getArrowPixmap(margen);
    const int arrowHeight = vector.length()-margen;
    const int arrowWide = std::max(15, 80 - arrowHeight/5);
    pixmap = pixmap.scaled(QSize(arrowHeight,arrowWide), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    qreal rot = qAtan2(vector.y(), vector.x());
    QPointF origin;
    QPixmap rotImage = Utility::getTransformedImage(pixmap, line.p1(), QPoint(-margen/2,arrowWide/2), rot, origin);
    painter->drawPixmap(origin, rotImage);

    if(this->numAttacks > 1)
    {
        QString text = "x" + QString::number(this->numAttacks);
        QPoint midPoint = line.p1() + QPoint(line.dx()/2, line.dy()/2);

        QFont font("Belwe Bd BT");
        font.setPixelSize(45);
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

        QFontMetrics fm(font);
        int textWide = fm.width(text);
        int textHigh = fm.height();
        QPainterPath path;
        path.addText(midPoint.x() - textWide/2, midPoint.y() + textHigh/4, font, text);
        painter->drawPath(path);
    }
}


