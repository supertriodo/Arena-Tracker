#ifndef LAYEREDSYNERGIES_H
#define LAYEREDSYNERGIES_H

#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QString>
#include <QMap>

class LayeredSynergies
{
public:
    LayeredSynergies();
    ~LayeredSynergies();

//Variables
private:
    // layeredSynergy -> map[code]=numCards
    static QMap<QStringList, QMap<QString, int>> layeredSynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    static QList<QStringList> getLayeredSynergiesFromJson(const QString &code);
    static void insertCards(const QString &synergyTag, const QMap<QString, int> &codeMap, QMap<QString, QMap<QString, int> > &synergyTagMap);

public:
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static void reset();
    static void updateLayeredSynergies(const QString &code);
    static void getLayeredSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                    const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                    const QString &text, CardType cardType, int attack, int cost);
};

#endif // LAYEREDSYNERGIES_H
