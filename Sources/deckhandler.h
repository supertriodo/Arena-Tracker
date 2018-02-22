#ifndef DECKHANDLER_H
#define DECKHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/deckcard.h"
#include "Cards/drawcard.h"
#include "Cards/rngcard.h"
#include "utility.h"
#include "enemydeckhandler.h"
#include "Widgets/bombwindow.h"
#include "planhandler.h"
#include <QObject>
#include <QMap>
#include <QSignalMapper>
#include <QFutureWatcher>


class DeckHandler : public QObject
{
    Q_OBJECT

public:
    DeckHandler(QObject *parent, Ui::Extended *ui, EnemyDeckHandler *enemyDeckHandler, PlanHandler *planHandler);
    ~DeckHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    QList<DrawCard> drawCardList;
    QList<RngCard> rngCardList;
    Ui::Extended *ui;
    QJsonObject decksJson;
    QString loadedDeckName;
    bool inGame, inArena;
    bool mouseInApp;
    Transparency transparency;
    bool drawAnimating;
    bool rngAnimating;
    bool showManaLimits;
    bool showRngList;
    int drawDisappear;
    QTreeWidgetItem *loadDeckClasses[10];
    QMap<QString, QTreeWidgetItem *> loadDeckItemsMap;
    EnemyDeckHandler *enemyDeckHandler;
    PlanHandler *planHandler;
    QString lastCreatedByCode;
    BombWindow *bombWindow;
    //Nos permite saber el code de las starting cards para devolverlas al deck durante el mulligan.
    //Tb permite identificar cartas originales de nuestro deck de outsiders
    QMap<int,QString> cardId2Code;
    //Lo usamos para diferenciar outsiders
    int firstOutsiderId;


//Metodos
private:
    void completeUI();
    void insertDeckCard(DeckCard &deckCard);
    void updateTransparency();
    void newDrawCard(QString code, bool mulligan);
    void newDeckCard(QString code, int total=1, bool add=false, bool outsider=false, int id=0);
    void drawFromDeck(QString code, int id);
    void showDeckButtons();
    void hideDeckButtons();
    void saveDecksJsonFile();
    void hideManageDecksButtons();
    void showManageDecksButtons();
    QString getNewDeckName();
    void addDeckToLoadTree(QString deckName);
    void removeDeckFromLoadTree(QString deckName);
    void createLoadDeckTreeWidget();
    bool isItemClass(QTreeWidgetItem *item);
    void showDeckTreeWidget();
    void addNewDeckMenu(QPushButton *button);
    bool newDeck(bool reset);
    void importDeckString();
    void importEnemyDeck();
    void hideUnknown(bool hidden = true);
    QString getCodeFromDraftLogLine(QString line);
    void updateManaLimits();
    bool isLastCreatedByCodeValid(QString code);
    void removeFromDeck(int index);
    void clearRngList();
    void createBombWindow();
    void showBombWindow();
    void newRngCard(QString code, int id);

public:
    void reset();
    void redrawAllCards();
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void updateIconSize(int cardHeight);
    QList<DeckCard> *getDeckComplete();
    int getNumCardRows();
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);
    void setTheme();
    void setGreyedHeight(int value);
    void setCardHeight(int value);
    void setDrawDisappear(int value);
    void loadDecks();
    bool askSaveDeck();
    void completeArenaDeck(QString draftLog);
    void setShowManaLimits(bool value);
    void setShowRngList(bool value);
    QList<DeckCard> getDeckCardList();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void deckSizeChanged();
    void showMessageProgressBar(QString text);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DeckHandler");

public slots:
    void newDeckCardAsset(QString code);
    void newDeckCardDraft(QString code);
    void newDeckCardWeb(QString code, int total);
    void newDeckCardOutsider(QString code, int id);
    void playerCardDraw(QString code, int id);
    void playerCardToHand(int id, QString code, int turn);
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
    void returnToDeck(QString code, int id);
    void setLastCreatedByCode(QString code, QString blockType);
    void removeRngCard(int id, QString code="");
    void setFirstOutsiderId(int id);

private slots:
    void removeOldestDrawCard();
    void clearDrawAnimating();
    void adjustDrawSize();
    void findDeckCardEntered(QListWidgetItem *item);
    void findDrawCardEntered(QListWidgetItem *item);
    void enableDeckButtonSave(bool enable=true);
    void saveDeck();
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
    void newEmptyDeck();
    void newCopyCurrentDeck();
    void newImportDeckString();
    void newCopyEnemyDeck();
    void adjustRngSize();
    void clearRngAnimating();
    void rngCardEntered(QListWidgetItem *item);
    void hideIfDeckSelected();
    void cardTotalPlus(QListWidgetItem *item);
    void exportDeckString();
};

#endif // DECKHANDLER_H
