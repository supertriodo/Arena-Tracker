#include "deckcard.h"
#include "../utility.h"
#include "../themehandler.h"
#include "../Widgets/scorebutton.h"
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
    scoreHA = 0;
    scoreHSR = 0;
    includedDecks = 0;
    showScores = badScoreHA = badScoreHSR = false;
    showHA = showHSR = true;
    redraftingReview = false;
    classOrder = -1;
    this->outsider = outsider;
}


DeckCard::~DeckCard()
{

}


bool DeckCard::isOutsider()
{
    return this->outsider;
}


void DeckCard::setRedraftingReview(bool show)
{
    this->redraftingReview = show;
    draw();
}


void DeckCard::setBadScoreHA(bool badScore)
{
    this->badScoreHA = badScore;
}


void DeckCard::setBadScoreHSR(bool badScore)
{
    this->badScoreHSR = badScore;
}


void DeckCard::hideScores()
{
    setShowScores(false);
    setBadScoreHA(false);
    setBadScoreHSR(false);
    setRedraftingReview(false);
}


void DeckCard::setShowScores(bool showScores)
{
    this->showScores = showScores;
    draw();
}


void DeckCard::setShowHAShowHSRScores(bool showHA, bool showHSR)
{
    this->showHA = showHA;
    this->showHSR = showHSR;
    if(showScores)  draw();
}


void DeckCard::setScores(int haTier, float hsrWR, int classOrder, int includedDecks)
{
    this->scoreHA = haTier;
    this->scoreHSR = hsrWR;
    this->includedDecks = includedDecks;
    this->classOrder = classOrder;
    this->showScores = true;
    //No hace falta hacer draw() pq en DraftHandler::setDeckScores() llamamos a DeckCard::setShowHAShowHSRScores despues de DeckCard::setScores
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
        cardSchool = Utility::getSchoolFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = {INVALID_CLASS};
        cardRace = {INVALID_RACE};
        cardSchool = INVALID_SCHOOL;
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
        cardSchool = Utility::getSchoolFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = {INVALID_CLASS};
        cardRace = {INVALID_RACE};
        cardSchool = INVALID_SCHOOL;
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

        painter.fillRect(canvas.rect(), QBrush(QColor(0,0,0,DISABLE_OPACITY)));
    painter.end();
}


QPixmap DeckCard::draw(int total, bool drawRarity, QColor nameColor, QString manaText, int cardWidth, QStringList mechanics)
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

        //Mechanic icon
        if(mechanics.count() > 0)
        {
            painter.drawPixmap(1, 4, 28, 28, QPixmap(":/Images/" + mechanics[0]));

            if(mechanics.count() > 1)
            {
                //Mecanic damage tiene 64x64 en lugar de 62x62 como el resto
                int mecPx;
                if(mechanics[1] == "damageMechanic.png")    mecPx = 64;
                else                                        mecPx = 62;
                painter.drawPixmap(15, 4, 14, 28, QPixmap(":/Images/" + mechanics[1]), mecPx/2, 0, mecPx/2, mecPx);
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
        if(total > 1 || total < 0)
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
            //Utility::shrinkText(font, name, 15*scale, maxNameLong);//Equivale a lo de arriba

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


    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Scores
        if(showScores && (showHA || showHSR))
        {
            int height = canvas.height()*1.3;
            int width = canvas.width();

            painter.setBrush(Qt::NoBrush);

            if(badScoreHA && showHA && scoreHA!=0)
            {
                painter.setPen(QPen(Qt::darkYellow, 8));
                painter.drawRect(canvas.rect());

                if(badScoreHSR && showHSR && scoreHSR!=0)
                {
                    painter.setPen(QPen(Qt::darkRed, 8));
                    painter.drawLines(QVector<QLine>{
                        QLine(0, 0, canvas.width(), 0),
                        QLine(0, 0, 0, canvas.height()/2),
                        QLine(canvas.width(), 0, canvas.width(), canvas.height()/2)
                    });
                }
            }
            else if(badScoreHSR && showHSR && scoreHSR!=0)
            {
                painter.setPen(QPen(Qt::darkRed, 8));
                painter.drawRect(canvas.rect());
            }
            if(showHA && scoreHA!=0)
            {
                painter.drawPixmap(width - (29*width/218) - 6*height/8, 0,
                                   ScoreButton::scorePixmap(Score_HearthArena, scoreHA, height, classOrder));
            }
            if(showHSR && scoreHSR!=0)
            {
                painter.drawPixmap(width - (29*width/218) - (showHA?2*6*height/8:6*height/8), 0,
                                   ScoreButton::scorePixmap(Score_HSReplay, scoreHSR, height, classOrder, includedDecks));
            }
        }

        //Cards in review redraft
        if(redraftingReview)
        {
            painter.setPen(QPen(Qt::white, 4));
            painter.drawRect(canvas.rect());
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
        //Utility::shrinkText(font, name, 15*scale, ((total==1?194:175)*scale) - customTextWide);//Equivale a lo de arriba

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
    if(rarity == FREE)              return WHITE;
    else if(rarity == COMMON)       return WHITE;
    else if(rarity == RARE)         return BLUE;
    else if(rarity == EPIC)         return VIOLET;
    else if(rarity == LEGENDARY)    return ORANGE;
    else                            return BLACK;
}


bool DeckCard::isCode(const QString &code)
{
    return (code == this->code);
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


QList<CardRace> DeckCard::getRace()
{
    return cardRace;
}


CardSchool DeckCard::getSchool()
{
    return cardSchool;
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
