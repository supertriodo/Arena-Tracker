#include "lavabutton.h"
#include "../themehandler.h"
#include <QtWidgets>

LavaButton::LavaButton(QWidget *parent) : QLabel(parent)
{
    this->value = 0;
}


void LavaButton::setValue(double value)
{
    this->value = value;
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

    QRect target(0, 0, width(), height());
    painter.drawPixmap(target, QPixmap(":Images/speedOpen.png"));
}












