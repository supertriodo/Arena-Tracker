#ifndef SYNERGYHANDLER_H
#define SYNERGYHANDLER_H

#include <QObject>
#include "Widgets/ui_extended.h"
#include "Synergies/draftitemcounter.h"
#include "Synergies/draftdropcounter.h"
#include "Synergies/statsynergies.h"
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
    StatSynergies costMinions, attackMinions, healthMinions, costSpells;
    StatSynergies costWeapons, attackWeapons, healthWeapons;
    DraftItemCounter **raceCounters, **schoolCounters, **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;
    DraftDropCounter **dropCounters;
    QStringList arenaSets;

//Metodos
public:
    void debugSynergiesSet(const QString &set, int openFrom=0, int openTo=-1, const QString &miniSet="", bool onlyCollectible=true);
    void debugSynergiesCode(QString code, int num=0);
    void debugMissingSynergiesAllSets();
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
    bool initSynergyCodes(bool all=false);
    void clearCounters();
    int draftedCardsCount();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void setTheme();
    int getCorrectedCardMana(DeckCard &deckCard);
    int getManaCounterCount();
    void setHidden(bool hide);
    void setArenaSets(QStringList arenaSets);
    QStringList getAllArenaCodes();

    bool isDrop2(const QString &code, int cost, int attack, int health);
    bool isDrop3(const QString &code, int cost, int attack, int health);
    bool isDrop4(const QString &code, int cost, int attack, int health);
    bool isAoeGen(const QString &code, const QString &text);
    bool isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                            const QString &text, const CardType &cardType, int attack);
    bool isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text);

private:
    void initSynergyCode(const QString &code, const QJsonArray &synergies);
    void initDirectLink(const QString &code, const QString &code2, const QStringList &coreCodes,
                        const QStringList &arenaCodes, bool all);
    void clearSynergyLists();
    bool shouldBeInSynergies(const QString &code);
    void createDraftItemCounters();
    void deleteDraftItemCounters();

    void updateManaCounter(DeckCard &deckCard);
    void updateRaceCounters(DeckCard &deckCard);
    void updateSchoolCounters(DeckCard &deckCard);
    void updateCardTypeCounters(DeckCard &deckCard, QMap<QString, QString> &spellMap, QMap<QString,
                                QString> &minionMap, QMap<QString, QString> &weaponMap);
    void updateDropCounters(DeckCard &deckCard, QMap<QString, QString> &drop2Map,
                            QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map);
    void updateMechanicCounters(DeckCard &deckCard,
                                QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                int &draw, int &toYourHand, int &discover);
    void updateStatsCards(DeckCard &deckCard);

    void getCardTypeSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergyTagMap);
    void getDropMechanicIcons(DeckCard &deckCard, QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor);
    void getRaceSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergyTagMap);
    void getSchoolSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergyTagMap);
    void getMechanicSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons);
    void getDirectLinkSynergies(DeckCard &deckCard, QMap<QString, int> &synergies);
    void getStatsCardsSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int> > &synergyTagMap);

    bool isSpellGen(const QString &code);
    bool isWeaponGen(const QString &code, const QString &text);
    bool isLocationGen(const QString &code);
    bool isMurlocGen(const QString &code);
    bool isDemonGen(const QString &code);
    bool isMechGen(const QString &code);
    bool isElementalGen(const QString &code);
    bool isBeastGen(const QString &code);
    bool isTotemGen(const QString &code);
    bool isPirateGen(const QString &code);
    bool isDragonGen(const QString &code);
    bool isNagaGen(const QString &code);
    bool isUndeadGen(const QString &code);
    bool isQuilboarGen(const QString &code);
    bool isDraeneiGen(const QString &code);
    //New race step
    bool isArcaneGen(const QString &code);
    bool isFelGen(const QString &code);
    bool isFireGen(const QString &code);
    bool isFrostGen(const QString &code);
    bool isHolyGen(const QString &code);
    bool isShadowGen(const QString &code);
    bool isNatureGen(const QString &code);
    bool isDiscoverDrawGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isDrawGen(const QString &code, const QString &text);
    bool isToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    int numDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    int numDrawGen(const QString &code, const QString &text);
    int numToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    bool isTaunt(const QString &code, const QJsonArray &mechanics);
    bool isTauntGen(const QString &code, const QJsonArray &referencedTags);
    bool isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                   const QString &text, const CardType &cardType, int attack);
    bool isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, const CardType &cardType, int attack);
    bool isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isHeroPowerGen(const QString &code, const QString &text);
    bool isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const CardType &cardType);
    bool isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isArmorGen(const QString &code, const QString &text);

    bool isMurlocSyn(const QString &code);
    bool isDemonSyn(const QString &code);
    bool isMechSyn(const QString &code);
    bool isElementalSyn(const QString &code);
    bool isBeastSyn(const QString &code);
    bool isTotemSyn(const QString &code);
    bool isPirateSyn(const QString &code);
    bool isDragonSyn(const QString &code);
    bool isNagaSyn(const QString &code);
    bool isUndeadSyn(const QString &code);
    bool isQuilboarSyn(const QString &code);
    bool isDraeneiSyn(const QString &code);
    //New race step
    bool isMurlocAllSyn(const QString &code, const QString &text);
    bool isDemonAllSyn(const QString &code, const QString &text);
    bool isMechAllSyn(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isElementalAllSyn(const QString &code, const QString &text);
    bool isBeastAllSyn(const QString &code, const QString &text);
    bool isTotemAllSyn(const QString &code, const QString &text);
    bool isPirateAllSyn(const QString &code, const QString &text);
    bool isDragonAllSyn(const QString &code, const QString &text);
    bool isNagaAllSyn(const QString &code, const QString &text);
    bool isUndeadAllSyn(const QString &code, const QString &text);
    bool isQuilboarAllSyn(const QString &code, const QString &text);
    bool isDraeneiAllSyn(const QString &code, const QString &text);
    //New race step
    bool isArcaneSyn(const QString &code);
    bool isFelSyn(const QString &code);
    bool isFireSyn(const QString &code);
    bool isFrostSyn(const QString &code);
    bool isHolySyn(const QString &code);
    bool isShadowSyn(const QString &code);
    bool isNatureSyn(const QString &code);
    bool isArcaneAllSyn(const QString &code, const QString &text);
    bool isFelAllSyn(const QString &code, const QString &text);
    bool isFireAllSyn(const QString &code, const QString &text);
    bool isFrostAllSyn(const QString &code, const QString &text);
    bool isHolyAllSyn(const QString &code, const QString &text);
    bool isShadowAllSyn(const QString &code, const QString &text);
    bool isNatureAllSyn(const QString &code, const QString &text);
    bool isDiscoverSyn(const QString &code);
    bool isDrawSyn(const QString &code);
    bool isToYourHandSyn(const QString &code);
    bool isSpellSyn(const QString &code);
    bool isSpellAllSyn(const QString &code, const QString &text);
    bool isWeaponSyn(const QString &code);
    bool isWeaponAllSyn(const QString &code, const QString &text);
    bool isLocationSyn(const QString &code);
    bool isLocationAllSyn(const QString &code, const QString &text);
    bool isPingSyn(const QString &code);
    bool isDamageMinionsSyn(const QString &code);
    bool isDestroySyn(const QString &code);
    bool isReachSyn(const QString &code);
    bool isAoeSyn(const QString &code);
    bool isTauntSyn(const QString &code);
    bool isTauntAllSyn(const QString &code);
    bool isDeathrattleSyn(const QString &code);
    bool isDeathrattleGoodAllSyn(const QString &code, const QString &text);
    bool isRestoreFriendlyHeroSyn(const QString &code);
    bool isArmorSyn(const QString &code);

signals:
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SynergyHandler");
    void itemEnter(QList<SynergyCard> &synergyCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemLeave();

private slots:
    void sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);
};

#endif // SYNERGYHANDLER_H
