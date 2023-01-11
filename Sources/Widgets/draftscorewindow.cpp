#include "draftscorewindow.h"
#include "../themehandler.h"
#include <QtWidgets>


DraftScoreWindow::DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, int classOrder) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
#ifdef QT_DEBUG
    #if DEBUG_OVERLAYS_LEFT
        screenIndex = 0;
    #endif
#endif

    scoreWidth = static_cast<int>(sizeCard.width()*0.7);

    QRect rectScreen;
    QList<QScreen *> screens = QGuiApplication::screens();
    if(screenIndex < screens.count())
    {
        QScreen *screen = screens[screenIndex];
        if(screen != nullptr)   rectScreen = screen->geometry();
    }

    int midCards = (rect.width() - 3*sizeCard.width())/2;
    resize(rect.width() + 2*MARGIN + midCards,
           rect.height() + 2*MARGIN - (sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN - midCards/2,
         static_cast<int>(rectScreen.y() + rect.y() - MARGIN + 2.65*sizeCard.height()));
    int synergyWidth = this->width()/3.5;  //List Widget need 25 px extra space more than the sizeCard.
    SynergyCard::setSynergyWidth(synergyWidth);


    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *horLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *verLayout[3];

    for(int i=0; i<3; i++)
    {
        //Warning Card
        warningCard[i] = new SynergyCard("");
        warningCard[i]->total = warningCard[i]->remaining = 1;
        warningCard[i]->setSynergyTag("Is this your card?");
        warningCardLabel[i] = new HoverLabel(centralWidget);
        warningCardLabel[i]->hide();

        connect(warningCardLabel[i], SIGNAL(enter(HoverLabel*)),
                this, SLOT(findWarningCardLabelEntered(HoverLabel*)));
        connect(warningCardLabel[i], SIGNAL(leave()),
                this, SIGNAL(cardLeave()));

        //Warning Ok
        warningOkLabel[i] = new HoverLabel(centralWidget);
        QPixmap pixmap(ThemeHandler::checkCardOkFile());
        warningOkLabel[i]->setPixmap(pixmap.scaledToWidth(scoreWidth/2,Qt::SmoothTransformation));
        warningOkLabel[i]->setToolTip("Click ok or look for your card in the combobox.");
        warningOkLabel[i]->hide();
        onWarnMode[i] = false;

        connect(warningOkLabel[i], SIGNAL(click(HoverLabel*)),
                this, SLOT(warningOkClick(HoverLabel*)));

        //Opacity effects
        QGraphicsOpacityEffect *effect;
        effect = new QGraphicsOpacityEffect(warningCardLabel[i]);
        effect->setOpacity(0);
        warningCardLabel[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(warningOkLabel[i]);
        effect->setOpacity(0);
        warningOkLabel[i]->setGraphicsEffect(effect);

        //LAYOUTS warning ok
        QHBoxLayout *horLayoutWarn1 = new QHBoxLayout();
        horLayoutWarn1->addStretch();
        horLayoutWarn1->addWidget(warningCardLabel[i]);
        horLayoutWarn1->addStretch();

        QHBoxLayout *horLayoutWarn2 = new QHBoxLayout();
        horLayoutWarn2->addStretch();
        horLayoutWarn2->addWidget(warningOkLabel[i]);
        horLayoutWarn2->addStretch();

        verLayout[i] = new QVBoxLayout();
        verLayout[i]->addLayout(horLayoutWarn1);
        verLayout[i]->addLayout(horLayoutWarn2);

        //Scores
        scoresPushButton[i] = new ScoreButton(centralWidget, Score_LightForge, -1);
        scoresPushButton[i]->setFixedHeight(scoreWidth);
        scoresPushButton[i]->setFixedWidth(scoreWidth);
        scoresPushButton[i]->hide();
        connect(scoresPushButton[i], SIGNAL(spreadLearningShow(bool)),
                this, SLOT(spreadLearningShow(bool)));

        scoresPushButton2[i] = new ScoreButton(centralWidget, Score_HearthArena, -1);
        scoresPushButton2[i]->setFixedHeight(scoreWidth);
        scoresPushButton2[i]->setFixedWidth(scoreWidth);
        scoresPushButton2[i]->hide();
        connect(scoresPushButton2[i], SIGNAL(spreadLearningShow(bool)),
                this, SLOT(spreadLearningShow(bool)));

        scoresPushButton3[i] = new ScoreButton(centralWidget, Score_HSReplay, classOrder);
        scoresPushButton3[i]->setFixedHeight(scoreWidth);
        scoresPushButton3[i]->setFixedWidth(scoreWidth);
        scoresPushButton3[i]->hide();
        connect(scoresPushButton3[i], SIGNAL(spreadLearningShow(bool)),
                this, SLOT(spreadLearningShow(bool)));
        connect(scoresPushButton3[i], SIGNAL(showHSRwebPicks()),
                this, SIGNAL(showHSRwebPicks()));

        twitchButton[i] = new TwitchButton(centralWidget, 0, 1);
        twitchButton[i]->setFixedHeight(scoreWidth);
        twitchButton[i]->setFixedWidth(scoreWidth);
        twitchButton[i]->hide();

        //Opacity effects
        effect = new QGraphicsOpacityEffect(scoresPushButton[i]);
        effect->setOpacity(0);
        scoresPushButton[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(scoresPushButton2[i]);
        effect->setOpacity(0);
        scoresPushButton2[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(scoresPushButton3[i]);
        effect->setOpacity(0);
        scoresPushButton3[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(twitchButton[i]);
        effect->setOpacity(0);
        twitchButton[i]->setGraphicsEffect(effect);

        //LAYOUTS scores
        horLayoutScores[i] = new QHBoxLayout();
        horLayoutScores[i]->addWidget(scoresPushButton[i]);
        horLayoutScores[i]->addWidget(scoresPushButton3[i]);
        horLayoutScores[i]->addWidget(scoresPushButton2[i]);

        QHBoxLayout *horLayoutScoresG = new QHBoxLayout();
        horLayoutScoresG->addStretch();
        horLayoutScoresG->addLayout(horLayoutScores[i]);
        horLayoutScoresG->addStretch();

        verLayout[i]->addLayout(horLayoutScoresG);

        gridLayoutMechanics[i] = new QGridLayout();
        horLayoutScores2[i] = new QHBoxLayout();
        horLayoutScores2[i]->addWidget(twitchButton[i]);
        horLayoutScores2[i]->addLayout(gridLayoutMechanics[i]);

        QHBoxLayout *horLayoutScores2G = new QHBoxLayout();
        horLayoutScores2G->addStretch();
        horLayoutScores2G->addLayout(horLayoutScores2[i]);
        horLayoutScores2G->addStretch();

        verLayout[i]->addLayout(horLayoutScores2G);

        //Synergies
        synergiesListWidget[i] = new MoveListWidget(centralWidget);
        synergiesListWidget[i]->setFixedHeight(0);
        synergiesListWidget[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setIconSize(QSize(synergyWidth, static_cast<int>(synergyWidth/218.0*35)));
        synergiesListWidget[i]->setMouseTracking(true);
        synergiesListWidget[i]->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        connect(synergiesListWidget[i], SIGNAL(itemEntered(QListWidgetItem*)),
                this, SLOT(findSynergyCardEntered(QListWidgetItem*)));
        connect(synergiesListWidget[i], SIGNAL(leave()),
                this, SIGNAL(cardLeave()));
        connect(synergiesListWidget[i], SIGNAL(leave()),
                this, SLOT(resumeSynergyMotion()));

        synergyMotions[i].running = false;

        //LAYOUTS synergies
        QVBoxLayout *verLayoutSynergy = new QVBoxLayout();
        verLayoutSynergy->addWidget(synergiesListWidget[i]);
        verLayoutSynergy->addStretch();

        QHBoxLayout *horLayoutSynergiesG = new QHBoxLayout();
        horLayoutSynergiesG->addStretch();
        horLayoutSynergiesG->addLayout(verLayoutSynergy);
        horLayoutSynergiesG->addStretch();

        verLayout[i]->addLayout(horLayoutSynergiesG);

        horLayout->addStretch();
        horLayout->addLayout(verLayout[i]);
        horLayout->addStretch();
    }

    hideSynergies();
    maxSynergyHeight1Row = rectScreen.y() + rectScreen.height() - this->y() - 2*MARGIN - 1.7*scoreWidth;
    maxSynergyHeight2Row = rectScreen.y() + rectScreen.height() - this->y() - 2*MARGIN - 2.2*scoreWidth;
    maxSynergyHeight = maxSynergyHeight2Row;
    scores2Rows = true;
    showLF = showHSR = showHA = showTwitch = false;
    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Scores");
}


DraftScoreWindow::~DraftScoreWindow()
{
    for(int i=0; i<3; i++)
    {
        synergiesListWidget[i]->clear();
        synergyCardLists[i].clear();
        delete warningCard[i];
    }
}


void DraftScoreWindow::setLearningMode(bool value)
{
    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setLearningMode(value);
        scoresPushButton2[i]->setLearningMode(value);
        scoresPushButton3[i]->setLearningMode(value);
    }
}


void DraftScoreWindow::spreadLearningShow(bool value)
{
    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setLearningShow(value);
        scoresPushButton2[i]->setLearningShow(value);
        scoresPushButton3[i]->setLearningShow(value);
    }
}


void DraftScoreWindow::checkScoresSpace()
{
    bool oldScores2Rows = scores2Rows;
    scores2Rows = showHA && showLF && showHSR && showTwitch;
    if(oldScores2Rows == scores2Rows)   return;
    reorderMechanics();

    if(scores2Rows)
    {
        emit pDebug("Scores - 2 rows");
        maxSynergyHeight = maxSynergyHeight2Row;
        resizeSynergyList();

        for(int i=0; i<3; i++)
        {
            Utility::clearLayout(horLayoutScores[i], false, false);
            Utility::clearLayout(horLayoutScores2[i], false, false);

            horLayoutScores[i]->addWidget(scoresPushButton[i]);
            horLayoutScores[i]->addWidget(scoresPushButton3[i]);
            horLayoutScores[i]->addWidget(scoresPushButton2[i]);
            horLayoutScores2[i]->addWidget(twitchButton[i]);
            horLayoutScores2[i]->addLayout(gridLayoutMechanics[i]);
        }
    }
    else
    {
        emit pDebug("Scores - 1 row");
        maxSynergyHeight = maxSynergyHeight1Row;
        resizeSynergyList();

        for(int i=0; i<3; i++)
        {
            Utility::clearLayout(horLayoutScores[i], false, false);
            Utility::clearLayout(horLayoutScores2[i], false, false);

            horLayoutScores[i]->addWidget(scoresPushButton[i]);
            horLayoutScores[i]->addWidget(scoresPushButton3[i]);
            horLayoutScores[i]->addWidget(twitchButton[i]);
            horLayoutScores[i]->addWidget(scoresPushButton2[i]);
            horLayoutScores2[i]->addLayout(gridLayoutMechanics[i]);
        }
    }
}


void DraftScoreWindow::reorderMechanics()
{
    for(int index=0; index<3; index++)
    {
        QList<QWidget *> labels;
        while(QLayoutItem* item = gridLayoutMechanics[index]->takeAt(0))
        {
            if(QWidget* widget = item->widget())
            {
                labels.append(widget);
            }
        }
        int i=0;
        for(QWidget* label: labels)
        {
            if(scores2Rows) gridLayoutMechanics[index]->addWidget(label, i%2, i/2);
            else            gridLayoutMechanics[index]->addWidget(label, 0, i);
            i++;
        }
    }
}


void DraftScoreWindow::showTwitchScores(bool show)
{
    showTwitch = show;
    checkScoresSpace();

    for(int i=0; i<3; i++)  twitchButton[i]->setVisible(showTwitch);
}


void DraftScoreWindow::setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR)
{
    showHA = draftMethodHA;
    showLF = draftMethodLF;
    showHSR = draftMethodHSR;
    checkScoresSpace();

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setVisible(showLF);
        scoresPushButton2[i]->setVisible(showHA);
        scoresPushButton3[i]->setVisible(showHSR);
    }
}


void DraftScoreWindow::setWantedMechanics(bool wantedMechanics[M_NUM_MECHANICS])
{
    for(int i=0; i<M_NUM_MECHANICS; i++)    this->wantedMechanics[i] = wantedMechanics[i];
}


void DraftScoreWindow::setWantedMechanic(uint mechanicIcon, bool value)
{
    wantedMechanics[mechanicIcon] = value;
}


bool DraftScoreWindow::isWantedMechanic(uint mechanicIcon)
{
    return wantedMechanics[mechanicIcon];
}


void DraftScoreWindow::setScores(float rating1, float rating2, float rating3,
                                 DraftMethod draftMethod,
                                 int includedDecks1, int includedDecks2, int includedDecks3)
{
    float bestRating = std::max(std::max(rating1, rating2), rating3);
    float ratings[3] = {rating1, rating2, rating3};
    int includedDecks[3] = {includedDecks1, includedDecks2, includedDecks3};

    for(int i=0; i<3; i++)
    {
        if(draftMethod == LightForge)
        {
            scoresPushButton[i]->setScore(ratings[i], bestRating);
            if(!onWarnMode[i])  Utility::fadeInWidget(scoresPushButton[i]);
        }
        else if(draftMethod == HSReplay)
        {
            scoresPushButton3[i]->setScore(ratings[i], bestRating, includedDecks[i]);
            if(!onWarnMode[i])  Utility::fadeInWidget(scoresPushButton3[i]);
        }
        else if(draftMethod == HearthArena)
        {
            scoresPushButton2[i]->setScore(ratings[i], bestRating);
            if(!onWarnMode[i])  Utility::fadeInWidget(scoresPushButton2[i]);
        }
    }

    if(draftMethod == HearthArena)
    {
        for(int i=0; i<3; i++)
        {
            //Fade-in warn mode
            if(onWarnMode[i])
            {
                Utility::fadeInWidget(warningCardLabel[i]);
                Utility::fadeInWidget(warningOkLabel[i]);
                warningCardLabel[i]->show();
                warningOkLabel[i]->show();
            }
            //Fade-in twitch scores
            else
            {
                showScores(i);
                Utility::fadeInWidget(twitchButton[i]);
            }
        }
    }
}


void DraftScoreWindow::setTwitchScores(int vote1, int vote2, int vote3, QString username)
{
    int votes[3] = {vote1, vote2, vote3};
    float totalVotes = votes[0] + votes[1] + votes[2];
    float topVotes = std::max(std::max(votes[0], votes[1]), votes[2]);

    if(totalVotes == 0)
    {
        for(int i=0; i<3; i++)  twitchButton[i]->reset();
        emit pDebug("Twitch scores reset.");
    }
    else
    {
        for(int i=0; i<3; i++)  twitchButton[i]->setValue(votes[i]/totalVotes, votes[i], FLOATEQ(votes[i], topVotes), username);
        emit pDebug(username + ": " + QString::number(votes[0]) + " - " +
                QString::number(votes[1]) + " - " + QString::number(votes[2]));
    }
}


void DraftScoreWindow::groupSynergyTags(QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    //Obtenemos codigos duplicados
    QStringList dupCodes;
    QList<QString> synergyTagMapKeys = synergyTagMap.keys();
    for(int i=0; i<synergyTagMapKeys.count(); i++)
    {
        const QList<QString> codeList = synergyTagMap[synergyTagMapKeys[i]].keys();
        for(const QString &code: codeList)
        {
            for(int j=i+1; j<synergyTagMapKeys.count() && !dupCodes.contains(code); j++)
            {
                if(synergyTagMap[synergyTagMapKeys[j]].contains(code))  dupCodes.append(code);
            }
        }
    }

    //Agrupamos tags de codigos duplicados
    for(const QString &code: dupCodes)
    {
        QString synergyTagJoin = "";
        int numCards = 0;
        for(int i=0; i<synergyTagMapKeys.count(); i++)
        {
            QString synergyTag = synergyTagMapKeys[i];
            if(synergyTagMap[synergyTag].contains(code))
            {
                if(synergyTagJoin.isEmpty())    synergyTagJoin.append(synergyTag);
                else                            synergyTagJoin.append(" - " + synergyTag);
                numCards = synergyTagMap[synergyTag][code];
                synergyTagMap[synergyTag].remove(code);
            }
        }
        synergyTagMap[synergyTagJoin][code] = numCards;
    }

    //Eliminamos synergyTag vacios
    for(int i=0; i<synergyTagMapKeys.count(); i++)
    {
        QString synergyTag = synergyTagMapKeys[i];
        if(synergyTagMap[synergyTag].isEmpty()) synergyTagMap.remove(synergyTag);
    }
}


void DraftScoreWindow::setSynergies(int posCard, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                    QMap<MechanicIcons, int> &mechanicIcons, const MechanicBorderColor dropBorderColor)
{
    if(posCard < 0 || posCard > 2)  return;

//    if(posCard==0){
//    QString codes[] = {"DMF_004", "DMF_247", "DMF_061", "DMF_730", "DMF_083"};
//    for(const QString &code: codes) synergyTagMap["Attack"][code]=1;
//    QString codes2[] = {"DMF_004", "DMF_247", "DMF_061", "DMF_730"};
//    for(const QString &code: codes2) synergyTagMap["Health"][code]=1;
//    QString codes3[] = {"DMF_061", "DMF_730"};
//    for(const QString &code: codes3) synergyTagMap["Cost"][code]=1;}
//    if(posCard==1){
//    QString codes[] = {"DMF_248", "DMF_247", "DMF_061", "DMF_730", "DMF_083", "DMF_090",
//        "DMF_105", "DMF_101", "DMF_244", "DMF_064"};
//    for(const QString &code: codes) synergyTagMap["Attack Buff"][code]=1;}
//    if(posCard==2){
//    QString codes[] = {"DMF_248", "DMF_247", "DMF_061", "DMF_730", "DMF_083", "DMF_090",
//        "DMF_105", "DMF_101", "DMF_244", "DMF_064", "DMF_054", "DMF_184", "DMF_186",
//        "DMF_517", "DMF_703", "DMF_701", "DMF_117", "DMF_118", "DMF_526", "DMF_124",
//        "DMF_073", "DMF_082", "DMF_174", "DMF_080", "DMF_078", "DMF_163"};
//    for(const QString &code: codes) synergyTagMap["Health"][code]=1;}

    synergiesListWidget[posCard]->clear();
    synergyCardLists[posCard].clear();
    groupSynergyTags(synergyTagMap);

    const QList<QString> codeList = synergyTagMap.keys();
    for(const QString &synergyTag: codeList)
    {
        QMap<int,SynergyCard> synergyCardMap;
        const QList<QString> codeList = synergyTagMap[synergyTag].keys();
        for(const QString &code: codeList)
        {
            int total = synergyTagMap[synergyTag][code];
            SynergyCard synergyCard(code);
            synergyCard.total = synergyCard.remaining = total;
            synergyCardMap.insertMulti(synergyCard.getCost(), synergyCard);
        }

        if(!synergyCardMap.isEmpty())   synergyCardMap.first().setSynergyTag(synergyTag);

        QList<SynergyCard> cardList = synergyCardMap.values();
        for(SynergyCard &synergyCard: cardList)
        {
            synergyCard.listItem = new QListWidgetItem(synergiesListWidget[posCard]);
            synergyCard.draw();
            synergyCardLists[posCard].append(synergyCard);
        }
    }


    //Add mechanic icons
    Utility::clearLayout(gridLayoutMechanics[posCard], true, false);

    //Drop mechanic first
    int posMech=0;
    const QList<MechanicIcons> iconList = mechanicIcons.keys();
    for(const MechanicIcons &mechanicIcon: iconList)
    {
        if(!isWantedMechanic(mechanicIcon))
        {
            mechanicIcons.remove(mechanicIcon);
        }
        else if(mechanicIcon == M_DROP2 || mechanicIcon == M_DROP3 || mechanicIcon == M_DROP4)
        {
            createMechanicIcon(posCard, posMech, mechanicIcon, mechanicIcons[mechanicIcon], dropBorderColor);
            posMech++;
            mechanicIcons.remove(mechanicIcon);
        }
    }
    //Other mechanics
    const QList<MechanicIcons> iconList2 = mechanicIcons.keys();
    for(const MechanicIcons &mechanicIcon: iconList2)
    {
        createMechanicIcon(posCard, posMech, mechanicIcon, mechanicIcons[mechanicIcon], dropBorderColor);
        posMech++;
    }
    if(posCard==2)  showSynergies();
}


void DraftScoreWindow::createMechanicIcon(int posCard, int posMech, MechanicIcons mechanicIcon, int count,
                                          const MechanicBorderColor dropBorderColor)
{
    QLabel *label = new QLabel();
    label->setPixmap(createMechanicIconPixmap(mechanicIcon, count, dropBorderColor));
    label->setToolTip(getMechanicTooltip(mechanicIcon));

    //Opacity effect
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
    effect->setOpacity(0);
    label->setGraphicsEffect(effect);

    if(!onWarnMode[posCard])    Utility::fadeInWidget(label);
    label->show();
    if(scores2Rows) gridLayoutMechanics[posCard]->addWidget(label, posMech%2, posMech/2);
    else            gridLayoutMechanics[posCard]->addWidget(label, 0, posMech);
}


void DraftScoreWindow::setWarningCard(const int posCard, const QString &code)
{
    warningCard[posCard]->setCode(code);
    warningCard[posCard]->draw(warningCardLabel[posCard]);
    onWarnMode[posCard] = true;
}


QString DraftScoreWindow::getMechanicFile(MechanicIcons mechanicIcon)
{
    if(mechanicIcon == M_DROP2)         return ThemeHandler::drop2CounterFile();
    else if(mechanicIcon == M_DROP3)    return ThemeHandler::drop3CounterFile();
    else if(mechanicIcon == M_DROP4)    return ThemeHandler::drop4CounterFile();
    else if(mechanicIcon == M_REACH)    return ThemeHandler::reachMechanicFile();
    else if(mechanicIcon == M_TAUNT_ALL)return ThemeHandler::tauntMechanicFile();
    else if(mechanicIcon == M_SURVIVABILITY)return ThemeHandler::survivalMechanicFile();
    else if(mechanicIcon == M_DISCOVER_DRAW)return ThemeHandler::drawMechanicFile();
    else if(mechanicIcon == M_PING)     return ThemeHandler::pingMechanicFile();
    else if(mechanicIcon == M_DAMAGE)   return ThemeHandler::damageMechanicFile();
    else if(mechanicIcon == M_DESTROY)  return ThemeHandler::destroyMechanicFile();
    else if(mechanicIcon == M_AOE)      return ThemeHandler::aoeMechanicFile();
    else                                return "";
}


bool DraftScoreWindow::paintDropBorder(QPainter &painter, MechanicIcons mechanicIcon,
                                       const MechanicBorderColor dropBorderColor)
{
    if(mechanicIcon == M_DROP2 || mechanicIcon == M_DROP3 || mechanicIcon == M_DROP4)
    {
        if(dropBorderColor == MechanicBorderRed)
        {
            painter.drawPixmap(0, 0, QPixmap(ThemeHandler::redMechanicFile()));
        }
        else if(dropBorderColor == MechanicBorderGreen)
        {
            painter.drawPixmap(0, 0, QPixmap(ThemeHandler::greenMechanicFile()));
        }
        return true;
    }
    else
    {
        return false;
    }
}


QPixmap DraftScoreWindow::createMechanicIconPixmap(MechanicIcons mechanicIcon, int count,
                                                   const MechanicBorderColor dropBorderColor)
{
    QString mechanicFile = getMechanicFile(mechanicIcon);
    QPixmap pixmap(mechanicFile);
    QString text = count<10?QString::number(count):"+";

    QPainter painter;
    painter.begin(&pixmap);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        bool drawNumber;
        if(paintDropBorder(painter, mechanicIcon, dropBorderColor))
        {
            drawNumber = false;
        }
        else if(count == 1)
        {
            painter.drawPixmap(0, 0, QPixmap(ThemeHandler::goldenMechanicFile()));
            drawNumber = false;
        }
        else
        {
            drawNumber = true;
        }

        if(drawNumber)
        {
            QFont font(ThemeHandler::bigFont());
            font.setPixelSize(30);
            QPen pen(BLACK);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.setBrush(WHITE);
            Utility::drawShadowText(painter, font, text, 50, 14, true, false);
        }
    painter.end();

    return pixmap.scaledToWidth(scoreWidth/2,Qt::SmoothTransformation);
}


QString DraftScoreWindow::getMechanicTooltip(MechanicIcons mechanicIcon)
{
    if(mechanicIcon == M_DROP2)         return "2 Drop";
    else if(mechanicIcon == M_DROP3)    return "3 Drop";
    else if(mechanicIcon == M_DROP4)    return "4 Drop";
    else if(mechanicIcon == M_REACH)    return "Reach";
    else if(mechanicIcon == M_TAUNT_ALL)return "Taunt";
    else if(mechanicIcon == M_SURVIVABILITY)return "Survival";
    else if(mechanicIcon == M_DISCOVER_DRAW)return "Draw";
    else if(mechanicIcon == M_PING)     return "Ping";
    else if(mechanicIcon == M_DAMAGE)   return "Removal";
    else if(mechanicIcon == M_DESTROY)  return "Hard\nRemoval";
    else if(mechanicIcon == M_AOE)      return "AOE";
    else                                return "";
}


void DraftScoreWindow::showScores(int i)
{
    scoresPushButton[i]->setVisible(showLF);
    scoresPushButton2[i]->setVisible(showHA);
    scoresPushButton3[i]->setVisible(showHSR);
    twitchButton[i]->setVisible(showTwitch);
}


void DraftScoreWindow::hideScores(bool quick)
{
    if(quick)
    {
        for(int i=0; i<3; i++)
        {
            QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton2[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton3[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(twitchButton[i]->graphicsEffect());
            eff->setOpacity(0);

            scoresPushButton[i]->hide();
            scoresPushButton2[i]->hide();
            scoresPushButton3[i]->hide();
            twitchButton[i]->hide();
        }
        clearMechanics();
    }
    else
    {
        for(int i=0; i<3; i++)
        {
            QPropertyAnimation *animation = Utility::fadeOutWidget(scoresPushButton[i]);
            Utility::fadeOutWidget(scoresPushButton2[i]);
            Utility::fadeOutWidget(scoresPushButton3[i]);
            Utility::fadeOutWidget(twitchButton[i]);
            Utility::fadeOutLayout(gridLayoutMechanics[i]);

            if(i==0)
            {
                if(animation != nullptr)    connect(animation, SIGNAL(finished()), this, SLOT(clearMechanics()));
                //Comentado pq en caso de no ocultar las scores prefiero dejar visibles las mechanics.
                //Se hara el clear al crear las mechanics del siguiente bucket.
                //else    clearMechanics();
            }
        }
    }

    hideWarnings();
    hideSynergies();
    this->update();
}


void DraftScoreWindow::hideWarnings()
{
    for(int i=0; i<3; i++)  hideWarning(i);
}


void DraftScoreWindow::hideWarning(int i)
{
    QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(warningCardLabel[i]->graphicsEffect());
    eff->setOpacity(0);
    eff = static_cast<QGraphicsOpacityEffect *>(warningOkLabel[i]->graphicsEffect());
    eff->setOpacity(0);
    warningCardLabel[i]->hide();
    warningOkLabel[i]->hide();
    onWarnMode[i]=false;
}


void DraftScoreWindow::clearMechanics()
{
    for(int index=0; index<3; index++)
    {
        Utility::clearLayout(gridLayoutMechanics[index], true, false);
    }
    update();
}


void DraftScoreWindow::showSynergies()
{
    resizeSynergyList();
    for(int i=0; i<3; i++)
    {
        if(!onWarnMode[i])
        {
            synergiesListWidget[i]->show();
            if(!synergyMotions[i].running)  stepScrollSynergies(i);
        }
    }
    this->update();
}


void DraftScoreWindow::hideSynergies()
{
    for(int index=0; index<3; index++)
    {
        synergiesListWidget[index]->hide();
        synergiesListWidget[index]->clear();
        synergyCardLists[index].clear();
    }
}


void DraftScoreWindow::redrawSynergyCards()
{
    for(int i=0; i<3; i++)
    {
        for(SynergyCard &synergyCard: synergyCardLists[i])
        {
            synergyCard.draw();
        }

        if(onWarnMode[i] && !warningCard[i]->getCode().isEmpty())
        {
            warningCard[i]->draw(warningCardLabel[i]);
        }
    }
}


void DraftScoreWindow::resizeSynergyList()
{
    int width = 0;
    for(int i=0; i<3; i++)
    {
        QListWidget *list = synergiesListWidget[i];
        int rowHeight = list->sizeHintForRow(0);
        int rows = list->count();
        int height = rows*rowHeight + 2*list->frameWidth();
        int maxSynergyHeightFit = maxSynergyHeight;
        if(rows>0)
        {
            width = list->sizeHintForColumn(0) + 2 * list->frameWidth();
            maxSynergyHeightFit-=maxSynergyHeightFit%list->sizeHintForRow(0);
        }
        if(height>maxSynergyHeightFit)  list->setFixedHeight(maxSynergyHeightFit);
        else                            list->setFixedHeight(height);

        //Moving scroll init
        synergyMotions[i].moveDown = true;
        synergyMotions[i].moving = true;
        synergyMotions[i].maximum = height - maxSynergyHeightFit;
        synergyMotions[i].stepValue = std::max(1,
                std::min(synergyMotions[i].maximum,maxSynergyHeightFit/2)
                /(2000/SYNERGY_MOTION_UPDATE_TIME)
                );
        synergyMotions[i].value = -(2000/SYNERGY_MOTION_UPDATE_TIME)*synergyMotions[i].stepValue;
    }

    if(width>0)
    {
        for(int i=0; i<3; i++)
        {
            synergiesListWidget[i]->setFixedWidth(width);
        }
    }
}


void DraftScoreWindow::stepScrollSynergies(int indexList)
{
    SynergyMotion &sm = synergyMotions[indexList];
    sm.running = true;
    MoveListWidget *lw = synergiesListWidget[indexList];
    if(sm.maximum <= 0 || lw->count()==0)
    {
        sm.running = false;
        return;
    }

    QTimer::singleShot(SYNERGY_MOTION_UPDATE_TIME, this, [=]() {stepScrollSynergies(indexList);});
    if(sm.moving)
    {
        sm.value = sm.value + (sm.moveDown?sm.stepValue:-sm.stepValue);
        lw->verticalScrollBar()->setValue(sm.value);

        if(sm.moveDown && sm.value >= sm.maximum)
        {
            sm.moveDown = !sm.moveDown;
            sm.value = sm.maximum + (2000/SYNERGY_MOTION_UPDATE_TIME)*sm.stepValue;
        }
        else if(!sm.moveDown && sm.value <= 0)
        {
            sm.moveDown = !sm.moveDown;
            sm.value = -(2000/SYNERGY_MOTION_UPDATE_TIME)*sm.stepValue;
        }
    }
}


void DraftScoreWindow::findSynergyCardEntered(QListWidgetItem * item)
{
    //Detect synergy list
    QListWidget * listWidget = item->listWidget();
    int indexList = -1;

    for(int i=0; i<3; i++)
    {
        if(synergiesListWidget[i] == listWidget)
        {
            indexList = i;
            break;
        }
    }
    if(indexList == -1) return;

    synergyMotions[indexList].moving = false;

    QString code = synergyCardLists[indexList][listWidget->row(item)].getCode();

    QRect rectCard = listWidget->visualItemRect(item);
    QPoint posCard = listWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int synergyListTop = listWidget->mapToGlobal(QPoint(0,0)).y();
    int synergyListBottom = listWidget->mapToGlobal(QPoint(0,listWidget->height())).y();
    emit cardEntered(code, globalRectCard, synergyListTop, synergyListBottom);
}


void DraftScoreWindow::resumeSynergyMotion()
{
    for(int i=0; i<3; i++)
    {
        synergyMotions[i].moving = true;
        synergyMotions[i].value = synergiesListWidget[i]->verticalScrollBar()->value();
    }
}


void DraftScoreWindow::findWarningCardLabelEntered(HoverLabel* hoverLabel)
{
    //Detect synergy list
    int index = -1;

    for(int i=0; i<3; i++)
    {
        if(warningCardLabel[i] == hoverLabel)
        {
            index = i;
            break;
        }
    }
    if(index == -1) return;

    QString code = warningCard[index]->getCode();

    QRect rectCard = hoverLabel->rect();
    QPoint posCard = hoverLabel->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int synergyListTop = hoverLabel->mapToGlobal(QPoint(0,0)).y();
    emit cardEntered(code, globalRectCard, synergyListTop, synergyListTop);
}


void DraftScoreWindow::setTheme()
{
    QPixmap pixmap(ThemeHandler::checkCardOkFile());
    for(int i=0; i<3; i++)
    {
        warningOkLabel[i]->setPixmap(pixmap.scaledToWidth(scoreWidth/2,Qt::SmoothTransformation));
    }
}


void DraftScoreWindow::warningOkClick(HoverLabel *hoverLabel)
{
    //Detect okLabel
    int index = -1;

    for(int i=0; i<3; i++)
    {
        if(warningOkLabel[i] == hoverLabel)
        {
            index = i;
            break;
        }
    }
    if(index == -1) return;

    hideWarning(index);
    Utility::fadeInWidget(scoresPushButton[index], true);
    Utility::fadeInWidget(scoresPushButton2[index], true);
    Utility::fadeInWidget(scoresPushButton3[index], true);
    Utility::fadeInWidget(twitchButton[index], true);
    showScores(index);
    synergiesListWidget[index]->show();
    Utility::fadeInLayout(gridLayoutMechanics[index]);
}
