#include "arenahandler.h"
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::Extended *ui) : QObject(parent)
{
    this->webUploader = NULL;
    this->deckHandler = deckHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->theme = ThemeWhite;

    completeUI();
}

ArenaHandler::~ArenaHandler()
{

}


void ArenaHandler::completeUI()
{
    createTreeWidget();

    ui->logTextEdit->setFrameShape(QFrame::NoFrame);
    ui->updateButton->setToolTip(tr("Reload latest arena from Arena Mastery."));

    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SLOT(openDonateWeb()));

    //Rewards UI
    ui->lineEditGold->setMinimumWidth(1);
    ui->lineEditArcaneDust->setMinimumWidth(1);
    ui->lineEditPack->setMinimumWidth(1);
    ui->lineEditPlainCard->setMinimumWidth(1);
    ui->lineEditGoldCard->setMinimumWidth(1);
    hideRewards();

    connect(ui->rewardsNoButton, SIGNAL(clicked(bool)),
            this, SLOT(hideRewards()));
    connect(ui->rewardsYesButton, SIGNAL(clicked(bool)),
            this, SLOT(hideRewards()));
    connect(ui->rewardsYesButton, SIGNAL(clicked(bool)),
            this, SLOT(uploadRewards()));
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
    treeWidget->setColumnWidth(4, 0);//60

    arenaHomeless = new QTreeWidgetItem(treeWidget);
    arenaHomeless->setExpanded(true);
    arenaHomeless->setText(0, "...");

    arenaCurrent = NULL;
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

    if(arenaMatch && webUploader!=NULL && webUploader->isConnected())
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
    emit pDebug("Show GameResult.");

    QTreeWidgetItem *item;

    if(!arenaMatch || arenaCurrent == NULL || arenaCurrentHero.compare(gameResult.playerHero)!=0)
    {
        item = new QTreeWidgetItem(arenaHomeless);
    }
    else
    {
        item = new QTreeWidgetItem(arenaCurrent);
        arenaCurrentGameList.append(gameResult);

        //Add game to score
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
        emit pDebug("Recalculate arena win/loses (1 game).");
    }

    item->setIcon(0, QIcon(":Images/" +
                           (gameResult.playerHero==""?("secretHunter"):("hero"+gameResult.playerHero))
                           + ".png"));
    item->setText(0, "vs");
    item->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter);
    item->setIcon(1, QIcon(":Images/" +
                           (gameResult.enemyHero==""?("secretHunter"):("hero"+gameResult.enemyHero))
                           + ".png"));
    if(!gameResult.enemyName.isEmpty())     item->setToolTip(1, gameResult.enemyName);
    item->setIcon(2, QIcon(gameResult.isFirst?":Images/first.png":":Images/coin.png"));
    item->setIcon(3, QIcon(gameResult.isWinner?":Images/win.png":":Images/lose.png"));

    setRowColor(item, WHITE);

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
    showArena(hero);

    if(webUploader==NULL || !webUploader->isConnected())
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
    emit pDebug("Show Arena.");

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

    arenaPreviousGameList.clear();
    arenaPreviousGameList = arenaCurrentGameList;
    arenaCurrentGameList = QList<GameResult>();
}


void ArenaHandler::showNoArena()
{
    if(noArena) return;

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->arenaTreeWidget);
    item->setText(0, "None");
    setRowColor(item, GREEN);
    arenaCurrent = NULL;
    arenaCurrentHero = "";
    noArena = true;

    arenaPreviousGameList.clear();
    arenaPreviousGameList = arenaCurrentGameList;
    arenaCurrentGameList = QList<GameResult>();

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


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    if(transparency != Transparent && theme == ThemeWhite) if(color == WHITE)  color = BLACK;

    for(int i=0;i<5;i++)
    {
        item->setForeground(i, QBrush(color));
    }
}


QColor ArenaHandler::getRowColor(QTreeWidgetItem *item)
{
    QColor color = item->foreground(0).color();

    if(color == BLACK)      color = WHITE;
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

    if(webUploader!=NULL && webUploader->isConnected())     webUploader->refresh();
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
        emit pDebug("Recalculate arena win/loses (Web sync).");
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


void ArenaHandler::hideRewards()
{
    ui->lineEditGold->hide();
    ui->lineEditArcaneDust->hide();
    ui->lineEditPack->hide();
    ui->lineEditPlainCard->hide();
    ui->lineEditGoldCard->hide();

    ui->labelGold->hide();
    ui->labelArcaneDust->hide();
    ui->labelPack->hide();
    ui->labelPlainCard->hide();
    ui->labelGoldCard->hide();

    ui->rewardsNoButton->hide();
    ui->rewardsYesButton->hide();
}


void ArenaHandler::showRewards()
{
    ui->lineEditGold->setText("");
    ui->lineEditGold->show();
    ui->lineEditGold->setFocus();
    ui->lineEditGold->selectAll();
    ui->lineEditArcaneDust->setText("0");
    ui->lineEditArcaneDust->show();
    ui->lineEditPack->setText("1");
    ui->lineEditPack->show();
    ui->lineEditPlainCard->setText("0");
    ui->lineEditPlainCard->show();
    ui->lineEditGoldCard->setText("0");
    ui->lineEditGoldCard->show();

    ui->labelGold->show();
    ui->labelArcaneDust->show();
    ui->labelPack->show();
    ui->labelPlainCard->show();
    ui->labelGoldCard->show();

    ui->rewardsNoButton->show();
    ui->rewardsYesButton->show();

    ui->tabWidget->setCurrentWidget(ui->tabArena);
}


void ArenaHandler::uploadRewards()
{
    ArenaRewards arenaRewards;

    arenaRewards.gold = ui->lineEditGold->text().toInt();
    arenaRewards.dust = ui->lineEditArcaneDust->text().toInt();
    arenaRewards.packs = ui->lineEditPack->text().toInt();
    arenaRewards.plainCards = ui->lineEditPlainCard->text().toInt();
    arenaRewards.goldCards = ui->lineEditGoldCard->text().toInt();

    if(webUploader!=NULL && webUploader->isConnected() && webUploader->uploadArenaRewards(arenaRewards))
    {
        enableRefreshButton(false);
    }
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

    if(transparency==Transparent)
    {
        ui->tabArena->setAttribute(Qt::WA_NoBackground);
        ui->tabArena->repaint();
        ui->tabLog->setAttribute(Qt::WA_NoBackground);
        ui->tabLog->repaint();

        ui->logTextEdit->setStyleSheet("QTextEdit{background-color: transparent; color: white;}");
    }
    else
    {
        ui->tabArena->setAttribute(Qt::WA_NoBackground, false);
        ui->tabArena->repaint();
        ui->tabLog->setAttribute(Qt::WA_NoBackground, false);
        ui->tabLog->repaint();

        ui->logTextEdit->setStyleSheet("");
    }

    if(theme == ThemeWhite)
    {
        //Change arenaTreeWidget normal color to (BLACK/WHITE)
        allToWhite();

        if(ui->updateButton->isEnabled())
        {
            ui->updateButton->setEnabled(false);
            if(webUploader!=NULL && webUploader->isConnected())     webUploader->refresh();
        }
    }
}


//Blanco opaco usa un theme diferente a los otros 3
void ArenaHandler::setTheme(Theme theme)
{
    this->theme = theme;

    if(transparency != Transparent)
    {
        //Change arenaTreeWidget normal color to (BLACK/WHITE)
        allToWhite();

        if(ui->updateButton->isEnabled())
        {
            ui->updateButton->setEnabled(false);
            if(webUploader!=NULL && webUploader->isConnected())     webUploader->refresh();
        }
    }
}

//Elimina la penultima arena si es igual a la ultima.
//La arena en log es la misma que la arena leida de web.
void ArenaHandler::removeDuplicateArena()
{
    emit pDebug("Try to remove dulicate arena.");

    if(arenaCurrentGameList.count() != arenaPreviousGameList.count())   return;

    for(int i=0; i<arenaCurrentGameList.count(); i++)
    {
        GameResult previous = arenaPreviousGameList[i];
        GameResult current = arenaCurrentGameList[i];

        if(previous.playerHero != current.playerHero)   return;
        if(previous.enemyHero != current.enemyHero)     return;
        if(previous.isFirst != current.isFirst)         return;
        if(previous.isWinner != current.isWinner)       return;
    }

    //Remove Previous Arena
    arenaPreviousGameList.clear();
    int indexCurrent = ui->arenaTreeWidget->indexOfTopLevelItem(arenaCurrent);
    QTreeWidgetItem *arenaRepeated = ui->arenaTreeWidget->topLevelItem(indexCurrent-1);
    if(arenaRepeated == arenaHomeless)  return;

    delete arenaRepeated;
    emit pDebug("Dulicate arena found and removed.");
}


