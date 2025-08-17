#include "cardtypecounter.h"
#include "../themehandler.h"

QMap<QString, DraftItemCounter*> CardTypeCounter::keySynergiesMap;
QMap<QString, QList<QString>> * CardTypeCounter::synergyCodes = nullptr;

CardTypeCounter::CardTypeCounter()
{

}


QMap<QString, DraftItemCounter*> * CardTypeCounter::createCardTypeCounters(QObject *parent, QGridLayout *mechanicsLayout)
{
    QMap<QString, QString> map = getMapKeySynergies();
    const auto keys = map.keys();
    for(const QString &key: keys)
    {
        const QString &synergyTag = map[key];
        DraftItemCounter * item;
        DraftItemCounter * itemAll = new DraftItemCounter(parent, synergyTag);

        if(key == "minion")
        {
            item = new DraftItemCounter(parent, "Minion", "Minion Gen", mechanicsLayout, 0, 0,
                                        QPixmap(ThemeHandler::minionsCounterFile()), 32, true, false);
        }
        else if(key == "spell")
        {
            item = new DraftItemCounter(parent, "Spell", "Spell Gen", mechanicsLayout, 0, 1,
                                        QPixmap(ThemeHandler::spellsCounterFile()), 32, true, false);
        }
        else if(key == "weapon")
        {
            item = new DraftItemCounter(parent, "Weapon", "Weapon Gen", mechanicsLayout, 0, 2,
                                        QPixmap(ThemeHandler::weaponsCounterFile()), 32, true, false);
        }
        else
        {
            item = new DraftItemCounter(parent, synergyTag);

        }
        keySynergiesMap.insert(key, item);
        keySynergiesMap.insert(key+"All", itemAll);
        //Qt los borrara cuando parent se destruya
    }

    return &CardTypeCounter::keySynergiesMap;
}


void CardTypeCounter::setTheme()
{
    keySynergiesMap["minion"]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()), 32, false);
    keySynergiesMap["spell"]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()), 32, false);
    keySynergiesMap["weapon"]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()), 32, false);
}


void CardTypeCounter::resetAll()
{
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        keySynergiesMap[key]->reset();
    }
}


void CardTypeCounter::setHidden(bool hide)
{
    const auto cardTypesKeys = CardTypeCounter::getListKeyLabels();
    for(const auto &key: cardTypesKeys)
    {
        if(hide)    keySynergiesMap[key]->hide();
        else        keySynergiesMap[key]->show();
    }
}


void CardTypeCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    const auto cardTypesKeys = CardTypeCounter::getListKeyLabels();
    for(const auto &key: cardTypesKeys)
    {
        keySynergiesMap[key]->setTransparency(transparency, mouseInApp);
    }
}


void CardTypeCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    CardTypeCounter::synergyCodes = synergyCodes;
}


int CardTypeCounter::draftedCardsCount()
{
    int num = 0;
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        num += keySynergiesMap[key]->count();
    }
    return num;
}


QStringList CardTypeCounter::debugCardTypeSynergies(const QString &code, const QString &text)
{
    QStringList mec;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKeyGen(key+"Gen", code, text))
        {
            mec << key+"Gen";
        }
        //Syn
        if(isKeyAllSyn(keyAll+"Syn", code, text))
        {
            mec << keyAll+"Syn";
        }
    }

    return mec;
}


void CardTypeCounter::getDirectLinkSynergies(const QString &code, const QMap<QString, QList<QString>> &directLinks, QMap<QString, int> &synergies)
{
    if(directLinks.contains(code))
    {
        const QList<QString> linkCodes = directLinks[code];

        for(const QString &linkCode: linkCodes)
        {
            if(keySynergiesMap["minion"]->insertCode(linkCode, synergies)){}
            else if(keySynergiesMap["weapon"]->insertCode(linkCode, synergies)){}
            else keySynergiesMap["spell"]->insertCode(linkCode, synergies);
        }
    }
}


void CardTypeCounter::getCardTypeCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap)
{
    spellMap = keySynergiesMap["spell"]->getCodeTagMap();
    minionMap = keySynergiesMap["minion"]->getCodeTagMap();
    weaponMap = keySynergiesMap["weapon"]->getCodeTagMap();
}


void CardTypeCounter::updateCardTypeCounters(const QString &code, QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                                             const QString &text, CardType cardType)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, cardType))
        {
            keySynergiesMap[key]->increase(code);
            keySynergiesMap[keyAll]->increase(code);

            if(key == "spell")  spellMap.insertMulti(code, "");
            if(key == "minion") minionMap.insertMulti(code, "");
            if(key == "weapon") weaponMap.insertMulti(code, "");
        }
        else if(isKeyGen(key+"Gen", code, text))
        {
            keySynergiesMap[keyAll]->increase(code);

            if(key == "spell")
            {
                keySynergiesMap[key]->increaseExtra(code);
                spellMap.insertMulti(code, ".");
            }
            if(key == "minion")
            {
                keySynergiesMap[key]->increaseExtra(code);
                minionMap.insertMulti(code, ".");
            }
            if(key == "weapon")
            {
                keySynergiesMap[key]->increaseExtra(code);
                weaponMap.insertMulti(code, ".");
            }
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


void CardTypeCounter::getCardTypeSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                           const QString &text, CardType cardType)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(!isWeakKey(key))
        {
            if(isKey(key, cardType))
            {
                keySynergiesMap[key]->insertSynCards(synergyTagMap);
                keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
            }
            else if(isKeyGen(key+"Gen", code, text))
            {
                keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
            }
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


//Sinergias que no queremos que se muestren en cada carta pq son debiles y muchas.
//Abarrotan la lista con sinergias innecesarias.
bool CardTypeCounter::isWeakKey(const QString &key)
{
    QStringList weakKeys = {"spell", "minion"};
    return weakKeys.contains(key);
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void CardTypeCounter::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
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
bool CardTypeCounter::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                                const QString &text, CardType cardType)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardType) ||
               isKeyGen(key+"Gen", code, text);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, cardType);
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


QString CardTypeCounter::getSynergyTag(const QString &key)
{
    if(!keySynergiesMap.contains(key))  return "";
    return keySynergiesMap[key]->getSynergyTag();
}


QStringList CardTypeCounter::getListKeyLabels()
{
    const QStringList keys = {"minion", "spell", "weapon"};
    return keys;
}


QStringList CardTypeCounter::getListKeySynergies()
{
    return getMapKeySynergies().keys();
}


QMap<QString, QString> CardTypeCounter::getMapKeySynergies()
{
    QMap<QString, QString> keys;

    keys["minion"] = "Minion";
    keys["spell"] = "Spell";
    keys["weapon"] = "Weapon";
    keys["location"] = "Location";

    return keys;
}


bool CardTypeCounter::isKey(const QString &key, CardType cardType)
{
    if(key == "minion")         return (cardType == MINION || cardType == HERO || cardType == LOCATION);
    else if(key == "spell")     return (cardType == SPELL);
    else if(key == "weapon")    return (cardType == WEAPON);
    else if(key == "location")  return (cardType == LOCATION);

    return false;
}
bool CardTypeCounter::isKeyGen(const QString &key, const QString &code, const QString &text)
{
    //spellGen minionGen weaponGen locationGen
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "weaponGen")
        {
            if(text.contains("equip "))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}
bool CardTypeCounter::isKeySyn(const QString &key, const QString &code)
{
    //spellSyn minionSyn weaponSyn locationSyn
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}
bool CardTypeCounter::isKeyAllSyn(const QString &key, const QString &code, const QString &text)
{
    //spellAllSyn minionAllSyn weaponAllSyn locationAllSyn
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "spellAllSyn")
        {
            return  text.contains("spellburst") ||
                   (text.contains("spell") && (text.contains("you cast") || text.contains("cost")));
        }
        else if(key == "weaponAllSyn")
        {
            return text.contains("weapon") && !text.contains("opponent's weapon");
        }
        else if(key == "locationAllSyn")
        {
            return text.contains("location");
        }
    }

    return false;
}

