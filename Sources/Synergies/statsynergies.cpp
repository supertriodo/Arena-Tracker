#include "statsynergies.h"
#include "qdebug.h"
#include <Sources/utility.h>

#define MAX_STAT 20

StatSynergies::StatSynergies()
{

}


void StatSynergies::clear()
{
    this->statsMap.clear();
    this->statsSynMap.clear();
}


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


QList<StatSyn> StatSynergies::getStatsSynergiesFromJson(const QString &code, QMap<QString, QList<QString>> &synergyCodes)
{
    QList<StatSyn> statSyns;

    if(!synergyCodes.contains(code))    return statSyns;
    for(QString mechanic: (const QList<QString>)synergyCodes[code])
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
