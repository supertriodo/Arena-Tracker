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

    mechanicCounters[V_ENRAGED] = new DraftItemCounter(this);
    mechanicCounters[V_OVERLOAD] = new DraftItemCounter(this);
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this);
    mechanicCounters[V_SECRET] = new DraftItemCounter(this);
    mechanicCounters[V_FREEZE] = new DraftItemCounter(this);
    mechanicCounters[V_DISCARD] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this);
    mechanicCounters[V_BATTLECRY] = new DraftItemCounter(this);
    mechanicCounters[V_SILENCE] = new DraftItemCounter(this);
    mechanicCounters[V_TAUNT_GIVER] = new DraftItemCounter(this);

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
        synergyCodes[code];
        QJsonArray synergies = jsonObject.value(code).toArray();
        for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
        {
            synergyCodes[code].append(it->toString());
        }
    }

    emit pDebug("Synergy Cards: " + QString::number(synergyCodes.count()));
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
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)       cardTypeCounters[V_SPELL]->increase(code);
    else if(isSpellGen(code))   cardTypeCounters[V_SPELL]->increase(code,false);

    if(cardType == WEAPON)      cardTypeCounters[V_WEAPON]->increase(code);
    else if(isWeaponGen(code))  cardTypeCounters[V_WEAPON]->increase(code,false);

    if(cardType == MINION)      cardTypeCounters[V_MINION]->increase(code);

    if(isSpellSyn(code))    cardTypeCounters[V_SPELL]->increaseSyn(code);
    if(isWeaponSyn(code))   cardTypeCounters[V_WEAPON]->increaseSyn(code);
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
    if(isEnrageGen(code, mechanics, referencedTags))                        mechanicCounters[V_ENRAGED]->increase(code);
    if(isOverloadGen(code))                                                 mechanicCounters[V_OVERLOAD]->increase(code);
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isSecretGen(code, mechanics))                                        mechanicCounters[V_SECRET]->increase(code);
    if(isFreezeGen(code, mechanics, referencedTags, text))                  mechanicCounters[V_FREEZE]->increase(code);
    if(isDiscardGen(code, text))                                            mechanicCounters[V_DISCARD]->increase(code);
    if(isDeathrattleGen(code, mechanics))                                   mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isBattlecryGen(code, mechanics))                                     mechanicCounters[V_BATTLECRY]->increase(code);
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mechanicCounters[V_SILENCE]->increase(code);
    if(isTauntGiverGen(code))                                               mechanicCounters[V_TAUNT_GIVER]->increase(code);

    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isEnrageSyn(code, text))                                             mechanicCounters[V_ENRAGED]->increaseSyn(code);
    if(isOverloadSyn(code, text))                                           mechanicCounters[V_OVERLOAD]->increaseSyn(code);
    if(isSecretSyn(code, referencedTags))                                   mechanicCounters[V_SECRET]->increaseSyn(code);
    if(isFreezeSyn(code, referencedTags, text))                             mechanicCounters[V_FREEZE]->increaseSyn(code);
    if(isDiscardSyn(code, text))                                            mechanicCounters[V_DISCARD]->increaseSyn(code);
    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    if(isBattlecrySyn(code, referencedTags, text))                          mechanicCounters[V_BATTLECRY]->increaseSyn(code);
    if(isSilenceOwnSyn(code, mechanics))                                    mechanicCounters[V_SILENCE]->increaseSyn(code);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_TAUNT_GIVER]->increaseSyn(code);
}


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QStringList &mechanicIcons)
{
    getCardTypeSynergies(deckCard, synergies);
    getRaceSynergies(deckCard, synergies);
    getMechanicSynergies(deckCard, synergies, mechanicIcons);
}


void SynergyHandler::getCardTypeSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL || isSpellGen(code))   cardTypeCounters[V_SPELL]->insertSynCards(synergies);
    if(cardType == WEAPON || isWeaponGen(code)) cardTypeCounters[V_WEAPON]->insertSynCards(synergies);

    if(isSpellSyn(code))    cardTypeCounters[V_SPELL]->insertCards(synergies);
    if(isWeaponSyn(code))   cardTypeCounters[V_WEAPON]->insertCards(synergies);
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
    if(isEnrageGen(code, mechanics, referencedTags))            mechanicCounters[V_ENRAGED]->insertSynCards(synergies);
    if(isOverloadGen(code))                                     mechanicCounters[V_OVERLOAD]->insertSynCards(synergies);
    if(isSecretGen(code, mechanics))                            mechanicCounters[V_SECRET]->insertSynCards(synergies);
    if(isFreezeGen(code, mechanics, referencedTags, text))      mechanicCounters[V_FREEZE]->insertSynCards(synergies);
    if(isDiscardGen(code, text))                                mechanicCounters[V_DISCARD]->insertSynCards(synergies);
    if(isDeathrattleGen(code, mechanics))                       mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergies);
    if(isBattlecryGen(code, mechanics))                         mechanicCounters[V_BATTLECRY]->insertSynCards(synergies);
    if(isSilenceOwnGen(code, mechanics, referencedTags))        mechanicCounters[V_SILENCE]->insertSynCards(synergies);
    if(isTauntGiverGen(code))                                   mechanicCounters[V_TAUNT_GIVER]->insertSynCards(synergies);

    if(isTauntSyn(code))                                        mechanicCounters[V_TAUNT]->insertCards(synergies);
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergies);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergies);
    if(isEnrageSyn(code, text))                                 mechanicCounters[V_ENRAGED]->insertCards(synergies);
    if(isOverloadSyn(code, text))                               mechanicCounters[V_OVERLOAD]->insertCards(synergies);
    if(isSecretSyn(code, referencedTags))                       mechanicCounters[V_SECRET]->insertCards(synergies);
    if(isFreezeSyn(code, referencedTags, text))                 mechanicCounters[V_FREEZE]->insertCards(synergies);
    if(isDiscardSyn(code, text))                                mechanicCounters[V_DISCARD]->insertCards(synergies);
    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergies);
    if(isBattlecrySyn(code, referencedTags, text))              mechanicCounters[V_BATTLECRY]->insertCards(synergies);
    if(isSilenceOwnSyn(code, mechanics))                        mechanicCounters[V_SILENCE]->insertCards(synergies);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))      mechanicCounters[V_TAUNT_GIVER]->insertCards(synergies);
}


//Increase counters
bool SynergyHandler::isSpellGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("spellGen");
    return false;
}
bool SynergyHandler::isWeaponGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("weaponGen");
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
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discoverGen") ||
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
                (text.contains("to your hand") && !text.contains("return"));
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
    //Anything that deals damage (no pings)
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
bool SynergyHandler::isEnrageGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageGen");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")) || referencedTags.contains(QJsonValue("ENRAGED")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool SynergyHandler::isOverloadGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("overloadGen");
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
bool SynergyHandler::isDeathrattleGen(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleGen");
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
bool SynergyHandler::isBattlecryGen(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecryGen");
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
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("weapon") && !text.contains("opponent's weapon");
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
bool SynergyHandler::isEnrageSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageSyn");
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
bool SynergyHandler::isBattlecrySyn(const QString &code, const QJsonArray &referencedTags, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecrySyn");
    }
    else if(referencedTags.contains(QJsonValue("BATTLECRY")))
    {
        return true;
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
