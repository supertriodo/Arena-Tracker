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
    QVBoxLayout *verLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *horLayoutScores = new QHBoxLayout();
    QHBoxLayout *horLayoutSynergies = new QHBoxLayout();
    verLayout->addLayout(horLayoutScores);
    verLayout->addLayout(horLayoutSynergies);

    for(int i=0; i<3; i++)
    {
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
        twitchButton[i]->setFixedHeight(static_cast<int>(scoreWidth*0.75));
        twitchButton[i]->setFixedWidth(static_cast<int>(scoreWidth*0.75));
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

        horLayoutScores->addStretch();
        horLayoutScores->addWidget(scoresPushButton[i]);
        horLayoutScores->addWidget(scoresPushButton3[i]);
        horLayoutScores->addWidget(twitchButton[i]);
        horLayoutScores->addWidget(scoresPushButton2[i]);
        horLayoutScores->addStretch();


        QVBoxLayout *verLayoutSynergy = new QVBoxLayout();

        horLayoutMechanics[i] = new QHBoxLayout();

        synergiesListWidget[i] = new MoveListWidget(centralWidget);
        synergiesListWidget[i]->setFixedHeight(0);
        synergiesListWidget[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setIconSize(QSize(synergyWidth, static_cast<int>(synergyWidth/218.0*35)));
        synergiesListWidget[i]->setMouseTracking(true);
        hideSynergies(i);

        connect(synergiesListWidget[i], SIGNAL(itemEntered(QListWidgetItem*)),
                this, SLOT(findSynergyCardEntered(QListWidgetItem*)));
        connect(synergiesListWidget[i], SIGNAL(leave()),
                this, SIGNAL(cardLeave()));

        verLayoutSynergy->addLayout(horLayoutMechanics[i]);
        verLayoutSynergy->addWidget(synergiesListWidget[i]);
        verLayoutSynergy->addStretch();

        horLayoutSynergies->addStretch();
        horLayoutSynergies->addLayout(verLayoutSynergy);
        horLayoutSynergies->addStretch();
    }

    maxSynergyHeight = rectScreen.y() + rectScreen.height() - this->y() - 2*MARGIN - 2*scoreWidth; //Extra scoreWidth
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
            int smallWidth = static_cast<int>(scoreWidth*0.75);
            scoresPushButton[i]->setFixedHeight(smallWidth);
            scoresPushButton[i]->setFixedWidth(smallWidth);
            scoresPushButton[i]->draw();

            scoresPushButton2[i]->setFixedHeight(smallWidth);
            scoresPushButton2[i]->setFixedWidth(smallWidth);
            scoresPushButton2[i]->draw();

            scoresPushButton3[i]->setFixedHeight(smallWidth);
            scoresPushButton3[i]->setFixedWidth(smallWidth);
            scoresPushButton3[i]->draw();
        }
    }
    else
    {
        for(int i=0; i<3; i++)
        {
            scoresPushButton[i]->setFixedHeight(scoreWidth);
            scoresPushButton[i]->setFixedWidth(scoreWidth);
            scoresPushButton[i]->draw();

            scoresPushButton2[i]->setFixedHeight(scoreWidth);
            scoresPushButton2[i]->setFixedWidth(scoreWidth);
            scoresPushButton2[i]->draw();

            scoresPushButton3[i]->setFixedHeight(scoreWidth);
            scoresPushButton3[i]->setFixedWidth(scoreWidth);
            scoresPushButton3[i]->draw();
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
        twitchButton[i]->setValue(0, 0, false);
        Utility::fadeInWidget(twitchButton[i]);
    }
}


void DraftScoreWindow::setTwitchScores(int vote1, int vote2, int vote3)
{
    int votes[3] = {vote1, vote2, vote3};
    float totalVotes = votes[0] + votes[1] + votes[2];
    float topVotes = std::max(std::max(votes[0], votes[1]), votes[2]);

    emit pDebug(QString::number(votes[0]) + " - " + QString::number(votes[1]) + " - " + QString::number(votes[2]));

    for(int i=0; i<3; i++)  twitchButton[i]->setValue(votes[i]/totalVotes, votes[i], FLOATEQ(votes[i], topVotes));
}


void DraftScoreWindow::setSynergies(int posCard, QMap<QString,int> &synergies, QMap<QString, int> &mechanicIcons,
                                    const MechanicBorderColor dropBorderColor)
{
    if(posCard < 0 || posCard > 2)  return;

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
    Utility::clearLayout(horLayoutMechanics[posCard], true);
    horLayoutMechanics[posCard]->addStretch();

    for(const QString &mechanicIcon: mechanicIcons.keys())
    {
        QLabel *label = new QLabel();
        label->setPixmap(createMechanicIconPixmap(mechanicIcon, mechanicIcons[mechanicIcon], dropBorderColor));
        label->setToolTip(getMechanicTooltip(mechanicIcon));
        label->hide();
        horLayoutMechanics[posCard]->addWidget(label);
    }

    horLayoutMechanics[posCard]->addStretch();
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
        Utility::showItemsLayout(horLayoutMechanics[i]);
    }
    this->update();
}


void DraftScoreWindow::hideSynergies(int index)
{
    synergiesListWidget[index]->hide();
    synergiesListWidget[index]->clear();
    synergiesDeckCardLists[index].clear();
    Utility::clearLayout(horLayoutMechanics[index], true);
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
        if(height>maxSynergyHeight)    height = maxSynergyHeight;

        list->setFixedHeight(height);
        if(rows>0)  width = list->sizeHintForColumn(0) + 2 * list->frameWidth();
    }

    if(width>0)
    {
        for(int i=0; i<3; i++)
        {
            synergiesListWidget[i]->setFixedWidth(width);
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

    QString code = synergiesDeckCardLists[indexList][listWidget->row(item)].getCode();

    QRect rectCard = listWidget->visualItemRect(item);
    QPoint posCard = listWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int synergyListTop = listWidget->mapToGlobal(QPoint(0,0)).y();
    int synergyListBottom = listWidget->mapToGlobal(QPoint(0,listWidget->height())).y();
    emit cardEntered(code, globalRectCard, synergyListTop, synergyListBottom);
}


void DraftScoreWindow::setNormalizedLF(bool value)
{
    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setNormalizedLF(value);
    }
}

