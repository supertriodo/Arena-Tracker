#include "racecounter.h"

QMap<QString, QList<QString>> * RaceCounter::synergyCodes;

RaceCounter::RaceCounter()
{

}


void RaceCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    RaceCounter::synergyCodes = synergyCodes;
}


bool RaceCounter::isCardType(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                 const QString &text, CardType cardType, int attack, int cost)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
    }

    return false;
}
bool RaceCounter::isCardTypeGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                    const QString &text, CardType cardType, int attack, int cost)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
    }

    return false;
}
bool RaceCounter::isCardTypeSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                    const QString &text, CardType cardType, int attack, int cost)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
    }

    return false;
}
bool RaceCounter::isCardTypeAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text, CardType cardType, int attack, int cost)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
        else if(key == "")
        {

        }
    }

    return false;
}
