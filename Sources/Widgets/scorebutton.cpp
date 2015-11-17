#include "scorebutton.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent) : QPushButton(parent)
{
    this->learningMode = false;
    this->learningShow = false;
}


void ScoreButton::leaveEvent(QEvent * e)
{
    setFlat(true);
    if(learningMode)
    {
        learningShow = false;
        setScore(score);
    }
    QPushButton::leaveEvent(e);
}


void ScoreButton::enterEvent(QEvent * e)
{
    setFlat(false);
    if(learningMode)
    {
        learningShow = true;
        setScore(score);
    }
    QPushButton::enterEvent(e);
}


void ScoreButton::setLearningMode(bool value)
{
    this->learningMode = value;
    this->setScore(score);
}


void ScoreButton::setScore(double score)
{
    bool hideScore = learningMode && !learningShow;
    this->score = score;
    int rating255 = std::max(std::min((int)(score*2.55), 255), 0);
    int r = std::min(255, (255 - rating255)*2);
    int g = std::min(255,rating255*2);
    int b = 0;

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

            "border-width: " + QString::number(width()/20) + "px;border-radius: "
            + QString::number(width()/3) + "px;}");
}


