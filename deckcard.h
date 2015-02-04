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

#endif // DECKCARD_H
