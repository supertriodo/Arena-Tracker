#include "lavabutton.h"
#include "../themehandler.h"
#include <QtWidgets>

LavaButton::LavaButton(QWidget *parent, double min, double max) : QLabel(parent)
{
    this->value = this->min = min;
    this->max = max;
    this->drawCards = this->toYourHandCards = this->discoverCards = 0;
}


void LavaButton::setValue(int totalMana, int numCards, int drawCards, int toYourHandCards, int discoverCards)
{
    this->drawCards += drawCards;
    this->toYourHandCards += toYourHandCards;
    this->discoverCards += discoverCards;

#ifdef QT_DEBUG
    qDebug() << QString("Mana: ") + QString::number(totalMana) +
                QString("- Cards: ") + QString::number(numCards) +
                QString("- Draw: ") + QString::number(this->drawCards) + "(" + QString::number(drawCards) + ")" +
                QString("- ToYourHand: ") + QString::number(this->toYourHandCards) + "(" + QString::number(toYourHandCards) + ")" +
                QString("- Discover: ") + QString::number(this->discoverCards) + "(" + QString::number(discoverCards) + ")";
#endif

    if(numCards == 0)    return;
    value = (
                totalMana +
                this->drawCards * (totalMana/(float)numCards) +
                this->toYourHandCards * 4 +
                this->discoverCards * 4
             )/(float)numCards;

    this->value_0_1 = (value - min)/(max - min);
    if(value_0_1 > 1) value_0_1 = 1;
    if(value_0_1 < 0) value_0_1 = 0;
    draw();
}


void LavaButton::draw()
{
    this->update();
}


void LavaButton::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QRect targetAll(0, 0, width(), height());
    painter.drawPixmap(targetAll, QPixmap(":Images/speedClose.png"));

    QRegion r(QRect(width()*0.15, height()*0.15, width()*0.7, height()*0.7), QRegion::Ellipse);
    painter.setClipRegion(r);
    painter.setClipping(true);
        QRect targetLava(0, (1-value_0_1)*(height()*80/128), width(), height());
        painter.drawPixmap(targetLava, QPixmap(":Images/speedLava.png"));
    painter.setClipping(false);

    QFont font(LG_FONT);
    font.setPixelSize(width()/2.7);

    QPen pen(BLACK);
    pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    QString text = QString::number(int(value*10));
    QFontMetrics fm = QFontMetrics(font);
    int textWide = fm.width(text);
    int textHigh = fm.height();

    QPainterPath path;
#ifdef Q_OS_WIN
    path.addText(this->width()/2 - textWide/2, this->height()/2 + textHigh*0.3, font, text);
#else
    path.addText(this->width()/2 - textWide/2, this->height()/2 + textHigh*0.4, font, text);
#endif
    painter.drawPath(path);

    painter.drawPixmap(targetAll, QPixmap(":Images/speedOpen.png"));
}

