#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>


class MinionGraphicsItem : public QGraphicsItem
{

//Constructor
public:
    MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn);

//Variables
private:
    QString code;
    int origAttack, origHealth;
    int attack, health, id;
    int damage;
    bool friendly, exausted, playerTurn;
    bool shield, taunt;

//Metodos
public:
    void setZonePos(bool friendly, int pos, int minionsZone);
    int getId();


public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void processTagChange(QString tag, QString value);
    void setPlayerTurn(bool playerTurn);
    void changeZone(bool playerTurn);
};

#endif // MINIONGRAPHICSITEM_H
