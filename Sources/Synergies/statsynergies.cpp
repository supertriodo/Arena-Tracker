#include "statsynergies.h"
#include "qdebug.h"
#include <Sources/utility.h>

#define MAX_STAT 20

StatSynergies StatSynergies::costMinions, StatSynergies::attackMinions, StatSynergies::healthMinions, StatSynergies::costSpells;
StatSynergies StatSynergies::costWeapons, StatSynergies::attackWeapons, StatSynergies::healthWeapons;
QMap<QString, QList<QString>> * StatSynergies::synergyCodes;
QRegularExpressionMatch StatSynergies::match;

StatSynergies::StatSynergies()
{

}


void StatSynergies::reset()
{
    this->statsMap.clear();
    this->statsSynMap.clear();
}


void StatSynergies::increaseSynM(const StatSyn &statSyn, const QString &code)
{
    switch(statSyn.op)
    {
        case S_EQUAL:
            increaseSyn(statSyn.statValue, code);
        break;
        case S_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                increaseSyn(i, code);
            }
        break;
        case S_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= MAX_STAT; i++)
            {
                increaseSyn(i, code);
            }
        break;
    }
}


void StatSynergies::increase(int statValue, const QString &code)
{
    increaseX(false, statValue, code);
}


void StatSynergies::increaseSyn(int statValue, const QString &code)
{
    increaseX(true, statValue, code);
}


void StatSynergies::increaseX(bool appendToSyn, int statValue, const QString &code)
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


//Tipico insertCardsX()
void StatSynergies::insertCardsM(const StatSyn &statSyn, QMap<QString,int> &synergies)
{
    switch(statSyn.op)
    {
        case S_EQUAL:
            insertCards(statSyn.statValue, synergies);
        break;
        case S_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                insertCards(i, synergies);
            }
        break;
        case S_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= MAX_STAT; i++)
            {
                insertCards(i, synergies);
            }
        break;
    }
}


void StatSynergies::insertCards(int statValue, QMap<QString,int> &synergies)
{
    insertCardsX(false, statValue, synergies);
}


void StatSynergies::insertSynCards(int statValue, QMap<QString,int> &synergies)
{
    insertCardsX(true, statValue, synergies);
}


//Tipico insertCards() o insertSynCards()
void StatSynergies::insertCardsX(bool insertSyn, int statValue, QMap<QString,int> &synergies)
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


StatSyn StatSynergies::getStatSyn(const QString &mechanic)
{
    StatSyn statSyn;

    static const auto regex = QRegularExpression("^=([<>]?)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)([0-9]*)$");
    if(mechanic.contains(regex, &match))
    {
        QString op = match.captured(1);
        QString syngen = match.captured(2);
        QString cardType = match.captured(3);
        QString stat = match.captured(4);
        int value = match.captured(5).toInt();

        if(op.isNull())         statSyn.op = S_EQUAL;
        else if(op == '<')      statSyn.op = S_LOWER_IGUAL;
        else/* if(op == '>')*/  statSyn.op = S_HIGHER_EQUAL;

        if(syngen == "Gen")             statSyn.isGen = true;
        else/* if(syngen == "Syn")*/    statSyn.isGen = false;

        if(cardType == "Minion")            statSyn.cardType = S_MINION;
        else if(cardType == "Spell")        statSyn.cardType = S_SPELL;
        else/* if(cardType == "Weapon")*/   statSyn.cardType = S_WEAPON;

        if(stat == "Cost")              statSyn.statKind = S_COST;
        else if(stat == "Attack")       statSyn.statKind = S_ATTACK;
        else/* if(stat == "Health")*/   statSyn.statKind = S_HEALTH;

        statSyn.statValue = value;

        qDebug()<<op<<syngen<<cardType<<stat<<value;
    }
    else
    {
        statSyn.statValue = -1;
    }
    return statSyn;
}


QList<StatSyn> StatSynergies::getStatsSynergiesFromJson(const QString &code)
{
    QList<StatSyn> statSynList;

    if(!synergyCodes->contains(code))    return statSynList;
    for(const QString &mechanic: qAsConst((*synergyCodes)[code]))
    {
        const StatSyn statSyn = getStatSyn(mechanic);
        if(statSyn.statValue != -1) statSynList.append(statSyn);
    }
    return statSynList;
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


void StatSynergies::updateStatsSynergies(const QString &code, CardType cardType, int attack, int health, int cost)
{
    if(cardType == MINION)
    {
        costMinions.increase(cost, code);
        attackMinions.increase(attack, code);
        healthMinions.increase(health, code);
    }
    else if(cardType == SPELL)
    {
        costSpells.increase(cost, code);
    }
    else if(cardType == WEAPON)
    {
        costWeapons.increase(cost, code);
        attackWeapons.increase(attack, code);
        healthWeapons.increase(health, code);
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
                if(statSyn.isGen)   costMinions.increase(statSyn.statValue, code);
                else                costMinions.increaseSynM(statSyn, code);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackMinions.increase(statSyn.statValue, code);
                else                attackMinions.increaseSynM(statSyn, code);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthMinions.increase(statSyn.statValue, code);
                else                healthMinions.increaseSynM(statSyn, code);
                break;
            }
            break;
        case S_SPELL:
            switch(statSyn.statKind)
            {
            case S_COST:
                if(statSyn.isGen)   costSpells.increase(statSyn.statValue, code);
                else                costSpells.increaseSynM(statSyn, code);
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
                if(statSyn.isGen)   costWeapons.increase(statSyn.statValue, code);
                else                costWeapons.increaseSynM(statSyn, code);
                break;
            case S_ATTACK:
                if(statSyn.isGen)   attackWeapons.increase(statSyn.statValue, code);
                else                attackWeapons.increaseSynM(statSyn, code);
                break;
            case S_HEALTH:
                if(statSyn.isGen)   healthWeapons.increase(statSyn.statValue, code);
                else                healthWeapons.increaseSynM(statSyn, code);
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


void StatSynergies::getStatsSynergiesFromStatSyn(const StatSyn &statSyn, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    switch(statSyn.cardType)
    {
    case S_MINION:
        switch(statSyn.statKind)
        {
        case S_COST:
            if(statSyn.isGen)   costMinions.insertSynCards(statSyn.statValue, synergyTagMap["Cost"]);
            else                costMinions.insertCardsM(statSyn, synergyTagMap["Cost"]);
            break;
        case S_ATTACK:
            if(statSyn.isGen)   attackMinions.insertSynCards(statSyn.statValue, synergyTagMap["Attack"]);
            else                attackMinions.insertCardsM(statSyn, synergyTagMap["Attack"]);
            break;
        case S_HEALTH:
            if(statSyn.isGen)   healthMinions.insertSynCards(statSyn.statValue, synergyTagMap["Health"]);
            else                healthMinions.insertCardsM(statSyn, synergyTagMap["Health"]);
            break;
        }
        break;
    case S_SPELL:
        switch(statSyn.statKind)
        {
        case S_COST:
            if(statSyn.isGen)   costSpells.insertSynCards(statSyn.statValue, synergyTagMap["Cost"]);
            else                costSpells.insertCardsM(statSyn, synergyTagMap["Cost"]);
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
            if(statSyn.isGen)   costWeapons.insertSynCards(statSyn.statValue, synergyTagMap["Cost"]);
            else                costWeapons.insertCardsM(statSyn, synergyTagMap["Cost"]);
            break;
        case S_ATTACK:
            if(statSyn.isGen)   attackWeapons.insertSynCards(statSyn.statValue, synergyTagMap["Attack"]);
            else                attackWeapons.insertCardsM(statSyn, synergyTagMap["Attack"]);
            break;
        case S_HEALTH:
            if(statSyn.isGen)   healthWeapons.insertSynCards(statSyn.statValue, synergyTagMap["Health"]);
            else                healthWeapons.insertCardsM(statSyn, synergyTagMap["Health"]);
            break;
        }
        break;
    }
}


void StatSynergies::getStatsSynergies(const QString &code, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                           CardType cardType, int attack, int health, int cost)
{
    if(cardType == MINION)
    {
        costMinions.insertSynCards(cost, synergyTagMap["Cost"]);
        attackMinions.insertSynCards(attack, synergyTagMap["Attack"]);
        healthMinions.insertSynCards(health, synergyTagMap["Health"]);
    }
    else if(cardType == SPELL)
    {
        costSpells.insertSynCards(cost, synergyTagMap["Cost"]);
    }
    else if(cardType == WEAPON)
    {
        costWeapons.insertSynCards(cost, synergyTagMap["Cost"]);
        attackWeapons.insertSynCards(attack, synergyTagMap["Attack"]);
        healthWeapons.insertSynCards(health, synergyTagMap["Health"]);
    }

    //Synergies
    const QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code);
    for(const StatSyn &statSyn: statSyns)
    {
        getStatsSynergiesFromStatSyn(statSyn, synergyTagMap);
    }
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void StatSynergies::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
{
    const StatSyn statSyn = getStatSyn(partSynergy);
    getStatsSynergiesFromStatSyn(statSyn, synergyTagMap);
}


//Usada por LayeredSynergies para verificar que el code hace sinergia con cada una de las partSynergy
//Suponemos que no se pueden usar synergias Gen
bool StatSynergies::isPartKey(const QString &partSynergy, QString &partSynergyTag,
                             CardType cardType, int attack, int health, int cost)
{
    const StatSyn statSyn = getStatSyn(partSynergy);
    switch(statSyn.cardType)
    {
    case S_MINION:
        if(cardType != MINION)  return false;
        break;
    case S_SPELL:
        if(cardType != SPELL)   return false;
        break;
    case S_WEAPON:
        if(cardType != WEAPON)  return false;
        break;
    }

    int cardValue;
    switch(statSyn.statKind)
    {
    case S_COST:
        cardValue = cost;
        partSynergyTag = "Cost";
        break;
    case S_ATTACK:
        cardValue = attack;
        partSynergyTag = "Attack";
        break;
    case S_HEALTH:
        cardValue = health;
        partSynergyTag = "Health";
        break;
    }

    int targetValue = statSyn.statValue;
    switch (statSyn.op)
    {
    case S_EQUAL:
        return cardValue == targetValue;
    case S_HIGHER_EQUAL:
        return cardValue >= targetValue;
    case S_LOWER_IGUAL:
        return cardValue <= targetValue;
    }

    return false;
}
