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
    this->match = new QRegularExpressionMatch();
    this->editingColumnText = false;
    this->lastRegion = 0;

    completeUI();
}

ArenaHandler::~ArenaHandler()
{
    delete match;
}


void ArenaHandler::completeUI()
{
    createTreeWidget();
    createComboBoxArenaRegion();

    setPremium(false);

    connect(ui->guideButton, SIGNAL(clicked()),
            this, SLOT(openUserGuide()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));
}


void ArenaHandler::createComboBoxArenaRegion()
{
    int hw = 20;
    QColor fgColor(ThemeHandler::fgColor());
    for(const QColor &color: {fgColor, ARENA_YELLOW, ARENA_GREEN, ARENA_RED, ARENA_BLUE})
    {
        QPixmap canvas(QSize(hw*2, hw));
        canvas.fill(color);
        ui->arenaRegionComboBox->addItem(canvas,"");
    }
    ui->arenaRegionComboBox->setIconSize(QSize(hw*2, hw));
    ui->arenaRegionComboBox->setEnabled(false);

    connect(ui->arenaRegionComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(regionChanged(int)));
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
    treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    treeWidget->setColumnWidth(0, 110);
    treeWidget->setColumnWidth(1, 50);
    treeWidget->setColumnWidth(2, 40);
    treeWidget->setColumnWidth(3, 40);
    treeWidget->setColumnWidth(4, 0);

    arenaCurrent = nullptr;
    arenaCurrentHero = "";

    for(int i=0; i<NUM_HEROS; i++)  rankedTreeItem[i] = nullptr;
    casualTreeItem = nullptr;
    adventureTreeItem = nullptr;
    tavernBrawlTreeItem = nullptr;
    friendlyTreeItem = nullptr;

    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(itemChanged(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));
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


void ArenaHandler::setColumnText(QTreeWidgetItem *item, int col, const QString &text)
{
    editingColumnText = false;
    item->setText(col, text);
}


void ArenaHandler::setColumnIcon(QTreeWidgetItem *item, int col, const QIcon &aicon)
{
    editingColumnText = false;
    item->setIcon(col, aicon);
}


void ArenaHandler::updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem)
{
    emit pDebug("Recalculate win/loses (1 game).");
    if(isWinner)
    {
        int wins = topLevelItem->text(2).toInt() + 1;
        setColumnText(topLevelItem, 2, QString::number(wins));
    }
    else
    {
        int loses = topLevelItem->text(3).toInt() + 1;
        setColumnText(topLevelItem, 3, QString::number(loses));
    }
}


QTreeWidgetItem *ArenaHandler::createTopLevelItem(QString title, QString hero, bool addAtStart, int wins, int losses, bool isArena)
{
    QTreeWidgetItem *item;

    if(addAtStart)
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(0, item);
    }
    else    item = new QTreeWidgetItem(ui->arenaTreeWidget);

    item->setExpanded(true);
    setColumnText(item, 0, title);
    if(!hero.isEmpty()) setColumnIcon(item, 1, QIcon(ThemeHandler::heroFile(hero)));
    setColumnText(item, 2, QString::number(wins));
    item->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
    setColumnText(item, 3, QString::number(losses));
    item->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);

    if(isArena) setRowColor(item);
    else        setRowColor(item, ThemeHandler::fgColor());

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
                emit pDebug("Create GameResult from arena: Different hero. Create a new arena.");
                showArena(gameResult.playerHero);
            }
            emit pDebug("Create GameResult from arena in arenaCurrent.");
            item = new QTreeWidgetItem();
            arenaCurrent->insertChild(0, item);
            updateWinLose(gameResult.isWinner, arenaCurrent);
            setColumnText(arenaCurrent, 0, QDateTime::currentDateTime().toString("d MMM"));
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
    setColumnIcon(item, 0, QIcon(iconFile));
    setColumnText(item, 0, "vs");
    item->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter);

    iconFile = (gameResult.enemyHero==""?":Images/secretHunter.png":ThemeHandler::heroFile(gameResult.enemyHero));
    setColumnIcon(item, 1, QIcon(iconFile));
    if(!gameResult.enemyName.isEmpty() && gameResult.enemyName != "UNKNOWN HUMAN PLAYER")
    {
        item->setToolTip(1, gameResult.enemyName);
    }
    setColumnIcon(item, 2, QIcon(gameResult.isFirst?ThemeHandler::firstFile():ThemeHandler::coinFile()));
    setColumnIcon(item, 3, QIcon(gameResult.isWinner?ThemeHandler::winFile():ThemeHandler::loseFile()));

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

    if(title.isEmpty()) title = QDateTime::currentDateTime().toString("d MMM");

    arenaCurrentHero = QString(hero);
    arenaCurrent = createTopLevelItem(title, arenaCurrentHero, true, wins, losses, true);
    arenaCurrent->setFlags(arenaCurrent->flags() | Qt::ItemIsEditable);
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    for(int i=0;i<5;i++)
    {
        item->setForeground(i, QBrush(color));
    }
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item)
{
    switch(lastRegion)
    {
        case 1:
            setRowColor(item, ARENA_YELLOW);
        break;
        case 2:
            setRowColor(item, ARENA_GREEN);
        break;
        case 3:
            setRowColor(item, ARENA_RED);
        break;
        case 4:
            setRowColor(item, ARENA_BLUE);
        break;
        default:
            setRowColor(item, ThemeHandler::fgColor());
        break;
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


void ArenaHandler::setTransparency(Transparency value)
{
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
 * "region" -> 0
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
        this->lastRegion = objArena["region"].toInt();

        QString title = (date == "current")?statsJson["lastGame"].toString():date;
        title = QDateTime::fromString(title, "yyyy.MM.dd hh:mm").toString("d MMM");
        showArena(hero, title, wins, losses);
        arenaStatLink[arenaCurrent] = date;

        //Set date of last arena if complete
        if(date == "current" && (wins>11 || losses>2))
        {
            QString date = statsJson["lastGame"].toString();
            statsJson[date] = statsJson["current"].toObject();
            statsJson.remove("current");
            arenaStatLink[arenaCurrent] = date;
            saveStatsJsonFile();
        }
    }

    //Sync arenaCurrent
    if(!statsJson.contains("current"))
    {
        arenaCurrent = nullptr;
        arenaCurrentHero = "";
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

        //Update arenaStatLink map
        QTreeWidgetItem *arenaPrevious = arenaStatLink.key("current", nullptr);
        if(arenaPrevious != nullptr)
        {
            arenaStatLink[arenaPrevious] = date;

            //Update arena title
            QString title = QDateTime::fromString(date, "yyyy.MM.dd hh:mm").toString("d MMM");
            setColumnText(arenaPrevious, 0, title);
        }
    }

    QJsonObject objArena;
    objArena["hero"] = hero;
    objArena["wins"] = wins;
    objArena["losses"] = losses;
    objArena["region"] = lastRegion;
    statsJson["current"] = objArena;

    arenaStatLink[arenaCurrent] = "current";

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


void ArenaHandler::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(!arenaStatLink.contains(item))   return;
    if(column == 0)
    {
        if(item->childCount()>0)    return;

        QString date = arenaStatLink[item];
        if(date != "current")
        {
            editingColumnText = true;
            ui->arenaTreeWidget->editItem(item, column);
        }
    }
    else
    {
        if(column == 1)
        {
            if(item->childCount()>0)    return;

            setColumnText(item, column,
                Utility::classLogNumber2classLName(
                statsJson[arenaStatLink[item]].toObject()["hero"].toString()));
        }
        editingColumnText = true;
        ui->arenaTreeWidget->editItem(item, column);
    }
}


void ArenaHandler::itemChanged(QTreeWidgetItem *item, int column)
{
    if(!editingColumnText)  return;
    editingColumnText = false;
    if(!arenaStatLink.contains(item))
    {
        emit pDebug("ERROR: Trying to edit an arena not in arenaStatLink.");
        return;
    }

    if(column == 0)         itemChangedDate(item, column);
    else if(column == 1)    itemChangedHero(item, column);
    else if(column == 2 || column == 3) itemChangedWL(item, column);
}


void ArenaHandler::itemChangedDate(QTreeWidgetItem *item, int column)
{
    QString text = item->text(column);
    QDateTime newDateD;

    if(text.contains(QRegularExpression("^\\d{1,2} \\d{1,2}$"), match))
    {
        newDateD = QDateTime::fromString(match->captured(0), "d M");
    }
    else if(text.contains(QRegularExpression("^\\d{1,2} \\w+\\.?$"), match))
    {
        newDateD = QDateTime::fromString(match->captured(0), "d MMM");
    }

    if(newDateD.isValid())
    {
        QDateTime currentD = QDateTime::currentDateTime();
        QString year = currentD.toString("yyyy.");
        QString monthDay = newDateD.toString("MM.dd");
        newDateD = QDateTime::fromString(year+monthDay, "yyyy.MM.dd");

        //Avoid setting dates in the future
        if(newDateD>currentD)   newDateD = newDateD.addYears(-1);

        //Add random hh:mm
        QString newDate;
        do
        {
            newDate = (newDateD.addSecs(qrand()%86400)).toString("yyyy.MM.dd hh:mm");
        }
        while(statsJson.contains(newDate));

        QString oldDate = arenaStatLink[item];
        statsJson[newDate] = statsJson[oldDate].toObject();
        statsJson.remove(oldDate);
        arenaStatLink[item] = newDate;
        saveStatsJsonFile();
    }

    QString titleDate = QDateTime::fromString(arenaStatLink[item], "yyyy.MM.dd hh:mm").toString("d MMM");
    setColumnText(item, 0, titleDate);
}


void ArenaHandler::itemChangedHero(QTreeWidgetItem *item, int column)
{
    QString text = item->text(column);

    if(text.contains(QRegularExpression("^\\w+"), match))
    {
        QString hero = match->captured(0);
        QString heroLog = "";
        //--------------------------------------------------------
        //----NEW HERO CLASS
        //--------------------------------------------------------
        if(hero.startsWith("dr"))       heroLog = "06";
        else if(hero.startsWith("hu"))  heroLog = "05";
        else if(hero.startsWith("ma"))  heroLog = "08";
        else if(hero.startsWith("pa"))  heroLog = "04";
        else if(hero.startsWith("pr"))  heroLog = "09";
        else if(hero.startsWith("ro"))  heroLog = "03";
        else if(hero.startsWith("sh"))  heroLog = "02";
        else if(hero.startsWith("warl"))heroLog = "07";
        else if(hero.startsWith("warr"))heroLog = "01";
        else if(hero.startsWith("de"))  heroLog = "10";

        if(!heroLog.isEmpty())
        {
            setColumnIcon(item, 1, QIcon(ThemeHandler::heroFile(heroLog)));

            QJsonObject objArena = statsJson[arenaStatLink[item]].toObject();
            objArena["hero"] = heroLog;
            statsJson[arenaStatLink[item]] = objArena;
            saveStatsJsonFile();
        }
    }

    setColumnText(item, column, "");
}


void ArenaHandler::itemChangedWL(QTreeWidgetItem *item, int column)
{
    QString text = item->text(column);

    if(text.contains(QRegularExpression("^\\d{1,2}$"), match))
    {
        int score = match->captured(0).toInt();

        //wins[0-12] losses[0-3]
        if((column == 2 && score < 13)||(column == 3 && score < 4))
        {
            QJsonObject objArena = statsJson[arenaStatLink[item]].toObject();
            objArena[column==2?"wins":"losses"] = score;
            statsJson[arenaStatLink[item]] = objArena;
            saveStatsJsonFile();
        }
    }

    int score = statsJson[arenaStatLink[item]].toObject()[column==2?"wins":"losses"].toInt();
    setColumnText(item, column, QString::number(score));
}


void ArenaHandler::regionChanged(int index)
{
    QList<QTreeWidgetItem *> items = ui->arenaTreeWidget->selectedItems();
    if(items.count() == 1 && arenaStatLink.contains(items[0]))
    {
        QTreeWidgetItem *item = items[0];
        QJsonObject objArena = statsJson[arenaStatLink[item]].toObject();
        if(objArena["region"] != index)
        {
            objArena["region"] = this->lastRegion = index;
            statsJson[arenaStatLink[item]] = objArena;
            saveStatsJsonFile();
            setRowColor(item);
        }
    }
    else
    {
        ui->arenaRegionComboBox->setEnabled(false);
    }
}


void ArenaHandler::itemSelectionChanged()
{
    QList<QTreeWidgetItem *> items = ui->arenaTreeWidget->selectedItems();
    if(items.count() == 1 && arenaStatLink.contains(items[0]))
    {
        int region = statsJson[arenaStatLink[items[0]]].toObject()["region"].toInt();
        ui->arenaRegionComboBox->setCurrentIndex(region);
        ui->arenaRegionComboBox->setEnabled(true);
    }
    else
    {
        ui->arenaRegionComboBox->setEnabled(false);
    }
}
