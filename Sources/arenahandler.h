#ifndef ARENAHANDLER_H
#define ARENAHANDLER_H

#include "ui_mainwindow.h"
#include "gamewatcher.h"
#include "webuploader.h"
#include "deckhandler.h"
#include "utility.h"
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
    QTreeWidgetItem *arenaHomeless, *arenaCurrent;
    QString arenaCurrentHero;
    QList<GameResult> arenaCurrentGameList; //Se usa en reshowGameResult
    bool noArena;
    Transparency transparency;


//Metodos
private:
    void completeUI();
    void createTreeWidget();
    void setRowColor(QTreeWidgetItem *item, QColor color);
    QColor getRowColor(QTreeWidgetItem *item);
    bool isRowOk(QTreeWidgetItem *item);
    void currentArenaToWhite();
    void allToWhite();

public:
    void setWebUploader(WebUploader *webUploader);
    bool isNoArena();
    void setTransparency(Transparency value);

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="ArenaHandler");

public slots:
    //GameWatcher
    void newGameResult(GameResult gameResult, bool arenaMatch);
    bool newArena(QString hero);
    void showRewards();

    //WebUploader
    QTreeWidgetItem *showGameResult(GameResult gameResult, bool arenaMatch=true);
    void showArena(QString hero);
    void reshowGameResult(GameResult gameResult);
    void reshowArena(QString hero);
    void showNoArena();
    void enableRefreshButton(bool enable=true);
    void syncArenaCurrent();

private slots:
    void refresh();
    void openDonateWeb();
    void hideRewards();
    void uploadRewards();
};

#endif // ARENAHANDLER_H
