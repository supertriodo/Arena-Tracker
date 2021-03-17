#include "miniongraphicsitem.h"
#include "../../utility.h"
#include "../../themehandler.h"
#include <QtWidgets>

MinionGraphicsItem::MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender)
{
    initCode(code);

    this->id = id;
    this->friendly = friendly;
    this->playerTurn = playerTurn;

    this->graphicsItemSender = graphicsItemSender;
    this->setZValue(-50);
    setAcceptHoverEvents(true);
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
    this->rush = copy->rush;
    this->exausted = copy->exausted;
    this->dead = copy->dead;
    this->toBeDestroyed = copy->toBeDestroyed;
    this->playerTurn = copy->playerTurn;
    this->addonsStacked = copy->addonsStacked;
    this->triggerMinion = triggerMinion;
    this->aura = copy->aura;
    this->zone = "PLAY";
    this->changeAttack = copy->changeAttack;
    this->changeHealth = copy->changeHealth;
    this->deadProb = 0;
    this->graphicsItemSender = copy->graphicsItemSender;
    this->setPos(copy->pos());
    this->setZValue(copy->zValue());
    setAcceptHoverEvents(true);

    const QList<Addon> addonList = copy->addons;
    for(const Addon &addon: addonList)
    {
        this->addons.append(addon);
    }
}


void MinionGraphicsItem::changeCode(QString newCode)
{
    initCode(newCode);
    update();
}


void MinionGraphicsItem::initCode(QString code)
{
    this->code = code;
    this->hero = false;
    this->attack = this->origAttack = Utility::getCardAttribute(code, "attack").toInt();
    this->health = this->origHealth = Utility::getCardAttribute(code, "health").toInt();
    if(health <= 0) this->health = this->origHealth = Utility::getCardAttribute(code, "durability").toInt();

    this->damage = 0;
    this->shield = false;
    this->taunt = false;
    this->stealth = false;
    this->frozen = false;
    this->windfury = false;
    this->charge = false;
    this->rush = false;
    this->exausted = true;
    this->dead = false;
    this->toBeDestroyed = false;
    this->addonsStacked = false;
    this->triggerMinion = false;
    this->aura = false;
    this->zone = "PLAY";
    this->changeAttack = ChangeNone;
    this->changeHealth = ChangeNone;
    this->deadProb = 0;

    for(const QJsonValue &value: (const QJsonArray)Utility::getCardAttribute(code, "mechanics").toArray())
    {
        processTagChange(value.toString(), "1");
    }

    //Leokk AURA
    if(code == LEOKK || code == GRIMSCALE_ORACLE)   processTagChange("AURA", "1");
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


int MinionGraphicsItem::getPotencialDamage(bool ignoreExausted)
{
    if(!ignoreExausted && (exausted || frozen))     return 0;
    else if(windfury)                               return attack*2;
    else                                            return attack;
}


int MinionGraphicsItem::getHealth()
{
    return health;
}


int MinionGraphicsItem::getHitsToDie(int missileDamage)
{
    if(shield)  return getRemainingHealth() + missileDamage;
    else        return getRemainingHealth();
}


int MinionGraphicsItem::getRemainingHealth()
{
    return health - damage;
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
    for(const Addon &addon: qAsConst(this->addons))
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


void MinionGraphicsItem::setDeadProb(float value)
{
    if(!FLOATEQ(deadProb, value))
    {
        deadProb = value;
        update();
    }
}


void MinionGraphicsItem::setExausted(bool value)
{
    if(exausted != value)
    {
        exausted = value;
        update();
    }
}


void MinionGraphicsItem::selectMinion(bool isSelected)
{
    if(isSelected)
    {
        this->setScale(1.5);
        this->setZValue(0);
    }
    else
    {
        this->setScale(1);
        if(isHero())    this->setZValue(-20);
        else            this->setZValue(-50);
    }
    update();
}


void MinionGraphicsItem::damagePlanningMinion(int damage)
{
    if(this->shield && this->damage >=0)
    {
        this->shield = false;
    }
    else
    {
        this->damage += damage;
        if(this->damage >= this->health)    this->dead = true;
    }
    update();
}


void MinionGraphicsItem::healPlanningMinion(bool allowRecoverShield)
{
    if(allowRecoverShield && this->damage == 0)
    {
        this->shield = true;
    }
    else
    {
        this->damage--;
        if(this->damage < this->health)     this->dead = false;
    }
    update();
}


void MinionGraphicsItem::addPlanningAddon(QString code, Addon::AddonType type)
{
    Addon addon;
    addon.code = code;
    addon.id = -1;
    addon.type = type;
    addon.number = 1;

    //Nuevo neutral
    if(type == Addon::AddonNeutral)
    {
        this->addons.append(addon);
        update();
    }
    else
    {
        //Actualizamos damage/life addon
        for(int i=0; i<addons.count(); i++)
        {
            if(addons[i].code == addon.code && addons[i].type != Addon::AddonNeutral)
            {
                int number = 0;
                if(addons[i].type == Addon::AddonDamage)    number -= addons[i].number;
                else                                        number += addons[i].number;
                if(addon.type == Addon::AddonDamage)        number -= addon.number;
                else                                        number += addon.number;
                if(number == 0)
                {
                    addons.removeAt(i);
                    if(type == Addon::AddonLife)    healPlanningMinion(true);
                    else                            damagePlanningMinion();
                }
                else
                {
                    if(number < 0)
                    {
                        addons[i].type = Addon::AddonDamage;
                        addons[i].number = -number;
                    }
                    else
                    {
                        addons[i].type = Addon::AddonLife;
                        addons[i].number = number;
                    }

                    if(type == Addon::AddonLife)    healPlanningMinion();
                    else                            damagePlanningMinion();
                }

                update();
                return;
            }
        }
        //Nuevo damage/life addon
        this->addons.append(addon);
        if(type == Addon::AddonLife)    healPlanningMinion();
        else                            damagePlanningMinion();
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
    for(const Addon &storedAddon: qAsConst(this->addons))
    {
        if(storedAddon.id == addon.id)  return;
    }
    this->addons.append(addon);
    update();
}


void MinionGraphicsItem::addAddonDamageLife(Addon addon)
{
    //Eliminar neutrales
    if(addon.code != SWIPE && addon.code != EXPLOSIVE_SHOT && addon.code != POWERSHOT)
    {//Siempre queremos ver el objetivo de swipe/...
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
    this->exausted = !(this->charge || this->rush);
}


QRectF MinionGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


void MinionGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    QRectF boundRect = boundingRect();
    graphicsItemSender->sendPlanCardEntered(code,
                        mapToScene(boundRect.topLeft()).toPoint(),
                        mapToScene(boundRect.bottomRight()).toPoint());
}


void MinionGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    graphicsItemSender->sendPlanCardLeave();
}


void MinionGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    graphicsItemSender->minionPress(this, event->button());
}


void MinionGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    graphicsItemSender->minionWheel(this, (event->delta()>0?true:false));
}


void MinionGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF posMouse = event->pos();
    for(int i=std::min(3, addons.count()-1); i>=0; i--)
    {
        QString addonCode = this->addons[i].code;
        QPoint posAddon;
        switch(i)
        {
            case 0:
                posAddon = QPoint(0, 10);
                break;
            case 3:
                posAddon = QPoint(0, -40);
                break;
            case 1:
                posAddon = QPoint(-25, -15);
                break;
            case 2:
                posAddon = QPoint(25, -15);
                break;
        }

        if(QLineF(posMouse, posAddon).length() < 30)
        {
            QRectF boundRect = boundingRect();
            graphicsItemSender->sendPlanCardEntered(addonCode,
                                mapToScene(boundRect.topLeft()).toPoint(),
                                mapToScene(boundRect.bottomRight()).toPoint());
            return;
        }
    }

    QRectF boundRect = boundingRect();
    graphicsItemSender->sendPlanCardEntered(code,
                        mapToScene(boundRect.topLeft()).toPoint(),
                        mapToScene(boundRect.bottomRight()).toPoint());
}


void MinionGraphicsItem::setZonePos(bool friendly, int pos, int minionsZone)
{
    const int hMinion = HEIGHT-5;
    const int wMinion = WIDTH-5;
    int x = static_cast<int>(wMinion*(pos - (minionsZone-1)/2.0));
    int y = friendly?hMinion/2:-hMinion/2;
    this->setPos(x, y);
}


void MinionGraphicsItem::updateStatsFromCard(CardGraphicsItem * card)
{
    if(card->attack != card->origAttack)    this->attack = card->attack;
    else                                    this->attack += card->buffAttack;

    if(card->health != card->origHealth)    this->health = card->health;
    else                                    this->health += card->buffHealth;
}


bool MinionGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"MINION TAG CHANGE -->"<<id<<tag<<value;

    //Evita addons provocado por cambios despues de morir(en el log los minion vuelven a damage 0 y estado original justo antes de desaparecer de la zona)
    //Terror de fatalidad envia TO_BE_DESTROYED despues de hacer 2 de damage, para dar tiempo a invocar el demonio.
    //Despues de morir por TO_BE_DESTROYED, vuelve a 0.
    //Dark Speaker (minion 3/6 swap stats con otro minion) produce cambia el health a 0 antes del swap
    //lo que hace que el y su objetivo aparezcan muertos, por eso (this->health > 0)
    if((this->damage >= this->health && this->health > 0)
            || (!this->hero && (this->zone !="PLAY" || this->toBeDestroyed)))
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
    else if(tag == "TO_BE_DESTROYED" || tag == "SHOULDEXITCOMBAT")
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
    else if(tag == "RUSH")
    {
        this->rush = (value=="1");
        if(rush)    this->exausted = false;
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

    if(triggerMinion)   painter->setOpacity(0.6);

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
    QFont font(ThemeHandler::cardsFont());
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
    QString text = QString::number(attack);
    Utility::drawShadowText(*painter, font, text, -35, 46, true);

    if(damage>0)                painter->setBrush(RED);
    else if(health>origHealth)  painter->setBrush(GREEN);
    else                        painter->setBrush(WHITE);
    text = QString::number(health-std::max(0,damage));//Usamos damage negativo en future planning al establecer addons damage/life
    Utility::drawShadowText(*painter, font, text, 34, 46, true);

    //Shield
    if(this->shield)
    {
        painter->drawPixmap(-71, -92, QPixmap(":Images/bgMinionShield.png"));
    }

    //Dead
    if(this->dead)
    {
        painter->drawPixmap(-23, 25, QPixmap(":Images/bgMinionDead.png"));
    }

    //Addons
    for(int i=0; i<this->addons.count() && i<4; i++)
    {
        QString addonCode = this->addons[i].code;
        int moveX = 0, moveY = 0;
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
            text = QString::number(addons[i].number);
            Utility::drawShadowText(*painter, font, text, moveX, moveY, true);
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

        Utility::drawShadowText(*painter, font, symbol, -35, 20, true);
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

        Utility::drawShadowText(*painter, font, symbol, 34, 20, true);
    }

    //Dead Prob
    if(!FLOATEQ(deadProb, 0))
    {
        painter->drawPixmap(-87/2, -94/2, QPixmap(":Images/bgHeroDead.png"));

        //Numero
        painter->setBrush(RED);
        text = QString::number(static_cast<int>(round(deadProb*100))) + "%";
        Utility::drawShadowText(*painter, font, text, 0, 0, true);
    }
}
