#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>

class DeckCard
{
public:
    DeckCard(){listItem = NULL; total=remaining=1;}
    QListWidgetItem *listItem;
    QString code;
    int cost;
    uint total;
    uint remaining;
};


class HandCard
{
public:
    HandCard(){listItem = NULL; id=turn=0; special=false; code="";}
    QListWidgetItem *listItem;
    int id;
    int turn;
    bool special;
    QString code;
};

#endif // DECKCARD_H
