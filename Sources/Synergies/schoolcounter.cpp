#include "schoolcounter.h"

QMap<QString, QList<QString>> * SchoolCounter::synergyCodes;

SchoolCounter::SchoolCounter()
{

}


void SchoolCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    SchoolCounter::synergyCodes = synergyCodes;
}


bool SchoolCounter::isCardType(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool SchoolCounter::isCardTypeGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool SchoolCounter::isCardTypeSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool SchoolCounter::isCardTypeAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
