#ifndef CARDTYPECOUNTER_H
#define CARDTYPECOUNTER_H

#include "Sources/Synergies/draftitemcounter.h"
#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class CardTypeCounter
{
public:
    CardTypeCounter();

//Variables
private:
    static QMap<QString, DraftItemCounter*> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    static QMap<QString, QString> getMapKeySynergies();
    static QString getSynergyTag(const QString &key);
    static bool isWeakKey(const QString &key);

    static bool isKey(const QString &key, CardType cardType);
    static bool isKeyGen(const QString &key, const QString &code, const QString &text);
    static bool isKeySyn(const QString &key, const QString &code);
    static bool isKeyAllSyn(const QString &key, const QString &code, const QString &text);

public:
    static QMap<QString, DraftItemCounter*> * createCardTypeCounters(QObject *parent, QGridLayout *mechanicsLayout);
    static QStringList getListKeySynergies();
    static QStringList getListKeyLabels();
    static void setTheme();
    static void resetAll();
    static void setHidden(bool hide);
    static void setTransparency(Transparency transparency, bool mouseInApp);
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static int draftedCardsCount();
    static QStringList debugCardTypeSynergies(const QString &code, const QString &text);
    static void getDirectLinkSynergies(const QString &code, const QMap<QString, QList<QString> > &directLinks, QMap<QString, int> &synergies);
    static void getCardTypeCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap);
    static void updateCardTypeCounters(const QString &code, QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                                       const QString &text, CardType cardType);
    static void getCardTypeSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, const QString &text, CardType cardType);
    static void getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                          const QString &text, CardType cardType);
};

#endif // CARDTYPECOUNTER_H
