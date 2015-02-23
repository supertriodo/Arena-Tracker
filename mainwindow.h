#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logloader.h"
#include "gamewatcher.h"
#include "webuploader.h"
#include "deckcard.h"
#include "hscarddownloader.h"
#include "resizebutton.h"
#include "deckhandler.h"
#include "enemyhandhandler.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QJsonObject>


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
    GameWatcher *gameWatcher;
    WebUploader *webUploader;
    HSCardDownloader *cardDownloader;
    DeckHandler *deckHandler;
    EnemyHandHandler *enemyHandHandler;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent, *arenaCurrentReward;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    QList<ArenaResult> arenaLogList;
    bool noArena;
    QMap<QString, QJsonObject> cardsJson;
    QPoint dragPosition;
    ResizeButton *resizeButton;



//Metodos
private:
    void createLogLoader();
    void createGameWatcher();
    void createCardDownloader();
    void createDeckHandler();
    void createEnemyHandHandler();
    void readSettings();
    void writeSettings();
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    bool isRowOk(QTreeWidgetItem *item);
    void newArenaRewards(ArenaRewards &arenaRewards);
    bool newArenaUploadButton(QString &hero);
    void initCardsJson();
    void resizeButtonsText();
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
    void resetDeckFromWeb();

    //DeckHandler//EnemyHandHandler
    void checkCardImage(QString code);

    //HSCardDownloader
    void redrawDownloadedCardImage(QString code);

    //Widgets
    void updateArenaFromWeb();
    void uploadOldLog();
    void enableRefreshButton();
    void openDonateWeb();
    void resizeSlot(QSize size);
    void tabChanged(int index);

    //MainWindow
    void writeLog(QString line);
    void writeLogConnected(QString line);
    void setStatusBarMessage(QString message, int timeout=0);
    void setStatusBarMessageConnected(QString message, int timeout=0);
};

#endif // MAINWINDOW_H
