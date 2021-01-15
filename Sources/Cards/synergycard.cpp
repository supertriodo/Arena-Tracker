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


void SynergyCard::draw()
{
    QPixmap canvas = DeckCard::draw(total, false, BLACK, false);
    if(!synergyTag.isEmpty())   drawSynergyTag(canvas);

    canvas = canvas.scaled(QSize(synergyWidth,35),
                           synergyWidth<218?Qt::KeepAspectRatio:Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
    this->listItem->setIcon(QIcon(canvas));
}


void SynergyCard::drawSynergyTag(QPixmap &canvas)
{
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
        font.setPixelSize(12);
        QFontMetrics fm(font);
        int textWide = fm.width(synergyTag);

        QColor synergyTagColor = QColor(ThemeHandler::synergyTagColor());
        painter.setPen(QPen(BLACK));
        painter.setBrush(synergyTagColor.isValid()?synergyTagColor:BLACK);
        painter.drawRoundRect(20, 0, 10+textWide, 15, 15, 80);

        painter.setPen(QPen(BLACK));
        painter.setBrush(WHITE);
        Utility::drawShadowText(painter, font, synergyTag, 25, 8, false);
    painter.end();
}


void SynergyCard::setSynergyWidth(int value)
{
    SynergyCard::synergyWidth = value;
}
