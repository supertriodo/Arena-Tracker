#include "arenahandler.h"
#include "Utils/qcompressor.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler,
                           TrackobotUploader *trackobotUploader, PlanHandler *planHandler,
                           Ui::Extended *ui) : QObject(parent)
{
    this->webUploader = NULL;
    this->trackobotUploader = trackobotUploader;
    this->deckHandler = deckHandler;
    this->planHandler = planHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->theme = ThemeWhite;
    this->mouseInApp = false;
    this->connectedAM = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    completeUI();
}

ArenaHandler::~ArenaHandler()
{
    delete networkManager;
}


void ArenaHandler::completeUI()
{
    createTreeWidget();

    ui->logTextEdit->setFrameShape(QFrame::NoFrame);

    connect(ui->updateButton, SIGNAL(clicked()),
            this, SLOT(refresh()));
    connect(ui->webButton, SIGNAL(clicked()),
            this, SLOT(openTBProfile()));
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
    friendlyTreeItem = NULL;
    lastReplayUploaded = NULL;
}


void ArenaHandler::deselectRow()
{
    ui->arenaTreeWidget->setCurrentItem(NULL);
}


void ArenaHandler::changedRow(QTreeWidgetItem *current)
{
    if(trackobotUploader->isConnected() && ui->arenaTreeWidget->selectionMode()!=QAbstractItemView::NoSelection && replayLogsMap.contains(current))
    {
        ui->replayButton->setEnabled(true);
    }
    else
    {
        ui->replayButton->setEnabled(false);
    }
}


void ArenaHandler::setConnectedAM(bool value)
{
    this->connectedAM = value;
}


void ArenaHandler::replayLog()
{
    if(!trackobotUploader->isConnected() || lastReplayUploaded != NULL || !replayLogsMap.contains(ui->arenaTreeWidget->currentItem())) return;

    QString logFileName = replayLogsMap[ui->arenaTreeWidget->currentItem()];
    QRegularExpressionMatch match;

    if(logFileName.contains(QRegularExpression(".*\\.(\\w+)\\.arenatracker"), &match))
    {
        QString replayId = match.captured(1);
        emit pDebug("Opening: http://www.zerotoheroes.com/r/hearthstone/" + replayId);
        QDesktopServices::openUrl(QUrl("http://www.zerotoheroes.com/r/hearthstone/" + replayId));
        return;
    }

    QString url = "";
    if(logFileName.startsWith("DRAFT"))
    {
        url = "http://www.zerotoheroes.com/api/hearthstone/upload/draft/";
    }
    else if(logFileName.startsWith("ARENA"))
    {
        url = "http://www.zerotoheroes.com/api/hearthstone/upload/game/";
    }
    else
    {
        emit pDebug(logFileName + "isn't a draft or arena game.", Error);
        return;
    }


    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    logFileName = compressLog(logFileName);
    QFile *file = new QFile(Utility::gameslogPath() + "/" + logFileName);
    if(!file->open(QIODevice::ReadOnly))
    {
        emit pDebug("Failed to open " + Utility::gameslogPath() + "/" + logFileName);
        return;
    }

    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\"; filename=\""+file->fileName()+"\""));
    textPart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(textPart);

    url += "ArenaTracker/" + trackobotUploader->getUsername();
    url.replace("+", "%2B");    //Encode +
    QNetworkRequest request;
    request.setUrl(QUrl(url).adjusted(QUrl::FullyEncoded));
    QNetworkReply *reply = networkManager->post(request, multiPart);
    multiPart->setParent(reply);

    this->lastReplayUploaded = ui->arenaTreeWidget->currentItem();
    emit pDebug("Uploading replay " + replayLogsMap[lastReplayUploaded] + (logFileName=="temp.gz"?"(gzipped)":"") + " to " + "http://www.zerotoheroes.com");

    deselectRow();
    ui->arenaTreeWidget->setSelectionMode(QAbstractItemView::NoSelection);
}


QString ArenaHandler::compressLog(QString logFileName)
{
    QFile inFile(Utility::gameslogPath() + "/" + logFileName);
    if(!inFile.open(QIODevice::ReadOnly))   return logFileName;
    QByteArray uncompressedData = inFile.readAll();
    inFile.close();
    QByteArray compressedData;
    QCompressor::gzipCompress(uncompressedData, compressedData);

    QFile outFile(Utility::gameslogPath() + "/" + "temp.gz");
    if(outFile.exists())
    {
        outFile.remove();
        emit pDebug("temp.gz removed.");
    }
    if(!outFile.open(QIODevice::WriteOnly)) return logFileName;
    outFile.write(compressedData);
    outFile.close();

    emit pDebug(logFileName + " compressed on temp.gz");
    return "temp.gz";
}


void ArenaHandler::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    //Remove temp.gz
    QFile tempFile(Utility::gameslogPath() + "/" + "temp.gz");
    if(tempFile.exists())
    {
        tempFile.remove();
        emit pDebug("temp.gz removed.");
    }

    ui->arenaTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    if(lastReplayUploaded == NULL)
    {
        emit pDebug("LastReplayUploaded is NULL");
        return;
    }

    QString logFileName = replayLogsMap[lastReplayUploaded];
    QByteArray jsonReply = reply->readAll();
    if(jsonReply.isEmpty())
    {
        emit pDebug("No reply from zerotoheroes.com when uploading " + logFileName);
        lastReplayUploaded = NULL;
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonReply);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray replayIds = jsonObject.value("reviewIds").toArray();
    if(replayIds.isEmpty())
    {
        emit pDebug("No review id found in the reply " + jsonReply + " from zerotoheroes.com when uploading " + logFileName);
        lastReplayUploaded = NULL;
        return;
    }
    QString replayId = replayIds.first().toString();

    emit pDebug("Replay " + logFileName + " uploaded to " + "http://www.zerotoheroes.com/r/hearthstone/" + replayId);

    //Include replayId in fileName
    QStringList logFileNameSplit = logFileName.split(".");
    if(logFileNameSplit.length() != 2)
    {
        emit pDebug(logFileName + "has no extension correct format.");
        return;
    }
    QString newLogFileName = logFileNameSplit[0] + "." + replayId + "." + logFileNameSplit[1];
    if(QFile::rename(Utility::gameslogPath() + "/" + logFileName, Utility::gameslogPath() + "/" + newLogFileName))
    {
        emit pDebug("Replay " + logFileName + " renamed to " + newLogFileName);
        replayLogsMap[lastReplayUploaded] = newLogFileName;
        setRowColor(lastReplayUploaded, SEA_GREEN);
    }
    else
    {
        emit pDebug("Failed replay " + logFileName + " rename to " + newLogFileName, Error);
    }

    lastReplayUploaded = NULL;

    emit pDebug("Opening: http://www.zerotoheroes.com/r/hearthstone/" + replayId);
    QDesktopServices::openUrl(QUrl("http://www.zerotoheroes.com/r/hearthstone/" + replayId));
}


void ArenaHandler::linkDraftLogToArenaCurrent(QString logFileName)
{
    if(arenaCurrent != NULL && !logFileName.isEmpty())  replayLogsMap[arenaCurrent] = logFileName;
}


void ArenaHandler::setWebUploader(WebUploader *webUploader)
{
    this->webUploader = webUploader;
}


void ArenaHandler::newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName, qint64 startGameEpoch)
{
    QTreeWidgetItem *item = showGameResult(gameResult, loadingScreen);

    if(item != NULL && !logFileName.isEmpty())  replayLogsMap[item] = logFileName;

    //Arena Mastery upload
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

    //Trackobot upload
    if(trackobotUploader != NULL && planHandler != NULL &&
            (loadingScreen == arena || loadingScreen == constructed || loadingScreen == friendly))
    {
        trackobotUploader->uploadResult(gameResult, loadingScreen, startGameEpoch, planHandler->getJsonCardHistory());
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

        case friendly:
            emit pDebug("Create GameResult from friendly.");
            emit pLog(tr("Log: New friendly game."));

            if(friendlyTreeItem == NULL)
            {
                emit pDebug("Create Category friendly.");
                friendlyTreeItem = createTopLevelItem("Friendly", "", false);
            }

            item = new QTreeWidgetItem(friendlyTreeItem);
            updateWinLose(gameResult.isWinner, friendlyTreeItem);
        break;

        default:
        break;
    }

    return item;
}


QTreeWidgetItem *ArenaHandler::showGameResultLog(const QString &logFileName)
{
    QRegularExpressionMatch match;
    if(logFileName.contains(QRegularExpression("(\\w+) \\w+-\\d+ \\d+-\\d+ (\\w+)vs(\\w+) (WIN|LOSE) (FIRST|COIN)(\\.\\w+)?\\.arenatracker"), &match))
    {
        GameResult gameResult;
        LoadingScreenState loadingScreen = Utility::getLoadingScreenFromString(match.captured(1));
        gameResult.playerHero = Utility::heroToLogNumber(match.captured(2));
        gameResult.enemyHero = Utility::heroToLogNumber(match.captured(3));
        gameResult.isWinner = match.captured(4)=="WIN";
        gameResult.isFirst = match.captured(5)=="FIRST";

        QTreeWidgetItem *item = showGameResult(gameResult, loadingScreen);
        if(item != NULL)
        {
            replayLogsMap[item] = logFileName;
            if(!match.captured(6).isEmpty())    setRowColor(item, SEA_GREEN);

        }
        return item;
    }

    return NULL;
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
            if(replayLogsMap[item].contains(QRegularExpression("ARENA .*\\.\\w+\\.arenatracker")))
            {
                setRowColor(item, SEA_GREEN);
            }
            else
            {
                setRowColor(item, GREEN);
            }
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


void ArenaHandler::showArenaLog(const QString &logFileName)
{
    QRegularExpressionMatch match;
    if(logFileName.contains(QRegularExpression("DRAFT \\w+-\\d+ \\d+-\\d+ (\\w+)(\\.\\w+)?\\.arenatracker"), &match))
    {
        QString playerHero = Utility::heroToLogNumber(match.captured(1));
        showArena(playerHero);
        if(!match.captured(2).isEmpty())    setRowColor(this->arenaCurrent, SEA_GREEN);
        linkDraftLogToArenaCurrent(logFileName);
    }
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

    if(replayLogsMap[arenaCurrent].contains(QRegularExpression("DRAFT .*\\.\\w+\\.arenatracker")))
    {
        setRowColor(arenaCurrent, SEA_GREEN);
    }
    else
    {
        setRowColor(arenaCurrent, GREEN);
    }
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
            if(color == WHITE || color == RED)
            {
                setRowColor(arenaCurrent->child(i), RED);
            }
            else
            {
                arenaCurrentGameList[i].isWinner?wins++:loses++;
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


void ArenaHandler::openTBProfile()
{
    if(trackobotUploader == NULL)   return;
    trackobotUploader->openTBProfile();
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


QString ArenaHandler::getArenaCurrentGameLog()
{
    if(replayLogsMap.contains(arenaCurrent))    return replayLogsMap[arenaCurrent];
    else                                        return "";
}
