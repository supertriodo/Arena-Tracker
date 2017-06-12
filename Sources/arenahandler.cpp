#include "arenahandler.h"
#include "Utils/qcompressor.h"
#include "themehandler.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler,
                           TrackobotUploader *trackobotUploader, PlanHandler *planHandler,
                           Ui::Extended *ui) : QObject(parent)
{
    this->trackobotUploader = trackobotUploader;
    this->deckHandler = deckHandler;
    this->planHandler = planHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->theme = ThemeWhite;
    this->mouseInApp = false;

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

    QFont font(BIG_FONT);
    font.setPixelSize(12);
    ui->logTextEdit->setFont(font);
    ui->logTextEdit->setFrameShape(QFrame::NoFrame);

    connect(ui->webButton, SIGNAL(clicked()),
            this, SLOT(openTBProfile()));
    connect(ui->replayButton, SIGNAL(clicked()),
            this, SLOT(replayLog()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SLOT(openDonateWeb()));

    completeRewardsUI();
}


void ArenaHandler::completeRewardsUI()
{
    QFont font(BIG_FONT);
    font.setPixelSize(20);
    ui->lineEditGold->setFont(font);
    ui->lineEditArcaneDust->setFont(font);
    ui->lineEditPack->setFont(font);
    ui->lineEditPlainCard->setFont(font);
    ui->lineEditGoldCard->setFont(font);

    ui->lineEditGold->setMinimumWidth(1);
    ui->lineEditArcaneDust->setMinimumWidth(1);
    ui->lineEditPack->setMinimumWidth(1);
    ui->lineEditPlainCard->setMinimumWidth(1);
    ui->lineEditGoldCard->setMinimumWidth(1);
    hideRewards();
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
    arenaCurrentHero = "";

    for(int i=0; i<9; i++)  rankedTreeItem[i]=NULL;
    casualTreeItem = NULL;
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


void ArenaHandler::replayLog()
{
    if(!trackobotUploader->isConnected() || lastReplayUploaded != NULL || !replayLogsMap.contains(ui->arenaTreeWidget->currentItem())) return;

    QString logFileName = replayLogsMap[ui->arenaTreeWidget->currentItem()];
    QRegularExpressionMatch match;

    if(logFileName.contains(QRegularExpression(".*\\.(\\w+)\\.arenatracker"), &match))
    {
        QString replayId = match.captured(1);
        emit pDebug("Opening: " + QString(Z2H_VIEW_REPLAY_URL) + replayId);
        QDesktopServices::openUrl(QUrl(Z2H_VIEW_REPLAY_URL + replayId));
        return;
    }

    QString url = "";
    if(logFileName.startsWith("DRAFT"))
    {
        url = Z2H_UPLOAD_DRAFT_URL;
    }
    else
    {
        url = Z2H_UPLOAD_GAME_URL;
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
    emit pDebug("Uploading replay " + replayLogsMap[lastReplayUploaded] + (logFileName=="temp.gz"?"(gzipped)":"") +
                " to " + url);

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

    emit pDebug("Replay " + logFileName + " uploaded to " + Z2H_VIEW_REPLAY_URL + replayId);
    emit showMessageProgressBar("Replay uploaded");

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
        setRowColor(lastReplayUploaded, QColor(ThemeHandler::themeColor2()));
    }
    else
    {
        emit pDebug("Failed replay " + logFileName + " rename to " + newLogFileName, Error);
    }

    lastReplayUploaded = NULL;

    emit pDebug("Opening: " + QString(Z2H_VIEW_REPLAY_URL) + replayId);
    QDesktopServices::openUrl(QUrl(Z2H_VIEW_REPLAY_URL + replayId));
}


void ArenaHandler::linkDraftLogToArenaCurrent(QString logFileName)
{
    if(arenaCurrent != NULL && !logFileName.isEmpty())  replayLogsMap[arenaCurrent] = logFileName;
}


void ArenaHandler::newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName, qint64 startGameEpoch)
{
    QTreeWidgetItem *item = showGameResult(gameResult, loadingScreen);

    if(item != NULL && !logFileName.isEmpty())  replayLogsMap[item] = logFileName;

    //Trackobot upload
    if(trackobotUploader != NULL && planHandler != NULL &&
            (loadingScreen == arena || loadingScreen == ranked || loadingScreen == casual || loadingScreen == friendly))
    {
        trackobotUploader->uploadResult(gameResult, loadingScreen, startGameEpoch, QDateTime::currentDateTime(), planHandler->getJsonCardHistory());
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
                updateWinLose(gameResult.isWinner, arenaCurrent);
            }
        break;

        case ranked:
            emit pDebug("Create GameResult from ranked with hero " + gameResult.playerHero + ".");
            emit pLog(tr("Log: New ranked game."));

            if(indexHero<0||indexHero>8)  return NULL;

            if(rankedTreeItem[indexHero] == NULL)
            {
                emit pDebug("Create Category ranked[" + QString::number(indexHero) + "].");
                rankedTreeItem[indexHero] = createTopLevelItem("Ranked", gameResult.playerHero, false);
            }

            item = new QTreeWidgetItem(rankedTreeItem[indexHero]);
            updateWinLose(gameResult.isWinner, rankedTreeItem[indexHero]);
        break;

        case casual:
            emit pDebug("Create GameResult from casual.");
            emit pLog(tr("Log: New casual game."));

            if(casualTreeItem == NULL)
            {
                emit pDebug("Create Category casual.");
                casualTreeItem = createTopLevelItem("Casual", "", false);
            }

            item = new QTreeWidgetItem(casualTreeItem);
            updateWinLose(gameResult.isWinner, casualTreeItem);
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
                friendlyTreeItem = createTopLevelItem("Duel", "", false);
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
    if(logFileName.contains(QRegularExpression("(\\w+) \\w+-\\d+ \\d+-\\d+ (\\w*)vs(\\w*) (WIN|LOSE) (FIRST|COIN)(\\.\\w+)?\\.arenatracker"), &match))
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
            if(!match.captured(6).isEmpty())    setRowColor(item, QColor(ThemeHandler::themeColor2()));

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


bool ArenaHandler::newArena(QString hero)
{
    showArena(hero);
    return true;
}


void ArenaHandler::showArenaLog(const QString &logFileName)
{
    QRegularExpressionMatch match;
    if(logFileName.contains(QRegularExpression("DRAFT \\w+-\\d+ \\d+-\\d+ (\\w+)(\\.\\w+)?\\.arenatracker"), &match))
    {
        QString playerHero = Utility::heroToLogNumber(match.captured(1));
        showArena(playerHero);
        if(!match.captured(2).isEmpty())    setRowColor(this->arenaCurrent, QColor(ThemeHandler::themeColor2()));
        linkDraftLogToArenaCurrent(logFileName);
    }
}


void ArenaHandler::showArena(QString hero)
{
    emit pDebug("Show Arena.");
    arenaCurrentHero = QString(hero);
    arenaCurrent = createTopLevelItem("Arena", arenaCurrentHero, true);
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    if(theme == ThemeWhite && (transparency != Transparent || mouseInApp))
        if(color == WHITE)  color = QColor(ThemeHandler::fgColor());

    for(int i=0;i<5;i++)
    {
        item->setForeground(i, QBrush(color));
    }
}


QColor ArenaHandler::getRowColor(QTreeWidgetItem *item)
{
    QColor color = item->foreground(0).color();

    if(color == QColor(ThemeHandler::fgColor()))      color = WHITE;
    return color;
}


bool ArenaHandler::isRowWhite(QTreeWidgetItem *item)
{
    return (getRowColor(item) == WHITE);
}


bool ArenaHandler::isRowOk(QTreeWidgetItem *item)
{
    return (getRowColor(item) != RED);
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


void ArenaHandler::updateWhiteRows()
{
    int numTopItems = ui->arenaTreeWidget->topLevelItemCount();
    for(int i=0; i<numTopItems; i++)
    {
        QTreeWidgetItem * item = ui->arenaTreeWidget->topLevelItem(i);
        int numItems = item->childCount();
        for(int j=0; j<numItems; j++)
        {
            if(isRowWhite(item->child(j))) setRowColor(item->child(j), WHITE);
        }
        if(isRowWhite(item))    setRowColor(item, WHITE);
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
        updateWhiteRows();
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
    updateWhiteRows();
}


QString ArenaHandler::getArenaCurrentDraftLog()
{
    if(replayLogsMap.contains(arenaCurrent))    return replayLogsMap[arenaCurrent];
    else                                        return "";
}
