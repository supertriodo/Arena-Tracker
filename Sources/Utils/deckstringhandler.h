#ifndef DECKSTRINGHANDLER_H
#define DECKSTRINGHANDLER_H

#include <QByteArray>

class DeckStringHandler
{
//Constructor
public:
    DeckStringHandler();
    ~DeckStringHandler();

//Variables

//Metodos
private:
    static void encodeVarint(quint64 value, QByteArray &stream);
    static quint64 decodeVarint(QByteArray &stream);

public:
    void parseDeckString(QByteArray &encodedDeckString);
};

#endif // DECKSTRINGHANDLER_H
