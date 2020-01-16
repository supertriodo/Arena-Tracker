#ifndef SECRETCARD_H
#define SECRETCARD_H

#include "deckcard.h"
#include <QTreeWidgetItem>

class SecretCard : public DeckCard
{
public:
    SecretCard();
    SecretCard(QString code);
    SecretCard(QString code, QString manaText);
    ~SecretCard();

//Variables
public:
    QTreeWidgetItem *treeItem;
    CardClass hero;
    QString manaText;

//Metodos
public:
    void draw();
};

#endif // SECRETCARD_H
