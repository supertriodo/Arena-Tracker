#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/secretcard.h"
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
    SecretsHandler(QObject *parent, Ui::Extended *ui);
    ~SecretsHandler();

//Variables
private:
    Ui::Extended *ui;
    QList<ActiveSecret> activeSecretList;
    QQueue<SecretTest> secretTests;
    bool synchronized;
    bool secretsAnimating;
    QString lastMinionDead;


//Metodos
private:
    void completeUI();
    void discardSecretOption(QString code, int delay=3000);
    void discardSecretOptionNow(QString code);
    void checkLastSecretOption(ActiveSecret &activeSecret);

public:
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void setSynchronized();

signals:
    void checkCardImage(QString code);
    void duplicated(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SecretsHandler");

public slots:
    void secretPlayed(int id, SecretHero hero);
    void secretStealed(int id, QString code);
    void secretRevealed(int id, QString code);
    void resetSecretsInterface();
    void playerSpellPlayed();
    void playerSpellObjPlayed();
    void playerMinionPlayed(int playerMinions);
    void enemyMinionDead(QString code);
    void avengeTested();
    void cSpiritTested();
    void playerAttack(bool isHeroFrom, bool isHeroTo);
    void resetLastMinionDead(QString code, QString subType);
    void playerHeroPower();

private slots:
    void discardSecretOptionDelay();
    void clearSecretsAnimating();
    void adjustSize();
    void findSecretCardEntered(QTreeWidgetItem *item);
};

#endif // SECRETSHANDLER_H
