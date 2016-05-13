#include "miniongraphicsitem.h"
#include "../utility.h"
#include <QtWidgets>

MinionGraphicsItem::MinionGraphicsItem(QString code)
{
    this->code = code;
    this->attack = Utility::getCardAtribute(code, "attack").toInt();
    this->health = Utility::getCardAtribute(code, "health").toInt();
}


QRectF MinionGraphicsItem::boundingRect() const
{
    //185x200 Minion Framework
    return QRectF( -140/2, -151/2, 140, 151);
}


void MinionGraphicsItem::setZonePos(bool friendly, int pos, int minionsZone)
{
    int x = 140*(pos - (minionsZone-1)/2.0);
    int y = friendly?150/2:-150/2;
    this->setPos(x, y);
}


void MinionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    //Card background
    painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + code + ".png")));
    painter->setBrushOrigin(QPointF(200-100,303-112));
    painter->drawEllipse(QPointF(0,0), 50, 68);


    //Minion template
    painter->drawPixmap(-140/2, -151/2, QPixmap(":Images/bgMinionSimple.png"));


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
    painter->setBrush(WHITE);
    QPen pen(BLACK);
    pen.setWidth(2);
    painter->setPen(pen);

    QFontMetrics fm(font);
    int textWide = fm.width(QString::number(attack));
    int textHigh = fm.height();

    QPainterPath path;
    path.addText(-37 - textWide/2, 40 + textHigh/4, font, QString::number(attack));
    painter->drawPath(path);

    textWide = fm.width(QString::number(health));
    path = QPainterPath();
    path.addText(32 - textWide/2, 40 + textHigh/4, font, QString::number(health));
    painter->drawPath(path);
}
