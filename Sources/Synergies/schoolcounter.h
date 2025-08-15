#ifndef SCHOOLCOUNTER_H
#define SCHOOLCOUNTER_H

#include "Sources/Cards/deckcard.h"
#include "Sources/Synergies/draftitemcounter.h"
#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class SchoolCounter
{
public:
    SchoolCounter();

//Variables
private:
    static DraftItemCounter **schoolCounters;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
public:
    static DraftItemCounter **createSchoolCounters(QObject *parent);
    static void deleteSchoolCounters();
    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugSchoolSynergies(const QString &code, const QString &text);
    static void getSchoolSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                   const QString &text, const CardSchool &cardSchool);
    static void updateSchoolCounters(DeckCard &deckCard);

private:
    //Gen
    static bool isArcaneGen(const QString &code);
    static bool isFelGen(const QString &code);
    static bool isFireGen(const QString &code);
    static bool isFrostGen(const QString &code);
    static bool isHolyGen(const QString &code);
    static bool isShadowGen(const QString &code);
    static bool isNatureGen(const QString &code);

    //Syn
    static bool isArcaneSyn(const QString &code);
    static bool isFelSyn(const QString &code);
    static bool isFireSyn(const QString &code);
    static bool isFrostSyn(const QString &code);
    static bool isHolySyn(const QString &code);
    static bool isShadowSyn(const QString &code);
    static bool isNatureSyn(const QString &code);
    static bool isArcaneAllSyn(const QString &code, const QString &text);
    static bool isFelAllSyn(const QString &code, const QString &text);
    static bool isFireAllSyn(const QString &code, const QString &text);
    static bool isFrostAllSyn(const QString &code, const QString &text);
    static bool isHolyAllSyn(const QString &code, const QString &text);
    static bool isShadowAllSyn(const QString &code, const QString &text);
    static bool isNatureAllSyn(const QString &code, const QString &text);
};

#endif // SCHOOLCOUNTER_H
