#ifndef DECKHANDLER_H
#define DECKHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/deckcard.h"
#include "Cards/drawcard.h"
#include "utility.h"
#include <QObject>
#include <QMap>
#include <QSignalMapper>

#define MALORNE QString("GVG_035")


class DeckHandler : public QObject
{
    Q_OBJECT

public:
    DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui);
    ~DeckHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    QList<DrawCard> drawCardList;
    Ui::Extended *ui;
    QMap<QString, QJsonObject> *cardsJson;
    QJsonObject decksJson;
    QString loadedDeckName;
    bool inGame, inArena, synchronized;
    Transparency transparency;
    int greyedHeight, cardHeight;
    bool drawAnimating;
    int drawDisappear;
    QTreeWidgetItem *loadDeckClasses[10];
    QMap<QString, QTreeWidgetItem *> loadDeckItemsMap;


//Metodos
private:
    void completeUI();
    void insertDeckCard(DeckCard &deckCard);
    void updateTransparency();
    void updateGreyedHeight();
    void updateCardHeight();
    void newDrawCard(QString code);
    void newDeckCard(QString card, int total=1, bool add=false);
    void drawFromDeck(QString code);
    void showDeckButtons();
    void hideDeckButtons();
    void saveDecksJsonFile();
    void hideManageDecksButtons();
    void showManageDecksButtons();
    QString getNewDeckName();
    bool askSaveDeck();
    void addDeckToLoadTree(QString deckName);
    void removeDeckFromLoadTree(QString deckName);
    void createDeckTreeWidget();
    bool isItemClass(QTreeWidgetItem *item);
    void showDeckTreeWidget();

public:
    void reset();
    void redrawDownloadedCardImage(QString code);
    QList<DeckCard> * getDeckComplete();
    void setTransparency(Transparency value);
    void setTheme(Theme value);
    void setGreyedHeight(int value);
    void setCardHeight(int value);
    void setDrawDisappear(int value);
    void setSynchronized();
    void loadDecks();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DeckHandler");

public slots:
    void newDeckCardAsset(QString code);
    void newDeckCardDraft(QString code);
    void newDeckCardWeb(QString code, int total);
    void showPlayerCardDraw(QString code);
    void enableDeckButtons();
    void cardTotalMin();
    void cardTotalPlus();
    void cardRemove();
    void lockDeckInterface();
    void unlockDeckInterface();
    void clearDrawList(bool forceClear=false);
    void deselectRow();
    void enterArena();
    void leaveArena();

private slots:
    void removeOldestDrawCard();
    void clearDrawAnimating();
    void adjustDrawSize();
    void findDeckCardEntered(QListWidgetItem *item);
    void findDrawCardEntered(QListWidgetItem *item);
    void enableDeckButtonSave();
    void saveDeck();
    void newDeck();
    void loadDeck(QString deckName);
    void removeDeck();
    void unselectClassItems();
    void loadSelectedDeck();
    void finishShowDeckTreeWidget();
    void finishHideDeckListWidget();
    void finishHideDeckTreeWidget();
    void finishShowDeckListWidget();
    void hideDeckTreeWidget();
    void toggleLoadDeckTreeWidget();
};

#endif // DECKHANDLER_H
