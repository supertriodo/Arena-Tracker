#include "lavabutton.h"
#include "../themehandler.h"
#include "Sources/constants.h"
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QDesktopServices>

LavaButton::LavaButton(QWidget *parent, float min, float max) : QLabel(parent)
{
    this->value = this->min = min;
    this->value_0_1 = 0;
    this->max = max;
}


void LavaButton::reset()
{
    this->value = 0;
    update();
}


void LavaButton::mousePressEvent(QMouseEvent *event)
{
    if(this->toolTip() == "Deck weight")
    {
        QDesktopServices::openUrl(QUrl(
            "https://triodo.gitbook.io/arena-tracker-documentation/en/drafting-tab#deck-weight-only-patreon"
            ));
    }

    QLabel::mousePressEvent(event);
}


void LavaButton::setValue(float manaAvg)
{
    float totalWeightMana = 0;
    int numCards = 0;

    for(SynergyWeightCard &synergyCard: synergyWeightCardList)
    {
        totalWeightMana += synergyCard.processWeightMana(manaAvg) * synergyCard.total;
        numCards += synergyCard.total;
    }

    this->value = totalWeightMana / std::max(1, numCards);
    this->value_0_1 = (value - min)/(max - min);
    if(value_0_1 > 1) value_0_1 = 1;
    if(value_0_1 < 0) value_0_1 = 0;
    draw();
}


void LavaButton::increase(SynergyWeightCard &synergyWeightCard)
{
    const QString &code = synergyWeightCard.getCode();
    bool duplicatedCard = false;
    for(SynergyWeightCard &synergyCard: synergyWeightCardList)
    {
        if(synergyCard.getCode() == code)
        {
            synergyCard.total++;
            synergyCard.remaining = synergyCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        synergyWeightCardList.append(synergyWeightCard);
    }
}
void LavaButton::increase(const QString &code, int draw, int toYourHand, int discover)
{
    bool duplicatedCard = false;
    for(SynergyWeightCard &synergyCard: synergyWeightCardList)
    {
        if(synergyCard.getCode() == code)
        {
            synergyCard.total++;
            synergyCard.remaining = synergyCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        synergyWeightCardList.append(SynergyWeightCard(code, draw, toYourHand, discover));
    }
}


QList<SynergyWeightCard> LavaButton::getsynergyWeightCardListDupped()
{
    QList<SynergyWeightCard> duppedList;
    for(const SynergyWeightCard &synergyCard: synergyWeightCardList)
    {
        for(int i=0; i<synergyCard.total; i++)
        {
            duppedList << synergyCard;
        }
    }
    return duppedList;
}


void LavaButton::draw()
{
    this->update();
}


void LavaButton::paintEvent(QPaintEvent *event)
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
    font.setPixelSize(static_cast<int>(width()/3.0));

    QPen pen(BLACK);
    pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    QString text = QString::number(int(value)) + "." + QString::number(int(value*10)%10);
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
    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::speedDWTextFile()));

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

