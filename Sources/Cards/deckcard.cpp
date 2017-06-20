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
        cost = Utility::getCardAttribute(code, "cost").toInt();
        type = getTypeFromString(Utility::getCardAttribute(code, "type").toString());
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = getRarityFromString(Utility::getCardAttribute(code, "rarity").toString());
        cardClass = getClassFromString(Utility::getCardAttribute(code, "playerClass").toString());
        cardRace = getRaceFromString(Utility::getCardAttribute(code, "race").toString());
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
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
        type = getTypeFromString(Utility::getCardAttribute(code, "type").toString());
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = getRarityFromString(Utility::getCardAttribute(code, "rarity").toString());
        cardClass = getClassFromString(Utility::getCardAttribute(code, "playerClass").toString());
        cardRace = getRaceFromString(Utility::getCardAttribute(code, "race").toString());
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
        cardRace = INVALID_RACE;
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


CardRace DeckCard::getRaceFromString(QString value)
{
    if(value == "BLOODELF")         return BLOODELF;
    else if(value == "DRAENEI")     return DRAENEI;
    else if(value == "DWARF")       return DWARF;
    else if(value == "GNOME")       return GNOME;
    else if(value == "GOBLIN")      return GOBLIN;
    else if(value == "HUMAN")       return HUMAN;
    else if(value == "NIGHTELF")    return NIGHTELF;
    else if(value == "ORC")         return ORC;
    else if(value == "TAUREN")      return TAUREN;
    else if(value == "TROLL")       return TROLL;
    else if(value == "UNDEAD")      return UNDEAD;
    else if(value == "WORGEN")      return WORGEN;
    else if(value == "GOBLIN2")     return GOBLIN2;
    else if(value == "MURLOC")      return MURLOC;
    else if(value == "DEMON")       return DEMON;
    else if(value == "SCOURGE")     return SCOURGE;
    else if(value == "MECHANICAL")  return MECHANICAL;
    else if(value == "ELEMENTAL")   return ELEMENTAL;
    else if(value == "OGRE")        return OGRE;
    else if(value == "BEAST")       return BEAST;
    else if(value == "TOTEM")       return TOTEM;
    else if(value == "NERUBIAN")    return NERUBIAN;
    else if(value == "PIRATE")      return PIRATE;
    else if(value == "DRAGON")      return DRAGON;
    else                            return INVALID_RACE;
}


CardClass DeckCard::getClassFromString(QString value)
{
    if(value == "")             return NEUTRAL;
    else if(value == "NEUTRAL") return NEUTRAL;
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

    if(!this->createdByCode.isEmpty() && this->code.isEmpty())
    {
        canvas = drawCustomCard(this->createdByCode, "BY:");
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
    QFont font(ThemeHandler::cardsFont());
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")                   source = QRectF(0,0,100,25);
        else if(type==MINION)                   source = QRectF(48,72,100,25);
        else                                    source = QRectF(48,98,100,25);
        if(total == 1 && rarity != LEGENDARY)   target = QRectF(113,6,100,25);
        else                                    target = QRectF(100,6,100,25);

        if(name == "unknown")   painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        else    painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + code + ".png"), source);

        //Background and #cards
        painter.setPen(QPen(BLACK));

        if(nameColor!=BLACK)                            painter.setBrush(nameColor);
        else if(drawRarity)                             painter.setBrush(getRarityColor());
        else if(outsider)                               painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);


        int maxNameLong;
        if(total == 1 && rarity != LEGENDARY)
        {
            maxNameLong = 174;
            painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard1File(cardClass):ThemeHandler::bgCard1File()));
        }
        else
        {
            maxNameLong = 155;
            painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard2File(cardClass):ThemeHandler::bgCard2File()));

            if(total > 1)
            {
                font.setPixelSize(22);//16pt
                Utility::drawShadowText(painter, font, QString::number(total), 202, 19, true);
            }
            else
            {
                painter.drawPixmap(195, 8, QPixmap(ThemeHandler::starFile()));
            }
        }


        //Name and mana
        if(name == "unknown")
        {
            font.setPixelSize(14);//10pt
            painter.setPen(QPen(BLACK));
            painter.setBrush(QColor(ThemeHandler::themeColor1()));
            Utility::drawShadowText(painter, font, "Unknown", 34, 20, false);
        }
        else
        {
            //Name
            int fontSize = 15;
            font.setPixelSize(fontSize);//11pt

            QFontMetrics fm(font);
            int textWide = fm.width(name);
            while(textWide>maxNameLong)
            {
                fontSize--;
                font.setPixelSize(fontSize);//<11pt
                fm = QFontMetrics(font);
                textWide = fm.width(name);
            }

            Utility::drawShadowText(painter, font, name, 34, 20, false);

            //Mana cost
            int manaSize = cost>9?26:18+1.5*cost;
            font.setPixelSize(manaSize);//20pt | 14 + cost
            Utility::drawShadowText(painter, font, QString::number(cost), 13, 20, true);
        }

        //Borders
        QPixmap pixmap(ThemeHandler::manaLimitFile());
        int pixmapHMid = pixmap.height()/2;
        int pixmapW = pixmap.width();
        if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
        if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
    painter.end();

    //Adapt to size
    if(resize)  return resizeCardHeight(canvas);
    else        return canvas;
}


QPixmap DeckCard::drawCustomCard(QString customCode, QString customText)
{
    QFont font(ThemeHandler::cardsFont());
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

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

        QFileInfo cardFI(Utility::hscardsPath() + "/" + customCode + ".png");
        if(cardFI.exists())
        {
            if(type==MINION)        source = QRectF(48,72,100,25);
            else                    source = QRectF(48,98,100,25);
            painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + customCode + ".png"), source);
        }
        else
        {
            source = QRectF(63,18,100,25);
            painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        }

        //Background
        painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardBYFile()));

        //BY
        int fontSize = 15;
        font.setPixelSize(fontSize);//11pt
        QFontMetrics fm(font);
        int customTextWide = fm.width(customText);
        painter.setBrush(BLACK);
        painter.setPen(QPen(WHITE));
        Utility::drawShadowText(painter, font, customText, 10, 20, false);


        //Name
        int nameWide = fm.width(name);
        int maxNameLong = 194 - customTextWide;
        while(nameWide>maxNameLong)
        {
            fontSize--;
            font.setPixelSize(fontSize);//<11pt
            fm = QFontMetrics(font);
            nameWide = fm.width(name);
        }

        painter.setPen(QPen(BLACK));

        if(outsider)                                    painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);

        Utility::drawShadowText(painter, font, name, 14 + customTextWide, 20, false);
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


QString DeckCard::getHeroLogNumber()
{
    return Utility::heroToLogNumber(cardClass);
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
    return (cardHeight<=35)?218:cardHeight/35.0*218;
}
