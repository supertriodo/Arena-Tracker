#ifndef ENEMYDECKHANDLER_H
#define ENEMYDECKHANDLER_H

#include "Widgets/ui_extended.h"
#include <QObject>
#include <QMap>

class EnemyDeckHandler : public QObject
{
    Q_OBJECT

public:
    EnemyDeckHandler(QObject *parent, Ui::Extended *ui);
    ~EnemyDeckHandler();

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
    CardClass getEnemyClass();

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
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="EnemyDeckHandler");

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

#endif // ENEMYDECKHANDLER_H
