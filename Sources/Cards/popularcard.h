#ifndef POPULARCARD_H
#define POPULARCARD_H

#include "deckcard.h"


class PopularCard : public DeckCard
{
public:
    PopularCard(QString code, QString manaText, QStringList mechanics);
    ~PopularCard();

//Variables
private:
    QString manaText;
    QStringList mechanics;

//Metodos
public:
    void draw();
};

#endif // POPULARCARD_H
