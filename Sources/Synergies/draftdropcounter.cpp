#include "draftdropcounter.h"
#include "../themehandler.h"
#include <QtWidgets>

DraftDropCounter::DraftDropCounter(QObject *parent, QString synergyTag, QHBoxLayout *hLayout, int targetDrops,
                                   QPixmap pixmap, bool iconHover) :
    DraftItemCounter(parent, synergyTag, hLayout, pixmap, iconHover)
{
    //Constructor MainWindow
    init(targetDrops);
}


DraftDropCounter::DraftDropCounter(QObject *parent, QString synergyTag, QGridLayout *gridLayout, int gridRow, int gridCol, int targetDrops,
                                   QPixmap pixmap, int iconWidth, bool iconHover) :
    DraftItemCounter(parent, synergyTag, gridLayout, gridRow, gridCol, pixmap, iconWidth, iconHover)
{
    //Constructor DraftMechanicsWindow
    init(targetDrops);
}


void DraftDropCounter::init(int targetDrops)
{
    labelIcon->setDisabled(false);
    this->targetDrops = targetDrops;
    this->numCards = 0;
    drawBorder(isGreenTargetDrop());
}


void DraftDropCounter::setTheme(QPixmap pixmap, int iconWidth, bool inDraftMechanicsWindow)
{
    DraftItemCounter::setTheme(pixmap, iconWidth, inDraftMechanicsWindow);
    drawBorder(isGreenTargetDrop());
}


void DraftDropCounter::drawBorder(bool greenBorder)
{
    QPixmap pixmap(*labelIcon->pixmap());
    QRect targetAll(0, 0, pixmap.width(), pixmap.height());
    QPainter painter;

    painter.begin(&pixmap);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        if(greenBorder)     painter.drawPixmap(targetAll, QPixmap(ThemeHandler::greenMechanicFile()));
        else                painter.drawPixmap(targetAll, QPixmap(ThemeHandler::redMechanicFile()));
    painter.end();

    labelIcon->setPixmap(pixmap);
}


void DraftDropCounter::reset()
{
    DraftItemCounter::reset();
    labelIcon->setDisabled(false);
    this->numCards = 0;
}


MechanicBorderColor DraftDropCounter::getMechanicBorderColor()
{
    if(count() >= targetDrops)  return MechanicBorderRed;
    else if(isGreenTargetDrop())return MechanicBorderGrey;
    else                        return MechanicBorderGreen;
}


bool DraftDropCounter::isGreenTargetDrop()
{
    int currentTargetDrops = targetDrops * numCards / 30;
    if(numCards >= 30)  currentTargetDrops = targetDrops - 1;
    return count() >= currentTargetDrops;
}


void DraftDropCounter::increase(const QString &code)
{
    bool beforeGreenTargetDrop = isGreenTargetDrop();
    DraftItemCounter::increase(code);
    bool afterGreenTargetDrop = isGreenTargetDrop();
    if(beforeGreenTargetDrop != afterGreenTargetDrop)   drawBorder(afterGreenTargetDrop);
}


void DraftDropCounter::setNumCards(int numCards)
{
    bool beforeGreenTargetDrop = isGreenTargetDrop();
    this->numCards = numCards;
    bool afterGreenTargetDrop = isGreenTargetDrop();
    if(beforeGreenTargetDrop != afterGreenTargetDrop)   drawBorder(afterGreenTargetDrop);

}


void DraftDropCounter::increaseNumCards()
{
    bool beforeGreenTargetDrop = isGreenTargetDrop();
    this->numCards++;
    bool afterGreenTargetDrop = isGreenTargetDrop();
    if(beforeGreenTargetDrop != afterGreenTargetDrop)   drawBorder(afterGreenTargetDrop);
}


