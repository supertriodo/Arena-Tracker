#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/secretcard.h"
#include "utility.h"
#include <QQueue>
#include <QObject>


//Secretos
#define AVENGE QString("FP1_020")
#define NOBLE_SACRIFICE QString("EX1_130")
#define REPENTANCE QString("EX1_379")
#define REDEMPTION QString("EX1_136")
#define EYE_FOR_AN_EYE QString("EX1_132")
#define COMPETITIVE_SPIRIT QString("AT_073")

#define FREEZING_TRAP QString("EX1_611")
#define EXPLOSIVE_TRAP QString("EX1_610")
#define BEAR_TRAP QString("AT_060")
#define SNIPE QString("EX1_609")
#define MISDIRECTION QString("EX1_533")
#define SNAKE_TRAP QString("EX1_554")

#define MIRROR_ENTITY QString("EX1_294")
#define DDUPLICATE QString("FP1_018")
#define ICE_BARRIER QString("EX1_289")
#define EFFIGY QString("AT_002")
#define VAPORIZE QString("EX1_594")
#define COUNTERSPELL QString("EX1_287")
#define SPELLBENDER QString("tt_010")
#define ICE_BLOCK QString("EX1_295")

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
    void checkLastSecretOption(ActiveSecret activeSecret);

public:
    void redrawDownloadedCardImage(QString code);
    void setSynchronized();

signals:
    void checkCardImage(QString code);
    void duplicated(QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="SecretsHandler");

public slots:
    void secretPlayed(int id, SecretHero hero);
    void secretStealed(int id, QString code);
    void secretRevealed(int id, QString code);
    void resetSecretsInterface();
    void playerSpellPlayed();
    void playerSpellObjPlayed();
    void playerMinionPlayed();
    void enemyMinionDead(QString code);
    void avengeTested();
    void cSpiritTested();
    void playerAttack(bool isHeroFrom, bool isHeroTo);
    void resetLastMinionDead(QString code, QString subType);

private slots:
    void discardSecretOptionDelay();
    void clearSecretsAnimating();
    void adjustSize();
};

#endif // SECRETSHANDLER_H
