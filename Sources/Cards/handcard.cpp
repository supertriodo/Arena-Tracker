#include "handcard.h"
#include "../themehandler.h"
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
    QFont font(ThemeHandler::cardsFont());
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
        painter.setFont(font);
        painter.setBrush(WHITE);
        painter.setPen(QPen(BLACK));
        Utility::drawShadowText(painter, font, text, 172, 20, true);

        //Buff
        if(buffAttack > 0 || buffHealth > 0)
        {
            font.setPixelSize(20);
            text = "+" + QString::number(buffAttack) + "/+" + QString::number(buffHealth);
            painter.setFont(font);
            painter.setBrush(BLACK);
            painter.setPen(QPen(GREEN));
            Utility::drawShadowText(painter, font, text, 42, 19, true);
        }
    painter.end();

    this->listItem->setIcon(QIcon(resizeCardHeight(canvas)));
}
