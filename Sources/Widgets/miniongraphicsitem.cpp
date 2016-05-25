#include "miniongraphicsitem.h"
#include "../utility.h"
#include <QtWidgets>

MinionGraphicsItem::MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn)
{
    this->code = code;
    this->id = id;
    this->friendly = friendly;
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
    this->playerTurn = playerTurn;

    foreach(QJsonValue value, Utility::getCardAtribute(code, "mechanics").toArray())
    {
        processTagChange(value.toString(), "1");
    }
}


MinionGraphicsItem::MinionGraphicsItem(MinionGraphicsItem *copy)
{
    this->code = copy->code;
    this->id = copy->id;
    this->friendly = copy->friendly;
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
    this->playerTurn = copy->playerTurn;
    this->setPos(copy->pos());
}


int MinionGraphicsItem::getId()
{
    return this->id;
}


QString MinionGraphicsItem::getCode()
{
    return this->code;
}


bool MinionGraphicsItem::isFriendly()
{
    return this->friendly;
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


void MinionGraphicsItem::changeZone(bool playerTurn)
{
    this->friendly = !this->friendly;
    this->playerTurn = this->friendly && playerTurn;
    if(friendly && charge)      this->exausted = false;
    else                        this->exausted = true;
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


void MinionGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"TAG CHANGE -->"<<id<<tag<<value;
    if(tag == "DAMAGE")
    {
        this->damage = value.toInt();
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
    }
    update();
}


void MinionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    //Card background
    painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + code + ".png")));
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
        painter->drawPixmap(-87/2, -94/2, QPixmap(":Images/bgMinionDead.png"));
    }
}
