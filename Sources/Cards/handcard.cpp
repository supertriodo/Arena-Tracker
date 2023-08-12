#include "handcard.h"
#include "../themehandler.h"
#include <QtWidgets>


HandCard::HandCard(QString code) : DeckCard(code)
{
    turn = 0;
    buffAttack = buffHealth = 0;
    forged = false;
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


void HandCard::forge()
{
    forged = true;
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
    //Scale
    float scale;
    int offsetY = 0;
    if(cardHeight <= 35)
    {
        scale = 1;
        offsetY = (35 - cardHeight)/2;
    }
    else    scale = cardHeight/35.0;


    //Imagenes
    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Background
        painter.drawPixmap(0,0,QPixmap(this->special?ThemeHandler::handCardBYUnknownFile():ThemeHandler::handCardFile()));
    painter.end();



    //Adapt to size
    canvas = resizeCardHeight(canvas);



    //Texto
    QFont font(ThemeHandler::cardsFont());
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Turn
        font.setPixelSize(25*scale);
        QString text = "T" + QString::number((this->turn+1)/2);
        painter.setBrush(WHITE);
        painter.setPen(QPen(BLACK));
        Utility::drawShadowText(painter, font, text, 172*scale, (20*scale) - offsetY, true);

        //Buff/Forged
        if(buffAttack > 0 || buffHealth > 0 || forged)
        {
            if(forged)  text = "Forged";
            else        text = "+" + QString::number(buffAttack) + "/+" + QString::number(buffHealth);
            font.setPixelSize(14*scale);
            Utility::drawTagText(painter, font, text, 20, 8, 5, 8, scale);
        }
    painter.end();

    this->listItem->setIcon(QIcon(canvas));
}
