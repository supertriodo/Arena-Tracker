#ifndef ENEMYHANDHANDLER_H
#define ENEMYHANDHANDLER_H

#include "Cards/handcard.h"
#include "utility.h"
#include "Widgets/ui_extended.h"
#include <QObject>
#include <QMap>

#define COIN QString("GAME_005")

class EnemyHandHandler : public QObject
{
    Q_OBJECT
public:
    EnemyHandHandler(QObject *parent, Ui::Extended *ui);
    ~EnemyHandHandler();

//Variables
private:
    QList<HandCard> enemyHandList;
    Ui::Extended *ui;
    bool inGame;
    bool mouseInApp;
    Transparency transparency;
    QString knownCard;
    int numKnownCards;
    QString lastCreatedByCode;


//Metodos
private:
    void reset();
    void completeUI();
    void updateTransparency();
    void convertKnownCard(QString &code, int quantity);
    bool isIDinHand(int id);

public:
    void redrawDownloadedCardImage(QString &code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    bool isFromEnemyDeck(int id);
    bool isEmpty();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="EnemyHandHandler");

public slots:
    void lastHandCardIsCoin();
    void showEnemyCardDraw(int id, int turn, bool special, QString code);
    void hideEnemyCardPlayed(int id, QString code);
    void lockEnemyInterface();
    void unlockEnemyInterface();
    void convertDuplicates(QString code);
    void setLastCreatedByCode(QString code);

private slots:
    void findHandCardEntered(QListWidgetItem *item);
};

#endif // ENEMYHANDHANDLER_H
