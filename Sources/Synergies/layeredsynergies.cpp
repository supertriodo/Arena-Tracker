#include "layeredsynergies.h"
#include "cardtypecounter.h"
#include "racecounter.h"
#include "schoolcounter.h"
#include "mechaniccounter.h"
#include "keysynergies.h"
#include "statsynergies.h"
#include <QDebug>
#include <QSet>

QMap<QStringList, QMap<QString, int>> LayeredSynergies::layeredSynergiesMap;
QMap<QString, QList<QString>> * LayeredSynergies::synergyCodes;

LayeredSynergies::LayeredSynergies()
{

}


LayeredSynergies::~LayeredSynergies()
{

}


void LayeredSynergies::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    LayeredSynergies::synergyCodes = synergyCodes;
}


void LayeredSynergies::reset()
{
    LayeredSynergies::layeredSynergiesMap.clear();
}


QList<QStringList> LayeredSynergies::getLayeredSynergiesFromJson(const QString &code)
{
    QList<QStringList> layeredSynergyList;

    const auto synergies = (*synergyCodes)[code];
    for(const QString &synergy: synergies)
    {
        if(synergy.startsWith('&'))
        {
            QStringList layeredSynergy = synergy.split('&', QString::SkipEmptyParts);
            std::sort(layeredSynergy.begin(), layeredSynergy.end());
            layeredSynergyList << layeredSynergy;
        }
    }

    return layeredSynergyList;
}


void LayeredSynergies::updateLayeredSynergies(const QString &code)
{
    //Ya hemos procesado previamente este codigo
    bool found = false;
    for(auto &codeMap: layeredSynergiesMap)
    {
        if(codeMap.contains(code))
        {
            codeMap[code]++;
            found = true;
        }
    }
    if(!found)
    {
        const QList<QStringList> layeredSynergyList = getLayeredSynergiesFromJson(code);
        for(const auto &layeredSynergy: layeredSynergyList)
        {
            QMap<QString, int> codeMap;
            if(layeredSynergiesMap.contains(layeredSynergy))    codeMap = layeredSynergiesMap[layeredSynergy];
            codeMap.insert(code, 1);
            layeredSynergiesMap.insert(layeredSynergy, codeMap);
        }
    }

    // qDebug()<<endl<<"----- UPDATE -----"<<code;
    // for(auto layeredSynergy: layeredSynergiesMap.keys())
    // {
    //     qDebug()<<layeredSynergy<<layeredSynergiesMap[layeredSynergy];
    // }
}


void LayeredSynergies::insertCards(const QString &synergyTag, const QMap<QString, int> &codeMap, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QMap<QString,int> synergies;
    if(synergyTagMap.contains(synergyTag))  synergies = synergyTagMap[synergyTag];

    const QList<QString> codeList = codeMap.keys();
    for(const QString &code: codeList)
    {
        if(!synergies.contains(code))
        {
            synergies.insert(code, codeMap[code]);
        }
    }

    if(!synergies.isEmpty())    synergyTagMap.insert(synergyTag, synergies);
}


void LayeredSynergies::getLayeredSynergies(const QString &code, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                           const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text,
                                           CardType cardType, const QList<CardRace> &cardRace, CardSchool cardSchool,
                                           int attack, int health, int cost)
{
    //Layered synergies del code
    const QList<QStringList> layeredSynergyList = getLayeredSynergiesFromJson(code);
    for(const QStringList &layeredSynergy: layeredSynergyList)
    {
        QMap<QString, QMap<QString, int>> partSynergyTagMap;

        // qDebug()<<endl<<"----- CODE -----"<<code;
        for(const QString &partSynergy: layeredSynergy)
        {
            getPartKeySynergies(partSynergy, code, partSynergyTagMap);
            // qDebug()<<partSynergy<<partSynergyTagMap;
        }
        //En caso de que no haya ningun codigo de una partSynergy
        if(layeredSynergy.count() != partSynergyTagMap.count()) continue;

        bool first = true;
        QString synergyTag = "";
        QSet<QString> intersectionCodes;

        const auto &partSynergyTagList = partSynergyTagMap.keys();
        for(const QString &partSynergyTag: partSynergyTagList)
        {
            const auto &keys = partSynergyTagMap[partSynergyTag].keys();
            QSet<QString> codes = QSet<QString>::fromList(keys);
            if(first)
            {
                intersectionCodes = codes;
                first = false;
            }
            else
            {
                synergyTag += " + ";
                intersectionCodes.intersect(codes);
            }
            synergyTag += partSynergyTag;
        }

        if(!intersectionCodes.isEmpty())
        {
            QMap<QString, int> codeMap;
            QMap<QString, int> &codeMapPart = partSynergyTagMap.first();
            for(const QString &code: qAsConst(intersectionCodes))
            {
                codeMap.insert(code, codeMapPart[code]);
            }
            insertCards(synergyTag, codeMap, synergyTagMap);
        }
    }

    //Layered synergies guardadas en layeredSynergiesMap
    const QList<QStringList> layeredSynergyMapKeys = layeredSynergiesMap.keys();
    for(const QStringList &layeredSynergy: layeredSynergyMapKeys)
    {
        bool fullSynergy = true;
        bool first = true;
        QString synergyTag = "";
        // qDebug()<<endl<<"----- STORED LAYSYN -----";
        for(const QString &partSynergy: layeredSynergy)
        {
            QString partSynergyTag;
            // qDebug()<<partSynergy;
            if(!isPartKey(partSynergy, code, partSynergyTag, mechanics, referencedTags, text, cardType, cardRace, cardSchool, attack, health, cost))
            {
                // qDebug()<<"FALSE";
                fullSynergy = false;
                break;
            }
            else
            {
                // qDebug()<<"TRUE";
                if(first)   first = false;
                else        synergyTag += " + ";
                synergyTag += partSynergyTag;
            }
        }

        if(fullSynergy)
        {
            insertCards(synergyTag, layeredSynergiesMap[layeredSynergy], synergyTagMap);
        }
    }
}


void LayeredSynergies::getPartKeySynergies(const QString &partSynergy, const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap)
{
    if(partSynergy.startsWith("="))
    {
        StatSynergies::getPartKeySynergies(partSynergy, synergyTagMap);
    }
    if(CardTypeCounter::getListValidSynergies().contains(partSynergy))
    {
        CardTypeCounter::getPartKeySynergies(partSynergy, synergyTagMap);
    }
    if(RaceCounter::getListValidSynergies().contains(partSynergy))
    {
        RaceCounter::getPartKeySynergies(partSynergy, synergyTagMap);
    }
    if(SchoolCounter::getListValidSynergies().contains(partSynergy))
    {
        SchoolCounter::getPartKeySynergies(partSynergy, synergyTagMap);
    }
    if(MechanicCounter::getListValidSynergies().contains(partSynergy))
    {
        MechanicCounter::getPartKeySynergies(partSynergy, code, synergyTagMap);
    }
    if(KeySynergies::getListValidSynergies().contains(partSynergy))
    {
        KeySynergies::getPartKeySynergies(partSynergy, synergyTagMap);
    }
}
bool LayeredSynergies::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                                 const QJsonArray &mechanics, const QJsonArray &referencedTags,const QString &text,
                                 CardType cardType, const QList<CardRace> &cardRace, CardSchool cardSchool,
                                 int attack, int health, int cost)
{
    if(partSynergy.startsWith("="))
    {
        return StatSynergies::isPartKey(partSynergy, partSynergyTag, cardType, attack, health, cost);
    }
    if(CardTypeCounter::getListValidSynergies().contains(partSynergy))
    {
        return CardTypeCounter::isPartKey(partSynergy, code, partSynergyTag, text, cardType);
    }
    if(RaceCounter::getListValidSynergies().contains(partSynergy))
    {
        return RaceCounter::isPartKey(partSynergy, code, partSynergyTag, mechanics, text, cardRace);
    }
    if(SchoolCounter::getListValidSynergies().contains(partSynergy))
    {
        return SchoolCounter::isPartKey(partSynergy, code, partSynergyTag, text, cardSchool);
    }
    if(MechanicCounter::getListValidSynergies().contains(partSynergy))
    {
        return MechanicCounter::isPartKey(partSynergy, code, partSynergyTag, mechanics, referencedTags, text, cardType, attack, cost);
    }
    if(KeySynergies::getListValidSynergies().contains(partSynergy))
    {
        return KeySynergies::isPartKey(partSynergy, code, partSynergyTag, mechanics, referencedTags, text, cardType, attack, cost);
    }
    return false;
}












