#ifndef SYNERGYHANDLER_H
#define SYNERGYHANDLER_H

#include <QObject>
#include "Widgets/ui_extended.h"
#include "Synergies/draftitemcounter.h"
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
    StatSynergies costMinions, attackMinions, healthMinions;
    DraftItemCounter **raceCounters, **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;
    QHBoxLayout *horLayoutCardTypes, *horLayoutMechanics1, *horLayoutMechanics2;

//Metodos
public:
    void debugSynergiesSet(const QString &set);
    void debugSynergiesCode(const QString &code, int num=0);
    void testSynergies();
    int getCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                        QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                        QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList);
    void updateCounters(DeckCard &deckCard, QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                        QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                        QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList);
    void getSynergies(DeckCard &deckCard, QMap<QString, int> &synergies, QStringList &mechanicIcons);
    void initSynergyCodes();
    void clearLists(bool keepCounters);
    int draftedCardsCount();
    void setTransparency(Transparency transparency, bool mouseInApp);

private:
    void createDraftItemCounters();
    void deleteDraftItemCounters();

    void updateManaCounter(DeckCard &deckCard);
    void updateRaceCounters(DeckCard &deckCard);
    void updateCardTypeCounters(DeckCard &deckCard, QStringList &spellList, QStringList &minionList, QStringList &weaponList);
    void updateMechanicCounters(DeckCard &deckCard,
                                QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList);
    void updateStatsCards(DeckCard &deckCard);

    void getCardTypeSynergies(DeckCard &deckCard, QMap<QString, int> &synergies);
    void getRaceSynergies(DeckCard &deckCard, QMap<QString, int> &synergies);
    void getMechanicSynergies(DeckCard &deckCard, QMap<QString, int> &synergies, QStringList &mechanicIcons);
    void getDirectLinkSynergies(DeckCard &deckCard, QMap<QString, int> &synergies);
    void getStatsCardsSynergies(DeckCard &deckCard, QMap<QString, int> &synergies);

private:
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
    bool isDiscoverDrawGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isTaunt(const QString &code, const QJsonArray &mechanics);
    bool isTauntGen(const QString &code, const QJsonArray &referencedTags);
    bool isAoeGen(const QString &code, const QString &text);
    bool isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, const CardType &cardType, int attack);
    bool isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                   const QString &text, const CardType &cardType, int attack);
    bool isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, const CardType &cardType, int attack);
    bool isEnrageGen(const QString &code, const QJsonArray &mechanics);
    bool isOverload(const QString &code);
    bool isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isSecretGen(const QString &code, const QJsonArray &mechanics);
    bool isFreezeEnemyGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isDiscardGen(const QString &code, const QString &text);
    bool isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const CardType &cardType);
    bool isBattlecryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isSilenceOwnGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isTauntGiverGen(const QString &code);
    bool isTokenGen(const QString &code, const QString &text);
    bool isTokenCardGen(const QString &code, int cost);
    bool isComboGen(const QString &code, const QJsonArray &mechanics);
    bool isWindfuryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isAttackBuffGen(const QString &code, const QString &text);
    bool isHealthBuffGen(const QString &code, const QString &text);
    bool isReturnGen(const QString &code, const QString &text);
    bool isStealthGen(const QString &code, const QJsonArray &mechanics);
    bool isDivineShield(const QString &code, const QJsonArray &mechanics);
    bool isDivineShieldGen(const QString &code, const QJsonArray &referencedTags);
    bool isRestoreTargetMinionGen(const QString &code, const QString &text);
    bool isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QString &text);
    bool isRestoreFriendlyMinionGen(const QString &code, const QString &text);
    bool isArmorGen(const QString &code, const QString &text);
    bool isLifestealMinon(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isSpellDamageGen(const QString &code);
    bool isEvolveGen(const QString &code, const QString &text);

    bool isMurlocSyn(const QString &code);
    bool isDemonSyn(const QString &code);
    bool isMechSyn(const QString &code);
    bool isElementalSyn(const QString &code);
    bool isBeastSyn(const QString &code);
    bool isTotemSyn(const QString &code);
    bool isPirateSyn(const QString &code);
    bool isDragonSyn(const QString &code, const QString &text);
    bool isMurlocAllSyn(const QString &code, const QString &text);
    bool isDemonAllSyn(const QString &code, const QString &text);
    bool isMechAllSyn(const QString &code, const QString &text);
    bool isElementalAllSyn(const QString &code, const QString &text);
    bool isBeastAllSyn(const QString &code, const QString &text);
    bool isTotemAllSyn(const QString &code, const QString &text);
    bool isPirateAllSyn(const QString &code, const QString &text);
    bool isDragonAllSyn(const QString &code);
    bool isSpellSyn(const QString &code, const QString &text);
    bool isWeaponSyn(const QString &code);
    bool isWeaponAllSyn(const QString &code, const QString &text);
    bool isEnrageSyn(const QString &code, const QString &text);
    bool isOverloadSyn(const QString &code, const QString &text);
    bool isPingSyn(const QString &code);
    bool isAoeSyn(const QString &code);
    bool isTauntSyn(const QString &code);
    bool isTauntAllSyn(const QString &code);
    bool isSecretSyn(const QString &code, const QJsonArray &referencedTags);
    bool isFreezeEnemySyn(const QString &code, const QJsonArray &referencedTags, const QString &text);
    bool isDiscardSyn(const QString &code, const QString &text);
    bool isDeathrattleSyn(const QString &code);
    bool isDeathrattleGoodAllSyn(const QString &code);
    bool isBattlecrySyn(const QString &code, const QJsonArray &referencedTags);
    bool isSilenceOwnSyn(const QString &code, const QJsonArray &mechanics);
    bool isTauntGiverSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType);
    bool isTokenSyn(const QString &code, const QString &text);
    bool isTokenCardSyn(const QString &code, const QString &text);
    bool isComboSyn(const QString &code, const QJsonArray &referencedTags, int cost);
    bool isWindfuryMinionSyn(const QString &code);
    bool isAttackBuffSyn(const QString &code);
    bool isHealthBuffSyn(const QString &code);
    bool isReturnSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text);
    bool isStealthSyn(const QString &code);
    bool isDivineShieldSyn(const QString &code);
    bool isDivineShieldAllSyn(const QString &code);
    bool isRestoreTargetMinionSyn(const QString &code);
    bool isRestoreFriendlyHeroSyn(const QString &code);
    bool isRestoreFriendlyMinionSyn(const QString &code);
    bool isArmorSyn(const QString &code);
    bool isLifestealMinionSyn(const QString &code);
    bool isSpellDamageSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text);
    bool isEvolveSyn(const QString &code);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SynergyHandler");
    void itemEnter(QList<DeckCard> &deckCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemLeave();

private slots:
    void sendItemEnter(QList<DeckCard> &deckCardList, QRect &labelRect);
};

#endif // SYNERGYHANDLER_H
