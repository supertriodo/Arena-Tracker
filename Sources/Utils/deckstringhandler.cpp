#include "deckstringhandler.h"
#include "../utility.h"
#include <QtWidgets>

DeckStringHandler::DeckStringHandler()
{

}


DeckStringHandler::DeckStringHandler(QByteArray encodedDeckString)
{
    this->valid = parseDeckString(encodedDeckString);
}


DeckStringHandler::~DeckStringHandler()
{

}


QList<CodeAndCount> DeckStringHandler::getDeck()
{
    return this->deck;
}


bool DeckStringHandler::parseDeckString(QByteArray &encodedDeckString)
{
    QList<quint64> cardsx1;
    QList<quint64> cardsx2;
    QMap<quint64, quint64> cardsxN;
    QByteArray data = QByteArray::fromBase64(encodedDeckString);

    if(takeChar(data) != 0x0)                   return false;
    if(readVarint(data) != DECKSTRING_VERSION)  return false;

    readVarint(data);//Format

    quint64 numHeroes = readVarint(data);
    for(uint i=0; i<numHeroes; i++)
    {
        readVarint(data);//Heroes
    }

    quint64 numCards1 = readVarint(data);
    for(uint i=0; i<numCards1; i++)
    {
        cardsx1.append(readVarint(data));
    }

    quint64 numCards2 = readVarint(data);
    for(uint i=0; i<numCards2; i++)
    {
        cardsx2.append(readVarint(data));
    }

    quint64 numCardsN = readVarint(data);
    for(uint i=0; i<numCardsN; i++)
    {
        quint64 id =  readVarint(data);
        quint64 count = readVarint(data);
        cardsxN[id] = count;
    }
    return buildDeck(cardsx1, cardsx2, cardsxN);
}


bool DeckStringHandler::buildDeck(QList<quint64> &cardsx1, QList<quint64> &cardsx2, QMap<quint64, quint64> &cardsxN)
{
    qSort(cardsx1);
    for(const quint64 &id: cardsx1)
    {
        QString code = getCode(id);
        if(code.isEmpty())  return false;
        else    deck.append(CodeAndCount(code, 1));
        qDebug()<<Utility::getCardAtribute(code, "name").toString()<<1;
    }

    qSort(cardsx2);
    for(const quint64 &id: cardsx2)
    {
        QString code = getCode(id);
        if(code.isEmpty())  return false;
        else    deck.append(CodeAndCount(code, 2));
        qDebug()<<Utility::getCardAtribute(code, "name").toString()<<2;
    }

    QList<quint64> cardsxNList = cardsxN.keys();
    qSort(cardsxNList);
    for(const quint64 &id: cardsxNList)
    {
        QString code = getCode(id);
        int count = cardsxN[id];
        if(code.isEmpty())  return false;
        else    deck.append(CodeAndCount(code, count));
        qDebug()<<Utility::getCardAtribute(code, "name").toString()<<count;
    }
    return true;
}


char DeckStringHandler::takeChar(QByteArray &stream)
{
    char byte = stream.at(0);
    stream.remove(0,1);
    return byte;
}


QString DeckStringHandler::getCode(const quint64 &dbfId)
{
    return Utility::getCodeFromCardAttribute("dbfId", QJsonValue((qint64)dbfId));
}


/**
 * Encodes an unsigned variable-length integer using the MSB algorithm.
 * This function assumes that the value is stored as little endian.
 * @param value The input value. Any standard integer type is allowed.
 * @param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
 * @return The number of bytes used in the output memory.
 */
//template<typename int_t = uint64_t>
void DeckStringHandler::writeVarint(quint64 value, QByteArray &stream)
{
//    int outputSize = 0;
    //While more than 7 bits of data are left, occupy the last output byte
    // and set the next byte flag
    while (value > 127) {
        //|128: Set the next byte flag
//        output[outputSize] = ((uint8_t)(value & 127)) | 128;
        stream.append(((uint8_t)(value & 127)) | 128);
        //Remove the seven bits we just wrote
        value >>= 7;
//        outputSize++;
    }
//    output[outputSize++] = ((uint8_t)value) & 127;
    stream.append(((uint8_t)value) & 127);
}
/**
 * Decodes an unsigned variable-length integer using the MSB algorithm.
 * @param value A variable-length encoded integer of arbitrary size.
 * @param inputSize How many bytes are
 */
//template<typename int_t = uint64_t>
quint64 DeckStringHandler::readVarint(QByteArray &stream)
{
    quint64 value = 0;
    int i = 0;
//    int_t ret = 0;
    while(!stream.isEmpty())
//    for(size_t i = 0; i < inputSize; i++)
    {
        char byte = takeChar(stream);
//        ret |= (input[i] & 127) << (7 * i);
        value |= (byte & 127) << (7 * i);
        //If the next-byte flag is set
//        if(!(input[i] & 128))   break;
        if(!(byte & 128))   return value;
        i++;
    }
    return 0;
}





