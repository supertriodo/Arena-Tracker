#include "racecounter.h"

QMap<QString, DraftItemCounter*> RaceCounter::keySynergiesMap;
QMap<QString, QList<QString>> * RaceCounter::synergyCodes = nullptr;

RaceCounter::RaceCounter()
{

}


void RaceCounter::createRaceCounters(QObject *parent)
{
    QMap<QString, QString> map = getMapKeySynergies();
    const auto keys = map.keys();
    for(const QString &key: keys)
    {
        const QString &synergyTag = map[key];
        keySynergiesMap.insert(key, new DraftItemCounter(parent, synergyTag));
        keySynergiesMap.insert(key+"All", new DraftItemCounter(parent, synergyTag));
        //Qt los borrara cuando parent se destruya
    }
}


void RaceCounter::resetAll()
{
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        keySynergiesMap[key]->reset();
    }
}


void RaceCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    RaceCounter::synergyCodes = synergyCodes;
}


QStringList RaceCounter::debugRaceSynergies(const QString &code, const QJsonArray &mechanics,
                                            const QString &text)
{
    QStringList mec;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Syn
        if(isKeyAllSyn(keyAll+"Syn", code, mechanics, text))
        {
            mec << keyAll+"Syn";
        }
    }
    return mec;
}


void RaceCounter::updateRaceCounters(const QString &code, const QJsonArray &mechanics,
                                     const QString &text, const QList<CardRace> &cardRace)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, cardRace))
        {
            keySynergiesMap[key]->increase(code);
            keySynergiesMap[keyAll]->increase(code);
        }
        else if(isKeyGen(key+"Gen", code))
        {
            keySynergiesMap[keyAll]->increase(code);
        }
        //Syn
        if(isKeySyn(key+"Syn", code))
        {
            keySynergiesMap[key]->increaseSyn(code);
        }
        else if(isKeyAllSyn(keyAll+"Syn", code, mechanics, text))
        {
            keySynergiesMap[keyAll]->increaseSyn(code);
        }
    }
}


void RaceCounter::getRaceSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                   const QJsonArray &mechanics,
                                   const QString &text, const QList<CardRace> &cardRace)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, cardRace))
        {
            keySynergiesMap[key]->insertSynCards(synergyTagMap);
            keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
        }
        else if(isKeyGen(key+"Gen", code))
        {
            keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
        }
        //Syn
        if(isKeySyn(key+"Syn", code))
        {
            keySynergiesMap[key]->insertCards(synergyTagMap);
        }
        else if(isKeyAllSyn(keyAll+"Syn", code, mechanics, text))
        {
            keySynergiesMap[keyAll]->insertCards(synergyTagMap);
        }
    }
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void RaceCounter::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
    }
    else
    {
        QString key = partSynergy;
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertSynCards(synergyTagMap);
        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
    }
}


//Usada por LayeredSynergies para verificar que el code hace sinergia con cada una de las partSynergy
bool RaceCounter::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                            const QJsonArray &mechanics, const QString &text, const QList<CardRace> &cardRace)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardRace) ||
               isKeyGen(key+"Gen", code);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardRace);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;
        const QString &keyAll = key+"All";

        return isKeyAllSyn(keyAll+"Syn", code, mechanics, text);
    }
    else
    {
        QString key = partSynergy;
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;
        const QString &keyAll = key+"All";

        return isKeySyn(key+"Syn", code) ||
               isKeyAllSyn(keyAll+"Syn", code, mechanics, text);
    }
}


QString RaceCounter::getSynergyTag(const QString &key)
{
    if(!keySynergiesMap.contains(key))  return "";
    return keySynergiesMap[key]->getSynergyTag();
}


QStringList RaceCounter::getListValidSynergies()
{
    QStringList validMecs;
    const auto rKeys = RaceCounter::getListKeySynergies();
    for(const QString &keyS: rKeys)
    {
        validMecs << keyS+"Gen" << keyS+"Syn" << keyS+"AllSyn";
    }
    return validMecs;
}


QStringList RaceCounter::getListKeySynergies()
{
    return getMapKeySynergies().keys();
}


QMap<QString, QString> RaceCounter::getMapKeySynergies()
{
    QMap<QString, QString> keys;

    keys["elemental"] = "Elemental";
    keys["beast"] = "Beast";
    keys["murloc"] = "Murloc";
    keys["dragon"] = "Dragon";
    keys["pirate"] = "Pirate";
    keys["mech"] = "Mech";
    keys["demon"] = "Demon";
    keys["totem"] = "Totem";
    keys["naga"] = "Naga";
    keys["undead"] = "Undead";
    keys["quilboar"] = "Quilboar";
    keys["draenei"] = "Draenei";
    //New race step

    return keys;
}


bool RaceCounter::isKey(const QString &key, const QList<CardRace> &cardRace)
{
    if(cardRace.contains(ALL))  return true;

    if(key == "elemental")      return cardRace.contains(ELEMENTAL);
    else if(key == "beast")     return cardRace.contains(BEAST);
    else if(key == "murloc")    return cardRace.contains(MURLOC);
    else if(key == "dragon")    return cardRace.contains(DRAGON);
    else if(key == "pirate")    return cardRace.contains(PIRATE);
    else if(key == "mech")      return cardRace.contains(MECHANICAL);
    else if(key == "demon")     return cardRace.contains(DEMON);
    else if(key == "totem")     return cardRace.contains(TOTEM);
    else if(key == "naga")      return cardRace.contains(NAGA);
    else if(key == "undead")    return cardRace.contains(UNDEAD);
    else if(key == "quilboar")  return cardRace.contains(QUILBOAR);
    else if(key == "draenei")   return cardRace.contains(DRAENEI);
    //New race step

    return false;
}


bool RaceCounter::isKeyGen(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


bool RaceCounter::isKeySyn(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


bool RaceCounter::isKeyAllSyn(QString key, const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        key.chop(6);
        if(key == "mech")
        {
            if(mechanics.contains(QJsonValue("MAGNETIC"))) return true;
        }
        return text.contains(key);
    }

    return false;
}

