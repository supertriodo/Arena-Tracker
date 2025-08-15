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
    static DraftItemCounter **cardTypeCounters;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
public:
    static DraftItemCounter **createCardTypeCounters(QObject *parent, QGridLayout *mechanicsLayout);
    static void deleteCardTypeCounters();
    static void setTheme();
    static void resetAll();
    static void setHidden(bool hide);
    static void setTransparency(Transparency transparency, bool mouseInApp);
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static int draftedCardsCount();
    static QStringList debugCardTypeSynergies(const QString &code, const QString &text);
    static void getDirectLinkSynergies(const QString &code, const QMap<QString, QList<QString> > &directLinks, QMap<QString, int> &synergies);
    static void getCardTypeCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap);
    static void getCardTypeSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, const QString &text, CardType cardType);
    static void updateCardTypeCounters(const QString &code, QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                                       const QString &text, CardType cardType);

private:
    static bool isCardType(CardType key, CardType cardType);
    static bool isCardTypeGen(const QString &key, const QString &code, const QString &text);
    static bool isCardTypeSyn(const QString &key, const QString &code);
    static bool isCardTypeAllSyn(const QString &key, const QString &code, const QString &text);
};

#endif // CARDTYPECOUNTER_H
