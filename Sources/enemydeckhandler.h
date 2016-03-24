#ifndef ENEMYDECKHANDLER_H
#define ENEMYDECKHANDLER_H

#include "Widgets/ui_extended.h"
#include "enemyhandhandler.h"
#include <QObject>
#include <QMap>

class EnemyDeckHandler : public QObject
{
    Q_OBJECT

public:
    EnemyDeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler);
    ~EnemyDeckHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    Ui::Extended *ui;
    EnemyHandHandler *enemyHandHandler;
    QMap<QString, QJsonObject> *cardsJson;
    Transparency transparency;
    bool inGame, mouseInApp;
    CardClass enemyClass;
    int lastSecretIdAdded;

//Metodos
public:
    void redrawAllCards();
    void redrawDownloadedCardImage(QString code);
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    QList<DeckCard> getDeckCardList();

private:
    void completeUI();
    void insertDeckCard(DeckCard &deckCard);
    void newDeckCard(QString card, int total=1, bool add=false);
    void updateTransparency();
    void reset();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="EnemyDeckHandler");

public slots:
    void enemyCardPlayed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void enemyKnownCardDraw(QString code);
    void setEnemyClass(QString hero);
    void lockEnemyDeckInterface();
    void unlockEnemyDeckInterface();

private slots:
    void findDeckCardEntered(QListWidgetItem *item);
};

#endif // ENEMYDECKHANDLER_H
