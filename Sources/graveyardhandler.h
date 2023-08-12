#ifndef GRAVEYARDHANDLER_H
#define GRAVEYARDHANDLER_H


#include "Widgets/ui_extended.h"
#include "Cards/synergycard.h"
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
    QList<SynergyCard> deckCardListPlayer, deckCardListEnemy;
    Ui::Extended *ui;
    Transparency transparency;
    bool inGame, mouseInApp;
    int lastSecretIdAdded;
    QPushButton *planPatreonButton;
    //Iniciamos racesPlayer[ALL]=-1; y cada amalgama hace racesPlayer[ALL]++;
    //De este modo el numero de razas sera racesPlayer.count()+racesPlayer[ALL]
    //Ya que cada amalgama es como incluir una raza distinta
    QMap<CardRace, int> racesPlayer, racesEnemy;
    QMap<CardSchool, bool> schoolsPlayer, schoolsEnemy;

//Metodos
public:
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void redrawDownloadedCardImage(QString code);
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    void setTheme();

private:
    void completeUI();
    void insertDeckCard(bool friendly, SynergyCard &deckCard);
    void newDeckCard(bool friendly, QString card, int id);
    void updateTransparency();
    void findDeckCardEntered(bool friendly, QListWidgetItem *item);
    void reset();
    void onlyShow(CardType cardType);
    void showAll();
    QPixmap drawNumberedIcon(QString iconFile, int numberPlayer, int numberEnemy);
    void updateRacesSchools(bool friendly, SynergyCard &deckCard);
    void redrawMinionSpellIcons();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void showPremiumDialog();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="GraveyardHandler");

public slots:
    void playerCardGraveyard(int id, QString code);
    void enemyCardGraveyard(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void lockGraveyardInterface();
    void unlockGraveyardInterface();
    void setPremium(bool premium);

private slots:
    void findPlayerDeckCardEntered(QListWidgetItem *item);
    void findEnemyDeckCardEntered(QListWidgetItem *item);
    void buttonPlayerClicked();
    void buttonEnemyClicked();
    void buttonMinionsClicked();
    void buttonWeaponsClicked();
    void buttonSpellsClicked();
    void buttonAllClicked();
};

#endif // GRAVEYARDHANDLER_H
