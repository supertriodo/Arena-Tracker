#ifndef RACECOUNTER_H
#define RACECOUNTER_H

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
    static QMap<QString, DraftItemCounter*> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    static QMap<QString, QString> getMapKeySynergies();
    static QString getSynergyTag(const QString &key);

    static bool isKey(const QString &key, const QList<CardRace> &cardRace);
    static bool isKeyGen(const QString &key, const QString &code);
    static bool isKeySyn(const QString &key, const QString &code);
    static bool isKeyAllSyn(QString key, const QString &code, const QJsonArray &mechanics, const QString &text);

public:
    static void createRaceCounters(QObject *parent);
    static QStringList getListKeySynergies();
    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugRaceSynergies(const QString &code, const QJsonArray &mechanics, const QString &text);
    static void updateRaceCounters(const QString &code, const QJsonArray &mechanics,
                                   const QString &text, const QList<CardRace> &cardRace);
    static void getRaceSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                 const QJsonArray &mechanics, const QString &text, const QList<CardRace> &cardRace);
    static void getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                          const QJsonArray &mechanics, const QString &text, const QList<CardRace> &cardRace);
};

#endif // RACECOUNTER_H
