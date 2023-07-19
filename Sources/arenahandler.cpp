#include "arenahandler.h"
#include "Utils/qcompressor.h"
#include "themehandler.h"
#include "Widgets/scorebutton.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

ArenaHandler::ArenaHandler(QObject *parent, DeckHandler *deckHandler, PlanHandler *planHandler,
                           Ui::Extended *ui) : QObject(parent)
{
    this->deckHandler = deckHandler;
    this->planHandler = planHandler;
    this->ui = ui;
    this->transparency = Opaque;
    this->mouseInApp = false;
    this->match = new QRegularExpressionMatch();
    this->editingColumnText = false;
    this->lastRegion = 0;
    this->statsJsonFile = "";
    this->downloadLB = true;

    createNetworkManager();
    clearLeaderboardMap();
    completeUI();
}

ArenaHandler::~ArenaHandler()
{
    delete match;
    delete nmLbGlobal;
    delete nmLbSearch;
}


void ArenaHandler::createNetworkManager()
{
    this->nmLbGlobal = new QNetworkAccessManager();
    this->nmLbSearch = new QNetworkAccessManager();
    connect(nmLbGlobal, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(nmLbGlobalFinished(QNetworkReply*)));
    connect(nmLbSearch, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(nmLbSearchFinished(QNetworkReply*)));
}


void ArenaHandler::completeUI()
{
    createArenaTreeWidget();
    createArenaStatsTreeWidget();
    createComboBoxArenaRegion();
    createComboBoxArenaStatsJson();
    completeButtons();

    ui->enemyRankingLabel->setFixedHeight(0);
    ui->enemyRankingLabel->hide();

    setPremium(false, false);
}


void ArenaHandler::completeButtons()
{
    ui->arenaDeleteButton->setHidden(true);

    connect(ui->donateButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));
    connect(ui->arenaNewButton, SIGNAL(clicked()),
            this, SLOT(arenaNewEmpty()));
    connect(ui->arenaDeleteButton, SIGNAL(clicked()),
            this, SLOT(arenaDelete()));
    connect(ui->arenaStatsButton, SIGNAL(clicked()),
            this, SLOT(toggleArenaStatsTreeWidget()));
}


void ArenaHandler::setNullTreeItems()
{
    arenaCurrent = nullptr;
    arenaCurrentHero = "";

    for(int i=0; i<NUM_HEROS; i++)  rankedTreeItem[i] = nullptr;
    casualTreeItem = nullptr;
    adventureTreeItem = nullptr;
    tavernBrawlTreeItem = nullptr;
    friendlyTreeItem = nullptr;
}


void ArenaHandler::createArenaTreeWidget()
{
    MoveTreeWidget *treeWidget = ui->arenaTreeWidget;
    treeWidget->setColumnCount(5);
    treeWidget->setEditableColumns({0,1,2,3});
    treeWidget->setIconSize(QSize(32,32));
    treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    treeWidget->setColumnWidth(0, 120);
    treeWidget->setColumnWidth(1, 50);
    treeWidget->setColumnWidth(2, 40);
    treeWidget->setColumnWidth(3, 40);
    treeWidget->setColumnWidth(4, 50);

    setNullTreeItems();

    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(itemChanged(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));
    connect(treeWidget, SIGNAL(xLeave()),
            this, SLOT(deselectRow()));
}


void ArenaHandler::createArenaStatsTreeWidget()
{
    MoveTreeWidget *treeWidget = ui->arenaStatsTreeWidget;
    treeWidget->setColumnCount(6);
    treeWidget->setEditableColumns({0});
    treeWidget->setIconSize(QSize(30,30));
    treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
    treeWidget->setHidden(true);
    treeWidget->setFixedHeight(0);
    treeWidget->setIndentation(10);

    treeWidget->setColumnWidth(0, 130);
    treeWidget->setColumnWidth(1, 60);
    treeWidget->setColumnWidth(2, 60);
    treeWidget->setColumnWidth(3, 60);
    treeWidget->setColumnWidth(4, 60);
    treeWidget->setColumnWidth(5, 0);

    //LEADERBOARD
    lbTreeItem = new QTreeWidgetItem(treeWidget);
    lbTreeItem->setExpanded(true);
    lbTreeItem->setText(0, "Leaderb.");
    lbTreeItem->setText(1, "Avg");
    lbTreeItem->setText(2, "#");
    lbTreeItem->setText(3, "Page");
    lbTreeItem->setText(4, "Reg");
    for(int j=1; j<5; j++)  lbTreeItem->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
    setRowColor(lbTreeItem, QColor(ThemeHandler::fgColor()));

    for(int i=0; i<3; i++)
    {
        QTreeWidgetItem *item = lbRegionTreeItem[i] = new QTreeWidgetItem(lbTreeItem);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        for(int j=1; j<5; j++)  item->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
        setRowColor(item, 0);
        item->setHidden(true);
    }

    //WINRATES
    new QTreeWidgetItem(treeWidget);//Blank space
    winrateTreeItem = new QTreeWidgetItem(treeWidget);
    winrateTreeItem->setExpanded(true);
    winrateTreeItem->setText(0, "Winrate");
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
        for(int j=1; j<5; j++)  item->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
        setRowColor(item, QColor(Utility::classOrder2classColor(i)));
        item->setHidden(true);
    }


    //BEST 30
    new QTreeWidgetItem(treeWidget);//Blank space
    best30TreeItem = new QTreeWidgetItem(treeWidget);
    best30TreeItem->setExpanded(true);
    best30TreeItem->setText(0, "Region");
    best30TreeItem->setText(1, "Avg");
    best30TreeItem->setText(2, "LB");
    best30TreeItem->setText(3, "Runs");
    for(int j=1; j<5; j++)  best30TreeItem->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
    setRowColor(best30TreeItem, QColor(ThemeHandler::fgColor()));

    for(int i=0; i<NUM_REGIONS; i++)
    {
        QTreeWidgetItem *item = best30RegionTreeItem[i] = new QTreeWidgetItem(best30TreeItem);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        for(int j=1; j<5; j++)  item->setTextAlignment(j, Qt::AlignHCenter|Qt::AlignVCenter);
        setRowColor(item, i);
        item->setHidden(true);
    }

    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(statItemChanged(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(statItemDoubleClicked(QTreeWidgetItem*,int)));
}


void ArenaHandler::loadRegionNames()
{
    for(int i=0; i<NUM_REGIONS; i++)
    {
        QTreeWidgetItem *item = best30RegionTreeItem[i];
        setColumnText(item, 0, getJsonExtraRegion(i));
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
    ui->arenaRegionComboBox->setHidden(true);

    connect(ui->arenaRegionComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(regionChanged(int)));
}


void ArenaHandler::createComboBoxArenaStatsJson()
{
    checkNewPeriod();
    ui->arenaStatsJsonComboBox->setHidden(true);

    QFileInfo dirInfo(Utility::arenaStatsPath());
    if(!dirInfo.exists())
    {
        emit pDebug("Cannot check Arena Stats dir. Dir doesn't exist.");
        return;
    }

    QDir dir(Utility::arenaStatsPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name|QDir::Reversed);
    QStringList filterName;
    filterName << "*.json";
    dir.setNameFilters(filterName);

    ui->arenaStatsJsonComboBox->addItem("Current period", "ArenaTrackerStats.json");

    QStringList files = dir.entryList();
    for(int i=0; i<files.length(); i++)
    {
        QString file = files[i];
        QString title = file;
        if(file == "ArenaTrackerStats.json")    continue;

        title.chop(5);
        QDateTime dateD = QDateTime::fromString(title, "yyyy-MM");
        if(dateD.isValid())
        {
            title = dateD.toString("yyyy MMM");
            dateD = dateD.addMonths(1);
            title += dateD.toString(" MMM");
        }

        ui->arenaStatsJsonComboBox->addItem(title, file);
    }

    if(ui->arenaStatsJsonComboBox->count() > 1)
    {
        ui->arenaStatsJsonComboBox->setHidden(false);
        connect(ui->arenaStatsJsonComboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(arenaStatsJsonChanged(int)));
    }
}


void ArenaHandler::checkNewPeriod()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runDate = settings.value("runDate", "").toString();
    QDateTime nowDateD = QDateTime::currentDateTime();

    if(!runDate.isEmpty())
    {
        QDateTime runDateD = QDateTime::fromString(runDate, "yyyy.MM.dd hh:mm");
        QString title = isNewPeriod(runDateD, nowDateD);
        if(!title.isEmpty())
        {
            QFile file(Utility::arenaStatsPath() + "/ArenaTrackerStats.json");
            if(file.exists())   file.rename(Utility::arenaStatsPath() + "/" + title + ".json");
        }
    }

    settings.setValue("runDate", nowDateD.toString("yyyy.MM.dd hh:mm"));
}


QString ArenaHandler::isNewPeriod(const QDateTime &leftD, const QDateTime &rightD)
{
    QString title = "";
    QDateTime limit = QDateTime::fromString(leftD.toString("yyyy.MM.01 10:00"), "yyyy.MM.dd hh:mm");
    if(limit.date().month()%2==0)   limit = limit.addMonths(1);
    else if(limit<=leftD)           limit = limit.addMonths(2);
    if(limit<=rightD)
    {
        limit = limit.addMonths(-2);
        title = limit.toString("yyyy-MM");
    }
    return title;
}


void ArenaHandler::setPremium(bool premium, bool load)
{
    this->premium = premium;
    ui->donateButton->setHidden(premium);
    ui->arenaStatsButton->setHidden(!premium);
    if(load)    loadSelectedStatsJsonFile();

    //Create Leaderboard map
    if(premium && downloadLB)
    {
        mapLeaderboard();
    }
}


void ArenaHandler::toggleArenaStatsTreeWidget()
{
    if(ui->arenaStatsTreeWidget->isHidden())    showArenaStatsTreeWidget();
    else                                        hideArenaStatsTreeWidget();
}


void ArenaHandler::showArenaStatsTreeWidget()
{
    //Search lb to update player stats
    bool searchOk = searchLeaderboard(getPlayerName());
    if(searchOk)
    {
        for(int i=0; i<3; i++)
        {
            if(searchPage[i] > 0)   lbTagPulsing(i);
        }
    }

    startProcessArenas2Stats();

    ui->arenaStatsButton->setEnabled(false);
    ui->arenaNewButton->setHidden(true);
    ui->arenaStatsJsonComboBox->setHidden(true);
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
    if(ui->arenaStatsTreeWidget->isHidden())    return;
    if(!ui->arenaStatsButton->isEnabled())      return;

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
    ui->arenaNewButton->setHidden(false);
    ui->arenaStatsJsonComboBox->setHidden(false);
}


void ArenaHandler::setCurrentStatsJson()
{
    int index = ui->arenaStatsJsonComboBox->findData("ArenaTrackerStats.json");
    if(index == -1) loadStatsJsonFile();
    else            ui->arenaStatsJsonComboBox->setCurrentIndex(index);
}


void ArenaHandler::newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, int avgHA, float avgHSR)
{
    setCurrentStatsJson();
    hideArenaStatsTreeWidget();
    showGameResult(gameResult, loadingScreen, avgHA, avgHSR);
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


void ArenaHandler::setColumnText(QTreeWidgetItem *item, int col, const QString &text, int maxNameLong)
{
    editingColumnText = false;
    item->setText(col, text);
    if(maxNameLong > 0)
    {
        QFont font(ThemeHandler::bigFont());
        Utility::shrinkText(font, text, 22, maxNameLong);//Default pixeSize en movetreewidget.cpp es 22
        item->setFont(col, font);
    }
}


void ArenaHandler::setColumnIcon(QTreeWidgetItem *item, int col, const QIcon &aicon)
{
    editingColumnText = false;
    item->setIcon(col, aicon);
}


void ArenaHandler::updateWinLose(bool isWinner, QTreeWidgetItem *topLevelItem)
{
    emit pDebug("Recalculate wins/losses (1 game).");
    if(isWinner)
    {
        int wins = getColumnText(topLevelItem, 2).toInt() + 1;
        setColumnText(topLevelItem, 2, QString::number(wins));
    }
    else
    {
        int losses = getColumnText(topLevelItem, 3).toInt() + 1;
        setColumnText(topLevelItem, 3, QString::number(losses));
    }
}


QTreeWidgetItem *ArenaHandler::createTopLevelItem(QString title, QString hero, int wins, int losses, int avgHA, float avgHSR,
                                                  bool isArena, bool insertPos1)
{
    QTreeWidgetItem *item;

    if(insertPos1 && ui->arenaTreeWidget->topLevelItemCount()>0)
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(1, item);
    }
    else
    {
        item = new QTreeWidgetItem();
        ui->arenaTreeWidget->insertTopLevelItem(0, item);
    }

    item->setExpanded(true);
    setColumnText(item, 0, title);
    if(!hero.isEmpty()) setColumnIcon(item, 1, QIcon(ThemeHandler::heroFile(hero)));
    setColumnText(item, 2, QString::number(wins));
    item->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter);
    setColumnText(item, 3, QString::number(losses));
    item->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter);
    float avgScore = (this->draftMethodAvgScore==HearthArena?avgHA:avgHSR);
    if(premium && avgScore != 0)
    {
        setColumnIcon(item, 4, ScoreButton::scoreIcon(scoreSourceFromDraftMethod(this->draftMethodAvgScore), avgScore));
    }

    if(isArena) setRowColor(item);
    else        setRowColor(item, ThemeHandler::fgColor());

    return item;
}


void ArenaHandler::loadSelectedStatsJsonFile()
{
    int index = ui->arenaStatsJsonComboBox->currentIndex();
    if(index == -1) loadStatsJsonFile();
    else
    {
        QString file = ui->arenaStatsJsonComboBox->currentData().toString();
        loadStatsJsonFile(file);
    }
}


void ArenaHandler::setDraftMethodAvgScore(DraftMethod draftMethodAvgScore)
{
    this->draftMethodAvgScore = draftMethodAvgScore;

    if(premium) loadSelectedStatsJsonFile();
}


ScoreSource ArenaHandler::scoreSourceFromDraftMethod(DraftMethod draftMethod)
{
    if(draftMethod == HearthArena)      return Score_HearthArena;
    //Usamos Heroes para que el rango de colores del icono sea mas amplio en lugar de ir de 50 a 60
    //Evitamos mostrar los avg wr todos verdes o rojos
    else if(draftMethod == HSReplay)    return Score_Heroes;
    return Score_None;
}


void ArenaHandler::setCurrentAvgScore(int avgHA, float avgHSR, QString heroLog)
{
    if(arenaCurrent != nullptr && statsJson.contains("current"))
    {
        QJsonObject objArena = statsJson["current"].toObject();

        if(objArena["hero"].toString() == heroLog)
        {
            if(objArena["avgHA"].toInt(0) == 0 || objArena["avgHSR"].toDouble(0) == 0)
            {
                objArena["avgHA"] = avgHA;
                objArena["avgHSR"] = round(avgHSR * 10)/10.0;
                statsJson["current"] = objArena;
                saveStatsJsonFile();

                float avgScore = (this->draftMethodAvgScore==HearthArena?avgHA:avgHSR);
                if(premium && avgScore != 0)
                {
                    setColumnIcon(arenaCurrent, 4, ScoreButton::scoreIcon(scoreSourceFromDraftMethod(this->draftMethodAvgScore),
                                                                          avgScore));
                }
                emit pDebug("Set AvgScore: " + QString::number(avgHA) + " - " + QString::number(avgHSR));
            }
            else    emit pDebug("Avoid Set AvgScore: AvgScore present on arenaCurrent.");
        }
        else    emit pDebug("Avoid Set AvgScore: Wrong hero.");
    }
    else    emit pDebug("Avoid Set AvgScore: No arenaCurrent.");
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
            setColorWrongArena(arenaCurrent);
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


QTreeWidgetItem *ArenaHandler::showGameResult(GameResult gameResult, LoadingScreenState loadingScreen, int avgHA, float avgHSR)
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
    float avgScore = (this->draftMethodAvgScore==HearthArena?avgHA:avgHSR);
    if(premium && avgScore != 0)
    {
        setColumnIcon(item, 4, ScoreButton::scoreIcon(scoreSourceFromDraftMethod(this->draftMethodAvgScore), avgScore));
    }

    setRowColor(item, ThemeHandler::fgColor());

    return item;
}


void ArenaHandler::newArena(QString hero)
{
    if(hero.isEmpty())
    {
        emit pDebug("New arena with empty hero.");
        return;
    }

    setCurrentStatsJson();
    hideArenaStatsTreeWidget();
    showArena(hero);
    newArenaStat(hero);
    saveStatsJsonFile();
}


QTreeWidgetItem *ArenaHandler::showArena(QString hero, QString title, int wins, int losses, int avgHA, float avgHSR,
                                         bool isArenaNewEmpty)
{
    emit pDebug("Show Arena" + QString(isArenaNewEmpty?" new empty.":"."));

    if(title.isEmpty()) title = QDateTime::currentDateTime().toString("d MMM");

    QTreeWidgetItem *item = createTopLevelItem(title, hero, wins, losses, avgHA, avgHSR,
                                               true, isArenaNewEmpty);
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

    QBrush brush;
    int wins = getColumnText(item, 2).toInt();
    int losses = getColumnText(item, 3).toInt();

    if(isCompleteArena(wins, losses))   brush = item->foreground(0);
    else                                brush = QBrush(ARENA_WRONG);

    item->setForeground(2, brush);
    item->setForeground(3, brush);
}


bool ArenaHandler::isCompleteArena(int wins, int losses)
{
    if(((losses==3 && wins<12) || (losses<3 && wins==12)))  return true;
    else                                                    return false;
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
    ui->arenaStatsButton->setIcon(QIcon(ThemeHandler::buttonGamesWebFile()));
    ui->arenaNewButton->setIcon(QIcon(ThemeHandler::buttonPlusFile()));
    ui->arenaDeleteButton->setIcon(QIcon(ThemeHandler::buttonRemoveFile()));
    ui->arenaTreeWidget->setTheme(false);
    ui->arenaStatsTreeWidget->setTheme(true);

    setRowColor(lbTreeItem, QColor(ThemeHandler::themeColor2()));
    setRowColor(winrateTreeItem, QColor(ThemeHandler::themeColor2()));
    setRowColor(best30TreeItem, QColor(ThemeHandler::themeColor2()));
}


/*
 * {
 * date ("1231"/"current") -> ObjectArena
 * "extra" -> ObjectExtra
 * }
 *
 * ObjectArena
 * {
 * "hero" -> "01"
 * "wins" -> 7
 * "losses" -> 3
 * "region" -> 0
 * }
 *
 * ObjectExtra
 * {
 * "lastGame" -> date ("1234")
 * }
 */
void ArenaHandler::loadStatsJsonFile(const QString &statsFile)
{
    //Clear arenas data
    ui->arenaTreeWidget->clear();
    arenaStatLink.clear();
    statsJsonFile = statsFile;
    statsJson = QJsonObject();
    lastRegion = 0;
    setNullTreeItems();

    //Load stats from file
    if(!jsonObjectFromFile(statsJson, statsFile))
    {
        //Failed to read but the file exists
        //statsJsonFile empty evitara que el archivo se sobreescriba con las nuevas arenas jugadas en esta sesion
        statsJsonFile = "";
        return;
    }

    emit pDebug("Loaded " + QString::number(statsJson.count()) + " entries from " + statsFile + ".");

    //Load arenas
    for(const QString &date: (const QStringList)statsJson.keys())
    {
        if(date == "extra")  continue;

        QJsonObject objArena = statsJson[date].toObject();
        QString hero = objArena["hero"].toString();
        int wins = objArena["wins"].toInt();
        int losses = objArena["losses"].toInt();
        this->lastRegion = objArena["region"].toInt();
        int avgHA = objArena["avgHA"].toInt(0);
        float avgHSR = objArena["avgHSR"].toDouble(0);

        QString title = (date == "current")?getJsonExtra("lastGame"):date;
        title = QDateTime::fromString(title, "yyyy.MM.dd hh:mm").toString("d MMM");
        showArena(hero, title, wins, losses, avgHA, avgHSR);
        arenaStatLink[arenaCurrent] = date;

        //Set date of last arena if complete
        if(date == "current" && (wins>11 || losses>2))
        {
            QString date = getJsonExtra("lastGame");
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

    loadRegionNames();
}
bool ArenaHandler::jsonObjectFromFile(QJsonObject &jsonObject, const QString &statsFile)
{
    QFile jsonFile(Utility::arenaStatsPath() + "/" + statsFile);
    if(!jsonFile.exists())
    {
        emit pDebug(statsFile + " doesn't exists.");
        return true;
    }

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Failed to load " + statsFile + " from disk.", DebugLevel::Error);
        return false;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    jsonObject = jsonDoc.object();
    return true;
}


void ArenaHandler::saveStatsJsonFile()
{
    if(statsJsonFile.isEmpty())  return;

    //Build json data from statsJson
    QJsonDocument jsonDoc;
    jsonDoc.setObject(statsJson);


    //Save to disk
    QFile jsonFile(Utility::arenaStatsPath() + "/" + statsJsonFile);
    if(jsonFile.exists())   jsonFile.remove();

    if(!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pDebug("Failed to create " + statsJsonFile + " on disk.", DebugLevel::Error);
        return;
    }
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    emit pDebug(statsJsonFile + " updated.");
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


void ArenaHandler::setJsonExtra(QString key, QString value)
{
    QJsonObject objExtra = statsJson["extra"].toObject();
    objExtra[key] = value;
    statsJson["extra"] = objExtra;
}


QString ArenaHandler::getJsonExtra(QString key)
{
    return statsJson["extra"].toObject()[key].toString();
}


QString ArenaHandler::getJsonExtraRegion(int i)
{
    QString region = getJsonExtra("region" + QString::number(i));

    if(!region.isEmpty())   return region;

    switch(i)
    {
        case 0:
            return "Main";
        break;
        case 1:
            return "Europe";
        break;
        case 2:
            return "America";
        break;
        case 3:
            return "Asia";
        break;
        default:
            return "Other";
        break;
    }
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
        QString date = getJsonExtra("lastGame");
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
        setJsonExtra("lastGame", date);
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
        setJsonExtra("lastGame", date);

        int wins = objArena["wins"].toInt();
        int losses = objArena["losses"].toInt();
        if(isCompleteArena(wins, losses))
        {
            QString heroLog = objArena["hero"].toString();
            int classOrder = Utility::classLogNumber2classOrder(heroLog);
            if(classOrder != -1)
            {
                ScoreButton::addRun(classOrder, wins, losses);
                emit pDebug("New run for PlayerWinrates - Hero: " + heroLog + " - Wins: " +
                            QString::number(wins) + " - Losses: " + QString::number(losses));
            }
        }
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
    else if(column < 4)
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

    if(!arenaStatLink.contains(item))
    {
        emit pDebug("ERROR: Trying to edit an arena not in arenaStatLink.");
        return;
    }

    if(column == 0)         itemChangedDate(item, column);
    else if(column == 1)    itemChangedHero(item, column);
    else if(column == 2 || column == 3) itemChangedWL(item, column);
    editingColumnText = true;
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
        QString hero = match->captured(0).toLower();
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
        else if(hero.startsWith("dem"))  heroLog = "10";
        else if(hero.startsWith("dea"))  heroLog = "11";

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
    setColumnText(item, column, QString::number(score));
    setColorWrongArena(item);
}


void ArenaHandler::arenaStatsJsonChanged(int index)
{
    QString file = ui->arenaStatsJsonComboBox->itemData(index).toString();
    loadStatsJsonFile(file);
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
        ui->arenaRegionComboBox->setHidden(true);
    }
}


void ArenaHandler::itemSelectionChanged()
{
    QList<QTreeWidgetItem *> items = ui->arenaTreeWidget->selectedItems();
    if(items.count() == 1 && arenaStatLink.contains(items[0]))
    {
        int region = statsJson[arenaStatLink[items[0]]].toObject()["region"].toInt();
        ui->arenaRegionComboBox->setCurrentIndex(region);
        ui->arenaRegionComboBox->setHidden(!premium);
        ui->arenaDeleteButton->setHidden(false);
    }
    else
    {
        ui->arenaRegionComboBox->setHidden(true);
        ui->arenaDeleteButton->setHidden(true);
    }
}


void ArenaHandler::deselectRow()
{
    ui->arenaTreeWidget->clearSelection();
}


void ArenaHandler::arenaNewEmpty()
{
    int ret = QMessageBox::question(ui->tabArena, "New Arena?",
                                   "Do you want to create a new arena?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if(ret == QMessageBox::Cancel)  return;

    QString hero = "01";
    QTreeWidgetItem *item = showArena(hero, "", 0, 0, 0, 0, true);
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
        ui->arenaDeleteButton->setHidden(true);
    }
}


int ArenaHandler::getRegionTreeItemIndex(QTreeWidgetItem *item)
{
    for(int i=0; i<NUM_REGIONS; i++)
    {
        if(item == best30RegionTreeItem[i]) return i;
    }
    return -1;
}


bool ArenaHandler::isLbRegionTreeItem(QTreeWidgetItem *item)
{
    for(int i=0; i<3; i++)
    {
        if(item == lbRegionTreeItem[i]) return true;
    }
    return false;
}


void ArenaHandler::statItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    int regionIndex = getRegionTreeItemIndex(item);
    if(column == 0 && (regionIndex != -1 || isLbRegionTreeItem(item)))
    {
        editingColumnText = true;
        ui->arenaStatsTreeWidget->editItem(item, column);
    }
    else if(column > 0 && isLbRegionTreeItem(item))
    {
        QString page = getColumnText(item, 3);
        QString region = getColumnText(item, 4);
        if(page != "--")
        {
            QDesktopServices::openUrl(QUrl(QString(LEADERBOARD_URL2) + "?region=" + region + "&leaderboardId=arena&page=" +
                                           page + "&seasonId=" + QString::number(seasonId)));
        }
    }
}


void ArenaHandler::statItemChanged(QTreeWidgetItem *item, int column)
{
    if(!editingColumnText)  return;

    int regionIndex = getRegionTreeItemIndex(item);
    if(column != 0) return;

    if(regionIndex != -1)
    {
        QString text = getColumnText(item, column);
        QString region = getJsonExtraRegion(regionIndex);
        if(text != region)
        {
            setJsonExtra("region" + QString::number(regionIndex), text);
            saveStatsJsonFile();
        }
    }
    else if(isLbRegionTreeItem(item))
    {
        QString tag = getColumnText(item, column);
        if(!tag.isEmpty() && tag != searchTag)
        {
            bool searchOk = searchLeaderboard(tag);
            if(searchOk)
            {
                for(int i=0; i<3; i++)
                {
                    if(searchPage[i] > 0)   lbTagPulsing(i);
                }
            }

            showLeaderboardStats(tag, true);
        }
    }
    editingColumnText = true;
}


void ArenaHandler::startProcessArenas2Stats()
{
    if(!futureProcessArenas2Stats.isRunning())
    {
        futureProcessArenas2Stats.setFuture(QtConcurrent::run(this, &ArenaHandler::processArenas2Stats));
    }
}


void ArenaHandler::processArenas2Stats()
{
    //Class winrates
    int classRuns[NUM_HEROS] = {0};
    int classWins[NUM_HEROS] = {0};
    int classLost[NUM_HEROS] = {0};

    //Best 30
    int regionRuns[NUM_REGIONS] = {0};
    int regionWins[NUM_REGIONS] = {0};
    float regionLBWins[NUM_REGIONS] = {0};

    //Load arenas
    for(const QString &date: (const QStringList)statsJson.keys())
    {
        if(date == "extra")  continue;

        QJsonObject objArena = statsJson[date].toObject();
        int wins = objArena["wins"].toInt();
        int losses = objArena["losses"].toInt();
        if(!isCompleteArena(wins, losses))  continue;

        QString heroLog = objArena["hero"].toString();
        int region = objArena["region"].toInt();
        QString dateTitle = (date == "current")?getJsonExtra("lastGame"):date;
        dateTitle = QDateTime::fromString(dateTitle, "yyyy.MM.dd hh:mm").toString("d MMM");

        //Class winrates
        int heroInt = Utility::classLogNumber2classOrder(heroLog);
        if(heroInt != -1)
        {
            classRuns[heroInt]++;
            classWins[heroInt] += wins;
            classLost[heroInt] += losses;
        }

        //Best 30
        if(region>-1 && region<NUM_REGIONS)
        {
            regionRuns[region]++;
            regionWins[region] += wins;

            if(regionRuns[region] == NUM_BEST_ARENAS)
            {
                regionLBWins[region] = regionWins[region];
            }
            else if(regionRuns[region] > NUM_BEST_ARENAS)
            {
                regionLBWins[region] = regionLBWins[region]*(NUM_BEST_ARENAS-1)/NUM_BEST_ARENAS;
                regionLBWins[region] += wins;
            }
        }
    }

    showArenas2StatsClass(classRuns, classWins, classLost);
    showArenas2StatsBest30(regionRuns, regionWins, regionLBWins);
    showLeaderboardStats("", true);
}


void ArenaHandler::showArenas2StatsClass(int classRuns[NUM_HEROS], int classWins[NUM_HEROS], int classLost[NUM_HEROS])
{
    for(int i=0; i<NUM_HEROS; i++)
    {
        QTreeWidgetItem *item = winrateClassTreeItem[i];
        int runs = classRuns[i];
        if(runs > 0)
        {
            int wins = classWins[i];
            int losses = classLost[i];

            setColumnText(item, 0, QString::number(static_cast<float>(wins*100)/(wins+losses), 'g', 3) + '%');
            setColumnText(item, 1, QString::number(static_cast<float>(wins)/runs, 'g', 3));
            setColumnText(item, 2, QString::number(runs));
            setColumnText(item, 3, QString::number(wins));
            setColumnText(item, 4, QString::number(losses));
            item->setHidden(false);
        }
        else    item->setHidden(true);
    }

    winrateTreeItem->sortChildren(0, Qt::DescendingOrder);
}


void ArenaHandler::showArenas2StatsBest30(int regionRuns[NUM_REGIONS], int regionWins[NUM_REGIONS], float regionLBWins[NUM_REGIONS])
{
    for(int i=0; i<NUM_REGIONS; i++)
    {
        QTreeWidgetItem *item = best30RegionTreeItem[i];
        int runs = regionRuns[i];
        if(runs > 0)
        {
            int wins = regionWins[i];
            setColumnText(item, 1, QString::number(static_cast<float>(wins)/runs, 'g', 3));
            if(runs >= NUM_BEST_ARENAS)
            {
                float lbWins = regionLBWins[i];
                setColumnText(item, 2, QString::number(static_cast<float>(lbWins)/NUM_BEST_ARENAS, 'g', 3));
            }
            else    setColumnText(item, 2, "--");
            setColumnText(item, 3, QString::number(runs));
            item->setHidden(false);
        }
        else    item->setHidden(true);
    }
}


void ArenaHandler::showLeaderboardStats(QString tag, bool allowTagChange)
{
    if(!downloadLB)
    {
        for(int i=0; i<3; i++)  lbRegionTreeItem[i]->setHidden(true);
        lbTreeItem->setHidden(true);
        return;
    }

    lbTreeItem->setHidden(false);
    if(tag.isEmpty())   tag = getPlayerName();
    bool empty = true;

    if(!allowTagChange)
    {
        bool sameTag = false;
        for(int i=0; i<3; i++)
        {
            QTreeWidgetItem *item = lbRegionTreeItem[i];
            if(!item->isHidden() && getColumnText(item, 0) == tag)
            {
                sameTag = true;
            }
        }
        if(!sameTag)    return;
    }

    for(int i=0; i<3; i++)
    {
        QTreeWidgetItem *item = lbRegionTreeItem[i];
        if(leaderboardMap[i].contains(tag))
        {
            float avg = leaderboardMap[i][tag].rating;
            int rank = leaderboardMap[i][tag].rank;
            int page = qCeil(rank/25.0);
            setColumnText(item, 0, tag, 100);//Aunque la columna tiene 130, si pongo mas de 100 no cabe
            setColumnText(item, 1, QString::number(avg, 'g', 3));
            setColumnText(item, 2, (rank>999)?QString::number(rank/1000)+"k":QString::number(rank));
            setColumnText(item, 3, QString::number(page));
            setColumnText(item, 4, number2LbRegion(i));
            item->setHidden(false);
            empty = false;
        }
        else    item->setHidden(true);
    }

    if(empty)
    {
        lbRegionTreeItem[0]->setHidden(false);
        lbRegionTreeItem[1]->setHidden(true);
        lbRegionTreeItem[2]->setHidden(true);

        QTreeWidgetItem *item = lbRegionTreeItem[0];
        setColumnText(item, 0, tag, 100);//Aunque la columna tiene 130, si pongo mas de 100 no cabe
        setColumnText(item, 1, "--");
        setColumnText(item, 2, "--");
        setColumnText(item, 3, "--");
        setColumnText(item, 4, "--");
    }
    else
    {
        lbTreeItem->sortChildren(0, Qt::DescendingOrder);
    }
}


void ArenaHandler::processPlayerWinrates()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QDate rotationDate = settings.value("rotationDate", QDate(2022,12,6)).toDate();
    emit pDebug("Buiding player winrates.");
    QtConcurrent::run([this,rotationDate]() {
        processWinratesFromDir(rotationDate);
    });
}


void ArenaHandler::processWinratesFromDir(const QDate &rotationDate)
{
    QFileInfo dirInfo(Utility::arenaStatsPath());
    if(!dirInfo.exists())   return;

    QDir dir(Utility::arenaStatsPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name|QDir::Reversed);
    QStringList filterName;
    filterName << "*.json";
    dir.setNameFilters(filterName);

    int classRuns[NUM_HEROS] = {0};
    int classWins[NUM_HEROS] = {0};
    int classLost[NUM_HEROS] = {0};
    QJsonObject jsonObject;

    if(!jsonObjectFromFile(jsonObject, "ArenaTrackerStats.json"))   return;
    bool gamesInRotation = processWinratesFromFile(classRuns, classWins, classLost, jsonObject, rotationDate);

    QStringList files = dir.entryList();
    for(int i=0; i<files.length() && gamesInRotation; i++)
    {
        const QString file = files[i];
        if(file == "ArenaTrackerStats.json")    continue;

        if(!jsonObjectFromFile(jsonObject, file))   continue;
        gamesInRotation = processWinratesFromFile(classRuns, classWins, classLost, jsonObject, rotationDate);
    }

    ScoreButton::setPlayerRuns(classRuns);
    ScoreButton::setPlayerWins(classWins);
    ScoreButton::setPlayerLost(classLost);
}


bool ArenaHandler::processWinratesFromFile(int classRuns[NUM_HEROS], int classWins[NUM_HEROS], int classLost[NUM_HEROS],
                                               QJsonObject &jsonObject, const QDate &rotationDate)
{
    bool gamesInRotation = true;
    for(const QString &date: (const QStringList)jsonObject.keys())
    {
        if(date == "extra")  continue;

        QJsonObject objArena = jsonObject[date].toObject();
        int wins = objArena["wins"].toInt();
        int losses = objArena["losses"].toInt();
        if(!isCompleteArena(wins, losses))  continue;

        QString dateS;
        if(date == "current")   dateS = jsonObject["extra"].toObject()["lastGame"].toString();
        else                    dateS = date;
        QDate dateD = QDate::fromString(dateS.left(10), "yyyy.MM.dd");

        if(rotationDate<=dateD)
        {
            QString heroLog = objArena["hero"].toString();
            int heroInt = Utility::classLogNumber2classOrder(heroLog);
            if(heroInt != -1)
            {
                classRuns[heroInt]++;
                classWins[heroInt] += wins;
                classLost[heroInt] += losses;
            }
        }
        else    gamesInRotation = false;
    }

    return gamesInRotation;
}


//***********************************************
//              Leaderboard load
//***********************************************
void ArenaHandler::mapLeaderboard()
{
    loadSeasonId();
    loadMapLeaderboard();

    for(int i=0; i<3; i++)
    {
        QString region = number2LbRegion(i);
        int page = leaderboardPage[i]+1;
        emit pDebug("Leaderboard ++ START ++: " + region + " --> P" + QString::number(page) + " --> S" + QString::number(seasonId));
        getLeaderboardPage(nmLbGlobal, region, page);
    }
}


void ArenaHandler::nmLbGlobalFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if(reply->error() != QNetworkReply::NoError)
    {
        pDebug(reply->url().toString() + " --> Failed. Retrying...");
        nmLbGlobal->get(QNetworkRequest(reply->url()));
    }
    else
    {
        replyMapLeaderboard(reply);
    }
}


void ArenaHandler::replyMapLeaderboard(QNetworkReply *reply)
{
    QString fullUrl = reply->url().toString();
    if(fullUrl.contains(QRegularExpression("region=(\\w+)&leaderboardId=arena&page=([0-9]+)&seasonId=([0-9]+)"), match))
    {
        QString region = match->captured(1);
        int page = match->captured(2).toInt();
        int season = match->captured(3).toInt();
        int regionNum = lbRegion2Number(region);
        emit pDebug("Leaderboard: " + region + " --> P" + QString::number(page) + " --> S" + QString::number(season));

        //Se ha registrado el cambio de season en medio de descarga de leaderboard, paramos para no guardar informacion de la season pasada que no se borrara
        if(season != seasonId)
        {
            emit pDebug("Leaderboard: Season changed while downloading leaderboard. Abort download.");
            return;
        }
        //Hay varios get repetidos running, puede ocurrir si se desactiva y activa rapido el checkbox ui->configCheckLB
        if(leaderboardPage[regionNum] == page)
        {
            emit pDebug("Leaderboard: Remove duplicate download reply on page: " + QString::number(page));
            return;
        }
        //Download leaderboard disabled
        if(!downloadLB)
        {
            saveMapLeaderboard();
            return;
        }

        QJsonArray rows = QJsonDocument::fromJson(reply->readAll()).object().value("leaderboard").toObject().value("rows").toArray();
        if(rows.isEmpty())
        {
            emit pDebug("Leaderboard: END");
            leaderboardPage[regionNum] = 0;//All pages read
            saveMapLeaderboard();
        }
        else
        {
            leaderboardPage[regionNum] = page;
            getLeaderboardPage(nmLbGlobal, region, page+1);

            for(const QJsonValue &row: rows)
            {
                QJsonObject object = row.toObject();
                QString tag = object["accountid"].toString();
                LeaderboardItem item;
                item.rank = object["rank"].toInt();
                item.rating = object["rating"].toDouble();
                leaderboardMap[regionNum][tag] = item;
            }
        }
    }
}


//Compartido por leaderboard stat search y enemy rank search lo que la hace algo problematica
bool ArenaHandler::searchLeaderboard(const QString &searchTag)
{
    if(!downloadLB) return false;

    for(int i=0; i<3; i++)
    {
        if(searchPage[i] != 0)
        {
            emit pDebug("Leaderboard Search ++ BUSY WITH ++: " + this->searchTag + ". Abort new search: " + searchTag);
            return false;
        }
    }

    QString searchRegion;
    this->searchTag = searchTag;

    int found = false;

    for(int i=0; i<3; i++)
    {
        searchPage[i] = 0;
        if(leaderboardMap[i].contains(searchTag))
        {
            int rank = leaderboardMap[i][searchTag].rank;
            searchPage[i] = qCeil(rank/25.0);
            searchRegion = number2LbRegion(i);
            emit pDebug("Leaderboard Search ++ START ++: " + searchTag + " --> " + searchRegion + " --> P" + QString::number(searchPage[i]) + " --> S" + QString::number(seasonId));
            if(searchPage[i] > 1)   getLeaderboardPage(nmLbSearch, searchRegion, searchPage[i]-1);
            getLeaderboardPage(nmLbSearch, searchRegion, searchPage[i]);
            getLeaderboardPage(nmLbSearch, searchRegion, searchPage[i]+1);
            found = true;
        }
    }
    if(!found)  emit pDebug("Leaderboard Search ++ NOT IN MAP ++: " + searchTag);
    return true;
}


void ArenaHandler::nmLbSearchFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if(reply->error() != QNetworkReply::NoError)
    {
        pDebug(reply->url().toString() + " --> Failed. Retrying...");
        nmLbSearch->get(QNetworkRequest(reply->url()));
    }
    else
    {
        replySearchLeaderboard(reply);
    }
}


void ArenaHandler::replySearchLeaderboard(QNetworkReply *reply)
{
    QString fullUrl = reply->url().toString();
    if(fullUrl.contains(QRegularExpression("region=(\\w+)&leaderboardId=arena&page=([0-9]+)&seasonId=([0-9]+)"), match))
    {
        QString region = match->captured(1);
        int page = match->captured(2).toInt();
        int season = match->captured(3).toInt();
        int regionNum = lbRegion2Number(region);
        emit pDebug("Leaderboard Search: " + region + " --> P" + QString::number(page) + " --> S" + QString::number(season));

        //Se ha registrado el cambio de season en medio de descarga de leaderboard, paramos para no guardar informacion de la season pasada que no se borrara
        if(season != seasonId)
        {
            emit pDebug("Leaderboard Search: Season changed while downloading leaderboard. Abort download.");
            return;
        }

        QJsonArray rows = QJsonDocument::fromJson(reply->readAll()).object().value("leaderboard").toObject().value("rows").toArray();
        if(rows.isEmpty())  emit pDebug("Leaderboard Search STOP LIMIT - RIGHT");
        else
        {
            for(const QJsonValue &row: rows)
            {
                QJsonObject object = row.toObject();
                QString tag = object["accountid"].toString();
                LeaderboardItem item;
                item.rank = object["rank"].toInt();
                item.rating = object["rating"].toDouble();
                leaderboardMap[regionNum][tag] = item;
                if(tag == searchTag)
                {
                    emit pDebug("Leaderboard Search FOUND");
                    searchPage[regionNum] = 0;
                    showLeaderboardStats(searchTag, false);//Update leaderboard treewidget
                    ui->enemyRankingLabel->updateRankingItem(searchTag, EnemyRankingItem(item.rank,item.rating,region));//Update EnemyRanking
                }
            }
            if(searchPage[regionNum] > 0)
            {
                int nextPage;
                if(page == searchPage[regionNum])       nextPage = 0;
                else if(page > searchPage[regionNum])   nextPage = page + 1;
                else                                    nextPage = page - 1;

                if(nextPage > 0)    getLeaderboardPage(nmLbSearch, region, nextPage);
                else                emit pDebug("Leaderboard Search STOP LIMIT - MIDDLE/LEFT");
            }
            else    emit pDebug("Leaderboard Search STOP FOUND");
        }
    }
}


/*
 * {
 * "EU" -> ObjectPlayers
 * "US" -> ObjectPlayers
 * "AP" -> ObjectPlayers
 * "pageEU" -> int
 * "pageUS" -> int
 * "pageAP" -> int
 * }
 *
 * ObjectPlayers
 * {
 * "tag1" -> ObjectItem
 * "tag2" -> ObjectItem
 * ...
 * }
 *
 * ObjectItem
 * {
 * "rank" -> int (1)
 * "rating" -> int (725) -> (7.25)
 * }
 */
bool ArenaHandler::loadMapLeaderboard()
{
    clearLeaderboardMap();

    QFile jsonFile(Utility::extraPath() + "/leaderboard.json");
    if(!jsonFile.exists())
    {
        emit pDebug("leaderboard.json doesn't exists.");
        return true;
    }

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Failed to load leaderboard.json from disk.", DebugLevel::Error);
        return false;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonObject jsonObject = jsonDoc.object();
    for(int i=0; i<3; i++)
    {
        json2RegionLeaderboard(jsonObject, number2LbRegion(i));
    }
    return true;
}


void ArenaHandler::clearLeaderboardMap()
{
    for(int i=0; i<3; i++)
    {
        leaderboardPage[i] = 0;
        leaderboardMap[i].clear();
    }
}


void ArenaHandler::json2RegionLeaderboard(const QJsonObject &jsonObject, const QString &region)
{
    int regionNum = lbRegion2Number(region);
    leaderboardPage[regionNum] = jsonObject["page"+region].toInt();
    QJsonObject objectPlayers = jsonObject[region].toObject();

    for(const QString &tag: objectPlayers.keys())
    {
        QJsonObject objectItem = objectPlayers[tag].toObject();
        LeaderboardItem item;
        item.rank = objectItem["rank"].toInt();
        item.rating = objectItem["rating"].toInt()/100.0;
        leaderboardMap[regionNum][tag] = item;
    }
}


void ArenaHandler::saveMapLeaderboard()
{
    QJsonObject regions;

    for(int i=0; i<3; i++)
    {
        QString region = number2LbRegion(i);
        regions[region] = regionLeaderboard2Json(i);
        regions["page"+region] = leaderboardPage[i];
    }

    //Build json data from statsJson
    QJsonDocument jsonDoc;
    jsonDoc.setObject(regions);

    //Save to disk
    QFile jsonFile(Utility::extraPath() + "/leaderboard.json");
    if(jsonFile.exists())   jsonFile.remove();

    if(!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pDebug("Failed to create leaderboard.json on disk.", DebugLevel::Error);
        return;
    }
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    emit pDebug("leaderboard.json updated.");
}

QJsonObject ArenaHandler::regionLeaderboard2Json(int numMap)
{
    QJsonObject region;
    for(const QString &key: (const QStringList)leaderboardMap[numMap].keys())
    {
        int rank = leaderboardMap[numMap][key].rank;
        int rating = static_cast<int>(leaderboardMap[numMap][key].rating*100);
        QJsonObject item;
        item["rank"] = rank;
        item["rating"] = rating;
        region[key] = item;
    }
    return region;
}


void ArenaHandler::getLeaderboardPage(QNetworkAccessManager *nm, QString region, int page)
{
    nm->get(QNetworkRequest(QUrl(QString(LEADERBOARD_URL) + "?region=" + region + "&"
            "leaderboardId=arena&page=" + QString::number(page) + "&seasonId=" + QString::number(seasonId))));
}


int ArenaHandler::lbRegion2Number(QString region)
{
    if(region == "EU")      return 0;
    else if(region == "US") return 1;
    else/*(region == "AP")*/ return 2;
}


QString ArenaHandler::number2LbRegion(int regionNum)
{
    if(regionNum == 0)      return "EU";
    else if(regionNum == 1) return "US";
    else/*(regionNum == 2)*/return "AP";
}


void ArenaHandler::changeSeasonId(int season)
{
    this->seasonId = season;//Por si ya estamos descargando el leaderboard
    clearLeaderboardMap();
    saveMapLeaderboard();//Clear the map on disk
}


QString ArenaHandler::getPlayerName()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    return settings.value("playerName", "").toString();
}


void ArenaHandler::loadSeasonId()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    seasonId = settings.value("seasonId", 38).toInt();
}


void ArenaHandler::lbTagPulsing(int index)
{
    static int r = 0;
    r = (r+4)%256;//0-254
    int pr = qAbs(r-127)+128;//127-0-127//255-128-255
    lbRegionTreeItem[index]->setForeground(0, QColor(pr, pr, pr));
    if(searchPage[index] > 0)   QTimer::singleShot(40, this, [=] () {lbTagPulsing(index); });
    else    setRowColor(lbRegionTreeItem[index], 0);
}


void ArenaHandler::showEnemyRanking(QString tag)
{
    if(!premium || !downloadLB)     return;
    bool searchOk = searchLeaderboard(tag);
    QList<EnemyRankingItem> enemyRankingItems;

    for(int i=0; i<3; i++)
    {
        if(leaderboardMap[i].contains(tag))
        {
            float rating = leaderboardMap[i][tag].rating;
            int rank = leaderboardMap[i][tag].rank;
            QString region = number2LbRegion(i);
            enemyRankingItems << EnemyRankingItem(rank, rating, region, searchOk);
        }
    }

    ui->enemyRankingLabel->setEnemyRanking(tag, enemyRankingItems);
}


void ArenaHandler::hideEnemyRanking()
{
    ui->enemyRankingLabel->hideEnemyRanking();
    if(searchTag == ui->enemyRankingLabel->getTag())
    {
        for(int i=0; i<3; i++)  searchPage[i] = 0;
    }
}


void ArenaHandler::setDownloadLB(bool value)
{
    this->downloadLB = value;

    if(downloadLB)
    {
        if(premium) mapLeaderboard();
    }
    else
    {
        hideEnemyRanking();
        for(int i=0; i<3; i++)  searchPage[i] = 0;
    }
}
