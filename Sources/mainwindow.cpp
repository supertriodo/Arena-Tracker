#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef QT_DEBUG
    writeLog(tr("MODE DEBUG"));
#endif

    readSettings();
    initCardsJson();
    completeUI();

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
    gameWatcher->setDeckRead();
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
    connect(draftHandler, SIGNAL(newDeckCard(QString,int)),
            deckHandler, SLOT(newDeckCard(QString,int)));
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

    connect(gameWatcher, SIGNAL(startGame()),
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

    connect(gameWatcher,SIGNAL(beginDraft(QString)),
            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher,SIGNAL(endDraft()),
            draftHandler, SLOT(endDraft()));
    connect(gameWatcher,SIGNAL(pauseDraft()),
            draftHandler, SLOT(pauseDraft()));
    connect(gameWatcher,SIGNAL(resumeDraft()),
            draftHandler, SLOT(resumeDraft()));
}


void MainWindow::createLogLoader()
{
    logLoader = new LogLoader(this);
    connect(logLoader, SIGNAL(synchronized()),
            this, SLOT(createWebUploader()));
    connect(logLoader, SIGNAL(synchronized()),
            gameWatcher, SLOT(setSynchronized()));
    connect(logLoader, SIGNAL(synchronized()),
            secretsHandler, SLOT(setSynchronized()));
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
    resizeArenaButtonsText();

    //Test
#ifdef QT_DEBUG
    test();
#endif
}


void MainWindow::completeUI()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->tabDraft->setVisible(false);

    resizeButton = new ResizeButton(this);
    ui->bottomLayout->addWidget(resizeButton);
    connect(resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));


    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");

    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->minimizeButton, SIGNAL(clicked()),
            this, SLOT(showMinimized()));
}


void MainWindow::readSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QPoint pos = settings.value("pos", QPoint(0,0)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
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
    resizeArenaButtonsText();
    event->accept();
}


void MainWindow::resizeArenaButtonsText()
{
    if(this->width() > 400)
    {
        ui->updateButton->setText("Refresh");
        ui->uploadButton->setText("Upload");
    }
    else
    {
        ui->updateButton->setText("");
        ui->uploadButton->setText("");
    }
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

#ifdef Q_OS_LINUX
            //Force begin draft
            if(event->key() == Qt::Key_1)   {deckHandler->reset();gameWatcher->newDraft("01");}
            if(event->key() == Qt::Key_2)   {deckHandler->reset();gameWatcher->newDraft("02");}
            if(event->key() == Qt::Key_3)   {deckHandler->reset();gameWatcher->newDraft("03");}
            if(event->key() == Qt::Key_4)   {deckHandler->reset();gameWatcher->newDraft("04");}
            if(event->key() == Qt::Key_5)   {deckHandler->reset();gameWatcher->newDraft("05");}
            if(event->key() == Qt::Key_6)   {deckHandler->reset();gameWatcher->newDraft("06");}
            if(event->key() == Qt::Key_7)   {deckHandler->reset();gameWatcher->newDraft("07");}
            if(event->key() == Qt::Key_8)   {deckHandler->reset();gameWatcher->newDraft("08");}
            if(event->key() == Qt::Key_9)   {deckHandler->reset();gameWatcher->newDraft("09");}
#endif
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
//Varios tab a la vez
//Crear archivo log con time.
//Revisar delays de secretos 1 a 1.

//BUGS CONOCIDOS
//Bug log tavern brawl (No hay [Bob] ---Register al entrar a tavern brawl)
//Draft. Cartas incorrectas.
//Bug cientifico loco muere y salta avenge.

//NUEVOS HEROES
//Evitar Asset hero powers (GameWatcher 201)
//Nuevo Json hearthArena
//Nuevo start draft manual (MainWindow 453)
