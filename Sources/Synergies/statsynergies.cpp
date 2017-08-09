#include "statsynergies.h"
#include <QtWidgets>

StatSynergies::StatSynergies()
{

}


void StatSynergies::clear()
{
    this->statsMap.clear();
    this->statsMapSyn.clear();
}


void StatSynergies::updateStatsMapSyn(const StatSyn &statSyn, QString &code)
{
    switch(statSyn.op)
    {
        case V_EQUAL:
            appendStatValue(true, statSyn.statValue, code);
        break;
        case V_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                appendStatValue(true, i, code);
            }
        break;
        case V_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= 15; i++)
            {
                appendStatValue(true, i, code);
            }
        break;
    }
}


void StatSynergies::appendStatValue(bool appendToSyn, int statValue, QString &code)
{
    QMap<int, QList<DeckCard>> &statsMap = (appendToSyn?this->statsMapSyn:this->statsMap);

    bool duplicatedCard = false;
    for(DeckCard &deckCard: statsMap[statValue])
    {
        if(deckCard.getCode() == code)
        {
            deckCard.total++;
            deckCard.remaining = deckCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        statsMap[statValue].append(DeckCard(code));
    }
}


void StatSynergies::insertStatCards(const StatSyn &statSyn, QMap<QString,int> &synergies)
{
    switch(statSyn.op)
    {
        case V_EQUAL:
            insertCards(false, statSyn.statValue, synergies);
        break;
        case V_LOWER_IGUAL:
            for(int i = statSyn.statValue; i >= 0; i--)
            {
                insertCards(false, i, synergies);
            }
        break;
        case V_HIGHER_EQUAL:
            for(int i = statSyn.statValue; i <= 15; i++)
            {
                insertCards(false, i, synergies);
            }
        break;
    }
}


void StatSynergies::insertCards(bool insertSyn, int statValue, QMap<QString,int> &synergies)
{
    QMap<int, QList<DeckCard>> &statsMap = (insertSyn?this->statsMapSyn:this->statsMap);
    if(!statsMap.contains(statValue))    return;

    for(DeckCard &deckCard: statsMap[statValue])
    {
        QString code = deckCard.getCode();
        if(!synergies.contains(code))
        {
            synergies[code] = deckCard.total;
        }
    }
}


void StatSynergies::qDebugContents()
{
    qDebug()<<"**StatsMinions**";
    for(const int key: statsMap.keys())
    {
        qDebug()<<'['<<key<<']'<<endl;
        for(DeckCard &deckCard: statsMap[key])
        {
            qDebug()<<'\t'<<deckCard.getName();
        }
    }

    qDebug()<<"**StatsSynergies**";
    for(const int key: statsMapSyn.keys())
    {
        qDebug()<<'['<<key<<']'<<endl;
        for(DeckCard &deckCard: statsMapSyn[key])
        {
            qDebug()<<'\t'<<deckCard.getName();
        }
    }
}


QList<StatSyn> StatSynergies::getStatsSynergiesFromJson(const QString &code, QMap<QString, QList<QString>> &synergyCodes)
{
    QList<StatSyn> statSyns;

    if(!synergyCodes.contains(code))    return statSyns;
    for(QString mechanic: synergyCodes[code])
    {
        if(mechanic[0] == '=')
        {
            mechanic.remove(0,1);

            //Operator
            StatSyn statSyn;
            QChar c = mechanic[0];
            if(c == '<')
            {
                statSyn.op = V_LOWER_IGUAL;
                mechanic.remove(0,1);
            }
            else if(c == '>')
            {
                statSyn.op = V_HIGHER_EQUAL;
                mechanic.remove(0,1);
            }
            else    statSyn.op = V_EQUAL;

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

            //Stat
            if(mechanic.startsWith("Cost"))
            {
                statSyn.statKind = V_COST;
                mechanic.remove(0,4);
            }
            else if(mechanic.startsWith("Attack"))
            {
                statSyn.statKind = V_ATTACK;
                mechanic.remove(0,6);
            }
            else if(mechanic.startsWith("Health"))
            {
                statSyn.statKind = V_HEALTH;
                mechanic.remove(0,6);
            }

            //Value
            statSyn.statValue = mechanic.toInt();
            statSyns.append(statSyn);
        }
    }
    return statSyns;
}
