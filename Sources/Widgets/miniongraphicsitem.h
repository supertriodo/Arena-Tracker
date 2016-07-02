#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>


class Addon
{
public:
    enum AddonType { AddonNeutral, AddonDamage, AddonLife };

    QString code;
    int id;
    int number;
    AddonType type;
};


class MinionGraphicsItem : public QGraphicsItem
{
public:
    enum ValueChange { ChangePositive, ChangeNegative, ChangeNone };

//Constructor
    MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    MinionGraphicsItem(MinionGraphicsItem *copy, bool triggerMinion=false);

//Variables
protected:
    QString code;
    int origAttack, origHealth;
    int attack, health, id;
    int damage;
    bool friendly, exausted, playerTurn, dead, hero, toBeDestroyed;
    bool shield, taunt, charge, stealth, frozen, windfury;
    QList<Addon> addons;
    bool addonsStacked;
    bool triggerMinion;
    bool aura;
    QString zone;
    ValueChange changeAttack, changeHealth;

public:
    static const int WIDTH = 142;
    static const int HEIGHT = 184;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool processTagChange(QString tag, QString value);
    void setPlayerTurn(bool playerTurn);
    void setDead(bool value);
    void changeZone();
    QString getCode();
    void setZonePos(bool friendly, int pos, int minionsZone);
    int getId();
    bool isFriendly();
    void addAddon(Addon addon);
    void addAddon(QString code, int id, Addon::AddonType type, int number=1);
    void checkDownloadedCode(QString code);
    bool isDead();
    bool isHero();
    bool isTriggerMinion();    
    bool isAura();    
    void setId(int value);
    void setChangeAttack(ValueChange value);
    void setChangeHealth(ValueChange value);
    int getAttack();
    int getHealth();    
    int getPotencialDamage();
    int getRemainingHealth();

private:
    void addAddonNeutral(Addon addon);
    void addAddonDamageLife(Addon addon);
    void stackAddons();
};

#endif // MINIONGRAPHICSITEM_H
