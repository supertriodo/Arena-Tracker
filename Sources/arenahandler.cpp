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
    createArenaTreeWidget();
    createArenaStatsTreeWidget();
    createComboBoxArenaRegion();
    completeButtons();

    setPremium(false);
}


void ArenaHandler::completeButtons()
{
    ui->arenaDeleteButton->setEnabled(false);

    connect(ui->guideButton, SIGNAL(clicked()),
            this, SLOT(openUserGuide()));
    connect(ui->donateButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));
    connect(ui->arenaNewButton, SIGNAL(clicked()),
            this, SLOT(arenaNewEmpty()));
    connect(ui->arenaDeleteButton, SIGNAL(clicked()),
            this, SLOT(arenaDelete()));
    connect(ui->arenaStatsButton, SIGNAL(clicked()),
            this, SLOT(toggleArenaStatsTreeWidget()));
}


void ArenaHandler::createArenaTreeWidget()
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


void ArenaHandler::createArenaStatsTreeWidget()
{
    QTreeWidget *treeWidget = ui->arenaStatsTreeWidget;
    treeWidget->setColumnCount(6);
    treeWidget->setIconSize(QSize(30,30));
    treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
    treeWidget->setHidden(true);
    treeWidget->setFixedHeight(0);
    treeWidget->setIndentation(10);

    treeWidget->setColumnWidth(0, 130);
    treeWidget->setColumnWidth(1, 60);
    treeWidget->setColumnWidth(2, 60);
    treeWidget->setColumnWidth(3, 90);
    treeWidget->setColumnWidth(4, 90);
    treeWidget->setColumnWidth(5, 0);

    //WINRATES
    winrateTreeItem = new QTreeWidgetItem(treeWidget);
    winrateTreeItem->setExpanded(true);
    winrateTreeItem->setText(0, "Winrate");//TODO
    winrateTreeItem->setText(1, "Avg");
    winrateTreeItem->setText(2, "Runs");
    winrateTreeItem->setText(3, "Win");
    winrateTreeItem->setText(4, "Lost");
    for(int j=1; j<5; j++)  winrateTreeItem->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
    setRowColor(winrateTreeItem, QColor(ThemeHandler::fgColor()));

    for(int i=0; i<NUM_HEROS; i++)
    {
        QTreeWidgetItem *item = winrateClassTreeItem[i] = new QTreeWidgetItem(winrateTreeItem);

        setColumnIcon(item, 0, QIcon(ThemeHandler::heroFile(i)));
        setColumnText(item, 0, "61.2%");
        setColumnText(item, 1, "8.27");
        setColumnText(item, 2, "23");
        setColumnText(item, 3, QString::number(999));
        setColumnText(item, 4, QString::number(999));
        for(int j=1; j<5; j++)  item->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
        setRowColor(item, QColor(Utility::classOrder2classColor(i)));
//        item->setHidden(true);
    }


    //BEST 30
    new QTreeWidgetItem(treeWidget);//Blank space
    best30TreeItem = new QTreeWidgetItem(treeWidget);
    best30TreeItem->setExpanded(true);
    best30TreeItem->setText(0, "Best 30");
    best30TreeItem->setText(1, "Avg");
    best30TreeItem->setText(2, "Runs");
    best30TreeItem->setText(3, "Start");
    best30TreeItem->setText(4, "End");
    for(int j=1; j<5; j++)  best30TreeItem->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
    setRowColor(best30TreeItem, QColor(ThemeHandler::fgColor()));

    for(int i=0; i<5; i++)
    {
        QTreeWidgetItem *item = best30RegionTreeItem[i] = new QTreeWidgetItem(best30TreeItem);

        setColumnText(item, 0, "Region");
        setColumnText(item, 1, "8.27");
        setColumnText(item, 2, "23");
        setColumnText(item, 3, "15 Ago.");
        setColumnText(item, 4, "19 Sept.");
        for(int j=1; j<5; j++)  item->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
        setRowColor(item, i);
//        item->setHidden(true);
    }
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


void ArenaHandler::toggleArenaStatsTreeWidget()
{
    if(ui->arenaStatsTreeWidget->isHidden())    showArenaStatsTreeWidget();
    else                                        hideArenaStatsTreeWidget();
}


void ArenaHandler::showArenaStatsTreeWidget()
{
    ui->arenaStatsButton->setEnabled(false);
    ui->arenaNewButton->setEnabled(false);
    ui->arenaTreeWidget->clearSelection();
    ui->arenaStatsTreeWidget->setHidden(false);
    int totalHeight = ui->arenaTreeWidget->height();

    //Show ArenaStatsTreeWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->arenaStatsTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->arenaStatsTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowArenaStatsTreeWidget()));

    animation = new QPropertyAnimation(ui->arenaStatsTreeWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->arenaStatsTreeWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowArenaStatsTreeWidget()));

    //Hide ArenaTreeWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->arenaTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation2, SIGNAL(finished()),
            this, SLOT(finishHideArenaTreeWidget()));
}


void ArenaHandler::finishShowArenaStatsTreeWidget()
{
    ui->arenaStatsTreeWidget->setMinimumHeight(0);
    ui->arenaStatsTreeWidget->setMaximumHeight(16777215);
    ui->arenaStatsButton->setEnabled(true);
}


void ArenaHandler::finishHideArenaTreeWidget()
{
    ui->arenaTreeWidget->setHidden(true);
    ui->arenaTreeWidget->setFixedHeight(0);
}


void ArenaHandler::hideArenaStatsTreeWidget()
{
    ui->arenaStatsButton->setEnabled(false);
    ui->arenaTreeWidget->setHidden(false);
    int totalHeight = ui->arenaStatsTreeWidget->height();

    //Show ArenaTreeWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->arenaTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->arenaTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowArenaTreeWidget()));

    animation = new QPropertyAnimation(ui->arenaTreeWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->arenaTreeWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowArenaTreeWidget()));

    //Hide ArenaStatsTreeWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->arenaStatsTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(HIDE_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation2, SIGNAL(finished()),
            this, SLOT(finishHideArenaStatsTreeWidget()));
}


void ArenaHandler::finishHideArenaStatsTreeWidget()
{
    ui->arenaStatsTreeWidget->setHidden(true);
    ui->arenaStatsTreeWidget->setFixedHeight(0);
}

void ArenaHandler::finishShowArenaTreeWidget()
{
    ui->arenaTreeWidget->setMinimumHeight(0);
    ui->arenaTreeWidget->setMaximumHeight(16777215);
    ui->arenaStatsButton->setEnabled(true);
    ui->arenaNewButton->setEnabled(true);
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


QString ArenaHandler::getColumnText(QTreeWidgetItem *item, int col)
{
    return item->text(col);
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
        int wins = getColumnText(topLevelItem, 2).toInt() + 1;
        setColumnText(topLevelItem, 2, QString::number(wins));
    }
    else
    {
        int loses = getColumnText(topLevelItem, 3).toInt() + 1;
        setColumnText(topLevelItem, 3, QString::number(loses));
    }
}


QTreeWidgetItem *ArenaHandler::createTopLevelItem(QString title, QString hero, int wins, int losses,
                                                  bool isArena, bool insertPos1)
{
    QTreeWidgetItem *item;

    if(insertPos1 && ui->arenaTreeWidget->topLevelItemCount()>0)
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(1, item);
    }
    else if(isArena)
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
                emit pDebug("Create GameResult from arena: No arenaCurrent/Different hero. Create a new arena.");
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
                rankedTreeItem[indexHero] = createTopLevelItem("Ranked", gameResult.playerHero);
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
                adventureTreeItem = createTopLevelItem("Solo", "");
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
                tavernBrawlTreeItem = createTopLevelItem("Brawl", "");
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
                friendlyTreeItem = createTopLevelItem("Friend", "");
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


QTreeWidgetItem *ArenaHandler::showArena(QString hero, QString title, int wins, int losses, bool isArenaNewEmpty)
{
    emit pDebug("Show Arena" + QString(isArenaNewEmpty?" new empty.":"."));

    if(title.isEmpty()) title = QDateTime::currentDateTime().toString("d MMM");

    QTreeWidgetItem *item = createTopLevelItem(title, hero, wins, losses, true, isArenaNewEmpty);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if(!isArenaNewEmpty)
    {
        arenaCurrentHero = hero;
        arenaCurrent = item;
    }
    setColorWrongArena(item);
    return item;
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item, QColor color)
{
    for(int i=0;i<5;i++)
    {
        item->setForeground(i, QBrush(color));
    }
}


void ArenaHandler::setRowColor(QTreeWidgetItem *item, int region)
{
    if(region == -1)    region = lastRegion;
    switch(region)
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


void ArenaHandler::setColorWrongArena(QTreeWidgetItem *item)
{
    if(item == nullptr) return;

    int wins = getColumnText(item, 2).toInt();
    int loses = getColumnText(item, 3).toInt();

    if(!((loses==3 && wins<12) || (loses<3 && wins==12)))
    {
        item->setForeground(2, QBrush(ARENA_WRONG));
        item->setForeground(3, QBrush(ARENA_WRONG));
    }
}


void ArenaHandler::openUserGuide()
{
    QDesktopServices::openUrl(QUrl(USER_GUIDE_URL));
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
    ui->arenaStatsButton->setIcon(QIcon(ThemeHandler::buttonGamesWebFile()));
    ui->arenaNewButton->setIcon(QIcon(ThemeHandler::buttonPlusFile()));
    ui->arenaDeleteButton->setIcon(QIcon(ThemeHandler::buttonRemoveFile()));
    ui->arenaTreeWidget->setTheme(false);
    ui->arenaStatsTreeWidget->setTheme(true);
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

    emit pDebug("Loaded " + QString::number(statsJson.count()) + " entries from ArenaTrackerStats.json.");

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
            date = getUniqueDate(date);
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


QString ArenaHandler::getUniqueDate(QString date)
{
    if(date.isEmpty())  date = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
    if(!statsJson.contains(date))   return date;

    QDateTime oldDateD = QDateTime::fromString(date, "yyyy.MM.dd hh:mm");
    QDateTime dateD = QDateTime::fromString(oldDateD.toString("yyyy.MM.dd"), "yyyy.MM.dd");

    //Add random hh:mm
    do
    {
        date = (dateD.addSecs(qrand()%86400)).toString("yyyy.MM.dd hh:mm");
    }
    while(statsJson.contains(date));

    return date;
}


/*
 * Item se usa cuando se crea una nueva arena con el boton arenaNewButton que no se define como current,
 * sino que se pone en segunda posicion.
 */
void ArenaHandler::newArenaStat(QString hero, int wins, int losses, QTreeWidgetItem *item)
{
    //Save date for previous current
    if(item == nullptr && statsJson.contains("current"))
    {
        QString date = statsJson["lastGame"].toString();
        date = getUniqueDate(date);
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

    if(item == nullptr)
    {
        statsJson["current"] = objArena;
        arenaStatLink[arenaCurrent] = "current";

        QString date = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
        statsJson["lastGame"] = date;
    }
    else
    {
        QString date = getUniqueDate("");
        statsJson[date] = objArena;
        arenaStatLink[item] = date;
    }
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
    QString text = getColumnText(item, column);
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
        QString newDate = newDateD.toString("yyyy.MM.dd hh:mm");
        newDate = getUniqueDate(newDate);
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
    QString text = getColumnText(item, column);

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
    QString text = getColumnText(item, column);

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

    QJsonObject objArena = statsJson[arenaStatLink[item]].toObject();
    int score = objArena[column==2?"wins":"losses"].toInt();
    int region = objArena["region"].toInt();
    setColumnText(item, column, QString::number(score));
    setRowColor(item, region);
    setColorWrongArena(item);
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
            setColorWrongArena(item);
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
        ui->arenaDeleteButton->setEnabled(true);
    }
    else
    {
        ui->arenaRegionComboBox->setEnabled(false);
        ui->arenaDeleteButton->setEnabled(false);
    }
}


void ArenaHandler::arenaNewEmpty()
{
    int ret = QMessageBox::question(ui->tabArena, "New Arena?",
                                   "Do you want to create a new arena?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel)  return;

    QString hero = "01";
    QTreeWidgetItem *item = showArena(hero, "", 0, 0, true);
    newArenaStat(hero, 0, 0, item);
    saveStatsJsonFile();
}


void ArenaHandler::arenaDelete()
{

    int ret = QMessageBox::question(ui->tabArena, "Remove Arena?",
                                   "Do you want to remove the selected arena?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel)  return;

    QList<QTreeWidgetItem *> items = ui->arenaTreeWidget->selectedItems();
    if(items.count() == 1 && arenaStatLink.contains(items[0]))
    {
        QTreeWidgetItem *item = items[0];
        QString date = arenaStatLink[item];
        statsJson.remove(date);
        arenaStatLink.remove(item);

        if(arenaCurrent == item)
        {
            arenaCurrent = nullptr;
            arenaCurrentHero = "";
        }
        delete item;
        saveStatsJsonFile();
    }
    else
    {
        ui->arenaDeleteButton->setEnabled(false);
    }
}
