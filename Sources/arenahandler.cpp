#include "arenahandler.h"
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::MainWindow *ui) : QObject(parent)
{
    this->webUploader = NULL;
    this->deckHandler = deckHandler;
    this->ui = ui;

    completeUI();

    //Iniciamos la loglist con la arena huerfana
    arenaLogList.append(ArenaResult());
}

ArenaHandler::~ArenaHandler()
{

}


void ArenaHandler::completeUI()
{
    createTreeWidget();

    ui->uploadButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    ui->uploadButton->setToolTip(tr("Upload"));
    ui->updateButton->setToolTip(tr("Refresh"));

    connect(ui->uploadButton, SIGNAL(clicked()),
            this, SLOT(uploadOldLog()));
    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(updateArenaFromWeb()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SLOT(openDonateWeb()));
}


void ArenaHandler::createTreeWidget()
{
    QTreeWidget *treeWidget = ui->arenaTreeWidget;
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


void ArenaHandler::setWebUploader(WebUploader *webUploader)
{
    this->webUploader = webUploader;
}


void ArenaHandler::enableButtons()
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
        QTreeWidgetItem *item = ui->arenaTreeWidget->topLevelItem(topLevelIndex);
        if(topLevelIndex == 1)  item->setIcon(4, QIcon(":Images/upload64.png"));

        for(int i=0; i<item->childCount(); i++)
        {
            QTreeWidgetItem *child = item->child(i);
            if(child->icon(4).isNull()) child->setIcon(4, QIcon(":Images/upload32.png"));
        }

        ui->uploadButton->setEnabled(true);
    }
}


void ArenaHandler::newGameResult(GameResult gameResult)
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


QTreeWidgetItem *ArenaHandler::showGameResult(GameResult gameResult)
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


void ArenaHandler::reshowGameResult(GameResult gameResult)
{
    if(arenaCurrent == NULL)    return;
    if(!isRowOk(arenaCurrent))  return;//Imposible

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


bool ArenaHandler::newArenaUploadButton(QString &hero)
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

        ui->arenaTreeWidget->insertTopLevelItem(ui->arenaTreeWidget->topLevelItemCount()-1,item);
        return false;
    }
    return true;
}


bool ArenaHandler::newArena(QString hero)
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


void ArenaHandler::showArena(QString hero)
{
    if(noArena)
    {
        QTreeWidgetItem *item = ui->arenaTreeWidget->takeTopLevelItem(ui->arenaTreeWidget->topLevelItemCount()-1);
        delete item;
        noArena = false;
    }

    arenaCurrentHero = QString(hero);
    arenaCurrent = new QTreeWidgetItem(ui->arenaTreeWidget);
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


void ArenaHandler::showNoArena()
{
    ui->updateButton->setEnabled(true);

    if(noArena) return;
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->arenaTreeWidget);
    item->setText(0, "NO Arena");
    setRowColor(item, GREEN);
    arenaCurrent = NULL;
    arenaCurrentReward = NULL;
    arenaCurrentHero = "";
    noArena = true;
}


void ArenaHandler::reshowArena(QString hero)
{
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


void ArenaHandler::uploadCurrentArenaRewards()
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


void ArenaHandler::showArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard)
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
void ArenaHandler::newArenaRewards(ArenaRewards &arenaRewards)
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


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    for(int i=0;i<5;i++)
    {
        item->setBackgroundColor(i, color);
    }
}


bool ArenaHandler::isRowOk(QTreeWidgetItem *item)
{
    return (item->backgroundColor(0) != RED);
}


void ArenaHandler::updateArenaFromWeb()
{
    webUploader->checkArenaCurrentReload();
    ui->updateButton->setEnabled(false);
    QTimer::singleShot(5000, this, SLOT(enableRefreshButton()));
}


void ArenaHandler::enableRefreshButton()
{
    if(!ui->updateButton->isEnabled())
    {
        ui->updateButton->setEnabled(true);
    }
}


void ArenaHandler::uploadOldLog()
{
    qDebug() << "MainWindow: " << "Uploading old log...";

    //Upload una arena por click
    ArenaResult arena = arenaLogList.first();
    arenaLogList.removeFirst();

    //Eliminamos el treeWidgetItem asociado
    if(arena.playerHero.isEmpty())
    {
        QList<QTreeWidgetItem *> itemList = ui->arenaTreeWidget->topLevelItem(0)->takeChildren();
        while(!itemList.empty())
        {
            QTreeWidgetItem *item = itemList.first();
            itemList.removeFirst();
            delete item;
        }
    }
    else
    {
        QTreeWidgetItem *item = ui->arenaTreeWidget->takeTopLevelItem(1);
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
        QTreeWidgetItem *item = ui->arenaTreeWidget->topLevelItem(1);
        item->setIcon(4, QIcon(":Images/upload64.png"));

        for(int i=0; i<item->childCount(); i++)
        {
            QTreeWidgetItem *child = item->child(i);
            if(child->icon(4).isNull()) child->setIcon(4, QIcon(":Images/upload32.png"));
        }
    }
}


void ArenaHandler::openDonateWeb()
{
    QDesktopServices::openUrl(QUrl(
        "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&"
        "item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted"
        ));
}
