#include "arenahandler.h"
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, Ui::Extended *ui) : QObject(parent)
{
    this->webUploader = NULL;
    this->deckHandler = deckHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->theme = ThemeWhite;
    this->mouseInApp = false;

    completeUI();
}

ArenaHandler::~ArenaHandler()
{

}


void ArenaHandler::completeUI()
{
    createTreeWidget();

    ui->logTextEdit->setFrameShape(QFrame::NoFrame);

    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui->webButton, SIGNAL(clicked()),
            this, SLOT(openAMWeb()));
    connect(ui->replayButton, SIGNAL(clicked()),
            this, SLOT(replayLog()));
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

    treeWidget->setColumnWidth(0, 110);
    treeWidget->setColumnWidth(1, 50);
    treeWidget->setColumnWidth(2, 40);
    treeWidget->setColumnWidth(3, 40);
    treeWidget->setColumnWidth(4, 0);

    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(changedRow(QTreeWidgetItem*)));

    arenaHomeless = new QTreeWidgetItem(treeWidget);
    arenaHomeless->setExpanded(true);
    arenaHomeless->setText(0, "Arena");
    arenaHomeless->setHidden(true);

    arenaCurrent = NULL;
    arenaPrevious = NULL;
    arenaCurrentHero = "";
    noArena = false;

    for(int i=0; i<9; i++)  constructedTreeItem[i]=NULL;
    adventureTreeItem = NULL;
    tavernBrawlTreeItem = NULL;
}


void ArenaHandler::deselectRow()
{
    ui->arenaTreeWidget->setCurrentItem(NULL);
}


void ArenaHandler::changedRow(QTreeWidgetItem *current)
{
    if(replayLogsMap.contains(current))     ui->replayButton->setEnabled(true);
    else                                    ui->replayButton->setEnabled(false);
}


void ArenaHandler::replayLog()
{
    if(!replayLogsMap.contains(ui->arenaTreeWidget->currentItem())) return;

    QString logFileName = replayLogsMap[ui->arenaTreeWidget->currentItem()];
    //Verifical logFileName existe
    qDebug()<<Utility::gameslogPath() + "/" + logFileName;

    deselectRow();
}


void ArenaHandler::linkLogToDraft(QString logFileName)
{
    if(arenaCurrent != NULL && !logFileName.isEmpty())  replayLogsMap[arenaCurrent] = logFileName;
}


void ArenaHandler::setWebUploader(WebUploader *webUploader)
{
    this->webUploader = webUploader;
}


void ArenaHandler::newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName)
{
    QTreeWidgetItem *item = showGameResult(gameResult, loadingScreen);

    if(item != NULL && !logFileName.isEmpty())  replayLogsMap[item] = logFileName;

    if(loadingScreen == arena && webUploader!=NULL && webUploader->isConnected())
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
        else if(item != NULL)   setRowColor(item, RED);
    }
}


void ArenaHandler::updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem)
{
    emit pDebug("Recalculate win/loses (1 game).");
    if(isWinner)
    {
        int wins = topLevelItem->text(2).toInt() + 1;
        topLevelItem->setText(2, QString::number(wins));
    }
    else
    {
        int loses = topLevelItem->text(3).toInt() + 1;
        topLevelItem->setText(3, QString::number(loses));
    }
}


QTreeWidgetItem *ArenaHandler::createTopLevelItem(QString title, QString hero, bool addAtEnd)
{
    QTreeWidgetItem *item;

    if(addAtEnd)    item = new QTreeWidgetItem(ui->arenaTreeWidget);
    else
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(0, item);
    }

    item->setExpanded(true);
    item->setText(0, title);
    if(!hero.isEmpty())     item->setIcon(1, QIcon(":Images/hero" + hero + ".png"));
    item->setText(2, "0");
    item->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
    item->setText(3, "0");
    item->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);

    setRowColor(item, WHITE);

    return item;
}


QTreeWidgetItem *ArenaHandler::createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen)
{
    QTreeWidgetItem *item = NULL;
    int indexHero = gameResult.playerHero.toInt()-1;

    switch(loadingScreen)
    {
        case menu:
            emit pDebug("Avoid GameResult from menu.");
        break;

        case spectator:
            emit pDebug("Avoid GameResult from spectator.");
        break;

        case arena:
            emit pLog(tr("Log: New arena game."));

            if(arenaCurrent == NULL || arenaCurrentHero.compare(gameResult.playerHero)!=0)
            {
                emit pDebug("Create GameResult from arena in arenaHomeless.");

                if(arenaHomeless->isHidden())   arenaHomeless->setHidden(false);

                item = new QTreeWidgetItem(arenaHomeless);
            }
            else
            {
                emit pDebug("Create GameResult from arena in arenaCurrent.");
                item = new QTreeWidgetItem(arenaCurrent);
                arenaCurrentGameList.append(gameResult);
                updateWinLose(gameResult.isWinner, arenaCurrent);
            }
        break;

        case constructed:
            emit pDebug("Create GameResult from constructed with hero " + gameResult.playerHero + ".");
            emit pLog(tr("Log: New ranked game."));

            if(indexHero<0||indexHero>8)  return NULL;

            if(constructedTreeItem[indexHero] == NULL)
            {
                emit pDebug("Create Category constructed[" + QString::number(indexHero) + "].");
                constructedTreeItem[indexHero] = createTopLevelItem("Ranked", gameResult.playerHero, false);
            }

            item = new QTreeWidgetItem(constructedTreeItem[indexHero]);
            updateWinLose(gameResult.isWinner, constructedTreeItem[indexHero]);
        break;

        case adventure:
            emit pDebug("Create GameResult from adventure.");
            emit pLog(tr("Log: New solo game."));

            if(adventureTreeItem == NULL)
            {
                emit pDebug("Create Category adventure.");
                adventureTreeItem = createTopLevelItem("Solo", "", false);
            }

            item = new QTreeWidgetItem(adventureTreeItem);
            updateWinLose(gameResult.isWinner, adventureTreeItem);
        break;

        case tavernBrawl:
            emit pDebug("Create GameResult from tavern brawl.");
            emit pLog(tr("Log: New tavern brawl game."));

            if(tavernBrawlTreeItem == NULL)
            {
                emit pDebug("Create Category tavern brawl.");
                tavernBrawlTreeItem = createTopLevelItem("Brawl", "", false);
            }

            item = new QTreeWidgetItem(tavernBrawlTreeItem);
            updateWinLose(gameResult.isWinner, tavernBrawlTreeItem);
        break;
    }

    return item;
}


QTreeWidgetItem *ArenaHandler::showGameResult(GameResult gameResult, LoadingScreenState loadingScreen)
{
    emit pDebug("Show GameResult.");

    QTreeWidgetItem *item = createGameInCategory(gameResult, loadingScreen);
    if(item == NULL)    return NULL;

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

    QTreeWidgetItem *item = showGameResult(gameResult, arena);
    if(item != NULL)    setRowColor(item, YELLOW);
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
        //La gameList se paso a previous en el showNoArena
    }
    else
    {
        //Pasamos la gameList a previous
        arenaPreviousGameList.clear();
        arenaPreviousGameList = arenaCurrentGameList;
        arenaCurrentGameList = QList<GameResult>();
        arenaPrevious = arenaCurrent;
    }

    arenaCurrentHero = QString(hero);
    arenaCurrent = createTopLevelItem("Arena", arenaCurrentHero, true);
}


void ArenaHandler::showNoArena()
{
    if(noArena) return;

    emit pDebug("Show no arena.");

    //Pasamos la gameList a previous
    arenaPreviousGameList.clear();
    arenaPreviousGameList = arenaCurrentGameList;
    arenaCurrentGameList = QList<GameResult>();
    arenaPrevious = arenaCurrent;

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->arenaTreeWidget);
    item->setText(0, "None");
    setRowColor(item, GREEN);
    arenaCurrent = NULL;
    arenaCurrentHero = "";
    noArena = true;
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
    if(theme == ThemeWhite && (transparency != Transparent || mouseInApp))
        if(color == WHITE)  color = BLACK;

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


void ArenaHandler::openAMWeb()
{
    QDesktopServices::openUrl(QUrl(WEB_URL));
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
    bool transparencyChanged = this->transparency != value;
    this->transparency = value;

    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabArena->setAttribute(Qt::WA_NoBackground);
        ui->tabArena->repaint();
    }
    else
    {
        ui->tabArena->setAttribute(Qt::WA_NoBackground, false);
        ui->tabArena->repaint();
    }

    //Habra que cambiar los colores en theme blanco si:
    //1) La transparencia se ha cambiado
    //2) El raton ha salido/entrado y estamos en transparente
    if(theme == ThemeWhite &&
            (transparencyChanged || this->transparency == Transparent ))
    {
        //Change arenaTreeWidget normal color to (BLACK/WHITE)
        allToWhite();

        //Solo recargamos arenaMastery si la transparencia se ha cambiado, no cuando el raton entra y sale.
        if(transparencyChanged && ui->updateButton->isEnabled())
        {
            ui->updateButton->setEnabled(false);
            if(webUploader!=NULL && webUploader->isConnected())     webUploader->refresh();
        }
    }
}


void ArenaHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    setTransparency(this->transparency);
}


//Blanco opaco usa un theme diferente a los otros 3
void ArenaHandler::setTheme(Theme theme)
{
    this->theme = theme;

    //Change arenaTreeWidget normal color to (BLACK/WHITE)
    allToWhite();

    if(ui->updateButton->isEnabled())
    {
        ui->updateButton->setEnabled(false);
        if(webUploader!=NULL && webUploader->isConnected())     webUploader->refresh();
    }
}

//Elimina la penultima arena si es igual a la ultima.
//La arena en log es la misma que la arena leida de web.
void ArenaHandler::removeDuplicateArena()
{
    emit pDebug("Try to remove dulicate arena.");

    if(noArena) return;
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
    if(arenaCurrent == NULL)                return;
    if(arenaPrevious == NULL)               return;
    if(arenaPrevious->text(0) != "Arena")   return;

    delete arenaPrevious;
    arenaPrevious = NULL;
    emit pDebug("Dulicate arena found and removed.");
}


void ArenaHandler::linkLogsToWebGames()
{
    emit pDebug("Link logs to web games.");

    QRegularExpressionMatch match;

    QDir dir(Utility::gameslogPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);

    //Games
    QStringList arenaFilter;
    arenaFilter << "ARENA*.arenatracker";
    dir.setNameFilters(arenaFilter);

    QStringList files = dir.entryList().mid(0, arenaCurrentGameList.count());
    int gameIndex = arenaCurrentGameList.count();
    foreach(QString file, files)
    {
        gameIndex --;

        if(!file.contains(QRegularExpression("ARENA .* (\\w+)vs(\\w+) (WIN|LOSE).arenatracker"), &match)) return;
        QString playerHero = match.captured(1);
        QString enemyHero = match.captured(2);
        bool isWinner = match.captured(3) == "WIN";

        GameResult gameWeb = arenaCurrentGameList[gameIndex];
        if(Utility::heroStringFromLogNumber(gameWeb.playerHero) != playerHero)  return;
        if(Utility::heroStringFromLogNumber(gameWeb.enemyHero) != enemyHero)  return;
        if(gameWeb.isWinner != isWinner)    return;

        replayLogsMap[arenaCurrent->child(gameIndex)] = file;
    }

    //Draft
    QStringList draftFilter;
    draftFilter << "DRAFT*.arenatracker";
    dir.setNameFilters(draftFilter);

    if(dir.entryList().isEmpty())   return;
    QString file = dir.entryList().first();

    if(!file.contains(QRegularExpression("DRAFT .* (\\w+).arenatracker"), &match)) return;
    QString playerHero = match.captured(1);

    if(Utility::heroStringFromLogNumber(arenaCurrentGameList[0].playerHero) != playerHero)  return;

    replayLogsMap[arenaCurrent] = file;
}



