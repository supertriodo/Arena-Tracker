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
    createComboBoxArenaStatsJson();
    completeButtons();

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

    treeWidget->setColumnWidth(0, 110);
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
    treeWidget->setColumnWidth(3, 90);
    treeWidget->setColumnWidth(4, 90);
    treeWidget->setColumnWidth(5, 0);

    //WINRATES
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
    best30TreeItem->setText(0, "Best 30");
    best30TreeItem->setText(1, "Avg");
    best30TreeItem->setText(2, "Runs");
    best30TreeItem->setText(3, "Start");
    best30TreeItem->setText(4, "End");
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
}


void ArenaHandler::toggleArenaStatsTreeWidget()
{
    if(ui->arenaStatsTreeWidget->isHidden())    showArenaStatsTreeWidget();
    else                                        hideArenaStatsTreeWidget();
}


void ArenaHandler::showArenaStatsTreeWidget()
{
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
    //Evitamos mostrar los avg wr todos verdes or rojos
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
        return false;
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


void ArenaHandler::statItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    int regionIndex = getRegionTreeItemIndex(item);
    if(column != 0 || regionIndex == -1)    return;

    ui->arenaStatsTreeWidget->editItem(item, column);
}


void ArenaHandler::statItemChanged(QTreeWidgetItem *item, int column)
{
    int regionIndex = getRegionTreeItemIndex(item);
    if(column != 0 || regionIndex == -1)    return;

    QString text = getColumnText(item, column);
    QString region = getJsonExtraRegion(regionIndex);
    if(text != region)
    {
        setJsonExtra("region" + QString::number(regionIndex), text);
        saveStatsJsonFile();
    }
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
    int best30Runs[NUM_REGIONS] = {0};
    int best30BestWins[NUM_REGIONS] = {0};
    int best30CurrentWins[NUM_REGIONS] = {0};
    QList<int> best30ListWins[NUM_REGIONS];
    QStringList best30ListDates[NUM_REGIONS];
    QString best30Start[NUM_REGIONS] = {""};
    QString best30End[NUM_REGIONS] = {""};

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
            if(best30Runs[region] < NUM_BEST_ARENAS)
            {
                best30Runs[region]++;
                best30CurrentWins[region] += wins;
                best30BestWins[region] = best30CurrentWins[region];
                if(best30Start[region].isEmpty())   best30Start[region] = dateTitle;
                best30End[region] = dateTitle;

                best30ListWins[region].append(wins);
                best30ListDates[region].append(dateTitle);
            }
            else
            {
                int prevWins = best30ListWins[region].takeFirst();
                best30ListDates[region].removeFirst();

                best30CurrentWins[region] = best30CurrentWins[region] - prevWins + wins;
                if(best30CurrentWins[region] >= best30BestWins[region])
                {
                    best30BestWins[region] = best30CurrentWins[region];
                    best30Start[region] = best30ListDates[region].first();
                    best30End[region] = dateTitle;
                }

                best30ListWins[region].append(wins);
                best30ListDates[region].append(dateTitle);
            }

//            qDebug()<<ArenaHandler::getJsonExtraRegion(region)<<best30ListWins[region]<<best30ListDates[region]<<
//                      best30CurrentWins[region]<<best30BestWins[region]<<best30Start[region]<<best30End[region];
        }
    }

    showArenas2StatsClass(classRuns, classWins, classLost);
    showArenas2StatsBest30(best30Runs, best30BestWins, best30Start, best30End);
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


void ArenaHandler::showArenas2StatsBest30(int best30Runs[NUM_REGIONS], int best30BestWins[NUM_REGIONS],
                                          QString best30Start[NUM_REGIONS], QString best30End[NUM_REGIONS])
{
    for(int i=0; i<NUM_REGIONS; i++)
    {
        QTreeWidgetItem *item = best30RegionTreeItem[i];
        int runs = best30Runs[i];
        if(runs > 0)
        {
            int wins = best30BestWins[i];
            QString start = best30Start[i];
            QString end = best30End[i];

            setColumnText(item, 1, QString::number(static_cast<float>(wins)/runs, 'g', 3));
            setColumnText(item, 2, QString::number(runs));
            setColumnText(item, 3, start);
            setColumnText(item, 4, end);
            item->setHidden(false);
        }
        else    item->setHidden(true);
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



