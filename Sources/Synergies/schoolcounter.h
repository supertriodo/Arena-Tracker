#ifndef SCHOOLCOUNTER_H
#define SCHOOLCOUNTER_H

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
    static QMap<QString, DraftItemCounter*> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    static QMap<QString, QString> getMapKeySynergies();
    static QString getSynergyTag(const QString &key);

    static bool isKey(const QString &key, const CardSchool &cardSchool);
    static bool isKeyGen(const QString &key, const QString &code);
    static bool isKeySyn(const QString &key, const QString &code);
    static bool isKeyAllSyn(QString key, const QString &code, const QString &text);

public:
    static void createSchoolCounters(QObject *parent);
    static QStringList getListKeySynergies();
    static void resetAll();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static QStringList debugSchoolSynergies(const QString &code, const QString &text);
    static void updateSchoolCounters(const QString &code, const QString &text, CardSchool cardSchool);
    static void getSchoolSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                   const QString &text, const CardSchool &cardSchool);
    static void getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag, const QString &text, CardSchool cardSchool);
};

#endif // SCHOOLCOUNTER_H
