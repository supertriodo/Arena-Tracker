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
    bool showAttackBar;
    QList<int> linkIdsList;
    bool gettingLinkCards;
    int playerTotalAttack, playerTotalMaxAttack, enemyTotalAttack, enemyTotalMaxAttack;


//Metodos
private:
    void reset();
    void completeUI();
    void updateTransparency();
    void convertKnownCard(QString &code, int quantity);
    bool isIDinHand(int id);
    bool isLastCreatedByCodeValid(QString code);
    void showHeroAttack();
    void hideHeroAttack();
    void resetHeroAttack();
    int getCardBuff(QString code);
    void linkCards(HandCard &card);
    void revealLinkedCards(QString code, QList<int> &ids);
    bool isClonerCard(QString code);
    void linkNextCards();

public:
    HandCard *getHandCard(int id);
    void redrawDownloadedCardImage(QString &code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    bool isEmpty();
    void setShowAttackBar(bool value);
    void setTheme();
    void redrawTotalAttack();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void revealEnemyCard(int id, QString code);
    void enemyCardDraw(int id, QString code, QString createdByCode, int turn);
    void enemyCardBuff(int id, int buffAttack, int buffHealth);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="EnemyHandHandler");

public slots:
    void lastHandCardIsCoin();
    void showEnemyCardDraw(int id, int turn, bool special, QString code);
    void hideEnemyCardPlayed(int id, QString code);
    void lockEnemyInterface();
    void unlockEnemyInterface();
    void revealCreatedByCard(QString code, QString createdByCode, int quantity);
    void setLastCreatedByCode(QString code);
    void drawHeroTotalAttack(bool friendly, int totalAttack, int totalMaxAttack);
    void buffHandCard(int id);

private slots:
    void findHandCardEntered(QListWidgetItem *item);
};

#endif // ENEMYHANDHANDLER_H
