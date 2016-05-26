#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>


class MinionGraphicsItem : public QGraphicsItem
{
//Constructor
public:
    MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    MinionGraphicsItem(MinionGraphicsItem *copy);

//Variables
protected:
    QString code;
    int origAttack, origHealth;
    int attack, health, id;
    int damage;
    bool friendly, exausted, playerTurn, dead;
    bool shield, taunt, charge, stealth, frozen, windfury;
    QList<QString> addons;

public:
    static const int WIDTH = 142;
    static const int HEIGHT = 184;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void processTagChange(QString tag, QString value);
    void setPlayerTurn(bool playerTurn);
    void setDead(bool value);
    void changeZone(bool playerTurn);
    QString getCode();
    void setZonePos(bool friendly, int pos, int minionsZone);
    int getId();
    bool isFriendly();
    void addAddon(QString code);
    void checkDownloadedCode(QString code);
};

#endif // MINIONGRAPHICSITEM_H
