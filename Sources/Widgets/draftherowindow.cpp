#include "draftherowindow.h"
#include <QtWidgets>


DraftHeroWindow::DraftHeroWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = static_cast<int>(sizeCard.width()*0.65);

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    int midCards = (rect.width() - 3*sizeCard.width())/2;
    resize(rect.width() + 2*MARGIN + midCards,
           rect.height() + 2*MARGIN - (sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN - midCards/2,
         static_cast<int>(rectScreen.y() + rect.y() - MARGIN + 1.5*sizeCard.height()));

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *horLayoutScores = new QHBoxLayout(centralWidget);

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i] = new ScoreButton(centralWidget, Score_Heroes, false);
        scoresPushButton[i]->setFixedHeight(scoreWidth);
        scoresPushButton[i]->setFixedWidth(scoreWidth);

        twitchButton[i] = new TwitchButton(centralWidget, 0, 1);
        twitchButton[i]->setFixedHeight(scoreWidth);
        twitchButton[i]->setFixedWidth(scoreWidth);
        twitchButton[i]->hide();

        //Opacity effects
        QGraphicsOpacityEffect *effect;
        effect = new QGraphicsOpacityEffect(scoresPushButton[i]);
        effect->setOpacity(0);
        scoresPushButton[i]->setGraphicsEffect(effect);
        effect = new QGraphicsOpacityEffect(twitchButton[i]);
        effect->setOpacity(0);
        twitchButton[i]->setGraphicsEffect(effect);

        horLayoutScores->addStretch();
        horLayoutScores->addWidget(scoresPushButton[i]);
        horLayoutScores->addWidget(twitchButton[i]);
        horLayoutScores->addStretch();
    }

    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Heroes");
}


DraftHeroWindow::~DraftHeroWindow()
{
}


void DraftHeroWindow::showTwitchScores(bool show)
{
    for(int i=0; i<3; i++)
    {
        if(show)    twitchButton[i]->show();
        else        twitchButton[i]->hide();
    }
}


void DraftHeroWindow::setScores(float rating1, float rating2, float rating3)
{
    float bestRating = std::max(std::max(rating1, rating2), rating3);
    float ratings[3] = {rating1, rating2, rating3};

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setScore(ratings[i], FLOATEQ(ratings[i], bestRating));
        Utility::fadeInWidget(scoresPushButton[i]);
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
            eff = static_cast<QGraphicsOpacityEffect *>(twitchButton[i]->graphicsEffect());
            eff->setOpacity(0);
        }
        else
        {
            QPropertyAnimation *animation = Utility::fadeOutWidget(scoresPushButton[i]);
            Utility::fadeOutWidget(twitchButton[i]);

            if(i==0 && animation != nullptr)     connect(animation, SIGNAL(finished()), this, SLOT(update()));
        }
    }
    this->update();
}


