#ifndef STATSYNERGIES_H
#define STATSYNERGIES_H

#include "Sources/utility.h"
#include <QMap>
#include <QObject>


enum SynergyStat {S_COST, S_ATTACK, S_HEALTH};
enum SynergyOp {S_EQUAL, S_HIGHER_EQUAL, S_LOWER_IGUAL};
enum SynergyType {S_MINION, S_SPELL, S_WEAPON};


class StatSyn
{
public:
    SynergyStat statKind;
    SynergyOp op;
    SynergyType cardType;
    int statValue;
    bool isGen;
};


class StatSynergies
{
public:
    StatSynergies();

//Variables
private:
    QMap<int, QMap<QString, int>> statsMap, statsSynMap;

    static StatSynergies costMinions, attackMinions, healthMinions, costSpells;
    static StatSynergies costWeapons, attackWeapons, healthWeapons;
    static QMap<QString, QList<QString>> * synergyCodes;

//Metodos
public:
    static QList<StatSyn> getStatsSynergiesFromJson(const QString &code);
    void reset();
    void qDebugContents();
    void updateStatsMapSyn(const StatSyn &statSyn, const QString &code);
    void appendStatValue(bool appendToSyn, int statValue, const QString &code);
    void insertStatCards(const StatSyn &statSyn, QMap<QString, int> &synergies);
    void insertCards(bool insertSyn, int statValue, QMap<QString, int> &synergies);

    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static void updateStatsCards(const QString &code, CardType cardType, int attack, int health, int cost);
    static void getStatsCardsSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                       CardType cardType, int attack, int health, int cost);
};

#endif // STATSYNERGIES_H
