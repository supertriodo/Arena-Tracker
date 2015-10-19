#ifndef HANDCARD_H
#define HANDCARD_H

#include "deckcard.h"
#include "utility.h"

class HandCard : public DeckCard
{
public:
    HandCard(QString code);
    ~HandCard();

//Variables
public:
    int id;
    int turn;
    bool special;

protected:
    QString createdByCode;

//Metodos
private:
    void drawDefaultHandCard();
    void drawCreatedByHandCard();

public:
    void draw() Q_DECL_OVERRIDE;
    void setCreatedByCode(QString code);
    QString getCreatedByCode();
};
#endif // HANDCARD_H
