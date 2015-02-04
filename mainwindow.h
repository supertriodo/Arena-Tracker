#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logloader.h"
#include "webuploader.h"
#include "deckcard.h"
#include "hscarddownloader.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QJsonObject>

#define GREEN QColor(200,250,200)
#define RED QColor(Qt::red)
#define YELLOW QColor(Qt::yellow)
#define WHITE QColor(Qt::white)

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
    void resetDeckCardList();
    void initCardsJson();
    void insertDeckCard(DeckCard &deckCard);
    void checkCardImage(DeckCard &deckCard);

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

//Slots
public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult);
    bool newArena(QString hero);
    void showArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void uploadCurrentArenaRewards();
    void newDeckCard(QString card);

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

    void writeLog(QString line);
    void lockDeckInterface();
    void unlockDeckInterface();
    void showCardDrawn(QString code);
    void drawListWidgetItem(DeckCard deckCard, bool drawTotal=true);
};

#endif // MAINWINDOW_H
