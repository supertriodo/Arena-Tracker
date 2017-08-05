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

    manaCounter = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("manaCounter.png"));

    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("minionsCounter.png"));
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("spellsCounter.png"));
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("weaponsCounter.png"));
    cardTypeCounters[V_WEAPON_ALL] = new DraftItemCounter(this);

    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(this);
    raceCounters[V_BEAST] = new DraftItemCounter(this);
    raceCounters[V_MURLOC] = new DraftItemCounter(this);
    raceCounters[V_DRAGON] = new DraftItemCounter(this);

    raceCounters[V_PIRATE] = new DraftItemCounter(this);
    raceCounters[V_MECHANICAL] = new DraftItemCounter(this);
    raceCounters[V_DEMON] = new DraftItemCounter(this);
    raceCounters[V_TOTEM] = new DraftItemCounter(this);

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("drawMechanic.png"));
    mechanicCounters[V_TAUNT] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("tauntMechanic.png"));
    mechanicCounters[V_AOE] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("aoeMechanic.png"));

    mechanicCounters[V_PING] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("pingMechanic.png"));
    mechanicCounters[V_DAMAGE_DESTROY] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("damageMechanic.png"));
    mechanicCounters[V_REACH] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("reachMechanic.png"));

    mechanicCounters[V_OVERLOAD] = new DraftItemCounter(this);
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this);
    mechanicCounters[V_SECRET] = new DraftItemCounter(this);
    mechanicCounters[V_FREEZE] = new DraftItemCounter(this);
    mechanicCounters[V_DISCARD] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_BATTLECRY] = new DraftItemCounter(this);
    mechanicCounters[V_SILENCE] = new DraftItemCounter(this);
    mechanicCounters[V_TAUNT_GIVER] = new DraftItemCounter(this);
    mechanicCounters[V_TOKEN] = new DraftItemCounter(this);
    mechanicCounters[V_WINDFURY] = new DraftItemCounter(this);
    mechanicCounters[V_ATTACK_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_HEALTH_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_RETURN] = new DraftItemCounter(this);
    mechanicCounters[V_STEALTH] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_ENRAGED_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_ENRAGED_ALL] = new DraftItemCounter(this);

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
    qDebug()<<directLinks;
}


void SynergyHandler::clearLists(bool keepCounters)
{
    synergyCodes.clear();

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
}


void SynergyHandler::updateManaCounter(DeckCard &deckCard)
{
    manaCounter->increase(deckCard.getCost(), draftedCardsCount());
}


void SynergyHandler::updateRaceCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    CardRace cardRace = deckCard.getRace();

    if(cardRace == MURLOC || isMurlocGen(code))         raceCounters[V_MURLOC]->increase(code);
    if(cardRace == DEMON || isDemonGen(code))           raceCounters[V_DEMON]->increase(code);
    if(cardRace == MECHANICAL || isMechGen(code))       raceCounters[V_MECHANICAL]->increase(code);
    if(cardRace == ELEMENTAL || isElementalGen(code))   raceCounters[V_ELEMENTAL]->increase(code);
    if(cardRace == BEAST || isBeastGen(code))           raceCounters[V_BEAST]->increase(code);
    if(cardRace == TOTEM || isTotemGen(code))           raceCounters[V_TOTEM]->increase(code);
    if(cardRace == PIRATE || isPirateGen(code))         raceCounters[V_PIRATE]->increase(code);
    if(cardRace == DRAGON || isDragonGen(code))         raceCounters[V_DRAGON]->increase(code);

    if(isMurlocSyn(code))   raceCounters[V_MURLOC]->increaseSyn(code);
    if(isDemonSyn(code))    raceCounters[V_DEMON]->increaseSyn(code);
    if(isMechSyn(code))     raceCounters[V_MECHANICAL]->increaseSyn(code);
    if(isElementalSyn(code))raceCounters[V_ELEMENTAL]->increaseSyn(code);
    if(isBeastSyn(code))    raceCounters[V_BEAST]->increaseSyn(code);
    if(isTotemSyn(code))    raceCounters[V_TOTEM]->increaseSyn(code);
    if(isPirateSyn(code))   raceCounters[V_PIRATE]->increaseSyn(code);
    if(isDragonSyn(code))   raceCounters[V_DRAGON]->increaseSyn(code);
}


void SynergyHandler::updateCardTypeCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)               cardTypeCounters[V_SPELL]->increase(code);
    else if(isSpellGen(code))           cardTypeCounters[V_SPELL]->increase(code,false);
    if(cardType == MINION)              cardTypeCounters[V_MINION]->increase(code);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->increase(code);
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
    }
    else if(isWeaponGen(code, text))    cardTypeCounters[V_WEAPON_ALL]->increase(code);


    if(isSpellSyn(code))                cardTypeCounters[V_SPELL]->increaseSyn(code);
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

    if(isDiscoverDrawGen(code))                                             mechanicCounters[V_DISCOVER_DRAW]->increase(code);
    if(isTaunt(code, mechanics))                                            mechanicCounters[V_TAUNT]->increase(code);
    else if(isTauntGen(code, referencedTags))                               mechanicCounters[V_TAUNT]->increase();
    if(isAoeGen(code))                                                      mechanicCounters[V_AOE]->increase(code);
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mechanicCounters[V_PING]->increase(code);
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)
            || isDestroyGen(code))                                          mechanicCounters[V_DAMAGE_DESTROY]->increase(code);
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mechanicCounters[V_REACH]->increase(code);
    if(isOverload(code))                                                    mechanicCounters[V_OVERLOAD]->increase(code);
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isSecretGen(code, mechanics))                                        mechanicCounters[V_SECRET]->increase(code);
    if(isFreezeGen(code, mechanics, referencedTags, text))                  mechanicCounters[V_FREEZE]->increase(code);
    if(isDiscardGen(code, text))                                            mechanicCounters[V_DISCARD]->increase(code);
    if(isDeathrattleMinion(code, mechanics, cardType))                      mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags))               mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increase(code);
    if(isBattlecryMinion(code, mechanics, cardType))                        mechanicCounters[V_BATTLECRY]->increase(code);
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mechanicCounters[V_SILENCE]->increase(code);
    if(isTauntGiverGen(code))                                               mechanicCounters[V_TAUNT_GIVER]->increase(code);
    if(isTokenGen(code, text))                                              mechanicCounters[V_TOKEN]->increase(code);
    if(isWindfuryMinion(code, mechanics, cardType))                         mechanicCounters[V_WINDFURY]->increase(code);
    if(isAttackBuffGen(code, text))                                         mechanicCounters[V_ATTACK_BUFF]->increase(code);
    if(isHealthBuffGen(code, text))                                         mechanicCounters[V_HEALTH_BUFF]->increase(code);
    if(isReturnGen(code, text))                                             mechanicCounters[V_RETURN]->increase(code);
    if(isStealthGen(code, mechanics))                                       mechanicCounters[V_STEALTH]->increase(code);
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
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isOverloadSyn(code, text))                                           mechanicCounters[V_OVERLOAD]->increaseSyn(code);
    if(isSecretSyn(code, referencedTags))                                   mechanicCounters[V_SECRET]->increaseSyn(code);
    if(isFreezeSyn(code, referencedTags, text))                             mechanicCounters[V_FREEZE]->increaseSyn(code);
    if(isDiscardSyn(code, text))                                            mechanicCounters[V_DISCARD]->increaseSyn(code);
    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    else if(isDeathrattleGoodAllSyn(code))                                  mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increaseSyn(code);
    if(isBattlecrySyn(code, referencedTags))                                mechanicCounters[V_BATTLECRY]->increaseSyn(code);
    if(isSilenceOwnSyn(code, mechanics))                                    mechanicCounters[V_SILENCE]->increaseSyn(code);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_TAUNT_GIVER]->increaseSyn(code);
    if(isTokenSyn(code, text))                                              mechanicCounters[V_TOKEN]->increaseSyn(code);
    if(isWindfurySyn(code))                                                 mechanicCounters[V_WINDFURY]->increaseSyn(code);
    if(isAttackBuffSyn(code))                                               mechanicCounters[V_ATTACK_BUFF]->increaseSyn(code);
    if(isHealthBuffSyn(code))                                               mechanicCounters[V_HEALTH_BUFF]->increaseSyn(code);
    if(isReturnSyn(code, mechanics, cardType, text))                        mechanicCounters[V_RETURN]->increaseSyn(code);
    if(isStealthSyn(code))                                                  mechanicCounters[V_STEALTH]->increaseSyn(code);
    if(isDivineShieldSyn(code))                                             mechanicCounters[V_DIVINE_SHIELD]->increaseSyn(code);
    else if(isDivineShieldAllSyn(code))                                     mechanicCounters[V_DIVINE_SHIELD_ALL]->increaseSyn(code);
    if(isEnrageMinionSyn(code))                                             mechanicCounters[V_ENRAGED_MINION]->increaseSyn(code);
    else if(isEnrageAllSyn(code, text))                                     mechanicCounters[V_ENRAGED_ALL]->increaseSyn(code);
}


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QStringList &mechanicIcons)
{
    getCardTypeSynergies(deckCard, synergies);
    getRaceSynergies(deckCard, synergies);
    getMechanicSynergies(deckCard, synergies, mechanicIcons);
    getDirectLinkSynergies(deckCard, synergies);
}


void SynergyHandler::getCardTypeSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL || isSpellGen(code))           cardTypeCounters[V_SPELL]->insertSynCards(synergies);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergies);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);
    }
    else if(isWeaponGen(code, text))            cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);


    if(isSpellSyn(code))                        cardTypeCounters[V_SPELL]->insertCards(synergies);
    if(isWeaponSyn(code))                       cardTypeCounters[V_WEAPON]->insertCards(synergies);
    else if(isWeaponAllSyn(code, text))         cardTypeCounters[V_WEAPON_ALL]->insertCards(synergies);
}


void SynergyHandler::getRaceSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    CardRace cardRace = deckCard.getRace();

    if(cardRace == MURLOC || isMurlocGen(code))         raceCounters[V_MURLOC]->insertSynCards(synergies);
    if(cardRace == DEMON || isDemonGen(code))           raceCounters[V_DEMON]->insertSynCards(synergies);
    if(cardRace == MECHANICAL || isMechGen(code))       raceCounters[V_MECHANICAL]->insertSynCards(synergies);
    if(cardRace == ELEMENTAL || isElementalGen(code))   raceCounters[V_ELEMENTAL]->insertSynCards(synergies);
    if(cardRace == BEAST || isBeastGen(code))           raceCounters[V_BEAST]->insertSynCards(synergies);
    if(cardRace == TOTEM || isTotemGen(code))           raceCounters[V_TOTEM]->insertSynCards(synergies);
    if(cardRace == PIRATE || isPirateGen(code))         raceCounters[V_PIRATE]->insertSynCards(synergies);
    if(cardRace == DRAGON || isDragonGen(code))         raceCounters[V_DRAGON]->insertSynCards(synergies);

    if(isMurlocSyn(code))   raceCounters[V_MURLOC]->insertCards(synergies);
    if(isDemonSyn(code))    raceCounters[V_DEMON]->insertCards(synergies);
    if(isMechSyn(code))     raceCounters[V_MECHANICAL]->insertCards(synergies);
    if(isElementalSyn(code))raceCounters[V_ELEMENTAL]->insertCards(synergies);
    if(isBeastSyn(code))    raceCounters[V_BEAST]->insertCards(synergies);
    if(isTotemSyn(code))    raceCounters[V_TOTEM]->insertCards(synergies);
    if(isPirateSyn(code))   raceCounters[V_PIRATE]->insertCards(synergies);
    if(isDragonSyn(code))   raceCounters[V_DRAGON]->insertCards(synergies);
}


void SynergyHandler::getMechanicSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QStringList &mechanicIcons)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();

    if(isDiscoverDrawGen(code))
    {
        mechanicIcons.append("drawMechanic.png");
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->insertSynCards(synergies);
        mechanicIcons.append("tauntMechanic.png");
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicIcons.append("tauntMechanic.png");
    }
    if(isAoeGen(code))
    {
        mechanicCounters[V_AOE]->insertSynCards(synergies);
        mechanicIcons.append("aoeMechanic.png");
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->insertSynCards(synergies);
        mechanicIcons.append("pingMechanic.png");
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack) || isDestroyGen(code))
    {
        mechanicIcons.append("damageMechanic.png");
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons.append("reachMechanic.png");
    }
    if(isJadeGolemGen(code, mechanics, referencedTags))         mechanicCounters[V_JADE_GOLEM]->insertCards(synergies);
    if(isOverload(code))                                        mechanicCounters[V_OVERLOAD]->insertSynCards(synergies);
    if(isSecretGen(code, mechanics))                            mechanicCounters[V_SECRET]->insertSynCards(synergies);
    if(isFreezeGen(code, mechanics, referencedTags, text))      mechanicCounters[V_FREEZE]->insertSynCards(synergies);
    if(isDiscardGen(code, text))                                mechanicCounters[V_DISCARD]->insertSynCards(synergies);
    if(isDeathrattleMinion(code, mechanics, cardType))          mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergies);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags))   mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertSynCards(synergies);
    if(isBattlecryMinion(code, mechanics, cardType))            mechanicCounters[V_BATTLECRY]->insertSynCards(synergies);
    if(isSilenceOwnGen(code, mechanics, referencedTags))        mechanicCounters[V_SILENCE]->insertSynCards(synergies);
    if(isTauntGiverGen(code))                                   mechanicCounters[V_TAUNT_GIVER]->insertSynCards(synergies);
    if(isTokenGen(code, text))                                  mechanicCounters[V_TOKEN]->insertSynCards(synergies);
    if(isWindfuryMinion(code, mechanics, cardType))             mechanicCounters[V_WINDFURY]->insertSynCards(synergies);
    if(isAttackBuffGen(code, text))                             mechanicCounters[V_ATTACK_BUFF]->insertSynCards(synergies);
    if(isHealthBuffGen(code, text))                             mechanicCounters[V_HEALTH_BUFF]->insertSynCards(synergies);
    if(isReturnGen(code, text))                                 mechanicCounters[V_RETURN]->insertSynCards(synergies);
    if(isStealthGen(code, mechanics))                           mechanicCounters[V_STEALTH]->insertSynCards(synergies);
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
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergies);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergies);
    if(isOverloadSyn(code, text))                               mechanicCounters[V_OVERLOAD]->insertCards(synergies);
    if(isSecretSyn(code, referencedTags))                       mechanicCounters[V_SECRET]->insertCards(synergies);
    if(isFreezeSyn(code, referencedTags, text))                 mechanicCounters[V_FREEZE]->insertCards(synergies);
    if(isDiscardSyn(code, text))                                mechanicCounters[V_DISCARD]->insertCards(synergies);
    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergies);
    else if(isDeathrattleGoodAllSyn(code))                      mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertCards(synergies);
    if(isBattlecrySyn(code, referencedTags))                    mechanicCounters[V_BATTLECRY]->insertCards(synergies);
    if(isSilenceOwnSyn(code, mechanics))                        mechanicCounters[V_SILENCE]->insertCards(synergies);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))      mechanicCounters[V_TAUNT_GIVER]->insertCards(synergies);
    if(isTokenSyn(code, text))                                  mechanicCounters[V_TOKEN]->insertCards(synergies);
    if(isWindfurySyn(code))                                     mechanicCounters[V_WINDFURY]->insertCards(synergies);
    if(isAttackBuffSyn(code))                                   mechanicCounters[V_ATTACK_BUFF]->insertCards(synergies);
    if(isHealthBuffSyn(code))                                   mechanicCounters[V_HEALTH_BUFF]->insertCards(synergies);
    if(isReturnSyn(code, mechanics, cardType, text))            mechanicCounters[V_RETURN]->insertCards(synergies);
    if(isStealthSyn(code))                                      mechanicCounters[V_STEALTH]->insertCards(synergies);
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
bool SynergyHandler::isDiscoverDrawGen(const QString &code)
{
    //TEST
    //&& (text.contains("draw") || text.contains("discover") || (text.contains("to") && text.contains("your") && text.contains("hand")))
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

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
        QString text = Utility::cardEnTextFromCode(code).toLower();
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
bool SynergyHandler::isAoeGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeGen");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  (text.contains("all") || text.contains("adjacent")) &&
                    (text.contains("damage") ||
                        (text.contains("destroy") && text.contains("minions"))
                     );
    }
}
bool SynergyHandler::isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingGen");
    }
    //Anything that deals damage
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("random") && !text.contains("hero"))
    {
        return true;
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
    else if(text.contains("+") && text.contains("give") && text.contains("attack") && text.contains("hero"))
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
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageMinionsGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
            !text.contains("1 damage") && !text.contains("all") && !text.contains("random") && !text.contains("hero"))
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
bool SynergyHandler::isDestroyGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroyGen");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("destroy") && text.contains("minion") &&
                !text.contains("all");
    }
}
bool SynergyHandler::isEnrageMinion(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageMinion");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")))
    {
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isFreezeGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                 const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeGen");
    }
    else if(mechanics.contains(QJsonValue("FREEZE")))
    {
        return true;
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return !text.contains("frozen");
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(cardType != MINION)  return false;
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattle");
    }
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    if(synergyCodes.contains(code))
    {
        return (synergyCodes[code].contains("deathrattle") || synergyCodes[code].contains("deathrattleGen")) &&
                !synergyCodes[code].contains("silenceOwnSyn");
    }
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isTauntGiverGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverGen");
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isTokenGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("2/2")) && !text.contains("opponent")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenGen");
    }
    else if((text.contains("1/1") || text.contains("2/1") || text.contains("1/2"))
            && text.contains("summon") && !text.contains("opponent"))
    {
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
            && !text.contains("hand") && !text.contains("random") && !text.contains("c'thun"))
    {
        return true;
    }
    else
    {
        return false;
    }
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
            && !text.contains("hand") && !text.contains("random") && !text.contains("c'thun"))
    {
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}



//Synergy items
bool SynergyHandler::isSpellSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
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
    else
    {
        return false;
    }
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
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("murloc");
    }
}
bool SynergyHandler::isDemonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("demon");
    }
}
bool SynergyHandler::isMechSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("mech");
    }
}
bool SynergyHandler::isElementalSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("elemental");
    }
}
bool SynergyHandler::isBeastSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("beast");
    }
}
bool SynergyHandler::isTotemSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("totem");
    }
}
bool SynergyHandler::isPirateSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("pirate");
    }
}
bool SynergyHandler::isDragonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("dragon");
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isFreezeSyn(const QString &code, const QJsonArray &referencedTags, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeSyn");
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return text.contains("frozen");
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isDeathrattleSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleSyn");
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isDeathrattleGoodAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleGoodAllSyn");
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isTauntGiverSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverSyn");
    }
    else if(mechanics.contains(QJsonValue("CANT_ATTACK")))
    {
        return true;
    }
    else if(cardType == MINION && attack ==0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isTokenSyn(const QString &code, const QString &text)
{
    //TEST
//    (text.contains("+") && (text.contains("minions") || text.contains("characters"))
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isWindfurySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("windfurySyn");
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isAttackBuffSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackBuffSyn");
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isHealthBuffSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("healthBuffSyn");
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
}
bool SynergyHandler::isDivineShieldSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldSyn");
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isDivineShieldAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldAllSyn");
    }
    else
    {
        return false;
    }
}
