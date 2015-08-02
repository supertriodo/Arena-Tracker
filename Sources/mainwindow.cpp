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

#ifdef QT_DEBUG
    writeLog(tr("MODE DEBUG"));
#endif

    completeUI();
    readSettings();
    initCardsJson();

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)

    createCardDownloader();
    createSecretsHandler();
    createDeckHandler();
    createDraftHandler();
    createEnemyHandHandler();
    createArenaHandler();
    createGameWatcher();
    createLogLoader();
}


MainWindow::~MainWindow()
{
    delete logLoader;
    delete gameWatcher;
    delete arenaHandler;
    delete webUploader;
    delete cardDownloader;
    delete enemyHandHandler;
    delete draftHandler;
    delete deckHandler;
    delete secretsHandler;
    delete resizeButton;
    delete ui;
}


void MainWindow::resetDeckFromWeb()
{
//    gameWatcher->setDeckRead();
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
    connect(draftHandler, SIGNAL(deckComplete()),
            this, SLOT(uploadDeck()));
    connect(draftHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
}


void MainWindow::createSecretsHandler()
{
    secretsHandler = new SecretsHandler(this, ui);
    connect(secretsHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
}


void MainWindow::createArenaHandler()
{
    arenaHandler = new ArenaHandler(this, deckHandler, ui);
    connect(arenaHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
}


void MainWindow::createDeckHandler()
{
    deckHandler = new DeckHandler(this, &cardsJson, ui);
    connect(deckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(deckHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLogConnected(QString)));
}


void MainWindow::createEnemyHandHandler()
{
    enemyHandHandler = new EnemyHandHandler(this, ui);
    connect(enemyHandHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyHandHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLogConnected(QString)));
}


void MainWindow::createCardDownloader()
{
    cardDownloader = new HSCardDownloader(this);
    connect(cardDownloader, SIGNAL(downloaded(QString)),
            this, SLOT(redrawDownloadedCardImage(QString)));
    connect(cardDownloader, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
}


void MainWindow::createGameWatcher()
{
    gameWatcher = new GameWatcher(this);

    connect(gameWatcher, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));

    connect(gameWatcher, SIGNAL(newGameResult(GameResult)),
            arenaHandler, SLOT(newGameResult(GameResult)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            arenaHandler, SLOT(newArena(QString)));
    connect(gameWatcher, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
            arenaHandler, SLOT(showArenaReward(int,int,bool,bool,bool)));
    connect(gameWatcher, SIGNAL(arenaRewardsComplete()),
            arenaHandler, SLOT(uploadCurrentArenaRewards()));

    connect(gameWatcher, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCard(QString)));
    connect(gameWatcher, SIGNAL(playerCardDraw(QString)),
            deckHandler, SLOT(showPlayerCardDraw(QString)));
    //connect en synchronizedDone
    connect(gameWatcher, SIGNAL(endGame()),
            deckHandler, SLOT(unlockDeckInterface()));

    connect(gameWatcher, SIGNAL(enemyCardDraw(int,int,bool,QString)),
            enemyHandHandler, SLOT(showEnemyCardDraw(int,int,bool,QString)));
    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString)),
            enemyHandHandler, SLOT(showEnemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(lastHandCardIsCoin()),
            enemyHandHandler, SLOT(lastHandCardIsCoin()));
    //connect en synchronizedDone
    connect(gameWatcher, SIGNAL(endGame()),
            enemyHandHandler, SLOT(unlockEnemyInterface()));

    connect(gameWatcher, SIGNAL(endGame()),
            secretsHandler, SLOT(resetSecretsInterface()));
    connect(gameWatcher, SIGNAL(enemySecretPlayed(int,SecretHero)),
            secretsHandler, SLOT(secretPlayed(int,SecretHero)));
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
    connect(logLoader, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
    connect(logLoader, SIGNAL(newLogLineRead(QString)),
            gameWatcher, SLOT(processLogLine(QString)));

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

    connect(gameWatcher, SIGNAL(startGame()),
            deckHandler, SLOT(lockDeckInterface()));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyHandHandler, SLOT(lockEnemyInterface()));

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
            arenaHandler, SLOT(enableButtons()));
    connect(webUploader, SIGNAL(noArenaFound()),
            arenaHandler, SLOT(showNoArena()));
    connect(webUploader, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
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
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabDraft));
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabHero));

    resizeButton = new ResizeButton(this);
    ui->bottomLayout->addWidget(resizeButton);
    connect(resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));

    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->minimizeButton, SIGNAL(clicked()),
            this, SLOT(showMinimized()));

    completeToolButton();
    completeHeroButtons();
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
    }

    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));
}


void MainWindow::completeToolButton()
{
    QMenu *menu = new QMenu(ui->toolButton);
    QMenu *newArenaMenu = new QMenu("New arena/draft", menu);

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
    ui->toolButton->setMenu(menu);
}


void MainWindow::confirmNewArenaDraft(QString hero)
{
    int ret = QMessageBox::question(this, tr("New arena: ") + hero,
                                   "Make sure you have already picked " + hero + " in hearthstone.\n"
                                   "Do you want to continue?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        qDebug() << "MainWindow: Nueva arena:" << hero;
        writeLog(tr("Menu: New arena: ") + hero);
        QString heroLog = Utility::heroToLogNumber(hero);
        arenaHandler->newArena(heroLog);
        draftHandler->beginDraft(heroLog);
    }
}


void MainWindow::readSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QPoint pos = settings.value("pos", QPoint(0,0)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    this->windowsFormation = none;
    resize(size);
    move(pos);
}


void MainWindow::writeSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeTabWidgets(event);
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
        if(newSize.height()>DIVIDE_TABS_V)  newWindowsFormation = _2X2;
        else                                newWindowsFormation = H2;
    }
    //H3
    else
    {
        newWindowsFormation = H3;
    }

    if(newWindowsFormation == windowsFormation) return;
    windowsFormation = newWindowsFormation;

    ui->tabWidget->hide();
    ui->tabWidgetH2->hide();
    ui->tabWidgetH3->hide();
    ui->tabWidgetV1->hide();
    ui->tabWidgetV2->hide();

    switch(windowsFormation)
    {
        case none:
        case H1:
            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            moveTabTo(ui->tabArena, ui->tabWidget);
            moveTabTo(ui->tabDeck, ui->tabWidget);
            moveTabTo(ui->tabEnemy, ui->tabWidget);
            moveTabTo(ui->tabLog, ui->tabWidget);
            ui->tabWidget->show();
            break;

        case H2:
            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            moveTabTo(ui->tabArena, ui->tabWidget);
            moveTabTo(ui->tabDeck, ui->tabWidget);
            moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
            moveTabTo(ui->tabLog, ui->tabWidgetH2);
            ui->tabWidget->show();
            ui->tabWidgetH2->show();
            break;

        case H3:
            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            moveTabTo(ui->tabArena, ui->tabWidget);
            moveTabTo(ui->tabDeck, ui->tabWidgetH2);
            moveTabTo(ui->tabEnemy, ui->tabWidgetH3);
            moveTabTo(ui->tabLog, ui->tabWidget);
            ui->tabWidget->show();
            ui->tabWidgetH2->show();
            ui->tabWidgetH3->show();
            break;

        case V2:
            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
            moveTabTo(ui->tabArena, ui->tabWidget);
            moveTabTo(ui->tabDeck, ui->tabWidgetV1);
            moveTabTo(ui->tabEnemy, ui->tabWidget);
            moveTabTo(ui->tabLog, ui->tabWidgetV1);
            ui->tabWidget->show();
            ui->tabWidgetV1->show();
            break;

        case _2X2:
            ui->arenaTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
            moveTabTo(ui->tabArena, ui->tabWidget);
            moveTabTo(ui->tabDeck, ui->tabWidgetV2);
            moveTabTo(ui->tabEnemy, ui->tabWidgetH2);
            moveTabTo(ui->tabLog, ui->tabWidgetV1);
            ui->tabWidget->show();
            ui->tabWidgetH2->show();
            ui->tabWidgetV1->show();
            ui->tabWidgetV2->show();
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


void MainWindow::writeLog(QString line)
{
    ui->textEdit->append(line);
}


void MainWindow::writeLogConnected(QString line)
{
    if(webUploader != NULL)     writeLog(line);
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


void MainWindow::uploadDeck()
{
    if(webUploader == NULL) return;

    QList<DeckCard> *deckCardList = deckHandler->getDeckComplete();
    webUploader->uploadDeck(deckCardList);
}


void MainWindow::test()
{

}

//TODO
//Consejos iniciales
//Crear archivo log con time.
//Uso en construido.
//Recuperar readingDeck.
//Automatizar inicio arena.
//Eliminar drafting de gameWatcher.
//Salida alternativa de drafting (+seguridad) en startGame

//BUGS CONOCIDOS
//Bug log tavern brawl (No hay [Bob] ---Register al entrar a tavern brawl)
//Draft. Cartas incorrectas.

//NUEVOS HEROES
//Evitar Asset hero powers (GameWatcher 201)
//Nuevo Json hearthArena
//Nuevo start draft menu
