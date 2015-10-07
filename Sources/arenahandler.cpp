#include "arenahandler.h"
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::MainWindow *ui) : QObject(parent)
{
    this->webUploader = NULL;
    this->deckHandler = deckHandler;
    this->ui = ui;
    this->transparency = Never;

    completeUI();
}

ArenaHandler::~ArenaHandler()
{

}


void ArenaHandler::completeUI()
{
    createTreeWidget();

    ui->updateButton->setToolTip(tr("Refresh"));
    ui->arenaTreeWidget->setFrameShape(QFrame::NoFrame);

    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SLOT(openDonateWeb()));
}


void ArenaHandler::createTreeWidget()
{
    QTreeWidget *treeWidget = ui->arenaTreeWidget;
    treeWidget->setColumnCount(5);
    treeWidget->setIconSize(QSize(32,32));
    treeWidget->setColumnWidth(0, 110);//120
    treeWidget->setColumnWidth(1, 50);//80
    treeWidget->setColumnWidth(2, 40);//60
    treeWidget->setColumnWidth(3, 40);//60
    treeWidget->setColumnWidth(4, 1);//60
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


void ArenaHandler::newGameResult(GameResult gameResult, bool arenaMatch)
{
    QTreeWidgetItem *item = showGameResult(gameResult, arenaMatch);

    if(arenaMatch && webUploader!=NULL)
    {
        QList<DeckCard> *deckCardList = deckHandler->getDeckComplete();
        bool uploadSuccess;
        if(deckCardList != NULL)    uploadSuccess=webUploader->uploadNewGameResult(gameResult,deckCardList);
        else                            uploadSuccess=webUploader->uploadNewGameResult(gameResult);

        if(uploadSuccess)
        {
            enableRefreshButton(false);
            currentArenaToWhite();
        }
        else                setRowColor(item, RED);
    }
}


QTreeWidgetItem *ArenaHandler::showGameResult(GameResult gameResult, bool arenaMatch)
{
    QTreeWidgetItem *item;

    if(!arenaMatch || arenaCurrent == NULL || arenaCurrentHero.compare(gameResult.playerHero)!=0)
    {
        item = new QTreeWidgetItem(arenaHomeless);
    }
    else
    {
        item = new QTreeWidgetItem(arenaCurrent);
        arenaCurrentGameList.append(gameResult);
    }

    item->setIcon(0, QIcon(":Images/" +
                           (gameResult.playerHero==""?("secretHunter"):("hero"+gameResult.playerHero))
                           + ".png"));
    item->setText(0, "vs");
    item->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter);
    item->setIcon(1, QIcon(":Images/" +
                           (gameResult.enemyHero==""?("secretHunter"):("hero"+gameResult.enemyHero))
                           + ".png"));
    item->setToolTip(1, gameResult.enemyName);
    item->setIcon(2, QIcon(gameResult.isFirst?":Images/first.png":":Images/coin.png"));
    item->setIcon(3, QIcon(gameResult.isWinner?":Images/win.png":":Images/lose.png"));

    setRowColor(item, WHITE);

    arenaCurrentReward = NULL;

    return item;
}


void ArenaHandler::reshowGameResult(GameResult gameResult)
{
    if(arenaCurrent == NULL)    return;
    if(!isRowOk(arenaCurrent))  return;//Imposible

    for(int i=0; i<arenaCurrent->childCount(); i++)
    {
        QTreeWidgetItem *item = arenaCurrent->child(i);
        QColor statusColor = getRowColor(item);
        if((statusColor == RED || statusColor == WHITE || statusColor == TRANSPARENT) &&
            arenaCurrentGameList.at(i).enemyHero == gameResult.enemyHero &&
            arenaCurrentGameList.at(i).isFirst == gameResult.isFirst &&
            arenaCurrentGameList.at(i).isWinner == gameResult.isWinner)
        {
            setRowColor(item, GREEN);
            return;
        }
    }

    QTreeWidgetItem *item = showGameResult(gameResult, true);
    setRowColor(item, YELLOW);
}


bool ArenaHandler::newArena(QString hero)
{
    deckHandler->reset();
    showArena(hero);

    if(webUploader==NULL)
    {
    }
    else if(!webUploader->uploadNewArena(hero))
    {
        setRowColor(arenaCurrent, RED);
        return false;
    }
    else    enableRefreshButton(false);
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

    setRowColor(arenaCurrent, WHITE);

    arenaCurrentReward = NULL;
    arenaCurrentGameList.clear();
}


void ArenaHandler::showNoArena()
{
    if(noArena) return;

    //Add last game to score
    if(arenaCurrent!=NULL && !arenaCurrentGameList.isEmpty())
    {
        if(arenaCurrentGameList.last().isWinner)
        {
            int wins = arenaCurrent->text(2).toInt() + 1;
            arenaCurrent->setText(2, QString::number(wins));
        }
        else
        {
            int loses = arenaCurrent->text(3).toInt() + 1;
            arenaCurrent->setText(3, QString::number(loses));
        }
        emit pDebug("Recalculate arena win/loses (last game).");
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->arenaTreeWidget);
    item->setText(0, "No arena");
    setRowColor(item, GREEN);
    arenaCurrent = NULL;
    arenaCurrentReward = NULL;
    arenaCurrentHero = "";
    arenaCurrentGameList.clear();
    noArena = true;

    emit pDebug("Show no arena.");
}


void ArenaHandler::reshowArena(QString hero)
{
    if(arenaCurrent == NULL || arenaCurrentHero != hero)
    {
        showArena(hero);
        setRowColor(arenaCurrent, YELLOW);
        return;
    }
    setRowColor(arenaCurrent, GREEN);
}


void ArenaHandler::uploadCurrentArenaRewards()
{
    if(arenaCurrentReward == NULL)
    {
        qDebug() << "MainWindow: "<< "ERROR: ArenaCurrentReward no existe al intentar crear los rewards para upload.";
        emit pDebug("ArenaCurrentReward doesn't exist when uploading rewards.",Error);
        return;
    }

    deckHandler->reset();

    ArenaRewards arenaRewards;

    arenaRewards.gold = arenaCurrentReward->text(0).toInt();
    arenaRewards.dust = arenaCurrentReward->text(1).toInt();
    arenaRewards.packs = arenaCurrentReward->text(2).toInt();
    arenaRewards.plainCards = arenaCurrentReward->text(3).toInt();
    arenaRewards.goldCards = arenaCurrentReward->text(4).toInt();

    if(webUploader==NULL)
    {
        setRowColor(arenaCurrentReward, WHITE);
    }
    else if(!webUploader->uploadArenaRewards(arenaRewards))
    {
        setRowColor(arenaCurrentReward, RED);
    }
    else
    {
        setRowColor(arenaCurrentReward, WHITE);
        enableRefreshButton(false);
        currentArenaToWhite();
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


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    if(transparency == Always)
    {
        if(color == GREEN)  color = LIMEGREEN;

        for(int i=0;i<5;i++)
        {
            item->setBackground(i, QBrush());
            item->setForeground(i, QBrush(color));
        }
    }
    else
    {
        for(int i=0;i<5;i++)
        {
            item->setBackground(i, QBrush(color));
            item->setForeground(i, QBrush(BLACK));
        }
    }
}


QColor ArenaHandler::getRowColor(QTreeWidgetItem *item)
{
    QColor color;

    if(transparency == Always)  color = item->foreground(0).color();
    else    color = item->backgroundColor(0);

    if(color == LIMEGREEN)  color = GREEN;
    return color;
}


bool ArenaHandler::isRowOk(QTreeWidgetItem *item)
{
    return (getRowColor(item) != RED);
}


void ArenaHandler::refresh()
{
    currentArenaToWhite();

    emit pDebug("\nRefresh Button.");
    ui->updateButton->setEnabled(false);
    webUploader->refresh();
}


void ArenaHandler::currentArenaToWhite()
{
    if(arenaCurrent != NULL)
    {
        setRowColor(arenaCurrent, WHITE);

        //Iniciamos a blanco todas las partidas que no esten en rojo
        for(int i=0; i<arenaCurrent->childCount(); i++)
        {
            setRowColor(arenaCurrent->child(i), WHITE);
        }
    }
}


void ArenaHandler::syncArenaCurrent()
{
    int wins = 0;
    int loses = 0;

    if(arenaCurrent != NULL)
    {
        for(int i=0; i<arenaCurrent->childCount(); i++)
        {
            QColor color = getRowColor(arenaCurrent->child(i));
            if(color == GREEN || color == YELLOW)
            {
                arenaCurrentGameList[i].isWinner?wins++:loses++;
            }
            else
            {
                setRowColor(arenaCurrent->child(i), RED);
            }
        }

        arenaCurrent->setText(2, QString::number(wins));
        arenaCurrent->setText(3, QString::number(loses));
        emit pDebug("Recalculate arena win/loses.");
    }
}


void ArenaHandler::enableRefreshButton(bool enable)
{
    ui->updateButton->setEnabled(enable);
}


void ArenaHandler::openDonateWeb()
{
    QDesktopServices::openUrl(QUrl(
        "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&"
        "item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted"
        ));
}


bool ArenaHandler::isNoArena()
{
    return noArena;
}


void ArenaHandler::allToWhite()
{
    int numTopItems = ui->arenaTreeWidget->topLevelItemCount();
    for(int i=0; i<numTopItems; i++)
    {
        QTreeWidgetItem * item = ui->arenaTreeWidget->topLevelItem(i);
        int numItems = item->childCount();
        for(int j=0; j<numItems; j++)
        {
            setRowColor(item->child(j), WHITE);
        }
        setRowColor(item, WHITE);
    }
}


void ArenaHandler::setTransparency(Transparency value)
{
    this->transparency = value;

    if(transparency==Always)
    {
        allToWhite();

        if(ui->updateButton->isEnabled())
        {
            ui->updateButton->setEnabled(false);
            webUploader->refresh();
        }

        ui->arenaTreeWidget->setStyleSheet("background-color: transparent;");
        ui->tabArena->setAttribute(Qt::WA_NoBackground);
        ui->tabArena->repaint();

        ui->logTextEdit->setStyleSheet("background-color: transparent; color: white;");
        ui->tabLog->setAttribute(Qt::WA_NoBackground);
        ui->tabLog->repaint();
    }
    else
    {
        allToWhite();

        if(ui->updateButton->isEnabled())
        {
            ui->updateButton->setEnabled(false);
            webUploader->refresh();
        }

        ui->arenaTreeWidget->setStyleSheet("");
        ui->tabArena->setAttribute(Qt::WA_NoBackground, false);
        ui->tabArena->repaint();

        ui->logTextEdit->setStyleSheet("");
        ui->tabLog->setAttribute(Qt::WA_NoBackground, false);
        ui->tabLog->repaint();
    }
}

