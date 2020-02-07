#ifndef DRAWCARDHANDLER_H
#define DRAWCARDHANDLER_H


#include "Widgets/ui_extended.h"
#include "Cards/drawcard.h"
#include <QObject>

class DrawCardHandler : public QObject
{
    Q_OBJECT
public:
    DrawCardHandler(QObject *parent, Ui::Extended *ui);
    ~DrawCardHandler();

//Variables
private:
    Ui::Extended *ui;
    bool drawAnimating;
    QList<DrawCard> drawCardList;
    int drawDisappear;


//Metodos
private:
    void completeUI();
    void newDrawCard(QString code, bool mulligan);

public:
    void setDrawDisappear(int value);
    void redrawAllCards();
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DrawCardHandler");

public slots:
    void playerCardToHand(int id, QString code, int turn);
    void clearDrawList();

private slots:
    void adjustDrawSize();
    void clearDrawAnimating();
    void removeOldestDrawCard();
    void findDrawCardEntered(QListWidgetItem *item);

};

#endif // DRAWCARDHANDLER_H


