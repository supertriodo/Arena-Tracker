#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logloader.h"
#include <QtWidgets>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();

#ifdef QT_DEBUG
    writeLog(tr("MODE DEBUG"));
#endif

    //Iniciamos la loglist con la arena huerfana
    arenaLogList.append(ArenaResult());

    initCardsJson();

    webUploader = NULL;//NULL indica que estamos leyendo el old log (primera lectura)

    createDeckHandler();
    createEnemyHandHandler();
    createCardDownloader();
    createGameWatcher();
    createLogLoader();

    completeUI();
}


MainWindow::~MainWindow()
{
    delete logLoader;
    delete gameWatcher;
    delete webUploader;
    delete cardDownloader;
    delete enemyHandHandler;
    delete deckHandler;
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
}


void MainWindow::createDeckHandler()
{
    deckHandler = new DeckHandler(this, &cardsJson, ui);
    connect(deckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(deckHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLogConnected(QString)));
    connect(deckHandler, SIGNAL(sendStatusBarMessage(QString,int)),
            this, SLOT(setStatusBarMessageConnected(QString,int)));
}


void MainWindow::createEnemyHandHandler()
{
    enemyHandHandler = new EnemyHandHandler(this, &cardsJson, ui);
    connect(enemyHandHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyHandHandler, SIGNAL(sendLog(QString)),
            this, SLOT(writeLogConnected(QString)));
    connect(enemyHandHandler, SIGNAL(sendStatusBarMessage(QString,int)),
            this, SLOT(setStatusBarMessageConnected(QString,int)));
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
            this, SLOT(newGameResult(GameResult)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            this, SLOT(newArena(QString)));
    connect(gameWatcher, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
            this, SLOT(showArenaReward(int,int,bool,bool,bool)));
    connect(gameWatcher, SIGNAL(arenaRewardsComplete()),
            this, SLOT(uploadCurrentArenaRewards()));

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
}


void MainWindow::createLogLoader()
{
    logLoader = new LogLoader(this);
    connect(logLoader, SIGNAL(synchronized()),
            this, SLOT(createWebUploader()));
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
    setStatusBarMessage(tr("Loading log..."), 5000);
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this, &cardsJson);
    connect(webUploader, SIGNAL(loadedGameResult(GameResult)),
            this, SLOT(showGameResult(GameResult)));
    connect(webUploader, SIGNAL(loadedArena(QString)),
            this, SLOT(showArena(QString)));
    connect(webUploader, SIGNAL(reloadedGameResult(GameResult)),
            this, SLOT(reshowGameResult(GameResult)));
    connect(webUploader, SIGNAL(reloadedArena(QString)),
            this, SLOT(reshowArena(QString)));
    connect(webUploader, SIGNAL(synchronized()),
            this, SLOT(enableButtons()));
    connect(webUploader, SIGNAL(noArenaFound()),
            this, SLOT(showNoArena()));
    connect(webUploader, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
#ifndef QT_DEBUG //Si tenemos una arena en web podemos seguir testeando deck en construido
    connect(webUploader, SIGNAL(newDeckCard(QString,int)),
            deckHandler, SLOT(newDeckCard(QString,int)));
    connect(webUploader, SIGNAL(newWebDeckCardList()),
            this, SLOT(resetDeckFromWeb()));
#endif
    ui->progressBar->setVisible(false);
    resizeButtonsText();
    setStatusBarMessage(tr("Loading Arena Mastery..."), 3000);
}


void MainWindow::completeUI()
{
    createTreeWidget();

    ui->tabWidget->setCurrentIndex(0);
    ui->uploadButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    ui->uploadButton->setToolTip(tr("Upload"));
    ui->updateButton->setToolTip(tr("Refresh"));
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);
    ui->deckListWidget->setIconSize(QSize(218,35));
    ui->deckListWidget->setStyleSheet("background-color: transparent;");
    ui->enemyHandListWidget->setIconSize(QSize(218,35));
    ui->enemyHandListWidget->setStyleSheet("background-color: transparent;");
    QPalette palette;
    palette.setColor( QPalette::WindowText, Qt::white );
    ui->statusBar->setPalette( palette );

    resizeButton = new ResizeButton(this);
    ui->bottomLayout->addWidget(resizeButton);
    connect(resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));


    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");
    QFont font("Belwe Bd BT");
    font.setPointSize(12);
    ui->statusBar->setFont(font);


    connect(ui->uploadButton, SIGNAL(clicked()),
            this, SLOT(uploadOldLog()));
    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(updateArenaFromWeb()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SLOT(openDonateWeb()));
    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->minimizeButton, SIGNAL(clicked()),
            this, SLOT(showMinimized()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            deckHandler, SLOT(enableDeckButtons()));
    connect(ui->deckButtonMin, SIGNAL(clicked()),
            deckHandler, SLOT(cardTotalMin()));
    connect(ui->deckButtonPlus, SIGNAL(clicked()),
            deckHandler, SLOT(cardTotalPlus()));
}


void MainWindow::tabChanged(int index)
{
    if(index == tabDeck)            deckHandler->showCount();
    else if(index == tabEnemy)      enemyHandHandler->showCount();
    else                            ui->statusBar->showMessage("");
}


void MainWindow::createTreeWidget()
{
    QTreeWidget *treeWidget = ui->treeWidget;
    treeWidget->setColumnCount(5);
    treeWidget->setIconSize(QSize(32,32));
    treeWidget->setColumnWidth(0, 120);
    treeWidget->setColumnWidth(1, 80);
    treeWidget->setColumnWidth(2, 60);
    treeWidget->setColumnWidth(3, 60);
    treeWidget->setColumnWidth(4, 60);
    treeWidget->header()->close();

    arenaHomeless = new QTreeWidgetItem(treeWidget);
    arenaHomeless->setExpanded(true);
    arenaHomeless->setText(0, "...");

    arenaCurrent = NULL;
    arenaCurrentReward = NULL;
    arenaCurrentHero = "";
    noArena = false;
}


void MainWindow::enableButtons()
{
    ui->updateButton->setEnabled(true);

    ArenaResult arena = arenaLogList.first();
    int topLevelIndex = -1;

    //Arena huerfana sin partidas no cuenta
    if( arena.playerHero.isEmpty() &&
        arena.gameResultList.empty() &&
        arena.arenaRewards.packs == 0)
    {
        arenaLogList.removeFirst();

        if(arenaLogList.isEmpty())  topLevelIndex = -1;
        else                        topLevelIndex = 1;

    }
    else    topLevelIndex = 0;

    if(topLevelIndex != -1)
    {
        //Marcamos la arena para upload con los iconos de upload
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(topLevelIndex);
        if(topLevelIndex == 1)  item->setIcon(4, QIcon(":Images/upload64.png"));

        for(int i=0; i<item->childCount(); i++)
        {
            QTreeWidgetItem *child = item->child(i);
            if(child->icon(4).isNull()) child->setIcon(4, QIcon(":Images/upload32.png"));
        }

        ui->uploadButton->setEnabled(true);
    }
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
    resizeButtonsText();
    event->accept();
}


void MainWindow::resizeButtonsText()
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


void MainWindow::setStatusBarMessage(QString message, int timeout)
{
    ui->statusBar->showMessage("     " + message, timeout);
}


void MainWindow::setStatusBarMessageConnected(QString message, int timeout)
{
    if(webUploader != NULL)     setStatusBarMessage(message, timeout);
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


void MainWindow::newGameResult(GameResult gameResult)
{
    QTreeWidgetItem *item = showGameResult(gameResult);

    if(webUploader==NULL)
    {
        setRowColor(item, WHITE);
        if(item->parent() == arenaHomeless)
        {
            arenaLogList.first().gameResultList.append(gameResult);
        }
        else
        {
            arenaLogList.last().gameResultList.append(gameResult);
        }
    }
    else
    {
        QList<DeckCard> *deckCardList = deckHandler->getDeckComplete();
        bool uploadSuccess;
        if(deckCardList != NULL)    uploadSuccess=webUploader->uploadNewGameResult(gameResult,deckCardList);
        else                            uploadSuccess=webUploader->uploadNewGameResult(gameResult);

        if(!uploadSuccess)
        {
            setRowColor(item, RED);
        }
    }
}


QTreeWidgetItem *MainWindow::showGameResult(GameResult gameResult)
{
    QTreeWidgetItem *item;

    if(arenaCurrent == NULL || arenaCurrentHero.compare(gameResult.playerHero)!=0)
    {
        item = new QTreeWidgetItem(arenaHomeless);
    }
    else
    {
        item = new QTreeWidgetItem(arenaCurrent);
        if(gameResult.isWinner)
        {
            int wins = arenaCurrent->text(2).toInt() + 1;
            arenaCurrent->setText(2, QString::number(wins));
        }
        else
        {
            int loses = arenaCurrent->text(3).toInt() + 1;
            arenaCurrent->setText(3, QString::number(loses));
        }
        arenaCurrentGameList.append(gameResult);
    }

    item->setIcon(0, QIcon(":Images/hero" + gameResult.playerHero + ".png"));
    item->setText(0, "vs");
    item->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter);
    item->setIcon(1, QIcon(":Images/hero" + gameResult.enemyHero + ".png"));
    item->setIcon(2, QIcon(gameResult.isFirst?":Images/first.png":":Images/coin.png"));
    item->setIcon(3, QIcon(gameResult.isWinner?":Images/win.png":":Images/lose.png"));

    setRowColor(item, GREEN);

    return item;
}


void MainWindow::reshowGameResult(GameResult gameResult)
{
    if(arenaCurrent == NULL)    return;
    if(!isRowOk(arenaCurrent))  return;//Imposible

    setStatusBarMessage(tr("Loading Arena Mastery..."), 3000);
    for(int i=0; i<arenaCurrent->childCount(); i++)
    {
        QTreeWidgetItem *item = arenaCurrent->child(i);
        if((item->backgroundColor(0) == RED || item->backgroundColor(0) == WHITE) &&
            arenaCurrentGameList.at(i).enemyHero == gameResult.enemyHero &&
            arenaCurrentGameList.at(i).isFirst == gameResult.isFirst &&
            arenaCurrentGameList.at(i).isWinner == gameResult.isWinner)
        {
            setRowColor(item, GREEN);
            return;
        }
    }

    QTreeWidgetItem *item = showGameResult(gameResult);
    setRowColor(item, YELLOW);
}


bool MainWindow::newArenaUploadButton(QString &hero)
{
    bool result = webUploader->uploadNewArena(hero);

    if(result)
    {
        showArena(hero);
    }
    else
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setExpanded(true);
        item->setText(0, "Arena");
        item->setIcon(1, QIcon(":Images/hero" + hero + ".png"));
        item->setText(2, "0");
        item->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
        item->setText(3, "0");
        item->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);

        setRowColor(item, RED);

        ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount()-1,item);
        return false;
    }
    return true;
}


bool MainWindow::newArena(QString hero)
{
    deckHandler->reset();
    showArena(hero);

    if(webUploader==NULL)
    {
        setRowColor(arenaCurrent, WHITE);
        ArenaResult arenaResult;
        arenaResult.playerHero = hero;
        arenaLogList.append(arenaResult);
    }
    else if(!webUploader->uploadNewArena(hero))
    {
        setRowColor(arenaCurrent, RED);
        return false;
    }
    return true;
}


void MainWindow::showArena(QString hero)
{
    if(noArena)
    {
        QTreeWidgetItem *item = ui->treeWidget->takeTopLevelItem(ui->treeWidget->topLevelItemCount()-1);
        delete item;
        noArena = false;
    }

    arenaCurrentHero = QString(hero);
    arenaCurrent = new QTreeWidgetItem(ui->treeWidget);
    arenaCurrent->setExpanded(true);
    arenaCurrent->setText(0, "Arena");
    arenaCurrent->setIcon(1, QIcon(":Images/hero" + arenaCurrentHero + ".png"));
    arenaCurrent->setText(2, "0");
    arenaCurrent->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
    arenaCurrent->setText(3, "0");
    arenaCurrent->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);

    setRowColor(arenaCurrent, GREEN);

    arenaCurrentReward = NULL;
    arenaCurrentGameList.clear();
}


void MainWindow::showNoArena()
{
    ui->updateButton->setEnabled(true);

    if(noArena) return;
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, "NO Arena");
    setRowColor(item, GREEN);
    arenaCurrent = NULL;
    arenaCurrentReward = NULL;
    arenaCurrentHero = "";
    noArena = true;
}


void MainWindow::reshowArena(QString hero)
{
    setStatusBarMessage(tr("Loading Arena Mastery..."), 3000);
    ui->updateButton->setEnabled(true);

    if(arenaCurrent == NULL || arenaCurrentHero != hero)
    {
        showArena(hero);
        setRowColor(arenaCurrent, YELLOW);
        return;
    }
    setRowColor(arenaCurrent, GREEN);

    //Iniciamos a blanco todas las partidas que no esten en rojo
    for(int i=0; i<arenaCurrent->childCount(); i++)
    {
        if(isRowOk(arenaCurrent->child(i)))
        {
            setRowColor(arenaCurrent->child(i), WHITE);
        }
    }

    //Los rewards en verde (no se comprobaran)
    if(arenaCurrentReward != NULL)
    {
        setRowColor(arenaCurrentReward, GREEN);
    }
}



void MainWindow::uploadCurrentArenaRewards()
{
    if(arenaCurrentReward == NULL)
    {
        qDebug() << "MainWindow: "<< "ERROR: ArenaCurrentReward no existe al intentar crear los rewards para upload.";
        return;
    }


    ArenaRewards arenaRewards;

    arenaRewards.gold = arenaCurrentReward->text(0).toInt();
    arenaRewards.dust = arenaCurrentReward->text(1).toInt();
    arenaRewards.packs = arenaCurrentReward->text(2).toInt();
    arenaRewards.plainCards = arenaCurrentReward->text(3).toInt();
    arenaRewards.goldCards = arenaCurrentReward->text(4).toInt();

    if(webUploader==NULL)
    {
        setRowColor(arenaCurrentReward, WHITE);
        if(arenaCurrentReward->parent() == arenaHomeless)
        {
            arenaLogList.first().arenaRewards = arenaRewards;
        }
        else
        {
            arenaLogList.last().arenaRewards = arenaRewards;
        }
    }
    else if(!webUploader->uploadArenaRewards(arenaRewards))
    {
        setRowColor(arenaCurrentReward, RED);
    }
    else
    {
        setRowColor(arenaCurrentReward, GREEN);
    }
}


void MainWindow::showArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard)
{
    //No guardamos los rewards si la arena de la web no esta al dia (y no existe) y recibimos nuevas recompensas
    //para evitar modificar otras recompensas que tengamos en arena homeless del old log.
    if(arenaCurrent == NULL && noArena)    return;

    if(arenaCurrentReward == NULL)
    {
        arenaCurrentReward = new QTreeWidgetItem(arenaCurrent==NULL?arenaHomeless:arenaCurrent);
        arenaCurrentReward->setText(0, "0");
        arenaCurrentReward->setIcon(0, QIcon(":Images/gold.png"));
        arenaCurrentReward->setText(1, "0");
        arenaCurrentReward->setIcon(1, QIcon(":Images/arcanedust.png"));
        arenaCurrentReward->setText(2, "0");
        arenaCurrentReward->setIcon(2, QIcon(":Images/boosterpack.png"));
        arenaCurrentReward->setText(3, "0");
        arenaCurrentReward->setIcon(3, QIcon(":Images/cardplain.png"));
        arenaCurrentReward->setText(4, "0");
        arenaCurrentReward->setIcon(4, QIcon(":Images/cardgold.png"));
    }

    if(gold != 0)
    {
        gold += arenaCurrentReward->text(0).toInt();
        arenaCurrentReward->setText(0, QString::number(gold));
    }
    else if(dust != 0)
    {
        dust += arenaCurrentReward->text(1).toInt();
        arenaCurrentReward->setText(1, QString::number(dust));
    }
    else if(pack)
    {
        int packs = arenaCurrentReward->text(2).toInt() + 1;
        arenaCurrentReward->setText(2, QString::number(packs));
    }
    else if(plainCard)
    {
        int plainCards = arenaCurrentReward->text(3).toInt() + 1;
        arenaCurrentReward->setText(3, QString::number(plainCards));
    }
    else if(goldCard)
    {
        int goldCards = arenaCurrentReward->text(4).toInt() + 1;
        arenaCurrentReward->setText(4, QString::number(goldCards));
    }
}


//Se usa para upload del old log
void MainWindow::newArenaRewards(ArenaRewards &arenaRewards)
{
    if(arenaCurrent == NULL && !noArena)
    {
        //Solo puede fallar si no nos hemos conectado a la web
        qDebug() << "MainWindow: " << "ERROR: Esto es IMPOSIBLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        return;
    }
    if(arenaCurrentReward == NULL)
    {
        arenaCurrentReward = new QTreeWidgetItem(arenaCurrent==NULL?arenaHomeless:arenaCurrent);
        arenaCurrentReward->setText(0, QString::number(arenaRewards.gold));
        arenaCurrentReward->setIcon(0, QIcon(":Images/gold.png"));
        arenaCurrentReward->setText(1, QString::number(arenaRewards.dust));
        arenaCurrentReward->setIcon(1, QIcon(":Images/arcanedust.png"));
        arenaCurrentReward->setText(2, QString::number(arenaRewards.packs));
        arenaCurrentReward->setIcon(2, QIcon(":Images/boosterpack.png"));
        arenaCurrentReward->setText(3, QString::number(arenaRewards.plainCards));
        arenaCurrentReward->setIcon(3, QIcon(":Images/cardplain.png"));
        arenaCurrentReward->setText(4, QString::number(arenaRewards.goldCards));
        arenaCurrentReward->setIcon(4, QIcon(":Images/cardgold.png"));
    }

    //Imposible, venimos de upload old log
    if(webUploader==NULL)
    {
        setRowColor(arenaCurrentReward, WHITE);
        arenaLogList.last().arenaRewards = arenaRewards;
    }
    else if(!webUploader->uploadArenaRewards(arenaRewards))
    {
        setRowColor(arenaCurrentReward, RED);
    }
    else
    {
        setRowColor(arenaCurrentReward, GREEN);
    }
}


void MainWindow::setRowColor(QTreeWidgetItem *item, QColor color)
{
    for(int i=0;i<5;i++)
    {
        item->setBackgroundColor(i, color);
    }
}


bool MainWindow::isRowOk(QTreeWidgetItem *item)
{
    return (item->backgroundColor(0) != RED);
}


void MainWindow::updateArenaFromWeb()
{
    setStatusBarMessage(tr("Loading Arena Mastery..."), 3000);
    webUploader->checkArenaCurrentReload();
    ui->updateButton->setEnabled(false);
    QTimer::singleShot(5000, this, SLOT(enableRefreshButton()));
}


void MainWindow::enableRefreshButton()
{
    if(!ui->updateButton->isEnabled())
    {
        setStatusBarMessage(tr("No internet access to Arena Mastery"));
        ui->updateButton->setEnabled(true);
    }
}


void MainWindow::uploadOldLog()
{
    qDebug() << "MainWindow: " << "Uploading old log...";

    setStatusBarMessage(tr("Uploading to Arena Mastery..."), 3000);

    //Upload una arena por click
    ArenaResult arena = arenaLogList.first();
    arenaLogList.removeFirst();

    //Eliminamos el treeWidgetItem asociado
    if(arena.playerHero.isEmpty())
    {
        QList<QTreeWidgetItem *> itemList = ui->treeWidget->topLevelItem(0)->takeChildren();
        while(!itemList.empty())
        {
            QTreeWidgetItem *item = itemList.first();
            itemList.removeFirst();
            delete item;
        }
    }
    else
    {
        QTreeWidgetItem *item = ui->treeWidget->takeTopLevelItem(1);
        delete item;
    }

    //Creamos la arena
    if(arena.playerHero.isEmpty() || newArenaUploadButton(arena.playerHero))
    {
        for(int j=0; j<arena.gameResultList.count(); j++)
        {
            GameResult gameResult = arena.gameResultList.at(j);
            newGameResult(gameResult);
        }
        if(arena.arenaRewards.packs > 0)
        {
            newArenaRewards(arena.arenaRewards);
        }
    }

    if(arenaLogList.isEmpty())
    {
        ui->uploadButton->setEnabled(false);
    }
    //Marcamos la siguiente arena con los iconos de upload
    else
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(1);
        item->setIcon(4, QIcon(":Images/upload64.png"));

        for(int i=0; i<item->childCount(); i++)
        {
            QTreeWidgetItem *child = item->child(i);
            if(child->icon(4).isNull()) child->setIcon(4, QIcon(":Images/upload32.png"));
        }
    }
}


void MainWindow::openDonateWeb()
{
    QDesktopServices::openUrl(QUrl(
        "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&"
        "item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted"
        ));
}


void MainWindow::checkCardImage(QString code)
{
    QFileInfo *cardFile = new QFileInfo("./HSCards/" + code + ".png");

    if(!cardFile->exists())
    {
        //La bajamos de HearthHead
        cardDownloader->downloadWebImage(code);
    }
}


void MainWindow::redrawDownloadedCardImage(QString code)
{
    deckHandler->redrawDownloadedCardImage(code);
    enemyHandHandler->redrawDownloadedCardImage(code);
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
        this->close();
    }
}



//TODO
//Secretos
//Invocar esbirro
//Ataque a esbirro (desde esbirro o heroe)
//Ataque a cabeza (desde esbirro o heroe)
//Muerte esbirro enemigo
//Lanzar hechizo (con o sin objetivo)
//Iceblock
