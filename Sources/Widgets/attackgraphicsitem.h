#ifndef ATTACKGRAPHICSITEM_H
#define ATTACKGRAPHICSITEM_H

#include <QGraphicsItem>


class AttackGraphicsItem : public QGraphicsItem
{
//Constructor
public:
    AttackGraphicsItem();

//Variables
private:
    QPoint from, to;
    bool friendly;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void setPointPos(bool isFrom, bool isHero, bool friendly, int pos, int minionsZone);
    bool isFriendly();
};

#endif // ATTACKGRAPHICSITEM_H
