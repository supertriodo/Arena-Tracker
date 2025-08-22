#ifndef SYNERGYHANDLER_H
#define SYNERGYHANDLER_H

#include <QObject>
#include "Widgets/ui_extended.h"
#include "Synergies/draftitemcounter.h"
#include "Synergies/draftdropcounter.h"
#include "utility.h"


class SynergyHandler : public QObject
{
    Q_OBJECT
public:
    SynergyHandler(QObject *parent, Ui::Extended *ui);
    ~SynergyHandler();

//Variables
private:
    Ui::Extended *ui;
    QMap<QString, QList<QString>> synergyCodes, directLinks;
    DraftItemCounter *manaCounter;

//Metodos
public:
    void debugSynergiesSet(const QString &set, int openFrom=0, int openTo=-1, const QString &miniSet="", bool onlyCollectible=true);
    QStringList debugSynergiesCode(QString code, int num=0);
    void debugMissingSynergies(bool onlyArena, bool showCards);
    void debugDrops();
    bool isValidSynergyCode(const QString &mechanic, QRegularExpressionMatch *match);
    void testSynergies(const QString &miniSet="");
    int getCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                    QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
                    QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                    QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                    QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                    QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                    int &draw, int &toYourHand, int &discover);
    void updateCounters(DeckCard &deckCard,
                        QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                        QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
                        QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                        QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                        QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                        QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                        int &draw, int &toYourHand, int &discover);
    void getSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergies,
                      QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor);
    bool initSynergyCodes(const QStringList &arenaCodes, bool all=false);
    void clearCounters();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void setTheme();
    int getCorrectedCardMana(DeckCard &deckCard);
    int getCorrectedCardMana(const QString &code, int cost);
    int getManaCounterCount();
    void setHidden(bool hide);

private:
    void initSynergyCode(const QString &code, const QJsonArray &synergies);
    void initDirectLink(const QString &code, const QString &code2, const QStringList &coreCodes,
                        const QStringList &arenaCodes, bool all);
    void clearSynergyLists();
    bool shouldBeInSynergies(const QString &code);
    void saveSynergiesSetJson(QMap<QString, QStringList> &synergiesMap);
    void connectCounters(QMap<QString, DraftDropCounter *> *dropCounters,
                         QMap<QString, DraftItemCounter *> *cardTypeCounters,
                         QMap<QString, DraftItemCounter *> *mechanicCounters);
    void createDraftItemCounters();

    void updateManaCounter(const QString &code, int cost);

signals:
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SynergyHandler");
    void itemEnter(QList<SynergyCard> &synergyCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemLeave();

private slots:
    void sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);
};

#endif // SYNERGYHANDLER_H
