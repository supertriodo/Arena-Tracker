#include "schoolcounter.h"

QMap<QString, DraftItemCounter*> SchoolCounter::keySynergiesMap;
QMap<QString, QList<QString>> * SchoolCounter::synergyCodes = nullptr;

SchoolCounter::SchoolCounter()
{

}


void SchoolCounter::createSchoolCounters(QObject *parent)
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


void SchoolCounter::resetAll()
{
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        keySynergiesMap[key]->reset();
    }
}


void SchoolCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    SchoolCounter::synergyCodes = synergyCodes;
}


QStringList SchoolCounter::debugSchoolSynergies(const QString &code, const QString &text)
{
    QStringList mec;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Syn
        if(isKeyAllSyn(keyAll+"Syn", code, text))
        {
            mec << keyAll+"Syn";
        }
    }
    return mec;
}


void SchoolCounter::updateSchoolCounters(const QString &code, const QString &text, CardSchool cardSchool)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, cardSchool))
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
        else if(isKeyAllSyn(keyAll+"Syn", code, text))
        {
            keySynergiesMap[keyAll]->increaseSyn(code);
        }
    }
}


void SchoolCounter::getSchoolSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                       const QString &text, const CardSchool &cardSchool)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, cardSchool))
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
        else if(isKeyAllSyn(keyAll+"Syn", code, text))
        {
            keySynergiesMap[keyAll]->insertCards(synergyTagMap);
        }
    }
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void SchoolCounter::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
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
bool SchoolCounter::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag, const QString &text, CardSchool cardSchool)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardSchool) ||
               isKeyGen(key+"Gen", code);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardSchool);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;
        const QString &keyAll = key+"All";

        return isKeyAllSyn(keyAll+"Syn", code, text);
    }
    else
    {
        QString key = partSynergy;
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;
        const QString &keyAll = key+"All";

        return isKeySyn(key+"Syn", code) ||
               isKeyAllSyn(keyAll+"Syn", code, text);
    }
}


QString SchoolCounter::getSynergyTag(const QString &key)
{
    if(!keySynergiesMap.contains(key))  return "";
    return keySynergiesMap[key]->getSynergyTag();
}


QStringList SchoolCounter::getListKeySynergies()
{
    return getMapKeySynergies().keys();
}


QMap<QString, QString> SchoolCounter::getMapKeySynergies()
{
    QMap<QString, QString> keys;

    keys["arcane"] = "Arcane";
    keys["fel"] = "Fel";
    keys["fire"] = "Fire";
    keys["frost"] = "Frost";
    keys["holy"] = "Holy";
    keys["shadow"] = "Shadow";
    keys["nature"] = "Nature";

    return keys;
}


bool SchoolCounter::isKey(const QString &key, const CardSchool &cardSchool)
{
    if(key == "arcane")         return (cardSchool == ARCANE);
    else if(key == "fel")       return (cardSchool == FEL);
    else if(key == "fire")      return (cardSchool == FIRE);
    else if(key == "frost")     return (cardSchool == FROST);
    else if(key == "holy")      return (cardSchool == HOLY);
    else if(key == "shadow")    return (cardSchool == SHADOW);
    else if(key == "nature")    return (cardSchool == NATURE);

    return false;
}


bool SchoolCounter::isKeyGen(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


bool SchoolCounter::isKeySyn(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


bool SchoolCounter::isKeyAllSyn(QString key, const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        key.chop(6);
        return text.contains(key+" spell");
    }

    return false;
}
