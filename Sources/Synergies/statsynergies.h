#ifndef STATSYNERGIES_H
#define STATSYNERGIES_H

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

//Metodos
public:
    static QList<StatSyn> getStatsSynergiesFromJson(const QString &code, QMap<QString, QList<QString> > &synergyCodes);
    void clear();
    void qDebugContents();
    void updateStatsMapSyn(const StatSyn &statSyn, const QString &code);
    void appendStatValue(bool appendToSyn, int statValue, const QString &code);
    void insertStatCards(const StatSyn &statSyn, QMap<QString, int> &synergies);
    void insertCards(bool insertSyn, int statValue, QMap<QString, int> &synergies);
};

#endif // STATSYNERGIES_H
