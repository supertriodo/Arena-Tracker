#include "cardtypecounter.h"
#include "../themehandler.h"

DraftItemCounter ** CardTypeCounter::cardTypeCounters;
QMap<QString, QList<QString>> * CardTypeCounter::synergyCodes;

CardTypeCounter::CardTypeCounter()
{

}


DraftItemCounter ** CardTypeCounter::createCardTypeCounters(QObject *parent, QGridLayout *mechanicsLayout)
{
    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(parent, "Minion", "Minion Gen", mechanicsLayout, 0, 0,
                                                      QPixmap(ThemeHandler::minionsCounterFile()), 32, true, false);
    cardTypeCounters[V_MINION_ALL] = new DraftItemCounter(parent, "Minion");
    cardTypeCounters[V_SPELL] = new DraftItemCounter(parent, "Spell", "Spell Gen", mechanicsLayout, 0, 1,
                                                     QPixmap(ThemeHandler::spellsCounterFile()), 32, true, false);
    cardTypeCounters[V_SPELL_ALL] = new DraftItemCounter(parent, "Spell");
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(parent, "Weapon", "Weapon Gen", mechanicsLayout, 0, 2,
                                                      QPixmap(ThemeHandler::weaponsCounterFile()), 32, true, false);
    cardTypeCounters[V_WEAPON_ALL] = new DraftItemCounter(parent, "Weapon");
    cardTypeCounters[V_LOCATION] = new DraftItemCounter(parent, "Location");
    cardTypeCounters[V_LOCATION_ALL] = new DraftItemCounter(parent, "Location");
    return cardTypeCounters;
}


void CardTypeCounter::deleteCardTypeCounters()
{
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        delete cardTypeCounters[i];
    }
    delete []cardTypeCounters;
}


void CardTypeCounter::setTheme()
{
    cardTypeCounters[V_MINION]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()), 32, false);
    cardTypeCounters[V_SPELL]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()), 32, false);
    cardTypeCounters[V_WEAPON]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()), 32, false);
}


void CardTypeCounter::resetAll()
{
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->reset();
    }
}


void CardTypeCounter::setHidden(bool hide)
{
    if(hide)
    {
        cardTypeCounters[V_MINION]->hide();
        cardTypeCounters[V_SPELL]->hide();
        cardTypeCounters[V_WEAPON]->hide();
    }
    else
    {
        cardTypeCounters[V_MINION]->show();
        cardTypeCounters[V_SPELL]->show();
        cardTypeCounters[V_WEAPON]->show();
    }
}


void CardTypeCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


void CardTypeCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    CardTypeCounter::synergyCodes = synergyCodes;
}


int CardTypeCounter::draftedCardsCount()
{
    int num = 0;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        num += cardTypeCounters[i]->count();
    }
    return num;
}


QStringList CardTypeCounter::debugCardTypeSynergies(const QString &code, const QString &text)
{
    QStringList mec;

    if(isCardTypeGen("weaponGen", code, text))          mec<<"weaponGen";
    if(isCardTypeAllSyn("spellAllSyn", code, text))     mec<<"spellAllSyn";
    if(isCardTypeAllSyn("weaponAllSyn", code, text))    mec<<"weaponAllSyn";
    if(isCardTypeAllSyn("locationAllSyn", code, text))  mec<<"locationAllSyn";

    return mec;
}


void CardTypeCounter::getDirectLinkSynergies(const QString &code, const QMap<QString, QList<QString>> &directLinks, QMap<QString, int> &synergies)
{
    if(directLinks.contains(code))
    {
        const QList<QString> linkCodes = directLinks[code];

        for(const QString &linkCode: linkCodes)
        {
            if(cardTypeCounters[V_MINION]->insertCode(linkCode, synergies)){}
            else if(cardTypeCounters[V_WEAPON]->insertCode(linkCode, synergies)){}
            else cardTypeCounters[V_SPELL]->insertCode(linkCode, synergies);
        }
    }
}


void CardTypeCounter::getCardTypeCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap)
{
    spellMap = cardTypeCounters[V_SPELL]->getCodeTagMap();
    minionMap = cardTypeCounters[V_MINION]->getCodeTagMap();
    weaponMap = cardTypeCounters[V_WEAPON]->getCodeTagMap();
}


void CardTypeCounter::getCardTypeSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                           const QString &text, CardType cardType)
{
    //Evita mostrar spellSyn/spellAllSyn cards en cada hechizo que veamos, es sinergia debil
    // if(cardType == SPELL)
    // {
    //    cardTypeCounters[V_SPELL]->insertSynCards(synergyTagMap);
    //    cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
    // }
    // else if(isSpellGen(code))                   cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
    // if(cardType == MINION)
    // {
    //     cardTypeCounters[V_MINION]->insertSynCards(synergyTagMap);
    //     cardTypeCounters[V_MINION_ALL]->insertSynCards(synergyTagMap);
    // }
    // else if(isMinionGen(code))                  cardTypeCounters[V_MINION_ALL]->insertSynCards(synergyTagMap);
    if(isCardType(WEAPON, cardType))
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isCardTypeGen("weaponGen", code, text))
    {
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);
    }
    if(isCardType(LOCATION, cardType))
    {
        cardTypeCounters[V_LOCATION]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_LOCATION_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isCardTypeGen("locationGen", code, text))
    {
        cardTypeCounters[V_LOCATION_ALL]->insertSynCards(synergyTagMap);
    }


    if(isCardTypeSyn("spellSyn", code))                     cardTypeCounters[V_SPELL]->insertCards(synergyTagMap);
    else if(isCardTypeAllSyn("spellAllSyn", code, text))    cardTypeCounters[V_SPELL_ALL]->insertCards(synergyTagMap);
    if(isCardTypeSyn("minionSyn", code))                    cardTypeCounters[V_MINION]->insertCards(synergyTagMap);
    else if(isCardTypeAllSyn("minionAllSyn", code, text))   cardTypeCounters[V_MINION_ALL]->insertCards(synergyTagMap);
    if(isCardTypeSyn("weaponSyn", code))                    cardTypeCounters[V_WEAPON]->insertCards(synergyTagMap);
    else if(isCardTypeAllSyn("weaponAllSyn", code, text))   cardTypeCounters[V_WEAPON_ALL]->insertCards(synergyTagMap);
    if(isCardTypeSyn("locationSyn", code))                  cardTypeCounters[V_LOCATION]->insertCards(synergyTagMap);
    else if(isCardTypeAllSyn("locationAllSyn", code, text)) cardTypeCounters[V_LOCATION_ALL]->insertCards(synergyTagMap);
}


void CardTypeCounter::updateCardTypeCounters(DeckCard &deckCard, QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(isCardType(SPELL, cardType))
    {
        cardTypeCounters[V_SPELL]->increase(code);
        cardTypeCounters[V_SPELL_ALL]->increase(code);
        spellMap.insertMulti(code, "");
    }
    else if(isCardTypeGen("spellGen", code, text))
    {
        cardTypeCounters[V_SPELL_ALL]->increase(code);
        cardTypeCounters[V_SPELL]->increaseExtra(code);
        spellMap.insertMulti(code, ".");
    }
    if(isCardType(MINION, cardType))
    {
        cardTypeCounters[V_MINION]->increase(code);
        cardTypeCounters[V_MINION_ALL]->increase(code);
        minionMap.insertMulti(code, "");
    }
    else if(isCardTypeGen("minionGen", code, text))
    {
        cardTypeCounters[V_MINION_ALL]->increase(code);
        cardTypeCounters[V_MINION]->increaseExtra(code);
        minionMap.insertMulti(code, ".");
    }
    if(isCardType(WEAPON, cardType))
    {
        cardTypeCounters[V_WEAPON]->increase(code);
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
        weaponMap.insertMulti(code, "");
    }
    else if(isCardTypeGen("weaponGen", code, text))
    {
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
        cardTypeCounters[V_WEAPON]->increaseExtra(code);
        weaponMap.insertMulti(code, ".");
    }
    if(isCardType(LOCATION, cardType))
    {
        cardTypeCounters[V_LOCATION]->increase(code);
        cardTypeCounters[V_LOCATION_ALL]->increase(code);
    }
    else if(isCardTypeGen("locationGen", code, text))
    {
        cardTypeCounters[V_LOCATION_ALL]->increase(code);
    }


    if(isCardTypeSyn("spellSyn", code))                     cardTypeCounters[V_SPELL]->increaseSyn(code);
    else if(isCardTypeAllSyn("spellAllSyn", code, text))    cardTypeCounters[V_SPELL_ALL]->increaseSyn(code);
    if(isCardTypeSyn("minionSyn", code))                    cardTypeCounters[V_MINION]->increaseSyn(code);
    else if(isCardTypeAllSyn("minionAllSyn", code, text))   cardTypeCounters[V_MINION_ALL]->increaseSyn(code);
    if(isCardTypeSyn("weaponSyn", code))                    cardTypeCounters[V_WEAPON]->increaseSyn(code);
    else if(isCardTypeAllSyn("weaponAllSyn", code, text))   cardTypeCounters[V_WEAPON_ALL]->increaseSyn(code);
    if(isCardTypeSyn("locationSyn", code))                  cardTypeCounters[V_LOCATION]->increaseSyn(code);
    else if(isCardTypeAllSyn("locationAllSyn", code, text)) cardTypeCounters[V_LOCATION_ALL]->increaseSyn(code);
}


bool CardTypeCounter::isCardType(CardType key, CardType cardType)
{
    if(key == MINION)   return (cardType == MINION || cardType == HERO || cardType == LOCATION);
    return (cardType == key);
}
bool CardTypeCounter::isCardTypeGen(const QString &key, const QString &code, const QString &text)
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
bool CardTypeCounter::isCardTypeSyn(const QString &key, const QString &code)
{
    //spellSyn minionSyn weaponSyn locationSyn
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}
bool CardTypeCounter::isCardTypeAllSyn(const QString &key, const QString &code, const QString &text)
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

