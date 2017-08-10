#ifndef SYNERGYHANDLER_H
#define SYNERGYHANDLER_H

#include <QObject>
#include "Widgets/ui_extended.h"
#include "Synergies/draftitemcounter.h"
#include "Synergies/statsynergies.h"
#include "utility.h"


enum VisibleRace {V_MURLOC, V_DEMON, V_MECHANICAL, V_ELEMENTAL, V_BEAST, V_TOTEM, V_PIRATE, V_DRAGON, V_NUM_RACES};
enum VisibleType {V_MINION, V_SPELL, V_WEAPON, V_WEAPON_ALL, V_NUM_TYPES};
enum VisibleMechanics {V_AOE, V_TAUNT, V_TAUNT_ALL, V_DISCOVER_DRAW, V_PING, V_DAMAGE, V_DESTROY, V_REACH,
                       V_OVERLOAD, V_JADE_GOLEM, V_SECRET, V_FREEZE_ENEMY, V_DISCARD, V_EVOLVE,
                       V_BATTLECRY, V_SILENCE, V_STEALTH, V_DEATHRATTLE, V_DEATHRATTLE_GOOD_ALL,
                       V_TAUNT_GIVER, V_TOKEN, V_WINDFURY, V_ATTACK_BUFF, V_HEALTH_BUFF, V_RETURN,
                       V_DIVINE_SHIELD, V_DIVINE_SHIELD_ALL, V_ENRAGED_MINION, V_ENRAGED_ALL,
                       V_RESTORE_FRIENDLY_HEROE, V_RESTORE_FRIENDLY_MINION, V_RESTORE_TARGET_MINION,
                       V_ARMOR, V_SPELL_DAMAGE, V_NUM_MECHANICS};


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
    void debugSynergies(QString set);
    void testSynergies();
    void updateCounters(DeckCard &deckCard);
    void getSynergies(DeckCard &deckCard, QMap<QString, int> &synergies, QStringList &mechanicIcons);
    void initSynergyCodes();
    void clearLists(bool keepCounters);
    int draftedCardsCount();
    void initCounters(QList<DeckCard> deckCardList);
    void setTransparency(Transparency transparency, bool mouseInApp);

private:
    void createDraftItemCounters();
    void deleteDraftItemCounters();

    void updateManaCounter(DeckCard &deckCard);
    void updateRaceCounters(DeckCard &deckCard);
    void updateCardTypeCounters(DeckCard &deckCard);
    void updateMechanicCounters(DeckCard &deckCard);
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
    bool isDestroyGen(const QString &code, const QString &text);
    bool isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                   const QString &text, const CardType &cardType, int attack);
    bool isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, const CardType &cardType, int attack);
    bool isEnrageMinion(const QString &code, const QJsonArray &mechanics);
    bool isEnrageGen(const QString &code, const QJsonArray &referencedTags);
    bool isOverload(const QString &code);
    bool isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isSecretGen(const QString &code, const QJsonArray &mechanics);
    bool isFreezeEnemyGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    bool isDiscardGen(const QString &code, const QString &text);
    bool isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isBattlecryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    bool isSilenceOwnGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    bool isTauntGiverGen(const QString &code);
    bool isTokenGen(const QString &code, const QString &text);
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
    bool isSpellDamageGen(const QString &code);
    bool isEvolveGen(const QString &code, const QString &text);

    bool isMurlocSyn(const QString &code, const QString &text);
    bool isDemonSyn(const QString &code, const QString &text);
    bool isMechSyn(const QString &code, const QString &text);
    bool isElementalSyn(const QString &code, const QString &text);
    bool isBeastSyn(const QString &code, const QString &text);
    bool isTotemSyn(const QString &code, const QString &text);
    bool isPirateSyn(const QString &code, const QString &text);
    bool isDragonSyn(const QString &code, const QString &text);
    bool isSpellSyn(const QString &code, const QString &text);
    bool isWeaponSyn(const QString &code);
    bool isWeaponAllSyn(const QString &code, const QString &text);
    bool isEnrageMinionSyn(const QString &code);
    bool isEnrageAllSyn(const QString &code, const QString &text);
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
    bool isWindfurySyn(const QString &code);
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
    bool isSpellDamageSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text);
    bool isEvolveSyn(const QString &code);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SynergyHandler");
};

#endif // SYNERGYHANDLER_H
