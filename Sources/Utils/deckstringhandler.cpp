#include "deckstringhandler.h"
#include "../utility.h"
#include <QtWidgets>



QList<CodeAndCount> DeckStringHandler::readDeckString(const QByteArray &encodedDeckString)
{
    QList<quint64> cardsx1;
    QList<quint64> cardsx2;
    QMap<quint64, quint64> cardsxN;
    QByteArray data = QByteArray::fromBase64(encodedDeckString);

    if(takeChar(data) != 0x0)                   return QList<CodeAndCount>();
    if(readVarint(data) != DECKSTRING_VERSION)  return QList<CodeAndCount>();

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


QByteArray DeckStringHandler::writeDeckString(const QList<CodeAndCount> &deckList)
{
    QList<quint64> cardsx1;
    QList<quint64> cardsx2;
    QMap<quint64, quint64> cardsxN;
    if(!debuildDeck(cardsx1, cardsx2, cardsxN, deckList))   return "";

    QByteArray data;
    data.append((char)0x0);
    writeVarint(DECKSTRING_VERSION, data);

    writeVarint(getFormat(deckList), data);

    quint64 hero = getHeroe(deckList);
    if(hero == 0)   return "";
    writeVarint(1, data);
    writeVarint(hero, data);

    qSort(cardsx1);
    writeVarint(cardsx1.count(), data);
    for(const quint64 &id: cardsx1)
    {
        writeVarint(id, data);
    }

    qSort(cardsx2);
    writeVarint(cardsx2.count(), data);
    for(const quint64 &id: cardsx2)
    {
        writeVarint(id, data);
    }

    QList<quint64> cardsxNList = cardsxN.keys();
    qSort(cardsxNList);
    writeVarint(cardsxNList.count(), data);
    for(const quint64 &id: cardsxNList)
    {
        quint64 count = cardsxN[id];
        writeVarint(id, data);
        writeVarint(count, data);
    }

    return data.toBase64();
}


FormatType DeckStringHandler::getFormat(const QList<CodeAndCount> &deckList)
{
    for(const CodeAndCount& codeAndCount: deckList)
    {
        QString code = codeAndCount.code;
        if(!Utility::isFromStandardSet(code))   return FT_WILD;
    }
    return FT_STANDARD;
}

quint64 DeckStringHandler::getHeroe(const QList<CodeAndCount> &deckList)
{
    for(const CodeAndCount& codeAndCount: deckList)
    {
        QString code = codeAndCount.code;
        DeckCard deckCard(code);
        QString hero = Utility::heroToLogNumber(deckCard.getCardClass());
        if(!hero.isEmpty())     return getId("HERO_" + hero);
    }
    return 0;
}


QList<CodeAndCount> DeckStringHandler::buildDeck(const QList<quint64> &cardsx1, const QList<quint64> &cardsx2, const QMap<quint64, quint64> &cardsxN)
{
    QList<CodeAndCount> deckList;

    for(const quint64 &id: cardsx1)
    {
        QString code = getCode(id);
        if(code.isEmpty())  return QList<CodeAndCount>();
        else    deckList.append(CodeAndCount(code, 1));
    }

    for(const quint64 &id: cardsx2)
    {
        QString code = getCode(id);
        if(code.isEmpty())  return QList<CodeAndCount>();
        else    deckList.append(CodeAndCount(code, 2));
    }

    for(const quint64 &id: cardsxN.keys())
    {
        QString code = getCode(id);
        int count = cardsxN[id];
        if(code.isEmpty())  return QList<CodeAndCount>();
        else    deckList.append(CodeAndCount(code, count));
    }
    return deckList;
}


bool DeckStringHandler::debuildDeck(QList<quint64> &cardsx1, QList<quint64> &cardsx2, QMap<quint64, quint64> &cardsxN,
                                    const QList<CodeAndCount> &deckList)
{
    for(const CodeAndCount& codeAndCount: deckList)
    {
        quint64 id = getId(codeAndCount.code);
        if(id == 0)     return false;
        int count = codeAndCount.count;
        switch(count)
        {
            case 1:
                cardsx1.append(id);
                break;
            case 2:
                cardsx2.append(id);
                break;
            default:
                cardsxN[id] = count;
                break;
        }
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


quint64 DeckStringHandler::getId(const QString &code)
{
    return Utility::getCardAtribute(code, "dbfId").toVariant().toULongLong();
}


void DeckStringHandler::writeVarint(quint64 value, QByteArray &stream)
{
    while (value > 127) {
        stream.append(((uint8_t)(value & 127)) | 128);
        value >>= 7;
    }
    stream.append(((uint8_t)value) & 127);
}


quint64 DeckStringHandler::readVarint(QByteArray &stream)
{
    quint64 value = 0;
    int i = 0;
    while(!stream.isEmpty())
    {
        char byte = takeChar(stream);
        value |= (byte & 127) << (7 * i);
        if(!(byte & 128))   return value;
        i++;
    }
    return 0;
}





