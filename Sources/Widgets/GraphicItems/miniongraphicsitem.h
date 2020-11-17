#ifndef MINIONGRAPHICSITEM_H
#define MINIONGRAPHICSITEM_H

#include <QGraphicsItem>
#include "graphicsitemsender.h"
#include "cardgraphicsitem.h"


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
    MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender);
    MinionGraphicsItem(MinionGraphicsItem *copy, bool triggerMinion=false);

//Variables
protected:
    QString code;
    int origAttack, origHealth;
    int attack, health, id;
    int damage;
    bool friendly, exausted, playerTurn, dead, hero, toBeDestroyed;
    bool shield, taunt, charge, rush, stealth, frozen, windfury;
    QList<Addon> addons;
    bool addonsStacked;
    bool triggerMinion;
    bool aura;
    QString zone;
    ValueChange changeAttack, changeHealth;
    GraphicsItemSender *graphicsItemSender;
    float deadProb;

public:
    static const int WIDTH = 142;
    static const int HEIGHT = 184;

//Metodos
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QGraphicsSceneWheelEvent *event) Q_DECL_OVERRIDE;

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
    int getPotencialDamage(bool ignoreExausted=false);
    int getRemainingHealth();
    void setDeadProb(float value=0);
    int getHitsToDie(int missileDamage);
    void selectMinion(bool isSelected=true);
    void damagePlanningMinion(int damage=1);
    void setExausted(bool value=true);
    void updateStatsFromCard(CardGraphicsItem *card);
    void addPlanningAddon(QString code, Addon::AddonType type);
    void changeCode(QString newCode);

private:
    void addAddonNeutral(Addon addon);
    void addAddonDamageLife(Addon addon);
    void stackAddons();
    void healPlanningMinion(bool allowRecoverShield=false);
    void initCode(QString code);
};

#endif // MINIONGRAPHICSITEM_H
