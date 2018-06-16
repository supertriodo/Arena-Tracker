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

        //Opacity effects
        QGraphicsOpacityEffect *effect;
        effect = new QGraphicsOpacityEffect(scoresPushButton[i]);
        effect->setOpacity(0);
        scoresPushButton[i]->setGraphicsEffect(effect);

        horLayoutScores->addStretch();
        horLayoutScores->addWidget(scoresPushButton[i]);
        horLayoutScores->addStretch();
    }

    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle("AT Heroes");
}


DraftHeroWindow::~DraftHeroWindow()
{
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
}


void DraftHeroWindow::hideScores(bool quick)
{
    for(int i=0; i<3; i++)
    {
        if(quick)
        {
            QGraphicsOpacityEffect *eff = (QGraphicsOpacityEffect *)scoresPushButton[i]->graphicsEffect();
            eff->setOpacity(0);
        }
        else
        {
            QPropertyAnimation *animation = Utility::fadeOutWidget(scoresPushButton[i]);

            if(i==0 && animation!=NULL)     connect(animation, SIGNAL(finished()), this, SLOT(update()));
        }
    }
    this->update();
}


