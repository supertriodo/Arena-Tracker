#include "mainwindow.h"
#include "Widgets/ui_extended.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include <QtWidgets>

using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::Extended)
{
    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");
    ui->setupUi(this);

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)
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

    readSettings();
    completeToolButton();
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

    ui->tabDeckLayout->setContentsMargins(11, 11, 11, 11);
    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    this->windowsFormation = None;
    resizeTabWidgets(event);
}


void MainWindow::resetDeckAlreadyRead()
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
        pDebug("Language: " + lang + "not supported. Using enUS.");
        pLog("Settings: Language " + lang + "not supported. Using enUS.");
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
    connect(ui->deckListWidget, SIGNAL(xLeave()),
            deckHandler, SLOT(deselectRow()));
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

    connect(gameWatcher, SIGNAL(newGameResult(GameResult, bool)),
            arenaHandler, SLOT(newGameResult(GameResult, bool)));
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
//    connect(gameWatcher,SIGNAL(leaveArena()), //No son necesarios, el draft no entra en loop
//            draftHandler, SLOT(pauseDraft()));
//    connect(gameWatcher,SIGNAL(enterArena()),
//            draftHandler, SLOT(resumeDraft()));
//    connect(gameWatcher,SIGNAL(enterArena()),
//            this, SLOT(showTabHeroOnNoArena()));
    connect(gameWatcher,SIGNAL(beginReadingDeck()),
            this, SLOT(resetDeck()));
    connect(gameWatcher,SIGNAL(pickCard(QString)),
            draftHandler, SLOT(pickCard(QString)));

    //Connect en webUploader
//    connect(gameWatcher,SIGNAL(beginReadingDeck()),
//            webUploader, SLOT(askArenaCards()));
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

    //Connections after synchronized
    connect(gameWatcher,SIGNAL(newArena(QString)),
            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            this, SLOT(resetDeckAlreadyRead()));

    //Test
    test();
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this);
    connect(webUploader, SIGNAL(loadedGameResult(GameResult)),
            arenaHandler, SLOT(showGameResult(GameResult)));
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
//    connect(webUploader, SIGNAL(noArenaFound()),
//            this, SLOT(resetDeck()));
    connect(webUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(webUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

    //No cargamos el deck desde arenaMastery, lo leemos del log
//#ifndef QT_DEBUG //En debug no se carga el deck de web
//    connect(webUploader, SIGNAL(newDeckCard(QString,int)),
//            deckHandler, SLOT(newDeckCardWeb(QString,int)));
//    connect(webUploader, SIGNAL(newWebDeckCardList()),
//            this, SLOT(resetDeckAlreadyRead()));
//#endif

//    //Connect de gameWatcher
//    connect(gameWatcher,SIGNAL(beginReadingDeck()),
//            webUploader, SLOT(askArenaCards()));

    arenaHandler->setWebUploader(webUploader);

    ui->progressBar->setVisible(false);
}


void MainWindow::completeUI()
{
    if(isMainWindow)
    {
        ui->tabWidget = new MoveTabWidget(this);
        ui->tabWidget->hide();

        ui->tabWidgetTemplate->clear();
        ui->tabWidgetTemplate->hide();
        ui->gridLayout->removeWidget(ui->tabWidgetTemplate);
        ui->gridLayout->addWidget(ui->tabWidget, 0, 0);

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

        completeHeroButtons();

        ui->arenaTreeWidget = new MoveTreeWidget(ui->tabArena);
        ui->tabArenalLayout->insertWidget(1, ui->arenaTreeWidget);
        ui->enemyHandListWidget = new MoveListWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->enemyHandListWidget);
        ui->secretsTreeWidget = new MoveTreeWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->secretsTreeWidget);
        ui->drawListWidget = new MoveListWidget(ui->tabEnemy);
        ui->tabEnemyLayout->addWidget(ui->drawListWidget);
        ui->deckListWidget = new MoveListWidget(ui->tabDeck);
        ui->tabDeckLayout->addWidget(ui->deckListWidget);


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


        ui->toolButton = new QPushButton("", this);
        ui->toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->toolButton->resize(24, 24);
        ui->toolButton->setIconSize(QSize(24, 24));
        ui->toolButton->setIcon(QIcon(":/Images/config.png"));
        ui->toolButton->setFlat(true);

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

        this->greyedHeight = settings.value("greyedHeight", -1).toInt();
        this->cardHeight = settings.value("cardHeight", 35).toInt();
        this->drawDisappear = settings.value("drawDisappear", 5).toInt();
        this->showDraftOverlay = settings.value("showDraftOverlay", true).toBool();
        this->draftLearningMode = settings.value("draftLearningMode", false).toBool();

        //Spread options to components
        deckHandler->setGreyedHeight((this->greyedHeight==-1)?this->cardHeight:this->greyedHeight);
        deckHandler->setCardHeight(this->cardHeight);
        deckHandler->setDrawDisappear(this->drawDisappear);
        draftHandler->setShowDraftOverlay(this->showDraftOverlay);
        draftHandler->setLearningMode(this->draftLearningMode);
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
    move(pos);
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
        ui->toolButton->move(left, top);
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

    if(newWindowsFormation == windowsFormation) return;
    windowsFormation = newWindowsFormation;

    ui->tabWidget->hide();
    ui->tabWidgetH2->hide();
    ui->tabWidgetH3->hide();
    ui->tabWidgetV1->hide();

    switch(windowsFormation)
    {
        case None:
        case H1:
            if(otherWindow == NULL)
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabDeck, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidget);
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
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
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
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetH2->show();
                ui->tabWidgetH3->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
                moveTabTo(ui->tabLog, ui->tabWidgetH3);
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
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetV1->show();
            }
            else
            {
                moveTabTo(ui->tabArena, ui->tabWidget);
                moveTabTo(ui->tabEnemy, ui->tabWidgetV1);
                moveTabTo(ui->tabLog, ui->tabWidget);
                ui->tabWidget->show();
                ui->tabWidgetV1->show();
            }
            break;
    }

    this->calculateMinimumWidth();
}


void MainWindow::moveTabTo(QWidget *widget, QTabWidget *tabWidget, int index)
{
    QString label = "";
    if(widget == ui->tabArena)
    {
        label = "Arena";
    }
    else if(widget == ui->tabDeck)
    {
        label = "Deck";
    }
    else if(widget == ui->tabEnemy)
    {
        label = "Hand";
    }
    else if(widget == ui->tabLog)
    {
        label = "Log";
    }

    if(index == -1)     tabWidget->addTab(widget, label);
    else                tabWidget->insertTab(index, widget, label);
}


void MainWindow::calculateMinimumWidth()
{
    if(!isMainWindow || (windowsFormation!=H1 && windowsFormation!=V2)) return;

    int minWidth = this->width() - ui->tabWidget->width();
    minWidth += ui->tabWidget->tabBar()->width();
    if(ui->minimizeButton!=NULL)    minWidth += ui->minimizeButton->width()*2;
    if(ui->closeButton!=NULL)       minWidth += ui->closeButton->width()*2;

    this->setMinimumWidth(minWidth);
}


//Fija la anchura de la ventana de deck.
void MainWindow::calculateDeckWindowMinimumWidth()
{
    if(this->otherWindow!=NULL && deckHandler!= NULL)
    {
        int deckWidth = this->otherWindow->width() - ui->deckListWidget->width() + ui->deckListWidget->sizeHintForColumn(0);
        this->otherWindow->setMinimumWidth(deckWidth);
        this->otherWindow->setMaximumWidth(deckWidth);
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
        pLog(tr("File: ERROR:Creating Arena Tracker log on disk. Make sure HSCards dir is in the same place as the exe."));
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


//MENUS
void MainWindow::addDraftMenu(QMenu *menu)
{
    QMenu *newArenaMenu = new QMenu("Force Draft", this);

    QSignalMapper* mapper = new QSignalMapper(this);
    QString heroes[9] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};

    for(int i=0; i<9; i++)
    {
        QAction *action = newArenaMenu->addAction(heroes[i]);
        mapper->setMapping(action, heroes[i]);
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    }

    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));

    menu->addMenu(newArenaMenu);
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


void MainWindow::addClearDeckMenu(QMenu *menu)
{
    QAction *action = menu->addAction("Clear Deck");
    connect(action, SIGNAL(triggered()), this, SLOT(confirmClearDeck()));
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


void MainWindow::addSplitAction(QMenu *menu)
{
    QAction * action = new QAction("Window Split", this);
    action->setCheckable(true);
    if(this->splitWindow) action->setChecked(true);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleSplitWindow()));
    menu->addAction(action);
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


void MainWindow::addTransparentMenu(QMenu *menu)
{
    QAction *action0 = new QAction("Transparent", this);
    QAction *action1 = new QAction("Auto", this);
    QAction *action2 = new QAction("Opaque", this);
    action0->setCheckable(true);
    action1->setCheckable(true);
    action2->setCheckable(true);
    connect(action0, SIGNAL(triggered()), this, SLOT(transparentAlways()));
    connect(action1, SIGNAL(triggered()), this, SLOT(transparentAuto()));
    connect(action2, SIGNAL(triggered()), this, SLOT(transparentNever()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action0);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);

    switch(transparency)
    {
        case Transparent:
            action0->setChecked(true);
            break;
        case AutoTransparent:
            action1->setChecked(true);
            break;
        case Opaque:
            action2->setChecked(true);
            break;
        default:
            transparency = AutoTransparent;
            action1->setChecked(true);
            break;
    }

    QMenu *transparentMenu = new QMenu("Transparency", this);
    transparentMenu->addAction(action0);
    transparentMenu->addAction(action1);
    transparentMenu->addAction(action2);
    menu->addMenu(transparentMenu);
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
}


void MainWindow::addThemeAction(QMenu *menu)
{
    QAction * action = new QAction("Dark Theme", this);
    action->setCheckable(true);
    if(this->theme == ThemeBlack) action->setChecked(true);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleTheme()));
    menu->addAction(action);
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


void MainWindow::updateMainUITheme()
{
    ui->tabWidget->setTheme(this->theme);
    ui->tabWidgetH2->setTheme(this->theme);
    ui->tabWidgetH3->setTheme(this->theme);
    ui->tabWidgetV1->setTheme(this->theme);
    updateButtonsTheme();

    QString mainCSS = "";
    if(theme == ThemeWhite)
    {
        mainCSS +=
                "QGroupBox {border-width: 0px; border-color: transparent;}"
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

                "QGroupBox {border-width: 0px; border-color: transparent; background-color: transparent; color: white;}"
                "QLabel {background-color: transparent; color: white;}"
                "QTextBrowser {background-color: transparent; color: white;}"
                ;
    }
    this->setStyleSheet(mainCSS);
    if(otherWindow!=NULL)   otherWindow->setStyleSheet(mainCSS);
}


void MainWindow::updateButtonsTheme()
{
    if(theme == ThemeWhite)
    {
        ui->closeButton->setStyleSheet("QPushButton {background: white; border: none;}"
                                       "QPushButton:hover {background: "
                                                      "qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, "
                                                      "stop: 0 white, stop: 1 #90EE90);}");
        ui->minimizeButton->setStyleSheet("QPushButton {background: white; border: none;}"
                                          "QPushButton:hover {background: "
                                                       "qlineargradient(x1: 1, y1: 1, x2: 0, y2: 0, "
                                                       "stop: 0 white, stop: 1 #90EE90);}");
        ui->toolButton->setStyleSheet("QPushButton {background: white; border: none;}"
                                      "QPushButton::menu-indicator {subcontrol-position: right;}"
                                      );
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
        ui->toolButton->setStyleSheet("QPushButton {background: black; border: none;}"
                                      "QPushButton::menu-indicator {subcontrol-position: right;}"
                                      );
    }
}


void MainWindow::addDeckWindowAction(QMenu *menu)
{
    QAction * deckWindowAction = new QAction("Show Deck Window", this);
    deckWindowAction->setCheckable(true);
    if(this->otherWindow!=NULL) deckWindowAction->setChecked(true);
    connect(deckWindowAction, SIGNAL(triggered()), this, SLOT(toggleDeckWindow()));
    menu->addAction(deckWindowAction);
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


void MainWindow::addTamGreyedMenu(QMenu *menu)
{
    QAction *action0 = new QAction("15px(Smallest)", this);
    QAction *action1 = new QAction("20px", this);
    QAction *action2 = new QAction("25px", this);
    QAction *action3 = new QAction("30px", this);
    QAction *action4 = new QAction("35px(Normal)", this);
    QAction *action5 = new QAction("= Card Size", this);
    action0->setCheckable(true);
    action1->setCheckable(true);
    action2->setCheckable(true);
    action3->setCheckable(true);
    action4->setCheckable(true);
    action5->setCheckable(true);
    connect(action0, SIGNAL(triggered()), this, SLOT(tamGreyed15px()));
    connect(action1, SIGNAL(triggered()), this, SLOT(tamGreyed20px()));
    connect(action2, SIGNAL(triggered()), this, SLOT(tamGreyed25px()));
    connect(action3, SIGNAL(triggered()), this, SLOT(tamGreyed30px()));
    connect(action4, SIGNAL(triggered()), this, SLOT(tamGreyed35px()));
    connect(action5, SIGNAL(triggered()), this, SLOT(tamGreyedAsCardSize()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action0);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    splitGroup->addAction(action3);
    splitGroup->addAction(action4);
    splitGroup->addAction(action5);

    switch(greyedHeight)
    {
        case 15:
            action0->setChecked(true);
            break;
        case 20:
            action1->setChecked(true);
            break;
        case 25:
            action2->setChecked(true);
            break;
        case 30:
            action3->setChecked(true);
            break;
        case 35:
            action4->setChecked(true);
            break;
        case -1:
            action5->setChecked(true);
            break;
        default:
            greyedHeight = -1;
            action5->setChecked(true);
            break;
    }

    QMenu *tamGreyedMenu = new QMenu("Deck: Greyed Size", this);
    tamGreyedMenu->addAction(action0);
    tamGreyedMenu->addAction(action1);
    tamGreyedMenu->addAction(action2);
    tamGreyedMenu->addAction(action3);
    tamGreyedMenu->addAction(action4);
    tamGreyedMenu->addAction(action5);
    menu->addMenu(tamGreyedMenu);
}


void MainWindow::tamGreyed15px()
{
    this->greyedHeight = 15;
    deckHandler->setGreyedHeight(this->greyedHeight);
}


void MainWindow::tamGreyed20px()
{
    this->greyedHeight = 20;
    deckHandler->setGreyedHeight(this->greyedHeight);
}


void MainWindow::tamGreyed25px()
{
    this->greyedHeight = 25;
    deckHandler->setGreyedHeight(this->greyedHeight);
}


void MainWindow::tamGreyed30px()
{
    this->greyedHeight = 30;
    deckHandler->setGreyedHeight(this->greyedHeight);
}


void MainWindow::tamGreyed35px()
{
    this->greyedHeight = 35;
    deckHandler->setGreyedHeight(this->greyedHeight);
}


void MainWindow::tamGreyedAsCardSize()
{
    this->greyedHeight = -1;
    deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::addTamCardMenu(QMenu *menu)
{
    QAction *action0 = new QAction("15px(Smallest)", this);
    QAction *action1 = new QAction("20px", this);
    QAction *action2 = new QAction("25px", this);
    QAction *action3 = new QAction("30px", this);
    QAction *action4 = new QAction("35px(Normal)", this);
    action0->setCheckable(true);
    action1->setCheckable(true);
    action2->setCheckable(true);
    action3->setCheckable(true);
    action4->setCheckable(true);
    connect(action0, SIGNAL(triggered()), this, SLOT(tamCard15px()));
    connect(action1, SIGNAL(triggered()), this, SLOT(tamCard20px()));
    connect(action2, SIGNAL(triggered()), this, SLOT(tamCard25px()));
    connect(action3, SIGNAL(triggered()), this, SLOT(tamCard30px()));
    connect(action4, SIGNAL(triggered()), this, SLOT(tamCard35px()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action0);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    splitGroup->addAction(action3);
    splitGroup->addAction(action4);

    switch(cardHeight)
    {
        case 15:
            action0->setChecked(true);
            break;
        case 20:
            action1->setChecked(true);
            break;
        case 25:
            action2->setChecked(true);
            break;
        case 30:
            action3->setChecked(true);
            break;
        case 35:
            action4->setChecked(true);
            break;
        default:
            cardHeight = 35;
            action4->setChecked(true);
            break;
    }

    QMenu *tamCardMenu = new QMenu("Deck: Card Size", this);
    tamCardMenu->addAction(action0);
    tamCardMenu->addAction(action1);
    tamCardMenu->addAction(action2);
    tamCardMenu->addAction(action3);
    tamCardMenu->addAction(action4);
    menu->addMenu(tamCardMenu);
}


void MainWindow::tamCard15px()
{
    this->cardHeight = 15;
    deckHandler->setCardHeight(this->cardHeight);
    if(this->greyedHeight==-1)  deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::tamCard20px()
{
    this->cardHeight = 20;
    deckHandler->setCardHeight(this->cardHeight);
    if(this->greyedHeight==-1)  deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::tamCard25px()
{
    this->cardHeight = 25;
    deckHandler->setCardHeight(this->cardHeight);
    if(this->greyedHeight==-1)  deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::tamCard30px()
{
    this->cardHeight = 30;
    deckHandler->setCardHeight(this->cardHeight);
    if(this->greyedHeight==-1)  deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::tamCard35px()
{
    this->cardHeight = 35;
    deckHandler->setCardHeight(this->cardHeight);
    if(this->greyedHeight==-1)  deckHandler->setGreyedHeight(this->cardHeight);
}


void MainWindow::addTimeDrawMenu(QMenu *menu)
{
    QAction *action0 = new QAction("Hide", this);
    QAction *action1 = new QAction("5s", this);
    QAction *action2 = new QAction("10s", this);
    QAction *action3 = new QAction("Turn", this);
    action0->setCheckable(true);
    action1->setCheckable(true);
    action2->setCheckable(true);
    action3->setCheckable(true);
    connect(action0, SIGNAL(triggered()), this, SLOT(timeDrawNo()));
    connect(action1, SIGNAL(triggered()), this, SLOT(timeDraw5s()));
    connect(action2, SIGNAL(triggered()), this, SLOT(timeDraw10s()));
    connect(action3, SIGNAL(triggered()), this, SLOT(timeDrawTurn()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action0);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    splitGroup->addAction(action3);

    switch(drawDisappear)
    {
        case -1:
            action0->setChecked(true);
            break;
        case 5:
            action1->setChecked(true);
            break;
        case 10:
            action2->setChecked(true);
            break;
        case 0:
            action3->setChecked(true);
            break;
        default:
            drawDisappear = 5;
            action1->setChecked(true);
            break;
    }

    QMenu *timeDrawMenu = new QMenu("Hand: Show Card Draw", this);
    timeDrawMenu->addAction(action0);
    timeDrawMenu->addAction(action1);
    timeDrawMenu->addAction(action2);
    timeDrawMenu->addAction(action3);
    menu->addMenu(timeDrawMenu);
}


void MainWindow::timeDrawNo()
{
    this->drawDisappear = -1;
    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::timeDraw5s()
{
    this->drawDisappear = 5;
    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::timeDraw10s()
{
    this->drawDisappear = 10;
    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::timeDrawTurn()
{
    this->drawDisappear = 0;
    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::addShowDraftOverlayAction(QMenu *menu)
{
    QAction * action = new QAction("Draft: Show Overlay", this);
    action->setCheckable(true);
    if(this->showDraftOverlay) action->setChecked(true);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleShowDraftOverlay()));
    menu->addAction(action);
}


void MainWindow::toggleShowDraftOverlay()
{
    this->showDraftOverlay = !this->showDraftOverlay;
    draftHandler->setShowDraftOverlay(this->showDraftOverlay);
}


void MainWindow::addDraftLearningModeAction(QMenu *menu)
{
    QAction * action = new QAction("Draft: Learning Mode", this);
    action->setCheckable(true);
    if(this->draftLearningMode) action->setChecked(true);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleDraftLearningMode()));
    menu->addAction(action);
}


void MainWindow::toggleDraftLearningMode()
{
    this->draftLearningMode = !this->draftLearningMode;
    draftHandler->setLearningMode(this->draftLearningMode);
}


void MainWindow::completeToolButton()
{
    QMenu *menu = new QMenu(this);
    addClearDeckMenu(menu);
    addDraftMenu(menu);
    menu->addSeparator();
    addTamCardMenu(menu);
    addTamGreyedMenu(menu);
    menu->addSeparator();
    addTimeDrawMenu(menu);
    menu->addSeparator();
    addShowDraftOverlayAction(menu);
    addDraftLearningModeAction(menu);
    menu->addSeparator();
    addTransparentMenu(menu);
    addThemeAction(menu);
    addSplitAction(menu);
    addDeckWindowAction(menu);

    ui->toolButton->setMenu(menu);

    updateMainUITheme();
}


//TODO
//Tooltip window
//New stats site
//Mostrar razas
//Eliminar arenaHandler setTheme
//Tam hands list sizePolicy

//BUGS CONOCIDOS
//Bug log tavern brawl (No hay [Bob] ---Register al entrar a tavern brawl) (Solo falla si no hay que hacer un mazo)

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
