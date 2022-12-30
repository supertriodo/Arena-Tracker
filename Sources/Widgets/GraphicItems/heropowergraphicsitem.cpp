#include "heropowergraphicsitem.h"
#include "herographicsitem.h"
#include "../../utility.h"
#include <QtWidgets>

HeroPowerGraphicsItem::HeroPowerGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender)
{
    this->code = code;
    this->id = id;
    this->exausted = false;
    this->friendly = friendly;
    this->graphicsItemSender = graphicsItemSender;
    this->playerTurn = playerTurn;
    this->cost = Utility::getCardAttribute(code, "cost").toInt(2);

    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    const int wHero = HeroGraphicsItem::WIDTH;
    int x = wHero/2 + WIDTH/2 - 20;
    int y = friendly?hMinion + hHero/2 + 20:-hMinion - hHero/2 + 20;
    this->showTransparent = false;
    this->setPos(x, y);
    this->setZValue(-15);
}


HeroPowerGraphicsItem::HeroPowerGraphicsItem(HeroPowerGraphicsItem *copy)
{
    this->code = copy->code;
    this->id = copy->id;
    this->exausted = copy->exausted;
    this->friendly = copy->friendly;
    this->showTransparent = false;
    this->graphicsItemSender = copy->graphicsItemSender;
    this->playerTurn = copy->playerTurn;
    this->cost = copy->cost;
    this->setPos(copy->pos());
    this->setZValue(copy->zValue());
}


void HeroPowerGraphicsItem::changeHeroPower(QString code, int id)
{
    this->code = code;
    this->id = id;
    this->exausted = false;
    this->cost = Utility::getCardAttribute(code, "cost").toInt(2);
    update();
}


int HeroPowerGraphicsItem::getId()
{
    return this->id;
}


QString HeroPowerGraphicsItem::getCode()
{
    return this->code;
}


int HeroPowerGraphicsItem::getCost()
{
    return this->cost;
}


void HeroPowerGraphicsItem::setPlayerTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    update();
}


void HeroPowerGraphicsItem::toggleExausted()
{
    this->exausted = !this->exausted;
    if(exausted)    this->showTransparent = false;
    update();
}


bool HeroPowerGraphicsItem::isExausted()
{
    return this->exausted;
}


void HeroPowerGraphicsItem::showManaPlayable(int mana)
{
    if(cost > mana)     this->showTransparent = true;
    else                this->showTransparent = false;
    update();
}


void HeroPowerGraphicsItem::setShowTransparent(bool value)
{
    if(showTransparent != value)
    {
        showTransparent = value;
        update();
    }
}


bool HeroPowerGraphicsItem::isTransparent()
{
    return showTransparent;
}


void HeroPowerGraphicsItem::checkDownloadedCode(QString code)
{
    if(this->code == code)  update();
}


QRectF HeroPowerGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


void HeroPowerGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    graphicsItemSender->heroPowerPress(this, event->button());
}


void HeroPowerGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"TAG CHANGE -->"<<id<<tag<<value;

    if(tag == "EXHAUSTED")
    {
        this->exausted = (value=="1");
        if(exausted)    this->showTransparent = false;
        update();
    }
}


void HeroPowerGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(showTransparent) painter->setOpacity(0.2);

    if(playerTurn == friendly)
    {
        //Card background
        painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + this->code + ".png")));
        painter->setBrushOrigin(QPointF(101,210));//99+2,204+6
        painter->drawEllipse(QPointF(2,6), 40, 40);

        //Glow
        if(exausted)
        {
            painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPowerGlow.png"));
        }

        //Marco
        if(cost == 1)   painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPower1.png"));
        else            painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPower.png"));
    }
    else    painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPowerClosed.png"));
}
