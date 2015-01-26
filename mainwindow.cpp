#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logloader.h"
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    completeUI();

    readSettings();

    //Iniciamos la loglist con la arena huerfana
    arenaLogList.append(ArenaResult());

    logLoader = NULL;
    webUploader = NULL;

    createLogLoader();
}


MainWindow::~MainWindow()
{
    if(logLoader != NULL)   delete logLoader;
    delete webUploader;
    delete ui;
}


void MainWindow::createLogLoader()
{
    if(logLoader != NULL)   return;
    qint64 logSize;
    logLoader = new LogLoader(this, logSize);
    connect(logLoader, SIGNAL(synchronized()),
            this, SLOT(createWebUploader()));
    connect(logLoader, SIGNAL(seekChanged(qint64)),
            this, SLOT(showLogLoadProgress(qint64)));
    connect(logLoader, SIGNAL(sendLog(QString)),
            this, SLOT(writeLog(QString)));
    //GameWatcher signals reemit
    connect(logLoader, SIGNAL(newGameResult(GameResult)),
            this, SLOT(newGameResult(GameResult)));
    connect(logLoader, SIGNAL(newArena(QString)),
            this, SLOT(newArena(QString)));
    connect(logLoader, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
            this, SLOT(showArenaReward(int,int,bool,bool,bool)));
    connect(logLoader, SIGNAL(arenaRewardsComplete()),
            this, SLOT(uploadCurrentArenaRewards()));

    ui->progressBar->setMaximum(logSize/1000);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    setStatusBarMessage(tr("Loading log..."), 5000);
}


void MainWindow::createWebUploader()
{
    if(webUploader != NULL)   return;
    webUploader = new WebUploader(this);
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
    ui->progressBar->setVisible(false);
    setStatusBarMessage(tr("Loading Arena Mastery..."), 3000);
}


void MainWindow::completeUI()
{
    createTreeWidget();

    ui->tabWidget->setTabText(0, "Arena");
    ui->tabWidget->setTabText(1, "Log");
    ui->tabWidget->setCurrentIndex(0);
    ui->uploadButton->setEnabled(false);
    ui->updateButton->setEnabled(false);

    connect(ui->uploadButton, SIGNAL(clicked()),
            this, SLOT(uploadOldLog()));
    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(updateArenaFromWeb()));
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


void MainWindow::writeLog(QString line)
{
    ui->textEdit->append(line);
}


//    if(line.startsWith("(Filename:"))   return;
//    QRegularExpressionMatch *match = new QRegularExpressionMatch();
//    if(line.contains("CREATE_GAME"))
//    {
//        ui->textEdit->append("------------------------------");
//    }
//    else if(line.contains(QRegularExpression("Creating ID=4 CardID=HERO_(\\d+)"), match))
//    {
//        ui->textEdit->append("Hero1: " + match->captured(1));
//    }
//    else if(line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
//    {
//        ui->textEdit->append("Hero2: " + match->captured(1));
//    }
//    else if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYER_ID value=2"), match))
//    {
//        ui->textEdit->append("Name2: " + match->captured(1));
//    }
//    else if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYER_ID value=1"), match))
//    {
//        ui->textEdit->append("Name1: " + match->captured(1));
//    }
//    else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
//    {
//        ui->textEdit->append(match->captured(1) + " GO FIRST");
//    }
//    else if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
//    {
//        ui->textEdit->append(match->captured(1) + " WON");
//    }


void MainWindow::setStatusBarMessage(const QString &message, int timeout)
{
    ui->statusBar->showMessage(message, timeout);
}


void MainWindow::showLogLoadProgress(qint64 logSeek)
{
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
    else if(!webUploader->uploadNewGameResult(gameResult))
    {
        setRowColor(item, RED);
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
void MainWindow::newArenaRewards(ArenaRewards arenaRewards)
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
//    for(int i=0; i<arenaLogList.count(); i++)
//    {
//        ArenaResult arena = arenaLogList.at(i);
//        qDebug() << arena.playerHero;

//        for(int j=0; j<arena.gameResultList.count(); j++)
//        {
//            qDebug() << arena.gameResultList.at(j).playerHero << arena.gameResultList.at(j).enemyHero;
//        }
//        qDebug() << arena.arenaRewards.gold << arena.arenaRewards.dust << arena.arenaRewards.packs
//                     << arena.arenaRewards.plainCards << arena.arenaRewards.goldCards;
//    }

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
