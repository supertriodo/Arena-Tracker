#include "schoolcounter.h"
#include "keysynergies.h"

DraftItemCounter ** SchoolCounter::schoolCounters;
QMap<QString, QList<QString>> * SchoolCounter::synergyCodes;

SchoolCounter::SchoolCounter()
{

}


DraftItemCounter ** SchoolCounter::createSchoolCounters(QObject *parent)
{
    schoolCounters = new DraftItemCounter *[V_NUM_SCHOOLS];
    schoolCounters[V_ARCANE] = new DraftItemCounter(parent, "Arcane");
    schoolCounters[V_FEL] = new DraftItemCounter(parent, "Fel");
    schoolCounters[V_FIRE] = new DraftItemCounter(parent, "Fire");
    schoolCounters[V_FROST] = new DraftItemCounter(parent, "Frost");
    schoolCounters[V_HOLY] = new DraftItemCounter(parent, "Holy");
    schoolCounters[V_SHADOW] = new DraftItemCounter(parent, "Shadow");
    schoolCounters[V_NATURE] = new DraftItemCounter(parent, "Nature");

    schoolCounters[V_ARCANE_ALL] = new DraftItemCounter(parent, "Arcane");
    schoolCounters[V_FEL_ALL] = new DraftItemCounter(parent, "Fel");
    schoolCounters[V_FIRE_ALL] = new DraftItemCounter(parent, "Fire");
    schoolCounters[V_FROST_ALL] = new DraftItemCounter(parent, "Frost");
    schoolCounters[V_HOLY_ALL] = new DraftItemCounter(parent, "Holy");
    schoolCounters[V_SHADOW_ALL] = new DraftItemCounter(parent, "Shadow");
    schoolCounters[V_NATURE_ALL] = new DraftItemCounter(parent, "Nature");

    return schoolCounters;
}


void SchoolCounter::deleteSchoolCounters()
{
    for(int i=0; i<V_NUM_SCHOOLS; i++)
    {
        delete schoolCounters[i];
    }
    delete []schoolCounters;
}


void SchoolCounter::resetAll()
{
    for(int i=0; i<V_NUM_SCHOOLS; i++)
    {
        schoolCounters[i]->reset();
    }
}


void SchoolCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    SchoolCounter::synergyCodes = synergyCodes;
}


QStringList SchoolCounter::debugSchoolSynergies(const QString &code, const QString &text)
{
    QStringList mec;

    if(isArcaneAllSyn(code, text))          mec<<"arcaneAllSyn";
    if(isFelAllSyn(code, text))             mec<<"felAllSyn";
    if(isFireAllSyn(code, text))            mec<<"fireAllSyn";
    if(isFrostAllSyn(code, text))           mec<<"frostAllSyn";
    if(isHolyAllSyn(code, text))            mec<<"holyAllSyn";
    if(isShadowAllSyn(code, text))          mec<<"shadowAllSyn";
    if(isNatureAllSyn(code, text))          mec<<"natureAllSyn";

    return mec;
}


void SchoolCounter::getSchoolSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                       const QString &text, const CardSchool &cardSchool)
{
    if(cardSchool == ARCANE)
    {
        schoolCounters[V_ARCANE]->insertSynCards(synergyTagMap);
        schoolCounters[V_ARCANE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isArcaneGen(code))      schoolCounters[V_ARCANE_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FEL)
    {
        schoolCounters[V_FEL]->insertSynCards(synergyTagMap);
        schoolCounters[V_FEL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFelGen(code))       schoolCounters[V_FEL_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FIRE)
    {
        schoolCounters[V_FIRE]->insertSynCards(synergyTagMap);
        schoolCounters[V_FIRE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFireGen(code))        schoolCounters[V_FIRE_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FROST)
    {
        schoolCounters[V_FROST]->insertSynCards(synergyTagMap);
        schoolCounters[V_FROST_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFrostGen(code))   schoolCounters[V_FROST_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == HOLY)
    {
        schoolCounters[V_HOLY]->insertSynCards(synergyTagMap);
        schoolCounters[V_HOLY_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isHolyGen(code))       schoolCounters[V_HOLY_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == SHADOW)
    {
        schoolCounters[V_SHADOW]->insertSynCards(synergyTagMap);
        schoolCounters[V_SHADOW_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isShadowGen(code))       schoolCounters[V_SHADOW_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == NATURE)
    {
        schoolCounters[V_NATURE]->insertSynCards(synergyTagMap);
        schoolCounters[V_NATURE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isNatureGen(code))      schoolCounters[V_NATURE_ALL]->insertSynCards(synergyTagMap);

    if(isArcaneSyn(code))                           schoolCounters[V_ARCANE]->insertCards(synergyTagMap);
    else if(isArcaneAllSyn(code, text))             schoolCounters[V_ARCANE_ALL]->insertCards(synergyTagMap);
    if(isFelSyn(code))                              schoolCounters[V_FEL]->insertCards(synergyTagMap);
    else if(isFelAllSyn(code, text))                schoolCounters[V_FEL_ALL]->insertCards(synergyTagMap);
    if(isFireSyn(code))                             schoolCounters[V_FIRE]->insertCards(synergyTagMap);
    else if(isFireAllSyn(code, text))               schoolCounters[V_FIRE_ALL]->insertCards(synergyTagMap);
    if(isFrostSyn(code))                            schoolCounters[V_FROST]->insertCards(synergyTagMap);
    else if(isFrostAllSyn(code, text))              schoolCounters[V_FROST_ALL]->insertCards(synergyTagMap);
    if(isHolySyn(code))                             schoolCounters[V_HOLY]->insertCards(synergyTagMap);
    else if(isHolyAllSyn(code, text))               schoolCounters[V_HOLY_ALL]->insertCards(synergyTagMap);
    if(isShadowSyn(code))                           schoolCounters[V_SHADOW]->insertCards(synergyTagMap);
    else if(isShadowAllSyn(code, text))             schoolCounters[V_SHADOW_ALL]->insertCards(synergyTagMap);
    if(isNatureSyn(code))                           schoolCounters[V_NATURE]->insertCards(synergyTagMap);
    else if(isNatureAllSyn(code, text))             schoolCounters[V_NATURE_ALL]->insertCards(synergyTagMap);
}


void SchoolCounter::updateSchoolCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardSchool cardSchool = deckCard.getSchool();

    if(cardSchool == ARCANE)
    {
        schoolCounters[V_ARCANE]->increase(code);
        schoolCounters[V_ARCANE_ALL]->increase(code);
    }
    else if(isArcaneGen(code))      schoolCounters[V_ARCANE_ALL]->increase(code);
    if(cardSchool == FEL)
    {
        schoolCounters[V_FEL]->increase(code);
        schoolCounters[V_FEL_ALL]->increase(code);
    }
    else if(isFelGen(code))         schoolCounters[V_FEL_ALL]->increase(code);
    if(cardSchool == FIRE)
    {
        schoolCounters[V_FIRE]->increase(code);
        schoolCounters[V_FIRE_ALL]->increase(code);
    }
    else if(isFireGen(code))        schoolCounters[V_FIRE_ALL]->increase(code);
    if(cardSchool == FROST)
    {
        schoolCounters[V_FROST]->increase(code);
        schoolCounters[V_FROST_ALL]->increase(code);
    }
    else if(isFrostGen(code))       schoolCounters[V_FROST_ALL]->increase(code);
    if(cardSchool == HOLY)
    {
        schoolCounters[V_HOLY]->increase(code);
        schoolCounters[V_HOLY_ALL]->increase(code);
    }
    else if(isHolyGen(code))        schoolCounters[V_HOLY_ALL]->increase(code);
    if(cardSchool == SHADOW)
    {
        schoolCounters[V_SHADOW]->increase(code);
        schoolCounters[V_SHADOW_ALL]->increase(code);
    }
    else if(isShadowGen(code))      schoolCounters[V_SHADOW_ALL]->increase(code);
    if(cardSchool == NATURE)
    {
        schoolCounters[V_NATURE]->increase(code);
        schoolCounters[V_NATURE_ALL]->increase(code);
    }
    else if(isNatureGen(code))      schoolCounters[V_NATURE_ALL]->increase(code);

    if(isArcaneSyn(code))                           schoolCounters[V_ARCANE]->increaseSyn(code);
    else if(isArcaneAllSyn(code, text))             schoolCounters[V_ARCANE_ALL]->increaseSyn(code);
    if(isFelSyn(code))                              schoolCounters[V_FEL]->increaseSyn(code);
    else if(isFelAllSyn(code, text))                schoolCounters[V_FEL_ALL]->increaseSyn(code);
    if(isFireSyn(code))                             schoolCounters[V_FIRE]->increaseSyn(code);
    else if(isFireAllSyn(code, text))               schoolCounters[V_FIRE_ALL]->increaseSyn(code);
    if(isFrostSyn(code))                            schoolCounters[V_FROST]->increaseSyn(code);
    else if(isFrostAllSyn(code, text))              schoolCounters[V_FROST_ALL]->increaseSyn(code);
    if(isHolySyn(code))                             schoolCounters[V_HOLY]->increaseSyn(code);
    else if(isHolyAllSyn(code, text))               schoolCounters[V_HOLY_ALL]->increaseSyn(code);
    if(isShadowSyn(code))                           schoolCounters[V_SHADOW]->increaseSyn(code);
    else if(isShadowAllSyn(code, text))             schoolCounters[V_SHADOW_ALL]->increaseSyn(code);
    if(isNatureSyn(code))                           schoolCounters[V_NATURE]->increaseSyn(code);
    else if(isNatureAllSyn(code, text))             schoolCounters[V_NATURE_ALL]->increaseSyn(code);
}


//Gen
bool SchoolCounter::isArcaneGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("arcaneGen");
    return false;
}
bool SchoolCounter::isFelGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("felGen");
    return false;
}
bool SchoolCounter::isFireGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("fireGen");
    return false;
}
bool SchoolCounter::isFrostGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("frostGen");
    return false;
}
bool SchoolCounter::isHolyGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("holyGen");
    return false;
}
bool SchoolCounter::isShadowGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("shadowGen");
    return false;
}
bool SchoolCounter::isNatureGen(const QString &code)
{
    if(synergyCodes->contains(code)) return (*synergyCodes)[code].contains("natureGen");
    return false;
}


//Syn
bool SchoolCounter::isArcaneSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("arcaneSyn");
    }
    return false;
}
bool SchoolCounter::isArcaneAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("arcaneAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "arcane spell");
    }
}
bool SchoolCounter::isFelSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("felSyn");
    }
    return false;
}
bool SchoolCounter::isFelAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("felAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "fel spell");
    }
}
bool SchoolCounter::isFireSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("fireSyn");
    }
    return false;
}
bool SchoolCounter::isFireAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("fireAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "fire spell");
    }
}
bool SchoolCounter::isFrostSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("frostSyn");
    }
    return false;
}
bool SchoolCounter::isFrostAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("frostAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "frost spell");
    }
}
bool SchoolCounter::isHolySyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("holySyn");
    }
    return false;
}
bool SchoolCounter::isHolyAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("holyAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "holy spell");
    }
}
bool SchoolCounter::isShadowSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("shadowSyn");
    }
    return false;
}
bool SchoolCounter::isShadowAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("shadowAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "shadow spell");
    }
}
bool SchoolCounter::isNatureSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("natureSyn");
    }
    return false;
}
bool SchoolCounter::isNatureAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("natureAllSyn");
    }
    else
    {
        return KeySynergies::containsAll(text, "nature spell");
    }
}
