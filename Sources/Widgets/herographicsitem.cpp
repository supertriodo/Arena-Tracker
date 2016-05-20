#include "herographicsitem.h"
#include "../utility.h"
#include <QtWidgets>

HeroGraphicsItem::HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn)
    :MinionGraphicsItem(code, id, friendly, playerTurn)
{
    this->armor = 0;
    this->exausted = false;

    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    int x = 0;
    int y = friendly?hMinion + HEIGHT/2:-hMinion - HEIGHT/2;
    this->setPos(x, y);
}


QRectF HeroGraphicsItem::boundingRect() const
{
    return QRectF( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
}


void HeroGraphicsItem::processTagChange(QString tag, QString value)
{
    if(tag == "ARMOR")
    {
        this->armor = value.toInt();
    }
    MinionGraphicsItem::processTagChange(tag, value);
}


void HeroGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QRectF target = QRectF( -80, -92, 160, 184);
    QRectF source(34, 112, 240, 276);
    QPixmap pixmap(Utility::hscardsPath() + "/" + code + ".png");
    painter->drawPixmap(target, pixmap, source);


    //Glow
    bool glow = (!exausted && !frozen && playerTurn && attack>0);
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
    int textWide = fm.width(QString::number(health-damage));
    int textHigh = fm.height();
    QPainterPath path;
    path.addText(70 - textWide/2, 55 + textHigh/4, font, QString::number(health-damage));
    painter->drawPath(path);

    //Attack
    if(attack > 0)
    {
        painter->drawPixmap(-114, -4, QPixmap(":Images/bgHeroAttack.png"));

        painter->setBrush(WHITE);
        textWide = fm.width(QString::number(attack));
        path = QPainterPath();
        path.addText(-66 - textWide/2, 55 + textHigh/4, font, QString::number(attack));
        painter->drawPath(path);
    }

    //Armor
    if(armor > 0)
    {
        painter->drawPixmap(41, -36, QPixmap(":Images/bgHeroArmor.png"));

        painter->setBrush(WHITE);
        textWide = fm.width(QString::number(armor));
        path = QPainterPath();
        path.addText(70 - textWide/2, textHigh/4, font, QString::number(armor));
        painter->drawPath(path);
    }
}


