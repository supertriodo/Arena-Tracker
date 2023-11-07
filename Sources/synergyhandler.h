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
    bool isValidSynergyCode(const QString &mechanic);
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
    bool containsAll(const QString &text, const QString &words);

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
    bool isEnrageGen(const QString &code, const QJsonArray &mechanics);
    bool isOverload(const QString &code);
    bool isOverloadGen(const QString &code, const QJsonArray &referencedTags);
    bool isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isHeroPowerGen(const QString &code, const QString &text);
    bool isSecret(const QString &code, const QJsonArray &mechanics);
    bool isSecretGen(const QString &code);
    bool isEcho(const QString &code, const QJsonArray &mechanics);
    bool isEchoGen(const QString &code, const QJsonArray &referencedTags);
    bool isRush(const QString &code, const QJsonArray &mechanics);
    bool isRushGen(const QString &code, const QJsonArray &referencedTags);
    bool isMagnetic(const QString &code, const QJsonArray &mechanics);
    bool isMagneticGen(const QString &code);
    bool isEggGen(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType);
    bool isDamageFriendlyHeroGen(const QString &code);
    bool isFreezeEnemyGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isDiscardGen(const QString &code, const QString &text);
    bool isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const CardType &cardType);
    bool isBattlecry(const QString &code, const QJsonArray &mechanics);
    bool isBattlecryGen(const QString &code);
    bool isSilenceOwnGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isTauntGiverGen(const QString &code);
    bool isTokenGen(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isTokenCardGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isCombo(const QString &code, const QJsonArray &mechanics);
    bool isComboGen(const QString &code);
    bool isWindfuryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isAttackBuffGen(const QString &code, const QString &text);
    bool isAttackNerfGen(const QString &code, const QString &text);
    bool isHealthBuffGen(const QString &code, const QString &text);
    bool isReturnGen(const QString &code, const QString &text);
    bool isStealthGen(const QString &code, const QJsonArray &mechanics);
    bool isDivineShield(const QString &code, const QJsonArray &mechanics);
    bool isDivineShieldGen(const QString &code, const QJsonArray &referencedTags);
    bool isRestoreTargetMinionGen(const QString &code, const QString &text);
    bool isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isRestoreFriendlyMinionGen(const QString &code, const QString &text);
    bool isArmorGen(const QString &code, const QString &text);
    bool isLifestealMinon(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isLifestealGen(const QString &code, const QJsonArray &referencedTags);
    bool isSpellDamageGen(const QString &code);
    bool isEvolveGen(const QString &code, const QString &text);
    bool isSpawnEnemyGen(const QString &code, const QString &text);
    bool isHandBuffGen(const QString &code, const QString &text);
    bool isEnemyDrawGen(const QString &code, const QString &text);
    bool isHeroAttackGen(const QString &code, const QString &text);
    bool isSpellBuffGen(const QString &code, const QString &text, const QJsonArray &mechanics, const CardType &cardType);
    bool isOtherClassGen(const QString &code, const QString &text);
    bool isSilverHandGen(const QString &code, const QString &text);
    bool isTreantGen(const QString &code, const QString &text);
    bool isLackeyGen(const QString &code, const QString &text);    
    bool isOutcast(const QString &code, const QJsonArray &mechanics);
    bool isOutcastGen(const QString &code);
    bool isChooseOne(const QString &code, const QJsonArray &mechanics);
    bool isChooseOneGen(const QString &code);
    bool isEndTurnGen(const QString &code, const QString &text);
    bool isRushGiverGen(const QString &code, const QString &text);
    bool isDredge(const QString &code, const QJsonArray &mechanics);
    bool isCorpseGen(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isExcavate(const QString &code, const QJsonArray &mechanics);

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
    bool isEnrageSyn(const QString &code, const QString &text);
    bool isOverloadSyn(const QString &code);
    bool isOverloadAllSyn(const QString &code);
    bool isPingSyn(const QString &code);
    bool isAoeSyn(const QString &code);
    bool isTauntSyn(const QString &code);
    bool isTauntAllSyn(const QString &code);
    bool isSecretSyn(const QString &code);
    bool isSecretAllSyn(const QString &code, const QJsonArray &referencedTags);
    bool isEchoSyn(const QString &code);
    bool isEchoAllSyn(const QString &code);
    bool isRushSyn(const QString &code);
    bool isRushAllSyn(const QString &code);
    bool isMagneticSyn(const QString &code);
    bool isMagneticAllSyn(const QString &code);
    bool isEggSyn(const QString &code, const QString &text);
    bool isDamageFriendlyHeroSyn(const QString &code);
    bool isFreezeEnemySyn(const QString &code, const QJsonArray &referencedTags, const QString &text);
    bool isDiscardSyn(const QString &code, const QString &text);
    bool isDeathrattleSyn(const QString &code);
    bool isDeathrattleGoodAllSyn(const QString &code, const QString &text);
    bool isBattlecrySyn(const QString &code);
    bool isBattlecryAllSyn(const QString &code, const QJsonArray &referencedTags);
    bool isSilenceOwnSyn(const QString &code, const QJsonArray &mechanics);
    bool isTauntGiverSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType);
    bool isTokenSyn(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isTokenCardSyn(const QString &code, const QString &text);
    bool isComboSyn(const QString &code);
    bool isComboAllSyn(const QString &code, const QJsonArray &referencedTags);
    bool isWindfuryMinionSyn(const QString &code);
    bool isAttackBuffSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType);
    bool isAttackNerfSyn(const QString &code, const QString &text);
    bool isHealthBuffSyn(const QString &code);
    bool isReturnSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text);
    bool isStealthSyn(const QString &code);
    bool isDivineShieldSyn(const QString &code);
    bool isDivineShieldAllSyn(const QString &code);
    bool isRestoreTargetMinionSyn(const QString &code, const QString &text);
    bool isRestoreFriendlyHeroSyn(const QString &code);
    bool isRestoreFriendlyMinionSyn(const QString &code, const QString &text);
    bool isArmorSyn(const QString &code);
    bool isLifestealMinionSyn(const QString &code);
    bool isLifestealAllSyn(const QString &code);
    bool isSpellDamageSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text);
    bool isEvolveSyn(const QString &code);
    bool isSpawnEnemySyn(const QString &code);
    bool isHandBuffSyn(const QString &code, const QString &text);
    bool isEnemyDrawSyn(const QString &code, const QString &text);
    bool isHeroAttackSyn(const QString &code);
    bool isSpellBuffSyn(const QString &code, const QString &text);
    bool isOtherClassSyn(const QString &code, const QString &text);
    bool isSilverHandSyn(const QString &code);
    bool isTreantSyn(const QString &code);
    bool isLackeySyn(const QString &code);
    bool isOutcastSyn(const QString &code);
    bool isOutcastAllSyn(const QString &code, const QJsonArray &referencedTags);
    bool isChooseOneSyn(const QString &code);
    bool isChooseOneAllSyn(const QString &code, const QJsonArray &referencedTags);
    bool isEndTurnSyn(const QString &code, const QString &text);
    bool isRushGiverSyn(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isDredgeSyn(const QString &code, const QString &text);
    bool isCorpseSyn(const QString &code, const QString &text);
    bool isExcavateSyn(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);

signals:
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SynergyHandler");
    void itemEnter(QList<SynergyCard> &synergyCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemLeave();

private slots:
    void sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);
};

#endif // SYNERGYHANDLER_H
