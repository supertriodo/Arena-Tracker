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
    const QList<int> keyList = statsMap.keys();
    for(const int key: keyList)
    {
        qDebug()<<'['<<key<<']'<<endl;
        for(DeckCard &deckCard: statsMap[key])
        {
            qDebug()<<'\t'<<deckCard.getName();
        }
    }

    qDebug()<<"**StatsSynergies**";
    const QList<int> keySynList = statsMapSyn.keys();
    for(const int key: keySynList)
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
