#ifndef RACECOUNTER_H
#define RACECOUNTER_H

#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class RaceCounter
{
public:
    RaceCounter();

    //Variables
private:
    static QMap<QString, QList<QString>> *synergyCodes;

    //Metodos
public:
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static bool isCardType(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, CardType cardType, int attack, int cost);
    static bool isCardTypeGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, CardType cardType, int attack, int cost);
    static bool isCardTypeSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, CardType cardType, int attack, int cost);
    static bool isCardTypeAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, CardType cardType, int attack, int cost);
};

#endif // RACECOUNTER_H
