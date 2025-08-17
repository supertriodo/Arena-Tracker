#include "statsynergies.h"
#include "qdebug.h"
#include <Sources/utility.h>

#define MAX_STAT 20

StatSynergies StatSynergies::costMinions, StatSynergies::attackMinions, StatSynergies::healthMinions, StatSynergies::costSpells;
StatSynergies StatSynergies::costWeapons, StatSynergies::attackWeapons, StatSynergies::healthWeapons;
QMap<QString, QList<QString>> * StatSynergies::synergyCodes;

StatSynergies::StatSynergies()
{

}


void StatSynergies::reset()
{
    this->statsMap.clear();
    this->statsSynMap.clear();
}


//Tipico increaseSyn()
void StatSynergies::updateStatsMapSyn(const StatSyn &statSyn, const QString &code)
{
    switch(statSyn.op)
    {
        case S_EQUAL:
            appendStatValue(true, statSyn.statValue, code);
        break;
        case S_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                appendStatValue(true, i, code);
            }
        break;
        case S_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= MAX_STAT; i++)
            {
                appendStatValue(true, i, code);
            }
        break;
    }
}


//Tipico increase() o increaseSyn()
void StatSynergies::appendStatValue(bool appendToSyn, int statValue, const QString &code)
{
    if(statValue > MAX_STAT)    statValue = MAX_STAT;
    QMap<int, QMap<QString, int>> &statsMap = (appendToSyn?this->statsSynMap:this->statsMap);

    bool duplicatedCard = false;
    const auto codes = statsMap[statValue].keys();
    for(const QString &co: qAsConst(codes))
    {
        if(co == code)
        {
            statsMap[statValue][co]++;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        statsMap[statValue][code] = 1;
    }
}


//Tipico insertCards()
void StatSynergies::insertStatCards(const StatSyn &statSyn, QMap<QString,int> &synergies)
{
    switch(statSyn.op)
    {
        case S_EQUAL:
            insertCards(false, statSyn.statValue, synergies);
        break;
        case S_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                insertCards(false, i, synergies);
            }
        break;
        case S_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= MAX_STAT; i++)
            {
                insertCards(false, i, synergies);
            }
        break;
    }
}


//Tipico insertCards() o insertSynCards()
void StatSynergies::insertCards(bool insertSyn, int statValue, QMap<QString,int> &synergies)
{
    if(statValue > MAX_STAT)    statValue = MAX_STAT;
    QMap<int, QMap<QString, int>> &statsMap = (insertSyn?this->statsSynMap:this->statsMap);
    if(!statsMap.contains(statValue))    return;

    const auto codes = statsMap[statValue].keys();
    for(const QString &code: qAsConst(codes))
    {
        if(!synergies.contains(code))
        {
            synergies[code] = statsMap[statValue][code];
        }
    }
}


void StatSynergies::qDebugContents()
{
    qDebug()<<"**StatsCards**";
    const auto keyList = statsMap.keys();
    for(const int key: keyList)
    {
        qDebug()<<'['<<key<<']';
        const auto codes = statsMap[key].keys();
        for(const QString &code: qAsConst(codes))
        {
            qDebug()<<'\t'<<code<<Utility::getCardAttribute(code, "name").toString();
        }
    }

    qDebug()<<"**StatsSynergies**";
    const auto keySynList = statsSynMap.keys();
    for(const int key: keySynList)
    {
        qDebug()<<'['<<key<<']';
        const auto codes = statsSynMap[key].keys();
        for(const QString &code: qAsConst(codes))
        {
            qDebug()<<'\t'<<code<<Utility::getCardAttribute(code, "name").toString();
        }
    }
}


QList<StatSyn> StatSynergies::getStatsSynergiesFromJson(const QString &code)
{
    QList<StatSyn> statSyns;

    if(!synergyCodes->contains(code))    return statSyns;
    for(QString mechanic: (const QList<QString>)(*synergyCodes)[code])
    {
        if(mechanic[0] == '=')
        {
            mechanic.remove(0,1);

            //Operator
            StatSyn statSyn;
            QChar c = mechanic[0];
            if(c == '<')
            {
                statSyn.op = S_LOWER_IGUAL;
                mechanic.remove(0,1);
            }
            else if(c == '>')
            {
                statSyn.op = S_HIGHER_EQUAL;
                mechanic.remove(0,1);
            }
            else    statSyn.op = S_EQUAL;

            //Gen o Syn
            if(mechanic.startsWith("Gen"))
            {
                statSyn.isGen = true;
                mechanic.remove(0,3);
            }
            else if(mechanic.startsWith("Syn"))
            {
                statSyn.isGen = false;
                mechanic.remove(0,3);
            }
            else
            {
                qDebug()<<"WARNING: Synergy Stat failed waiting Gen/Syn:" << mechanic << "Code:" << code;
                break;
            }

            //CardType
            if(mechanic.startsWith("Minion"))
            {
                statSyn.cardType = S_MINION;
                mechanic.remove(0,6);
            }
            else if(mechanic.startsWith("Spell"))
            {
                statSyn.cardType = S_SPELL;
                mechanic.remove(0,5);
            }
            else if(mechanic.startsWith("Weapon"))
            {
                statSyn.cardType = S_WEAPON;
                mechanic.remove(0,6);
            }
            else
            {
                qDebug()<<"WARNING: Synergy Stat failed waiting Minion/Spell/Weapon:" << mechanic << "Code:" << code;
                break;
            }

            //Stat
            if(mechanic.startsWith("Cost"))
            {
                statSyn.statKind = S_COST;
                mechanic.remove(0,4);
            }
            else if(mechanic.startsWith("Attack"))
            {
                statSyn.statKind = S_ATTACK;
                mechanic.remove(0,6);
            }
            else if(mechanic.startsWith("Health"))
            {
                statSyn.statKind = S_HEALTH;
                mechanic.remove(0,6);
            }
            else
            {
                qDebug()<<"WARNING: Synergy Stat failed waiting Cost/Attack/Health:" << mechanic << "Code:" << code;
                break;
            }

            //Value
            statSyn.statValue = mechanic.toInt();
            statSyns.append(statSyn);

            //Error
            if(statSyn.statValue == 0)
            {
                qDebug()<<"WARNING: Synergy Stat failed waiting value if not 0:" << mechanic << "Code:" << code;
            }
        }
    }
    return statSyns;
}


void StatSynergies::resetAll()
{
    costMinions.reset();
    attackMinions.reset();
    healthMinions.reset();
    costSpells.reset();
    costWeapons.reset();
    attackWeapons.reset();
    healthWeapons.reset();
}


void StatSynergies::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    StatSynergies::synergyCodes = synergyCodes;
}


void StatSynergies::updateStatsCards(const QString &code, CardType cardType, int attack, int health, int cost)
{
    if(cardType == MINION)
    {
        costMinions.appendStatValue(false, cost, code);
        attackMinions.appendStatValue(false, attack, code);
        healthMinions.appendStatValue(false, health, code);
    }
    else if(cardType == SPELL)
    {
        costSpells.appendStatValue(false, cost, code);
    }
    else if(cardType == WEAPON)
    {
        costWeapons.appendStatValue(false, cost, code);
        attackWeapons.appendStatValue(false, attack, code);
        healthWeapons.appendStatValue(false, health, code);
    }

    //Synergies
    const QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.cardType)
        {
        case S_MINION:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costMinions.appendStatValue(false, statSyn.statValue, code);
                else                costMinions.updateStatsMapSyn(statSyn, code);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackMinions.appendStatValue(false, statSyn.statValue, code);
                else                attackMinions.updateStatsMapSyn(statSyn, code);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthMinions.appendStatValue(false, statSyn.statValue, code);
                else                healthMinions.updateStatsMapSyn(statSyn, code);
                break;
            }
            break;
        case S_SPELL:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costSpells.appendStatValue(false, statSyn.statValue, code);
                else                costSpells.updateStatsMapSyn(statSyn, code);
                break;
            case S_ATTACK:
            case S_HEALTH:
                break;
            }
            break;
        case S_WEAPON:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costWeapons.appendStatValue(false, statSyn.statValue, code);
                else                costWeapons.updateStatsMapSyn(statSyn, code);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackWeapons.appendStatValue(false, statSyn.statValue, code);
                else                attackWeapons.updateStatsMapSyn(statSyn, code);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthWeapons.appendStatValue(false, statSyn.statValue, code);
                else                healthWeapons.updateStatsMapSyn(statSyn, code);
                break;
            }
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


void StatSynergies::getStatsCardsSynergies(const QString &code, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                           CardType cardType, int attack, int health, int cost)
{
    if(cardType == MINION)
    {
        costMinions.insertCards(true, cost, synergyTagMap["Cost"]);
        attackMinions.insertCards(true, attack, synergyTagMap["Attack"]);
        healthMinions.insertCards(true, health, synergyTagMap["Health"]);
    }
    else if(cardType == SPELL)
    {
        costSpells.insertCards(true, cost, synergyTagMap["Cost"]);
    }
    else if(cardType == WEAPON)
    {
        costWeapons.insertCards(true, cost, synergyTagMap["Cost"]);
        attackWeapons.insertCards(true, attack, synergyTagMap["Attack"]);
        healthWeapons.insertCards(true, health, synergyTagMap["Health"]);
    }

    //Synergies
    const QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.cardType)
        {
        case S_MINION:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costMinions.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                else                costMinions.insertStatCards(statSyn, synergyTagMap["Cost"]);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackMinions.insertCards(true, statSyn.statValue, synergyTagMap["Attack"]);
                else                attackMinions.insertStatCards(statSyn, synergyTagMap["Attack"]);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthMinions.insertCards(true, statSyn.statValue, synergyTagMap["Health"]);
                else                healthMinions.insertStatCards(statSyn, synergyTagMap["Health"]);
                break;
            }
            break;
        case S_SPELL:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costSpells.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                else                costSpells.insertStatCards(statSyn, synergyTagMap["Cost"]);
                break;
            case S_ATTACK:
            case S_HEALTH:
                break;
            }
            break;
        case S_WEAPON:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                else                costWeapons.insertStatCards(statSyn, synergyTagMap["Cost"]);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Attack"]);
                else                attackWeapons.insertStatCards(statSyn, synergyTagMap["Attack"]);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Health"]);
                else                healthWeapons.insertStatCards(statSyn, synergyTagMap["Health"]);
                break;
            }
            break;
        }
    }
}


// //Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
// void KeySynergies::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
// {
//     if(partSynergy.endsWith("AllSyn"))
//     {
//         QString key = partSynergy;
//         key.chop(6);
//         const QString &keyAll = key+"All";

//         if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertCards(synergyTagMap);
//     }
//     else if(partSynergy.endsWith("Syn"))
//     {
//         QString key = partSynergy;
//         key.chop(3);

//         if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertCards(synergyTagMap);
//     }
//     else if(partSynergy.endsWith("Gen"))
//     {
//         QString key = partSynergy;
//         key.chop(3);
//         const QString &keyAll = key+"All";

//         if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
//     }
//     else
//     {
//         QString key = partSynergy;
//         const QString &keyAll = key+"All";

//         if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertSynCards(synergyTagMap);
//         if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
//     }
// }


// //Usada por LayeredSynergies para verificar que el code hace sinergia con cada una de las partSynergy
// bool KeySynergies::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
//                              const QJsonArray &mechanics, const QJsonArray &referencedTags,
//                              const QString &text, CardType cardType, int attack, int cost)
// {
//     if(partSynergy.endsWith("AllSyn"))
//     {
//         QString key = partSynergy;
//         key.chop(6);
//         partSynergyTag = getSynergyTag(key);

//         return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost) ||
//                isKeyGen(key+"Gen", code, mechanics, referencedTags, text, cardType, attack, cost);
//     }
//     else if(partSynergy.endsWith("Syn"))
//     {
//         QString key = partSynergy;
//         key.chop(3);
//         partSynergyTag = getSynergyTag(key);

//         return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost);
//     }
//     else if(partSynergy.endsWith("Gen"))
//     {
//         QString key = partSynergy;
//         key.chop(3);
//         partSynergyTag = getSynergyTag(key);
//         const QString &keyAll = key+"All";

//         return isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost);
//     }
//     else
//     {
//         QString key = partSynergy;
//         partSynergyTag = getSynergyTag(key);
//         const QString &keyAll = key+"All";

//         return isKeySyn(key+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost) ||
//                isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost);
//     }
// }
