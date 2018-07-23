#include "twitchbutton.h"
#include "../themehandler.h"
#include <QtWidgets>

TwitchButton::TwitchButton(QWidget *parent, float min, float max) : QLabel(parent)
{
    this->value = this->min = min;
    this->max = max;
    this->votes = 0;
}


void TwitchButton::reset()
{
    this->votes = 0;
    this->value = 0;
    update();
}


void TwitchButton::setValue(float value, int votes, bool isBestScore)
{
    this->votes = votes;
    this->value = value;
    this->isBestScore = isBestScore;

    this->value_0_1 = (value - min)/(max - min);
    if(value_0_1 > 1) value_0_1 = 1;
    if(value_0_1 < 0) value_0_1 = 0;
    draw();
}


void TwitchButton::draw()
{
    this->update();
}


void TwitchButton::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPixmap canvas(width(), height());
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QRect targetAll(0, 0, width(), height());
    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedCloseFile()));

    QRegion r(QRect(static_cast<int>(width()*0.15), static_cast<int>(height()*0.15),
                    static_cast<int>(width()*0.7), static_cast<int>(height()*0.7)), QRegion::Ellipse);
    painter.setClipRegion(r);
    painter.setClipping(true);
        QRect targetLava(0, static_cast<int>((1-value_0_1)*(height()*80/128)), width(), height());
        painter.drawPixmap(targetLava, QPixmap(ThemeHandler::speedLavaFile()));
    painter.setClipping(false);

    QFont font(LG_FONT);
    if(votes > 999)         font.setPixelSize(static_cast<int>(width()/3.75));
    else                    font.setPixelSize(static_cast<int>(width()/3.0));

    QPen pen(BLACK);
    pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    QString text;
    if(votes > 9999)    text = QString::number(int(votes/1000)) + 'K';
    else                text = QString::number(int(votes));
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

    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedOpenFile()));
    if(isBestScore) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedTwitchTextFile()));

    QPainter painterObject(this);
    if(isEnabled())
    {
        painterObject.drawPixmap(targetAll, canvas);
    }
    else
    {
        QIcon icon(canvas);
        painterObject.drawPixmap(targetAll, icon.pixmap(width(), height(), QIcon::Disabled, QIcon::On));
    }
}

