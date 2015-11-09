#include "draftscorewindow.h"
#include <QtWidgets>

DraftScoreWindow::DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    scoreWidth = sizeCard.width()*0.7;

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rectScreen = screen->geometry();
    int midCards = (rect.width() - 3*sizeCard.width())/2;
    resize(rect.width() + 2*MARGIN + midCards,
           rect.height() + 2*MARGIN - (sizeCard.height()-scoreWidth));
    move(rectScreen.x() + rect.x() - MARGIN - midCards/2,
         rectScreen.y() + rect.y() - MARGIN + 2.65*sizeCard.height());
    int synergyWidth = this->width()/3-10;  //List Widget need 10 px (maybe 11px) extra space more than the sizeCard.


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

        horLayoutScores->addStretch();
        horLayoutScores->addWidget(scoresPushButton[i]);
        horLayoutScores->addStretch();


        QVBoxLayout *verLayoutSynergy = new QVBoxLayout();
        synergiesListWidget[i] = new QListWidget(centralWidget);
        synergiesListWidget[i]->setMinimumHeight(0);
        synergiesListWidget[i]->setMaximumHeight(0);
        synergiesListWidget[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        synergiesListWidget[i]->setIconSize(QSize(std::min(218, synergyWidth),35));
        synergiesListWidget[i]->setFrameShape(QFrame::NoFrame);
        synergiesListWidget[i]->setStyleSheet("background-color: transparent;");
        verLayoutSynergy->addWidget(synergiesListWidget[i]);
        verLayoutSynergy->addStretch();

        horLayoutSynergies->addStretch();
        if(synergyWidth > 218)  horLayoutSynergies->addSpacing(std::min(30, synergyWidth-218));
        horLayoutSynergies->addLayout(verLayoutSynergy);
        horLayoutSynergies->addStretch();
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

        if(i == 0)  connect(animation, SIGNAL(finished()), this, SLOT(showSynergies()));


        //Insert synergies
        synergiesListWidget[i]->clear();

        QStringList synergiesList = synergies[i].split(" / ", QString::SkipEmptyParts);
        foreach(QString name, synergiesList)
        {
            QString code;
            int total = getCard(name, code);
            DeckCard deckCard(code);
            deckCard.total = deckCard.remaining = total;
            deckCard.listItem = new QListWidgetItem(synergiesListWidget[i]);
            deckCard.draw();
        }
    }
}


int DraftScoreWindow::getCard(QString &name, QString &code)
{
    //Mechwarper (2x)
    QRegularExpressionMatch match;
    if(name.contains(QRegularExpression("([\\w ]*\\w) *\\((\\d+)x\\)"), &match))
    {
        code = Utility::cardEnCodeFromName(match.captured(1));
        return match.captured(2).toInt();
    }
    else
    {
        code = Utility::cardEnCodeFromName(name);
        return 1;
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
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->start();

        animation = new QPropertyAnimation(scoresPushButton[i], "minimumHeight");
        animation->setDuration(ANIMATION_TIME/2);
        animation->setStartValue(scoreWidth);
        animation->setEndValue(0);
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->start();

        connect(animation, SIGNAL(finished()),
                this, SLOT(update()));

        hideSynergies(i);
    }
    this->update();
}


void DraftScoreWindow::showSynergies()
{
    for(int i=0; i<3; i++)
    {
        showSynergies(i);
    }
    this->update();
}


void DraftScoreWindow::showSynergies(int index)
{
    //Calcula height
    int rowHeight = synergiesListWidget[index]->sizeHintForRow(0);
    int rows = synergiesListWidget[index]->count();
    int height = rows*rowHeight + 2*synergiesListWidget[index]->frameWidth();
    if(height>maxSynergyHeight)    height = maxSynergyHeight;

    synergiesListWidget[index]->setMinimumHeight(height);
    synergiesListWidget[index]->setMaximumHeight(height);
    synergiesListWidget[index]->show();
}


void DraftScoreWindow::hideSynergies(int index)
{
    synergiesListWidget[index]->hide();
}


