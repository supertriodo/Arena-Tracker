#include "draftherowindow.h"
#include <QtWidgets>


DraftHeroWindow::DraftHeroWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = sizeCard.width()*0.65;

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    int midCards = (rect.width() - 3*sizeCard.width())/2;
    resize(rect.width() + 2*MARGIN + midCards,
           rect.height() + 2*MARGIN - (sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN - midCards/2,
         rectScreen.y() + rect.y() - MARGIN + 1.5*sizeCard.height());

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *horLayoutScores = new QHBoxLayout(centralWidget);

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i] = new ScoreButton(centralWidget, Score_Heroes, false);
        scoresPushButton[i]->setFixedHeight(scoreWidth);
        scoresPushButton[i]->setFixedWidth(scoreWidth);

        twitchButton[i] = new TwitchButton(centralWidget, 0, 1);
        twitchButton[i]->setFixedHeight(scoreWidth*0.75);
        twitchButton[i]->setFixedWidth(scoreWidth*0.75);
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


void DraftHeroWindow::setScores(double rating1, double rating2, double rating3)
{
    double bestRating = std::max(std::max(rating1, rating2), rating3);
    double ratings[3] = {rating1, rating2, rating3};

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i]->setScore(ratings[i], ratings[i]==bestRating);
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


void DraftHeroWindow::setTwitchScores(int vote1, int vote2, int vote3)
{
    int votes[3] = {vote1, vote2, vote3};
    float totalVotes = votes[0] + votes[1] + votes[2];
    double topVotes = std::max(std::max(votes[0], votes[1]), votes[2]);

    for(int i=0; i<3; i++)  twitchButton[i]->setValue(votes[i]/totalVotes, votes[i], votes[i]==topVotes);
}


void DraftHeroWindow::hideScores(bool quick)
{
    for(int i=0; i<3; i++)
    {
        if(quick)
        {
            QGraphicsOpacityEffect *eff = (QGraphicsOpacityEffect *)scoresPushButton[i]->graphicsEffect();
            eff->setOpacity(0);
            eff = (QGraphicsOpacityEffect *)twitchButton[i]->graphicsEffect();
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


