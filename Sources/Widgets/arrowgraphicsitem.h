#ifndef ARROWGRAPHICSITEM_H
#define ARROWGRAPHICSITEM_H

#include <QGraphicsItem>
#include "miniongraphicsitem.h"


class ArrowGraphicsItem : public QGraphicsItem
{
public:
    enum ArrowType { minionAttack, heroAttack, reinforcement };

//Constructor
public:
    ArrowGraphicsItem(ArrowType arrowType=minionAttack);

//Variables
private:
    MinionGraphicsItem *from;
    MinionGraphicsItem *to;
    bool friendly;
    ArrowType arrowType;
    int numAttacks;

//Metodos
private:
    QPixmap getArrowPixmap(int &margen);

public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool isFriendly();
    void setEnd(bool isFrom, MinionGraphicsItem *item);
    void prepareGeometryChange();
    void increaseNumAttacks();
    MinionGraphicsItem *getEnd(bool isFrom);
    ArrowType getArrowType();
};

#endif // ARROWGRAPHICSITEM_H
