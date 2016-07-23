#ifndef DECKHANDLER_H
#define DECKHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/deckcard.h"
#include "Cards/drawcard.h"
#include "utility.h"
#include "enemydeckhandler.h"
#include <QObject>
#include <QMap>
#include <QSignalMapper>


class DeckHandler : public QObject
{
    Q_OBJECT

public:
    DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui, EnemyDeckHandler *enemyDeckHandler);
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
    bool mouseInApp;
    Transparency transparency;
    bool drawAnimating;
    bool showManaLimits;
    int drawDisappear;
    QTreeWidgetItem *loadDeckClasses[10];
    QMap<QString, QTreeWidgetItem *> loadDeckItemsMap;
    EnemyDeckHandler *enemyDeckHandler;
    QString lastCreatedByCode;


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
    void importHearthHead();
    bool showHearthHeadHowTo();
    bool deckBuilderPY();
    void showInstallPY();
    void importEnemyDeck();
    void hideUnknown(bool hidden = true);
    QString getCodeFromDraftLogLine(QString line);
    void updateManaLimits();
    bool isLastCreatedByCodeValid(QString code);
    void removeFromDeck(int index);

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
    void setTheme(Theme value);
    void setGreyedHeight(int value);
    void setCardHeight(int value);
    void setDrawDisappear(int value);
    void setSynchronized();
    void loadDecks();
    bool askSaveDeck();
    void completeArenaDeck(QString draftLog);
    void setShowManaLimits(bool value);

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void deckSizeChanged();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DeckHandler");

public slots:
    void newDeckCardAsset(QString code);
    void newDeckCardDraft(QString code);
    void newDeckCardWeb(QString code, int total);
    void newDeckCardOutsider(QString code, int id);
    void showPlayerCardDraw(QString code, int id, bool mulligan);
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
    void askCreateDeckPY();
    void returnToDeck(QString code, int id);
    void setLastCreatedByCode(QString code);

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
    void newImportHearthHead();
    void newCopyEnemyDeck();
};

#endif // DECKHANDLER_H
