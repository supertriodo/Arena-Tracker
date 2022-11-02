#ifndef DECKHANDLER_H
#define DECKHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/deckcard.h"
#include "utility.h"
#include "enemydeckhandler.h"
#include <QObject>
#include <QMap>
#include <QSignalMapper>
#include <QFutureWatcher>


class DeckHandler : public QObject
{
    Q_OBJECT

public:
    DeckHandler(QObject *parent, Ui::Extended *ui, EnemyDeckHandler *enemyDeckHandler);
    ~DeckHandler();

//Variables
private:
    QList<DeckCard> deckCardList;
    Ui::Extended *ui;
    QJsonObject decksJson;
    QString loadedDeckName;
    bool inGame, inArena;
    bool mouseInApp;
    Transparency transparency;
    bool showManaLimits;
    QTreeWidgetItem *loadDeckClasses[NUM_HEROS + 1];
    QMap<QString, QTreeWidgetItem *> loadDeckItemsMap;
    EnemyDeckHandler *enemyDeckHandler;
    QString lastCreatedByCode;
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
    void newDeckCard(QString code, int total=1, bool outsider=false, int id=0);
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
    bool newDeck(bool reset, bool askSave = true);
    void importDeckString();
    void importDeckString(QString deckString);
    void importEnemyDeck();
    void hideUnknown(bool hidden = true);
    QString getCodeFromDraftLogLine(QString line);
    void updateManaLimits();
    bool isLastCreatedByCodeValid(const QString &code);
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
    void setTheme();
    void setGreyedHeight(int value);
    void setCardHeight(int value);
    void loadDecks();
    bool askSaveDeck();
    void completeArenaDeck(QString draftLog);
    void setShowManaLimits(bool value);
    QList<DeckCard> getDeckCardList();

signals:
    void checkCardImage(QString code);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void deckSizeChanged();
    void showMessageProgressBar(QString text);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DeckHandler");

public slots:
    void newDeckCardAsset(QString code);
    void newDeckCardDraft(QString code);
    void newDeckCardOutsider(QString code, int id);
    void playerCardDraw(QString code, int id);
    void enableDeckButtons();
    void cardTotalMin();
    void cardTotalPlus();
    void cardRemove();
    void lockDeckInterface();
    void unlockDeckInterface();
    void deselectRow();
    void enterArena();
    void leaveArena();
    void returnToDeck(QString code, int id);
    void setLastCreatedByCode(QString code, QString blockType);
    void setFirstOutsiderId(int id);

private slots:
    void findDeckCardEntered(QListWidgetItem *item);
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
    void hideIfDeckSelected();
    void cardTotalPlus(QListWidgetItem *item);
    void exportDeckString();
};

#endif // DECKHANDLER_H
