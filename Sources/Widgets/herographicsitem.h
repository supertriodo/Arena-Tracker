#ifndef HEROGRAPHICSITEM_H
#define HEROGRAPHICSITEM_H

#include "miniongraphicsitem.h"
#include "weapongraphicsitem.h"
#include "../Cards/secretcard.h"


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
    HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    HeroGraphicsItem(HeroGraphicsItem *copy);

//Variables:
private:
    int armor, minionsAttack, minionsMaxAttack, resources, resourcesUsed;
    bool isNowBoard;
    QList<SecretIcon> secretsList;
    WeaponGraphicsItem * heroWeapon;

public:
    static const int WIDTH = 230;
    static const int HEIGHT = 184;

//Metodos
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
    void setMinionsAttack(int minionsAttack);
    void setResources(int resources);
    int getResources();
    void setResourcesUsed(int resourcesUsed);
    void setMinionsMaxAttack(int minionsMaxAttack);
    void setHeroWeapon(WeaponGraphicsItem * heroWeapon = NULL);
};

#endif // HEROGRAPHICSITEM_H
