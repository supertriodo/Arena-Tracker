#include "mechaniccounter.h"

QMap<QString, QList<QString>> * MechanicCounter::synergyCodes;

MechanicCounter::MechanicCounter()
{

}


void MechanicCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    MechanicCounter::synergyCodes = synergyCodes;
}


bool MechanicCounter::isCardType(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool MechanicCounter::isCardTypeGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool MechanicCounter::isCardTypeSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool MechanicCounter::isCardTypeAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
