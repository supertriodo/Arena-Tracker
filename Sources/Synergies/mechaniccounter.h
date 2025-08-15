#ifndef MECHANICCOUNTER_H
#define MECHANICCOUNTER_H

#include "Sources/Synergies/draftitemcounter.h"
#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class MechanicCounter
{
public:
    MechanicCounter();

//Variables
private:
    static DraftItemCounter **mechanicCounters;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
public:
    static DraftItemCounter **createMechanicCounters(QObject *parent, QGridLayout *mechanicsLayout);
    static void deleteMechanicCounters();
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
    static void getMechanicSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons,
                                     const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                     const QString &text, CardType cardType, int attack, int cost);
    static void updateMechanicCounters(const QString &code,
                                       QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                       QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                       QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                       QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                       int &draw, int &toYourHand, int &discover,
                                       const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text, CardType cardType, int attack, int cost);

    //Gen
    static bool isAoeGen(const QString &code, const QString &text);
    static bool isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                            const QString &text, const CardType &cardType, int attack);
    static bool isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text);

private:
    static bool isDiscoverDrawGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    static bool isDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    static bool isDrawGen(const QString &code, const QString &text);
    static bool isToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    static int numDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    static int numDrawGen(const QString &code, const QString &text);
    static int numToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text);
    static bool isTaunt(const QString &code, const QJsonArray &mechanics);
    static bool isTauntGen(const QString &code, const QJsonArray &referencedTags);
    static bool isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                   const QString &text, const CardType &cardType, int attack);
    static bool isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text, const CardType &cardType, int attack);
    static bool isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags);
    static bool isHeroPowerGen(const QString &code, const QString &text);
    static bool isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType);
    static bool isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const CardType &cardType);
    static bool isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text);
    static bool isArmorGen(const QString &code, const QString &text);

    //Syn
    static bool isDiscoverSyn(const QString &code);
    static bool isDrawSyn(const QString &code);
    static bool isToYourHandSyn(const QString &code);
    static bool isPingSyn(const QString &code);
    static bool isDamageMinionsSyn(const QString &code);
    static bool isDestroySyn(const QString &code);
    static bool isReachSyn(const QString &code);
    static bool isAoeSyn(const QString &code);
    static bool isTauntSyn(const QString &code);
    static bool isTauntAllSyn(const QString &code);
    static bool isDeathrattleSyn(const QString &code);
    static bool isDeathrattleGoodAllSyn(const QString &code, const QString &text);
    static bool isRestoreFriendlyHeroSyn(const QString &code);
    static bool isArmorSyn(const QString &code);
};

#endif // MECHANICCOUNTER_H
