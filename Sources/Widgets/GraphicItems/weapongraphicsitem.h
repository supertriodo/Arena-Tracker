#ifndef WEAPONGRAPHICSITEM_H
#define WEAPONGRAPHICSITEM_H

#include "miniongraphicsitem.h"

class WeaponGraphicsItem : public MinionGraphicsItem
{
//Constructor
public:
    WeaponGraphicsItem(QString code, int id, bool friendly, GraphicsItemSender *graphicsItemSender);
    WeaponGraphicsItem(WeaponGraphicsItem *copy);

//Variables:
private:
    int durability, origDurability;

public:
    static const int WIDTH = 162;
    static const int HEIGHT = 142;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool processTagChange(QString tag, QString value);
};

#endif // WEAPONGRAPHICSITEM_H
