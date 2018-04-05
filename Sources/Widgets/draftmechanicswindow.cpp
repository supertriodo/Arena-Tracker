#include "draftmechanicswindow.h"
#include "../themehandler.h"
#include <QtWidgets>


DraftMechanicsWindow::DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex,
                                           bool patreonVersion, DraftMethod draftMethod, bool normalizedLF) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    this->patreonVersion = patreonVersion;
    this->draftMethod = draftMethod;
    scoreWidth = sizeCard.width()*0.7;

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    resize(rect.width() * 1.3,
           rect.height() * 1.1);
    move(rectScreen.x() + rect.x() - 0.15*rect.width(),
         rectScreen.y() + rect.y() - 0.05*rect.height() - 1.6*sizeCard.height());


    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet(".QWidget{border-image: url(" +
                                 (patreonVersion?ThemeHandler::bgDraftMechanicsHelpFile():ThemeHandler::bgDraftMechanicsHelpFile()) +
                                 ") 0 0 0 0 stretch stretch;border-width: 0px;}");
    showingHelp = true;
    QHBoxLayout *centralLayout = new QHBoxLayout();
    QVBoxLayout *adjustLayout = new QVBoxLayout(centralWidget);
    adjustLayout->addStretch();
    adjustLayout->addLayout(centralLayout);
    adjustLayout->addStretch();


    //Card Types
    QGridLayout *cardTypeLayout = new QGridLayout();

    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, cardTypeLayout, 0, 0, QPixmap(ThemeHandler::minionsCounterFile()), scoreWidth/2, false);
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, cardTypeLayout, 0, 1, QPixmap(ThemeHandler::spellsCounterFile()), scoreWidth/2, false);
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, cardTypeLayout, 1, 0, QPixmap(ThemeHandler::weaponsCounterFile()), scoreWidth/2, false);
    manaCounter = new DraftItemCounter(this, cardTypeLayout, 1, 1, QPixmap(ThemeHandler::manaCounterFile()), scoreWidth/2, false);

    //SCORES
    lavaButton = new LavaButton(centralWidget, 3, 5);
    lavaButton->setFixedHeight(scoreWidth);
    lavaButton->setFixedWidth(scoreWidth);
    lavaButton->setValue(0);
    lavaButton->setToolTip("Deck weight");
    lavaButton->hide();

    scoreButtonLF = new ScoreButton(centralWidget, LightForge, normalizedLF);
    scoreButtonLF->setFixedHeight(scoreWidth);
    scoreButtonLF->setFixedWidth(scoreWidth);
    scoreButtonLF->setScore(0, true);
    lavaButton->setToolTip("LightForge deck average");
    scoreButtonLF->hide();

    scoreButtonHA = new ScoreButton(centralWidget, HearthArena, false);
    scoreButtonHA->setFixedHeight(scoreWidth);
    scoreButtonHA->setFixedWidth(scoreWidth);
    scoreButtonHA->setScore(0, true);
    lavaButton->setToolTip("HearthArena deck average");
    scoreButtonHA->hide();

    QHBoxLayout *scoresLayout = new QHBoxLayout();
    scoresLayout->addWidget(lavaButton);
    scoresLayout->addWidget(scoreButtonLF);
    scoresLayout->addWidget(scoreButtonHA);

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

    //Mechanics
    QGridLayout *mechanicsLayout = new QGridLayout();

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_AOE] = new DraftItemCounter(this, mechanicsLayout, 0, 0, QPixmap(ThemeHandler::aoeMechanicFile()), scoreWidth/2);
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(this, mechanicsLayout, 0, 1, QPixmap(ThemeHandler::tauntMechanicFile()), scoreWidth/2);
    mechanicCounters[V_SURVIVABILITY] = new DraftItemCounter(this, mechanicsLayout, 0, 2, QPixmap(ThemeHandler::survivalMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, mechanicsLayout, 0, 3, QPixmap(ThemeHandler::drawMechanicFile()), scoreWidth/2);

    mechanicCounters[V_PING] = new DraftItemCounter(this, mechanicsLayout, 1, 0, QPixmap(ThemeHandler::pingMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(this, mechanicsLayout, 1, 1, QPixmap(ThemeHandler::damageMechanicFile()), scoreWidth/2);
    mechanicCounters[V_DESTROY] = new DraftItemCounter(this, mechanicsLayout, 1, 2, QPixmap(ThemeHandler::destroyMechanicFile()), scoreWidth/2);
    mechanicCounters[V_REACH] = new DraftItemCounter(this, mechanicsLayout, 1, 3, QPixmap(ThemeHandler::reachMechanicFile()), scoreWidth/2);

    connect(mechanicCounters[V_AOE], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_PING], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));
    connect(mechanicCounters[V_REACH], SIGNAL(iconEnter(QList<DeckCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect&)));

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
    show();
}


DraftMechanicsWindow::~DraftMechanicsWindow()
{
    deleteDraftItemCounters();
}


void DraftMechanicsWindow::deleteDraftItemCounters()
{
    delete manaCounter;

    delete cardTypeCounters[V_MINION];
    delete cardTypeCounters[V_SPELL];
    delete cardTypeCounters[V_WEAPON];
    delete []cardTypeCounters;

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
    this->centralWidget()->setStyleSheet(".QWidget{border-image: url(" + ThemeHandler::bgDraftMechanicsFile() + ") 0 0 0 0 stretch stretch;border-width: 0px;}");

    cardTypeCounters[V_MINION]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()), scoreWidth/2, true);
    cardTypeCounters[V_SPELL]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()), scoreWidth/2, true);
    cardTypeCounters[V_WEAPON]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()), scoreWidth/2, true);
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()), scoreWidth/2, true);

    mechanicCounters[V_AOE]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_TAUNT_ALL]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_SURVIVABILITY]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DISCOVER_DRAW]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()), scoreWidth/2, true);

    mechanicCounters[V_PING]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DAMAGE]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_DESTROY]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()), scoreWidth/2, true);
    mechanicCounters[V_REACH]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()), scoreWidth/2, true);
}


void DraftMechanicsWindow::setDraftMethod(DraftMethod draftMethod)
{
    this->draftMethod = draftMethod;
    if(showingHelp) return;
    switch(draftMethod)
    {
        case All:
            scoreButtonLF->hide();
            scoreButtonHA->show();
            break;
        case LightForge:
            scoreButtonLF->show();
            scoreButtonHA->hide();
            break;
        case HearthArena:
            scoreButtonLF->hide();
            scoreButtonHA->show();
            break;
        default:
            scoreButtonLF->hide();
            scoreButtonHA->hide();
            break;
    }
}


void DraftMechanicsWindow::setScores(int deckScoreHA, int deckScoreLF)
{
    scoreButtonLF->setScore(deckScoreLF, true);
    scoreButtonHA->setScore(deckScoreHA, true);
}


void DraftMechanicsWindow::updateManaCounter(int numIncrease, int draftedCardsCount)
{
    if(showingHelp)
    {
        showingHelp = false;
        centralWidget()->setStyleSheet(".QWidget{border-image: url(" + ThemeHandler::bgDraftMechanicsFile() + ") 0 0 0 0 stretch stretch;border-width: 0px;}");
        setDraftMethod(this->draftMethod);
        lavaButton->show();
    }
    manaCounter->increase(numIncrease, draftedCardsCount);
    if(draftedCardsCount > 0)   lavaButton->setValue(float(manaCounter->count())/draftedCardsCount);
}


void DraftMechanicsWindow::updateCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                                    QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                    QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList)
{
    for(const QString &code: spellList)     cardTypeCounters[V_SPELL]->increase(code);
    for(const QString &code: minionList)    cardTypeCounters[V_MINION]->increase(code);
    for(const QString &code: weaponList)    cardTypeCounters[V_WEAPON]->increase(code);

    for(const QString &code: aoeList)       mechanicCounters[V_AOE]->increase(code);
    for(const QString &code: tauntList)     mechanicCounters[V_TAUNT_ALL]->increase(code);
    for(const QString &code: survivabilityList) mechanicCounters[V_SURVIVABILITY]->increase(code);
    for(const QString &code: drawList)      mechanicCounters[V_DISCOVER_DRAW]->increase(code);

    for(const QString &code: pingList)      mechanicCounters[V_PING]->increase(code);
    for(const QString &code: damageList)    mechanicCounters[V_DAMAGE]->increase(code);
    for(const QString &code: destroyList)   mechanicCounters[V_DESTROY]->increase(code);
    for(const QString &code: reachList)     mechanicCounters[V_REACH]->increase(code);
}


void DraftMechanicsWindow::clearLists()
{
    manaCounter->reset();

    cardTypeCounters[V_MINION]->reset();
    cardTypeCounters[V_SPELL]->reset();
    cardTypeCounters[V_WEAPON]->reset();

    mechanicCounters[V_AOE]->reset();
    mechanicCounters[V_TAUNT_ALL]->reset();
    mechanicCounters[V_SURVIVABILITY]->reset();
    mechanicCounters[V_DISCOVER_DRAW]->reset();

    mechanicCounters[V_PING]->reset();
    mechanicCounters[V_DAMAGE]->reset();
    mechanicCounters[V_DESTROY]->reset();
    mechanicCounters[V_REACH]->reset();
}


void DraftMechanicsWindow::sendItemEnter(QList<DeckCard> &deckCardList, QRect &labelRect)
{
    QPoint topLeftWindow = this->mapToGlobal(QPoint(0,0));
    QPoint bottomRightWindow = this->mapToGlobal(QPoint(width(),height()));
    int iconCenterX = labelRect.x() + labelRect.width()/2;
    QPoint originList(iconCenterX, bottomRightWindow.y());

    int maxLeft = topLeftWindow.x();
    int maxRight = bottomRightWindow.x();

    emit itemEnter(deckCardList, originList, maxLeft, maxRight);
}


void DraftMechanicsWindow::setNormalizedLF(bool value)
{
    scoreButtonLF->setNormalizedLF(value);
}
