#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/secretcard.h"
#include "enemyhandhandler.h"
#include "utility.h"
#include <QQueue>
#include <QObject>



class SecretTest
{
public:
    QString code;
    bool secretRevealedLastSecond;
};


class ActiveSecret
{
public:
    int id;
    SecretCard root;
    QList<SecretCard> children;
};


class SecretsHandler : public QObject
{
    Q_OBJECT
public:
    SecretsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler);
    ~SecretsHandler();

//Variables
private:
    Ui::Extended *ui;
    EnemyHandHandler *enemyHandHandler;
    QList<ActiveSecret> activeSecretList;
    QQueue<SecretTest> secretTests;
    bool secretsAnimating;
    QString lastMinionDead, lastMinionPlayed, lastSpellPlayed;


//Metodos
private:
    void completeUI();
    void discardSecretOption(QString code, int delay=8000);
    void discardSecretOptionNow(QString code);
    void checkLastSecretOption(ActiveSecret &activeSecret);
    void unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState, bool discover=false);
    void knownSecretPlayed(int id, CardClass hero, QString code);

public:
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    QStringList getSecretOptionCodes(int id);

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void revealCreatedByCard(QString code, QString createdByCode, int quantity);
    void isolatedSecret(int id, QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SecretsHandler");

public slots:
    void secretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState);
    void secretStolen(int id, QString code);
    void secretRevealed(int id, QString code);
    void resetSecretsInterface();
    void playerSpellPlayed(QString code);
    void playerSpellObjMinionPlayed();
    void playerSpellObjHeroPlayed();
    void playerBattlecryObjHeroPlayed();
    void playerMinionPlayed(QString code, int playerMinions);
    void enemyMinionGraveyard(int id, QString code, bool isPlayerTurn);
    void avengeTested();
    void handOfSalvationTested();
    void cSpiritTested();
    void playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions);
    void resetLastMinionDead(QString code, QString subType);
    void playerHeroPower();

private slots:
    void discardSecretOptionDelay();
    void clearSecretsAnimating();
    void adjustSize();
    void findSecretCardEntered(QTreeWidgetItem *item);
};

#endif // SECRETSHANDLER_H
