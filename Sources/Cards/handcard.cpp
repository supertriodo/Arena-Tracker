#include "handcard.h"
#include <QtWidgets>


HandCard::HandCard(QString code) : DeckCard(code)
{
    turn = 0;
}


HandCard::~HandCard()
{

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
    QFont font("Belwe Bd BT");
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

        QFontMetrics fm(font);
        int textWide = fm.width("T"+QString::number((this->turn+1)/2));
        int textHigh = fm.height();

        painter.setFont(font);
        painter.setBrush(WHITE);
        painter.setPen(QPen(BLACK));

        QPainterPath path;
        path.addText(172 - textWide/2, 20 + textHigh/4, font, "T"+QString::number((this->turn+1)/2));
        painter.drawPath(path);
    painter.end();

    this->listItem->setIcon(QIcon(resizeCardHeight(canvas)));
}
