#ifndef SECRETCARD_H
#define SECRETCARD_H

#include "deckcard.h"
#include <QTreeWidgetItem>

enum SecretHero { mage, hunter, paladin, unknown };

class SecretCard : public DeckCard
{
public:
    SecretCard();
    SecretCard(QString code);
    ~SecretCard();

//Variables
public:
    QTreeWidgetItem *treeItem;
    SecretHero hero;

//Metodos
public:
    void draw() Q_DECL_OVERRIDE;
};

#endif // SECRETCARD_H
