#ifndef KEYSYNERGIES_H
#define KEYSYNERGIES_H

#include "Sources/utility.h"
#include "qjsonarray.h"
#include <QString>
#include <QMap>

class KeySynergies
{
public:
    KeySynergies();
    KeySynergies(QString synergyTag);
    ~KeySynergies();

//Variables
private:
    QMap<QString, int> codeMap, codeSynMap;
    QString synergyTag;

    static QMap<QString, KeySynergies> keySynergiesMap;
    static QMap<QString, QList<QString>> *synergyCodes;

//Metodos
private:
    void reset();
    void increase(const QString &code);
    void increaseSyn(const QString &code);
    void insertCards(QMap<QString, QMap<QString, int> > &synergyTagMap);
    void insertSynCards(QMap<QString, QMap<QString, int> > &synergyTagMap);

    static QMap<QString, QString> getMapKeySynergies();
    static QString getSynergyTag(const QString &key);
    static bool isWeakKeySyn(const QString &key);

public:
    static QStringList getListKeySynergies();
    static QStringList getListValidSynergies();
    static void createKeySynergies();
    static void setSynergyCodes(QMap<QString, QList<QString> > *synergyCodes);
    static void resetAll();
    static bool containsAll(const QString &text, const QString &words);
    static QStringList debugKeySynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                   const QString &text, CardType cardType, int attack, int cost);
    static void updateKeySynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                   const QString &text, CardType cardType, int attack, int cost);
    static void getKeySynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap,
                                const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                const QString &text, CardType cardType, int attack, int cost);
    static void getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap);
    static bool isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                          const QJsonArray &mechanics, const QJsonArray &referencedTags,
                          const QString &text, CardType cardType, int attack, int cost);

    static bool isKey(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                      const QString &text, CardType cardType, int attack, int cost);
    static bool isKeyGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost);
    static bool isKeySyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost);
    static bool isKeyAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                            const QString &text, CardType cardType, int attack, int cost);
};

#endif // KEYSYNERGIES_H
