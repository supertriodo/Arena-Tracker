#ifndef DECKSTRINGHANDLER_H
#define DECKSTRINGHANDLER_H

#include <QByteArray>
#include <QList>
#include <QMap>

#define DECKSTRING_VERSION 1

enum FormatType { FT_UNKNOWN = 0, FT_WILD = 1, FT_STANDARD = 2 };

class CodeAndCount
{
public:
    CodeAndCount(QString code, int count) : code(code), count(count){}
    QString code;
    int count;
};

class DeckStringHandler
{
//Metodos
private:
    static void writeVarint(quint64 value, QByteArray &stream);
    static quint64 readVarint(QByteArray &stream);
    static char takeChar(QByteArray &stream);
    static QString getCode(const quint64 &dbfId);
    static quint64 getId(const QString &code);
    static QList<CodeAndCount> buildDeck(const QList<quint64> &cardsx1, const QList<quint64> &cardsx2, const QMap<quint64, quint64> &cardsxN);
    static bool debuildDeck(QList<quint64> &cardsx1, QList<quint64> &cardsx2, QMap<quint64, quint64> &cardsxN, const QList<CodeAndCount> &deckList);
    static FormatType getFormat(const QList<CodeAndCount> &deckList);
    static quint64 getHeroId(const QList<CodeAndCount> &deckList);
    static QString getHeroLog(const QList<CodeAndCount> &deckList);
    static QString getHeroString(const QList<CodeAndCount> &deckList);
    static QList<CodeAndCount> readDeckString(const QByteArray &encodedDeckString);
    static QByteArray writeDeckStringBA(const QList<CodeAndCount> &deckList);
    static QByteArray readDeckStringPage(QString &text, QString &deckName);
    static QString writeDeckStringPage(const QList<CodeAndCount> &deckList, const QString &deckName, const QByteArray &encodedDeckString);

public:
    static QList<CodeAndCount> readDeckString(QString &text, QString &deckName);
    static QString writeDeckString(const QList<CodeAndCount> &deckList, const QString &deckName);
};

#endif // DECKSTRINGHANDLER_H
