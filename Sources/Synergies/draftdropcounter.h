#ifndef DRAFTDROPCOUNTER_H
#define DRAFTDROPCOUNTER_H

#include "draftitemcounter.h"

#define TARGET_DROP_2 6
#define TARGET_DROP_3 5
#define TARGET_DROP_4 4

class DraftDropCounter : public DraftItemCounter
{
    Q_OBJECT
public:
    DraftDropCounter(QObject *parent, QString synergyTag, QString synergyTagExtra,
                     QHBoxLayout *hLayout, int targetDrops, QPixmap pixmap, bool iconHover=true);
    DraftDropCounter(QObject *parent, QString synergyTag, QString synergyTagExtra,
                     QGridLayout *gridLayout, int gridRow, int gridCol, int targetDrops,
                     QPixmap pixmap, int iconWidth, bool iconHover=true);

//Variables
private:
    int numCards;
    int targetDrops;

//Metodos
private:
    void drawBorder(bool greenBorder);
    bool isGreenTargetDrop();
    void init(int targetDrops);

public:
    void reset();
    void setTheme(QPixmap pixmap, int iconWidth=32, bool inDraftMechanicsWindow=false);
    void increase(const QString &code);
    void setNumCards(int numCards);
    void increaseNumCards();
    MechanicBorderColor getMechanicBorderColor();
};

#endif // DRAFTDROPCOUNTER_H
