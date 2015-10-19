#ifndef ENEMYHANDHANDLER_H
#define ENEMYHANDHANDLER_H

#include "handcard.h"
#include "utility.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QMap>

#define COIN QString("GAME_005")

class EnemyHandHandler : public QObject
{
    Q_OBJECT
public:
    EnemyHandHandler(QObject *parent, Ui::MainWindow *ui);
    ~EnemyHandHandler();

//Variables
private:
    QList<HandCard> enemyHandList;
    Ui::MainWindow *ui;
    bool inGame;
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

public:
    void redrawDownloadedCardImage(QString &code);
    void setTransparency(Transparency value);

signals:
    void checkCardImage(QString code);
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
};

#endif // ENEMYHANDHANDLER_H
