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
//Constructor
public:
    DeckStringHandler();
    DeckStringHandler(QByteArray encodedDeckString);
    ~DeckStringHandler();

//Variables
private:
    QList<CodeAndCount> deck;
    bool valid;

//Metodos
private:
    void writeVarint(quint64 value, QByteArray &stream);
    quint64 readVarint(QByteArray &stream);
    char takeChar(QByteArray &stream);
    QString getCode(const quint64 &dbfId);
    bool buildDeck(QList<quint64> &cardsx1, QList<quint64> &cardsx2, QMap<quint64, quint64> &cardsxN);
    bool parseDeckString(QByteArray &encodedDeckString);

public:
    QList<CodeAndCount> getDeck();
};

#endif // DECKSTRINGHANDLER_H
