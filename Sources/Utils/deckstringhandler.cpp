#include "deckstringhandler.h"
#include <QtWidgets>

DeckStringHandler::DeckStringHandler()
{

}


DeckStringHandler::~DeckStringHandler()
{

}


/**
 * Encodes an unsigned variable-length integer using the MSB algorithm.
 * This function assumes that the value is stored as little endian.
 * @param value The input value. Any standard integer type is allowed.
 * @param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
 * @return The number of bytes used in the output memory.
 */
//template<typename int_t = uint64_t>
void DeckStringHandler::encodeVarint(quint64 value, QByteArray &stream)
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
quint64 DeckStringHandler::decodeVarint(QByteArray &stream)
{
    quint64 value = 0;
    int i = 0;
//    int_t ret = 0;
    while(!stream.isEmpty())
//    for(size_t i = 0; i < inputSize; i++)
    {
        char byte = stream.at(0);
        stream.remove(0,1);
//        ret |= (input[i] & 127) << (7 * i);
        value |= (byte & 127) << (7 * i);
        //If the next-byte flag is set
//        if(!(input[i] & 128))   break;
        if(!(byte & 128))   return value;
        i++;
    }
    return 0;
}


void DeckStringHandler::parseDeckString(QByteArray &encodedDeckString)
{
//        QByteArray deckString = QByteArray::fromBase64(encodedDeckString);
//        QDataStream stream(deckString);

//        char byte;
//        stream >> &byte;
//        if(byte != 0x0)
//        {
//            qDebug()<<"no \0";
//            return;
//        }

//        quint64 format = readVarint(stream);

//        quint64 numHeroes = readVarint(stream);
//        for(uint i=0; i<numHeroes; i++)
//        {
//            qDebug()<<"Heroes:"<<readVarint(stream);
//        }

//        quint64 numCards1 = readVarint(stream);
//        for(uint i=0; i<numCards1; i++)
//        {
//            qDebug()<<"Cards x 1:"<<readVarint(stream);
//        }

//        quint64 numCards2 = readVarint(stream);
//        for(uint i=0; i<numCards2; i++)
//        {
//            qDebug()<<"Cards x 2:"<<readVarint(stream);
//        }

//        quint64 numCardsN = readVarint(stream);
//        for(uint i=0; i<numCardsN; i++)
//        {
//            qDebug()<<"Cards x N:"<<readVarint(stream)<<readVarint(stream);
//        }
}



















