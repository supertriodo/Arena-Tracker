#ifndef RACECOUNTER_H
#define RACECOUNTER_H

#include "Sources/Cards/deckcard.h"
#include "Sources/Synergies/draftitemcounter.h"
#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QMap>
#include <QString>

class RaceCounter
{
public:
    RaceCounter();

//Variables
private:
    static DraftItemCounter **raceCounters;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
public:
    static DraftItemCounter **createRaceCounters(QObject *parent);
    static void deleteRaceCounters();
    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugRaceSynergies(const QString &code, const QJsonArray &mechanics, const QString &text);
    static void getRaceSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                 const QJsonArray &mechanics, const QString &text, const QList<CardRace> &cardRace);
    static void updateRaceCounters(DeckCard &deckCard);

private:
    //Gen
    static bool isMurlocGen(const QString &code);
    static bool isDemonGen(const QString &code);
    static bool isMechGen(const QString &code);
    static bool isElementalGen(const QString &code);
    static bool isBeastGen(const QString &code);
    static bool isTotemGen(const QString &code);
    static bool isPirateGen(const QString &code);
    static bool isDragonGen(const QString &code);
    static bool isNagaGen(const QString &code);
    static bool isUndeadGen(const QString &code);
    static bool isQuilboarGen(const QString &code);
    static bool isDraeneiGen(const QString &code);
    //New race step

    //Syn
    static bool isMurlocSyn(const QString &code);
    static bool isDemonSyn(const QString &code);
    static bool isMechSyn(const QString &code);
    static bool isElementalSyn(const QString &code);
    static bool isBeastSyn(const QString &code);
    static bool isTotemSyn(const QString &code);
    static bool isPirateSyn(const QString &code);
    static bool isDragonSyn(const QString &code);
    static bool isNagaSyn(const QString &code);
    static bool isUndeadSyn(const QString &code);
    static bool isQuilboarSyn(const QString &code);
    static bool isDraeneiSyn(const QString &code);
    //New race step
    static bool isMurlocAllSyn(const QString &code, const QString &text);
    static bool isDemonAllSyn(const QString &code, const QString &text);
    static bool isMechAllSyn(const QString &code, const QJsonArray &mechanics, const QString &text);
    static bool isElementalAllSyn(const QString &code, const QString &text);
    static bool isBeastAllSyn(const QString &code, const QString &text);
    static bool isTotemAllSyn(const QString &code, const QString &text);
    static bool isPirateAllSyn(const QString &code, const QString &text);
    static bool isDragonAllSyn(const QString &code, const QString &text);
    static bool isNagaAllSyn(const QString &code, const QString &text);
    static bool isUndeadAllSyn(const QString &code, const QString &text);
    static bool isQuilboarAllSyn(const QString &code, const QString &text);
    static bool isDraeneiAllSyn(const QString &code, const QString &text);
    //New race step
};

#endif // RACECOUNTER_H
