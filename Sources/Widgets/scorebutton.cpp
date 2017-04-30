#include "scorebutton.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent, DraftMethod draftMethod) : QPushButton(parent)
{
    this->learningMode = false;
    this->learningShow = false;
    this->isBestScore = false;
    this->draftMethod = draftMethod;
}


void ScoreButton::leaveEvent(QEvent * e)
{
    setFlat(true);
    if(learningMode)
    {
        learningShow = false;
        draw();
    }
    QPushButton::leaveEvent(e);
}


void ScoreButton::enterEvent(QEvent * e)
{
    setFlat(false);
    if(learningMode)
    {
        learningShow = true;
        draw();
    }
    QPushButton::enterEvent(e);
}


void ScoreButton::setLearningMode(bool value)
{
    this->learningMode = value;
    draw();
}


QString ScoreButton::getBackgroundImageCSS()
{
    switch(draftMethod)
    {
        case HearthArena:
            return "border-image: url(:/Images/bgScoreButtonHA.png) 0 0 0 0 stretch stretch;";
        case LightForge:
            return "border-image: url(:/Images/bgScoreButtonLF.png) 0 0 0 0 stretch stretch;";
        default:
            return "";
    }
}


void ScoreButton::getScoreColor(int &r, int &g, int &b, double score)
{
    int rating255 = 0;
    if(draftMethod == HearthArena)      rating255 = std::max(std::min((int)(score*2.55), 255), 0);
    else if(draftMethod == LightForge)  rating255 = std::max(std::min((int)((score-50)*2.55), 255), 0);
    r = std::min(255, (255 - rating255)*2);
    g = std::min(255,rating255*2);
    b = 0;
}


void ScoreButton::setScore(double score, bool isBestScore)
{
    this->score = score;
    this->isBestScore = isBestScore;
    draw();
}


void ScoreButton::draw()
{
    bool hideScore = learningMode && !learningShow;

    int r, g, b;
    getScoreColor(r, g, b, score);

    if(hideScore)    this->setText("?");
    else                this->setText(QString::number((int)score));
    QString gradientCSS = "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                          "stop: 0 black, "
                          "stop: 0.5 rgb("+ QString::number(r) +","+ QString::number(g) +","+ QString::number(b) +"), "
                          "stop: 1 black);";

    this->setStyleSheet(
            "QPushButton{background-color: " + (hideScore?"black;":gradientCSS) +
            "color: " + (hideScore?"white;":"black;") +

            "border-style: solid;border-color: black;" +

            "border-width: 1px;" +
            "border-radius: " + QString::number(width()/3) + "px;" +

            ((isBestScore&&!hideScore)?getBackgroundImageCSS():"") + "}");
    this->update();
}


