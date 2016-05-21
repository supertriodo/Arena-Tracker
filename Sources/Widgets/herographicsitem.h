#ifndef HEROGRAPHICSITEM_H
#define HEROGRAPHICSITEM_H

#include "miniongraphicsitem.h"

class HeroGraphicsItem : public MinionGraphicsItem
{
//Constructor
public:
    HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    HeroGraphicsItem(HeroGraphicsItem *copy);

//Variables:
private:
    int armor;

public:
    static const int WIDTH = 230;
    static const int HEIGHT = 184;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void processTagChange(QString tag, QString value);
};

#endif // HEROGRAPHICSITEM_H
