#include "heropowergraphicsitem.h"
#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>

HeroPowerGraphicsItem::HeroPowerGraphicsItem(QString code, int id, bool friendly, bool playerTurn)
{
    this->code = code;
    this->id = id;
    this->exausted = false;
    this->friendly = friendly;
    this->playerTurn = playerTurn;

    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    const int wHero = HeroGraphicsItem::WIDTH;
    int x = wHero/2 + WIDTH/2 - 20;
    int y = friendly?hMinion + hHero/2 + 20:-hMinion - hHero/2 + 15;
    this->setPos(x, y);
    this->setZValue(-1);
}


HeroPowerGraphicsItem::HeroPowerGraphicsItem(HeroPowerGraphicsItem *copy)
{
    this->code = copy->code;
    this->id = copy->id;
    this->exausted = copy->exausted;
    this->friendly = copy->friendly;
    this->playerTurn = copy->playerTurn;
    this->setPos(copy->pos());
    this->setZValue(copy->zValue());
}


void HeroPowerGraphicsItem::changeHeroPower(QString code, int id)
{
    this->code = code;
    this->id = id;
    this->exausted = false;
    update();
}


int HeroPowerGraphicsItem::getId()
{
    return this->id;
}


void HeroPowerGraphicsItem::setPlayerTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    update();
}


void HeroPowerGraphicsItem::checkDownloadedCode(QString code)
{
    if(this->code == code)  update();
}


QRectF HeroPowerGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


void HeroPowerGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"TAG CHANGE -->"<<id<<tag<<value;

    if(tag == "EXHAUSTED")
    {
        this->exausted = (value=="1");
        update();
    }
}


void HeroPowerGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(playerTurn == friendly)
    {
        //Card background
        painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + this->code + ".png")));
        painter->setBrushOrigin(QPointF(101,209));
        painter->drawEllipse(QPointF(0,0), 40, 40);

        //Glow
        if(exausted)
        {
            painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPowerGlow.png"));
        }

        //Marco
        painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPower.png"));
    }
    else    painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgHeroPowerClosed.png"));
}
