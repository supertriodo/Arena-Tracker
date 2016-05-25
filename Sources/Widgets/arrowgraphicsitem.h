#ifndef ARROWGRAPHICSITEM_H
#define ARROWGRAPHICSITEM_H

#include <QGraphicsItem>
#include "miniongraphicsitem.h"


class ArrowGraphicsItem : public QGraphicsItem
{
//Constructor
public:
    ArrowGraphicsItem(bool attack=true);

//Variables
private:
    MinionGraphicsItem *from;
    MinionGraphicsItem *to;
    bool friendly;
    bool attack;
    int numAttacks;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool isFriendly();
    void setEnd(bool isFrom, MinionGraphicsItem *item);
    void prepareGeometryChange();
    void increaseNumAttacks();
    MinionGraphicsItem *getEnd(bool isFrom);
};

#endif // ARROWGRAPHICSITEM_H
