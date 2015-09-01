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
#include "arenahandler.h"
#include "secretshandler.h"
#include "drafthandler.h"
#include <QMainWindow>
#include <QJsonObject>

#define DIVIDE_TABS_H 600//700 Rewards
#define DIVIDE_TABS_H2 900//1000 Rewards
#define DIVIDE_TABS_V 900


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
    enum WindowsFormation {H1, H2, H3, V2, _2X2, none};

//Variables
private:
    Ui::MainWindow *ui;
    LogLoader *logLoader;
    GameWatcher *gameWatcher;
    WebUploader *webUploader;
    HSCardDownloader *cardDownloader;
    DeckHandler *deckHandler;
    EnemyHandHandler *enemyHandHandler;
    ArenaHandler *arenaHandler;
    SecretsHandler *secretsHandler;
    DraftHandler * draftHandler;
    QMap<QString, QJsonObject> cardsJson;
    QPoint dragPosition;
    ResizeButton *resizeButton;
    WindowsFormation windowsFormation;
    QFile* atLogFile;
    bool splitWindow;



//Metodos
private:
    void createLogLoader();
    void createArenaHandler();
    void createGameWatcher();
    void createCardDownloader();
    void createDeckHandler();
    void createEnemyHandHandler();
    void createSecretsHandler();
    void createDraftHandler();
    void createWebUploader();
    void readSettings();
    void writeSettings();
    void completeUI();
    void completeToolButton();
    void addDraftMenu(QMenu *menu);
    void addClearDeckMenu(QMenu *menu);
    void addSplitMenu(QMenu *menu);
    void completeHeroButtons();
    void initCardsJson();
    void resizeTabWidgets(QResizeEvent *event);
    void moveTabTo(QWidget *widget, QTabWidget *tabWidget, int index=-1);
    void resetSettings();
    void createLogFile();
    void closeLogFile();
    void resetDeck(bool deckRead=false);
    void test();

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

//Slots
public slots:
    //LogLoader
    void showLogLoadProgress(qint64 logSeek);

    //GameWatcher
    void showTabHeroOnNoArena();

    //WebUploader
    void resetDeckFromWeb();

    //Multi Handlers
    void checkCardImage(QString code);

    //HSCardDownloader
    void redrawDownloadedCardImage(QString code);

    //DraftHandler
    void uploadDeck();

    //Widgets
    void resizeSlot(QSize size);

    //MainWindow
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="MainWindow");
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file);

private slots:
    void synchronizedDone();
    void confirmNewArenaDraft(QString hero);
    void confirmClearDeck();
    void splitWindowAuto();
    void splitWindowNever();
};

#endif // MAINWINDOW_H
