#include "arenahandler.h"
#include "Utils/qcompressor.h"
#include "themehandler.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, PlanHandler *planHandler,
                           Ui::Extended *ui) : QObject(parent)
{
    this->deckHandler = deckHandler;
    this->planHandler = planHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->mouseInApp = false;
    this->arenaCurrentDraftFile = "";

    completeUI();
}

ArenaHandler::~ArenaHandler()
{
}


void ArenaHandler::completeUI()
{
    createTreeWidget();
    setPremium(false);

    connect(ui->guideButton, SIGNAL(clicked()),
            this, SLOT(openUserGuide()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));
}


void ArenaHandler::setPremium(bool premium)
{
    if(premium)
    {
        ui->donateButton->hide();
    }
    else
    {
        ui->donateButton->show();
    }
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

    arenaHomeless = new QTreeWidgetItem(treeWidget);
    arenaHomeless->setExpanded(true);
    arenaHomeless->setText(0, "Arena");
    arenaHomeless->setHidden(true);

    arenaCurrent = nullptr;
    arenaCurrentHero = "";

    for(int i=0; i<NUM_HEROS; i++)  rankedTreeItem[i] = nullptr;
    casualTreeItem = nullptr;
    adventureTreeItem = nullptr;
    tavernBrawlTreeItem = nullptr;
    friendlyTreeItem = nullptr;
}


void ArenaHandler::linkDraftLogToArenaCurrent(QString logFileName)
{
    arenaCurrentDraftFile = logFileName;
}


void ArenaHandler::newGameResult(GameResult gameResult, LoadingScreenState loadingScreen)
{
    showGameResult(gameResult, loadingScreen);
    if(loadingScreen == arena)
    {
        newArenaGameStat(gameResult);
        saveStatsJsonFile();
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


QTreeWidgetItem *ArenaHandler::createTopLevelItem(QString title, QString hero, bool addAtStart, int wins, int losses)
{
    QTreeWidgetItem *item;

    if(addAtStart)
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(1, item);
    }
    else    item = new QTreeWidgetItem(ui->arenaTreeWidget);

    item->setExpanded(true);
    item->setText(0, title);
    if(!hero.isEmpty())     item->setIcon(1, QIcon(ThemeHandler::heroFile(hero)));
    item->setText(2, QString::number(wins));
    item->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
    item->setText(3, QString::number(losses));
    item->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);

    setRowColor(item, ThemeHandler::fgColor());

    return item;
}


QTreeWidgetItem *ArenaHandler::createGameInCategory(GameResult &gameResult, LoadingScreenState loadingScreen)
{
    QTreeWidgetItem *item = nullptr;
    int indexHero = gameResult.playerHero.toInt()-1;

    switch(loadingScreen)
    {
        case menu:
            emit pDebug("Avoid GameResult from menu.");
        break;

        case arena:

            if(arenaCurrent == nullptr || arenaCurrentHero.compare(gameResult.playerHero)!=0)
            {
                emit pDebug("Create GameResult from arena in arenaHomeless.");

                if(arenaHomeless->isHidden())   arenaHomeless->setHidden(false);

                item = new QTreeWidgetItem();
                arenaHomeless->insertChild(0, item);
            }
            else
            {
                emit pDebug("Create GameResult from arena in arenaCurrent.");
                item = new QTreeWidgetItem();
                arenaCurrent->insertChild(0, item);
                updateWinLose(gameResult.isWinner, arenaCurrent);
            }
        break;

        case ranked:
            emit pDebug("Create GameResult from ranked with hero " + gameResult.playerHero + ".");

            if(indexHero<0||indexHero>(NUM_HEROS-1))    return nullptr;

            if(rankedTreeItem[indexHero] == nullptr)
            {
                emit pDebug("Create Category ranked[" + QString::number(indexHero) + "].");
                rankedTreeItem[indexHero] = createTopLevelItem("Ranked", gameResult.playerHero, false);
            }

            item = new QTreeWidgetItem();
            rankedTreeItem[indexHero]->insertChild(0, item);
            updateWinLose(gameResult.isWinner, rankedTreeItem[indexHero]);
        break;

        case adventure:
            emit pDebug("Create GameResult from adventure.");

            if(adventureTreeItem == nullptr)
            {
                emit pDebug("Create Category adventure.");
                adventureTreeItem = createTopLevelItem("Solo", "", false);
            }

            item = new QTreeWidgetItem();
            adventureTreeItem->insertChild(0, item);
            updateWinLose(gameResult.isWinner, adventureTreeItem);
        break;

        case tavernBrawl:
            emit pDebug("Create GameResult from tavern brawl.");

            if(tavernBrawlTreeItem == nullptr)
            {
                emit pDebug("Create Category tavern brawl.");
                tavernBrawlTreeItem = createTopLevelItem("Brawl", "", false);
            }

            item = new QTreeWidgetItem();
            tavernBrawlTreeItem->insertChild(0, item);
            updateWinLose(gameResult.isWinner, tavernBrawlTreeItem);
        break;

        case friendly:
            emit pDebug("Create GameResult from friendly.");

            if(friendlyTreeItem == nullptr)
            {
                emit pDebug("Create Category friendly.");
                friendlyTreeItem = createTopLevelItem("Friend", "", false);
            }

            item = new QTreeWidgetItem();
            friendlyTreeItem->insertChild(0, item);
            updateWinLose(gameResult.isWinner, friendlyTreeItem);
        break;

        default:
        break;
    }

    return item;
}


QTreeWidgetItem *ArenaHandler::showGameResult(GameResult gameResult, LoadingScreenState loadingScreen)
{
    emit pDebug("Show GameResult.");

    QTreeWidgetItem *item = createGameInCategory(gameResult, loadingScreen);
    if(item == nullptr)    return nullptr;

    QString iconFile = (gameResult.playerHero==""?":Images/secretHunter.png":ThemeHandler::heroFile(gameResult.playerHero));
    item->setIcon(0, QIcon(iconFile));
    item->setText(0, "vs");
    item->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter);

    iconFile = (gameResult.enemyHero==""?":Images/secretHunter.png":ThemeHandler::heroFile(gameResult.enemyHero));
    item->setIcon(1, QIcon(iconFile));
    if(!gameResult.enemyName.isEmpty() && gameResult.enemyName != "UNKNOWN HUMAN PLAYER")
    {
        item->setToolTip(1, gameResult.enemyName);
    }
    item->setIcon(2, QIcon(gameResult.isFirst?ThemeHandler::firstFile():ThemeHandler::coinFile()));
    item->setIcon(3, QIcon(gameResult.isWinner?ThemeHandler::winFile():ThemeHandler::loseFile()));

    setRowColor(item, ThemeHandler::fgColor());

    return item;
}


void ArenaHandler::newArena(QString hero)
{
    showArena(hero);
    newArenaStat(hero);
    saveStatsJsonFile();
}


void ArenaHandler::showArena(QString hero, QString title, int wins, int losses)
{
    emit pDebug("Show Arena.");
    arenaCurrentHero = QString(hero);
    arenaCurrent = createTopLevelItem(title, arenaCurrentHero, true, wins, losses);
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    for(int i=0;i<5;i++)
    {
        item->setForeground(i, QBrush(color));
    }
}


QColor ArenaHandler::getRowColor(QTreeWidgetItem *item)
{
    return item->foreground(0).color();
}


void ArenaHandler::openUserGuide()
{
    QDesktopServices::openUrl(QUrl(USER_GUIDE_URL));
}


void ArenaHandler::redrawRow(QTreeWidgetItem *item)
{
    bool isTransparent = transparency == Transparent && !mouseInApp;
    if(isTransparent)   setRowColor(item, WHITE);
    else                setRowColor(item, ThemeHandler::fgColor());
}


void ArenaHandler::redrawAllRows()
{
    int numTopItems = ui->arenaTreeWidget->topLevelItemCount();
    for(int i=0; i<numTopItems; i++)
    {
        QTreeWidgetItem * item = ui->arenaTreeWidget->topLevelItem(i);
        int numItems = item->childCount();
        for(int j=0; j<numItems; j++)   redrawRow(item->child(j));
        redrawRow(item);
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

    //Habra que cambiar los colores si:
    //1) La transparencia se ha cambiado
    //2) El raton ha salido/entrado y estamos en transparente
    if(transparencyChanged || this->transparency == Transparent )
    {
        redrawAllRows();
    }
}


void ArenaHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    setTransparency(this->transparency);
}


void ArenaHandler::setTheme()
{
    ui->guideButton->setIcon(QIcon(ThemeHandler::buttonGamesGuideFile()));
    ui->arenaTreeWidget->setTheme(false);
}


QString ArenaHandler::getArenaCurrentDraftLog()
{
    return arenaCurrentDraftFile;
}


/*
 * {
 * "lastGame" -> date ("1234")
 * date ("1231"/"current") -> ObjectArena
 * }
 *
 * ObjectArena
 * {
 * "hero" -> "01"
 * "wins" -> 7
 * "losses" -> 3
 * }
 */
void ArenaHandler::loadStatsJsonFile()
{
    //Load stats from file
    QFile jsonFile(Utility::dataPath() + "/ArenaTrackerStats.json");
    if(!jsonFile.exists())
    {
        emit pDebug("ArenaTrackerStats.json doesn't exists.");
        return;
    }

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Failed to load ArenaTrackerStats.json from disk.", DebugLevel::Error);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    statsJson = jsonDoc.object();

    emit pDebug("Loaded " + QString::number(statsJson.count()) + " arenas from ArenaTrackerStats.json.");

    //Load arenas
    for(const QString &date: (const QStringList)statsJson.keys())
    {
        if(date == "lastGame")  continue;
        QJsonObject objArena = statsJson[date].toObject();
        QString hero = objArena["hero"].toString();
        int wins = objArena["wins"].toInt();
        int losses = objArena["losses"].toInt();

        QString title = (date == "current")?statsJson["lastGame"].toString():date;
        title = QDateTime::fromString(title, "yyyy.MM.dd hh:mm").toString("dd MMM");//("d MMM");
        showArena(hero, title, wins, losses);
    }
}


void ArenaHandler::saveStatsJsonFile()
{
    //Build json data from statsJson
    QJsonDocument jsonDoc;
    jsonDoc.setObject(statsJson);


    //Save to disk
    QFile jsonFile(Utility::dataPath() + "/ArenaTrackerStats.json");
    if(jsonFile.exists())   jsonFile.remove();

    if(!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pDebug("Failed to create ArenaTrackerStats.json on disk.", DebugLevel::Error);
        return;
    }
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    emit pDebug("ArenaTrackerStats.json updated.");
}


void ArenaHandler::newArenaStat(QString hero, int wins, int losses)
{
    //Save date for previous current
    if(statsJson.contains("current"))
    {
        QString date = statsJson["lastGame"].toString();
        statsJson[date] = statsJson["current"].toObject();
    }

    QJsonObject objArena;
    objArena["hero"] = hero;
    objArena["wins"] = wins;
    objArena["losses"] = losses;
    statsJson["current"] = objArena;

    QString date = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
    statsJson["lastGame"] = date;
}


void ArenaHandler::newArenaGameStat(GameResult gameResult)
{
    if(statsJson.contains("current") && (statsJson["current"].toObject()["hero"].toString() == gameResult.playerHero))
    {
        QJsonObject objArena = statsJson["current"].toObject();
        if(gameResult.isWinner) objArena["wins"] = objArena["wins"].toInt()+1;
        else                    objArena["losses"] = objArena["losses"].toInt()+1;
        statsJson["current"] = objArena;

        QString date = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
        statsJson["lastGame"] = date;
    }
    else
    {
        newArenaStat(gameResult.playerHero, (gameResult.isWinner)?1:0, (gameResult.isWinner)?0:1);
    }
}

