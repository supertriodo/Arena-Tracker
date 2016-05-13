#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>

class MinionGraphicsItem : public QGraphicsItem
{
public:
    MinionGraphicsItem(QString code);

//Variables
    QString code;
    int attack, health;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

    void setZonePos(bool friendly, int pos, int minionsZone);
};

#endif // MINIONGRAPHICSITEM_H
