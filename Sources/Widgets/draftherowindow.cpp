#include "draftherowindow.h"
#include <QtWidgets>


DraftHeroWindow::DraftHeroWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = static_cast<int>(sizeCard.width()*0.65);

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
         static_cast<int>(rectScreen.y() + rect.y() - MARGIN + 1.5*sizeCard.height()));

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *horLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *verLayout[3];

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i] = new ScoreButton(centralWidget, Score_Heroes, -1);
        scoresPushButton[i]->setFixedHeight(scoreWidth);
        scoresPushButton[i]->setFixedWidth(scoreWidth);

        scoresPlayerPushButton[i] = new ScoreButton(centralWidget, Score_Heroes_Player, -1);
        scoresPlayerPushButton[i]->setFixedHeight(scoreWidth);
        scoresPlayerPushButton[i]->setFixedWidth(scoreWidth);

        twitchButton[i] = new TwitchButton(centralWidget, 0, 1);
        twitchButton[i]->setFixedHeight(scoreWidth);
        twitchButton[i]->setFixedWidth(scoreWidth);
        twitchButton[i]->hide();

        //Opacity effects
        QGraphicsOpacityEffect *effect;
        effect = new QGraphicsOpacityEffect(scoresPushButton[i]);
        effect->setOpacity(0);
        scoresPushButton[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(scoresPlayerPushButton[i]);
        effect->setOpacity(0);
        scoresPlayerPushButton[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(twitchButton[i]);
        effect->setOpacity(0);
        twitchButton[i]->setGraphicsEffect(effect);

        //LAYOUTS scores
        horLayoutScores[i] = new QHBoxLayout();
        horLayoutScores[i]->addWidget(scoresPushButton[i]);
        horLayoutScores[i]->addWidget(scoresPlayerPushButton[i]);

        QHBoxLayout *horLayoutScoresG = new QHBoxLayout();
        horLayoutScoresG->addStretch();
        horLayoutScoresG->addLayout(horLayoutScores[i]);
        horLayoutScoresG->addStretch();

        verLayout[i] = new QVBoxLayout();
        verLayout[i]->addLayout(horLayoutScoresG);

        horLayoutScores2[i] = new QHBoxLayout();
        horLayoutScores2[i]->addWidget(twitchButton[i]);

        QHBoxLayout *horLayoutScores2G = new QHBoxLayout();
        horLayoutScores2G->addStretch();
        horLayoutScores2G->addLayout(horLayoutScores2[i]);
        horLayoutScores2G->addStretch();

        verLayout[i]->addLayout(horLayoutScores2G);
        verLayout[i]->addStretch();

        horLayout->addStretch();
        horLayout->addLayout(verLayout[i]);
        horLayout->addStretch();
    }

    scores2Rows = true;
    showTwitch = false;
    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Heroes");
}


DraftHeroWindow::~DraftHeroWindow()
{
}


void DraftHeroWindow::checkScoresSpace()
{
    bool oldScores2Rows = scores2Rows;
    scores2Rows = showTwitch;
    if(oldScores2Rows == scores2Rows)   return;

    if(scores2Rows)
    {
        emit pDebug("Scores Heroes - 2 rows");

        for(int i=0; i<3; i++)
        {
            Utility::clearLayout(horLayoutScores[i], false, false);
            Utility::clearLayout(horLayoutScores2[i], false, false);

            horLayoutScores[i]->addWidget(scoresPushButton[i]);
            horLayoutScores[i]->addWidget(scoresPlayerPushButton[i]);
            horLayoutScores2[i]->addWidget(twitchButton[i]);
        }
    }
    else
    {
        emit pDebug("Scores Heroes - 1 row");

        for(int i=0; i<3; i++)
        {
            Utility::clearLayout(horLayoutScores[i], false, false);
            Utility::clearLayout(horLayoutScores2[i], false, false);

            horLayoutScores[i]->addWidget(scoresPushButton[i]);
            horLayoutScores[i]->addWidget(twitchButton[i]);
            horLayoutScores[i]->addWidget(scoresPlayerPushButton[i]);
        }
    }
}


void DraftHeroWindow::showTwitchScores(bool show)
{
    showTwitch = show;
    checkScoresSpace();

    for(int i=0; i<3; i++)  twitchButton[i]->setVisible(showTwitch);
}


void DraftHeroWindow::setScores(int classOrder[3])
{
    float ratings[3] = {ScoreButton::getHeroScore(classOrder[0]), ScoreButton::getHeroScore(classOrder[1]),
                        ScoreButton::getHeroScore(classOrder[2])};
    float ratingsPlayer[3] = {ScoreButton::getPlayerWinrate(classOrder[0]), ScoreButton::getPlayerWinrate(classOrder[1]),
                              ScoreButton::getPlayerWinrate(classOrder[2])};
    float bestRating = std::max(std::max(ratings[0], ratings[1]), ratings[2]);
    float bestRatingPlayer = std::max(std::max(ratingsPlayer[0], ratingsPlayer[1]), ratingsPlayer[2]);

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setClassOrder(classOrder[i]);
        scoresPushButton[i]->setScore(ratings[i], bestRating);
        Utility::fadeInWidget(scoresPushButton[i]);

        scoresPlayerPushButton[i]->setClassOrder(classOrder[i]);
        scoresPlayerPushButton[i]->setScore(ratingsPlayer[i], bestRatingPlayer);
        Utility::fadeInWidget(scoresPlayerPushButton[i]);
    }

    resetTwitchScore();
}


void DraftHeroWindow::resetTwitchScore()
{
    for(int i=0; i<3; i++)
    {
        twitchButton[i]->setValue(0, 0, false);
        Utility::fadeInWidget(twitchButton[i]);
    }
}


void DraftHeroWindow::setTwitchScores(int vote1, int vote2, int vote3, QString username)
{
    int votes[3] = {vote1, vote2, vote3};
    float totalVotes = votes[0] + votes[1] + votes[2];
    float topVotes = std::max(std::max(votes[0], votes[1]), votes[2]);

    for(int i=0; i<3; i++)  twitchButton[i]->setValue(votes[i]/totalVotes, votes[i], FLOATEQ(votes[i], topVotes), username);
}


void DraftHeroWindow::hideScores(bool quick)
{
    for(int i=0; i<3; i++)
    {
        if(quick)
        {
            QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(scoresPushButton[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(scoresPlayerPushButton[i]->graphicsEffect());
            eff->setOpacity(0);
            eff = static_cast<QGraphicsOpacityEffect *>(twitchButton[i]->graphicsEffect());
            eff->setOpacity(0);
        }
        else
        {
            QPropertyAnimation *animation = Utility::fadeOutWidget(scoresPushButton[i]);
            Utility::fadeOutWidget(scoresPlayerPushButton[i]);
            Utility::fadeOutWidget(twitchButton[i]);

            if(i==0 && animation != nullptr)     connect(animation, SIGNAL(finished()), this, SLOT(update()));
        }
    }
    this->update();
}


