#include "deckcard.h"
#include "../utility.h"
#include "../themehandler.h"
#include <QtWidgets>


bool DeckCard::drawClassColor = false;
bool DeckCard::drawSpellWeaponColor = false;
int DeckCard::cardHeight = 35;


DeckCard::DeckCard(QString code, bool outsider)
{
    setCode(code);
    listItem = nullptr;
    total = remaining = 1;
    topManaLimit = bottomManaLimit = false;
    special = false;
    createdByCode = "";
    id = 0;
    this->outsider = outsider;
}


DeckCard::~DeckCard()
{

}


bool DeckCard::isOutsider()
{
    return this->outsider;
}


void DeckCard::setManaLimit(bool top)
{
    if(top)
    {
        if(!topManaLimit)
        {
            topManaLimit = true;
            draw();
        }
    }
    else
    {
        if(!bottomManaLimit)
        {
            bottomManaLimit = true;
            draw();
        }
    }
}


void DeckCard::resetManaLimits()
{
    bool redraw = topManaLimit || bottomManaLimit;
    topManaLimit = bottomManaLimit = false;
    if(redraw)  draw();
}


void DeckCard::setCode(QString code)
{
    this->code = code;

    if(!code.isEmpty())
    {
        cost = Utility::getCardAttribute(code, "cost").toInt();
        type = Utility::getTypeFromCode(code);
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = Utility::getRarityFromCode(code);
        cardClass = Utility::getClassFromCode(code);
        cardRace = Utility::getRaceFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = {INVALID_CLASS};
        cardRace = INVALID_RACE;
    }
}


//Reusamos cost/type/name/rarity para createdByCode
void DeckCard::setCreatedByCode(QString code)
{
    if(!this->code.isEmpty()) return;

    this->createdByCode = code;

    if(!createdByCode.isEmpty())
    {
        cost = Utility::getCardAttribute(code, "cost").toInt();
        type = Utility::getTypeFromCode(code);
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = Utility::getRarityFromCode(code);
        cardClass = Utility::getClassFromCode(code);
        cardRace = Utility::getRaceFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = {INVALID_CLASS};
        cardRace = INVALID_RACE;
    }
}


QString DeckCard::getCreatedByCode()
{
    return this->createdByCode;
}


void DeckCard::draw()
{
    QPixmap canvas;

    if(!this->createdByCode.isEmpty() && this->code.isEmpty())
    {
        canvas = drawCustomCard(this->createdByCode, "BY:");
    }
    else
    {
        if(remaining > 0)   canvas = draw(remaining, false, BLACK);
        else                canvas = draw(total, false, BLACK);
    }


    if(remaining == 0)  disablePixmap(canvas);
    this->listItem->setIcon(QIcon(canvas));
}


void DeckCard::disablePixmap(QPixmap &canvas)
{
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        painter.fillRect(canvas.rect(), QBrush(QColor(0,0,0,150)));
    painter.end();
}


QPixmap DeckCard::draw(int total, bool drawRarity, QColor nameColor, QString manaText, int cardWidth)
{
    //Scale
    float scale;
    int offsetY = 0;
    if(cardWidth == 0)
    {
        if(cardHeight <= 35)
        {
            scale = 1;
            offsetY = (35 - cardHeight)/2;
        }
        else    scale = cardHeight/35.0;
    }
    else
    {
        scale = cardWidth/218.0;
    }


    //Imagenes
    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Borders behind
        if(ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }

        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")                   source = QRectF(0,0,100,25);
        else if(type==MINION)                   source = QRectF(46,72,100,25);
        else                                    source = QRectF(46,98,100,25);
        if(total == 1 && rarity != LEGENDARY)   target = QRectF(113,6,100,25);
        else                                    target = QRectF(100,6,100,25);

        if(name == "unknown")   painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        else    painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + code + ".png"), source);

        //Background and legendary star
        int maxNameLong;
        if(total == 1 && rarity != LEGENDARY)
        {
            maxNameLong = 174*scale;
            if(cardClass.count() > 0)
                painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard1File(cardClass[0]):ThemeHandler::bgCard1File()));
            if(cardClass.count() > 1)
                painter.drawPixmap(0, CARD_SIZE.height()/2,
                                   QPixmap(drawClassColor?ThemeHandler::bgCard1File(cardClass[1]):ThemeHandler::bgCard1File()),
                                   0, CARD_SIZE.height()/2, CARD_SIZE.width(), CARD_SIZE.height() - CARD_SIZE.height()/2);
        }
        else
        {
            maxNameLong = 155*scale;

            if(cardClass.count() > 0)
                painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard2File(cardClass[0]):ThemeHandler::bgCard2File()));
            if(cardClass.count() > 1)
                painter.drawPixmap(0, CARD_SIZE.height()/2,
                                   QPixmap(drawClassColor?ThemeHandler::bgCard2File(cardClass[1]):ThemeHandler::bgCard2File()),
                                   0, CARD_SIZE.height()/2, CARD_SIZE.width(), CARD_SIZE.height() - CARD_SIZE.height()/2);
            //Legendary star
            if(total == 1 && rarity == LEGENDARY)
            {
                painter.drawPixmap(195, 8, QPixmap(ThemeHandler::starFile()));
            }
        }

        //Borders front
        if(!ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }
    painter.end();



    //Adapt to size
    if(cardWidth == 0)  canvas = resizeCardHeight(canvas);
    else                canvas = canvas.scaled(QSize(cardWidth,35),
                            cardWidth<218?Qt::KeepAspectRatio:Qt::KeepAspectRatioByExpanding,
                            Qt::SmoothTransformation);



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


        //Text pen/brush color
        painter.setPen(QPen(BLACK));
        if(nameColor!=BLACK)                            painter.setBrush(nameColor);
        else if(drawRarity)                             painter.setBrush(getRarityColor());
        else if(outsider)                               painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);

        //Number cards
        if(total > 1)
        {
            font.setPixelSize(22*scale);
            Utility::drawShadowText(painter, font, QString::number(total), 202*scale, (19*scale) - offsetY, true);
        }


        //Name and mana
        if(name == "unknown")
        {
            font.setPixelSize(15*scale);
            painter.setPen(QPen(BLACK));
            QColor themeColor1 = QColor(ThemeHandler::themeColor1());
            painter.setBrush(themeColor1.isValid()?themeColor1:WHITE);
            Utility::drawShadowText(painter, font, "Unknown", 34*scale, (20*scale) - offsetY, false);
        }
        else
        {
            //Name
            int fontSize = 15*scale;
            font.setPixelSize(fontSize);

            QFontMetrics fm(font);
            int textWide = fm.width(name);
            while(textWide>maxNameLong)
            {
                fontSize--;
                font.setPixelSize(fontSize);
                fm = QFontMetrics(font);
                textWide = fm.width(name);
            }

            Utility::drawShadowText(painter, font, name, 34*scale, (20*scale) - offsetY, false);

            //Mana cost
            if(manaText.isEmpty())
            {
                font.setPixelSize(22*scale);
                Utility::drawShadowText(painter, font, QString::number(cost), 13*scale, (20*scale) - offsetY, true);
            }
            //Custom mana number
            else
            {
                painter.setBrush(WHITE);
                font.setPixelSize(16*scale);
                Utility::drawShadowText(painter, font, manaText, 13*scale, (20*scale) - offsetY, true);
            }
        }
    painter.end();

    return canvas;
}


QPixmap DeckCard::drawCustomCard(QString customCode, QString customText)
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

        //Borders behind
        if(ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }

        //Card
        QRectF target;
        QRectF source;

        QFileInfo cardFI(Utility::hscardsPath() + "/" + customCode + ".png");
        if(cardFI.exists())
        {
            if(type==MINION)        source = QRectF(46,72,100,25);
            else                    source = QRectF(46,98,100,25);
            if(total == 1)          target = QRectF(113,6,100,25);
            else                    target = QRectF(100,6,100,25);
            painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + customCode + ".png"), source);
        }
        else
        {
            source = QRectF(63,18,100,25);
            target = QRectF(113,6,100,25);
            painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        }

        //Background
        if(total == 1)
        {
            painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardBYFile()));
        }
        else
        {
            painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardBYFile2()));
        }

        //Borders front
        if(!ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }
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

        //BY
        int fontSize = 15*scale;
        font.setPixelSize(fontSize);
        QFontMetrics fm(font);
        int customTextWide = fm.width(customText);
        painter.setBrush(BLACK);
        painter.setPen(QPen(WHITE));
        Utility::drawShadowText(painter, font, customText, 10*scale, (20*scale) - offsetY, false);

        //Name
        int nameWide = fm.width(name);
        int maxNameLong = ((total==1?194:175)*scale) - customTextWide;
        while(nameWide>maxNameLong)
        {
            fontSize--;
            font.setPixelSize(fontSize);
            fm = QFontMetrics(font);
            nameWide = fm.width(name);
        }

        painter.setPen(QPen(BLACK));
        if(outsider)                                    painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);

        Utility::drawShadowText(painter, font, name, (14*scale) + customTextWide, (20*scale) - offsetY, false);

        //#cards
        if(total > 1)
        {
            font.setPixelSize(22*scale);
            Utility::drawShadowText(painter, font, QString::number(total), 202*scale, (19*scale) - offsetY, true);
        }
    painter.end();

    return canvas;
}


QPixmap DeckCard::resizeCardHeight(QPixmap &canvas)
{
    if(cardHeight==35)  return canvas;

    if(cardHeight<25)
    {
        canvas = canvas.copy(0,0+6,218,35-10);
        return canvas.scaled(QSize(218,cardHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else if(cardHeight>35)
    {
        return canvas.scaled(QSize(218,cardHeight), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    else    //25<=cardHeight<35
    {
        int reducePx = 35 - cardHeight;//1-10
        int topPx = reducePx/2+1;
        return canvas.copy(0,0+topPx,218,35-reducePx);
    }
}


QColor DeckCard::getRarityColor()
{
    if(rarity == COMMON)            return WHITE;
    else if(rarity == RARE)         return BLUE;
    else if(rarity == EPIC)         return VIOLET;
    else if(rarity == LEGENDARY)    return ORANGE;
    else                            return BLACK;
}


QString DeckCard::getCode()
{
    return code;
}


CardType DeckCard::getType()
{
    return type;
}


QString DeckCard::getName()
{
    return name;
}


int DeckCard::getCost()
{
    return cost;
}

CardRarity DeckCard::getRarity()
{
    return rarity;
}


QList<CardClass> DeckCard::getCardClass()
{
    return cardClass;
}


CardRace DeckCard::getRace()
{
    return cardRace;
}


void DeckCard::setDrawClassColor(bool value)
{
    DeckCard::drawClassColor = value;
}


void DeckCard::setDrawSpellWeaponColor(bool value)
{
    DeckCard::drawSpellWeaponColor = value;
}


void DeckCard::setCardHeight(int value)
{
    DeckCard::cardHeight = value;
}


int DeckCard::getCardHeight()
{
    return DeckCard::cardHeight;
}


int DeckCard::getCardWidth()
{
    return (cardHeight<=35)?218:static_cast<int>(cardHeight/35.0*218);
}
