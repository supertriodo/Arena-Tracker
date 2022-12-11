#ifndef HEROGRAPHICSITEM_H
#define HEROGRAPHICSITEM_H

#include "miniongraphicsitem.h"
#include "weapongraphicsitem.h"
#include "../../Cards/secretcard.h"


class HeroGraphicsItem : public MinionGraphicsItem
{
    class SecretIcon
    {
    public:
        int id;
        QString code;
        CardClass secretHero;
    };

//Constructor
public:
    HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender);
    HeroGraphicsItem(HeroGraphicsItem *copy, bool copySecretCodes=false);

//Variables:
private:
    QString heroCode;
    int armor, minionsAttack, minionsMaxAttack, resources, resourcesUsed, corpses, corpsesUsed, spellDamage;
    bool showAllInfo;
    QList<SecretIcon> secretsList;
    WeaponGraphicsItem * heroWeapon;

public:
    static const int WIDTH = 230;
    static const int HEIGHT = 184;

//Metodos
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QGraphicsSceneWheelEvent *event) Q_DECL_OVERRIDE;

private:
    void sendHeroTotalAttackChange();
    void initHeroCode();

public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool processTagChange(QString tag, QString value);
    void addSecret(int id, CardClass secretHero);
    void removeSecret(int id);
    void showSecret(int id, QString code);
    CardClass getSecretHero(int id);
    bool isYourSecret(int id);
    void changeHero(QString code, int id);
    void setMinionsAttack(int minionsAttack, bool sendHandTab);
    void setResources(int resources);
    int getResources();
    int getArmorHealth();
    void setResourcesUsed(int resourcesUsed);
    void setMinionsMaxAttack(int minionsMaxAttack);
    void setHeroWeapon(WeaponGraphicsItem * heroWeapon = nullptr, bool isNowBoard = true);
    void setPlayerTurn(bool playerTurn);
    void setSpellDamage(int spellDamage);
    int getSpellDamage();
    void setShowAllInfo(bool value=true);
    void addResourcesUsed(int value);
    int getAvailableResources();
    void damagePlanningMinion(int damage=1);
    void checkDownloadedCode(QString code);
    QList<CardClass> getCardClass();
    void setCorpses(int corpses);
    void setCorpsesUsed(int corpsesUsed);
};

#endif // HEROGRAPHICSITEM_H
