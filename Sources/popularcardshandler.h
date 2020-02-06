#ifndef POPULARCARDSHANDLER_H
#define POPULARCARDSHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/popularcard.h"
#include "enemyhandhandler.h"
#include <QObject>

class PopularCardsHandler : public QObject
{
    Q_OBJECT
public:
    PopularCardsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler);
    ~PopularCardsHandler();

//Variables
private:
    Ui::Extended *ui;
    EnemyHandHandler *enemyHandHandler;
    QList<PopularCard> popularCardList;
    CardClass enemyClass;
    bool cardsAnimating;
    bool patreonVersion;
    bool inArena;
    int popularCardsShown;
    int enemyMana;
    int enemyOverloadOwed;
    QList<QString> cardsByPickrate[9][9];
    QMap<QString, float> *cardsPickratesMap;


//Metodos
private:
    void completeUI();
    void showPopularCards();
    void clearAndHide();

public:
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void createCardsByPickrate(const QMap<QString, float> cardsPickratesMap[], QStringList codeList);
    void setCardsPickratesMap(QMap<QString, float> cardsPickratesMap[]);
    void setPopularCardsShown(int value);

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="PopularCardsHandler");

public slots:
    void resetCardsInterface();
    void newTurn(bool isPlayerTurn, int numTurn);
    void setEnemyClass(QString hero);
    void setPremium(bool premium);
    void enemyTagChange(QString tag, QString value);
    void enterArena();
    void leaveArena();

private slots:
    void clearCardsAnimating();
    void adjustSize();
    void findCardEntered(QListWidgetItem *item);
};

#endif // POPULARCARDSHANDLER_H
