#include "draftdropcounter.h"
#include "../themehandler.h"
#include <QtWidgets>

DraftDropCounter ** DraftDropCounter::dropCounters;
QMap<QString, QList<QString>> * DraftDropCounter::synergyCodes;

DraftDropCounter::DraftDropCounter(QObject *parent, QString synergyTag, QString synergyTagExtra,
                                   QGridLayout *gridLayout, int gridRow, int gridCol, int targetDrops,
                                   QPixmap pixmap, int iconWidth, bool iconHover, bool inDraftMechanicsWindow) :
    DraftItemCounter(parent, synergyTag, synergyTagExtra, gridLayout, gridRow, gridCol, pixmap,
                     iconWidth, iconHover, inDraftMechanicsWindow)
{
    //Constructor DraftMechanicsWindow/MainWindow
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
    drawBorder(isGreenTargetDrop());
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


DraftDropCounter ** DraftDropCounter::createDropCounters(QObject *parent, QGridLayout *mechanicsLayout)
{
    dropCounters = new DraftDropCounter *[V_NUM_DROPS];
    dropCounters[V_DROP2] = new DraftDropCounter(parent, "2 Drop", "2 Cost", mechanicsLayout, 1, 0, TARGET_DROP_2,
                                                 QPixmap(ThemeHandler::drop2CounterFile()), 32, true, false);
    dropCounters[V_DROP3] = new DraftDropCounter(parent, "3 Drop", "3 Cost", mechanicsLayout, 1, 1, TARGET_DROP_3,
                                                 QPixmap(ThemeHandler::drop3CounterFile()), 32, true, false);
    dropCounters[V_DROP4] = new DraftDropCounter(parent, "4 Drop", "4 Cost", mechanicsLayout, 1, 2, TARGET_DROP_4,
                                                 QPixmap(ThemeHandler::drop4CounterFile()), 32, true, false);

    return dropCounters;
}


void DraftDropCounter::deleteDropCounters()
{
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        delete dropCounters[i];
    }
    delete []dropCounters;
}


void DraftDropCounter::setTheme()
{
    dropCounters[V_DROP2]->setTheme(QPixmap(ThemeHandler::drop2CounterFile()), 32, false);
    dropCounters[V_DROP3]->setTheme(QPixmap(ThemeHandler::drop3CounterFile()), 32, false);
    dropCounters[V_DROP4]->setTheme(QPixmap(ThemeHandler::drop4CounterFile()), 32, false);
}


void DraftDropCounter::resetAll()
{
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->reset();
    }
}


void DraftDropCounter::setHidden(bool hide)
{
    if(hide)
    {
        dropCounters[V_DROP2]->hide();
        dropCounters[V_DROP3]->hide();
        dropCounters[V_DROP4]->hide();
    }
    else
    {
        dropCounters[V_DROP2]->show();
        dropCounters[V_DROP3]->show();
        dropCounters[V_DROP4]->show();
    }
}


void DraftDropCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


void DraftDropCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    DraftDropCounter::synergyCodes = synergyCodes;
}


QStringList DraftDropCounter::debugDropSynergies(const QString &code, int attack, int health, int cost)
{
    QStringList mec;

    if(isDrop2(code, cost, attack, health)) mec<<"drop2";
    if(isDrop3(code, cost, attack, health)) mec<<"drop3";
    if(isDrop4(code, cost, attack, health)) mec<<"drop4";

    return mec;
}


void DraftDropCounter::getDropCounters(QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map)
{
    drop2Map = dropCounters[V_DROP2]->getCodeTagMap();
    drop3Map = dropCounters[V_DROP3]->getCodeTagMap();
    drop4Map = dropCounters[V_DROP4]->getCodeTagMap();
}


void DraftDropCounter::getDropMechanicIcons(const QString &code, QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor,
                                            int attack, int health, int cost)
{
    if(isDrop2(code, cost, attack, health))
    {
        mechanicIcons[M_DROP2] = dropCounters[V_DROP2]->count() + 1;
        dropBorderColor = dropCounters[V_DROP2]->getMechanicBorderColor();
    }
    else if(isDrop3(code, cost, attack, health))
    {
        mechanicIcons[M_DROP3] = dropCounters[V_DROP3]->count() + 1;
        dropBorderColor = dropCounters[V_DROP3]->getMechanicBorderColor();
    }
    else if(isDrop4(code, cost, attack, health))
    {
        mechanicIcons[M_DROP4] = dropCounters[V_DROP4]->count() + 1;
        dropBorderColor = dropCounters[V_DROP4]->getMechanicBorderColor();
    }
    else
    {
        dropBorderColor = MechanicBorderGrey;
    }
}


void DraftDropCounter::updateDropCounters(const QString &code, QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
                                          int attack, int health, int cost)
{
    if(isDrop2(code, cost, attack, health))
    {
        dropCounters[V_DROP2]->increase(code);
        drop2Map.insertMulti(code, "");
        if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code, "2 Drop");
            drop3Map.insertMulti(code, "2 Drop");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code, "2 Drop");
            drop4Map.insertMulti(code, "2 Drop");
        }
    }
    else if(isDrop3(code, cost, attack, health))
    {
        dropCounters[V_DROP3]->increase(code);
        drop3Map.insertMulti(code, "");
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code, "3 Drop");
            drop2Map.insertMulti(code, "3 Drop");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code, "3 Drop");
            drop4Map.insertMulti(code, "3 Drop");
        }
    }
    else if(isDrop4(code, cost, attack, health))
    {
        dropCounters[V_DROP4]->increase(code);
        drop4Map.insertMulti(code, "");
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code, "4 Drop");
            drop2Map.insertMulti(code, "4 Drop");
        }
        else if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code, "4 Drop");
            drop3Map.insertMulti(code, "4 Drop");
        }
    }
    else
    {
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code);
            drop2Map.insertMulti(code, ".");
        }
        else if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code);
            drop3Map.insertMulti(code, ".");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code);
            drop4Map.insertMulti(code, ".");
        }
    }

    //Hay una carta mas en el mazo
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->increaseNumCards();
    }
}


bool DraftDropCounter::isDrop2(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("drop2");
    }
    else if(cost == 2 && !(
                 (attack==1 && health<4) ||
                 (attack==2 && health<2)
                 ))
    {
        return true;
    }
    return false;
}
bool DraftDropCounter::isDrop3(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("drop3");
    }
    else if(cost == 3 && health != 1 && !(
                 (attack==1 && health<7) ||
                 (attack==2 && health<4)
                 ))
    {
        return true;
    }
    return false;
}
bool DraftDropCounter::isDrop4(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("drop4");
    }
    else if(cost == 4 && health != 1 && !(
                 (attack==1) ||
                 (attack==2 && health<5) ||
                 (attack==3 && health<5) ||
                 (attack==4 && health<3)
                 ))
    {
        return true;
    }
    return false;
}
