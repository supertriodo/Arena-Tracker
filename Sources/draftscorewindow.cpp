#include "draftscorewindow.h"
#include <QtWidgets>

DraftScoreWindow::DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = sizeCard.width()*0.8;

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    resize(rect.width()+2*MARGIN-(sizeCard.width()-scoreWidth),
           rect.height()+2*MARGIN-(sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN + (sizeCard.width()-scoreWidth)/2,
         rectScreen.y() + rect.y() - MARGIN + 2.6*sizeCard.height());


    QWidget *centralWidget = new QWidget(this);
    QFont font("Belwe Bd BT");
    font.setPointSize(scoreWidth/3);

    QVBoxLayout *verLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *horLayoutScores = new QHBoxLayout();
    QHBoxLayout *horLayoutSynergies = new QHBoxLayout();
    verLayout->addLayout(horLayoutScores);
    verLayout->addLayout(horLayoutSynergies);

    for(int i=0; i<3; i++)
    {
        scoresPushButton[i] = new ScoreButton(centralWidget, i);
        scoresPushButton[i]->setMinimumHeight(0);
        scoresPushButton[i]->setMaximumHeight(0);
        scoresPushButton[i]->setMinimumWidth(scoreWidth);
        scoresPushButton[i]->setFont(font);

        horLayoutScores->addWidget(scoresPushButton[i]);
        if(i<2)    horLayoutScores->addStretch();

        connect(scoresPushButton[i], SIGNAL(enter(int)),
                this, SLOT(showSynergies(int)));
        connect(scoresPushButton[i], SIGNAL(leave(int)),
                this, SLOT(hideSynergies(int)));


        QVBoxLayout *verLayoutSynergy = new QVBoxLayout();
        synergiesListWidget[i] = new QListWidget(centralWidget);
        synergiesListWidget[i]->setMinimumHeight(0);
        synergiesListWidget[i]->setMaximumHeight(0);
        synergiesListWidget[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setIconSize(CARD_SIZE);
        synergiesListWidget[i]->setFrameShape(QFrame::NoFrame);
        synergiesListWidget[i]->setStyleSheet("background-color: transparent;");
        verLayoutSynergy->addWidget(synergiesListWidget[i]);
        verLayoutSynergy->addStretch();
        horLayoutSynergies->addLayout(verLayoutSynergy);
        if(i<2)    horLayoutSynergies->addStretch();
    }

    maxSynergyHeight = rectScreen.y() + rectScreen.height() - this->y() - 2*MARGIN - 2*scoreWidth; //Extra scoreWidth
    setCentralWidget(centralWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


DraftScoreWindow::~DraftScoreWindow()
{
}


void DraftScoreWindow::setScores(double rating1, double rating2, double rating3,
                                 QString synergy1, QString synergy2, QString synergy3)
{
    double ratings[3] = {rating1, rating2, rating3};
    QString synergies[3] = {synergy1, synergy2, synergy3};
    double maxRating = std::max(std::max(rating1,rating2),rating3);

    for(int i=0; i<3; i++)
    {
        int rating255 = std::max(std::min((int)(ratings[i]*2.55), 255), 0);
        int r = std::min(255, (255 - rating255)*2);
        int g = std::min(255,rating255*2);
        int b = 0;

        scoresPushButton[i]->setText(QString::number((int)ratings[i]));
        scoresPushButton[i]->setStyleSheet(
                    "QPushButton{background-color: "

                    "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                    "stop: 0 black, "
                    "stop: 0.5 rgb("+ QString::number(r) +","+ QString::number(g) +","+ QString::number(b) +"), "
                    "stop: 1 black);"

                    "border-style: solid;border-color: black;" +
                    ((!synergies[i].isEmpty())?"border-bottom-style: dotted;":"") +

                    "border-width: " + QString::number(scoreWidth/20) + "px;border-radius: "
                    + QString::number(scoreWidth/3) + "px;}");

        QPropertyAnimation *animation = new QPropertyAnimation(scoresPushButton[i], "maximumHeight");
        animation->setDuration(ANIMATION_TIME);
        animation->setStartValue(0);
        animation->setEndValue(scoreWidth);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start();


        animation = new QPropertyAnimation(scoresPushButton[i], "minimumHeight");
        animation->setDuration(ANIMATION_TIME);
        animation->setStartValue(0);
        animation->setEndValue(scoreWidth);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start();

        scoresPushButton[i]->setDrawArrow(!synergies[i].isEmpty());
        scoresPushButton[i]->setDrawHLines(ratings[i]==maxRating);


        //Insert synergies
        synergiesListWidget[i]->clear();

        QStringList synergiesList = synergies[i].split(". ", QString::SkipEmptyParts);
        foreach(QString name, synergiesList)
        {
            QString code = Utility::cardEnCodeFromName(name);
            DeckCard deckCard(code);
            deckCard.listItem = new QListWidgetItem(synergiesListWidget[i]);
            deckCard.draw();
        }

        synergiesListWidget[i]->setMaximumWidth(0);
    }
}


void DraftScoreWindow::hideScores()
{
    for(int i=0; i<3; i++)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(scoresPushButton[i], "maximumHeight");
        animation->setDuration(ANIMATION_TIME/2);
        animation->setStartValue(scoreWidth);
        animation->setEndValue(0);
        animation->setEasingCurve(QEasingCurve::InQuad);
        animation->start();

        animation = new QPropertyAnimation(scoresPushButton[i], "minimumHeight");
        animation->setDuration(ANIMATION_TIME/2);
        animation->setStartValue(scoreWidth);
        animation->setEndValue(0);
        animation->setEasingCurve(QEasingCurve::InQuad);
        animation->start();
    }
}


void DraftScoreWindow::showSynergies(int index)
{
    QPropertyAnimation *animation = new QPropertyAnimation(synergiesListWidget[index], "maximumWidth");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(0);
    animation->setEndValue(225);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();

    //Calcula height
    int rowHeight = synergiesListWidget[index]->sizeHintForRow(0);
    int rows = synergiesListWidget[index]->count();
    int height = rows*rowHeight + 2*synergiesListWidget[index]->frameWidth();
    if(height>maxSynergyHeight)    height = maxSynergyHeight;

    synergiesListWidget[index]->setMinimumHeight(height);
    synergiesListWidget[index]->setMaximumHeight(height);
}


void DraftScoreWindow::hideSynergies(int index)
{
    QPropertyAnimation *animation = new QPropertyAnimation(synergiesListWidget[index], "maximumWidth");
    animation->setDuration(ANIMATION_TIME/2);
    animation->setStartValue(synergiesListWidget[index]->maximumWidth());
    animation->setEndValue(0);
    animation->setEasingCurve(QEasingCurve::InQuad);
    animation->start();
}


