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
    static QRegularExpressionMatch match;

//Metodos
private:
    static QList<StatSyn> getStatsSynergiesFromJson(const QString &code);
    static StatSyn getStatSyn(const QString &mechanic);
    static void getStatsSynergiesFromStatSyn(const StatSyn &statSyn, QMap<QString, QMap<QString, int> > &synergyTagMap);
    void reset();
    void qDebugContents();
    void increaseSynM(const StatSyn &statSyn, const QString &code);
    void increase(int statValue, const QString &code);
    void increaseSyn(int statValue, const QString &code);
    void increaseX(bool appendToSyn, int statValue, const QString &code);
    void insertCardsM(const StatSyn &statSyn, QMap<QString, int> &synergies);
    void insertCards(int statValue, QMap<QString,int> &synergies);
    void insertSynCards(int statValue, QMap<QString,int> &synergies);
    void insertCardsX(bool insertSyn, int statValue, QMap<QString, int> &synergies);

public:
    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static void updateStatsSynergies(const QString &code, CardType cardType, int attack, int health, int cost);
    static void getStatsSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                  CardType cardType, int attack, int health, int cost);
    static void getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, QString &partSynergyTag, CardType cardType, int attack, int health, int cost);
};

#endif // STATSYNERGIES_H
