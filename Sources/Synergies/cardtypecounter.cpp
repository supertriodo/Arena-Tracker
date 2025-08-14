#include "cardtypecounter.h"

QMap<QString, QList<QString>> * CardTypeCounter::synergyCodes;

CardTypeCounter::CardTypeCounter()
{

}


void CardTypeCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    CardTypeCounter::synergyCodes = synergyCodes;
}


bool CardTypeCounter::isCardType(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool CardTypeCounter::isCardTypeGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool CardTypeCounter::isCardTypeSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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
bool CardTypeCounter::isCardTypeAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
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

