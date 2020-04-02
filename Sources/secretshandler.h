#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/secretcard.h"
#include "enemyhandhandler.h"
#include "planhandler.h"
#include "utility.h"
#include <QQueue>
#include <QObject>


class MagneticPlayed
{
public:
    QString code;
    int id;
    int playerMinions;
};


class SecretTest
{
public:
    QString code;
    bool secretRevealedLastSecond;
};


class ActiveSecretId
{
public:
    int id;
    QList<QString> codes;
};


class ActiveSecret
{
public:
    CardClass hero;
    QList<ActiveSecretId> activeSecretIds;
    QList<SecretCard> children;
};


class SecretsHandler : public QObject
{
    Q_OBJECT
public:
    SecretsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler, PlanHandler *planHandler);
    ~SecretsHandler();

//Variables
private:
    Ui::Extended *ui;
    EnemyHandHandler *enemyHandHandler;
    PlanHandler *planHandler;
    QList<ActiveSecret> activeSecretList;
    QQueue<SecretTest> secretTestQueue;
    QQueue<MagneticPlayed> magneticPlayedQueue;
    bool secretsAnimating;
    bool showSecrets;
    QString lastMinionDead, lastMinionPlayed, lastSpellPlayed;
    QStringList arenaSets;
    //List of code secrets ordered by pickrate for all classes, used as options for a new unknown secret played.
    QList<QString> secretsByPickrate[NUM_HEROS];
    QMap<QString, float> *cardsPickratesMap;


//Metodos
private:
    void completeUI();
    void discardSecretOption(QString code, int delay=8000);
    void discardSecretOptionNow(QString code);
    void unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState);
    void knownSecretPlayed(int id, CardClass hero, QString code, LoadingScreenState loadingScreenState);
    bool isFromArenaSets(QString code);
    void unknownSecretPlayedAddOption(QString code, bool inArena, ActiveSecret &activeSecret, QString manaText="");
    void playerMinionPlayedNow(QString code, int playerMinions);
    ActiveSecret *getActiveSecret(CardClass hero, bool inArena);
    void updateShowSecrets();
    void createSecretsByPickrate();

public:
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    QStringList getSecretOptionCodes(int id);
    void setArenaSets(QStringList &arenaSets);
    void sortSecretsByPickrate(const QMap<QString, float> cardsPickratesMap[]);
    void setCardsPickratesMap(QMap<QString, float> cardsPickratesMap[]);
    void setShowSecrets(bool value);

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void revealCreatedByCard(QString code, QString createdByCode, int quantity);
    void isolatedSecret(int id, QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SecretsHandler");

public slots:
    void secretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState);
    void secretStolen(int id, QString code, LoadingScreenState loadingScreenState);
    void secretRevealed(int id, QString code);
    void resetSecretsInterface();
    void playerSpellPlayed(QString code);
    void playerSpellObjMinionPlayed();
    void playerSpellObjHeroPlayed();
    void playerBattlecryObjHeroPlayed();
    void playerMinionPlayed(QString code, int id, int playerMinions);
    void enemyMinionGraveyard(int id, QString code, bool isPlayerTurn);
    void avengeTested();
    void handOfSalvationTested();
    void _3CardsPlayedTested();
    void cSpiritTested();
    void playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions);
    void resetLastMinionDead(QString code, QString subType);
    void playerHeroPower();

private slots:
    void discardSecretOptionDelay();
    void clearSecretsAnimating();
    void adjustSize();
    void findSecretCardEntered(QListWidgetItem *item);
    void playerMinionPlayedDelay();
};

#endif // SECRETSHANDLER_H
