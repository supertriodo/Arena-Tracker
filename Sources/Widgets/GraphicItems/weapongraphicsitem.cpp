#include "weapongraphicsitem.h"
#include "herographicsitem.h"
#include "../../utility.h"
#include <QtWidgets>

WeaponGraphicsItem::WeaponGraphicsItem(QString code, int id, bool friendly, GraphicsItemSender *graphicsItemSender)
    :MinionGraphicsItem(code, id, friendly, false, graphicsItemSender)
{
    this->durability = this->origDurability =
            this->health = this->origHealth =
            Utility::getCardAtribute(code, "durability").toInt();

    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    const int wHero = HeroGraphicsItem::WIDTH;
    int x = -wHero/2 - WIDTH/2 + 20;
    int y = friendly?hMinion + hHero/2 + 15:-hMinion - hHero/2 + 15;
    this->setPos(x, y);
    this->setZValue(-40);
}


WeaponGraphicsItem::WeaponGraphicsItem(WeaponGraphicsItem *copy)
    :MinionGraphicsItem(copy)
{
    this->durability = copy->durability;
    this->origDurability = copy->origDurability;
}


QRectF WeaponGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


bool WeaponGraphicsItem::processTagChange(QString tag, QString value)
{
    bool healing = false;
    if(tag == "DURABILITY")
    {
        int newDurability = value.toInt();
        if(newDurability > this->durability)  healing = true;
        this->durability = this->health = newDurability;
    }
    else
    {
        return MinionGraphicsItem::processTagChange(tag, value);
    }
    update();
    return healing;
}


void WeaponGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    //Card background
    painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + this->code + ".png")));
    painter->setBrushOrigin(QPointF(99,193));
    painter->drawEllipse(QPointF(0,0), 56, 56);

    //Marco
    painter->drawPixmap(-WIDTH/2, -HEIGHT/2, QPixmap(":/Images/bgWeaponSimple.png"));

    //Attack/Durability
    QFont font(HS_FONT);
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
    QString text = QString::number(attack);
    int textWide = fm.width(text);
    int textHigh = fm.height();
    QPainterPath path;
    path.addText(-49 - textWide/2, 37 + textHigh/4, font, text);
    painter->drawPath(path);

    if(damage>0)                painter->setBrush(RED);
    else if(durability>origDurability)  painter->setBrush(GREEN);
    else                        painter->setBrush(WHITE);
    text = QString::number(durability-damage);
    textWide = fm.width(text);
    path = QPainterPath();
    path.addText(47 - textWide/2, 37 + textHigh/4, font, text);
    painter->drawPath(path);

    //Dead
    if(this->dead)
    {
        painter->drawPixmap(-23, 17, QPixmap(":Images/bgMinionDead.png"));
    }
}
