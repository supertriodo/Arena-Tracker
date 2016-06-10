#include "miniongraphicsitem.h"
#include "../utility.h"
#include <QtWidgets>

MinionGraphicsItem::MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn)
{
    this->code = code;
    this->id = id;
    this->friendly = friendly;
    this->hero = false;
    this->attack = this->origAttack = Utility::getCardAtribute(code, "attack").toInt();
    this->health = this->origHealth = Utility::getCardAtribute(code, "health").toInt();
    this->damage = 0;
    this->shield = false;
    this->taunt = false;
    this->stealth = false;
    this->frozen = false;
    this->windfury = false;
    this->charge = false;
    this->exausted = true;
    this->dead = false;
    this->toBeDestroyed = false;
    this->playerTurn = playerTurn;
    this->addonsStacked = false;
    this->triggerMinion = false;
    this->aura = false;
    this->zone = "PLAY";
    this->changeAttack = ChangeNone;
    this->changeHealth = ChangeNone;

    foreach(QJsonValue value, Utility::getCardAtribute(code, "mechanics").toArray())
    {
        processTagChange(value.toString(), "1");
    }

    //Leokk AURA
    if(code == LEOKK)   processTagChange("AURA", "1");
}


MinionGraphicsItem::MinionGraphicsItem(MinionGraphicsItem *copy, bool triggerMinion)
{
    this->code = copy->code;
    this->id = copy->id;
    this->friendly = copy->friendly;
    this->hero = copy->hero;
    this->attack = copy->attack;
    this->origAttack = copy->origAttack;
    this->health = copy->health;
    this->origHealth = copy->origHealth;
    this->damage = copy->damage;
    this->shield = copy->shield;
    this->taunt = copy->taunt;
    this->stealth = copy->stealth;
    this->frozen = copy->frozen;
    this->windfury = copy->windfury;
    this->charge = copy->charge;
    this->exausted = copy->exausted;
    this->dead = copy->dead;
    this->toBeDestroyed = copy->toBeDestroyed;
    this->playerTurn = copy->playerTurn;
    this->addonsStacked = copy->addonsStacked;
    this->setPos(copy->pos());
    this->triggerMinion = triggerMinion;
    this->aura = copy->aura;
    this->zone = "PLAY";
    this->changeAttack = copy->changeAttack;
    this->changeHealth = copy->changeHealth;

    foreach(Addon addon, copy->addons)
    {
        this->addons.append(addon);
    }
}


int MinionGraphicsItem::getId()
{
    return this->id;
}


QString MinionGraphicsItem::getCode()
{
    return this->code;
}


int MinionGraphicsItem::getAttack()
{
    return attack;
}


int MinionGraphicsItem::getHealth()
{
    return health;
}


bool MinionGraphicsItem::isFriendly()
{
    return this->friendly;
}


bool MinionGraphicsItem::isDead()
{
    return this->dead;
}


bool MinionGraphicsItem::isHero()
{
    return this->hero;
}


bool MinionGraphicsItem::isTriggerMinion()
{
    return this->triggerMinion;
}


bool MinionGraphicsItem::isAura()
{
    return this->aura;
}


void MinionGraphicsItem::checkDownloadedCode(QString code)
{
    bool needUpdate = false;

    if(this->code == code)  needUpdate = true;
    foreach(Addon addon, this->addons)
    {
        if(addon.code == code)   needUpdate = true;
    }

    if(needUpdate)  this->update();
}


void MinionGraphicsItem::setPlayerTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    update();
}


void MinionGraphicsItem::setDead(bool value)
{
    this->dead = value;
    update();
}


void MinionGraphicsItem::setId(int value)
{
    this->id = value;
}


void MinionGraphicsItem::setChangeAttack(ValueChange value)
{
    if(changeAttack == ChangeNone)
    {
        changeAttack = value;
        update();
    }
}


void MinionGraphicsItem::setChangeHealth(ValueChange value)
{
    if(changeHealth == ChangeNone)
    {
        changeHealth = value;
        update();
    }
}


void MinionGraphicsItem::addAddon(QString code, int id, Addon::AddonType type, int number)
{
    Addon addon;
    addon.code = code;
    addon.id = id;
    addon.type = type;
    addon.number = number;
    this->addAddon(addon);
}


//Paso por valor para almacenar una copia de addon
//Solo un addon por id fuente, sino un hechizo con objetivo que causa damage pondria 2 addons (objetivo y damage)
//Muestra multiples addons life/damage.
//Solo muestra un neutral por id, si no hay ningun damage/life del mismo id.
void MinionGraphicsItem::addAddon(Addon addon)
{
    if(addon.type == Addon::AddonNeutral)   addAddonNeutral(addon);
    else                                    addAddonDamageLife(addon);
}


void MinionGraphicsItem::addAddonNeutral(Addon addon)
{
    foreach(Addon storedAddon, this->addons)
    {
        if(storedAddon.id == addon.id)  return;
    }
    this->addons.append(addon);
    update();
}


void MinionGraphicsItem::addAddonDamageLife(Addon addon)
{
    //Eliminar neutrales
    if(addon.code != SWIPE)//Siempre queremos ver el objetivo de swipe
    {
        for(int i=0; i<addons.count(); i++)
        {
            if(addons[i].id == addon.id && addons[i].type == Addon::AddonNeutral)
            {
                addons.removeAt(i);
                i--;
            }
        }
    }

    //Evitar addon si existe del elemento contrario
    Addon::AddonType opType = (addon.type == Addon::AddonDamage)?Addon::AddonLife:Addon::AddonDamage;
    for(int i=0; i<addons.count(); i++)
    {
        if(addons[i].code == addon.code && addons[i].type == opType)
        {
            qDebug()<<"Avoid addon."<<addon.code<<"Oposite found in minion.";
            return;
        }
    }

    //Incluir addon
    if(addonsStacked)
    {
        for(int i=0; i<addons.count(); i++)
        {
            if(addons[i].code == addon.code && addons[i].type == addon.type)
            {
                addons[i].number += addon.number;
                addon.number = 0;
                break;
            }
        }
        //Es nuevo
        if(addon.number > 0)
        {
            addons.append(addon);
        }
    }
    else
    {
        addons.append(addon);
        if(addons.count()>4)    stackAddons();
    }
    update();
}


void MinionGraphicsItem::stackAddons()
{
    if(addonsStacked)   return;
    addonsStacked = true;

    for(int i=0; i<addons.count(); i++)
    {
        if(addons[i].type!=Addon::AddonNeutral)
        {
            for(int j=i+1; j<addons.count(); j++)
            {
                if( addons[i].code == addons[j].code &&
                    addons[i].type == addons[j].type)
                {
                    addons[i].number += addons[j].number;
                    addons.removeAt(j);
                    j--;
                }
            }
        }
    }
}


void MinionGraphicsItem::changeZone()
{
    this->friendly = !this->friendly;
    this->exausted = !this->charge;
}


QRectF MinionGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


void MinionGraphicsItem::setZonePos(bool friendly, int pos, int minionsZone)
{
    const int hMinion = HEIGHT-5;
    const int wMinion = WIDTH-5;
    int x = wMinion*(pos - (minionsZone-1)/2.0);
    int y = friendly?hMinion/2:-hMinion/2;
    this->setPos(x, y);
}


bool MinionGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"TAG CHANGE -->"<<id<<tag<<value;

    //Evita addons provocado por cambios despues de morir(en el log los minion vuelven a damage 0 y estado original justo antes de desaparecer de la zona)
    //Terror de fatalidad envia TO_BE_DESTROYED despues de hacer 2 de damage, para dar tiempo a invocar el demonio.
    //Despues de morir por TO_BE_DESTROYED, vuelve a 0.
    if(this->damage >= this->health || this->zone !="PLAY" || this->toBeDestroyed)
    {
        this->dead = true;
    }

    bool healing = false;
    if(tag == "DAMAGE")
    {
        int newDamage = value.toInt();
        if(newDamage < this->damage)    healing = true;
        this->damage = newDamage;
    }
    if(tag == "TO_BE_DESTROYED" || tag == "SHOULDEXITCOMBAT")
    {
        this->toBeDestroyed = true;
        return healing;
    }
    else if(tag == "ATK")
    {
        this->attack = value.toInt();
    }
    else if(tag == "HEALTH")
    {
        this->health = value.toInt();
    }
    else if(tag == "EXHAUSTED")
    {
        this->exausted = (value=="1");
    }
    else if(tag == "DIVINE_SHIELD")
    {
        this->shield = (value=="1");
    }
    else if(tag == "TAUNT")
    {
        this->taunt = (value=="1");
    }
    else if(tag == "CHARGE")
    {
        this->charge = (value=="1");
        if(charge)    this->exausted = false;
    }
    else if(tag == "STEALTH")
    {
        this->stealth = (value=="1");
    }
    else if(tag == "FROZEN")
    {
        this->frozen = (value=="1");
    }
    else if(tag == "WINDFURY")
    {
        this->windfury = (value=="1");
        return healing;
    }
    else if(tag == "AURA")
    {
        this->aura = (value=="1");
        return healing;
    }
    else if (tag == "ZONE")
    {
        this->zone = value;
        return healing;
    }
    else
    {
        return healing;
    }
    update();
    return healing;
}


void MinionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(triggerMinion)   painter->setOpacity(0.5);

    //Card background
    painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + this->code + ".png")));
    painter->setBrushOrigin(QPointF(100,191));
    painter->drawEllipse(QPointF(0,0), 50, 68);

    //Stealth
    if(this->stealth)
    {
        painter->drawPixmap(-52, -71, QPixmap(":Images/bgMinionStealth.png"));
    }

    //Taunt/Frozen/Minion template
    bool glow = (!exausted && !frozen && (playerTurn==friendly) && attack>0);
    if(this->taunt)
    {
        painter->drawPixmap(-70, -96, QPixmap(":Images/bgMinionTaunt" + QString(glow?"Glow":"Simple") + ".png"));

        if(this->frozen)        painter->drawPixmap(-76, -82, QPixmap(":Images/bgMinionFrozen.png"));
        else                    painter->drawPixmap(-70, -80, QPixmap(":Images/bgMinionSimple.png"));
    }
    else
    {
        if(this->frozen)        painter->drawPixmap(-76, -82, QPixmap(":Images/bgMinionFrozen.png"));
        else                    painter->drawPixmap(-70, -80, QPixmap(":Images/bgMinion" + QString(glow?"Glow":"Simple") + ".png"));
    }



    //Attack/Health
    QFont font("Belwe Bd BT");
    font.setPixelSize(45);
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
    font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif
    painter->setFont(font);
    QPen pen(BLACK);
    pen.setWidth(2);
    painter->setPen(pen);

    if(attack>origAttack)   painter->setBrush(GREEN);
    else                    painter->setBrush(WHITE);
    QFontMetrics fm(font);
    int textWide = fm.width(QString::number(attack));
    int textHigh = fm.height();
    QPainterPath path;
    path.addText(-35 - textWide/2, 46 + textHigh/4, font, QString::number(attack));
    painter->drawPath(path);

    if(damage>0)                painter->setBrush(RED);
    else if(health>origHealth)  painter->setBrush(GREEN);
    else                        painter->setBrush(WHITE);
    textWide = fm.width(QString::number(health-damage));
    path = QPainterPath();
    path.addText(34 - textWide/2, 46 + textHigh/4, font, QString::number(health-damage));
    painter->drawPath(path);

    //Shield
    if(this->shield)
    {
        painter->drawPixmap(-71, -92, QPixmap(":Images/bgMinionShield.png"));
    }

    //Dead
    if(this->dead)
    {
        painter->drawPixmap(-23, 25, 46, 50, QPixmap(":Images/bgMinionDead.png"));
    }

    //Addons
    for(int i=0; i<this->addons.count() && i<4; i++)
    {
        QString addonCode = this->addons[i].code;
        int moveX, moveY;
        switch(i)
        {
            case 0:
                moveX = 0;
                moveY = 10;
                break;
            case 3:
                moveX = 0;
                moveY = -40;
                break;
            case 1:
                moveX = -25;
                moveY = -15;
                break;
            case 2:
                moveX = 25;
                moveY = -15;
                break;
        }

        if(addonCode == "FATIGUE")
        {
            painter->drawPixmap(moveX-32, moveY-32, QPixmap(":Images/bgFatigueAddon.png"));
        }
        else
        {
            painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + addonCode + ".png")));
            painter->setBrushOrigin(QPointF(100+moveX,202+moveY));
            painter->drawEllipse(QPointF(moveX,moveY), 32, 32);
        }

        QString addonPng;
        if(addons[i].type == Addon::AddonDamage)
        {
            addonPng = ":Images/bgMinionAddonDamage.png";
            painter->setBrush(RED);
        }
        else if(addons[i].type == Addon::AddonLife)
        {
            addonPng = ":Images/bgMinionAddonLife.png";
            painter->setBrush(GREEN);
        }
        else
        {
            addonPng = ":Images/bgMinionAddon.png";
            painter->setBrush(WHITE);
        }
        painter->drawPixmap(moveX-35, moveY-35, QPixmap(addonPng));

        //Numero
        if(addons[i].number > 1)
        {
            textWide = fm.width(QString::number(addons[i].number));
            path = QPainterPath();
            path.addText(moveX - textWide/2, moveY + textHigh/4, font, QString::number(addons[i].number));
            painter->drawPath(path);
        }
    }

    //Change ATK/HEALTH
    if(changeAttack != ChangeNone)
    {
        QString symbol;
        if(changeAttack == ChangePositive)
        {
            symbol = "+";
            painter->setBrush(GREEN);
        }
        else
        {
            symbol = "-";
            painter->setBrush(RED);
        }

        textWide = fm.width(symbol);
        path = QPainterPath();
        path.addText(-35 - textWide/2, 20 + textHigh/4, font, symbol);
        painter->drawPath(path);
    }
    if(changeHealth != ChangeNone)
    {
        QString symbol;
        if(changeHealth == ChangePositive)
        {
            symbol = "+";
            painter->setBrush(GREEN);
        }
        else
        {
            symbol = "-";
            painter->setBrush(RED);
        }

        textWide = fm.width(symbol);
        path = QPainterPath();
        path.addText(34 - textWide/2, 20 + textHigh/4, font, symbol);
        painter->drawPath(path);
    }
}
