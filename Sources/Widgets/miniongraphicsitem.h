#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>


class Addon
{
public:
    QString code;
    int id;
    int number;
};


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
    bool friendly, exausted, playerTurn, dead, hero;
    bool shield, taunt, charge, stealth, frozen, windfury;
    QList<Addon> addons;

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
    void addAddon(Addon addon);
    void addAddon(QString code, int id, int number=1);
    void checkDownloadedCode(QString code);
    bool isDead();
    bool isHero();
};

#endif // MINIONGRAPHICSITEM_H
