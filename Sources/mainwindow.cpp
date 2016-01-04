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

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)
    hstatsUploader = NULL;
    atLogFile = NULL;
    isMainWindow = true;
    otherWindow = NULL;

    createLogFile();
    completeUI();
    checkHSCardsDir();

    createCardDownloader();
    createDeckHandler();
    createDraftHandler();
    createEnemyHandHandler();
    createSecretsHandler();
    createArenaHandler();
    createGameWatcher();
    createLogLoader();
    createCardWindow();

    readSettings();
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
    hstatsUploader = NULL;
    cardDownloader = NULL;
    enemyHandHandler = NULL;
    draftHandler = NULL;
    deckHandler = NULL;
    secretsHandler = NULL;
    isMainWindow = false;
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

    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    this->windowsFormation = None;
    resizeTabWidgets(event);

    connect(ui->minimizeButton, SIGNAL(clicked()),
            this->otherWindow, SLOT(showMinimized()));
}


void MainWindow::destroySecondaryWindow()
{
    disconnect(ui->minimizeButton, 0, this->otherWindow, 0);
    this->otherWindow->close();
    this->otherWindow = NULL;
    deckHandler->setTransparency(this->transparency);

    ui->tabDeckLayout->setContentsMargins(0, 40, 0, 9);
    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    this->windowsFormation = None;
    resizeTabWidgets(event);
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
    QDir dir(QFileInfo(logLoader->getLogConfigPath()).absolutePath() + "/Cache/UberText");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QString lang;

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
        QDir dirHSCards(Utility::appPath() + "/HSCards");
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


    if(lang != "enGB" && lang != "enUS" && lang != "esES" && lang != "esMX" &&
            lang != "deDE" && lang != "frFR" && lang != "itIT" &&
            lang != "plPL" && lang != "ptBR" && lang != "ruRU" &&
            lang != "koKR" && lang != "zhCN" && lang != "zhTW" && lang != "jaJP")
    {
        pDebug("Language: " + lang + " not supported. Using enUS.");
        pLog("Settings: Language " + lang + " not supported. Using enUS.");
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
    QFile jsonFile(":Json/AllSets." + lang + ".json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonAllSets = jsonDoc.object();
    for(QJsonObject::const_iterator it=jsonAllSets.constBegin(); it!=jsonAllSets.constEnd(); it++)
    {
        QJsonArray array = it.value().toArray();
        for(QJsonArray::const_iterator it2=array.constBegin(); it2!=array.constEnd(); it2++)
        {
            cardsJson[(*it2).toObject().value("id").toString()] = (*it2).toObject();
        }
    }
}


void MainWindow::initCardsJson()
{
    QString lang = getHSLanguage();
    createCardsJsonMap(this->cardsJson, lang);
    DeckCard::setCardsJson(&cardsJson);
    GameWatcher::setCardsJson(&cardsJson);


    if(lang == "enUS")
    {
        Utility::setCardsJson(&cardsJson);
    }
    else
    {
        createCardsJsonMap(enCardsJson, "enUS");
        Utility::setCardsJson(&enCardsJson);
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
    draftHandler = new DraftHandler(this, &cardsJson, ui);
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


void MainWindow::createDeckHandler()
{
    deckHandler = new DeckHandler(this, &cardsJson, ui);
    connect(deckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(deckHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(deckHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

    deckHandler->loadDecks();
}


void MainWindow::createEnemyHandHandler()
{
    enemyHandHandler = new EnemyHandHandler(this, ui);
    connect(enemyHandHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyHandHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(enemyHandHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createCardWindow()
{
    cardWindow = new CardWindow(this);
    connect(deckHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(enemyHandHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(secretsHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(draftHandler, SIGNAL(overlayCardEntered(QString, QRect, int, int, bool)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int, bool)));

    connect(ui->deckListWidget, SIGNAL(leave()),
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

    connect(gameWatcher, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(gameWatcher, SIGNAL(pDebug(QString,qint64,DebugLevel,QString)),
            this, SLOT(pDebug(QString,qint64,DebugLevel,QString)));

    connect(gameWatcher, SIGNAL(newGameResult(GameResult, LoadingScreen)),
            arenaHandler, SLOT(newGameResult(GameResult, LoadingScreen)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            arenaHandler, SLOT(newArena(QString)));
    connect(gameWatcher, SIGNAL(inRewards()),
            arenaHandler, SLOT(showRewards()));

    connect(gameWatcher, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCardAsset(QString)));
    connect(gameWatcher, SIGNAL(playerCardDraw(QString)),
            deckHandler, SLOT(showPlayerCardDraw(QString)));
    connect(gameWatcher, SIGNAL(playerTurnStart()),
            deckHandler, SLOT(clearDrawList()));
    connect(gameWatcher, SIGNAL(startGame()),
            deckHandler, SLOT(lockDeckInterface()));
    connect(gameWatcher, SIGNAL(endGame()),
            deckHandler, SLOT(unlockDeckInterface()));
    connect(gameWatcher, SIGNAL(enterArena()),
            deckHandler, SLOT(enterArena()));
    connect(gameWatcher, SIGNAL(leaveArena()),
            deckHandler, SLOT(leaveArena()));

    connect(gameWatcher, SIGNAL(enemyCardDraw(int,int,bool,QString)),
            enemyHandHandler, SLOT(showEnemyCardDraw(int,int,bool,QString)));
    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString)),
            enemyHandHandler, SLOT(hideEnemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(lastHandCardIsCoin()),
            enemyHandHandler, SLOT(lastHandCardIsCoin()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString)),
            enemyHandHandler, SLOT(setLastCreatedByCode(QString)));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyHandHandler, SLOT(lockEnemyInterface()));
    connect(gameWatcher, SIGNAL(endGame()),
            enemyHandHandler, SLOT(unlockEnemyInterface()));

    connect(gameWatcher, SIGNAL(endGame()),
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
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString)),
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
    connect(logLoader, SIGNAL(seekChanged(qint64)),
            this, SLOT(showLogLoadProgress(qint64)));
    connect(logLoader, SIGNAL(newLogLineRead(QString,qint64)),
            gameWatcher, SLOT(processLogLine(QString,qint64)));
    connect(logLoader, SIGNAL(logConfigSet()),
            this, SLOT(initCardsJson()));
    connect(logLoader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(logLoader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

    //Connect de draftHandler
    connect(draftHandler, SIGNAL(draftEnded()),
            logLoader, SLOT(setUpdateTimeMax()));
    connect(draftHandler, SIGNAL(draftStarted()),
            logLoader, SLOT(setUpdateTimeMin()));

    qint64 logSize;
    logLoader->init(logSize);

    ui->progressBar->setMaximum(logSize/1000);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
}


void MainWindow::synchronizedDone()
{
    createWebUploader();
    gameWatcher->setSynchronized();
    secretsHandler->setSynchronized();
    deckHandler->setSynchronized();
    spreadTransparency();

    ui->progressBar->setVisible(false);

    //Connections after synchronized
    connect(gameWatcher,SIGNAL(newArena(QString)),
            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            this, SLOT(resetDeckDontRead()));

    //Test
    QTimer::singleShot(5000, this, SLOT(test()));
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this);
    connect(webUploader, SIGNAL(loadedGameResult(GameResult, LoadingScreen)),
            arenaHandler, SLOT(showGameResult(GameResult, LoadingScreen)));
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
    connect(webUploader, SIGNAL(loadArenaCurrentFinished()),
            arenaHandler, SLOT(removeDuplicateArena()));
    connect(webUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(webUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));


    arenaHandler->setWebUploader(webUploader);
    tryConnectAM();
}


void MainWindow::createHStatsUploader()
{
    if(hstatsUploader != NULL)  return;
    hstatsUploader = new HearthstatsUploader(this);

    connect(hstatsUploader, SIGNAL(connectionTried(bool)),
            this, SLOT(updateHStatsConnectButton(bool)));
    connect(hstatsUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(hstatsUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

//    arenaHandler->setWebUploader(webUploader);
    tryConnectHStats();
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

        ui->arenaTreeWidget = new MoveTreeWidget(ui->tabArena);
        ui->tabArenalLayout->insertWidget(1, ui->arenaTreeWidget);
        ui->enemyHandListWidget = new MoveListWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->enemyHandListWidget);
        ui->secretsTreeWidget = new MoveTreeWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->secretsTreeWidget);
        ui->drawListWidget = new MoveListWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->drawListWidget);
        ui->deckListWidget = new MoveListWidget(ui->tabDeck);
        ui->tabDeckLayout->insertWidget(0, ui->deckListWidget);

        completeHeroButtons();
        completeConfigTab();


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
        ui->closeButton->resize(24, 24);
        ui->closeButton->setIconSize(QSize(24, 24));
        ui->closeButton->setIcon(QIcon(":/Images/close.png"));
        ui->closeButton->setFlat(true);
        connect(ui->closeButton, SIGNAL(clicked()),
                this, SLOT(close()));


        ui->minimizeButton = new QPushButton("", this);
        ui->minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->minimizeButton->resize(24, 24);
        ui->minimizeButton->setIconSize(QSize(24, 24));
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


void MainWindow::completeHeroButtons()
{
    QSignalMapper* mapper = new QSignalMapper(this);
    QString heroes[9] = {"Warrior", "Shaman", "Rogue",
                         "Paladin", "Hunter", "Druid",
                         "Warlock", "Mage", "Priest"};
    QPushButton *heroButtons[9] = {ui->heroButton1, ui->heroButton2, ui->heroButton3,
                                  ui->heroButton4, ui->heroButton5, ui->heroButton6,
                                  ui->heroButton7, ui->heroButton8, ui->heroButton9};

    for(int i=0; i<9; i++)
    {
        mapper->setMapping(heroButtons[i], heroes[i]);
        connect(heroButtons[i], SIGNAL(clicked()), mapper, SLOT(map()));
        heroButtons[i]->setToolTip(heroes[i]);
    }

    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));
}


void MainWindow::initConfigTab(int tooltipScale, QString AMplayerEmail, QString AMpassword,
                               QString HStatsPlayerEmail, QString HStatsPassword)
{
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
    if(this->cardHeight<ui->configSliderCardSize->minimum() || this->cardHeight>ui->configSliderCardSize->maximum())  this->cardHeight = 35;
    if(ui->configSliderCardSize->value() == this->cardHeight)   updateTamCard(this->cardHeight);
    else    ui->configSliderCardSize->setValue(this->cardHeight);

    if(tooltipScale<ui->configSliderTooltipSize->minimum() || tooltipScale>ui->configSliderTooltipSize->maximum())  tooltipScale = 10;
    if(ui->configSliderTooltipSize->value() == tooltipScale) updateTooltipScale(tooltipScale);
    else ui->configSliderTooltipSize->setValue(tooltipScale);

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

    //Arena Mastery
    ui->configLineEditMastery->setText(AMplayerEmail);
    ui->configLineEditMastery2->setText(AMpassword);

    //Hearth Stats
    ui->configLineEditHStats->setText(HStatsPlayerEmail);
    ui->configLineEditHStats2->setText(HStatsPassword);
}


void MainWindow::moveInScreen(QPoint pos, QSize size)
{
    QPoint points[4];
    points[0] = pos + QPoint(5,5);
    points[1] = pos + QPoint(-5 + size.width(), 5);
    points[2] = pos + QPoint(5, -5 + size.height());
    points[3] = pos + QPoint(-5 + size.width(), -5 + size.height());

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();

        for(int i=0; i<4; i++)
        {
            if(geometry.contains(points[i]))
            {
                move(pos);
                return;
            }
        }
    }

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
        size = settings.value("size", QSize(400, 400)).toSize();

        this->splitWindow = settings.value("splitWindow", false).toBool();
        this->transparency = (Transparency)settings.value("transparent", AutoTransparent).toInt();
        this->theme = (Theme)settings.value("theme", ThemeBlack).toInt();
        spreadTheme();

        int numWindows = settings.value("numWindows", 2).toInt();
        if(numWindows == 2) createSecondaryWindow();

        this->greyedHeight = settings.value("greyedHeight", 35).toInt();
        this->cardHeight = settings.value("cardHeight", 35).toInt();
        this->drawDisappear = settings.value("drawDisappear", 5).toInt();
        this->showDraftOverlay = settings.value("showDraftOverlay", true).toBool();
        this->draftLearningMode = settings.value("draftLearningMode", false).toBool();
        int tooltipScale = settings.value("tooltipScale", 10).toInt();
        QString AMplayerEmail = settings.value("playerEmail", "").toString();
        QString AMpassword = settings.value("password", "").toString();
        QString HStatsPlayerEmail = "";//settings.value("HStatsPlayerEmail", "").toString();
        QString HStatsPassword = "";//settings.value("HStatsPassword", "").toString();

        initConfigTab(tooltipScale, AMplayerEmail, AMpassword, HStatsPlayerEmail, HStatsPassword);
    }
    else
    {
        pos = settings.value("pos2", QPoint(0,0)).toPoint();
        size = settings.value("size2", QSize(400, 400)).toSize();

        this->splitWindow = false;
        this->transparency = Transparent;
        this->theme = ThemeBlack;
    }
    this->setAttribute(Qt::WA_TranslucentBackground, true);
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
        settings.setValue("greyedHeight", this->greyedHeight);
        settings.setValue("cardHeight", this->cardHeight);
        settings.setValue("drawDisappear", this->drawDisappear);
        settings.setValue("showDraftOverlay", this->showDraftOverlay);
        settings.setValue("draftLearningMode", this->draftLearningMode);
        settings.setValue("tooltipScale", ui->configSliderTooltipSize->value());
        settings.setValue("playerEmail", ui->configLineEditMastery->text());
        settings.setValue("password", ui->configLineEditMastery2->text());
//        settings.setValue("HStatsPlayerEmail", ui->configLineEditHStats->text());
//        settings.setValue("HStatsPassword", ui->configLineEditHStats2->text());
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
            if(event->key() == Qt::Key_R)   resetSettings();
            else if(event->key() == Qt::Key_1)       draftHandler->pickCard("0");
            else if(event->key() == Qt::Key_2)  draftHandler->pickCard("1");
            else if(event->key() == Qt::Key_3)  draftHandler->pickCard("2");
            else if(event->key() == Qt::Key_L)  deckHandler->loadDeck("testDeck");
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


void MainWindow::resizeSlot(QSize size)
{
    resize(size);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QWidget *widget = this->centralWidget();

    if(isMainWindow)
    {
        resizeTabWidgets(event);

        int top = widget->pos().y();
        int bottom = top + widget->height();
        int left = widget->pos().x();
        int right = left + widget->width();

        ui->closeButton->move(right-24, top);
        ui->minimizeButton->move(right-48, top);
        ui->resizeButton->move(right-24, bottom-24);
    }
    else
    {
        int top = widget->pos().y();
        int bottom = top + widget->height();
        int left = widget->pos().x();
        int right = left + widget->width();

        ui->resizeButton->move(right-24, bottom-24);
    }

    event->accept();
}


void MainWindow::resizeTabWidgets(QResizeEvent *event)
{
    if(ui->tabWidget == NULL)   return;

    QSize newSize = event->size();
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

    switch(newWindowsFormation)
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

    if(newWindowsFormation == windowsFormation) return;
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
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
            }
            break;

        case H2:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidgetH2);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
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
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
                ui->tabWidgetH3->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
                moveTabTo(ui->tabConfig, ui->tabWidgetH3);
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
                moveTabTo(ui->tabConfig, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetV1->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetV1);
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
    if(!isMainWindow || (windowsFormation!=H1 && windowsFormation!=V2)) return;

    int minWidth = this->width() - ui->tabWidget->width();
    minWidth += ui->tabWidget->tabBar()->width();
    if(ui->minimizeButton!=NULL)    minWidth += ui->minimizeButton->width();
    if(ui->closeButton!=NULL)       minWidth += ui->closeButton->width();

    this->setMinimumWidth(minWidth);
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


void MainWindow::pDebug(QString line, DebugLevel debugLevel, QString file)
{
    pDebug(line, 0, debugLevel, file);
}


void MainWindow::pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file)
{
    (void)debugLevel;
    QString logLine;
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    if(numLine > 0)
    {
        file += "(" + QString::number(numLine) + ")";
    }
    if(line[0]==QChar('\n'))
    {
        line.remove(0, 1);
        logLine = '\n' + timeStamp + " - " + file + ": " + line;
    }
    else
    {
        logLine = timeStamp + " - " + file + ": " + line;
    }

    qDebug().noquote() << logLine;

    if(atLogFile != NULL)
    {
        QTextStream stream(atLogFile);
        stream << logLine << endl;
    }
}


void MainWindow::pLog(QString line)
{
    if(isMainWindow)    ui->logTextEdit->append(line);
    else                this->otherWindow->pLog(line);
}


void MainWindow::showLogLoadProgress(qint64 logSeek)
{
    if(logSeek == 0)     //Log reset
    {
        deckHandler->unlockDeckInterface();
        enemyHandHandler->unlockEnemyInterface();
        gameWatcher->reset();
    }
    ui->progressBar->setValue(logSeek/1000);
}


void MainWindow::checkCardImage(QString code)
{
    QFileInfo cardFile(Utility::appPath() + "/HSCards/" + code + ".png");

    if(!cardFile.exists())
    {
        //La bajamos de HearthHead
        cardDownloader->downloadWebImage(code);
    }
}


void MainWindow::redrawDownloadedCardImage(QString code)
{
    deckHandler->redrawDownloadedCardImage(code);
    enemyHandHandler->redrawDownloadedCardImage(code);
    secretsHandler->redrawDownloadedCardImage(code);
    draftHandler->reHistDownloadedCardImage(code);
}


void MainWindow::resetSettings()
{
    int ret = QMessageBox::warning(this, tr("Reset settings"),
                                   tr("Do you want to reset Arena Tracker settings?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logPath", "");
        settings.setValue("logConfig", "");
        settings.setValue("playerTag", "");
        settings.setValue("playerEmail", "");
        settings.setValue("password", "");
        settings.setValue("sizeDraft", QSize(350, 400));

        resize(QSize(400, 400));
        move(QPoint(0,0));

        if(otherWindow != NULL)
        {
            otherWindow->resize(QSize(400, 400));
            otherWindow->move(QPoint(0,0));
        }
        this->close();
    }
}


void MainWindow::createLogFile()
{
    atLogFile = new QFile(Utility::appPath() + "/HSCards/ArenaTrackerLog.txt");
    if(atLogFile->exists())  atLogFile->remove();
    if(!atLogFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        pDebug("Failed to create Arena Tracker log on disk.", Error);
        pLog(tr("File: ERROR: Creating Arena Tracker log on disk. Make sure HSCards dir is in the same place as the exe."));
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


void MainWindow::checkHSCardsDir()
{
    QFileInfo dir(Utility::appPath() + "/HSCards");

    if(dir.exists())
    {
        if(dir.isDir())
        {
            pLog("Settings: Path HSCards: " + dir.absoluteFilePath());
            pDebug("Path HSCards: " + dir.absoluteFilePath());
        }
        else
        {
            pLog("Settings: " + dir.absoluteFilePath() + " is not a directory.");
            pDebug(dir.absoluteFilePath() + " is not a directory.");
        }
    }
    else
    {
        pLog("Settings: HSCards dir not found on " + dir.absoluteFilePath() + " Move the directory there.");
        pDebug("HSCards dir not found on " + dir.absoluteFilePath() + " Move the directory there.");
        QMessageBox::warning(this, tr("HSCards not found"),
                             "HSCards dir not found on:\n" +
                             dir.absoluteFilePath() +
                             "\nMove the directory there.");
    }
}


void MainWindow::test()
{
}


//Config Tab
void MainWindow::addDraftMenu(QPushButton *button)
{
    QMenu *newArenaMenu = new QMenu(button);

    QSignalMapper* mapper = new QSignalMapper(button);
    QString heroes[9] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    QString heroesLogNumber[9] = {"06", "05", "08", "04", "09", "03", "02", "07", "01"};

    for(int i=0; i<9; i++)
    {
        QAction *action = newArenaMenu->addAction(QIcon(":/Images/hero" + heroesLogNumber[i] + ".png"), heroes[i]);
        mapper->setMapping(action, heroes[i]);
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
//        arenaHandler->newArena(heroLog);
        draftHandler->beginDraft(heroLog);
    }
}


void MainWindow::confirmClearDeck()
{
    int ret = QMessageBox::question(this, tr("Clear Deck"),
                                   "Do you want to clear your deck?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        pDebug("Manual clear deck");
        pLog(tr("Menu: Clear deck"));
        resetDeck();
    }
}


void MainWindow::toggleSplitWindow()
{
    this->splitWindow = !this->splitWindow;
    spreadSplitWindow();
}


void MainWindow::spreadSplitWindow()
{
    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    resizeTabWidgets(event);

    if(isMainWindow && otherWindow != NULL)
    {
        otherWindow->splitWindow = this->splitWindow;
    }
}


void MainWindow::transparentAlways()
{
    this->transparency = Transparent;
    spreadTransparency();
}


void MainWindow::transparentAuto()
{
    this->transparency = AutoTransparent;
    spreadTransparency();
}


void MainWindow::transparentNever()
{
    this->transparency = Opaque;
    spreadTransparency();
}


void MainWindow::spreadTransparency()
{
    deckHandler->setTransparency((this->otherWindow!=NULL)?Transparent:this->transparency);
    enemyHandHandler->setTransparency(this->transparency);
    arenaHandler->setTransparency(this->transparency);
    draftHandler->setTransparency(this->transparency);
    updateOtherTabsTransparency();
}


//Update Config and Log tabs transparency
void MainWindow::updateOtherTabsTransparency()
{
    if(transparency==Transparent)
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

        QString labelCSS = "QLabel {background-color: transparent; color: white;}";
        ui->configLabelDeckNormal->setStyleSheet(labelCSS);
        ui->configLabelDrawTime->setStyleSheet(labelCSS);
        ui->configLabelDrawTimeValue->setStyleSheet(labelCSS);

        QString radioCSS = "QRadioButton {background-color: transparent; color: white;}";
        ui->configRadioTransparent->setStyleSheet(radioCSS);
        ui->configRadioAuto->setStyleSheet(radioCSS);
        ui->configRadioOpaque->setStyleSheet(radioCSS);

        QString checkCSS = "QCheckBox {background-color: transparent; color: white;}";
        ui->configCheckDarkTheme->setStyleSheet(checkCSS);
        ui->configCheckWindowSplit->setStyleSheet(checkCSS);
        ui->configCheckDeckWindow->setStyleSheet(checkCSS);
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

        ui->configLabelDeckNormal->setStyleSheet("");
        ui->configLabelDrawTime->setStyleSheet("");
        ui->configLabelDrawTimeValue->setStyleSheet("");

        ui->configRadioTransparent->setStyleSheet("");
        ui->configRadioAuto->setStyleSheet("");
        ui->configRadioOpaque->setStyleSheet("");

        ui->configCheckDarkTheme->setStyleSheet("");
        ui->configCheckWindowSplit->setStyleSheet("");
        ui->configCheckDeckWindow->setStyleSheet("");
        ui->configCheckOverlay->setStyleSheet("");
        ui->configCheckLearning->setStyleSheet("");

        ui->logTextEdit->setStyleSheet("");
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
                                       "QPushButton:hover {background: "
                                                      "qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, "
                                                      "stop: 0 #F0F0F0, stop: 1 #90EE90);}");
        ui->minimizeButton->setStyleSheet("QPushButton {background: #F0F0F0; border: none;}"
                                          "QPushButton:hover {background: "
                                                       "qlineargradient(x1: 1, y1: 1, x2: 0, y2: 0, "
                                                       "stop: 0 #F0F0F0, stop: 1 #90EE90);}");
    }
    else
    {
        ui->closeButton->setStyleSheet("QPushButton {background: black; border: none;}"
                                       "QPushButton:hover {background: "
                                                      "qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, "
                                                      "stop: 0 black, stop: 1 #006400);}");
        ui->minimizeButton->setStyleSheet("QPushButton {background: black; border: none;}"
                                          "QPushButton:hover {background: "
                                                       "qlineargradient(x1: 1, y1: 1, x2: 0, y2: 0, "
                                                       "stop: 0 black, stop: 1 #006400);}");
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
}


void MainWindow::updateTamCard(int value)
{
    this->cardHeight = value;
    deckHandler->setCardHeight(value);
    calculateDeckWindowMinimumWidth();

    QString labelText = QString::number(value) + " px";
    ui->configSliderCardSize->setToolTip(labelText);
    ui->configLabelDeckNormal2->setText(labelText);


    this->greyedHeight = value;
    deckHandler->setGreyedHeight(value);
}


void MainWindow::updateTooltipScale(int value)
{
    cardWindow->scale(value);

    QString labelText = "x"+QString::number(value/10.0);
    ui->configSliderTooltipSize->setToolTip(labelText);
    ui->configLabelDeckTooltip2->setText(labelText);
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
            ui->configButtonMastery->setIcon(QIcon(":/Images/refresh64.png"));
            ui->configButtonMastery->setEnabled(false);
            break;
    }
}


void MainWindow::tryConnectAM()
{
    if(webUploader == NULL) return;
    if(arenaHandler == NULL)return;
    if(ui->configLineEditMastery->text().isEmpty())     return;
    if(ui->configLineEditMastery2->text().isEmpty())    return;

    arenaHandler->currentArenaToWhite();
    webUploader->tryConnect(ui->configLineEditMastery->text(), ui->configLineEditMastery2->text());
    ui->configButtonMastery->setIcon(QIcon(":/Images/refresh64.png"));
    ui->configButtonMastery->setEnabled(false);
}


void MainWindow::updateHStatsConnectButton(bool isConnected)
{
    if(isConnected) updateHStatsConnectButton(1);
    else            updateHStatsConnectButton(0);
}


void MainWindow::updateHStatsConnectButton(int value)
{
    switch(value)
    {
        case 0:
            ui->configButtonHStats->setIcon(QIcon(":/Images/lose.png"));
            ui->configButtonHStats->setEnabled(true);
            break;
        case 1:
            ui->configButtonHStats->setIcon(QIcon(":/Images/win.png"));
            ui->configButtonHStats->setEnabled(true);
            break;
        case 2:
            ui->configButtonHStats->setIcon(QIcon(":/Images/refresh64.png"));
            ui->configButtonHStats->setEnabled(false);
            break;
    }
}


void MainWindow::tryConnectHStats()
{
    if(hstatsUploader == NULL) return;
    if(arenaHandler == NULL)return;
    if(ui->configLineEditHStats->text().isEmpty())     return;
    if(ui->configLineEditHStats2->text().isEmpty())    return;

//    arenaHandler->currentArenaToWhite();
    hstatsUploader->tryConnect(ui->configLineEditHStats->text(), ui->configLineEditHStats2->text());
    ui->configButtonHStats->setIcon(QIcon(":/Images/refresh64.png"));
    ui->configButtonHStats->setEnabled(false);
}


void MainWindow::completeConfigTab()
{
    //Cambiar en Designer margenes/spacing de nuevos configBox a 5-9-5-9/5
    //Actions
    connect(ui->configButtonClearDeck, SIGNAL(clicked()), this, SLOT(confirmClearDeck()));
    addDraftMenu(ui->configButtonForceDraft);

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

    //Hand
    connect(ui->configSliderDrawTime, SIGNAL(valueChanged(int)), this, SLOT(updateTimeDraw(int)));

    //Draft
    connect(ui->configCheckOverlay, SIGNAL(clicked()), this, SLOT(toggleShowDraftOverlay()));
    connect(ui->configCheckLearning, SIGNAL(clicked()), this, SLOT(toggleDraftLearningMode()));

    //Arena Mastery
    connect(ui->configLineEditMastery, SIGNAL(textChanged(QString)), this, SLOT(updateAMConnectButton()));
    connect(ui->configLineEditMastery, SIGNAL(editingFinished()), this, SLOT(tryConnectAM()));

    connect(ui->configLineEditMastery2, SIGNAL(textChanged(QString)), this, SLOT(updateAMConnectButton()));
    connect(ui->configLineEditMastery2, SIGNAL(editingFinished()), this, SLOT(tryConnectAM()));

    connect(ui->configButtonMastery, SIGNAL(clicked()), this, SLOT(updateAMConnectButton()));
    connect(ui->configButtonMastery, SIGNAL(clicked()), this, SLOT(tryConnectAM()));

    //Hearth Stats
    ui->configBoxHStats->hide();
//    connect(ui->configLineEditHStats, SIGNAL(textChanged(QString)), this, SLOT(updateHStatsConnectButton()));
//    connect(ui->configLineEditHStats, SIGNAL(editingFinished()), this, SLOT(tryConnectHStats()));

//    connect(ui->configLineEditHStats2, SIGNAL(textChanged(QString)), this, SLOT(updateHStatsConnectButton()));
//    connect(ui->configLineEditHStats2, SIGNAL(editingFinished()), this, SLOT(tryConnectHStats()));

//    connect(ui->configButtonHStats, SIGNAL(clicked()), this, SLOT(updateHStatsConnectButton()));
//    connect(ui->configButtonHStats, SIGNAL(clicked()), this, SLOT(tryConnectHStats()));

    completeHighResConfigTab();
}


void MainWindow::completeHighResConfigTab()
{
    int screenHeight = getScreenHighest();
    if(screenHeight < 1000) return;

    int maxCard = (int)(screenHeight/1000.0*35);
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


LoadingScreen MainWindow::getLoadingScreen()
{
    if(gameWatcher != NULL) return gameWatcher->getLoadingScreen();
    else                    return menu;
}


//TODO
//Button to web
//triodo: you can check for cardids on drawn cards
//(log.config) y test
//Test duplicate con arenaPrevious
//Nuevo formato Json cards. Ahora es solo un array de cartas. Eliminado objeto de sets de array.

//New/Remove deck poner loadedDeck a NULL


//BUGS CONOCIDOS
//Tab Config ScrollArea slider transparent CSS

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
