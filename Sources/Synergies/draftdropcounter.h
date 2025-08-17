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
                     QGridLayout *gridLayout, int gridRow, int gridCol, int targetDrops,
                     QPixmap pixmap, int iconWidth, bool iconHover=true, bool inDraftMechanicsWindow=true);

//Variables
private:
    int numCards;
    int targetDrops;

    static QMap<QString, DraftDropCounter*> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    void drawBorder(bool greenBorder);
    bool isGreenTargetDrop();
    void init(int targetDrops);

    static QMap<QString, QString> getMapKeySynergies();

public:
    void reset();
    void setTheme(QPixmap pixmap, int iconWidth=32, bool inDraftMechanicsWindow=false);
    void increase(const QString &code);
    void setNumCards(int numCards);
    void increaseNumCards();
    MechanicBorderColor getMechanicBorderColor();

    static QMap<QString, DraftDropCounter *> *createDropCounters(QObject *parent, QGridLayout *mechanicsLayout);
    static QStringList getListKeySynergies();
    static QStringList getListValidSynergies();
    static QStringList getListKeyLabels();
    static void setTheme();
    static void resetAll();
    static void setHidden(bool hide);
    static void setTransparency(Transparency transparency, bool mouseInApp);
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugDropSynergies(const QString &code, int attack, int health, int cost);
    static void getDropCounters(QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map);
    static void getDropMechanicIcons(const QString &code, QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor,
                                     int attack, int health, int cost);
    static void updateDropCounters(const QString &code, QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
                                   int attack, int health, int cost);

    static bool isDrop2(const QString &code, int cost, int attack, int health);
    static bool isDrop3(const QString &code, int cost, int attack, int health);
    static bool isDrop4(const QString &code, int cost, int attack, int health);
};

#endif // DRAFTDROPCOUNTER_H
