#include "handcard.h"
#include <QtWidgets>


HandCard::HandCard(QString code) : DeckCard(code)
{
    id = turn = 0;
    special = false;
}


HandCard::~HandCard()
{

}


//Reusamos cost/type/name/rarity para createdByCode
void HandCard::setCreatedByCode(QString code)
{
    this->createdByCode = code;

    if(!this->code.isEmpty()) return;

    if(!createdByCode.isEmpty())
    {
        cost = (*cardsJson)[code].value("cost").toInt();
        type = (*cardsJson)[code].value("type").toString();
        name = (*cardsJson)[code].value("name").toString();
        rarity = (*cardsJson)[code].value("rarity").toString();
    }
    else
    {
        cost = -1;
        type = "Minion";
        name = "unknown";
        rarity = "";
    }
}


QString HandCard::getCreatedByCode()
{
    return this->createdByCode;
}


void HandCard::draw()
{
    if(!this->code.isEmpty())
    {
        DeckCard::draw();
    }
    else if(!this->createdByCode.isEmpty())
    {
        drawCreatedByHandCard();
    }
    else
    {
        drawDefaultHandCard();
    }
}


void HandCard::drawCreatedByHandCard()
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

        //Card
        QRectF target = QRectF(113,6,100,25);;
        QRectF source;
        if(type==QString("Minion"))         source = QRectF(48,72,100,25);
        else                                source = QRectF(48,98,100,25);
        painter.drawPixmap(target, QPixmap(Utility::appPath() + "/HSCards/" + createdByCode + ".png"), source);

        //Background
        painter.drawPixmap(0,0,QPixmap(":Images/handCard3.png"));

        //BY
        int fontSize = 11;
        font.setPointSize(fontSize);
        font.setBold(true);
        font.setKerning(true);
#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

        QFontMetrics fm(font);
        int textWide = fm.width("BY:");
        int textHigh = fm.height();

        painter.setFont(font);
        painter.setBrush(BLACK);
        painter.setPen(QPen(WHITE));

        QPainterPath path;
        path.addText(20 - textWide/2, 20 + textHigh/4, font, "BY:");
        painter.drawPath(path);


        //Name
        textWide = fm.width(name);
        int maxNameLong = 174;
        while(textWide>maxNameLong)
        {
            fontSize--;
            font.setPointSize(fontSize);
            fm = QFontMetrics(font);
            textWide = fm.width(name);
            textHigh = fm.height();
        }

        painter.setFont(font);
        painter.setPen(QPen(BLACK));
        if(type==QString("Minion"))         painter.setBrush(WHITE);
        else if (type==QString("Spell"))    painter.setBrush(YELLOW);
        else                                painter.setBrush(ORANGE);

        path = QPainterPath();
        path.addText(34, 20 + textHigh/4, font, name);
        painter.drawPath(path);
    painter.end();

    this->listItem->setIcon(QIcon(canvas));
    this->listItem->setToolTip("<html><img src=./HSCards/" + this->createdByCode + ".png/></html>");
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
        int fontSize = 18;
        font.setPointSize(fontSize);
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

    this->listItem->setIcon(QIcon(canvas));
}
