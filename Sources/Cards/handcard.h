#ifndef HANDCARD_H
#define HANDCARD_H

#include "deckcard.h"
#include "../utility.h"

class HandCard : public DeckCard
{
public:
    HandCard(QString code);
    ~HandCard();

//Variables
public:
    int turn;
    int buffAttack, buffHealth;
    QList<int> linkIdsList;


//Metodos
private:
    void drawDefaultHandCard();

public:
    void draw();
    void addBuff(int addAttack, int addHealth);
};
#endif // HANDCARD_H
