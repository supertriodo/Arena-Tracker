#include "synergyhandler.h"
#include <QtWidgets>

SynergyHandler::SynergyHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;

    createDraftItemCounters();
}


SynergyHandler::~SynergyHandler()
{
    deleteDraftItemCounters();
}


void SynergyHandler::createDraftItemCounters()
{
    horLayoutCardTypes = new QHBoxLayout();
    horLayoutMechanics1 = new QHBoxLayout();
    horLayoutMechanics2 = new QHBoxLayout();


    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(":/Images/minionsCounter.png"));
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(":/Images/spellsCounter.png"));
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(":/Images/weaponsCounter.png"));
    cardTypeCounters[V_WEAPON_ALL] = new DraftItemCounter(this);

    manaCounter = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(":/Images/manaCounter.png"));

    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(this);
    raceCounters[V_BEAST] = new DraftItemCounter(this);
    raceCounters[V_MURLOC] = new DraftItemCounter(this);
    raceCounters[V_DRAGON] = new DraftItemCounter(this);
    raceCounters[V_PIRATE] = new DraftItemCounter(this);
    raceCounters[V_MECHANICAL] = new DraftItemCounter(this);
    raceCounters[V_DEMON] = new DraftItemCounter(this);
    raceCounters[V_TOTEM] = new DraftItemCounter(this);

    raceCounters[V_ELEMENTAL_ALL] = new DraftItemCounter(this);
    raceCounters[V_BEAST_ALL] = new DraftItemCounter(this);
    raceCounters[V_MURLOC_ALL] = new DraftItemCounter(this);
    raceCounters[V_DRAGON_ALL] = new DraftItemCounter(this);
    raceCounters[V_PIRATE_ALL] = new DraftItemCounter(this);
    raceCounters[V_MECHANICAL_ALL] = new DraftItemCounter(this);
    raceCounters[V_DEMON_ALL] = new DraftItemCounter(this);
    raceCounters[V_TOTEM_ALL] = new DraftItemCounter(this);

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_AOE] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(":/Images/aoeMechanic.png"));
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(":/Images/tauntMechanic.png"));
    mechanicCounters[V_RESTORE_FRIENDLY_HEROE] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(":/Images/restoreMechanic.png"));
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(":/Images/drawMechanic.png"));

    mechanicCounters[V_PING] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(":/Images/pingMechanic.png"));
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(":/Images/damageMechanic.png"));
    mechanicCounters[V_DESTROY] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(":/Images/destroyMechanic.png"));
    mechanicCounters[V_REACH] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(":/Images/reachMechanic.png"));

    mechanicCounters[V_TAUNT] = new DraftItemCounter(this);
    mechanicCounters[V_OVERLOAD] = new DraftItemCounter(this);
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this);
    mechanicCounters[V_SECRET] = new DraftItemCounter(this);
    mechanicCounters[V_FREEZE_ENEMY] = new DraftItemCounter(this);
    mechanicCounters[V_DISCARD] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_BATTLECRY] = new DraftItemCounter(this);
    mechanicCounters[V_SILENCE] = new DraftItemCounter(this);
    mechanicCounters[V_TAUNT_GIVER] = new DraftItemCounter(this);
    mechanicCounters[V_TOKEN] = new DraftItemCounter(this);
    mechanicCounters[V_TOKEN_CARD] = new DraftItemCounter(this);
    mechanicCounters[V_COMBO] = new DraftItemCounter(this);
    mechanicCounters[V_WINDFURY_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_ATTACK_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_HEALTH_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_RETURN] = new DraftItemCounter(this);
    mechanicCounters[V_STEALTH] = new DraftItemCounter(this);
    mechanicCounters[V_SPELL_DAMAGE] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_ENRAGED_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_ENRAGED_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_RESTORE_FRIENDLY_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_RESTORE_TARGET_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_ARMOR] = new DraftItemCounter(this);
    mechanicCounters[V_EVOLVE] = new DraftItemCounter(this);
    mechanicCounters[V_LIFESTEAL_MINION] = new DraftItemCounter(this);

    horLayoutCardTypes->addStretch();
    horLayoutMechanics1->addStretch();
    horLayoutMechanics2->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutCardTypes);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics1);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics2);
}


void SynergyHandler::deleteDraftItemCounters()
{
    delete manaCounter;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        delete cardTypeCounters[i];
    }
    delete []cardTypeCounters;

    for(int i=0; i<V_NUM_RACES; i++)
    {
        delete raceCounters[i];
    }
    delete []raceCounters;

    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        delete mechanicCounters[i];
    }
    delete []mechanicCounters;
}


void SynergyHandler::initSynergyCodes()
{
    synergyCodes.clear();
    directLinks.clear();

    QFile jsonFile(Utility::extraPath() + "/synergies.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonObject = jsonDoc.object();

    for(const QString &code: jsonObject.keys())
    {
        if(jsonObject.value(code).isArray())
        {
            synergyCodes[code];
            QJsonArray synergies = jsonObject.value(code).toArray();
            for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
            {
                synergyCodes[code].append(it->toString());
            }
        }
    }
    emit pDebug("Synergy Cards: " + QString::number(synergyCodes.count()));


    //Direct links
    QJsonObject dlObject = jsonObject.value("DIRECT_LINKS").toObject();
    for(const QString &code: dlObject.keys())
    {
        if(dlObject.value(code).isArray())
        {
            QJsonArray synergies = dlObject.value(code).toArray();
            for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
            {
                directLinks[code].append(it->toString());
                directLinks[it->toString()].append(code);
            }
        }
    }
    emit pDebug("Direct Link Cards: " + QString::number(directLinks.count()));
//    qDebug()<<directLinks;//TODO
}


void SynergyHandler::clearLists(bool keepCounters)
{
    synergyCodes.clear();
    directLinks.clear();

    if(!keepCounters)
    {
        //Reset counters
        manaCounter->reset();
        for(int i=0; i<V_NUM_TYPES; i++)
        {
            cardTypeCounters[i]->reset();
        }
        for(int i=0; i<V_NUM_RACES; i++)
        {
            raceCounters[i]->reset();
        }
        for(int i=0; i<V_NUM_MECHANICS; i++)
        {
            mechanicCounters[i]->reset();
        }

        //Reset stats maps
        costMinions.clear();
        attackMinions.clear();
        healthMinions.clear();
    }
}


int SynergyHandler::draftedCardsCount()
{
    int num = 0;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        num += cardTypeCounters[i]->count();
    }
    return num;
}


void SynergyHandler::initCounters(QList<DeckCard> deckCardList)
{
    if(draftedCardsCount() > 0) return;

    for(DeckCard deckCard: deckCardList)
    {
        if(deckCard.getType() == INVALID_TYPE)  continue;
        for(uint i=0; i<deckCard.total; i++)
        {
            updateCounters(deckCard);
        }
    }

    emit pDebug("Counters starts with " + QString::number(draftedCardsCount()) + " cards.");
}


void SynergyHandler::setTransparency(Transparency transparency, bool mouseInApp)
{
    manaCounter->setTransparency(transparency, mouseInApp);
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_RACES; i++)
    {
        raceCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


void SynergyHandler::updateCounters(DeckCard &deckCard)
{
    updateRaceCounters(deckCard);
    updateCardTypeCounters(deckCard);
    updateManaCounter(deckCard);
    updateMechanicCounters(deckCard);
    updateStatsCards(deckCard);
}


void SynergyHandler::updateManaCounter(DeckCard &deckCard)
{
    manaCounter->increase(deckCard.getCost(), draftedCardsCount());
}


void SynergyHandler::updateRaceCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardRace cardRace = deckCard.getRace();

    if(cardRace == MURLOC)
    {
        raceCounters[V_MURLOC]->increase(code);
        raceCounters[V_MURLOC_ALL]->increase(code);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->increase(code);
    if(cardRace == DEMON)
    {
        raceCounters[V_DEMON]->increase(code);
        raceCounters[V_DEMON_ALL]->increase(code);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->increase(code);
    if(cardRace == MECHANICAL)
    {
        raceCounters[V_MECHANICAL]->increase(code);
        raceCounters[V_MECHANICAL_ALL]->increase(code);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->increase(code);
    if(cardRace == ELEMENTAL)
    {
        raceCounters[V_ELEMENTAL]->increase(code);
        raceCounters[V_ELEMENTAL_ALL]->increase(code);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->increase(code);
    if(cardRace == BEAST)
    {
        raceCounters[V_BEAST]->increase(code);
        raceCounters[V_BEAST_ALL]->increase(code);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->increase(code);
    if(cardRace == TOTEM)
    {
        raceCounters[V_TOTEM]->increase(code);
        raceCounters[V_TOTEM_ALL]->increase(code);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->increase(code);
    if(cardRace == PIRATE)
    {
        raceCounters[V_PIRATE]->increase(code);
        raceCounters[V_PIRATE_ALL]->increase(code);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->increase(code);
    if(cardRace == DRAGON)
    {
        raceCounters[V_DRAGON]->increase(code);
        raceCounters[V_DRAGON_ALL]->increase(code);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->increase(code);

    if(isMurlocSyn(code))                   raceCounters[V_MURLOC]->increaseSyn(code);
    else if(isMurlocAllSyn(code, text))     raceCounters[V_MURLOC_ALL]->increaseSyn(code);
    if(isDemonSyn(code))                    raceCounters[V_DEMON]->increaseSyn(code);
    else if(isDemonAllSyn(code, text))      raceCounters[V_DEMON_ALL]->increaseSyn(code);
    if(isMechSyn(code))                     raceCounters[V_MECHANICAL]->increaseSyn(code);
    else if(isMechAllSyn(code, text))       raceCounters[V_MECHANICAL_ALL]->increaseSyn(code);
    if(isElementalSyn(code))                raceCounters[V_ELEMENTAL]->increaseSyn(code);
    else if(isElementalAllSyn(code, text))  raceCounters[V_ELEMENTAL_ALL]->increaseSyn(code);
    if(isBeastSyn(code))                    raceCounters[V_BEAST]->increaseSyn(code);
    else if(isBeastAllSyn(code, text))      raceCounters[V_BEAST_ALL]->increaseSyn(code);
    if(isTotemSyn(code))                    raceCounters[V_TOTEM]->increaseSyn(code);
    else if(isTotemAllSyn(code, text))      raceCounters[V_TOTEM_ALL]->increaseSyn(code);
    if(isPirateSyn(code))                   raceCounters[V_PIRATE]->increaseSyn(code);
    else if(isPirateAllSyn(code, text))     raceCounters[V_PIRATE_ALL]->increaseSyn(code);
    if(isDragonSyn(code, text))             raceCounters[V_DRAGON]->increaseSyn(code);
    else if(isDragonAllSyn(code))           raceCounters[V_DRAGON_ALL]->increaseSyn(code);
}


void SynergyHandler::updateCardTypeCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)               cardTypeCounters[V_SPELL]->increase(code);
    else if(isSpellGen(code))           cardTypeCounters[V_SPELL]->increase(code,false);
    if(cardType == MINION || cardType == HERO)  cardTypeCounters[V_MINION]->increase(code);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->increase(code);
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
    }
    else if(isWeaponGen(code, text))    cardTypeCounters[V_WEAPON_ALL]->increase(code);


    if(isSpellSyn(code, text))          cardTypeCounters[V_SPELL]->increaseSyn(code);
    if(isWeaponSyn(code))               cardTypeCounters[V_WEAPON]->increaseSyn(code);
    else if(isWeaponAllSyn(code, text)) cardTypeCounters[V_WEAPON_ALL]->increaseSyn(code);
}


void SynergyHandler::updateMechanicCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();

    if(isDiscoverDrawGen(code, mechanics, referencedTags, text))            mechanicCounters[V_DISCOVER_DRAW]->increase(code);
    if(isAoeGen(code, text))                                                mechanicCounters[V_AOE]->increase(code);
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mechanicCounters[V_PING]->increase(code);
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)) mechanicCounters[V_DAMAGE]->increase(code);
    if(isDestroyGen(code, mechanics, text))                                 mechanicCounters[V_DESTROY]->increase(code);
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mechanicCounters[V_REACH]->increase(code);
    if(isOverload(code))                                                    mechanicCounters[V_OVERLOAD]->increase(code);
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isSecretGen(code, mechanics))                                        mechanicCounters[V_SECRET]->increase(code);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mechanicCounters[V_FREEZE_ENEMY]->increase(code);
    if(isDiscardGen(code, text))                                            mechanicCounters[V_DISCARD]->increase(code);
    if(isDeathrattleMinion(code, mechanics, cardType))                      mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increase(code);
    if(isBattlecryMinion(code, mechanics, cardType))                        mechanicCounters[V_BATTLECRY]->increase(code);
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mechanicCounters[V_SILENCE]->increase(code);
    if(isTauntGiverGen(code))                                               mechanicCounters[V_TAUNT_GIVER]->increase(code);
    if(isTokenGen(code, text))                                              mechanicCounters[V_TOKEN]->increase(code);
    if(isTokenCardGen(code, cost))                                          mechanicCounters[V_TOKEN_CARD]->increase(code);
    if(isComboGen(code, mechanics))                                         mechanicCounters[V_COMBO]->increase(code);
    if(isWindfuryMinion(code, mechanics, cardType))                         mechanicCounters[V_WINDFURY_MINION]->increase(code);
    if(isAttackBuffGen(code, text))                                         mechanicCounters[V_ATTACK_BUFF]->increase(code);
    if(isHealthBuffGen(code, text))                                         mechanicCounters[V_HEALTH_BUFF]->increase(code);
    if(isReturnGen(code, text))                                             mechanicCounters[V_RETURN]->increase(code);
    if(isStealthGen(code, mechanics))                                       mechanicCounters[V_STEALTH]->increase(code);
    if(isSpellDamageGen(code))                                              mechanicCounters[V_SPELL_DAMAGE]->increase(code);
    if(isEvolveGen(code, text))                                             mechanicCounters[V_EVOLVE]->increase(code);
    if(isRestoreTargetMinionGen(code, text))                                mechanicCounters[V_RESTORE_TARGET_MINION]->increase(code);
    if(isRestoreFriendlyHeroGen(code, mechanics, text))                     mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increase(code);
    if(isRestoreFriendlyMinionGen(code, text))                              mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increase(code);
    if(isLifestealMinon(code, mechanics, cardType))                         mechanicCounters[V_LIFESTEAL_MINION]->increase(code);
    if(isArmorGen(code, text))
    {
        mechanicCounters[V_ARMOR]->increase(code);
        mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increase();
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->increase(code);
        mechanicCounters[V_TAUNT_ALL]->increase(code);
    }
    else if(isTauntGen(code, referencedTags))                               mechanicCounters[V_TAUNT_ALL]->increase(code);
    if(isDivineShield(code, mechanics))
    {
        mechanicCounters[V_DIVINE_SHIELD]->increase(code);
        mechanicCounters[V_DIVINE_SHIELD_ALL]->increase(code);
    }
    else if(isDivineShieldGen(code, referencedTags))                        mechanicCounters[V_DIVINE_SHIELD_ALL]->increase(code);
    if(isEnrageMinion(code, mechanics))
    {
        mechanicCounters[V_ENRAGED_MINION]->increase(code);
        mechanicCounters[V_ENRAGED_ALL]->increase(code);
    }
    if(isEnrageGen(code, referencedTags))                                   mechanicCounters[V_ENRAGED_ALL]->increase(code);


    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    else if(isTauntAllSyn(code))                                            mechanicCounters[V_TAUNT_ALL]->increaseSyn(code);
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isOverloadSyn(code, text))                                           mechanicCounters[V_OVERLOAD]->increaseSyn(code);
    if(isSecretSyn(code, referencedTags))                                   mechanicCounters[V_SECRET]->increaseSyn(code);
    if(isFreezeEnemySyn(code, referencedTags, text))                        mechanicCounters[V_FREEZE_ENEMY]->increaseSyn(code);
    if(isDiscardSyn(code, text))                                            mechanicCounters[V_DISCARD]->increaseSyn(code);
    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    else if(isDeathrattleGoodAllSyn(code))                                  mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increaseSyn(code);
    if(isBattlecrySyn(code, referencedTags))                                mechanicCounters[V_BATTLECRY]->increaseSyn(code);
    if(isSilenceOwnSyn(code, mechanics))                                    mechanicCounters[V_SILENCE]->increaseSyn(code);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_TAUNT_GIVER]->increaseSyn(code);
    if(isTokenSyn(code, text))                                              mechanicCounters[V_TOKEN]->increaseSyn(code);
    if(isTokenCardSyn(code, text))                                          mechanicCounters[V_TOKEN_CARD]->increaseSyn(code);
    if(isComboSyn(code, referencedTags, cost))                              mechanicCounters[V_COMBO]->increaseSyn(code);
    if(isWindfuryMinionSyn(code))                                           mechanicCounters[V_WINDFURY_MINION]->increaseSyn(code);
    if(isAttackBuffSyn(code))                                               mechanicCounters[V_ATTACK_BUFF]->increaseSyn(code);
    if(isHealthBuffSyn(code))                                               mechanicCounters[V_HEALTH_BUFF]->increaseSyn(code);
    if(isReturnSyn(code, mechanics, cardType, text))                        mechanicCounters[V_RETURN]->increaseSyn(code);
    if(isStealthSyn(code))                                                  mechanicCounters[V_STEALTH]->increaseSyn(code);
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   mechanicCounters[V_SPELL_DAMAGE]->increaseSyn(code);
    if(isEvolveSyn(code))                                                   mechanicCounters[V_EVOLVE]->increaseSyn(code);
    if(isRestoreTargetMinionSyn(code))                                      mechanicCounters[V_RESTORE_TARGET_MINION]->increaseSyn(code);
    if(isRestoreFriendlyHeroSyn(code))                                      mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increaseSyn(code);
    if(isRestoreFriendlyMinionSyn(code))                                    mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increaseSyn(code);
    if(isLifestealMinionSyn(code))                                          mechanicCounters[V_LIFESTEAL_MINION]->increaseSyn(code);
    if(isArmorSyn(code))                                                    mechanicCounters[V_ARMOR]->increaseSyn(code);
    if(isDivineShieldSyn(code))                                             mechanicCounters[V_DIVINE_SHIELD]->increaseSyn(code);
    else if(isDivineShieldAllSyn(code))                                     mechanicCounters[V_DIVINE_SHIELD_ALL]->increaseSyn(code);
    if(isEnrageMinionSyn(code))                                             mechanicCounters[V_ENRAGED_MINION]->increaseSyn(code);
    else if(isEnrageAllSyn(code, text))                                     mechanicCounters[V_ENRAGED_ALL]->increaseSyn(code);
}


void SynergyHandler::updateStatsCards(DeckCard &deckCard)
{
    if(deckCard.getType() != MINION)    return;

    //Stats
    QString code = deckCard.getCode();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();

    costMinions.appendStatValue(false, deckCard.getCost(), code);
    attackMinions.appendStatValue(false, attack, code);
    healthMinions.appendStatValue(false, health, code);

    //Synergies
    QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.statKind)
        {
            case V_COST:
                if(statSyn.isGen)   costMinions.appendStatValue(false, statSyn.statValue, code);
                else                costMinions.updateStatsMapSyn(statSyn, code);
            break;
            case V_ATTACK:
                if(statSyn.isGen)   attackMinions.appendStatValue(false, statSyn.statValue, code);
                else                attackMinions.updateStatsMapSyn(statSyn, code);
            break;
            case V_HEALTH:
                if(statSyn.isGen)   healthMinions.appendStatValue(false, statSyn.statValue, code);
                else                healthMinions.updateStatsMapSyn(statSyn, code);
            break;
        }
    }

//    qDebug()<<"*****COST MAP*****";
//    costMinions.qDebugContents();
//    qDebug()<<"*****ATTACK MAP*****";
//    attackMinions.qDebugContents();
//    qDebug()<<"*****HEALTH MAP*****";
//    healthMinions.qDebugContents();
}


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QStringList &mechanicIcons)
{
    getCardTypeSynergies(deckCard, synergies);
    getRaceSynergies(deckCard, synergies);
    getMechanicSynergies(deckCard, synergies, mechanicIcons);
    getDirectLinkSynergies(deckCard, synergies);
    getStatsCardsSynergies(deckCard, synergies);
}


void SynergyHandler::getCardTypeSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    //Evita mostrar spellSyn cards en cada hechizo que veamos
//    if(cardType == SPELL || isSpellGen(code))   cardTypeCounters[V_SPELL]->insertSynCards(synergies);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergies);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);
    }
    else if(isWeaponGen(code, text))            cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);


    if(isSpellSyn(code, text))                  cardTypeCounters[V_SPELL]->insertCards(synergies);
    if(isWeaponSyn(code))                       cardTypeCounters[V_WEAPON]->insertCards(synergies);
    else if(isWeaponAllSyn(code, text))         cardTypeCounters[V_WEAPON_ALL]->insertCards(synergies);
}


void SynergyHandler::getRaceSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardRace cardRace = deckCard.getRace();

    if(cardRace == MURLOC)
    {
        raceCounters[V_MURLOC]->insertSynCards(synergies);
        raceCounters[V_MURLOC_ALL]->insertSynCards(synergies);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->insertSynCards(synergies);
    if(cardRace == DEMON)
    {
        raceCounters[V_DEMON]->insertSynCards(synergies);
        raceCounters[V_DEMON_ALL]->insertSynCards(synergies);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->insertSynCards(synergies);
    if(cardRace == MECHANICAL)
    {
        raceCounters[V_MECHANICAL]->insertSynCards(synergies);
        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergies);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergies);
    if(cardRace == ELEMENTAL)
    {
        raceCounters[V_ELEMENTAL]->insertSynCards(synergies);
        raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergies);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergies);
    if(cardRace == BEAST)
    {
        raceCounters[V_BEAST]->insertSynCards(synergies);
        raceCounters[V_BEAST_ALL]->insertSynCards(synergies);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->insertSynCards(synergies);
    if(cardRace == TOTEM)
    {
        raceCounters[V_TOTEM]->insertSynCards(synergies);
        raceCounters[V_TOTEM_ALL]->insertSynCards(synergies);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->insertSynCards(synergies);
    if(cardRace == PIRATE)
    {
        raceCounters[V_PIRATE]->insertSynCards(synergies);
        raceCounters[V_PIRATE_ALL]->insertSynCards(synergies);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->insertSynCards(synergies);
    if(cardRace == DRAGON)
    {
        raceCounters[V_DRAGON]->insertSynCards(synergies);
        raceCounters[V_DRAGON_ALL]->insertSynCards(synergies);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->insertSynCards(synergies);

    if(isMurlocSyn(code))                   raceCounters[V_MURLOC]->insertCards(synergies);
    else if(isMurlocAllSyn(code, text))     raceCounters[V_MURLOC_ALL]->insertCards(synergies);
    if(isDemonSyn(code))                    raceCounters[V_DEMON]->insertCards(synergies);
    else if(isDemonAllSyn(code, text))      raceCounters[V_DEMON_ALL]->insertCards(synergies);
    if(isMechSyn(code))                     raceCounters[V_MECHANICAL]->insertCards(synergies);
    else if(isMechAllSyn(code, text))       raceCounters[V_MECHANICAL_ALL]->insertCards(synergies);
    if(isElementalSyn(code))                raceCounters[V_ELEMENTAL]->insertCards(synergies);
    else if(isElementalAllSyn(code, text))  raceCounters[V_ELEMENTAL_ALL]->insertCards(synergies);
    if(isBeastSyn(code))                    raceCounters[V_BEAST]->insertCards(synergies);
    else if(isBeastAllSyn(code, text))      raceCounters[V_BEAST_ALL]->insertCards(synergies);
    if(isTotemSyn(code))                    raceCounters[V_TOTEM]->insertCards(synergies);
    else if(isTotemAllSyn(code, text))      raceCounters[V_TOTEM_ALL]->insertCards(synergies);
    if(isPirateSyn(code))                   raceCounters[V_PIRATE]->insertCards(synergies);
    else if(isPirateAllSyn(code, text))     raceCounters[V_PIRATE_ALL]->insertCards(synergies);
    if(isDragonSyn(code, text))             raceCounters[V_DRAGON]->insertCards(synergies);
    else if(isDragonAllSyn(code))           raceCounters[V_DRAGON_ALL]->insertCards(synergies);
}


void SynergyHandler::getMechanicSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QStringList &mechanicIcons)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();
    bool addRestoreIcon = false;

    if(isDiscoverDrawGen(code, mechanics, referencedTags, text))
    {
        mechanicIcons.append(":/Images/drawMechanic.png");
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->insertSynCards(synergies);
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergies);
        mechanicIcons.append(":/Images/tauntMechanic.png");
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergies);
        mechanicIcons.append(":/Images/tauntMechanic.png");
    }
    if(isAoeGen(code, text))
    {
        mechanicCounters[V_AOE]->insertSynCards(synergies);
        mechanicIcons.append(":/Images/aoeMechanic.png");
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->insertSynCards(synergies);
        mechanicIcons.append(":/Images/pingMechanic.png");
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons.append(":/Images/damageMechanic.png");
    }
    if(isDestroyGen(code, mechanics, text))
    {
        mechanicIcons.append(":/Images/destroyMechanic.png");
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons.append(":/Images/reachMechanic.png");
    }
    if(isArmorGen(code, text))
    {
        mechanicCounters[V_ARMOR]->insertSynCards(synergies);
        addRestoreIcon = true;
    }
    if(isRestoreFriendlyHeroGen(code, mechanics, text))
    {
        mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertSynCards(synergies);
        addRestoreIcon = true;
    }
    if(addRestoreIcon)  mechanicIcons.append(":/Images/restoreMechanic.png");
    if(isRestoreTargetMinionGen(code, text))                    mechanicCounters[V_RESTORE_TARGET_MINION]->insertSynCards(synergies);
    if(isRestoreFriendlyMinionGen(code, text))                  mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertSynCards(synergies);
    if(isLifestealMinon(code, mechanics, cardType))             mechanicCounters[V_LIFESTEAL_MINION]->insertSynCards(synergies);
    if(isJadeGolemGen(code, mechanics, referencedTags))         mechanicCounters[V_JADE_GOLEM]->insertCards(synergies);//Sinergias gen-gen
    if(isOverload(code))                                        mechanicCounters[V_OVERLOAD]->insertSynCards(synergies);
    if(isSecretGen(code, mechanics))                            mechanicCounters[V_SECRET]->insertSynCards(synergies);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text)) mechanicCounters[V_FREEZE_ENEMY]->insertSynCards(synergies);
    if(isDiscardGen(code, text))                                mechanicCounters[V_DISCARD]->insertSynCards(synergies);
    if(isDeathrattleMinion(code, mechanics, cardType))          mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergies);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType)) mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertSynCards(synergies);
    if(isBattlecryMinion(code, mechanics, cardType))            mechanicCounters[V_BATTLECRY]->insertSynCards(synergies);
    if(isSilenceOwnGen(code, mechanics, referencedTags))        mechanicCounters[V_SILENCE]->insertSynCards(synergies);
    if(isTauntGiverGen(code))                                   mechanicCounters[V_TAUNT_GIVER]->insertSynCards(synergies);
    if(isTokenGen(code, text))                                  mechanicCounters[V_TOKEN]->insertSynCards(synergies);
    if(isTokenCardGen(code, cost))                              mechanicCounters[V_TOKEN_CARD]->insertSynCards(synergies);
    if(isComboGen(code, mechanics))                             mechanicCounters[V_COMBO]->insertSynCards(synergies);
    if(isWindfuryMinion(code, mechanics, cardType))             mechanicCounters[V_WINDFURY_MINION]->insertSynCards(synergies);
    if(isAttackBuffGen(code, text))                             mechanicCounters[V_ATTACK_BUFF]->insertSynCards(synergies);
    if(isHealthBuffGen(code, text))                             mechanicCounters[V_HEALTH_BUFF]->insertSynCards(synergies);
    if(isReturnGen(code, text))                                 mechanicCounters[V_RETURN]->insertSynCards(synergies);
    if(isStealthGen(code, mechanics))                           mechanicCounters[V_STEALTH]->insertSynCards(synergies);
    if(isSpellDamageGen(code))                                  mechanicCounters[V_SPELL_DAMAGE]->insertSynCards(synergies);
    if(isEvolveGen(code, text))                                 mechanicCounters[V_EVOLVE]->insertSynCards(synergies);
    if(isDivineShield(code, mechanics))
    {
        mechanicCounters[V_DIVINE_SHIELD]->insertSynCards(synergies);
        mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergies);
    }
    else if(isDivineShieldGen(code, referencedTags))            mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergies);
    if(isEnrageMinion(code, mechanics))
    {
        mechanicCounters[V_ENRAGED_MINION]->insertSynCards(synergies);
        mechanicCounters[V_ENRAGED_ALL]->insertSynCards(synergies);
    }
    if(isEnrageGen(code, referencedTags))                       mechanicCounters[V_ENRAGED_ALL]->insertSynCards(synergies);

    if(isTauntSyn(code))                                        mechanicCounters[V_TAUNT]->insertCards(synergies);
    else if(isTauntAllSyn(code))                                mechanicCounters[V_TAUNT_ALL]->insertCards(synergies);
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergies);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergies);
    if(isOverloadSyn(code, text))                               mechanicCounters[V_OVERLOAD]->insertCards(synergies);
    if(isSecretSyn(code, referencedTags))                       mechanicCounters[V_SECRET]->insertCards(synergies);
    if(isFreezeEnemySyn(code, referencedTags, text))            mechanicCounters[V_FREEZE_ENEMY]->insertCards(synergies);
    if(isDiscardSyn(code, text))                                mechanicCounters[V_DISCARD]->insertCards(synergies);
    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergies);
    else if(isDeathrattleGoodAllSyn(code))                      mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertCards(synergies);
    if(isBattlecrySyn(code, referencedTags))                    mechanicCounters[V_BATTLECRY]->insertCards(synergies);
    if(isSilenceOwnSyn(code, mechanics))                        mechanicCounters[V_SILENCE]->insertCards(synergies);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))      mechanicCounters[V_TAUNT_GIVER]->insertCards(synergies);
    if(isTokenSyn(code, text))                                  mechanicCounters[V_TOKEN]->insertCards(synergies);
    if(isTokenCardSyn(code, text))                              mechanicCounters[V_TOKEN_CARD]->insertCards(synergies);
    if(isComboSyn(code, referencedTags, cost))                  mechanicCounters[V_COMBO]->insertCards(synergies);
    if(isWindfuryMinionSyn(code))                               mechanicCounters[V_WINDFURY_MINION]->insertCards(synergies);
    if(isAttackBuffSyn(code))                                   mechanicCounters[V_ATTACK_BUFF]->insertCards(synergies);
    if(isHealthBuffSyn(code))                                   mechanicCounters[V_HEALTH_BUFF]->insertCards(synergies);
    if(isReturnSyn(code, mechanics, cardType, text))            mechanicCounters[V_RETURN]->insertCards(synergies);
    if(isStealthSyn(code))                                      mechanicCounters[V_STEALTH]->insertCards(synergies);
    if(isSpellDamageSyn(code, mechanics, cardType, text))       mechanicCounters[V_SPELL_DAMAGE]->insertCards(synergies);
    if(isEvolveSyn(code))                                       mechanicCounters[V_EVOLVE]->insertCards(synergies);
    if(isRestoreTargetMinionSyn(code))                          mechanicCounters[V_RESTORE_TARGET_MINION]->insertCards(synergies);
    if(isRestoreFriendlyHeroSyn(code))                          mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertCards(synergies);
    if(isRestoreFriendlyMinionSyn(code))                        mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertCards(synergies);
    if(isArmorSyn(code))                                        mechanicCounters[V_ARMOR]->insertCards(synergies);
    if(isLifestealMinionSyn(code))                              mechanicCounters[V_LIFESTEAL_MINION]->insertCards(synergies);
    if(isDivineShieldSyn(code))                                 mechanicCounters[V_DIVINE_SHIELD]->insertCards(synergies);
    else if(isDivineShieldAllSyn(code))                         mechanicCounters[V_DIVINE_SHIELD_ALL]->insertCards(synergies);
    if(isEnrageMinionSyn(code))                                 mechanicCounters[V_ENRAGED_MINION]->insertCards(synergies);
    else if(isEnrageAllSyn(code, text))                         mechanicCounters[V_ENRAGED_ALL]->insertCards(synergies);
}


void SynergyHandler::getDirectLinkSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();

    if(directLinks.contains(code))
    {
        QList<QString> linkCodes = directLinks[code];

        for(const QString linkCode: linkCodes)
        {
            if(cardTypeCounters[V_MINION]->insertCode(linkCode, synergies)){}
            else if(cardTypeCounters[V_WEAPON]->insertCode(linkCode, synergies)){}
            else cardTypeCounters[V_SPELL]->insertCode(linkCode, synergies);
        }
    }
}


void SynergyHandler::getStatsCardsSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    if(deckCard.getType() != MINION)    return;

    //Stats
    QString code = deckCard.getCode();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();

    costMinions.insertCards(true, deckCard.getCost(), synergies);
    attackMinions.insertCards(true, attack, synergies);
    healthMinions.insertCards(true, health, synergies);

    //Synergies
    QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.statKind)
        {
            case V_COST:
                if(statSyn.isGen)   costMinions.insertCards(true, statSyn.statValue, synergies);
                else                costMinions.insertStatCards(statSyn, synergies);
            break;
            case V_ATTACK:
                if(statSyn.isGen)   attackMinions.insertCards(true, statSyn.statValue, synergies);
                else                attackMinions.insertStatCards(statSyn, synergies);
            break;
            case V_HEALTH:
                if(statSyn.isGen)   healthMinions.insertCards(true, statSyn.statValue, synergies);
                else                healthMinions.insertStatCards(statSyn, synergies);
            break;
        }
    }
}


void SynergyHandler::testSynergies()
{
    initSynergyCodes();
    int num = 0;
    for(const QString &code: Utility::getStandardCodes())
    {
        DeckCard deckCard(code);
        CardType cardType = deckCard.getType();
        QString text = Utility::cardEnTextFromCode(code).toLower();
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
        QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
        if(
//                text.contains("lifesteal") &&
//                !referencedTags.contains(QJsonValue("LIFESTEAL")) &&
//                !mechanics.contains(QJsonValue("LIFESTEAL"))
//                (text.contains("deal") && text.contains("1 damage") &&
//                            !text.contains("enemy") && !text.contains("random") && !text.contains("hero"))
//                isMurlocAllSyn(code, text)
//                isDemonAllSyn(code, text)
//                isMechAllSyn(code, text)
//                isElementalAllSyn(code, text)
//                isBeastAllSyn(code, text)
//                isTotemAllSyn(code, text)
//                isPirateAllSyn(code, text)
                isDragonSyn(code, text)
            )
        {
            qDebug()<<++num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;
        }
    }
}


void SynergyHandler::debugSynergiesSet(const QString &set)
{
    initSynergyCodes();
    int num = 0;
    for(const QString &code: Utility::getSetCodes(set))
    {
        debugSynergiesCode(code, ++num);
    }
}


void SynergyHandler::debugSynergiesCode(const QString &code, int num)
{
    QStringList mec, syn, manual;

    if(synergyCodes.contains(code)) manual<<synergyCodes[code];
    DeckCard deckCard(code);
    CardType cardType = deckCard.getType();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    if(isMurlocAllSyn(code, text))      syn<<"murlocAllSyn";
    if(isDemonAllSyn(code, text))       syn<<"demonAllSyn";
    if(isMechAllSyn(code, text))        syn<<"mechAllSyn";
    if(isElementalAllSyn(code, text))   syn<<"elementalAllSyn";
    if(isBeastAllSyn(code, text))       syn<<"beastAllSyn";
    if(isTotemAllSyn(code, text))       syn<<"totemAllSyn";
    if(isPirateAllSyn(code, text))      syn<<"pirateAllSyn";
    if(isDragonSyn(code, text))         syn<<"dragonSyn";

    if(isWeaponGen(code, text))         mec<<"weaponGen";
    if(isSpellSyn(code, text))          syn<<"spellSyn";
    if(isWeaponAllSyn(code, text))      syn<<"weaponAllSyn";

    if(isDiscoverDrawGen(code, mechanics, referencedTags, text))            mec<<"discover o drawGen o toYourHandGen";
    if(isAoeGen(code, text))                                                mec<<"aoeGen";
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mec<<"pingGen";
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"damageMinionsGen";
    if(isDestroyGen(code, mechanics, text))                                 mec<<"destroyGen";
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"reachGen";
    if(isOverload(code))                                                    mec<<"overload";
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mec<<"jadeGolemGen";
    if(isSecretGen(code, mechanics))                                        mec<<"secretGen";
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mec<<"freezeEnemyGen";
    if(isDiscardGen(code, text))                                            mec<<"discardGen";
    if(isDeathrattleMinion(code, mechanics, cardType))                      mec<<"deathrattle o deathrattleOpponent";
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mec<<"deathrattle o deathrattleGen";
    if(isBattlecryMinion(code, mechanics, cardType))                        mec<<"battlecry";
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mec<<"silenceOwnGen";
    if(isTokenGen(code, text))                                              mec<<"tokenGen";
    if(isWindfuryMinion(code, mechanics, cardType))                         mec<<"windfury";
    if(isAttackBuffGen(code, text))                                         mec<<"attackBuffGen";
    if(isHealthBuffGen(code, text))                                         mec<<"healthBuffGen";
    if(isReturnGen(code, text))                                             mec<<"returnGen";
    if(isStealthGen(code, mechanics))                                       mec<<"stealthGen";
    if(isSpellDamageGen(code))                                              mec<<"spellDamageGen";
    if(isEvolveGen(code, text))                                             mec<<"evolveGen";
    if(isRestoreTargetMinionGen(code, text))                                mec<<"restoreTargetMinionGen";
    if(isRestoreFriendlyHeroGen(code, mechanics, text))                     mec<<"restoreFriendlyHeroGen o lifesteal";
    if(isRestoreFriendlyMinionGen(code, text))                              mec<<"restoreFriendlyMinionGen";
    if(isArmorGen(code, text))                                              mec<<"armorGen";
    if(isLifestealMinon(code, mechanics, cardType))                         mec<<"lifesteal";
    if(isTaunt(code, mechanics))                                            mec<<"taunt";
    else if(isTauntGen(code, referencedTags))                               mec<<"tauntGen";
    if(isDivineShield(code, mechanics))                                     mec<<"divineShield";
    else if(isDivineShieldGen(code, referencedTags))                        mec<<"divineShieldGen";
    if(isEnrageMinion(code, mechanics))                                     mec<<"enrageMinion";
    if(isEnrageGen(code, referencedTags))                                   mec<<"enrageGen";
    if(isComboGen(code, mechanics))                                         mec<<"comboGen";


    if(isOverloadSyn(code, text))                                           syn<<"overloadSyn";
    if(isSecretSyn(code, referencedTags))                                   syn<<"secretSyn";
    if(isFreezeEnemySyn(code, referencedTags, text))                        syn<<"freezeEnemySyn";
    if(isDiscardSyn(code, text))                                            syn<<"discardSyn";
    if(isBattlecrySyn(code, referencedTags))                                syn<<"battlecrySyn";
    if(isSilenceOwnSyn(code, mechanics))                                    syn<<"silenceOwnSyn";
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  syn<<"tauntGiverSyn";
    if(isTokenSyn(code, text))                                              syn<<"tokenSyn";
    if(isReturnSyn(code, mechanics, cardType, text))                        syn<<"returnSyn";
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   syn<<"spellDamageSyn";
    if(isEnrageAllSyn(code, text))                                          syn<<"enrageAllSyn";
    if(isTokenCardSyn(code, text))                                          syn<<"tokenCardSyn";

    qDebug()<<num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;

    if(!manual.isEmpty())   qDebug()<<"-----MANUAL: "<<manual;
    else                    qDebug()<<mec<<syn;
}


//Increase counters
bool SynergyHandler::isSpellGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("spellGen");
    return false;
}
bool SynergyHandler::isWeaponGen(const QString &code, const QString &text)
{
    //NO TEST
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponGen");
    }
    else if(text.contains("equip "))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isMurlocGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("murlocGen");
    return false;
}
bool SynergyHandler::isDemonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("demonGen");
    return false;
}
bool SynergyHandler::isMechGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("mechGen");
    return false;
}
bool SynergyHandler::isElementalGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("elementalGen");
    return false;
}
bool SynergyHandler::isBeastGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("beastGen");
    return false;
}
bool SynergyHandler::isTotemGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("totemGen");
    return false;
}
bool SynergyHandler::isPirateGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("pirateGen");
    return false;
}
bool SynergyHandler::isDragonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("dragonGen");
    return false;
}
bool SynergyHandler::isDiscoverDrawGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text)
{
    //TEST
    //&& (text.contains("draw") || text.contains("discover") || (text.contains("to") && text.contains("your") && text.contains("hand")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discover") ||
                synergyCodes[code].contains("drawGen") ||
                synergyCodes[code].contains("toYourHandGen");
    }
    else if(mechanics.contains(QJsonValue("DISCOVER")) || referencedTags.contains(QJsonValue("DISCOVER")))
    {
        return true;
    }
    else
    {
        return  text.contains("draw") ||
                (text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return"));
    }
}
bool SynergyHandler::isTaunt(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("taunt");
    }
    else if(mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGen");
    }
    else if(referencedTags.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
//bool SynergyHandler::isRestoreGen(const QString &code)
//{
//    if(synergyCodes.contains(code))
//    {
//        return synergyCodes[code].contains("restoreGen");
//    }
//    else
//    {
//        QString text = Utility::cardEnTextFromCode(code).toLower();
//        return  text.contains("restore");
//    }
//}
//El minimo aoe es 3 pings a esbirros enemigos
bool SynergyHandler::isAoeGen(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeGen");
    }
    else
    {
        return  (text.contains("all") || text.contains("adjacent")) &&
                    (text.contains("damage") ||
                        (text.contains("destroy") && text.contains("minions"))
                     );
    }
}
//El ping debe poder seleccionar a un enemigo
bool SynergyHandler::isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingGen");
    }
    //Anything that deals damage
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("hero"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    else if(attack != 1)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool SynergyHandler::isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                              const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("reachGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
            !text.contains("1 damage") && !text.contains("minion") && !text.contains("random") && !text.contains("to your hero"))
    {
        return true;
    }
    //Hero attack
    else if(text.contains("+") && text.contains("give") && text.contains("attack") &&
            (text.contains("hero") || text.contains("character")))
    {
        return true;
    }
    else if(attack < 2)  return false;
    //Charge and stealth minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")) ||
            mechanics.contains(QJsonValue("STEALTH")) || referencedTags.contains(QJsonValue("STEALTH")))
        {
            return !text.contains("gain <b>charge</b>") && !text.contains("can't attack heroes");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool SynergyHandler::isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                      const QString &text, const CardType &cardType, int attack)
{
    //TEST
//    (text.contains("damage") && text.contains("deal") &&
//                !text.contains("1 damage") && !text.contains("all") && !text.contains("hero")) &&
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageMinionsGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
            !text.contains("1 damage") && !text.contains("all") && !text.contains("hero"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    //Hero attack
    else if(text.contains("+") && text.contains("give") && text.contains("attack") &&
            (text.contains("hero") || text.contains("character")))
    {
        return true;
    }
    else if(attack == 1)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool SynergyHandler::isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroyGen");
    }
    else if(text.contains("destroy") && text.contains("minion") &&
            !text.contains("all"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    return false;
}
bool SynergyHandler::isEnrageMinion(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //text.contains("takes") && text.contains("damage")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageMinion");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEnrageGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageGen");
    }
    else if(referencedTags.contains(QJsonValue("ENRAGED")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOverload(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("overload");
    }
    else
    {
        int overload = Utility::getCardAttribute(code, "overload").toInt();
        return overload > 0;
    }
}
bool SynergyHandler::isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("jadeGolemGen");
    }
    else if(mechanics.contains(QJsonValue("JADE_GOLEM")) || referencedTags.contains(QJsonValue("JADE_GOLEM")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSecretGen(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secretGen");
    }
    else if(mechanics.contains(QJsonValue("SECRET")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isFreezeEnemyGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                 const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeEnemyGen");
    }
    else if(mechanics.contains(QJsonValue("FREEZE")))
    {
        return true;
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return !text.contains("frozen");
    }
    return false;
}
bool SynergyHandler::isDiscardGen(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discardGen");
    }
    else if(text.contains("discard") && text.contains("random card"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattle") || synergyCodes[code].contains("deathrattleOpponent");
    }
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                          const CardType &cardType)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return (synergyCodes[code].contains("deathrattle") || synergyCodes[code].contains("deathrattleGen")) &&
                !synergyCodes[code].contains("silenceOwnSyn") &&
                !synergyCodes[code].contains("deathrattleOpponent");
    }
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isBattlecryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecry");
    }
    else if(mechanics.contains(QJsonValue("BATTLECRY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilenceOwnGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silenceOwnGen");
    }
    else if(mechanics.contains(QJsonValue("SILENCE")))
    {
        return true;
    }
    else if(referencedTags.contains(QJsonValue("SILENCE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGiverGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverGen");
    }
    return false;
}
bool SynergyHandler::isTokenGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("2/2")) && !text.contains("opponent")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenGen");
    }
    else if((text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("0/2") || text.contains("0/1"))
            && text.contains("summon") && !text.contains("opponent"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenCardGen(const QString &code, int cost)
{
    //Incluimos cartas que en conjunto permitan jugar 2+ cartas de coste 0/1/2
    //TEST
    //text.contains("to") && text.contains("your") && text.contains("hand")
    if(cost == 0)   return true;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenCardGen");
    }
    return false;
}
bool SynergyHandler::isComboGen(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboGen");
    }
    else if(mechanics.contains(QJsonValue("COMBO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isWindfuryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("windfury");
    }
    else if(mechanics.contains(QJsonValue("WINDFURY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isAttackBuffGen(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+")
//    && (text.contains("minion") || text.contains("character"))
//    && (text.contains("attack") || text.contains("/+")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackBuffGen");
    }
    else if(text.contains("+")
            && (text.contains("give") || text.contains("have"))
            && (text.contains("minion") || text.contains("character"))
            && (text.contains("attack") || text.contains("/+"))
            &&/* !text.contains("hand") && *//*!text.contains("random") && */!text.contains("c'thun"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHealthBuffGen(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+")
//    && (text.contains("minion") || text.contains("character"))
//    && (text.contains("health") || text.contains("/+")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("healthBuffGen");
    }
    else if(text.contains("+")
            && (text.contains("give") || text.contains("have"))
            && (text.contains("minion") || text.contains("character"))
            && (text.contains("health") || text.contains("/+"))
            && /*!text.contains("hand") && *//*!text.contains("random") && */!text.contains("c'thun"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isReturnGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("return a friendly minion")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("returnGen");
    }
    else if(text.contains("return a friendly minion"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isStealthGen(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("STEALTH")) ||  referencedTags.contains(QJsonValue("STEALTH")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("stealthGen");
    }
    else if(mechanics.contains(QJsonValue("STEALTH")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDivineShield(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DIVINE_SHIELD")) ||  referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShield");
    }
    else if(mechanics.contains(QJsonValue("DIVINE_SHIELD")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDivineShieldGen(const QString &code, const QJsonArray &referencedTags)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("STEALTH")) ||  referencedTags.contains(QJsonValue("STEALTH")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldGen");
    }
    else if(referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRestoreTargetMinionGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("restore") // No hace falta "heal", restore lo cubre todo excepto Prophet Velen y Wickerflame Burnbristle
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreTargetMinionGen");
    }
    else if(text.contains("restore") && !text.contains("friendly") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHeroGen") || synergyCodes[code].contains("lifesteal");
    }
    else if(mechanics.contains(QJsonValue("LIFESTEAL")))
    {
        return true;
    }
    else if(text.contains("restore"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyMinionGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyMinionGen");
    }
    else if(text.contains("restore") && text.contains("friendly") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isArmorGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("armor")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armorGen");
    }
    else if(text.contains("armor"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isLifestealMinon(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifesteal");
    }
    else if(mechanics.contains(QJsonValue("LIFESTEAL")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSpellDamageGen(const QString &code)
{
    //TEST
    //&& text.contains("spell") && text.contains("damage")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellDamageGen");
    }
    else
    {
        int spellDamage = Utility::getCardAttribute(code, "spellDamage").toInt();
        return spellDamage > 0;
    }
}
bool SynergyHandler::isEvolveGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("transform") && text.contains("cost") && text.contains("more")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("evolveGen");
    }
    else if(text.contains("transform") && text.contains("cost") && text.contains("more"))
    {
        return true;
    }
    return false;
}


//Synergy items
bool SynergyHandler::isSpellSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellSyn");
    }
    else
    {
        return  text.contains("spell") && (text.contains("you cast") || text.contains("cost"));
    }
}
bool SynergyHandler::isWeaponSyn(const QString &code)
{
    //NO TEST
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponSyn");
    }
    return false;
}
bool SynergyHandler::isWeaponAllSyn(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("weapon")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponAllSyn");
    }
    else
    {
        return text.contains("weapon") && !text.contains("opponent's weapon");
    }
}
bool SynergyHandler::isMurlocSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("murlocSyn");
    }
    return false;
}
bool SynergyHandler::isMurlocAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("murlocAllSyn");
    }
    else
    {
        return text.contains("murloc");
    }
}
bool SynergyHandler::isDemonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonSyn");
    }
    return false;
}
bool SynergyHandler::isDemonAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonAllSyn");
    }
    else
    {
        return text.contains("demon");
    }
}
bool SynergyHandler::isMechSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechSyn");
    }
    return false;
}
bool SynergyHandler::isMechAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechAllSyn");
    }
    else
    {
        return text.contains("mech");
    }
}
bool SynergyHandler::isElementalSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalSyn");
    }
    return false;
}
bool SynergyHandler::isElementalAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalAllSyn");
    }
    else
    {
        return text.contains("elemental");
    }
}
bool SynergyHandler::isBeastSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastSyn");
    }
    return false;
}
bool SynergyHandler::isBeastAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastAllSyn");
    }
    else
    {
        return text.contains("beast");
    }
}
bool SynergyHandler::isTotemSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemSyn");
    }
    return false;
}
bool SynergyHandler::isTotemAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemAllSyn");
    }
    else
    {
        return text.contains("totem");
    }
}
bool SynergyHandler::isPirateSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateSyn");
    }
    return false;
}
bool SynergyHandler::isPirateAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateAllSyn");
    }
    else
    {
        return text.contains("pirate");
    }
}
bool SynergyHandler::isDragonSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonSyn");
    }
    else
    {
        return text.contains("dragon");//Con los dragones la synergia por defecto es con dragones en mano
    }
}
bool SynergyHandler::isDragonAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonAllSyn");
    }
    return false;
}
bool SynergyHandler::isEnrageMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageMinionSyn");
    }
    return false;
}
bool SynergyHandler::isEnrageAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageAllSyn");
    }
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("enemy") && !text.contains("random") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOverloadSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("overloadSyn");
    }
    else if(text.contains("overload"))
    {
        int overload = Utility::getCardAttribute(code, "overload").toInt();
        return overload == 0;
    }
    return false;
}
bool SynergyHandler::isPingSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingSyn");
    }
    return false;
}
bool SynergyHandler::isAoeSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeSyn");
    }
    return false;
}
bool SynergyHandler::isTauntSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntSyn");
    }
    return false;
}
bool SynergyHandler::isTauntAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntAllSyn");
    }
    return false;
}
bool SynergyHandler::isSecretSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secretSyn");
    }
    else if(referencedTags.contains(QJsonValue("SECRET")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isFreezeEnemySyn(const QString &code, const QJsonArray &referencedTags, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeEnemySyn");
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return text.contains("frozen");
    }
    return false;
}
bool SynergyHandler::isDiscardSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discardSyn");
    }
    else if(text.contains("discard") && !text.contains("random card"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleSyn");
    }
    return false;
}
bool SynergyHandler::isDeathrattleGoodAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleGoodAllSyn");
    }
    return false;
}
bool SynergyHandler::isBattlecrySyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecrySyn");
    }
    else if(referencedTags.contains(QJsonValue("BATTLECRY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilenceOwnSyn(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silenceOwnSyn");
    }
    else if(mechanics.contains(QJsonValue("CANT_ATTACK")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGiverSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverSyn") || synergyCodes[code].contains("deathrattleOpponent");
    }
    else if(mechanics.contains(QJsonValue("CANT_ATTACK")))
    {
        return true;
    }
    else if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenSyn(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+") && (text.contains("minions") || text.contains("characters"))
//    && !text.contains("hand")
//    || (text.contains("control") && text.contains("least") && text.contains("minions")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenSyn");
    }
    else if(text.contains("+")
            && (text.contains("minions") || text.contains("characters"))
            && !text.contains("hand"))
    {
        return true;
    }
    else if(text.contains("control") && text.contains("least") && text.contains("minions"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenCardSyn(const QString &code, const QString &text)
{
    //TEST
    //text.contains("play") && text.contains("card") && !text.contains("player")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenCardSyn");
    }
    else if(text.contains("play") && text.contains("card") && !text.contains("player"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isComboSyn(const QString &code, const QJsonArray &referencedTags, int cost)
{
    if(cost == 0)   return true;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboSyn") || synergyCodes[code].contains("tokenCardGen");
    }
    else if(referencedTags.contains(QJsonValue("COMBO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isWindfuryMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("windfuryMinionSyn");
    }
    return false;
}
bool SynergyHandler::isAttackBuffSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackBuffSyn");
    }
    return false;
}
bool SynergyHandler::isHealthBuffSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("healthBuffSyn");
    }
    return false;
}
bool SynergyHandler::isReturnSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text)
{
    //TEST
    //&& mechanics.contains(QJsonValue("BATTLECRY"))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("returnSyn");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("BATTLECRY")))
    {
        if(
                ((text.contains("summon") || text.contains("give") || text.contains("restore")) && !text.contains("opponent")) ||
                (text.contains("deal") && !(text.contains("your") && text.contains("hero"))) ||
                text.contains("swap") ||
                (text.contains("play") && !text.contains("played")) ||
                text.contains("discover") ||
                text.contains("draw") ||
                (text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return")) ||
                text.contains("destroy") ||
                text.contains("transform") ||
                (text.contains("take") && text.contains("control")) ||
                text.contains("freeze") ||
                text.contains("remove") ||
                text.contains("silence") ||
                text.contains("cast") ||
                text.contains("equip")
           )
        {
            return true;
        }
    }
    return false;
}
bool SynergyHandler::isStealthSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("stealthSyn");
    }
    return false;
}
bool SynergyHandler::isDivineShieldSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldSyn");
    }
    return false;
}
bool SynergyHandler::isDivineShieldAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldAllSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreTargetMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreTargetMinionSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyHeroSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHeroSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyMinionSyn");
    }
    return false;
}
bool SynergyHandler::isArmorSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armorSyn");
    }
    return false;
}
bool SynergyHandler::isLifestealMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifestealMinionSyn");
    }
    return false;
}
bool SynergyHandler::isSpellDamageSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text)
{
    //TEST
//    && (text.contains("all") || text.contains("adjacent")) && (text.contains("damage"))
//    && cardType == SPELL
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellDamageSyn");
    }
    else if(cardType != SPELL)  return false;
    else if(mechanics.contains(QJsonValue("SECRET")))   return false;
    else if(
            (text.contains("all") || text.contains("adjacent")) && (text.contains("damage"))
            && !text.contains("random")
           )
    {
            return true;
    }
    return false;
}
bool SynergyHandler::isEvolveSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("evolveSyn");
    }
    return false;
}

