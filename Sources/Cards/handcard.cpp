#include "handcard.h"
#include <QtWidgets>


HandCard::HandCard(QString code) : DeckCard(code)
{
    turn = 0;
    buffAttack = buffHealth = 0;
    linkIdsList.clear();
}


HandCard::~HandCard()
{

}


void HandCard::addBuff(int addAttack, int addHealth)
{
    buffAttack += addAttack;
    buffHealth += addHealth;
    draw();
}


void HandCard::draw()
{
    if(!this->code.isEmpty() || !this->createdByCode.isEmpty())
    {
        DeckCard::draw();
    }
    else
    {
        drawDefaultHandCard();
    }
}


void HandCard::drawDefaultHandCard()
{
    QFont font(HS_FONT);
    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Background
        painter.drawPixmap(0,0,QPixmap(this->special?":Images/handCard2.png":":Images/handCard1.png"));

        //Turn
        font.setPixelSize(25);//18
        font.setBold(true);
        font.setKerning(true);
#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

        QString text = "T" + QString::number((this->turn+1)/2);
        QFontMetrics fm(font);
        int textWide = fm.width(text);
        int textHigh = fm.height();

        painter.setFont(font);
        painter.setBrush(WHITE);
        painter.setPen(QPen(BLACK));

        QPainterPath path;
        path.addText(172 - textWide/2, 20 + textHigh/4, font, text);
        painter.drawPath(path);

        //Buff
        if(buffAttack > 0 || buffHealth > 0)
        {
            font.setPixelSize(20);
            text = "+" + QString::number(buffAttack) + "/+" + QString::number(buffHealth);
            fm = QFontMetrics(font);
            textWide = fm.width(text);
            textHigh = fm.height();
            painter.setFont(font);
            painter.setBrush(BLACK);
            painter.setPen(QPen(GREEN));

            path = QPainterPath();
            path.addText(42 - textWide/2, 19 + textHigh/4, font, text);
            painter.drawPath(path);
        }
    painter.end();

    this->listItem->setIcon(QIcon(resizeCardHeight(canvas)));
}
