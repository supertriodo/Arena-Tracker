#ifndef SECRETSWINDOW_H
#define SECRETSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "movelistwidget.h"
#include "../secretshandler.h"


class CardListWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    CardListWindow(QWidget *parent, SecretsHandler *secretsHandler);
    ~CardListWindow();

//Variables
private:
    MoveListWidget * listWidget;
    SecretsHandler * secretsHandler;

//Metodos
public:


signals:

public slots:
    void loadSecret(int id, QRect &rectCard, int maxTop=-1, int maxBottom=-1, bool alignReverse=false);
    void loadDraftItem(QList<DeckCard> &deckCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1, bool alignReverse=false);
};


#endif // SECRETSWINDOW_H
