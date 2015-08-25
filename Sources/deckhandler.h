#ifndef DECKHANDLER_H
#define DECKHANDLER_H

#include "ui_mainwindow.h"
#include "deckcard.h"
#include "utility.h"
#include <QObject>
#include <QMap>

#define MALORNE QString("GVG_035")


class DeckHandler : public QObject
{
    Q_OBJECT
public:
    DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::MainWindow *ui);
    ~DeckHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    Ui::MainWindow *ui;
    QMap<QString, QJsonObject> *cardsJson;
    bool inGame;


//Metodos
private:
    void completeUI();
    void insertDeckCard(DeckCard &deckCard);

public:
    void reset();
    void redrawDownloadedCardImage(QString code);
    QList<DeckCard> * getDeckComplete();

signals:
    void checkCardImage(QString code);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DeckHandler");

public slots:
    void newDeckCardAsset(QString code);
    void newDeckCard(QString card, int total=1, bool noAdd=false);
    void showPlayerCardDraw(QString code);
    void enableDeckButtons();
    void cardTotalMin();
    void cardTotalPlus();
    void cardRemove();
    void lockDeckInterface();
    void unlockDeckInterface();
};

#endif // DECKHANDLER_H
