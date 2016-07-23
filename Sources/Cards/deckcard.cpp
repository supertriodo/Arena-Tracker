#include "deckcard.h"
#include "../utility.h"
#include <QtWidgets>


bool DeckCard::drawClassColor = false;
bool DeckCard::drawSpellWeaponColor = false;
int DeckCard::cardHeight = 35;
QMap<QString, QJsonObject> * DeckCard::cardsJson;


DeckCard::DeckCard(QString code, bool outsider)
{
    setCode(code);
    listItem = NULL;
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
        cost = (*cardsJson)[code].value("cost").toInt();
        type = getTypeFromString((*cardsJson)[code].value("type").toString());
        name = (*cardsJson)[code].value("name").toString();
        rarity = getRarityFromString((*cardsJson)[code].value("rarity").toString());
        cardClass = getClassFromString((*cardsJson)[code].value("playerClass").toString());
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
    }
}


//Reusamos cost/type/name/rarity para createdByCode
void DeckCard::setCreatedByCode(QString code)
{
    if(!this->code.isEmpty()) return;

    this->createdByCode = code;

    if(!createdByCode.isEmpty())
    {
        cost = (*cardsJson)[code].value("cost").toInt();
        type = getTypeFromString((*cardsJson)[code].value("type").toString());
        name = (*cardsJson)[code].value("name").toString();
        rarity = getRarityFromString((*cardsJson)[code].value("rarity").toString());
        cardClass = getClassFromString((*cardsJson)[code].value("playerClass").toString());
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
    }
}


QString DeckCard::getCreatedByCode()
{
    return this->createdByCode;
}


CardRarity DeckCard::getRarityFromString(QString value)
{
    if(value == "FREE")             return COMMON;
    else if(value == "COMMON")      return COMMON;
    else if(value == "RARE")        return RARE;
    else if(value == "EPIC")        return EPIC;
    else if(value == "LEGENDARY")   return LEGENDARY;
    else                            return INVALID_RARITY;
}


CardType DeckCard::getTypeFromString(QString value)
{
    if(value == "MINION")           return MINION;
    else if(value == "SPELL")       return SPELL;
    else if(value == "WEAPON")      return WEAPON;
    else if(value == "ENCHANTMENT") return ENCHANTMENT;
    else if(value == "HERO")        return HERO;
    else if(value == "HERO_POWER")  return HERO_POWER;
    else                            return INVALID_TYPE;
}


CardClass DeckCard::getClassFromString(QString value)
{
    if(value == "")             return NEUTRAL;
    else if(value == "DRUID")   return DRUID;
    else if(value == "HUNTER")  return HUNTER;
    else if(value == "MAGE")    return MAGE;
    else if(value == "PALADIN") return PALADIN;
    else if(value == "PRIEST")  return PRIEST;
    else if(value == "ROGUE")   return ROGUE;
    else if(value == "SHAMAN")  return SHAMAN;
    else if(value == "WARLOCK") return WARLOCK;
    else if(value == "WARRIOR") return WARRIOR;
    else if(value == "DREAM")   return DREAM;
    else                        return INVALID_CLASS;
}


void DeckCard::draw()
{
    QPixmap canvas;

    if(!this->createdByCode.isEmpty())
    {
        canvas = drawCreatedByCard();
    }
    else
    {
        if(remaining > 0)   canvas = draw(remaining, false, BLACK);
        else                canvas = draw(total, false, BLACK);
    }


    if(remaining > 0)       this->listItem->setIcon(QIcon(canvas));
    else                    this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(
                                    canvas.size(), QIcon::Disabled, QIcon::On)));
}


QPixmap DeckCard::draw(uint total, bool drawRarity, QColor nameColor, bool resize)
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

        //Borders
        painter.setPen(GREEN);
        painter.setBrush(BLACK);
        if(topManaLimit)        painter.drawRect(QRect(1, -1, 8, 6));
        if(bottomManaLimit)     painter.drawRect(QRect(1, 30, 8, 6));

        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")                   source = QRectF(0,0,100,25);
        else if(type==MINION)                   source = QRectF(48,72,100,25);
        else                                    source = QRectF(48,98,100,25);
        if(total == 1 && rarity != LEGENDARY)   target = QRectF(113,6,100,25);
        else                                    target = QRectF(100,6,100,25);

        if(name == "unknown")   painter.drawPixmap(target, QPixmap(":Images/unknown.png"), source);
        else    painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + code + ".png"), source);

        //Background and #cards
        if(nameColor!=BLACK)                            painter.setPen(QPen(nameColor));
        else if(drawRarity)                             painter.setPen(QPen(getRarityColor()));
        else if(outsider)                               painter.setPen(QPen(VIOLET));
        else if(drawSpellWeaponColor && type==SPELL)    painter.setPen(QPen(YELLOW));
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setPen(QPen(ORANGE));
        else                                            painter.setPen(QPen(WHITE));

        int maxNameLong;
        if(total == 1 && rarity != LEGENDARY)
        {
            maxNameLong = 174;
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard1" + (drawClassColor?Utility::getHeroName(cardClass):QString("")) + ".png"));
        }
        else
        {
            maxNameLong = 155;
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard2" + (drawClassColor?Utility::getHeroName(cardClass):QString("")) + ".png"));

            if(total > 1)
            {
                font.setPixelSize(22);//16pt
                painter.setFont(font);
                painter.drawText(QRectF(190,6,26,24), Qt::AlignCenter, QString::number(total));
            }
            else
            {
                font.setPixelSize(28);//20pt
                painter.setFont(font);
                painter.drawText(QRectF(190,9,26,24), Qt::AlignCenter, "*");
            }
        }


        //Name and mana
        if(name == "unknown")
        {
            font.setPixelSize(14);//10pt
            painter.setFont(font);
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(34,7,154,23), Qt::AlignVCenter, "Unknown");
        }
        else
        {
            //Name
            int fontSize = 15;
            font.setPixelSize(fontSize);//11pt
            font.setBold(true);
            font.setKerning(true);

#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

            QFontMetrics fm(font);
            int textWide = fm.width(name);
            int textHigh = fm.height();
            while(textWide>maxNameLong)
            {
                fontSize--;
                font.setPixelSize(fontSize);//<11pt
                fm = QFontMetrics(font);
                textWide = fm.width(name);
                textHigh = fm.height();
            }

            painter.setFont(font);
            painter.setBrush(painter.pen().color());
            painter.setPen(QPen(BLACK));

            QPainterPath path;
            path.addText(34, 20 + textHigh/4, font, name);
            painter.drawPath(path);


            //Mana cost
            int manaSize = cost>9?26:18+1.5*cost;
            font.setPixelSize(manaSize);//20pt | 14 + cost
            painter.setFont(font);

            fm = QFontMetrics(font);
            textWide = fm.width(QString::number(cost));
            textHigh = fm.height();

            path = QPainterPath();
            path.addText(13 - textWide/2, 20 + textHigh/4, font, QString::number(cost));
            painter.drawPath(path);
        }
    painter.end();

    //Adapt to size
    if(resize)  return resizeCardHeight(canvas);
    else        return canvas;
}


QPixmap DeckCard::drawCreatedByCard()
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

        QFileInfo cardFI(Utility::hscardsPath() + "/" + createdByCode + ".png");
        if(cardFI.exists())
        {
            if(type==MINION)        source = QRectF(48,72,100,25);
            else                    source = QRectF(48,98,100,25);
            painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + createdByCode + ".png"), source);
        }
        else
        {
            source = QRectF(63,18,100,25);
            painter.drawPixmap(target, QPixmap(":Images/unknown.png"), source);
        }

        //Background
        painter.drawPixmap(0,0,QPixmap(":Images/handCard3.png"));

        //BY
        int fontSize = 15;
        font.setPixelSize(fontSize);//11pt
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
            font.setPixelSize(fontSize);//<11pt
            fm = QFontMetrics(font);
            textWide = fm.width(name);
            textHigh = fm.height();
        }

        painter.setFont(font);
        painter.setPen(QPen(BLACK));

        if(outsider)                                    painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);

        path = QPainterPath();
        path.addText(34, 20 + textHigh/4, font, name);
        painter.drawPath(path);
    painter.end();

    return resizeCardHeight(canvas);
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


CardClass DeckCard::getCardClass()
{
    return cardClass;
}


void DeckCard::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    DeckCard::cardsJson = cardsJson;
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
