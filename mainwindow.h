#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logloader.h"
#include "webuploader.h"
#include "deckcard.h"
#include "hscarddownloader.h"
#include "resizebutton.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QJsonObject>

#define GREEN QColor(200,250,200)
#define RED QColor(Qt::red)
#define YELLOW QColor(Qt::yellow)
#define WHITE QColor(Qt::white)
#define BLACK QColor(Qt::black)
#define CARD_SIZE QSize(218,35)
#define COIN QString("GAME_005")
#define MALORNE QString("GVG_035")

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    enum TabEnum { tabArena, tabDeck, tabEnemy, tabLog };

//Variables
private:
    Ui::MainWindow *ui;
    LogLoader *logLoader;
    WebUploader *webUploader;
    HSCardDownloader *cardDownloader;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent, *arenaCurrentReward;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    QList<ArenaResult> arenaLogList;
    bool noArena;
    QList<DeckCard> deckCardList;
    QMap<QString, QJsonObject> cardsJson;
    int remainingCards;
    QPoint dragPosition;
    ResizeButton *resizeButton;
    QList<HandCard> enemyHandList;


//Metodos
private:
    void createLogLoader();
    void createCardDownloader();
    void readSettings();
    void writeSettings();
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    bool isRowOk(QTreeWidgetItem *item);
    void newArenaRewards(ArenaRewards &arenaRewards);
    bool newArenaUploadButton(QString &hero);
    void setStatusBarMessage(const QString &message, int timeout=0);
    void initCardsJson();
    void insertDeckCard(DeckCard &deckCard);
    void checkCardImage(QString code);
    void resizeButtonsText();
    void resetDeckCardList();
    void drawCardItem(QListWidgetItem * item, QString code, uint total);
    void drawDeckCardItem(DeckCard deckCard, bool drawTotal=true);
    void drawHandCardItem(HandCard handCard);
    void resetSettings();

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

//Slots
public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult);
    bool newArena(QString hero);
    void showArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void uploadCurrentArenaRewards();
    void newDeckCard(QString card, int total=1);
    void showPlayerCardDraw(QString code);
    void showEnemyCardDraw(int id, int turn, bool special, QString code);
    void showEnemyCardPlayed(int id, QString code);

    //LogLoader
    void createWebUploader();
    void showLogLoadProgress(qint64 logSeek);

    //WebUploader
    QTreeWidgetItem *showGameResult(GameResult gameResult);
    void showArena(QString hero);
    void reshowGameResult(GameResult gameResult);
    void reshowArena(QString hero);
    void enableButtons();
    void showNoArena();

    //Buttons
    void updateArenaFromWeb();
    void uploadOldLog();
    void enableRefreshButton();
    void openDonateWeb();
    void enableDeckButtons();
    void cardTotalMin();
    void cardTotalPlus();
    void resizeSlot(QSize size);

    void redrawDownloadedCardImage(QString code);
    void writeLog(QString line);
    void lockDeckInterface();
    void unlockDeckInterface();
    void resetDeckFromWeb();
    void lastHandCardIsCoin();
};

#endif // MAINWINDOW_H
