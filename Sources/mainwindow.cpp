#include "mainwindow.h"
#include "Widgets/ui_extended.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include "Widgets/cardwindow.h"
#include "versionchecker.h"
#include <QtWidgets>

using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::Extended)
{
    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");
    ui->setupUi(this);

    atLogFile = NULL;
    isMainWindow = true;
    mouseInApp = false;
    otherWindow = NULL;
    copyGameLogs = false;
    draftLogFile = "";
    cardHeight = -1;

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)
    logLoader = NULL;
    gameWatcher = NULL;
    arenaHandler = NULL;
    cardDownloader = NULL;
    enemyHandHandler = NULL;
    draftHandler = NULL;
    deckHandler = NULL;
    enemyDeckHandler = NULL;
    secretsHandler = NULL;

    createDataDir();
    createLogFile();
    completeUI();

    createCardDownloader();
    createPlanHandler();
    createEnemyHandHandler();
    createEnemyDeckHandler();//-->EnemyHandHandler
    createDeckHandler();//-->EnemyDeckHandler
    createDraftHandler();//-->DeckHandler
    createSecretsHandler();//-->EnemyHandHandler
    createArenaHandler();//-->DeckHandler
    createGameWatcher();//-->A lot
    createLogLoader();//-->GameWatcher -->DraftHandler
    createCardWindow();//-->A lot

    readSettings();
    checkGamesLogDir();
    createVersionChecker();
}


MainWindow::MainWindow(QWidget *parent, MainWindow *primaryWindow) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::Extended)
{
    atLogFile = NULL;
    logLoader = NULL;
    gameWatcher = NULL;
    arenaHandler = NULL;
    webUploader = NULL;
    cardDownloader = NULL;
    enemyHandHandler = NULL;
    draftHandler = NULL;
    deckHandler = NULL;
    enemyDeckHandler = NULL;
    secretsHandler = NULL;
    isMainWindow = false;
    mouseInApp = false;
    copyGameLogs = false;
    draftLogFile = "";
    cardHeight = -1;
    otherWindow = primaryWindow;

    completeUI();
    readSettings();
}


MainWindow::~MainWindow()
{
    if(logLoader != NULL)       delete logLoader;
    if(gameWatcher != NULL)     delete gameWatcher;
    if(arenaHandler != NULL)    delete arenaHandler;
    if(webUploader != NULL)     delete webUploader;
    if(cardDownloader != NULL)  delete cardDownloader;
    if(enemyDeckHandler != NULL) delete enemyDeckHandler;
    if(enemyHandHandler != NULL) delete enemyHandHandler;
    if(draftHandler != NULL)    delete draftHandler;
    if(deckHandler != NULL)     delete deckHandler;
    if(secretsHandler != NULL)  delete secretsHandler;
    if(ui != NULL)              delete ui;
    closeLogFile();
}


void MainWindow::createSecondaryWindow()
{
    this->otherWindow = new MainWindow(0, this);
    calculateDeckWindowMinimumWidth();
    deckHandler->setTransparency(Transparent);
    updateMainUITheme();

    this->windowsFormation = None;
    this->resizeChecks(this->size());

    connect(ui->minimizeButton, SIGNAL(clicked()),
            this->otherWindow, SLOT(showMinimized()));
}


void MainWindow::destroySecondaryWindow()
{
    disconnect(ui->minimizeButton, 0, this->otherWindow, 0);
    this->otherWindow->close();
    this->otherWindow = NULL;
    deckHandler->setTransparency(this->transparency);

    ui->tabDeckLayout->setContentsMargins(0, 40, 0, 0);
    this->windowsFormation = None;
    this->resize(this->width() + 30, this->height());
    this->resize(this->width() - 30, this->height());
}


void MainWindow::resetDeckDontRead()
{
    resetDeck(true);
}


void MainWindow::resetDeck(bool deckRead)
{
    gameWatcher->setDeckRead(deckRead);
    deckHandler->reset();
}


QString MainWindow::getHSLanguage()
{
    QString lang = "";

    if(logLoader != NULL)
    {
        QDir dir(QFileInfo(logLoader->getLogConfigPath()).absolutePath() + "/Cache/UberText");
        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Time);


        switch (dir.count())
        {
        case 0:
            lang = "enUS";
            break;
        case 1:
            foreach(QString file, dir.entryList())
            {
                lang = file.mid(5,4);
            }
            break;
        default:
            QStringList files = dir.entryList();
            lang = files.takeFirst().mid(5,4);

            //Remove old languages files
            foreach(QString file, files)
            {
                dir.remove(file);
                pDebug(file + " removed.");
            }

            //Remove old image cards
            QDir dirHSCards(Utility::hscardsPath());
            dirHSCards.setFilter(QDir::Files);
            QStringList filters("*_*.png");
            dirHSCards.setNameFilters(filters);

            foreach(QString file, dirHSCards.entryList())
            {
                dirHSCards.remove(file);
                pDebug(file + " removed.");
            }

            break;
        }
    }


    if(lang != "enGB" && lang != "enUS" && lang != "esES" && lang != "esMX" &&
            lang != "deDE" && lang != "frFR" && lang != "itIT" &&
            lang != "plPL" && lang != "ptBR" && lang != "ruRU" &&
            lang != "koKR" && lang != "zhCN" && lang != "zhTW" &&
            lang != "jaJP" && lang != "thTH")
    {
        pDebug("Language: " + lang + " not supported. Using enUS.");
        pLog("Settings: Language " + lang + " not supported. Using enUS.");
        lang = "enUS";
    }
    else if(lang == "enGB")
    {
        pDebug("Language: " + lang + ". Using enUS.");
        pLog("Settings: Language " + lang + ". Using enUS.");
        lang = "enUS";
    }
    else
    {
        pDebug("Language: " + lang + ".");
        pLog("Settings: Language " + lang + ".");
    }

    cardDownloader->setLang(lang);
    return lang;
}


void MainWindow::createCardsJsonMap(QMap<QString, QJsonObject> &cardsJson, QString lang)
{
    QFile jsonFile(":Json/cards." + lang + ".json");

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonArray jsonArray = jsonDoc.array();
    foreach(QJsonValue jsonCard, jsonArray)
    {
        QJsonObject jsonCardObject = jsonCard.toObject();
        cardsJson[jsonCardObject.value("id").toString()] = jsonCardObject;
    }
}


void MainWindow::initCardsJson()
{
    QString lang = getHSLanguage();
    createCardsJsonMap(this->cardsJson, lang);
    DeckCard::setCardsJson(&cardsJson);
    Utility::setCardsJson(&cardsJson);


    if(lang == "enUS")
    {
        Utility::setEnCardsJson(&cardsJson);
    }
    else
    {
        createCardsJsonMap(enCardsJson, "enUS");
        Utility::setEnCardsJson(&enCardsJson);
    }
}


void MainWindow::createVersionChecker()
{
    VersionChecker *versionChecker = new VersionChecker(this);
    connect(versionChecker, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(versionChecker, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createDraftHandler()
{
    draftHandler = new DraftHandler(this, ui);
    connect(draftHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(draftHandler, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCardDraft(QString)));
    connect(draftHandler, SIGNAL(draftEnded()),
            this, SLOT(uploadDeck()));

    //Connect en logLoader
//    connect(draftHandler, SIGNAL(draftEnded()),
//            logLoader, SLOT(setUpdateTimeMax()));
//    connect(draftHandler, SIGNAL(draftStarted()),
//            logLoader, SLOT(setUpdateTimeMin()));

    connect(draftHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(draftHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createSecretsHandler()
{
    secretsHandler = new SecretsHandler(this, ui);
    connect(secretsHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(secretsHandler, SIGNAL(duplicated(QString)),
            enemyHandHandler, SLOT(convertDuplicates(QString)));
    connect(secretsHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(secretsHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createArenaHandler()
{
    arenaHandler = new ArenaHandler(this, deckHandler, ui);
    connect(arenaHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(arenaHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createEnemyDeckHandler()
{
    enemyDeckHandler = new EnemyDeckHandler(this, &cardsJson, ui, enemyHandHandler);
    connect(enemyDeckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyDeckHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(enemyDeckHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(enemyDeckHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createDeckHandler()
{
    deckHandler = new DeckHandler(this, &cardsJson, ui, enemyDeckHandler);
    connect(deckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(deckHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(deckHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(deckHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
    connect(deckHandler, SIGNAL(deckSizeChanged()),
            this, SLOT(spreadCorrectTamCard()));

    //connect de completeConfigTab
    connect(ui->configButtonCreateDeckPY, SIGNAL(clicked()),
            deckHandler, SLOT(askCreateDeckPY()));

    deckHandler->loadDecks();
}


void MainWindow::createEnemyHandHandler()
{
    enemyHandHandler = new EnemyHandHandler(this, ui);
    connect(enemyHandHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyHandHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(enemyHandHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(enemyHandHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createPlanHandler()
{
    planHandler = new PlanHandler(this, ui);
    connect(planHandler, SIGNAL(checkCardImage(QString, bool)),
            this, SLOT(checkCardImage(QString, bool)));
    connect(planHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(planHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(planHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createCardWindow()
{
    cardWindow = new CardWindow(this);
    connect(deckHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(enemyDeckHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(enemyHandHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(secretsHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(draftHandler, SIGNAL(overlayCardEntered(QString, QRect, int, int, bool)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int, bool)));

    connect(ui->deckListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->enemyDeckListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->drawListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->enemyHandListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->secretsTreeWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(draftHandler, SIGNAL(overlayCardLeave()),
            cardWindow, SLOT(hide()));
    connect(draftHandler, SIGNAL(draftStarted()),
            cardWindow, SLOT(hide()));
}


void MainWindow::createCardDownloader()
{
    cardDownloader = new HSCardDownloader(this);
    connect(cardDownloader, SIGNAL(downloaded(QString)),
            this, SLOT(redrawDownloadedCardImage(QString)));
    connect(cardDownloader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(cardDownloader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createGameWatcher()
{
    gameWatcher = new GameWatcher(this);

    connect(gameWatcher, SIGNAL(arenaDeckRead()),
            this, SLOT(completeArenaDeck()));
    connect(gameWatcher, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(gameWatcher, SIGNAL(pDebug(QString,qint64,DebugLevel,QString)),
            this, SLOT(pDebug(QString,qint64,DebugLevel,QString)));

    connect(gameWatcher, SIGNAL(newGameResult(GameResult, LoadingScreenState, QString)),
            arenaHandler, SLOT(newGameResult(GameResult, LoadingScreenState, QString)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            arenaHandler, SLOT(newArena(QString)));
    connect(gameWatcher, SIGNAL(inRewards()),
            arenaHandler, SLOT(showRewards()));

    connect(gameWatcher, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCardAsset(QString)));
    connect(gameWatcher, SIGNAL(playerCardDraw(QString, bool)),
            deckHandler, SLOT(showPlayerCardDraw(QString, bool)));
    connect(gameWatcher, SIGNAL(playerReturnToDeck(QString)),
            deckHandler, SLOT(returnToDeck(QString)));
    connect(gameWatcher, SIGNAL(clearDrawList(bool)),
            deckHandler, SLOT(clearDrawList(bool)));
    connect(gameWatcher, SIGNAL(startGame()),
            deckHandler, SLOT(lockDeckInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool)),
            deckHandler, SLOT(unlockDeckInterface()));
    connect(gameWatcher, SIGNAL(enterArena()),
            deckHandler, SLOT(enterArena()));
    connect(gameWatcher, SIGNAL(leaveArena()),
            deckHandler, SLOT(leaveArena()));

    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString)),
            enemyDeckHandler, SLOT(enemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretRevealed(int, QString)),
            enemyDeckHandler, SLOT(enemySecretRevealed(int, QString)));
    connect(gameWatcher, SIGNAL(enemyKnownCardDraw(QString)),
            enemyDeckHandler, SLOT(enemyKnownCardDraw(QString)));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyDeckHandler, SLOT(lockEnemyDeckInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool)),
            enemyDeckHandler, SLOT(unlockEnemyDeckInterface()));
    connect(gameWatcher, SIGNAL(enemyHero(QString)),
            enemyDeckHandler, SLOT(setEnemyClass(QString)));

    connect(gameWatcher, SIGNAL(enemyCardDraw(int,int,bool,QString)),
            enemyHandHandler, SLOT(showEnemyCardDraw(int,int,bool,QString)));
    //Se llama desde enemyDeckHandler->enemyCardPlayed()
//    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString)),
//            enemyHandHandler, SLOT(hideEnemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(lastHandCardIsCoin()),
            enemyHandHandler, SLOT(lastHandCardIsCoin()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString, int, int)),
            enemyHandHandler, SLOT(setLastCreatedByCode(QString)));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyHandHandler, SLOT(lockEnemyInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool)),
            enemyHandHandler, SLOT(unlockEnemyInterface()));

    connect(gameWatcher, SIGNAL(playerMinionZonePlayAdd(QString,int,int)),
            planHandler, SLOT(playerMinionZonePlayAdd(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayAdd(QString,int,int)),
            planHandler, SLOT(enemyMinionZonePlayAdd(QString,int,int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlayAddTriggered(QString,int,int)),
            planHandler, SLOT(playerMinionZonePlayAddTriggered(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayAddTriggered(QString,int,int)),
            planHandler, SLOT(enemyMinionZonePlayAddTriggered(QString,int,int)));
    connect(gameWatcher, SIGNAL(playerHeroZonePlayAdd(QString,int)),
            planHandler, SLOT(playerHeroZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(enemyHeroZonePlayAdd(QString,int)),
            planHandler, SLOT(enemyHeroZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(playerWeaponZonePlayAdd(QString, int)),
            planHandler, SLOT(playerWeaponZonePlayAdd(QString, int)));
    connect(gameWatcher, SIGNAL(enemyWeaponZonePlayAdd(QString, int)),
            planHandler, SLOT(enemyWeaponZonePlayAdd(QString, int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlayRemove(int)),
            planHandler, SLOT(playerMinionZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayRemove(int)),
            planHandler, SLOT(enemyMinionZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlaySteal(int,int)),
            planHandler, SLOT(playerMinionZonePlaySteal(int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlaySteal(int,int)),
            planHandler, SLOT(enemyMinionZonePlaySteal(int,int)));
    connect(gameWatcher, SIGNAL(playerMinionPosChange(int,int)),
            planHandler, SLOT(playerMinionPosChange(int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionPosChange(int,int)),
            planHandler, SLOT(enemyMinionPosChange(int,int)));
    connect(gameWatcher, SIGNAL(playerMinionTagChange(int,QString,QString)),
            planHandler, SLOT(playerMinionTagChange(int,QString,QString)));
    connect(gameWatcher, SIGNAL(enemyMinionTagChange(int,QString,QString)),
            planHandler, SLOT(enemyMinionTagChange(int,QString,QString)));
    connect(gameWatcher, SIGNAL(zonePlayAttack(QString, int,int)),
            planHandler, SLOT(zonePlayAttack(QString, int,int)));
    connect(gameWatcher, SIGNAL(newTurn(bool, int)),
            planHandler, SLOT(newTurn(bool, int)));
    connect(gameWatcher, SIGNAL(logTurn()),
            planHandler, SLOT(resetLastPowerAddon()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString,QString,int, int)),
            planHandler, SLOT(setLastTriggerId(QString,QString,int, int)));
    connect(gameWatcher, SIGNAL(playerCardObjPlayed(QString,int,int)),
            planHandler, SLOT(playerCardObjPlayed(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyCardObjPlayed(QString,int,int)),
            planHandler, SLOT(enemyCardObjPlayed(QString,int,int)));
    connect(gameWatcher, SIGNAL(startGame()),
            planHandler, SLOT(lockPlanInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool)),
            planHandler, SLOT(endGame(bool)));


    connect(gameWatcher, SIGNAL(endGame(bool)),
            secretsHandler, SLOT(resetSecretsInterface()));
    connect(gameWatcher, SIGNAL(enemySecretPlayed(int,SecretHero)),
            secretsHandler, SLOT(secretPlayed(int,SecretHero)));
    connect(gameWatcher, SIGNAL(enemySecretStealed(int,QString)),
            secretsHandler, SLOT(secretStealed(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretRevealed(int, QString)),
            secretsHandler, SLOT(secretRevealed(int, QString)));
    connect(gameWatcher, SIGNAL(playerSpellPlayed()),
            secretsHandler, SLOT(playerSpellPlayed()));
    connect(gameWatcher, SIGNAL(playerSpellObjPlayed()),
            secretsHandler, SLOT(playerSpellObjPlayed()));
    connect(gameWatcher, SIGNAL(playerMinionPlayed(int)),
            secretsHandler, SLOT(playerMinionPlayed(int)));
    connect(gameWatcher, SIGNAL(enemyMinionDead(QString)),
            secretsHandler, SLOT(enemyMinionDead(QString)));
    connect(gameWatcher, SIGNAL(avengeTested()),
            secretsHandler, SLOT(avengeTested()));
    connect(gameWatcher, SIGNAL(cSpiritTested()),
            secretsHandler, SLOT(cSpiritTested()));
    connect(gameWatcher, SIGNAL(playerAttack(bool,bool)),
            secretsHandler, SLOT(playerAttack(bool,bool)));
    connect(gameWatcher, SIGNAL(playerHeroPower()),
            secretsHandler, SLOT(playerHeroPower()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString, int, int)),
            secretsHandler, SLOT(resetLastMinionDead(QString, QString)));

    //Connect en synchronizedDone
//    connect(gameWatcher,SIGNAL(newArena(QString)),
//            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher,SIGNAL(activeDraftDeck()),
            draftHandler, SLOT(endDraft()));
    connect(gameWatcher,SIGNAL(startGame()),    //Salida alternativa de drafting (+seguridad)
            draftHandler, SLOT(endDraft()));
    connect(gameWatcher,SIGNAL(needResetDeck()),
            this, SLOT(resetDeck()));
    connect(gameWatcher,SIGNAL(pickCard(QString)),
            draftHandler, SLOT(pickCard(QString)));
}


void MainWindow::showTabHeroOnNoArena()
{
    if(webUploader == NULL) return;
    if(arenaHandler->isNoArena())
    {
        ui->tabWidget->addTab(ui->tabHero, "Draft");
        ui->tabWidget->setCurrentWidget(ui->tabHero);
        this->calculateMinimumWidth();
    }
}


void MainWindow::createLogLoader()
{
    logLoader = new LogLoader(this);
    connect(logLoader, SIGNAL(synchronized()),
            this, SLOT(synchronizedDone()));
    connect(logLoader, SIGNAL(logReset()),
            this, SLOT(logReset()));
    connect(logLoader, SIGNAL(newLogLineRead(LogComponent, QString,qint64,qint64)),
            gameWatcher, SLOT(processLogLine(LogComponent, QString,qint64,qint64)));
    connect(logLoader, SIGNAL(logConfigSet()),
            this, SLOT(initCardsJson()));
    connect(logLoader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(logLoader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

    //Connect en synchronizedDone
//    connect(gameWatcher, SIGNAL(gameLogComplete(qint64,qint64,QString)),
//            logLoader, SLOT(copyGameLog(qint64,qint64,QString)));

    //Connect de draftHandler
    connect(draftHandler, SIGNAL(draftEnded()),
            logLoader, SLOT(setUpdateTimeMax()));
    connect(draftHandler, SIGNAL(draftStarted()),
            logLoader, SLOT(setUpdateTimeMin()));

    if(!logLoader->init())  QTimer::singleShot(1, this, SLOT(closeApp()));
}


void MainWindow::synchronizedDone()
{
    createWebUploader();
    gameWatcher->setSynchronized();
    secretsHandler->setSynchronized();
    deckHandler->setSynchronized();

    spreadTransparency(this->transparency);

    //Connections after synchronized
    connect(gameWatcher,SIGNAL(newArena(QString)),
            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            this, SLOT(resetDeckDontRead()));
    connect(gameWatcher, SIGNAL(gameLogComplete(qint64,qint64,QString)),
            logLoader, SLOT(copyGameLog(qint64,qint64,QString)));


    //Test
    QTimer::singleShot(5000, this, SLOT(test()));
}


void MainWindow::currentArenaToWhiteAM(bool connected)
{
    if(connected) arenaHandler->currentArenaToWhite();
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this);
    connect(webUploader, SIGNAL(loadedGameResult(GameResult, LoadingScreenState)),
            arenaHandler, SLOT(showGameResult(GameResult, LoadingScreenState)));
    connect(webUploader, SIGNAL(loadedArena(QString)),
            arenaHandler, SLOT(showArena(QString)));
    connect(webUploader, SIGNAL(reloadedGameResult(GameResult)),
            arenaHandler, SLOT(reshowGameResult(GameResult)));
    connect(webUploader, SIGNAL(reloadedArena(QString)),
            arenaHandler, SLOT(reshowArena(QString)));
    connect(webUploader, SIGNAL(synchronized()),
            arenaHandler, SLOT(enableRefreshButton()));
    connect(webUploader, SIGNAL(synchronized()),
            arenaHandler, SLOT(syncArenaCurrent()));
    connect(webUploader, SIGNAL(noArenaFound()),
            arenaHandler, SLOT(showNoArena()));
    connect(webUploader, SIGNAL(connectionTried(bool)),
            this, SLOT(updateAMConnectButton(bool)));
    connect(webUploader, SIGNAL(connectionTried(bool)),
            this, SLOT(currentArenaToWhiteAM(bool)));
    connect(webUploader, SIGNAL(loadArenaCurrentFinished()),
            arenaHandler, SLOT(removeDuplicateArena()));
    connect(webUploader, SIGNAL(loadArenaCurrentFinished()),
            arenaHandler, SLOT(linkLogsToWebGames()));
    connect(webUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(webUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));


    arenaHandler->setWebUploader(webUploader);
    tryConnectAM();
}


void MainWindow::completeUI()
{
    if(isMainWindow)
    {
        ui->tabWidget->clear();
        ui->tabWidget->hide();

        ui->tabWidgetH2 = new MoveTabWidget(this);
        ui->tabWidgetH2->hide();
        ui->gridLayout->addWidget(ui->tabWidgetH2, 0, 1);
        ui->tabWidgetH3 = new MoveTabWidget(this);
        ui->tabWidgetH3->hide();
        ui->gridLayout->addWidget(ui->tabWidgetH3, 0, 2);
        ui->tabWidgetV1 = new MoveTabWidget(this);
        ui->tabWidgetV1->hide();
        ui->tabWidgetV1->setTabBarAutoHide(true);
        ui->gridLayout->addWidget(ui->tabWidgetV1, 1, 0);

        ui->progressBar->setVisible(false);

        completeConfigTab();

        connect(ui->tabWidget, SIGNAL(currentChanged(int)),
                this, SLOT(spreadMouseInApp()));

        ui->replayButton->hide();//TODO eliminar


#ifdef QT_DEBUG
        pLog(tr("MODE DEBUG"));
        pDebug("MODE DEBUG");
#endif

#ifdef Q_OS_WIN
        pLog(tr("Settings: Platform: Windows"));
        pDebug("Platform: Windows");
#endif

#ifdef Q_OS_MAC
        pLog(tr("Settings: Platform: Mac"));
        pDebug("Platform: Mac");
#endif

#ifdef Q_OS_LINUX
        pLog(tr("Settings: Platform: Linux"));
        pDebug("Platform: Linux");
#endif
    }
    else
    {
        this->setWindowIcon(QIcon(":/Images/icon.png"));

        setCentralWidget(this->otherWindow->ui->tabDeck);
        this->otherWindow->ui->tabDeckLayout->setContentsMargins(0, 0, 0, 0);
        this->otherWindow->ui->tabDeck->show();
    }
    completeUIButtons();
}


void MainWindow::completeUIButtons()
{
    if(isMainWindow)
    {
        ui->closeButton = new QPushButton("", this);
        ui->closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->closeButton->setIcon(QIcon(":/Images/close.png"));
        ui->closeButton->setFlat(true);
        connect(ui->closeButton, SIGNAL(clicked()),
                this, SLOT(closeApp()));


        ui->minimizeButton = new QPushButton("", this);
        ui->minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->minimizeButton->setIcon(QIcon(":/Images/minimize.png"));
        ui->minimizeButton->setFlat(true);
        connect(ui->minimizeButton, SIGNAL(clicked()),
                this, SLOT(showMinimized()));
    }


    ui->resizeButton = new ResizeButton(this);
    ui->resizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->resizeButton->resize(24, 24);
    ui->resizeButton->setIconSize(QSize(24, 24));
    ui->resizeButton->setIcon(QIcon(":/Images/resize.png"));
    ui->resizeButton->setFlat(true);
    connect(ui->resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));
}


void MainWindow::closeApp()
{
    //Check unsaved decks
    if(ui->deckButtonSave->isEnabled() && !deckHandler->askSaveDeck())   return;
    removeNonCompleteDraft();
    close();
}


//void MainWindow::completeHeroButtons()
//{
//    QSignalMapper* mapper = new QSignalMapper(this);
//    QString heroes[9] = {"Warrior", "Shaman", "Rogue",
//                         "Paladin", "Hunter", "Druid",
//                         "Warlock", "Mage", "Priest"};
//    QPushButton *heroButtons[9] = {ui->heroButton1, ui->heroButton2, ui->heroButton3,
//                                  ui->heroButton4, ui->heroButton5, ui->heroButton6,
//                                  ui->heroButton7, ui->heroButton8, ui->heroButton9};

//    for(int i=0; i<9; i++)
//    {
//        mapper->setMapping(heroButtons[i], heroes[i]);
//        connect(heroButtons[i], SIGNAL(clicked()), mapper, SLOT(map()));
//        heroButtons[i]->setToolTip(heroes[i]);
//    }

//    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));
//}


void MainWindow::initConfigTab(int tooltipScale, int cardHeight, bool autoSize,
                               bool showClassColor, bool showSpellColor,
                               bool createGoldenCards, int maxGamesLog,
                               QString AMplayerEmail, QString AMpassword)
{
    //Actions
    ui->configCheckGoldenCards->setChecked(createGoldenCards);

    //UI
    switch(transparency)
    {
        case Transparent:
            ui->configRadioTransparent->setChecked(true);
            break;
        case AutoTransparent:
            ui->configRadioAuto->setChecked(true);
            break;
        case Opaque:
            ui->configRadioOpaque->setChecked(true);
            break;
        default:
            transparency = AutoTransparent;
            ui->configRadioAuto->setChecked(true);
            break;
    }

    if(this->theme == ThemeBlack) ui->configCheckDarkTheme->setChecked(true);
    if(this->splitWindow) ui->configCheckWindowSplit->setChecked(true);
    if(this->otherWindow!=NULL) ui->configCheckDeckWindow->setChecked(true);

    //Deck
    if(cardHeight<ui->configSliderCardSize->minimum() || cardHeight>ui->configSliderCardSize->maximum())  cardHeight = 35;
    if(ui->configSliderCardSize->value() == cardHeight)   updateTamCard(cardHeight);
    else    ui->configSliderCardSize->setValue(cardHeight);

    if(tooltipScale<ui->configSliderTooltipSize->minimum() || tooltipScale>ui->configSliderTooltipSize->maximum())  tooltipScale = 10;
    if(ui->configSliderTooltipSize->value() == tooltipScale) updateTooltipScale(tooltipScale);
    else ui->configSliderTooltipSize->setValue(tooltipScale);

    ui->configCheckAutoSize->setChecked(autoSize);

    ui->configCheckClassColor->setChecked(showClassColor);
    updateShowClassColor(showClassColor);

    ui->configCheckSpellColor->setChecked(showSpellColor);
    updateShowSpellColor(showSpellColor);

    //Hand
    //Slider            0  - Ns - 11
    //DrawDissapear     -1 - Ns - 0
    switch(this->drawDisappear)
    {
        case -1:
            ui->configSliderDrawTime->setValue(0);
            updateTimeDraw(0);
            break;
        case 0:
            ui->configSliderDrawTime->setValue(11);
            break;
        default:
            if(this->drawDisappear<-1 || this->drawDisappear>10)    this->drawDisappear = 5;
            ui->configSliderDrawTime->setValue(this->drawDisappear);
            break;
    }


    //Draft
    if(this->showDraftOverlay) ui->configCheckOverlay->setChecked(true);
    draftHandler->setShowDraftOverlay(this->showDraftOverlay);

    if(this->draftLearningMode) ui->configCheckLearning->setChecked(true);
    draftHandler->setLearningMode(this->draftLearningMode);

    //Zero To Heroes
    ui->configSliderZero->setValue(maxGamesLog);
    updateMaxGamesLog(maxGamesLog);

    //Arena Mastery
    ui->configLineEditMastery->setText(AMplayerEmail);
    ui->configLineEditMastery2->setText(AMpassword);
}


void MainWindow::moveInScreen(QPoint pos, QSize size)
{
    QRect appRect(pos, size);
    QPoint midPoint = appRect.center();

    QString message = (isMainWindow?QString("TabsWindow: "):QString("DeckWindow: ")) +
            "Window Pos: (" + QString::number(pos.x()) + "," + QString::number(pos.y()) +
            ") - Size: (" + QString::number(size.width()) + "," + QString::number(size.height()) +
            ") - Mid: (" + QString::number(midPoint.x()) + "," + QString::number(midPoint.y()) + ")";
    pDebug(message);

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();

        if(geometry.contains(midPoint))
        {
            message = (isMainWindow?QString("TabsWindow: "):QString("DeckWindow: ")) +
                    "Window in screen: (" + QString::number(geometry.left()) + "," + QString::number(geometry.top()) + "," +
                    QString::number(geometry.right()) + "," + QString::number(geometry.bottom()) + ")";
            pDebug(message);
            move(pos);
            return;
        }
    }

    message = (isMainWindow?QString("TabsWindow: "):QString("DeckWindow: ")) +
            "Window outside screens. Move to (0,0)";
    pDebug(message);
    move(QPoint(0,0));
}


void MainWindow::readSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QPoint pos;
    QSize size;

    if(isMainWindow)
    {
        pos = settings.value("pos", QPoint(0,0)).toPoint();
        size = settings.value("size", QSize(255, 600)).toSize();

        this->splitWindow = settings.value("splitWindow", false).toBool();
        this->transparency = (Transparency)settings.value("transparent", AutoTransparent).toInt();
        this->theme = (Theme)settings.value("theme", ThemeBlack).toInt();
        spreadTheme();

        int numWindows = settings.value("numWindows", 2).toInt();
        if(numWindows == 2) createSecondaryWindow();

        int cardHeight = settings.value("cardHeight", 35).toInt();
        this->drawDisappear = settings.value("drawDisappear", 5).toInt();
        this->showDraftOverlay = settings.value("showDraftOverlay", true).toBool();
        this->draftLearningMode = settings.value("draftLearningMode", false).toBool();
        int tooltipScale = settings.value("tooltipScale", 10).toInt();
        bool autoSize = settings.value("autoSize", true).toBool();
        bool showClassColor = settings.value("showClassColor", true).toBool();
        bool showSpellColor = settings.value("showSpellColor", true).toBool();
        bool createGoldenCards = settings.value("createGoldenCards", false).toBool();
        int maxGamesLog = settings.value("maxGamesLog", 15).toInt();
        QString AMplayerEmail = settings.value("playerEmail", "").toString();
        QString AMpassword = settings.value("password", "").toString();

        initConfigTab(tooltipScale, cardHeight, autoSize, showClassColor, showSpellColor, createGoldenCards, maxGamesLog, AMplayerEmail, AMpassword);
    }
    else
    {
        pos = settings.value("pos2", QPoint(0,0)).toPoint();
        size = settings.value("size2", QSize(255, 600)).toSize();

        this->splitWindow = false;
        this->transparency = Transparent;
        this->theme = ThemeBlack;
    }
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->windowsFormation = None;
    this->show();
    this->setMinimumSize(100,200);  //El minimumSize inicial es incorrecto
    this->windowsFormation = None;
    resize(size);
    moveInScreen(pos, size);
}


void MainWindow::writeSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    if(isMainWindow)
    {
        settings.setValue("pos", pos());
        settings.setValue("size", size());
        settings.setValue("splitWindow", this->splitWindow);
        settings.setValue("transparent", (int)this->transparency);
        settings.setValue("theme", (int)this->theme);
        settings.setValue("numWindows", (this->otherWindow == NULL)?1:2);
        settings.setValue("cardHeight", ui->configSliderCardSize->value());
        settings.setValue("drawDisappear", this->drawDisappear);
        settings.setValue("showDraftOverlay", this->showDraftOverlay);
        settings.setValue("draftLearningMode", this->draftLearningMode);
        settings.setValue("tooltipScale", ui->configSliderTooltipSize->value());
        settings.setValue("autoSize", ui->configCheckAutoSize->isChecked());
        settings.setValue("showClassColor", ui->configCheckClassColor->isChecked());
        settings.setValue("showSpellColor", ui->configCheckSpellColor->isChecked());
        settings.setValue("createGoldenCards", ui->configCheckGoldenCards->isChecked());
        settings.setValue("maxGamesLog", ui->configSliderZero->value());
        settings.setValue("playerEmail", ui->configLineEditMastery->text());
        settings.setValue("password", ui->configLineEditMastery2->text());
    }
    else
    {
        settings.setValue("pos2", pos());
        settings.setValue("size2", size());
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);

    if(isMainWindow && (otherWindow != NULL))
    {
        otherWindow->close();
    }

    writeSettings();
    event->accept();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPosition = event->globalPos() - dragPosition;
        int top = newPosition.y();
        int bottom = top + this->height();
        int left = newPosition.x();
        int right = left + this->width();
        int midX = (left + right)/2;
        int midY = (top + bottom)/2;

        const int stickyMargin = 10;

        foreach (QScreen *screen, QGuiApplication::screens())
        {
            if (!screen)    continue;
            QRect screenRect = screen->geometry();
            int topScreen = screenRect.y();
            int bottomScreen = topScreen + screenRect.height();
            int leftScreen = screenRect.x();
            int rightScreen = leftScreen + screenRect.width();

            if(midX < leftScreen || midX > rightScreen ||
                    midY < topScreen || midY > bottomScreen) continue;

            if(std::abs(top - topScreen) < stickyMargin)
            {
                newPosition.setY(topScreen);
            }
            else if(std::abs(bottom - bottomScreen) < stickyMargin)
            {
                newPosition.setY(bottomScreen - this->height());
            }
            if(std::abs(left - leftScreen) < stickyMargin)
            {
                newPosition.setX(leftScreen);
            }
            else if(std::abs(right - rightScreen) < stickyMargin)
            {
                newPosition.setX(rightScreen - this->width());
            }
            move(newPosition);
            event->accept();
            return;
        }

        move(newPosition);
        event->accept();
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Control)
    {
        if(event->modifiers()&Qt::ControlModifier)
        {
            if(event->key() == Qt::Key_R)       resetSettings();
            else if(event->key() == Qt::Key_1)  draftHandler->pickCard("0");
            else if(event->key() == Qt::Key_2)  draftHandler->pickCard("1");
            else if(event->key() == Qt::Key_3)  draftHandler->pickCard("2");
        }
    }
}


//Restaura ambas ventanas minimizadas
void MainWindow::changeEvent(QEvent * event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        if((windowState() & Qt::WindowMinimized) == 0)
        {
            if(this->otherWindow != NULL)
            {
                this->otherWindow->setWindowState(Qt::WindowActive);
            }
        }

    }
}


void MainWindow::leaveEvent(QEvent * e)
{
    QMainWindow::leaveEvent(e);

    this->mouseInApp = false;
    spreadMouseInApp();

    if(arenaHandler != NULL)    arenaHandler->deselectRow();
}


void MainWindow::enterEvent(QEvent * e)
{
    QMainWindow::enterEvent(e);

    this->mouseInApp = true;
    spreadMouseInApp();
}


void MainWindow::spreadMouseInApp()
{
    if(!isMainWindow)   return;

    QWidget *currentTab = ui->tabWidget->currentWidget();

    if(currentTab == ui->tabDeck)           deckHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabEnemy)     enemyHandHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabPlan)      planHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabEnemyDeck) enemyDeckHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabArena)     arenaHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabDraft)     draftHandler->setMouseInApp(mouseInApp);
    else                                    updateOtherTabsTransparency();

    //Fade Bar
    if(transparency==Transparent)
    {
        if(mouseInApp)      fadeBarAndButtons(false);
        else                fadeBarAndButtons(true);
    }
    else if(transparency==AutoTransparent && currentTab != ui->tabDeck && currentTab != ui->tabEnemy && currentTab != ui->tabEnemyDeck)
    {
        fadeBarAndButtons(false);
    }

    //Split windows
    if(windowsFormation == H2 || windowsFormation == H3)
    {
        currentTab = ui->tabWidgetH2->currentWidget();

        if(currentTab == ui->tabDeck)           deckHandler->setMouseInApp(mouseInApp);
        else if(currentTab == ui->tabEnemy)     enemyHandHandler->setMouseInApp(mouseInApp);
        else if(currentTab == ui->tabEnemyDeck) enemyDeckHandler->setMouseInApp(mouseInApp);

        if(windowsFormation == H3)
        {
            currentTab = ui->tabWidgetH3->currentWidget();

            if(currentTab == ui->tabDeck)           deckHandler->setMouseInApp(mouseInApp);
            else if(currentTab == ui->tabEnemy)     enemyHandHandler->setMouseInApp(mouseInApp);
            else if(currentTab == ui->tabEnemyDeck) enemyDeckHandler->setMouseInApp(mouseInApp);
        }
    }
    else if(windowsFormation == V2)
    {
        currentTab = ui->tabWidgetV1->currentWidget();

        if(currentTab == ui->tabDeck)           deckHandler->setMouseInApp(mouseInApp);
        else if(currentTab == ui->tabEnemy)     enemyHandHandler->setMouseInApp(mouseInApp);
        else if(currentTab == ui->tabEnemyDeck) enemyDeckHandler->setMouseInApp(mouseInApp);
    }
}


void MainWindow::resizeSlot(QSize size)
{
    resize(size);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resizeChecks(this->size());
    event->accept();
}


void MainWindow::resizeChecks(QSize size)
{
    QWidget *widget = this->centralWidget();

    if(isMainWindow)
    {
        resizeTabWidgets(size);

        int top = widget->pos().y();
        int bottom = top + widget->height();
        int left = widget->pos().x();
        int right = left + widget->width();

        resizeTopButtons(right, top);
        ui->resizeButton->move(right-24, bottom-24);

        if(otherWindow == NULL) spreadCorrectTamCard();
    }
    else
    {
        int top = widget->pos().y();
        int bottom = top + widget->height();
        int left = widget->pos().x();
        int right = left + widget->width();

        ui->resizeButton->move(right-24, bottom-24);

        otherWindow->spreadCorrectTamCard();
    }
}


void MainWindow::resizeTopButtons(int right, int top)
{
    int limitWidth = ui->tabWidget->tabBar()->width() + 48;

    int buttonsWidth;
    bool smallButtons = this->width() < limitWidth;
    if(smallButtons)
    {
        buttonsWidth = 19;
        ui->closeButton->move(right-buttonsWidth, top);
        ui->minimizeButton->move(right-buttonsWidth, top+buttonsWidth);
    }
    else
    {
        buttonsWidth = 24;
        ui->closeButton->move(right-buttonsWidth, top);
        ui->minimizeButton->move(right-2*buttonsWidth, top);
    }

    ui->closeButton->resize(buttonsWidth, buttonsWidth);
    ui->closeButton->setIconSize(QSize(buttonsWidth, buttonsWidth));
    ui->minimizeButton->resize(buttonsWidth, buttonsWidth);
    ui->minimizeButton->setIconSize(QSize(buttonsWidth, buttonsWidth));
}


void MainWindow::resizeTabWidgets(QSize newSize)
{
    if(ui->tabWidget == NULL)   return;

    WindowsFormation newWindowsFormation;

    //H1
    if(newSize.width()<=DIVIDE_TABS_H)
    {
        if(newSize.height()>DIVIDE_TABS_V)  newWindowsFormation = V2;
        else                                newWindowsFormation = H1;
    }
    //H2
    else if(newSize.width()>DIVIDE_TABS_H && newSize.width()<=DIVIDE_TABS_H2)
    {
        newWindowsFormation = H2;
    }
    //H3
    else
    {
        newWindowsFormation = H3;
    }

    if(!this->splitWindow)  newWindowsFormation = H1;

    switch(windowsFormation)
    {
        case None:
        case H1:
            break;

        case H2:
            ui->tabWidgetH2->setFixedWidth(this->width()/2);
            break;

        case H3:
            ui->tabWidgetH2->setFixedWidth(this->width()/3);
            ui->tabWidgetH3->setFixedWidth(this->width()/3);
            break;

        case V2:
            ui->tabWidgetV1->setFixedHeight(this->height()/2);
            break;
    }

    if(newWindowsFormation != windowsFormation) resizeTabWidgets(newWindowsFormation);
}


void MainWindow::resizeTabWidgets(WindowsFormation newWindowsFormation)
{
    windowsFormation = newWindowsFormation;

    ui->tabWidget->hide();
    ui->tabWidgetH2->hide();
    ui->tabWidgetH3->hide();
    ui->tabWidgetV1->hide();

    QWidget * currentTab = ui->tabWidget->currentWidget();

    switch(windowsFormation)
    {
        case None:
        case H1:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabLog));
                ui->tabWidget->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabLog));
                ui->tabWidget->show();
            }
            break;

        case H2:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidgetH2);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabLog));
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
            }
            break;

        case H3:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidgetH2);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH3);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
                ui->tabWidgetH3->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH3);
                moveTabTo(ui->tabPlan, ui->tabWidgetH2);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
                ui->tabWidgetH3->show();
            }
            break;

        case V2:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidgetV1);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetV1->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetV1);
                moveTabTo(ui->tabPlan, ui->tabWidget);
                moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetV1->show();
            }
            break;
    }
    ui->tabWidget->setCurrentWidget(currentTab);

    this->calculateMinimumWidth();
}


void MainWindow::moveTabTo(QWidget *widget, QTabWidget *tabWidget)
{
    QIcon icon;
    if(widget == ui->tabArena)
    {
        icon = QIcon(":/Images/arena.png");
    }
    else if(widget == ui->tabDeck)
    {
        icon = QIcon(":/Images/deck.png");
    }
    else if(widget == ui->tabEnemy)
    {
        icon = QIcon(":/Images/hand.png");
    }
    else if(widget == ui->tabPlan)
    {
        icon = QIcon(":/Images/plan.png");
    }
    else if(widget == ui->tabEnemyDeck)
    {
        icon = QIcon(":/Images/enemyDeck.png");
    }
    else if(widget == ui->tabLog)
    {
        icon = QIcon(":/Images/log.png");
    }
    else if(widget == ui->tabConfig)
    {
        icon = QIcon(":/Images/config.png");
    }
    tabWidget->addTab(widget, icon, "");
}


void MainWindow::calculateMinimumWidth()
{
    if(isMainWindow)
    {
        int minWidth = ui->tabWidget->tabBar()->width() + 19;
        this->setMinimumWidth(minWidth);
    }
}


//Fija la anchura de la ventana de deck.
void MainWindow::calculateDeckWindowMinimumWidth()
{
    if(this->otherWindow!=NULL && deckHandler!= NULL)
    {
        int deckWidth = this->otherWindow->width() - ui->deckListWidget->width() + ui->deckListWidget->sizeHintForColumn(0);
        this->otherWindow->setFixedWidth(deckWidth);
    }
}


void MainWindow::removeNonCompleteDraft()
{
    //Remove old not-complete draft
    if(!draftLogFile.isEmpty())
    {
        //Check dir
        QFileInfo dirInfo(Utility::gameslogPath());
        if(!dirInfo.exists())
        {
            pDebug("Cannot remove non-complete draft Log. GamesLog dir doesn't exist.");
            return;
        }

        QDir dir(Utility::gameslogPath());
        dir.remove(draftLogFile);
        pDebug("Remove non-complete draft: " + draftLogFile);
        draftLogFile = "";
    }
}


void MainWindow::checkDraftLogLine(QString logLine, QString file)
{
    //New Draft
    if(file == "DraftHandler")
    {
        QRegularExpressionMatch match;
        if(logLine.contains(QRegularExpression("DraftHandler: Begin draft\\. Heroe: (\\d+)"), &match))
        {
            //Check dir
            QFileInfo dir(Utility::gameslogPath());
            if(!dir.exists())
            {
                pDebug("Cannot create draft Log. GamesLog dir doesn't exist.");
                return;
            }

            //Remove old not-complete draft
            removeNonCompleteDraft();

            QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh-mm");
            QString playerHero = Utility::heroStringFromLogNumber(match.captured(1));
            QString fileName = "DRAFT " + timeStamp + " " + playerHero + ".arenatracker";

            QFile logDraft(Utility::gameslogPath() + "/" + fileName);
            if(!logDraft.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                pDebug("Cannot create draft log file...", Error);
                pLog(tr("Log: ERROR:Cannot create draft log file..."));
                return;
            }

            QTextStream stream(&logDraft);
            stream << logLine << endl;
            logDraft.close();

            pDebug("Start DraftLog: " + fileName);
            draftLogFile = fileName;
            return;
        }
    }

    //Continue Draft
    bool copyLogLine = false;
    bool endDraftLog = false;

    if(!draftLogFile.isEmpty())
    {
        if(file == "DraftHandler")
        {
            if(logLine.contains("New codes"))
            {
                copyLogLine = true;
            }
            else if(logLine.contains("End draft"))
            {
                copyLogLine = true;
                endDraftLog = true;
            }
        }
        else if(file == "GameWatcher" && logLine.contains("Pick card"))
        {
            copyLogLine = true;
        }

        if(copyLogLine)
        {
            QFile logDraft(Utility::gameslogPath() + "/" + draftLogFile);
            if(!logDraft.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                pDebug("Cannot open draft log file...", Error);
                pLog(tr("Log: ERROR:Cannot open draft log file..."));
                return;
            }

            QTextStream stream(&logDraft);
            stream << logLine << endl;
            logDraft.close();
        }
        if(endDraftLog)
        {
            pDebug("End DraftLog: " + draftLogFile);
            if(arenaHandler != NULL)    arenaHandler->linkLogToDraft(draftLogFile);
            draftLogFile = "";
        }
    }
}


void MainWindow::pDebug(QString line, DebugLevel debugLevel, QString file)
{
    pDebug(line, 0, debugLevel, file);
}


void MainWindow::pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file)
{
    if(!isMainWindow)
    {
        this->otherWindow->pDebug(line, numLine, debugLevel, file);
        return;
    }

    (void)debugLevel;
    QString logLine = "";
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    if(line[0]==QChar('\n'))
    {
        line.remove(0, 1);
        logLine += '\n';
    }

    logLine += timeStamp + " - " + file;
    if(numLine > 0) logLine += "(" + QString::number(numLine) + ")";
    logLine += ": " + line;

    qDebug().noquote() << logLine;

    if(atLogFile != NULL)
    {
        QTextStream stream(atLogFile);
        stream << logLine << endl;
    }

    if(copyGameLogs)    checkDraftLogLine(logLine, file);
}


void MainWindow::pLog(QString line)
{
    if(isMainWindow)    ui->logTextEdit->append(line);
    else                this->otherWindow->pLog(line);
}


void MainWindow::logReset()
{
    deckHandler->unlockDeckInterface();
    deckHandler->leaveArena();
    enemyHandHandler->unlockEnemyInterface();
    gameWatcher->reset();
}


void MainWindow::checkCardImage(QString code, bool isHero)
{
    QFileInfo cardFile(Utility::hscardsPath() + "/" + code + ".png");

    if(!cardFile.exists())
    {
        //La bajamos de HearthHead
        cardDownloader->downloadWebImage(code, isHero);
    }
}


void MainWindow::redrawDownloadedCardImage(QString code)
{
    deckHandler->redrawDownloadedCardImage(code);
    enemyDeckHandler->redrawDownloadedCardImage(code);
    enemyHandHandler->redrawDownloadedCardImage(code);
    planHandler->redrawDownloadedCardImage(code);
    secretsHandler->redrawDownloadedCardImage(code);
    draftHandler->reHistDownloadedCardImage(code);
}


void MainWindow::resetSettings()
{
    int ret = QMessageBox::warning(0, tr("Reset settings"),
                                   tr("Do you want to reset Arena Tracker settings?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logsDirPath", "");
        settings.setValue("logConfig", "");
        settings.setValue("playerTag", "");
        settings.setValue("sizeDraft", QSize(255, 600));

        resize(QSize(255, 600));
        move(QPoint(0,0));

        if(otherWindow != NULL)
        {
            otherWindow->resize(QSize(255, 600));
            otherWindow->move(QPoint(0,0));
        }
        this->close();
    }
}


void MainWindow::createLogFile()
{
    atLogFile = new QFile(Utility::dataPath() + "/ArenaTrackerLog.txt");
    if(atLogFile->exists())  atLogFile->remove();
    if(!atLogFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        pDebug("Failed to create Arena Tracker log on disk.", Error);
        pLog(tr("File: ERROR: Failed to create Arena Tracker log on disk."));
        atLogFile = NULL;
    }
}


void MainWindow::closeLogFile()
{
    if(atLogFile == NULL)   return;
    atLogFile->close();
    delete atLogFile;
    atLogFile = NULL;
}


void MainWindow::uploadDeck()
{
    QList<DeckCard> *deckCardList = deckHandler->getDeckComplete();
    if(deckCardList != NULL)
    {
        gameWatcher->setDeckRead();
        if(webUploader != NULL)     webUploader->uploadDeck(deckCardList);
    }
    else
    {
        gameWatcher->setDeckRead(false);
    }
}


bool MainWindow::createDir(QString pathDir)
{
    QFileInfo dirInfo(pathDir);
    if(!dirInfo.exists())
    {
        QDir().mkdir(pathDir);
        pDebug(pathDir + " created.");
        return true;
    }
    return false;
}


void MainWindow::createDataDir()
{
    if(createDir(Utility::dataPath()))
    {
        //Rescue old decks json
        QFile file(Utility::appPath() + "/HSCards/ArenaTrackerDecks.json");
        if(file.exists())
        {
            file.copy(Utility::dataPath() + "/ArenaTrackerDecks.json");
            pDebug("ArenaTrackerDecks.json recovered.");
        }
    }
    createDir(Utility::hscardsPath());
    createDir(Utility::gameslogPath());
    if(createDir(Utility::extraPath()))
    {
        QFile file1(":Extra/deckBuilder.py");
        file1.copy(Utility::extraPath() + "/deckBuilder.py");

        QFile file2(":Extra/arenaTemplate.png");
        file2.copy(Utility::extraPath() + "/arenaTemplate.png");

        QFile file3(":Extra/collectionTemplate.png");
        file3.copy(Utility::extraPath() + "/collectionTemplate.png");

        pDebug("Extra files created.");
    }

    pDebug("Path Arena Tracker Dir: " + Utility::dataPath());
}


void MainWindow::checkGamesLogDir()
{
    QFileInfo dirInfo(Utility::gameslogPath());
    if(!dirInfo.exists())
    {
        pDebug("Cannot check GamesLog dir. Dir doesn't exist.");
        return;
    }

    int maxGamesLog = ui->configSliderZero->value();
    if(maxGamesLog == 100)
    {
        pDebug("GamesLog: Keep ALL.");
        return;
    }
    pDebug("GamesLog: Keep recent " + QString::number(maxGamesLog) + ".");

    QDir dir(Utility::gameslogPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QStringList filterName;
    filterName << "*.arenatracker";
    dir.setNameFilters(filterName);

    QStringList files = dir.entryList().mid(maxGamesLog);
    foreach(QString file, files)
    {
        dir.remove(file);
        pDebug(file + " removed.");
    }
}


void MainWindow::completeArenaDeck()
{
    if(arenaHandler == NULL)    return;

    QString arenaCurrentGameLog = arenaHandler->getArenaCurrentGameLog();
    if(arenaCurrentGameLog.isEmpty())
    {
        pDebug("Completing Arena Deck: No draft log.");
    }
    else
    {
        pDebug("Completing Arena Deck: " + arenaCurrentGameLog);

        if(deckHandler != NULL) deckHandler->completeArenaDeck(arenaCurrentGameLog);
    }
}


void MainWindow::test()
{
    planHandler->playerMinionZonePlayAdd("AT_003", 1, 1);
    planHandler->enemyMinionZonePlayAdd("AT_042t2", 2, 1);
    planHandler->playerMinionZonePlayAdd("CS1_042", 3, 1);
    planHandler->enemyMinionZonePlayAdd("EX1_020", 4, 1);
    planHandler->playerHeroZonePlayAdd("HERO_08", 11);
    planHandler->enemyHeroZonePlayAdd("HERO_09", 12);

    planHandler->newTurn(true, 1);
    planHandler->zonePlayAttack("AT_003",1,2);
    planHandler->zonePlayAttack("AT_003",3,2);
    planHandler->zonePlayAttack("AT_003",11,4);

    planHandler->newTurn(false, 2);
    planHandler->zonePlayAttack("AT_003",12,11);
    planHandler->zonePlayAttack("AT_003",12,11);
    planHandler->setLastTriggerId("", "FATIGUE", 0, 0);
    planHandler->playerMinionTagChange(11, "DAMAGE", "1");

//    QTimer::singleShot(10000, this, SLOT(test2()));
}

void MainWindow::test2()
{
//    planHandler->reset();
}


//Config Tab
void MainWindow::addDraftMenu(QPushButton *button)
{
    QMenu *newArenaMenu = new QMenu(button);

    QSignalMapper* mapper = new QSignalMapper(button);

    for(int i=0; i<9; i++)
    {
        QAction *action = newArenaMenu->addAction(QIcon(":/Images/hero" + Utility::getHeroLogNumber(i) + ".png"), Utility::getHeroName(i));
        mapper->setMapping(action, Utility::getHeroName(i));
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    }

    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));

    button->setMenu(newArenaMenu);
}


void MainWindow::confirmNewArenaDraft(QString hero)
{
    int ret = QMessageBox::question(this, tr("New arena: ") + hero,
                                   "Make sure you have already picked " + hero + " in hearthstone.\n"
                                   "You shouldn't move hearthstone window until the end of the draft.\n"
                                   "Do you want to continue?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        pDebug("Manual draft: " + hero);
        pLog(tr("Menu: Force draft: ") + hero);
        QString heroLog = Utility::heroToLogNumber(hero);
        draftHandler->beginDraft(heroLog);
    }
}


void MainWindow::toggleSplitWindow()
{
    this->splitWindow = !this->splitWindow;
    spreadSplitWindow();
}


void MainWindow::spreadSplitWindow()
{
    resizeTabWidgets(this->size());

    if(isMainWindow && otherWindow != NULL)
    {
        otherWindow->splitWindow = this->splitWindow;
    }
}


void MainWindow::transparentAlways()
{
    spreadTransparency(Transparent);
}


void MainWindow::transparentAuto()
{
    spreadTransparency(AutoTransparent);
}


void MainWindow::transparentNever()
{
    spreadTransparency(Opaque);
}


void MainWindow::spreadTransparency(Transparency newTransparency)
{
    this->transparency = newTransparency;

    deckHandler->setTransparency((this->otherWindow!=NULL)?Transparent:this->transparency);
    enemyDeckHandler->setTransparency(this->transparency);
    enemyHandHandler->setTransparency(this->transparency);
    planHandler->setTransparency(this->transparency);
    arenaHandler->setTransparency(this->transparency);
    draftHandler->setTransparency(this->transparency);
    updateOtherTabsTransparency();
}


//Update Config and Log tabs transparency
void MainWindow::updateOtherTabsTransparency()
{
    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabLog->setAttribute(Qt::WA_NoBackground);
        ui->tabLog->repaint();
        ui->tabConfig->setAttribute(Qt::WA_NoBackground);
        ui->tabConfig->repaint();

        QString groupBoxCSS =
                "QGroupBox {border: 2px solid #0F4F0F; border-radius: 5px; margin-top: 5px; background-color: transparent; color: white;}"
                "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}";
        ui->configBoxActions->setStyleSheet(groupBoxCSS);
        ui->configBoxUI->setStyleSheet(groupBoxCSS);
        ui->configBoxDeck->setStyleSheet(groupBoxCSS);
        ui->configBoxHand->setStyleSheet(groupBoxCSS);
        ui->configBoxDraft->setStyleSheet(groupBoxCSS);
        ui->configBoxMastery->setStyleSheet(groupBoxCSS);
        ui->configBoxZero->setStyleSheet(groupBoxCSS);

        QString labelCSS = "QLabel {background-color: transparent; color: white;}";
        ui->configLabelDeckNormal->setStyleSheet(labelCSS);
        ui->configLabelDeckNormal2->setStyleSheet(labelCSS);
        ui->configLabelDeckTooltip->setStyleSheet(labelCSS);
        ui->configLabelDeckTooltip2->setStyleSheet(labelCSS);
        ui->configLabelDrawTime->setStyleSheet(labelCSS);
        ui->configLabelDrawTimeValue->setStyleSheet(labelCSS);
        ui->configLabelMastery->setStyleSheet(labelCSS);
        ui->configLabelMastery2->setStyleSheet(labelCSS);
        ui->configLabelZero->setStyleSheet(labelCSS);
        ui->configLabelZero2->setStyleSheet(labelCSS);

        QString radioCSS = "QRadioButton {background-color: transparent; color: white;}";
        ui->configRadioTransparent->setStyleSheet(radioCSS);
        ui->configRadioAuto->setStyleSheet(radioCSS);
        ui->configRadioOpaque->setStyleSheet(radioCSS);

        QString checkCSS = "QCheckBox {background-color: transparent; color: white;}";
        ui->configCheckGoldenCards->setStyleSheet(checkCSS);
        ui->configCheckDarkTheme->setStyleSheet(checkCSS);
        ui->configCheckWindowSplit->setStyleSheet(checkCSS);
        ui->configCheckDeckWindow->setStyleSheet(checkCSS);
        ui->configCheckClassColor->setStyleSheet(checkCSS);
        ui->configCheckSpellColor->setStyleSheet(checkCSS);
        ui->configCheckOverlay->setStyleSheet(checkCSS);
        ui->configCheckLearning->setStyleSheet(checkCSS);

        ui->logTextEdit->setStyleSheet("QTextEdit{background-color: transparent; color: white;}");
    }
    else
    {
        ui->tabLog->setAttribute(Qt::WA_NoBackground, false);
        ui->tabLog->repaint();
        ui->tabConfig->setAttribute(Qt::WA_NoBackground, false);
        ui->tabConfig->repaint();

        ui->configBoxActions->setStyleSheet("");
        ui->configBoxUI->setStyleSheet("");
        ui->configBoxDeck->setStyleSheet("");
        ui->configBoxHand->setStyleSheet("");
        ui->configBoxDraft->setStyleSheet("");
        ui->configBoxMastery->setStyleSheet("");
        ui->configBoxZero->setStyleSheet("");

        ui->configLabelDeckNormal->setStyleSheet("");
        ui->configLabelDeckNormal2->setStyleSheet("");
        ui->configLabelDeckTooltip->setStyleSheet("");
        ui->configLabelDeckTooltip2->setStyleSheet("");
        ui->configLabelDrawTime->setStyleSheet("");
        ui->configLabelDrawTimeValue->setStyleSheet("");
        ui->configLabelMastery->setStyleSheet("");
        ui->configLabelMastery2->setStyleSheet("");
        ui->configLabelZero->setStyleSheet("");
        ui->configLabelZero2->setStyleSheet("");

        ui->configRadioTransparent->setStyleSheet("");
        ui->configRadioAuto->setStyleSheet("");
        ui->configRadioOpaque->setStyleSheet("");

        ui->configCheckGoldenCards->setStyleSheet("");
        ui->configCheckDarkTheme->setStyleSheet("");
        ui->configCheckWindowSplit->setStyleSheet("");
        ui->configCheckDeckWindow->setStyleSheet("");
        ui->configCheckClassColor->setStyleSheet("");
        ui->configCheckSpellColor->setStyleSheet("");
        ui->configCheckOverlay->setStyleSheet("");
        ui->configCheckLearning->setStyleSheet("");

        ui->logTextEdit->setStyleSheet("");
    }
}


void MainWindow::fadeBarAndButtons(bool fadeOut)
{
    if(fadeOut)
    {
        bool inTabEnemy = ui->tabWidget->currentWidget() == ui->tabEnemy;
        if(inTabEnemy && enemyHandHandler->isEmpty())
        {
            Utility::fadeInWidget(ui->tabWidget->tabBar());
            Utility::fadeInWidget(ui->tabWidgetH2->tabBar());
            Utility::fadeInWidget(ui->tabWidgetH3->tabBar());
        }
        else
        {
            Utility::fadeOutWidget(ui->tabWidget->tabBar());
            Utility::fadeOutWidget(ui->tabWidgetH2->tabBar());
            Utility::fadeOutWidget(ui->tabWidgetH3->tabBar());
        }
        Utility::fadeOutWidget(ui->minimizeButton);
        Utility::fadeOutWidget(ui->closeButton);
        Utility::fadeOutWidget(ui->resizeButton);
    }
    else
    {
        Utility::fadeInWidget(ui->tabWidget->tabBar());
        Utility::fadeInWidget(ui->tabWidgetH2->tabBar());
        Utility::fadeInWidget(ui->tabWidgetH3->tabBar());
        Utility::fadeInWidget(ui->minimizeButton);
        Utility::fadeInWidget(ui->closeButton);
        Utility::fadeInWidget(ui->resizeButton);
    }
}


void MainWindow::toggleTheme()
{
    if(this->theme == ThemeWhite)   this->theme = ThemeBlack;
    else                            this->theme = ThemeWhite;
    spreadTheme();
}


void MainWindow::spreadTheme()
{
    updateMainUITheme();
    arenaHandler->setTheme(this->theme);
    deckHandler->setTheme(this->theme);
}


void MainWindow::updateTabWidgetsTheme()
{
    ui->tabWidget->setTheme(this->theme, "left");
    ui->tabWidgetH2->setTheme(this->theme, "center");
    ui->tabWidgetH3->setTheme(this->theme, "center");
    ui->tabWidgetV1->setTheme(this->theme, "left");
}


void MainWindow::updateMainUITheme()
{
    updateTabWidgetsTheme();
    updateButtonsTheme();

    QString mainCSS = "";
    if(theme == ThemeWhite)
    {
        mainCSS +=
                "QGroupBox {border: 2px solid #0F4F0F; border-radius: 5px; margin-top: 5px; background-color: transparent; color: black;}"
                "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}"
                "QToolTip {border: 2px solid green; border-radius: 2px; color: green;}"
                "QTextEdit {background: transparent;}"
                "QLineEdit {border: 1px solid black;border-radius: 5px;background: white;}"
                ;
    }
    else
    {
        mainCSS +=
                "QMenu {background-color: #0F4F0F; color: white;}"
                "QMenu::item:selected {background-color: black; color: white;}"

                "QScrollBar:vertical {background-color: black; border: 2px solid green; width: 15px; margin: 15px 0px 15px 0px;}"
                "QScrollBar::handle:vertical {background: #0F4F0F; min-height: 20px;}"
                "QScrollBar::add-line:vertical {border: 2px solid green;background: #0F4F0F; height: 15px; subcontrol-position: bottom; subcontrol-origin: margin;}"
                "QScrollBar::sub-line:vertical {border: 2px solid green;background: #0F4F0F; height: 15px; subcontrol-position: top; subcontrol-origin: margin;}"
                "QScrollBar:up-arrow:vertical, QScrollBar::down-arrow:vertical {border: 2px solid black; width: 3px; height: 3px; background: green;}"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none;}"

                "QProgressBar {background-color: black;}"
                "QProgressBar::chunk {background-color: #0F4F0F;}"

                "QDialog {background: black;}"
                "QPushButton {background: #0F4F0F; color: white;}"
                "QToolTip {border: 2px solid green; border-radius: 2px; color: green;}"

                "QGroupBox {border: 2px solid #0F4F0F; border-radius: 5px; margin-top: 5px; background-color: transparent; color: white;}"
                "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}"
                "QLabel {background-color: transparent; color: white;}"
                "QTextBrowser {background-color: transparent; color: white;}"
                "QRadioButton {background-color: transparent; color: white;}"
                "QCheckBox {background-color: transparent; color: white;}"
                "QTextEdit{background-color: transparent; color: white;}"

                "QLineEdit {border: 2px solid rgb(50,175,50);border-radius: 5px;background: #0F4F0F; color: white;selection-background-color: black;}"
                ;
    }
    this->setStyleSheet(mainCSS);
    if(otherWindow!=NULL)   otherWindow->setStyleSheet(mainCSS);
}


void MainWindow::updateButtonsTheme()
{
    if(theme == ThemeWhite)
    {
        ui->closeButton->setStyleSheet("QPushButton {background: #F0F0F0; border: none;}"
                                       "QPushButton:hover {background: #0F4F0F;}");
        ui->minimizeButton->setStyleSheet("QPushButton {background: #F0F0F0; border: none;}"
                                          "QPushButton:hover {background: #0F4F0F;}");
    }
    else
    {
        ui->closeButton->setStyleSheet("QPushButton {background: black; border: none;}"
                                       "QPushButton:hover {background: #0F4F0F;}");
        ui->minimizeButton->setStyleSheet("QPushButton {background: black; border: none;}"
                                          "QPushButton:hover {background: #0F4F0F;}");
    }
}


void MainWindow::toggleDeckWindow()
{
    if(this->otherWindow == NULL)
    {
        createSecondaryWindow();
    }
    else
    {
        destroySecondaryWindow();
    }
    spreadCorrectTamCard();
}


int MainWindow::getAutoTamCard()
{
    int numCards = deckHandler->getNumCardRows();
    int deckHeight = ui->tabDeck->height();
    if(this->otherWindow == NULL)   deckHeight -= 40;

    if(numCards > 0)    return deckHeight/numCards;
    else                return -1;
}


int MainWindow::getTamCard()
{
    bool autoSize = ui->configCheckAutoSize->isChecked();

    int tamCardSlider = ui->configSliderCardSize->value();

    if(autoSize)
    {
        int tamCardAuto = getAutoTamCard();
        if(tamCardAuto == -1)   return tamCardSlider;
        else                    return std::min(tamCardAuto, tamCardSlider);
    }
    else
    {
        return tamCardSlider;
    }
}


void MainWindow::spreadTamCard(int value)
{
    if(value < ui->configSliderCardSize->minimum()) value = ui->configSliderCardSize->minimum();
    if(this->cardHeight == value) return;

    this->cardHeight = value;
    DeckCard::setCardHeight(value);

    if(deckHandler != NULL)
    {
        deckHandler->updateIconSize(value);
        deckHandler->redrawAllCards();
    }

    if(enemyDeckHandler != NULL)    enemyDeckHandler->redrawAllCards();
    if(secretsHandler != NULL)      secretsHandler->redrawAllCards();
    if(enemyHandHandler != NULL)    enemyHandHandler->redrawAllCards();

    if(draftHandler != NULL)
    {
        draftHandler->redrawAllCards();
        draftHandler->updateTamCard(value);
    }

    calculateDeckWindowMinimumWidth();
}


void MainWindow::spreadCorrectTamCard()
{
    spreadTamCard(getTamCard());
}


void MainWindow::updateTamCard(int value)
{
    spreadCorrectTamCard();

    QString labelText = QString::number(value) + " px";
    ui->configSliderCardSize->setToolTip(labelText);
    ui->configLabelDeckNormal2->setText(labelText);
}


void MainWindow::updateTooltipScale(int value)
{
    cardWindow->scale(value);

    QString labelText = "x"+QString::number(value/10.0);
    ui->configSliderTooltipSize->setToolTip(labelText);
    ui->configLabelDeckTooltip2->setText(labelText);
}


void MainWindow::updateShowClassColor(bool checked)
{
    DeckCard::setDrawClassColor(checked);
    deckHandler->redrawClassCards();
    secretsHandler->redrawClassCards();
    enemyHandHandler->redrawClassCards();
    draftHandler->redrawAllCards();
    enemyDeckHandler->redrawClassCards();
}


void MainWindow::updateShowSpellColor(bool checked)
{
    DeckCard::setDrawSpellWeaponColor(checked);
    deckHandler->redrawSpellWeaponCards();
    secretsHandler->redrawSpellWeaponCards();
    enemyHandHandler->redrawSpellWeaponCards();
    draftHandler->redrawAllCards();
    enemyDeckHandler->redrawSpellWeaponCards();
}


//Valores drawDisappear:
//  -1  No show
//  0   Turn
//  n   Ns
void MainWindow::updateTimeDraw(int value)
{
    //Slider            0  - Ns - 11
    //DrawDissapear     -1 - Ns - 0

    QString labelText;

    switch(value)
    {
        case 0:
            this->drawDisappear = -1;
            labelText = "No";
            break;
        case 11:
            this->drawDisappear = 0;
            labelText = "Turn";
            break;
        default:
            this->drawDisappear = value;
            labelText = QString::number(value) + "s";
            break;
    }

    ui->configLabelDrawTimeValue->setText(labelText);
    ui->configSliderDrawTime->setToolTip(labelText);

    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::toggleShowDraftOverlay()
{
    this->showDraftOverlay = !this->showDraftOverlay;
    draftHandler->setShowDraftOverlay(this->showDraftOverlay);
}


void MainWindow::toggleDraftLearningMode()
{
    this->draftLearningMode = !this->draftLearningMode;
    draftHandler->setLearningMode(this->draftLearningMode);
}


void MainWindow::updateMaxGamesLog(int value)
{
    if(value == 0)
    {
        copyGameLogs = false;
        gameWatcher->setCopyGameLogs(false);
    }
    else
    {
        copyGameLogs = true;
        gameWatcher->setCopyGameLogs(true);
    }

    QString labelText;
    if(value == 100)
    {
        labelText = "ALL";

    }
    else if(value == 0)
    {
        labelText = "OFF";
    }
    else
    {
        labelText = QString::number(ui->configSliderZero->value());
    }

    ui->configLabelZero2->setText(labelText);
    ui->configSliderZero->setToolTip(labelText);
}


void MainWindow::updateAMConnectButton(bool isConnected)
{
    if(isConnected) updateAMConnectButton(1);
    else            updateAMConnectButton(0);
}


void MainWindow::updateAMConnectButton(int value)
{
    switch(value)
    {
        case 0:
            ui->configButtonMastery->setIcon(QIcon(":/Images/lose.png"));
            ui->configButtonMastery->setEnabled(true);
            break;
        case 1:
            ui->configButtonMastery->setIcon(QIcon(":/Images/win.png"));
            ui->configButtonMastery->setEnabled(true);
            break;
        case 2:
            ui->configButtonMastery->setIcon(QIcon(":/Images/refresh.png"));
            ui->configButtonMastery->setEnabled(true);
            break;
    }
}


void MainWindow::tryConnectAM()
{
    if(webUploader == NULL) return;
    if(arenaHandler == NULL)return;
    if(ui->configLineEditMastery->text().isEmpty())     return;
    if(ui->configLineEditMastery2->text().isEmpty())    return;

    ui->configButtonMastery->setIcon(QIcon(":/Images/refresh.png"));
    ui->configButtonMastery->setEnabled(false);
    webUploader->tryConnect(ui->configLineEditMastery->text(), ui->configLineEditMastery2->text());
}


void MainWindow::completeConfigTab()
{
    //Cambiar en Designer margenes/spacing de nuevos configBox a 5-9-5-9/5
    //Actions
    addDraftMenu(ui->configButtonForceDraft);
    //connect en createDeckHandler

    //UI
    connect(ui->configRadioTransparent, SIGNAL(clicked()), this, SLOT(transparentAlways()));
    connect(ui->configRadioAuto, SIGNAL(clicked()), this, SLOT(transparentAuto()));
    connect(ui->configRadioOpaque, SIGNAL(clicked()), this, SLOT(transparentNever()));

    connect(ui->configCheckDarkTheme, SIGNAL(clicked()), this, SLOT(toggleTheme()));
    connect(ui->configCheckWindowSplit, SIGNAL(clicked()), this, SLOT(toggleSplitWindow()));
    connect(ui->configCheckDeckWindow, SIGNAL(clicked()), this, SLOT(toggleDeckWindow()));

    //Deck
    connect(ui->configSliderCardSize, SIGNAL(valueChanged(int)), this, SLOT(updateTamCard(int)));
    connect(ui->configSliderTooltipSize, SIGNAL(valueChanged(int)), this, SLOT(updateTooltipScale(int)));
    connect(ui->configCheckAutoSize, SIGNAL(clicked()), this, SLOT(spreadCorrectTamCard()));
    connect(ui->configCheckClassColor, SIGNAL(clicked(bool)), this, SLOT(updateShowClassColor(bool)));
    connect(ui->configCheckSpellColor, SIGNAL(clicked(bool)), this, SLOT(updateShowSpellColor(bool)));

    //Hand
    connect(ui->configSliderDrawTime, SIGNAL(valueChanged(int)), this, SLOT(updateTimeDraw(int)));

    //Draft
    connect(ui->configCheckOverlay, SIGNAL(clicked()), this, SLOT(toggleShowDraftOverlay()));
    connect(ui->configCheckLearning, SIGNAL(clicked()), this, SLOT(toggleDraftLearningMode()));

    //Zero To Heroes
    connect(ui->configSliderZero, SIGNAL(valueChanged(int)), this, SLOT(updateMaxGamesLog(int)));

    //Arena Mastery
    connect(ui->configLineEditMastery, SIGNAL(textChanged(QString)), this, SLOT(updateAMConnectButton()));
    connect(ui->configLineEditMastery, SIGNAL(returnPressed()), this, SLOT(tryConnectAM()));

    connect(ui->configLineEditMastery2, SIGNAL(textChanged(QString)), this, SLOT(updateAMConnectButton()));
    connect(ui->configLineEditMastery2, SIGNAL(returnPressed()), this, SLOT(tryConnectAM()));

    connect(ui->configButtonMastery, SIGNAL(clicked()), this, SLOT(tryConnectAM()));


    completeHighResConfigTab();
}


void MainWindow::completeHighResConfigTab()
{
    int screenHeight = getScreenHighest();
    if(screenHeight < 1000) return;

    int maxCard = (int)(screenHeight/1000.0*50);
    maxCard -= maxCard%5;
    ui->configSliderCardSize->setMaximum(maxCard);

    int maxTooltip = (int)(screenHeight/1000.0*15);
    maxTooltip -= maxTooltip%5;
    ui->configSliderTooltipSize->setMaximum(maxTooltip);
}


int MainWindow::getScreenHighest()
{
    int height = 0;

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();
        if(geometry.height()>height)    height = geometry.height();
    }
    return height;
}


LoadingScreenState MainWindow::getLoadingScreen()
{
    if(gameWatcher != NULL) return gameWatcher->getLoadingScreen();
    else                    return menu;
}


//TODO
//Hide Track secrets, avoid draft
//Windfury and tag NUM_ATTACKS_THIS_TURN
//Addon colores
//Jugar secreto
//No addons para atk/health si en la zona hay auras, dog, stormwind TAG CHANGE --> 25 "AURA" "1"
//Recuperar reinforcement en different board creados por spell con obj
//Numero stack

//Minions generados
//D 12:32:49.4409880 PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=[name=Recluta Mano de Plata id=97 zone=PLAY zonePos=6 cardId=CS2_101t player=1] tag=ZONE_POSITION value=7
//D 12:32:49.4423580 PowerTaskList.DebugPrintPower() -     FULL_ENTITY - Updating [name=Explorador múrloc id=98 zone=PLAY zonePos=6 cardId=EX1_506a player=1] CardID=EX1_506a
//D 12:32:49.4437560 PowerTaskList.DebugPrintPower() -         tag=EXHAUSTED value=1
//D 12:32:49.4447720 PowerTaskList.DebugPrintPower() -         tag=HEALTH value=1
//D 12:32:49.4457920 PowerTaskList.DebugPrintPower() -         tag=ATK value=1
//D 12:32:49.4466890 PowerTaskList.DebugPrintPower() -         tag=ZONE value=PLAY
//D 12:32:49.4476200 PowerTaskList.DebugPrintPower() -         tag=CONTROLLER value=1
//D 12:32:49.4486250 PowerTaskList.DebugPrintPower() -         tag=ENTITY_ID value=98
//D 12:32:49.4496270 PowerTaskList.DebugPrintPower() -         tag=FACTION value=NEUTRAL
//D 12:32:49.4505810 PowerTaskList.DebugPrintPower() -         tag=CARDTYPE value=MINION
//D 12:32:49.4515220 PowerTaskList.DebugPrintPower() -         tag=RARITY value=COMMON
//D 12:32:49.4524630 PowerTaskList.DebugPrintPower() -         tag=ZONE_POSITION value=6
//D 12:32:49.4533940 PowerTaskList.DebugPrintPower() -         tag=CREATOR value=4
//D 12:32:49.4542560 PowerTaskList.DebugPrintPower() - BLOCK_END

//Cambio Heroe (Zone)
//D 12:57:15.3458590 ZoneChangeList.ProcessChanges() - processing index=0 change=powerTask=[power=[type=TAG_CHANGE entity=
//[id=88 cardId=BRM_027h name=Ragnaros, Señor del Fuego] tag=LINKED_ENTITY value=64] complete=False]
//entity=[name=Ragnaros, Señor del Fuego id=88 zone=PLAY zonePos=0 cardId=BRM_027h player=1] srcZoneTag=INVALID srcPos= dstZoneTag=INVALID dstPos=


//SPECTATOR GAMES
//Si empiezan desde el principio todo correcto.
//Si empiezan a medias faltara: name1, name2, playerTag, firstPlayer
//  Problemas: En PlanHandler el heroe muerto puede ser incorrecto.

//BUGS CONOCIDOS
//Tab Config ScrollArea slider transparent CSS
//Cazar crash bug en drafting con 31 cartas

//REWARDS
//Despues de cada newGameResult se carga checkArenaCurrentReload que si ha terminado la arena enviara un showNoArena a ArenaHandler.
//Esto reinicia las variables y si luego subimos los rewards va a fallar porque no hay arenaCurrent.
//Para reactivar los rewards habra que arreglar esto. Reactivar paso de gameResultSent a reloadArenaCurrent en replyFinished
//Descomentar en resizeTabWidgets (adjustToContents)

//NUEVAS CARTAS
//Update Json cartas
//Update Json heroes HA (Utility::heroToLogNumber) para saber el numero de cada heroe
//Update secrets

//NUEVOS HEROES
//Evitar Asset hero powers (GameWatcher 201)
//Incluir nuevo hero power en isHeroPower(QString code) de GameWatcher
//Nuevo Json hearthArena
//Nuevo start draft menu

//NUEVOS BACKGROUND
//Coger el color de una parte clara de un carta de clase
//Colores->Colorear...(4 opcion por abajo)
//Colores->Tono y saturacion...(2 opcion) Luminosidad +50
