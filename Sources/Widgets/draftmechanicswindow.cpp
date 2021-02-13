#include "draftmechanicswindow.h"
#include "../themehandler.h"
#include <QtWidgets>


DraftMechanicsWindow::DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex,
                                           bool patreonVersion) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    this->patreonVersion = patreonVersion;
    scoreWidth = static_cast<int>(sizeCard.width()*0.7);

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    resize(static_cast<int>(rect.width() * 1.3),
           static_cast<int>(rect.height() * 1.1));
    move(static_cast<int>(rectScreen.x() + rect.x() - 0.15*rect.width()),
         static_cast<int>(rectScreen.y() + rect.y() - 0.05*rect.height() - 1.6*sizeCard.height()));


    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *centralLayout = new QHBoxLayout();
    QVBoxLayout *adjustLayout = new QVBoxLayout(centralWidget);
    adjustLayout->addStretch();
    adjustLayout->addLayout(centralLayout);
    adjustLayout->addStretch();

    //Help mark
    helpMark = new HoverLabel(this);
    helpMark->setPixmap(QPixmap(":Images/secretHunter.png").scaledToWidth(scoreWidth/2, Qt::SmoothTransformation));
    helpMark->move(width() - static_cast<int>(scoreWidth*0.775), static_cast<int>(scoreWidth*0.235));
    helpMark->resize(scoreWidth/2, scoreWidth/2);
    helpMark->hide();
    connect(helpMark, SIGNAL(enter()),
            this, SLOT(showHelp()));
    connect(helpMark, SIGNAL(leave()),
            this, SLOT(hideHelp()));


    //Card Types
    QGridLayout *cardTypeLayout = new QGridLayout();

    cardTypeCounters = new DraftItemCounter *[M_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, "Minion", cardTypeLayout, 0, 0, QPixmap(ThemeHandler::minionsCounterFile()), scoreWidth/2);
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, "Spell", cardTypeLayout, 0, 1, QPixmap(ThemeHandler::spellsCounterFile()), scoreWidth/2);
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, "Weapon", cardTypeLayout, 1, 0, QPixmap(ThemeHandler::weaponsCounterFile()), scoreWidth/2);
    manaCounter = new DraftItemCounter(this, "Mana AVG", cardTypeLayout, 1, 1, QPixmap(ThemeHandler::manaCounterFile()), scoreWidth/2, false);

//    connect(cardTypeCounters[V_MINION], SIGNAL(iconEnter(QList<SynergyCard>&,QRect &)),
//            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect &)));
    connect(cardTypeCounters[V_SPELL], SIGNAL(iconEnter(QList<SynergyCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect &)));
    connect(cardTypeCounters[V_WEAPON], SIGNAL(iconEnter(QList<SynergyCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect &)));

//    connect(cardTypeCounters[V_MINION], SIGNAL(iconLeave()),
//            this, SIGNAL(itemLeave()));
    connect(cardTypeCounters[V_SPELL], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(cardTypeCounters[V_WEAPON], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));


    //SCORES
    lavaButton = new LavaButton(centralWidget, 3, 5.5);
    lavaButton->setFixedHeight(scoreWidth);
    lavaButton->setFixedWidth(scoreWidth);
    lavaButton->setToolTip("Deck weight");

    scoreButtonLF = new ScoreButton(centralWidget, Score_LightForge);
    scoreButtonLF->setFixedHeight(scoreWidth);
    scoreButtonLF->setFixedWidth(scoreWidth);
    scoreButtonLF->setScore(0, 0);
    scoreButtonLF->setToolTip("LightForge deck average");

    scoreButtonHA = new ScoreButton(centralWidget, Score_HearthArena);
    scoreButtonHA->setFixedHeight(scoreWidth);
    scoreButtonHA->setFixedWidth(scoreWidth);
    scoreButtonHA->setScore(0, 0);
    scoreButtonHA->setToolTip("HearthArena deck average");

    scoreButtonHSR = new ScoreButton(centralWidget, Score_HSReplay);
    scoreButtonHSR->setFixedHeight(scoreWidth);
    scoreButtonHSR->setFixedWidth(scoreWidth);
    scoreButtonHSR->setScore(0, 0);
    scoreButtonHSR->setToolTip("HSReplay winrate deck average");

    QHBoxLayout *scoresLayout = new QHBoxLayout();
    scoresLayout->addWidget(lavaButton);
    scoresLayout->addWidget(scoreButtonLF);
    scoresLayout->addWidget(scoreButtonHA);
    scoresLayout->addWidget(scoreButtonHSR);

    //Patreon
    if(!patreonVersion)
    {
        QPushButton *patreonButton = new QPushButton(centralWidget);
        patreonButton->setFlat(true);
        patreonButton->setIcon(QIcon(":/Images/becomePatreon.png"));
        patreonButton->setIconSize(QSize(217, 51));
        patreonButton->setToolTip("Unlock Synergies and draft mechanics becoming a patron (3$)");
        scoresLayout->addWidget(patreonButton);

        connect(patreonButton, SIGNAL(clicked()),
                this, SIGNAL(showPremiumDialog()));
    }

    //Mechanics & drops
    QGridLayout *mechanicsLayout = new QGridLayout();

    dropCounters = new DraftDropCounter *[V_NUM_DROPS];
    dropCounters[V_DROP2] = new DraftDropCounter(this, "2 drop", mechanicsLayout, 0, 0, TARGET_DROP_2,
                                                 QPixmap(ThemeHandler::drop2CounterFile()), scoreWidth/2);
    dropCounters[V_DROP3] = new DraftDropCounter(this, "3 drop", mechanicsLayout, 0, 1, TARGET_DROP_3,
                                                 QPixmap(ThemeHandler::drop3CounterFile()), scoreWidth/2);
    dropCounters[V_DROP4] = new DraftDropCounter(this, "4 drop", mechanicsLayout, 0, 2, TARGET_DROP_4,
                                                 QPixmap(ThemeHandler::drop4CounterFile()), scoreWidth/2);

    connect(dropCounters[V_DROP2], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(dropCounters[V_DROP3], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(dropCounters[V_DROP4], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));

    connect(dropCounters[V_DROP2], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(dropCounters[V_DROP3], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(dropCounters[V_DROP4], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_REACH] = new DraftItemCounter(this, "Reach", mechanicsLayout, 1, 0, QPixmap(ThemeHandler::reachMechanicFile()), scoreWidth/2);
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(this, "Taunt", mechanicsLayout, 1, 1, QPixmap(ThemeHandler::tauntMechanicFile()), scoreWidth/2);
    mechanicCounters[V_SURVIVABILITY] = new DraftItemCounter(this, "Survival", mechanicsLayout, 1, 2, QPixmap(ThemeHandler::survivalMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, "Draw", mechanicsLayout, 1, 3, QPixmap(ThemeHandler::drawMechanicFile()), scoreWidth/2);

    mechanicCounters[V_PING] = new DraftItemCounter(this, "Ping", mechanicsLayout, 2, 0, QPixmap(ThemeHandler::pingMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(this, "Removal", mechanicsLayout, 2, 1, QPixmap(ThemeHandler::damageMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DESTROY] = new DraftItemCounter(this, "Hard removal", mechanicsLayout, 2, 2, QPixmap(ThemeHandler::destroyMechanicFile()), scoreWidth/2);
    mechanicCounters[V_AOE] = new DraftItemCounter(this, "AOE", mechanicsLayout, 2, 3, QPixmap(ThemeHandler::aoeMechanicFile()), scoreWidth/2);


    connect(mechanicCounters[V_AOE], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_PING], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_REACH], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));

    connect(mechanicCounters[V_AOE], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_PING], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_REACH], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));


    centralLayout->addStretch();
    centralLayout->addLayout(cardTypeLayout);
    centralLayout->addLayout(scoresLayout);
    centralLayout->addLayout(mechanicsLayout);
    centralLayout->addStretch();

    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Mechanics");

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(centralWidget);
    effect->setOpacity(0);
    centralWidget->setGraphicsEffect(effect);
    Utility::fadeInWidget(centralWidget);
    showHelp();
    show();
}


DraftMechanicsWindow::~DraftMechanicsWindow()
{
    deleteDraftItemCounters();
    delete lavaButton;
    delete scoreButtonHA;
    delete scoreButtonLF;
    delete scoreButtonHSR;
    delete helpMark;
}


void DraftMechanicsWindow::deleteDraftItemCounters()
{
    delete manaCounter;

    delete cardTypeCounters[V_MINION];
    delete cardTypeCounters[V_SPELL];
    delete cardTypeCounters[V_WEAPON];
    delete []cardTypeCounters;

    delete dropCounters[V_DROP2];
    delete dropCounters[V_DROP3];
    delete dropCounters[V_DROP4];
    delete []dropCounters;

    delete mechanicCounters[V_AOE];
    delete mechanicCounters[V_TAUNT_ALL];
    delete mechanicCounters[V_SURVIVABILITY];
    delete mechanicCounters[V_DISCOVER_DRAW];

    delete mechanicCounters[V_PING];
    delete mechanicCounters[V_DAMAGE];
    delete mechanicCounters[V_DESTROY];
    delete mechanicCounters[V_REACH];
    delete []mechanicCounters;
}


void DraftMechanicsWindow::setTheme()
{
    if(showingHelp) showHelp();
    else            hideHelp();

    cardTypeCounters[V_MINION]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()), scoreWidth/2, true);
    cardTypeCounters[V_SPELL]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()), scoreWidth/2, true);
    cardTypeCounters[V_WEAPON]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()), scoreWidth/2, true);
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()), scoreWidth/2, true);

    dropCounters[V_DROP2]->setTheme(QPixmap(ThemeHandler::drop2CounterFile()), scoreWidth/2, true);
    dropCounters[V_DROP3]->setTheme(QPixmap(ThemeHandler::drop3CounterFile()), scoreWidth/2, true);
    dropCounters[V_DROP4]->setTheme(QPixmap(ThemeHandler::drop4CounterFile()), scoreWidth/2, true);

    mechanicCounters[V_AOE]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_TAUNT_ALL]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_SURVIVABILITY]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DISCOVER_DRAW]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()), scoreWidth/2, true);

    mechanicCounters[V_PING]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DAMAGE]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DESTROY]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_REACH]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()), scoreWidth/2, true);
}


void DraftMechanicsWindow::setDraftMethodAvgScore(DraftMethod draftMethodAvgScore)
{
    this->draftMethodAvgScore = draftMethodAvgScore;
    if(showingHelp) return;

    scoreButtonLF->hide();
    scoreButtonHA->hide();
    scoreButtonHSR->hide();

    switch(draftMethodAvgScore)
    {
        case LightForge:
            scoreButtonLF->show();
        break;
        case HearthArena:
            scoreButtonHA->show();
        break;
        case HSReplay:
            scoreButtonHSR->show();
        break;
        default:
        break;
    }
}


void DraftMechanicsWindow::setShowDrops(bool value)
{
    this->showDrops = value;
    if(showingHelp) showHelp();
    else            hideHelp();
}


void DraftMechanicsWindow::setScores(int deckScoreHA, int deckScoreLF, float deckScoreHSR)
{
    scoreButtonLF->setScore(deckScoreLF, deckScoreLF);
    scoreButtonHA->setScore(deckScoreHA, deckScoreHA);
    scoreButtonHSR->setScore(deckScoreHSR, deckScoreHSR);
}


void DraftMechanicsWindow::showHelp()
{
    showingHelp = true;
    centralWidget()->setStyleSheet(".QWidget{border-image: url(" +
                                 (showDrops?ThemeHandler::bgDraftMechanicsHelpDropsFile():ThemeHandler::bgDraftMechanicsHelpFile()) +
                                 ") 0 0 0 0 stretch stretch;border-width: 0px;}");
    scoreButtonLF->hide();
    scoreButtonHA->hide();
    scoreButtonHSR->hide();
    lavaButton->hide();

    cardTypeCounters[V_MINION]->hide();
    cardTypeCounters[V_SPELL]->hide();
    cardTypeCounters[V_WEAPON]->hide();
    manaCounter->hide();

    dropCounters[V_DROP2]->hide();
    dropCounters[V_DROP3]->hide();
    dropCounters[V_DROP4]->hide();

    mechanicCounters[V_REACH]->hide();
    mechanicCounters[V_TAUNT_ALL]->hide();
    mechanicCounters[V_SURVIVABILITY]->hide();
    mechanicCounters[V_DISCOVER_DRAW]->hide();

    mechanicCounters[V_PING]->hide();
    mechanicCounters[V_DAMAGE]->hide();
    mechanicCounters[V_DESTROY]->hide();
    mechanicCounters[V_AOE]->hide();
}


void DraftMechanicsWindow::hideHelp()
{
    showingHelp = false;
    centralWidget()->setStyleSheet(".QWidget{border-image: url(" +
                                   ThemeHandler::bgDraftMechanicsFile() +
                                   ") 0 0 0 0 stretch stretch;border-width: 0px;}");
    setDraftMethodAvgScore(this->draftMethodAvgScore);
    lavaButton->show();
    helpMark->show();

    cardTypeCounters[V_MINION]->show();
    cardTypeCounters[V_SPELL]->show();
    cardTypeCounters[V_WEAPON]->show();
    manaCounter->show();

    if(showDrops)
    {
        mechanicCounters[V_REACH]->hide();
        mechanicCounters[V_TAUNT_ALL]->hide();
        mechanicCounters[V_SURVIVABILITY]->hide();
        mechanicCounters[V_DISCOVER_DRAW]->hide();

        dropCounters[V_DROP2]->show();
        dropCounters[V_DROP3]->show();
        dropCounters[V_DROP4]->show();
    }
    else
    {
        dropCounters[V_DROP2]->hide();
        dropCounters[V_DROP3]->hide();
        dropCounters[V_DROP4]->hide();

        mechanicCounters[V_REACH]->show();
        mechanicCounters[V_TAUNT_ALL]->show();
        mechanicCounters[V_SURVIVABILITY]->show();
        mechanicCounters[V_DISCOVER_DRAW]->show();
    }

    mechanicCounters[V_PING]->show();
    mechanicCounters[V_DAMAGE]->show();
    mechanicCounters[V_DESTROY]->show();
    mechanicCounters[V_AOE]->show();
}


void DraftMechanicsWindow::updateDeckWeight(int numCards, int draw, int toYourHand, int discover)
{
    lavaButton->setValue(manaCounter->count(), numCards, draw, toYourHand, discover);

    if(showingHelp) hideHelp();
}


void DraftMechanicsWindow::updateManaCounter(int manaIncrease, int numCards)
{
    manaCounter->increase(manaIncrease, numCards);
}


void DraftMechanicsWindow::updateCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                                    QStringList &drop2List, QStringList &drop3List, QStringList &drop4List,
                                    QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                    QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList,
                                    int manaIncrease, int numCards)
{
    for(const QString &code: spellList)     cardTypeCounters[V_SPELL]->increase(code);
    for(const QString &code: minionList)    cardTypeCounters[V_MINION]->increase(code);
    for(const QString &code: weaponList)    cardTypeCounters[V_WEAPON]->increase(code);

    for(const QString &code: drop2List)     dropCounters[V_DROP2]->increase(code);
    for(const QString &code: drop3List)     dropCounters[V_DROP3]->increase(code);
    for(const QString &code: drop4List)     dropCounters[V_DROP4]->increase(code);

    for(const QString &code: aoeList)       mechanicCounters[V_AOE]->increase(code);
    for(const QString &code: tauntList)     mechanicCounters[V_TAUNT_ALL]->increase(code);
    for(const QString &code: survivabilityList) mechanicCounters[V_SURVIVABILITY]->increase(code);
    for(const QString &code: drawList)      mechanicCounters[V_DISCOVER_DRAW]->increase(code);

    for(const QString &code: pingList)      mechanicCounters[V_PING]->increase(code);
    for(const QString &code: damageList)    mechanicCounters[V_DAMAGE]->increase(code);
    for(const QString &code: destroyList)   mechanicCounters[V_DESTROY]->increase(code);
    for(const QString &code: reachList)     mechanicCounters[V_REACH]->increase(code);

    updateManaCounter(manaIncrease, numCards);

    for(int i=0; i<V_NUM_DROPS; i++)        dropCounters[i]->setNumCards(numCards);
}


void DraftMechanicsWindow::sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect)
{
    QPoint topLeftWindow = this->mapToGlobal(QPoint(0,0));
    QPoint bottomRightWindow = this->mapToGlobal(QPoint(width(),height()));
    int iconCenterX = labelRect.x() + labelRect.width()/2;
    QPoint originList(iconCenterX, bottomRightWindow.y());

    int maxLeft = topLeftWindow.x();
    int maxRight = bottomRightWindow.x();

    emit itemEnter(synergyCardList, originList, maxLeft, maxRight);
}


int DraftMechanicsWindow::draftedCardsCount()
{
    int num = 0;
    for(int i=0; i<M_NUM_TYPES; i++)
    {
        num += cardTypeCounters[i]->count();
    }
    return num;
}
