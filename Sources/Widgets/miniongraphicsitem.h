#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>

class MinionGraphicsItem : public QGraphicsItem
{

//Constructor
public:
    MinionGraphicsItem(QString code, int id);

//Variables
private:
    QString code;
    int attack, health, id;

//Metodos
public:
    void setZonePos(bool friendly, int pos, int minionsZone);
    int getId();


public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

#endif // MINIONGRAPHICSITEM_H
