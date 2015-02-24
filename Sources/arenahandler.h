#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "ui_mainwindow.h"
#include "gamewatcher.h"
#include "webuploader.h"
#include "deckhandler.h"
#include <QObject>
#include <QTreeWidgetItem>

class ArenaHandler : public QObject
{
    Q_OBJECT
public:
    ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::MainWindow *ui);
    ~ArenaHandler();

//Variables
private:
    WebUploader *webUploader;
    DeckHandler *deckHandler;
    Ui::MainWindow *ui;
    QTreeWidgetItem *arenaHomeless, *arenaCurrent, *arenaCurrentReward;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    QList<ArenaResult> arenaLogList;
    bool noArena;

//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    bool isRowOk(QTreeWidgetItem *item);
    void newArenaRewards(ArenaRewards &arenaRewards);
    bool newArenaUploadButton(QString &hero);

public:
    void setWebUploader(WebUploader *webUploader);

signals:
    void sendLog(QString line);
    void sendStatusBarMessage(QString message, int timeout=0);

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult);
    bool newArena(QString hero);
    void showArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard);
    void uploadCurrentArenaRewards();

    //WebUploader
    QTreeWidgetItem *showGameResult(GameResult gameResult);
    void showArena(QString hero);
    void reshowGameResult(GameResult gameResult);
    void reshowArena(QString hero);
    void showNoArena();
    void enableButtons();

    //Widgets
    void updateArenaFromWeb();
    void uploadOldLog();
    void enableRefreshButton();
    void openDonateWeb();
};

#endif // ARENAHANDLER_H
