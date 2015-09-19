#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility.h"
#include <QtWidgets>

using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow)
{
    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");
    ui->setupUi(this);

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)
    atLogFile = NULL;
    isMainWindow = true;
    otherWindow = NULL;

    createLogFile();
    readSettings();
    completeUI();
    initCardsJson();

    createCardDownloader();
    createSecretsHandler();
    createDeckHandler();
    createDraftHandler();
    createEnemyHandHandler();
    createArenaHandler();
    createGameWatcher();
    createLogLoader();
}


MainWindow::MainWindow(QWidget *parent, MainWindow *primaryWindow) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    readSettings();
    completeUI();
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
    if(resizeButton != NULL)    delete resizeButton;
    if(ui != NULL)              delete ui;
    closeLogFile();
}


void MainWindow::createSecondaryWindow()
{
    this->otherWindow = new MainWindow(0, this);
    this->otherWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    this->otherWindow->show();

    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    this->windowsFormation = None;
    resizeTabWidgets(event);
}


void MainWindow::destroySecondaryWindow()
{
    this->otherWindow->close();
    this->otherWindow = NULL;

    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    this->windowsFormation = None;
    resizeTabWidgets(event);
}


void MainWindow::resetDeckFromWeb()
{
    resetDeck(true);
}


void MainWindow::resetDeck(bool deckRead)
{
    gameWatcher->setDeckRead(deckRead);
    deckHandler->reset();
}


void MainWindow::initCardsJson()
{
    QFile jsonFile(":Json/AllSets.json");
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

    DeckCard::setCardsJson(&cardsJson);
    GameWatcher::setCardsJson(&cardsJson);
}


void MainWindow::createDraftHandler()
{
    draftHandler = new DraftHandler(this, &cardsJson, ui);
    connect(draftHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(draftHandler, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCard(QString)));
    connect(draftHandler, SIGNAL(draftEnded()),
            this, SLOT(uploadDeck()));
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

    connect(gameWatcher, SIGNAL(newGameResult(GameResult)),
            arenaHandler, SLOT(newGameResult(GameResult)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            arenaHandler, SLOT(newArena(QString)));
    connect(gameWatcher, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
            arenaHandler, SLOT(showArenaReward(int,int,bool,bool,bool)));
    connect(gameWatcher, SIGNAL(arenaRewardsComplete()),
            arenaHandler, SLOT(uploadCurrentArenaRewards()));

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
            enemyHandHandler, SLOT(showEnemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(lastHandCardIsCoin()),
            enemyHandHandler, SLOT(lastHandCardIsCoin()));
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
    connect(gameWatcher, SIGNAL(playerMinionPlayed()),
            secretsHandler, SLOT(playerMinionPlayed()));
    connect(gameWatcher, SIGNAL(enemyMinionDead()),
            secretsHandler, SLOT(enemyMinionDead()));
    connect(gameWatcher, SIGNAL(avengeTested()),
            secretsHandler, SLOT(avengeTested()));
    connect(gameWatcher, SIGNAL(cSpiritTested()),
            secretsHandler, SLOT(cSpiritTested()));
    connect(gameWatcher, SIGNAL(playerAttack(bool,bool)),
            secretsHandler, SLOT(playerAttack(bool,bool)));

//    connect(gameWatcher,SIGNAL(newArena(QString)),
//            draftHandler, SLOT(beginDraft(QString)));
//    connect(gameWatcher,SIGNAL(activeDraftDeck()),
//            draftHandler, SLOT(endDraft()));
    connect(gameWatcher,SIGNAL(startGame()),    //Salida alternativa de drafting (+seguridad)
            draftHandler, SLOT(endDraft()));
    connect(gameWatcher,SIGNAL(leaveArena()),
            draftHandler, SLOT(pauseDraft()));
    connect(gameWatcher,SIGNAL(enterArena()),
            draftHandler, SLOT(resumeDraft()));
    connect(gameWatcher,SIGNAL(enterArena()),
            this, SLOT(showTabHeroOnNoArena()));
}


void MainWindow::showTabHeroOnNoArena()
{
    if(webUploader == NULL) return;
    if(arenaHandler->isNoArena())
    {
        ui->tabWidget->addTab(ui->tabHero, "Draft");
        ui->tabWidget->setCurrentWidget(ui->tabHero);
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
    connect(logLoader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(logLoader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

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
    spreadTransparency();
    deckHandler->setGreyedHeight(this->greyedHeight);

    //Test
#ifdef QT_DEBUG
    test();
#endif
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this, &cardsJson);
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
    connect(webUploader, SIGNAL(noArenaFound()),
            arenaHandler, SLOT(showNoArena()));
    connect(webUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(webUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
#ifndef QT_DEBUG //Si tenemos una arena en web podemos seguir testeando deck en construido
    connect(webUploader, SIGNAL(newDeckCard(QString,int)),
            deckHandler, SLOT(newDeckCard(QString,int)));
    connect(webUploader, SIGNAL(newWebDeckCardList()),
            this, SLOT(resetDeckFromWeb()));
#endif

    arenaHandler->setWebUploader(webUploader);

    ui->progressBar->setVisible(false);
}


void MainWindow::completeUI()
{
    ui->tabWidget->hide();
    ui->tabWidgetH2->hide();
    ui->tabWidgetH3->hide();
    ui->tabWidgetV1->hide();
    ui->tabWidgetV2->hide();

    ui->tabWidget->setCurrentIndex(0);

    resizeButton = new ResizeButton(this);
    ui->bottomLayout->addWidget(resizeButton);
    connect(resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));
    connect(ui->minimizeButton, SIGNAL(clicked()),
            this, SLOT(showMinimized()));

    if(isMainWindow)
    {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabDraft));
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabHero));

        connect(ui->closeButton, SIGNAL(clicked()),
                this, SLOT(close()));

        completeToolButton();
        completeHeroButtons();

#ifdef QT_DEBUG
        pLog(tr("MODE DEBUG"));
        pDebug("MODE DEBUG");
#endif
    }
    else
    {
        delete ui->closeButton; ui->closeButton = NULL;
        delete ui->toolButton;  ui->toolButton = NULL;
        delete ui->progressBar; ui->progressBar = NULL;

        ui->tabWidget->clear();
        moveTabTo(this->otherWindow->ui->tabDeck, ui->tabWidget);
        ui->tabWidget->show();
    }
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


void MainWindow::addDraftMenu(QMenu *menu)
{
    QMenu *newArenaMenu = new QMenu("New arena/draft", this);

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
        pDebug("New manual arena: " + hero);
        pLog(tr("Menu: New arena: ") + hero);
        sizePreDraft = this->size();
        QString heroLog = Utility::heroToLogNumber(hero);
        arenaHandler->newArena(heroLog);
        draftHandler->beginDraft(heroLog);
    }
}


void MainWindow::addClearDeckMenu(QMenu *menu)
{
    QAction *action = menu->addAction("Clear deck");
    connect(action, SIGNAL(triggered()), this, SLOT(confirmClearDeck()));
}


void MainWindow::confirmClearDeck()
{
    int ret = QMessageBox::question(this, tr("Clear deck"),
                                   "Do you want to clear your deck?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        pDebug("Manual clear deck");
        pLog(tr("Menu: Clear deck"));
        resetDeck();
    }
}


void MainWindow::addSplitMenu(QMenu *menu)
{
    QAction *action1 = new QAction("Auto", this);
    QAction *action2 = new QAction("Never", this);
    action1->setCheckable(true);
    action2->setCheckable(true);
    connect(action1, SIGNAL(triggered()), this, SLOT(splitWindowAuto()));
    connect(action2, SIGNAL(triggered()), this, SLOT(splitWindowNever()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    this->splitWindow?action1->setChecked(true):action2->setChecked(true);

    QMenu *windowSplitMenu = new QMenu("Window split", this);
    windowSplitMenu->addAction(action1);
    windowSplitMenu->addAction(action2);
    menu->addMenu(windowSplitMenu);
}


void MainWindow::splitWindowAuto()
{
    this->splitWindow = true;
    spreadSplitWindow();
}


void MainWindow::splitWindowNever()
{
    this->splitWindow = false;
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
    QAction *action0 = new QAction("Always", this);
    QAction *action1 = new QAction("Auto", this);
    QAction *action2 = new QAction("Never", this);
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
        case Always:
            action0->setChecked(true);
            break;
        case Auto:
            action1->setChecked(true);
            break;
        case Never:
            action2->setChecked(true);
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
    this->transparency = Always;
    spreadTransparency();
}


void MainWindow::transparentAuto()
{
    this->transparency = Auto;
    spreadTransparency();
}


void MainWindow::transparentNever()
{
    this->transparency = Never;
    spreadTransparency();
}


void MainWindow::spreadTransparency()
{
    deckHandler->setTransparency(this->transparency);
    enemyHandHandler->setTransparency(this->transparency);
    arenaHandler->setTransparency(this->transparency);
    draftHandler->setTransparency(this->transparency);

    if(isMainWindow && otherWindow != NULL)
    {
        otherWindow->transparency = this->transparency;
    }
}


void MainWindow::addNumWindowsMenu(QMenu *menu)
{
    QAction *action1 = new QAction("1", this);
    QAction *action2 = new QAction("2", this);
    action1->setCheckable(true);
    action2->setCheckable(true);
    connect(action1, SIGNAL(triggered()), this, SLOT(numWindows1()));
    connect(action2, SIGNAL(triggered()), this, SLOT(numWindows2()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    (this->otherWindow == NULL)?action1->setChecked(true):action2->setChecked(true);

    QMenu *numWindowsMenu = new QMenu("Windows", this);
    numWindowsMenu->addAction(action1);
    numWindowsMenu->addAction(action2);
    menu->addMenu(numWindowsMenu);
}


void MainWindow::numWindows1()
{
    if(this->otherWindow != NULL)   destroySecondaryWindow();
}


void MainWindow::numWindows2()
{
    if(this->otherWindow == NULL)   createSecondaryWindow();
}


void MainWindow::addTamGreyedMenu(QMenu *menu)
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
    connect(action0, SIGNAL(triggered()), this, SLOT(tamGreyed15px()));
    connect(action1, SIGNAL(triggered()), this, SLOT(tamGreyed20px()));
    connect(action2, SIGNAL(triggered()), this, SLOT(tamGreyed25px()));
    connect(action3, SIGNAL(triggered()), this, SLOT(tamGreyed30px()));
    connect(action4, SIGNAL(triggered()), this, SLOT(tamGreyed35px()));

    QActionGroup *splitGroup = new QActionGroup(this);
    splitGroup->addAction(action0);
    splitGroup->addAction(action1);
    splitGroup->addAction(action2);
    splitGroup->addAction(action3);
    splitGroup->addAction(action4);

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
    }

    QMenu *tamGreyedMenu = new QMenu("Deck greyed size", this);
    tamGreyedMenu->addAction(action0);
    tamGreyedMenu->addAction(action1);
    tamGreyedMenu->addAction(action2);
    tamGreyedMenu->addAction(action3);
    tamGreyedMenu->addAction(action4);
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


void MainWindow::completeToolButton()
{
    QMenu *menu = new QMenu(this);
    addDraftMenu(menu);
    addClearDeckMenu(menu);
    menu->addSeparator();
    addNumWindowsMenu(menu);
    addSplitMenu(menu);
    addTransparentMenu(menu);
    addTamGreyedMenu(menu);

    ui->toolButton->setMenu(menu);
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

        this->splitWindow = settings.value("splitWindow", true).toBool();
        this->transparency = (Transparency)settings.value("transparent", Auto).toInt();

        int numWindows = settings.value("numWindows", 1).toInt();
        if(numWindows == 2) createSecondaryWindow();

        this->greyedHeight = settings.value("greyedHeight", 25).toInt();
    }
    else
    {
        pos = settings.value("pos2", QPoint(0,0)).toPoint();
        size = settings.value("size2", QSize(400, 400)).toSize();

        this->splitWindow = otherWindow->splitWindow;
        this->transparency = otherWindow->transparency;
    }
    this->windowsFormation = None;
    resize(size);
    move(pos);
    sizePreDraft = size;
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
        settings.setValue("greyedHeight", this->greyedHeight);
        settings.setValue("numWindows", (this->otherWindow == NULL)?1:2);
    }
    else
    {
        settings.setValue("pos2", pos());
        settings.setValue("size2", size());
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isMainWindow && (otherWindow != NULL))
    {
        otherWindow->close();
    }

    writeSettings();
    event->accept();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(isMainWindow)    resizeTabWidgets(event);
    event->accept();
}


void MainWindow::resizeTabWidgets(QResizeEvent *event)
{
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
    ui->tabWidgetV2->hide();

    switch(windowsFormation)
    {
        case None:
        case H1:
//            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);//Reactivar con rewards

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
//            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
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
//            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
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
//            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
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
        label = "Enemy";
    }
    else if(widget == ui->tabLog)
    {
        label = "Log";
    }

    if(index == -1)     tabWidget->addTab(widget, label);
    else                tabWidget->insertTab(index, widget, label);
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
    ui->logTextEdit->append(line);
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
    QFileInfo cardFile("./HSCards/" + code + ".png");

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
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWindow::resizeSlot(QSize size)
{
    resize(size);
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Alt && event->key() != Qt::Key_Control)
    {
        if(event->modifiers()&Qt::AltModifier && event->modifiers()&Qt::ControlModifier)
        {
            if(event->key() == Qt::Key_R)   resetSettings();
        }
    }
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
        resize(QSize(400, 400));
        move(QPoint(0,0));
        this->close();
    }
}


void MainWindow::createLogFile()
{
    atLogFile = new QFile("./HSCards/ArenaTrackerLog.txt");
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

    //Recuperamos el size preDraft
    if(sizePreDraft.width()<this->minimumWidth())   setMinimumWidth(sizePreDraft.width());
    resize(sizePreDraft);
}


void MainWindow::test()
{

}


//TODO
//Consejos iniciales
//Uso en construido.
//Check new version
//Menu drawdelay //llamar clearDrawList() antes de cambiar drawDissapear
//bug

//BUGS CONOCIDOS
//Bug log tavern brawl (No hay [Bob] ---Register al entrar a tavern brawl) (Solo falla si no hay que hacer un mazo)

//DECK IN ARENA MASTERY
//Create arena cards comentado en webUploader.

//REWARDS
//Despues de cada newGameResult se carga checkArenaCurrentReload que si ha terminado la arena enviara un showNoArena a ArenaHandler.
//Esto reinicia las variables y si luego subimos los rewards va a fallar porque no hay arenaCurrent.
//Para reactivar los rewards habra que arreglar esto. Reactivar paso de gameResultSent a reloadArenaCurrent en replyFinished
//Descomentar en resizeTabWidgets (adjustToContents)

//NUEVAS CARTAS
//Update Json cartas
//Update Json heroes HA
//Update secrets

//NUEVOS HEROES
//Evitar Asset hero powers (GameWatcher 201)
//Nuevo Json hearthArena
//Nuevo start draft menu
