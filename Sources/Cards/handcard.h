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
    QList<int> linkIdsList;

private:
    int buffAttack, buffHealth;
    bool forged;

//Metodos
private:
    void drawDefaultHandCard();

public:
    void draw();
    void addBuff(int addAttack, int addHealth);
    void forge();
};
#endif // HANDCARD_H
