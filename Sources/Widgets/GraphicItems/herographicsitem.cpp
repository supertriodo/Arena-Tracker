#include "herographicsitem.h"
#include "../../utility.h"
#include <QtWidgets>

HeroGraphicsItem::HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender)
    :MinionGraphicsItem(code, id, friendly, playerTurn, graphicsItemSender)
{
    this->armor = 0;
    this->exausted = false;
    this->hero = true;
    this->minionsAttack = this->minionsMaxAttack = 0;
    this->resources = 1;
    this->resourcesUsed = 0;
    this->spellDamage = 0;
    this->showAllInfo = true;
    this->heroWeapon = NULL;

    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    int x = 0;
    int y = friendly?hMinion + HEIGHT/2:-hMinion - HEIGHT/2;
    this->setPos(x, y);
    this->setZValue(-20);
}


HeroGraphicsItem::HeroGraphicsItem(HeroGraphicsItem *copy)
    :MinionGraphicsItem(copy)
{
    this->armor = copy->armor;
    this->minionsAttack = copy->minionsAttack;
    this->minionsMaxAttack = copy->minionsMaxAttack;
    this->resources = copy->resources;
    this->resourcesUsed = copy->resourcesUsed;
    this->spellDamage = copy->spellDamage;
    this->showAllInfo = false;
    this->heroWeapon = NULL; //No lo necesitamos en la copia, solo en nowBoard

    foreach(SecretIcon secretIcon, copy->secretsList)
    {
        this->secretsList.append(secretIcon);
    }
}


void HeroGraphicsItem::changeHero(QString code, int id)
{
    this->code = code;
    this->id = id;
    this->attack = this->origAttack = 0;
    this->health = this->origHealth = Utility::getCardAtribute(code, "health").toInt();
    this->damage = 0;
    this->armor = 0;
    update();
}


void HeroGraphicsItem::setShowAllInfo(bool value)
{
    this->showAllInfo = value;
}


void HeroGraphicsItem::setMinionsAttack(int minionsAttack)
{
    this->minionsAttack = minionsAttack;
    update();
    sendHeroTotalAttackChange();
}


void HeroGraphicsItem::setMinionsMaxAttack(int minionsMaxAttack)
{
    this->minionsMaxAttack = minionsMaxAttack;
    update();
    sendHeroTotalAttackChange();
}


void HeroGraphicsItem::setHeroWeapon(WeaponGraphicsItem *heroWeapon)
{
    this->heroWeapon = heroWeapon;
    update();
    sendHeroTotalAttackChange();
}


void HeroGraphicsItem::setSpellDamage(int spellDamage)
{
    this->spellDamage = spellDamage;
}


void HeroGraphicsItem::addResourcesUsed(int value)
{
    this->resourcesUsed += value;
    update();
}


void HeroGraphicsItem::setResourcesUsed(int resourcesUsed)
{
    this->resourcesUsed = resourcesUsed;
    update();
}


int HeroGraphicsItem::getAvailableResources()
{
    return resources - resourcesUsed;
}


void HeroGraphicsItem::setResources(int resources)
{
    this->resources = resources;
    update();
}


int HeroGraphicsItem::getSpellDamage()
{
    return this->spellDamage;
}


int HeroGraphicsItem::getResources()
{
    return this->resources;
}


QRectF HeroGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2 - 30, -HEIGHT/2 - 13, WIDTH + 30 + 30, HEIGHT + 13);
}


void HeroGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
}


void HeroGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    graphicsItemSender->sendPlanCardLeave();
}


void HeroGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF posMouse = event->pos();
    for(int i=std::min(3, addons.count()-1); i>=0; i--)
    {
        QString addonCode = this->addons[i].code;
        QPoint posAddon;
        switch(i)
        {
            case 0:
                posAddon = QPoint(0, 40);
                break;
            case 3:
                posAddon = QPoint(0, -40);
                break;
            case 1:
                posAddon = QPoint(-30, 0);
                break;
            case 2:
                posAddon = QPoint(30, 0);
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

    for(int i=std::min(4, secretsList.count()-1); i>=0; i--)
    {
        QString secretCode = this->secretsList[i].code;
        QPoint posSecret;

        switch(i)
        {
            case 0:
                posSecret = QPoint(0, -75);
                break;
            case 1:
                posSecret = QPoint(-40, -55);
                break;
            case 2:
                posSecret = QPoint(40, -55);
                break;
            case 3:
                posSecret = QPoint(-60, -20);
                break;
            case 4:
                posSecret = QPoint(60, -20);
                break;
        }

        if(QLineF(posMouse, posSecret).length() < 16)
        {
            QRectF boundRect = boundingRect();
            graphicsItemSender->sendPlanCardEntered(secretCode,
                                mapToScene(boundRect.topLeft()).toPoint(),
                                mapToScene(boundRect.bottomRight()).toPoint());
            return;
        }
    }

    graphicsItemSender->sendPlanCardLeave();
}


void HeroGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    graphicsItemSender->heroPress(this, event->button());
}


bool HeroGraphicsItem::isYourSecret(int id)
{
    foreach(SecretIcon secretIcon, secretsList)
    {
        if(secretIcon.id == id)     return true;
    }
    return false;
}


CardClass HeroGraphicsItem::getSecretHero(int id)
{
    foreach(SecretIcon secretIcon, secretsList)
    {
        if(secretIcon.id == id)     return secretIcon.secretHero;
    }
    return INVALID_CLASS;
}


void HeroGraphicsItem::addSecret(int id, CardClass secretHero)
{
    SecretIcon secretIcon;
    secretIcon.id = id;
    secretIcon.secretHero = secretHero;
    secretIcon.code = "";
    secretsList.append(secretIcon);
    update();
}


void HeroGraphicsItem::removeSecret(int id)
{
    for(int i=0; i<secretsList.count(); i++)
    {
        if(secretsList[i].id == id)
        {
            secretsList.removeAt(i);
            update();
            return;
        }
    }
}


void HeroGraphicsItem::showSecret(int id, QString code)
{
    for(int i=0; i<secretsList.count(); i++)
    {
        if(secretsList[i].id == id)
        {
            secretsList[i].code = code;
            update();
            return;
        }
    }
}


bool HeroGraphicsItem::processTagChange(QString tag, QString value)
{
    bool healing = false;
    if(tag == "ARMOR")
    {
        int newArmor = value.toInt();
        if(newArmor > this->armor)  healing = true;
        this->armor = newArmor;
    }
    else
    {
        bool ret = MinionGraphicsItem::processTagChange(tag, value);
        if(tag == "ATK" || tag == "EXHAUSTED" || tag == "FROZEN" || tag == "WINDFURY")  sendHeroTotalAttackChange();
        return ret;
    }
    update();
    return healing;
}


void HeroGraphicsItem::setPlayerTurn(bool playerTurn)
{
    MinionGraphicsItem::setPlayerTurn(playerTurn);
    sendHeroTotalAttackChange();
}


void HeroGraphicsItem::sendHeroTotalAttackChange()
{
    int totalAttack = this->minionsAttack;
    int totalMaxAttack = this->minionsMaxAttack;
    bool glow = (!exausted && !frozen && (playerTurn==friendly) && attack>0);
    if(windfury)
    {
        if(glow)                                totalAttack += attack*2;
        if(attack == 0 && heroWeapon != NULL)   totalMaxAttack += heroWeapon->getAttack()*2;
        else                                    totalMaxAttack += attack*2;
    }
    else
    {
        if(glow)                                totalAttack += attack;
        if(attack == 0 && heroWeapon != NULL)   totalMaxAttack += heroWeapon->getAttack();
        else                                    totalMaxAttack += attack;
    }

    if(playerTurn!=friendly)    totalAttack = totalMaxAttack;
    graphicsItemSender->sendHeroTotalAttackChange(friendly, totalAttack, totalMaxAttack);
}


void HeroGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(code.startsWith("HERO_"))
    {
        QRectF target = QRectF( -80, -92, 160, 184);
        QRectF source(34, 112, 240, 276);
        QPixmap pixmap(Utility::hscardsPath() + "/" + code + ".png");
        painter->drawPixmap(target, pixmap, source);
    }


    //Glow
    bool glow = (!exausted && !frozen && (playerTurn==friendly) && attack>0);
    if(glow)
    {
        painter->drawPixmap(-84, -92, QPixmap(":Images/bgHeroGlow.png"));
    }

    //Frozen
    if(this->frozen)    painter->drawPixmap(-104, -104, QPixmap(":Images/bgHeroFrozen.png"));


    //Health
    painter->drawPixmap(36, -6, QPixmap(":Images/bgHeroLife.png"));

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

    if(damage>0)                painter->setBrush(RED);
    else                        painter->setBrush(WHITE);
    QFontMetrics fm(font);
    QString text = QString::number(health-damage);
    int textWide = fm.width(text);
    int textHigh = fm.height();
    QPainterPath path;
    path.addText(70 - textWide/2, 55 + textHigh/4, font, text);
    painter->drawPath(path);

    //Attack
    if(attack > 0)
    {
        painter->drawPixmap(-114, -4, QPixmap(":Images/bgHeroAttack.png"));

        painter->setBrush(WHITE);
        text = QString::number(attack);
        textWide = fm.width(text);
        path = QPainterPath();
        path.addText(-66 - textWide/2, 55 + textHigh/4, font, text);
        painter->drawPath(path);
    }

    //Armor
    if(armor > 0)
    {
        painter->drawPixmap(41, -36, QPixmap(":Images/bgHeroArmor.png"));

        painter->setBrush(WHITE);
        text = QString::number(armor);
        textWide = fm.width(text);
        path = QPainterPath();
        path.addText(70 - textWide/2, textHigh/4, font, text);
        painter->drawPath(path);
    }

    //Dead
    if(this->dead)
    {
        painter->drawPixmap(-87/2, -94/2, QPixmap(":Images/bgHeroDead.png"));
    }

    //Secrets
    for(int i=0; i<this->secretsList.count() && i<5; i++)
    {
        int moveX, moveY;
        switch(i)
        {
            case 0:
                moveX = 0;
                moveY = -75;
                break;
            case 1:
                moveX = -40;
                moveY = -55;
                break;
            case 2:
                moveX = 40;
                moveY = -55;
                break;
            case 3:
                moveX = -60;
                moveY = -20;
                break;
            case 4:
                moveX = 60;
                moveY = -20;
                break;
        }

        QPixmap pixmap;
        CardClass secretHero = this->secretsList[i].secretHero;
        switch(secretHero)
        {
            case PALADIN:
                pixmap = QPixmap(":Images/secretPaladin.png");
                break;
            case HUNTER:
                pixmap = QPixmap(":Images/secretHunter.png");
                break;
            case MAGE:
                pixmap = QPixmap(":Images/secretMage.png");
                break;
            default:
                qDebug()<<"SECRET HERO UNKNOWN";
                break;
        }

        painter->drawPixmap(moveX-20, moveY-20, 40, 40, pixmap);

        QString secretCode = secretsList[i].code;
        if(!secretCode.isEmpty())
        {
            painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + secretCode + ".png")));
            painter->setBrushOrigin(QPointF(100+moveX,202+moveY));
            painter->drawEllipse(QPointF(moveX,moveY), 16, 16);
        }
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
                moveY = 40;
                break;
            case 3:
                moveX = 0;
                moveY = -40;
                break;
            case 1:
                moveX = -30;
                moveY = 0;
                break;
            case 2:
                moveX = 30;
                moveY = 0;
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

    //Dead Prob
    if(deadProb != 0)
    {
        painter->drawPixmap(-87/2, -94/2, QPixmap(":Images/bgHeroDead.png"));

        //Numero
        painter->setBrush(RED);
        text = QString::number((int)round(deadProb*100)) + "%";
        textWide = fm.width(text);
        path = QPainterPath();
        path.addText(-textWide/2, textHigh/4, font, text);
        painter->drawPath(path);
    }

    if(playerTurn == friendly || showAllInfo)
    {
        //Mana
        painter->drawPixmap(WIDTH/2-50, -HEIGHT/2+13, QPixmap(":Images/bgCrystal.png"));

        if(playerTurn != friendly)      text = QString::number(resources);
        else if(resourcesUsed == 0)     text = QString::number(resources);
        else                            text = QString::number(resources-resourcesUsed) + "/" + QString::number(resources);
        textWide = fm.width(text);
        path = QPainterPath();
        path.addText(WIDTH/2-17 - textWide/2, -HEIGHT/2+46 + textHigh/4, font, text);
        painter->setBrush(WHITE);
        painter->drawPath(path);


        //Total attack
        painter->drawPixmap(-WIDTH/2-30, -HEIGHT/2-13, QPixmap(":Images/bgTotalAttack.png"));
        int totalAttack = this->minionsAttack;
        int totalMaxAttack = this->minionsMaxAttack;
        if(windfury)
        {
            if(glow)                                totalAttack += attack*2;
            if(attack == 0 && heroWeapon != NULL)   totalMaxAttack += heroWeapon->getAttack()*2;
            else                                    totalMaxAttack += attack*2;
        }
        else
        {
            if(glow)                                totalAttack += attack;
            if(attack == 0 && heroWeapon != NULL)   totalMaxAttack += heroWeapon->getAttack();
            else                                    totalMaxAttack += attack;
        }
        if(playerTurn != friendly)              text = QString::number(totalMaxAttack);
        else if(totalAttack == totalMaxAttack)  text = QString::number(totalAttack);
        else                                    text = QString::number(totalAttack) + "/" + QString::number(totalMaxAttack);
        textWide = fm.width(text);
        path = QPainterPath();
        path.addText(-WIDTH/2+18 - textWide/2, -HEIGHT/2+46 + textHigh/4, font, text);
        painter->setBrush(WHITE);
        painter->drawPath(path);
    }
}


