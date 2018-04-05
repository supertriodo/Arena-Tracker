#include "lavabutton.h"
#include "../themehandler.h"
#include <QtWidgets>

LavaButton::LavaButton(QWidget *parent, double min, double max) : QLabel(parent)
{
    this->value = this->min = min;
    this->max = max;
}


void LavaButton::setValue(double value)
{
    if(value > max) value = max;
    if(value < min) value = min;
    this->value = value;
    this->value_0_1 = (value - min)/(max - min);
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












