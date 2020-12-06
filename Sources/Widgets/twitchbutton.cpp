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
    setValue(0, 0, false);
}


void TwitchButton::setValue(float value, int votes, bool isBestScore, QString username)
{
    if(votes <= this->votes)  username = "";
    this->votes = votes;
    this->value = value;
    this->isBestScore = isBestScore;

    this->value_0_1 = (value - min)/(max - min);
    if(value_0_1 > 1) value_0_1 = 1;
    if(value_0_1 < 0) value_0_1 = 0;

    bool needDraw = ftList.isEmpty();

    if(!username.isEmpty())
    {
        FloatingText ft;
        ft.username = username;
        ft.birth = QDateTime::currentMSecsSinceEpoch();
        if(ftList.isEmpty())    ft.up = qrand()%2;
        else                    ft.up = !ftList.first().up;
        ftList.prepend(ft);
    }

    if(needDraw)    draw();
    else            update();
}


void TwitchButton::draw()
{
    if(!ftList.isEmpty())   QTimer::singleShot(FT_DRAW_STEP, this, SLOT(draw()));
    for(int i=0; i<ftList.count();)
    {
        qint64 ftLife = QDateTime::currentMSecsSinceEpoch() - ftList[i].birth;
        if(ftLife>FT_MAX_LIFE)  ftList.removeAt(i);
        else                    i++;
    }

    update();
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

    Utility::drawShadowText(painter, font, text, this->width()/2, this->height()/2, true, false);

    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedOpenFile()));
    if(isBestScore) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedTwitchTextFile()));

    //Floating text
    for(const FloatingText &ft: ftList)
    {
        qint64 ftLife = QDateTime::currentMSecsSinceEpoch() - ft.birth;
        int ftSize = static_cast<int>((width()/3.0)*(0+(ftLife/FT_SIZE)));
        font.setPixelSize(ftSize>0?ftSize:1);
        pen.setWidth(font.pixelSize()/20);
        painter.setPen(pen);
        painter.setOpacity(1-(ftLife/FT_OPACITY));
        int y = ft.up?(this->height()/2) * (1-(ftLife/FT_OFFSET)):(this->height()/2) * (1+(ftLife/FT_OFFSET));

        double offsetY = 0.25;
        #ifdef Q_OS_WIN
            offsetY += 0.05;
        #else
            offsetY += 0.1;//Necesitaba un reajuste, 0.15 se queda descuadrado
        #endif
        QFontMetrics fm(font);
        int textWide = fm.width(ft.username);
        int textHigh = fm.height();
        if(textWide>this->width())
        {
            QPainterPath path;
            path.addText(0, y + textHigh*offsetY, font, ft.username);
            painter.drawPath(path);
        }
        else
        {
            QPainterPath path;
            path.addText((this->width()/2) - (textWide/2), y + textHigh*offsetY, font, ft.username);
            painter.drawPath(path);
        }
    }
    painter.setOpacity(1.0);

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
