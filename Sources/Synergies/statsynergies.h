#ifndef STATSYNERGIES_H
#define STATSYNERGIES_H

#include <QObject>
#include "../Cards/deckcard.h"


enum VisibleStat {V_COST, V_ATTACK, V_HEALTH};
enum VisibleOp {V_EQUAL,V_HIGHER_EQUAL, V_LOWER_IGUAL};


class StatSyn
{
public:
    VisibleStat statKind;
    VisibleOp op;
    int statValue;
    bool isGen;
};


class StatSynergies
{
public:
    StatSynergies();

//Variables
private:
    QMap<int, QList<DeckCard>> statsMap, statsMapSyn;

//Metodos
public:
    static QList<StatSyn> getStatsSynergiesFromJson(const QString &code, QMap<QString, QList<QString> > &synergyCodes);
    void clear();
    void qDebugContents();
    void updateStatsMapSyn(const StatSyn &statSyn, QString &code);
    void appendStatValue(bool appendToSyn, int statValue, QString &code);
    void insertStatCards(const StatSyn &statSyn, QMap<QString, int> &synergies);
    void insertCards(bool insertSyn, int statValue, QMap<QString, int> &synergies);
};

#endif // STATSYNERGIES_H
