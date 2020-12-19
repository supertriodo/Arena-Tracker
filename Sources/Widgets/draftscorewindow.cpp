#include "draftscorewindow.h"
#include "../themehandler.h"
#include <QtWidgets>


DraftScoreWindow::DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, bool normalizedLF) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = static_cast<int>(sizeCard.width()*0.7);

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    int midCards = (rect.width() - 3*sizeCard.width())/2;
    resize(rect.width() + 2*MARGIN + midCards,
           rect.height() + 2*MARGIN - (sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN - midCards/2,
         static_cast<int>(rectScreen.y() + rect.y() - MARGIN + 2.65*sizeCard.height()));
    int synergyWidth = this->width()/3-10;  //List Widget need 10 px (maybe 11px) extra space more than the sizeCard.


    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *horLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *verLayout[3];

    for(int i=0; i<3; i++)
    {
        //Scores
        scoresPushButton[i] = new ScoreButton(centralWidget, Score_LightForge, normalizedLF);
        scoresPushButton[i]->setFixedHeight(scoreWidth);
        scoresPushButton[i]->setFixedWidth(scoreWidth);
        scoresPushButton[i]->hide();
        connect(scoresPushButton[i], SIGNAL(spreadLearningShow(bool)),
                this, SLOT(spreadLearningShow(bool)));

        scoresPushButton2[i] = new ScoreButton(centralWidget, Score_HearthArena, false);
        scoresPushButton2[i]->setFixedHeight(scoreWidth);
        scoresPushButton2[i]->setFixedWidth(scoreWidth);
        scoresPushButton2[i]->hide();
        connect(scoresPushButton2[i], SIGNAL(spreadLearningShow(bool)),
                this, SLOT(spreadLearningShow(bool)));

        scoresPushButton3[i] = new ScoreButton(centralWidget, Score_HSReplay, false);
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
        QGraphicsOpacityEffect *effect;
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
        horLayoutScores[i]->addWidget(twitchButton[i]);
        horLayoutScores[i]->addWidget(scoresPushButton2[i]);

        QHBoxLayout *horLayoutScoresG = new QHBoxLayout();
        horLayoutScoresG->addStretch();
        horLayoutScoresG->addLayout(horLayoutScores[i]);
        horLayoutScoresG->addStretch();

        verLayout[i] = new QVBoxLayout();
        verLayout[i]->addLayout(horLayoutScoresG);

        gridLayoutMechanics[i] = new QGridLayout();
        horLayoutScores2[i] = new QHBoxLayout();
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
        hideSynergies(i);

        connect(synergiesListWidget[i], SIGNAL(itemEntered(QListWidgetItem*)),
                this, SLOT(findSynergyCardEntered(QListWidgetItem*)));
        connect(synergiesListWidget[i], SIGNAL(leave()),
                this, SIGNAL(cardLeave()));
        connect(synergiesListWidget[i], SIGNAL(leave()),
                this, SLOT(resumeSynergyMotion()));

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

    maxSynergyHeight = rectScreen.y() + rectScreen.height() - this->y() - 2*MARGIN - 2.5*scoreWidth;
    scores2Rows = false;
    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Scores");
}


DraftScoreWindow::~DraftScoreWindow()
{
    for(int i=0; i<3; i++)
    {
        synergiesListWidget[i]->clear();
        synergiesDeckCardLists[i].clear();
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


void DraftScoreWindow::checkScoresSpace(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR, bool showTwitch)
{
    if(draftMethodHA && draftMethodLF && draftMethodHSR && showTwitch)
    {
        for(int i=0; i<3; i++)
        {
            horLayoutScores[i]->removeWidget(twitchButton[i]);
            horLayoutScores2[i]->insertWidget(0, twitchButton[i]);
        }
    }
    else
    {
        for(int i=0; i<3; i++)
        {
            horLayoutScores2[i]->removeWidget(twitchButton[i]);
            horLayoutScores[i]->insertWidget(2, twitchButton[i]);
        }
    }
    bool oldScores2Rows = scores2Rows;
    scores2Rows = draftMethodHA && draftMethodLF && draftMethodHSR && showTwitch;
    if(oldScores2Rows != scores2Rows)   reorderMechanics();
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
    checkScoresSpace(scoresPushButton2[0]->isVisible(), scoresPushButton[0]->isVisible(),
            scoresPushButton3[0]->isVisible(), show);

    for(int i=0; i<3; i++)  twitchButton[i]->setVisible(show);
}


void DraftScoreWindow::setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR)
{
    checkScoresSpace(draftMethodHA, draftMethodLF, draftMethodHSR, twitchButton[0]->isVisible());

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setVisible(draftMethodLF);
        scoresPushButton2[i]->setVisible(draftMethodHA);
        scoresPushButton3[i]->setVisible(draftMethodHSR);
    }
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
            scoresPushButton[i]->setScore(ratings[i], FLOATEQ(ratings[i], bestRating));
            Utility::fadeInWidget(scoresPushButton[i]);
        }
        else if(draftMethod == HSReplay)
        {
            scoresPushButton3[i]->setScore(ratings[i], FLOATEQ(ratings[i], bestRating), includedDecks[i]);
            Utility::fadeInWidget(scoresPushButton3[i]);
        }
        else if(draftMethod == HearthArena)
        {
            scoresPushButton2[i]->setScore(ratings[i], FLOATEQ(ratings[i], bestRating));
            QPropertyAnimation *animation = Utility::fadeInWidget(scoresPushButton2[i]);

            if(i==0 && animation != nullptr)     connect(animation, SIGNAL(finished()), this, SLOT(showSynergies()));
        }
    }

    if(draftMethod == HearthArena)  resetTwitchScore();
}


void DraftScoreWindow::resetTwitchScore()
{
    for(int i=0; i<3; i++)
    {
        twitchButton[i]->reset();
        Utility::fadeInWidget(twitchButton[i]);
    }
}


void DraftScoreWindow::setTwitchScores(int vote1, int vote2, int vote3, QString username)
{
    int votes[3] = {vote1, vote2, vote3};
    float totalVotes = votes[0] + votes[1] + votes[2];
    float topVotes = std::max(std::max(votes[0], votes[1]), votes[2]);

    emit pDebug(username + ": " + QString::number(votes[0]) + " - " +
            QString::number(votes[1]) + " - " + QString::number(votes[2]));

    for(int i=0; i<3; i++)  twitchButton[i]->setValue(votes[i]/totalVotes, votes[i], FLOATEQ(votes[i], topVotes), username);
}


void DraftScoreWindow::setSynergies(int posCard, QMap<QString,int> &synergies, QMap<QString, int> &mechanicIcons,
                                    const MechanicBorderColor dropBorderColor)
{
    if(posCard < 0 || posCard > 2)  return;

//TODO remove
//    QString codes[] = {"DMF_248", "DMF_247", "DMF_061", "DMF_730", "DMF_083", "DMF_090",
//        "DMF_105", "DMF_101", "DMF_244", "DMF_064", "DMF_054", "DMF_184", "DMF_186",
//        "DMF_517", "DMF_703", "DMF_701", "DMF_117", "DMF_118", "DMF_526", "DMF_124",
//        "DMF_073", "DMF_082", "DMF_174", "DMF_080", "DMF_078", "DMF_163"};
//    for(const QString &code: codes) synergies[code]=1;

    synergiesListWidget[posCard]->clear();
    synergiesDeckCardLists[posCard].clear();

    QMap<int,DeckCard> deckCardMap;
    for(const QString &code: synergies.keys())
    {
        int total = synergies[code];
        DeckCard deckCard(code);
        deckCard.total = deckCard.remaining = total;
        deckCardMap.insertMulti(deckCard.getCost(), deckCard);
    }

    for(DeckCard &deckCard: deckCardMap.values())
    {
        deckCard.listItem = new QListWidgetItem(synergiesListWidget[posCard]);
        deckCard.draw();
        synergiesDeckCardLists[posCard].append(deckCard);
    }


    //Add mechanic icons
    Utility::clearLayout(gridLayoutMechanics[posCard], true, false);

    int i=0;
    for(const QString &mechanicIcon: mechanicIcons.keys())
    {
        QLabel *label = new QLabel();
        label->setPixmap(createMechanicIconPixmap(mechanicIcon, mechanicIcons[mechanicIcon], dropBorderColor));
        label->setToolTip(getMechanicTooltip(mechanicIcon));
        label->hide();
        if(scores2Rows) gridLayoutMechanics[posCard]->addWidget(label, i%2, i/2);
        else            gridLayoutMechanics[posCard]->addWidget(label, 0, i);
        i++;
    }
}


bool DraftScoreWindow::paintDropBorder(QPainter &painter, const QString &mechanicIcon,
                                       const MechanicBorderColor dropBorderColor)
{
    if(     mechanicIcon == ThemeHandler::drop2CounterFile() ||
            mechanicIcon == ThemeHandler::drop3CounterFile() ||
            mechanicIcon == ThemeHandler::drop4CounterFile())
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


QPixmap DraftScoreWindow::createMechanicIconPixmap(const QString &mechanicIcon, int count,
                                                   const MechanicBorderColor dropBorderColor)
{
    QPixmap pixmap(mechanicIcon);
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


QString DraftScoreWindow::getMechanicTooltip(QString iconName)
{
    if(iconName == ThemeHandler::aoeMechanicFile())             return "AOE";
    else if(iconName == ThemeHandler::tauntMechanicFile())      return "Taunt";
    else if(iconName == ThemeHandler::survivalMechanicFile())   return "Survival";
    else if(iconName == ThemeHandler::drawMechanicFile())       return "Draw";
    else if(iconName == ThemeHandler::pingMechanicFile())       return "Ping";
    else if(iconName == ThemeHandler::damageMechanicFile())     return "Removal";
    else if(iconName == ThemeHandler::destroyMechanicFile())    return "Hard\nRemoval";
    else if(iconName == ThemeHandler::reachMechanicFile())      return "Reach";
    else if(iconName == ThemeHandler::drop2CounterFile())       return "2 Drop";
    else if(iconName == ThemeHandler::drop3CounterFile())       return "3 Drop";
    else if(iconName == ThemeHandler::drop4CounterFile())       return "4 Drop";
    else    return "";
}


void DraftScoreWindow::hideScores(bool quick)
{
    for(int i=0; i<3; i++)
    {
        if(quick)
        {
            QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton2[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton3[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(twitchButton[i]->graphicsEffect());
            eff->setOpacity(0);
        }
        else
        {
            QPropertyAnimation *animation = Utility::fadeOutWidget(scoresPushButton[i]);
            Utility::fadeOutWidget(scoresPushButton2[i]);
            Utility::fadeOutWidget(scoresPushButton3[i]);
            Utility::fadeOutWidget(twitchButton[i]);

            if(i==0 && animation != nullptr)     connect(animation, SIGNAL(finished()), this, SLOT(update()));
        }

        hideSynergies(i);
    }
    this->update();
}


void DraftScoreWindow::showSynergies()
{
    resizeSynergyList();

    for(int i=0; i<3; i++)
    {
        synergiesListWidget[i]->show();
        Utility::showItemsLayout(gridLayoutMechanics[i]);
        stepScrollSynergies(i);
    }
    this->update();
}


void DraftScoreWindow::hideSynergies(int index)
{
    synergiesListWidget[index]->hide();
    synergiesListWidget[index]->clear();
    synergiesDeckCardLists[index].clear();
    Utility::clearLayout(gridLayoutMechanics[index], true, false);
}


void DraftScoreWindow::redrawSynergyCards()
{
    for(int i=0; i<3; i++)
    {
        for(DeckCard &deckCard: synergiesDeckCardLists[i])
        {
            deckCard.draw();
        }
    }
    resizeSynergyList();
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
        if(height>maxSynergyHeight) list->setFixedHeight(maxSynergyHeight);
        else                        list->setFixedHeight(height);
        if(rows>0)  width = list->sizeHintForColumn(0) + 2 * list->frameWidth();

        //Moving scroll init
        synergyMotions[i].moveDown = true;
        synergyMotions[i].moving = true;
        synergyMotions[i].maximum = height - maxSynergyHeight;
        synergyMotions[i].stepValue = synergyMotions[i].maximum/(2000/SYNERGY_MOTION_UPDATE_TIME);
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
    MoveListWidget *lw = synergiesListWidget[indexList];
    if(sm.maximum <= 0 || lw->count()==0)
        return;

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

    QString code = synergiesDeckCardLists[indexList][listWidget->row(item)].getCode();

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


void DraftScoreWindow::setNormalizedLF(bool value)
{
    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setNormalizedLF(value);
    }
}

