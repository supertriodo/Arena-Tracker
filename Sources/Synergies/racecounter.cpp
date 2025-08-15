#include "racecounter.h"

DraftItemCounter ** RaceCounter::raceCounters;
QMap<QString, QList<QString>> * RaceCounter::synergyCodes;

RaceCounter::RaceCounter()
{

}


DraftItemCounter ** RaceCounter::createRaceCounters(QObject *parent)
{
    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(parent, "Elemental");
    raceCounters[V_BEAST] = new DraftItemCounter(parent, "Beast");
    raceCounters[V_MURLOC] = new DraftItemCounter(parent, "Murloc");
    raceCounters[V_DRAGON] = new DraftItemCounter(parent, "Dragon");
    raceCounters[V_PIRATE] = new DraftItemCounter(parent, "Pirate");
    raceCounters[V_MECHANICAL] = new DraftItemCounter(parent, "Mech");
    raceCounters[V_DEMON] = new DraftItemCounter(parent, "Demon");
    raceCounters[V_TOTEM] = new DraftItemCounter(parent, "Totem");
    raceCounters[V_NAGA] = new DraftItemCounter(parent, "Naga");
    raceCounters[V_UNDEAD] = new DraftItemCounter(parent, "Undead");
    raceCounters[V_QUILBOAR] = new DraftItemCounter(parent, "Quilboar");
    raceCounters[V_DRAENEI] = new DraftItemCounter(parent, "Draenei");
    //New race step

    raceCounters[V_ELEMENTAL_ALL] = new DraftItemCounter(parent, "Elemental");
    raceCounters[V_BEAST_ALL] = new DraftItemCounter(parent, "Beast");
    raceCounters[V_MURLOC_ALL] = new DraftItemCounter(parent, "Murloc");
    raceCounters[V_DRAGON_ALL] = new DraftItemCounter(parent, "Dragon");
    raceCounters[V_PIRATE_ALL] = new DraftItemCounter(parent, "Pirate");
    raceCounters[V_MECHANICAL_ALL] = new DraftItemCounter(parent, "Mech");
    raceCounters[V_DEMON_ALL] = new DraftItemCounter(parent, "Demon");
    raceCounters[V_TOTEM_ALL] = new DraftItemCounter(parent, "Totem");
    raceCounters[V_NAGA_ALL] = new DraftItemCounter(parent, "Naga");
    raceCounters[V_UNDEAD_ALL] = new DraftItemCounter(parent, "Undead");
    raceCounters[V_QUILBOAR_ALL] = new DraftItemCounter(parent, "Quilboar");
    raceCounters[V_DRAENEI_ALL] = new DraftItemCounter(parent, "Draenei");
    //New race step

    return raceCounters;
}


void RaceCounter::deleteRaceCounters()
{
    for(int i=0; i<V_NUM_RACES; i++)
    {
        delete raceCounters[i];
    }
    delete []raceCounters;
}


void RaceCounter::resetAll()
{
    for(int i=0; i<V_NUM_RACES; i++)
    {
        raceCounters[i]->reset();
    }
}


void RaceCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    RaceCounter::synergyCodes = synergyCodes;
}


QStringList RaceCounter::debugRaceSynergies(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    QStringList mec;

    if(isMurlocAllSyn(code, text))          mec<<"murlocAllSyn";
    if(isDemonAllSyn(code, text))           mec<<"demonAllSyn";
    if(isMechAllSyn(code, mechanics, text)) mec<<"mechAllSyn";
    if(isElementalAllSyn(code, text))       mec<<"elementalAllSyn";
    if(isBeastAllSyn(code, text))           mec<<"beastAllSyn";
    if(isTotemAllSyn(code, text))           mec<<"totemAllSyn";
    if(isPirateAllSyn(code, text))          mec<<"pirateAllSyn";
    if(isDragonAllSyn(code, text))          mec<<"dragonAllSyn";
    if(isNagaAllSyn(code, text))            mec<<"nagaAllSyn";
    if(isUndeadAllSyn(code, text))          mec<<"undeadAllSyn";
    if(isQuilboarAllSyn(code, text))        mec<<"quilboarAllSyn";
    if(isDraeneiAllSyn(code, text))         mec<<"draeneiAllSyn";
    //New race step

    return mec;
}


void RaceCounter::getRaceSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                   const QJsonArray &mechanics,
                                   const QString &text, const QList<CardRace> &cardRace)
{
    if(cardRace.contains(MURLOC) || cardRace.contains(ALL))
    {
        raceCounters[V_MURLOC]->insertSynCards(synergyTagMap);
        raceCounters[V_MURLOC_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DEMON) || cardRace.contains(ALL))
    {
        raceCounters[V_DEMON]->insertSynCards(synergyTagMap);
        raceCounters[V_DEMON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(MECHANICAL) || cardRace.contains(ALL))
    {
        raceCounters[V_MECHANICAL]->insertSynCards(synergyTagMap);
        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(ELEMENTAL) || cardRace.contains(ALL))
    {
        raceCounters[V_ELEMENTAL]->insertSynCards(synergyTagMap);
        raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(BEAST) || cardRace.contains(ALL))
    {
        raceCounters[V_BEAST]->insertSynCards(synergyTagMap);
        raceCounters[V_BEAST_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(TOTEM) || cardRace.contains(ALL))
    {
        raceCounters[V_TOTEM]->insertSynCards(synergyTagMap);
        raceCounters[V_TOTEM_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(PIRATE) || cardRace.contains(ALL))
    {
        raceCounters[V_PIRATE]->insertSynCards(synergyTagMap);
        raceCounters[V_PIRATE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DRAGON) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAGON]->insertSynCards(synergyTagMap);
        raceCounters[V_DRAGON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(NAGA) || cardRace.contains(ALL))
    {
        raceCounters[V_NAGA]->insertSynCards(synergyTagMap);
        raceCounters[V_NAGA_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isNagaGen(code))        raceCounters[V_NAGA_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(UNDEAD) || cardRace.contains(ALL))
    {
        raceCounters[V_UNDEAD]->insertSynCards(synergyTagMap);
        raceCounters[V_UNDEAD_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isUndeadGen(code))      raceCounters[V_UNDEAD_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(QUILBOAR) || cardRace.contains(ALL))
    {
        raceCounters[V_QUILBOAR]->insertSynCards(synergyTagMap);
        raceCounters[V_QUILBOAR_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isQuilboarGen(code))    raceCounters[V_QUILBOAR_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DRAENEI) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAENEI]->insertSynCards(synergyTagMap);
        raceCounters[V_DRAENEI_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDraeneiGen(code))    raceCounters[V_DRAENEI_ALL]->insertSynCards(synergyTagMap);
    //New race step

    if(isMurlocSyn(code))                           raceCounters[V_MURLOC]->insertCards(synergyTagMap);
    else if(isMurlocAllSyn(code, text))             raceCounters[V_MURLOC_ALL]->insertCards(synergyTagMap);
    if(isDemonSyn(code))                            raceCounters[V_DEMON]->insertCards(synergyTagMap);
    else if(isDemonAllSyn(code, text))              raceCounters[V_DEMON_ALL]->insertCards(synergyTagMap);
    if(isMechSyn(code))                             raceCounters[V_MECHANICAL]->insertCards(synergyTagMap);
    else if(isMechAllSyn(code, mechanics, text))    raceCounters[V_MECHANICAL_ALL]->insertCards(synergyTagMap);
    if(isElementalSyn(code))                        raceCounters[V_ELEMENTAL]->insertCards(synergyTagMap);
    else if(isElementalAllSyn(code, text))          raceCounters[V_ELEMENTAL_ALL]->insertCards(synergyTagMap);
    if(isBeastSyn(code))                            raceCounters[V_BEAST]->insertCards(synergyTagMap);
    else if(isBeastAllSyn(code, text))              raceCounters[V_BEAST_ALL]->insertCards(synergyTagMap);
    if(isTotemSyn(code))                            raceCounters[V_TOTEM]->insertCards(synergyTagMap);
    else if(isTotemAllSyn(code, text))              raceCounters[V_TOTEM_ALL]->insertCards(synergyTagMap);
    if(isPirateSyn(code))                           raceCounters[V_PIRATE]->insertCards(synergyTagMap);
    else if(isPirateAllSyn(code, text))             raceCounters[V_PIRATE_ALL]->insertCards(synergyTagMap);
    if(isDragonSyn(code))                           raceCounters[V_DRAGON]->insertCards(synergyTagMap);
    else if(isDragonAllSyn(code, text))             raceCounters[V_DRAGON_ALL]->insertCards(synergyTagMap);
    if(isNagaSyn(code))                             raceCounters[V_NAGA]->insertCards(synergyTagMap);
    else if(isNagaAllSyn(code, text))               raceCounters[V_NAGA_ALL]->insertCards(synergyTagMap);
    if(isUndeadSyn(code))                           raceCounters[V_UNDEAD]->insertCards(synergyTagMap);
    else if(isUndeadAllSyn(code, text))             raceCounters[V_UNDEAD_ALL]->insertCards(synergyTagMap);
    if(isQuilboarSyn(code))                         raceCounters[V_QUILBOAR]->insertCards(synergyTagMap);
    else if(isQuilboarAllSyn(code, text))           raceCounters[V_QUILBOAR_ALL]->insertCards(synergyTagMap);
    if(isDraeneiSyn(code))                          raceCounters[V_DRAENEI]->insertCards(synergyTagMap);
    else if(isDraeneiAllSyn(code, text))            raceCounters[V_DRAENEI_ALL]->insertCards(synergyTagMap);
    //New race step
}


void RaceCounter::updateRaceCounters(const QString &code, const QJsonArray &mechanics,
                                     const QString &text, const QList<CardRace> &cardRace)
{
    if(cardRace.contains(MURLOC) || cardRace.contains(ALL))
    {
        raceCounters[V_MURLOC]->increase(code);
        raceCounters[V_MURLOC_ALL]->increase(code);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->increase(code);
    if(cardRace.contains(DEMON) || cardRace.contains(ALL))
    {
        raceCounters[V_DEMON]->increase(code);
        raceCounters[V_DEMON_ALL]->increase(code);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->increase(code);
    if(cardRace.contains(MECHANICAL) || cardRace.contains(ALL))
    {
        raceCounters[V_MECHANICAL]->increase(code);
        raceCounters[V_MECHANICAL_ALL]->increase(code);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->increase(code);
    if(cardRace.contains(ELEMENTAL) || cardRace.contains(ALL))
    {
        raceCounters[V_ELEMENTAL]->increase(code);
        raceCounters[V_ELEMENTAL_ALL]->increase(code);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->increase(code);
    if(cardRace.contains(BEAST) || cardRace.contains(ALL))
    {
        raceCounters[V_BEAST]->increase(code);
        raceCounters[V_BEAST_ALL]->increase(code);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->increase(code);
    if(cardRace.contains(TOTEM) || cardRace.contains(ALL))
    {
        raceCounters[V_TOTEM]->increase(code);
        raceCounters[V_TOTEM_ALL]->increase(code);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->increase(code);
    if(cardRace.contains(PIRATE) || cardRace.contains(ALL))
    {
        raceCounters[V_PIRATE]->increase(code);
        raceCounters[V_PIRATE_ALL]->increase(code);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->increase(code);
    if(cardRace.contains(DRAGON) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAGON]->increase(code);
        raceCounters[V_DRAGON_ALL]->increase(code);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->increase(code);
    if(cardRace.contains(NAGA) || cardRace.contains(ALL))
    {
        raceCounters[V_NAGA]->increase(code);
        raceCounters[V_NAGA_ALL]->increase(code);
    }
    else if(isNagaGen(code))        raceCounters[V_NAGA_ALL]->increase(code);
    if(cardRace.contains(UNDEAD) || cardRace.contains(ALL))
    {
        raceCounters[V_UNDEAD]->increase(code);
        raceCounters[V_UNDEAD_ALL]->increase(code);
    }
    else if(isUndeadGen(code))      raceCounters[V_UNDEAD_ALL]->increase(code);
    if(cardRace.contains(QUILBOAR) || cardRace.contains(ALL))
    {
        raceCounters[V_QUILBOAR]->increase(code);
        raceCounters[V_QUILBOAR_ALL]->increase(code);
    }
    else if(isQuilboarGen(code))    raceCounters[V_QUILBOAR_ALL]->increase(code);
    if(cardRace.contains(DRAENEI) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAENEI]->increase(code);
        raceCounters[V_DRAENEI_ALL]->increase(code);
    }
    else if(isDraeneiGen(code))    raceCounters[V_DRAENEI_ALL]->increase(code);
    //New race step

    if(isMurlocSyn(code))                           raceCounters[V_MURLOC]->increaseSyn(code);
    else if(isMurlocAllSyn(code, text))             raceCounters[V_MURLOC_ALL]->increaseSyn(code);
    if(isDemonSyn(code))                            raceCounters[V_DEMON]->increaseSyn(code);
    else if(isDemonAllSyn(code, text))              raceCounters[V_DEMON_ALL]->increaseSyn(code);
    if(isMechSyn(code))                             raceCounters[V_MECHANICAL]->increaseSyn(code);
    else if(isMechAllSyn(code, mechanics, text))    raceCounters[V_MECHANICAL_ALL]->increaseSyn(code);
    if(isElementalSyn(code))                        raceCounters[V_ELEMENTAL]->increaseSyn(code);
    else if(isElementalAllSyn(code, text))          raceCounters[V_ELEMENTAL_ALL]->increaseSyn(code);
    if(isBeastSyn(code))                            raceCounters[V_BEAST]->increaseSyn(code);
    else if(isBeastAllSyn(code, text))              raceCounters[V_BEAST_ALL]->increaseSyn(code);
    if(isTotemSyn(code))                            raceCounters[V_TOTEM]->increaseSyn(code);
    else if(isTotemAllSyn(code, text))              raceCounters[V_TOTEM_ALL]->increaseSyn(code);
    if(isPirateSyn(code))                           raceCounters[V_PIRATE]->increaseSyn(code);
    else if(isPirateAllSyn(code, text))             raceCounters[V_PIRATE_ALL]->increaseSyn(code);
    if(isDragonSyn(code))                           raceCounters[V_DRAGON]->increaseSyn(code);
    else if(isDragonAllSyn(code, text))             raceCounters[V_DRAGON_ALL]->increaseSyn(code);
    if(isNagaSyn(code))                             raceCounters[V_NAGA]->increaseSyn(code);
    else if(isNagaAllSyn(code, text))               raceCounters[V_NAGA_ALL]->increaseSyn(code);
    if(isUndeadSyn(code))                           raceCounters[V_UNDEAD]->increaseSyn(code);
    else if(isUndeadAllSyn(code, text))             raceCounters[V_UNDEAD_ALL]->increaseSyn(code);
    if(isQuilboarSyn(code))                         raceCounters[V_QUILBOAR]->increaseSyn(code);
    else if(isQuilboarAllSyn(code, text))           raceCounters[V_QUILBOAR_ALL]->increaseSyn(code);
    if(isDraeneiSyn(code))                         raceCounters[V_DRAENEI]->increaseSyn(code);
    else if(isDraeneiAllSyn(code, text))           raceCounters[V_DRAENEI_ALL]->increaseSyn(code);
    //New race step
}


//Gen
bool RaceCounter::isMurlocGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("murlocGen");
    return false;
}
bool RaceCounter::isDemonGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("demonGen");
    return false;
}
bool RaceCounter::isMechGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("mechGen");
    return false;
}
bool RaceCounter::isElementalGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("elementalGen");
    return false;
}
bool RaceCounter::isBeastGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("beastGen");
    return false;
}
bool RaceCounter::isTotemGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("totemGen");
    return false;
}
bool RaceCounter::isPirateGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("pirateGen");
    return false;
}
bool RaceCounter::isDragonGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("dragonGen");
    return false;
}
bool RaceCounter::isNagaGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("nagaGen");
    return false;
}
bool RaceCounter::isUndeadGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("undeadGen");
    return false;
}
bool RaceCounter::isQuilboarGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("quilboarGen");
    return false;
}
bool RaceCounter::isDraeneiGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("draeneiGen");
    return false;
}
//New race step

//Syn
bool RaceCounter::isMurlocSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("murlocSyn");
    }
    return false;
}
bool RaceCounter::isMurlocAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("murlocAllSyn");
    }
    else
    {
        return text.contains("murloc");
    }
}
bool RaceCounter::isDemonSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("demonSyn");
    }
    return false;
}
bool RaceCounter::isDemonAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("demonAllSyn");
    }
    else
    {
        return text.contains("demon");
    }
}
bool RaceCounter::isMechSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("mechSyn");
    }
    return false;
}
bool RaceCounter::isMechAllSyn(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("mechAllSyn");
    }
    else if(mechanics.contains(QJsonValue("MAGNETIC"))) return true;
    else
    {
        return text.contains("mech");
    }
}
bool RaceCounter::isElementalSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("elementalSyn");
    }
    return false;
}
bool RaceCounter::isElementalAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("elementalAllSyn");
    }
    else
    {
        return text.contains("elemental");
    }
}
bool RaceCounter::isBeastSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("beastSyn");
    }
    return false;
}
bool RaceCounter::isBeastAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("beastAllSyn");
    }
    else
    {
        return text.contains("beast");
    }
}
bool RaceCounter::isTotemSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("totemSyn");
    }
    return false;
}
bool RaceCounter::isTotemAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("totemAllSyn");
    }
    else
    {
        return text.contains("totem");
    }
}
bool RaceCounter::isPirateSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("pirateSyn");
    }
    return false;
}
bool RaceCounter::isPirateAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("pirateAllSyn");
    }
    else
    {
        return text.contains("pirate");
    }
}
bool RaceCounter::isDragonSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("dragonSyn");
    }
    return false;
}
bool RaceCounter::isDragonAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("dragonAllSyn");
    }
    else
    {
        return text.contains("dragon");
    }
}
bool RaceCounter::isNagaSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("nagaSyn");
    }
    return false;
}
bool RaceCounter::isNagaAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("nagaAllSyn");
    }
    else
    {
        return text.contains("naga");
    }
}
bool RaceCounter::isUndeadSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("undeadSyn");
    }
    return false;
}
bool RaceCounter::isUndeadAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("undeadAllSyn");
    }
    else
    {
        return text.contains("undead");
    }
}
bool RaceCounter::isQuilboarSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("quilboarSyn");
    }
    return false;
}
bool RaceCounter::isQuilboarAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("quilboarAllSyn");
    }
    else
    {
        return text.contains("quilboar");
    }
}
bool RaceCounter::isDraeneiSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("draeneiSyn");
    }
    return false;
}
bool RaceCounter::isDraeneiAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("draeneiAllSyn");
    }
    else
    {
        return text.contains("draenei");
    }
}
//New race step
