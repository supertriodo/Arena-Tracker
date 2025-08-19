#ifndef MECHANICCOUNTER_H
#define MECHANICCOUNTER_H

#include "Sources/Synergies/draftitemcounter.h"
#include "Sources/synergyhandler.h"
#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class MechanicCounter
{
    friend SynergyHandler;

public:
    MechanicCounter();

//Variables
private:
    static QMap<QString, DraftItemCounter*> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    static QMap<QString, QString> getMapKeySynergies();
    static bool hasKeyAll(const QString &key);
    static QString getSynergyTag(const QString &key);

    static bool isKey(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                      const QString &text, CardType cardType, int attack, int cost);
    static bool isKeyGen(const QString &key, const QString &code, const QJsonArray &referencedTags);
    static bool isKeySyn(const QString &key, const QString &code);
    static bool isKeyAllSyn(const QString &key, const QString &code);

    static int numDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    static int numDrawGen(const QString &code, const QString &text);
    static int numToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    static bool isDiscoverDrawGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    static bool isDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    static bool isDrawGen(const QString &code, const QString &text);
    static bool isToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    static bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const CardType &cardType);
    static bool isDeathrattleGoodAllSyn(const QString &code, const QString &text);

public:
    static QMap<QString, DraftItemCounter *> *createMechanicCounters(QObject *parent, QGridLayout *mechanicsLayout);
    static QStringList getListKeySynergies();
    static QStringList getListValidSynergies();
    static QStringList getListKeyLabels();
    static void setTheme();
    static void resetAll();
    static void setHidden(bool hide);
    static void setTransparency(Transparency transparency, bool mouseInApp);
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugMechanicSynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                              const QString &text, CardType cardType, int attack, int cost);
    static void getMechanicCounters(QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                    QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                    QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                    QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                    int &draw, int &toYourHand, int &discover);
    static void updateMechanicCounters(const QString &code,
                                       QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                       QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                       QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                       QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                       int &draw, int &toYourHand, int &discover,
                                       const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text, CardType cardType, int attack, int cost);
    static void getMechanicSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons,
                                     const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                     const QString &text, CardType cardType, int attack, int cost);
    static void getPartKeySynergies(const QString &partSynergy, const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                          const QJsonArray &mechanics, const QJsonArray &referencedTags,
                          const QString &text, CardType cardType, int attack, int cost);

    static bool isAoeGen(const QString &code, const QString &text);
    static bool isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                            const QString &text, const CardType &cardType, int attack);
    static bool isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text);
};

#endif // MECHANICCOUNTER_H
