#include "synergycard.h"
#include "../themehandler.h"
#include <QtWidgets>

int SynergyCard::synergyWidth = 218;


SynergyCard::SynergyCard(QString code) : DeckCard(code)
{
    this->synergyTag = "";
}


SynergyCard::~SynergyCard()
{

}


void SynergyCard::setSynergyTag(QString synergyTag)
{
    this->synergyTag = synergyTag;
}
QString SynergyCard::getSynergyTag()
{
    return this->synergyTag;
}


void SynergyCard::draw(bool useSynergyWidth)
{
    QPixmap canvas = DeckCard::draw(total, false, BLACK, "", useSynergyWidth?synergyWidth:0);
    if(remaining == 0)          disablePixmap(canvas);
    if(!synergyTag.isEmpty())   drawSynergyTag(canvas, useSynergyWidth);

    this->listItem->setIcon(QIcon(canvas));
}


void SynergyCard::draw(QLabel *label)
{
    QPixmap canvas = DeckCard::draw(total, false, BLACK, "", synergyWidth);
    if(remaining == 0)          disablePixmap(canvas);
    if(!synergyTag.isEmpty())   drawSynergyTag(canvas);

    label->setPixmap(canvas);
}


void SynergyCard::drawSynergyTag(QPixmap &canvas, bool useSynergyWidth)
{
    //Scale
    float scale;
    if(useSynergyWidth)         scale = synergyWidth/218.0;
    else if(cardHeight <= 35)   scale = 1;
    else                        scale = cardHeight/35.0;

    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Synergy Tag
        QFont font(ThemeHandler::cardsFont());
        font.setBold(true);
        font.setKerning(true);
#ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif
        font.setPixelSize(12*scale);
        QFontMetrics fm(font);
        int textWide = fm.width(synergyTag);

        QColor synergyTagColor = QColor(ThemeHandler::synergyTagColor());
        painter.setPen(QPen(BLACK));
        painter.setBrush(synergyTagColor.isValid()?synergyTagColor:BLACK);
        painter.drawRoundRect(20*scale, 0, (10*scale)+textWide, 15*scale, 15, 80);

        painter.setPen(QPen(BLACK));
        painter.setBrush(WHITE);
        Utility::drawShadowText(painter, font, synergyTag, 25*scale, 8*scale, false);
    painter.end();
}


void SynergyCard::setRaceSchoolTag()
{
    if(type == MINION)      setRaceTag();
    else if(type == SPELL)  setSchoolTag();
}


void SynergyCard::setRaceTag()
{
    QString synergyTagJoin = "";

    if(cardRace.contains(ALL))
    {
        synergyTagJoin.append("All");
    }
    if(cardRace.contains(MURLOC))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Murloc");
        else                            synergyTagJoin.append(" - Murloc");
    }
    if(cardRace.contains(DEMON))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Demon");
        else                            synergyTagJoin.append(" - Demon");
    }
    if(cardRace.contains(MECHANICAL))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Mech");
        else                            synergyTagJoin.append(" - Mech");
    }
    if(cardRace.contains(ELEMENTAL))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Elemental");
        else                            synergyTagJoin.append(" - Elemental");
    }
    if(cardRace.contains(BEAST))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Beast");
        else                            synergyTagJoin.append(" - Beast");
    }
    if(cardRace.contains(TOTEM))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Totem");
        else                            synergyTagJoin.append(" - Totem");
    }
    if(cardRace.contains(PIRATE))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Pirate");
        else                            synergyTagJoin.append(" - Pirate");
    }
    if(cardRace.contains(DRAGON))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Dragon");
        else                            synergyTagJoin.append(" - Dragon");
    }
    if(cardRace.contains(NAGA))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Naga");
        else                            synergyTagJoin.append(" - Naga");
    }
    if(cardRace.contains(UNDEAD))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Undead");
        else                            synergyTagJoin.append(" - Undead");
    }
    if(cardRace.contains(QUILBOAR))
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Quilboar");
        else                            synergyTagJoin.append(" - Quilboar");
    }

    setSynergyTag(synergyTagJoin);
}


void SynergyCard::setSchoolTag()
{
    QString synergyTagJoin = "";

    if(cardSchool == ARCANE)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Arcane");
        else                            synergyTagJoin.append(" - Arcane");
    }
    if(cardSchool == FEL)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Fel");
        else                            synergyTagJoin.append(" - Fel");
    }
    if(cardSchool == FIRE)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Fire");
        else                            synergyTagJoin.append(" - Fire");
    }
    if(cardSchool == FROST)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Frost");
        else                            synergyTagJoin.append(" - Frost");
    }
    if(cardSchool == HOLY)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Holy");
        else                            synergyTagJoin.append(" - Holy");
    }
    if(cardSchool == SHADOW)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Shadow");
        else                            synergyTagJoin.append(" - Shadow");
    }
    if(cardSchool == NATURE)
    {
        if(synergyTagJoin.isEmpty())    synergyTagJoin.append("Nature");
        else                            synergyTagJoin.append(" - Nature");
    }

    setSynergyTag(synergyTagJoin);
}


void SynergyCard::setSynergyWidth(int value)
{
    SynergyCard::synergyWidth = value;
}
