#ifndef GRAVEYARDHANDLER_H
#define GRAVEYARDHANDLER_H


#include "Widgets/ui_extended.h"
#include <QObject>
#include <QMap>

class GraveyardHandler : public QObject
{
    Q_OBJECT

public:
    GraveyardHandler(QObject *parent, Ui::Extended *ui);
    ~GraveyardHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    Ui::Extended *ui;
    Transparency transparency;
    bool inGame, mouseInApp;
    CardClass enemyClass;
    int lastSecretIdAdded;
    int firstOutsiderId;

//Metodos
public:
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void redrawDownloadedCardImage(QString code);
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    QList<DeckCard> getDeckCardList();

private:
    void completeUI();
    void insertDeckCard(DeckCard &deckCard);
    void newDeckCard(QString card, int id, int total=1, bool add=true);
    void updateTransparency();
    void reset();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="GraveyardHandler");

public slots:
    void enemyCardPlayed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void enemyKnownCardDraw(int id, QString code);
    void setEnemyClass(QString hero);
    void lockEnemyDeckInterface();
    void unlockEnemyDeckInterface();
    void setFirstOutsiderId(int id);

private slots:
    void findDeckCardEntered(QListWidgetItem *item);
};

#endif // GRAVEYARDHANDLER_H
