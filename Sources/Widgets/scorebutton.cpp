#include "scorebutton.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent, DraftMethod draftMethod) : QPushButton(parent)
{
    this->learningMode = false;
    this->learningShow = false;
    this->isBestScore = false;
    this->draftMethod = draftMethod;
    this->score = 0;
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
    else if(draftMethod == LightForge)  rating255 = std::max(std::min((int)((score)*2.55), 255), 0);
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

    QString rgb = "rgb("+ QString::number(r) +","+ QString::number(g) +","+ QString::number(b) +")";
    QString rgbMid = "rgb("+ QString::number(r/4) +","+ QString::number(g/4) +","+ QString::number(0) +")";
    QString gradientCSS = "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                          "stop: 0 " + rgbMid + ", "
                          "stop: 0.5 " + rgb + ", "
                          "stop: 1 " + rgbMid + ");";

    this->setStyleSheet(
            "QPushButton{background-color: " + (hideScore?"black;":gradientCSS) +
            "color: " + (hideScore?"white;":"black;") +

            "border-style: solid;border-color: black;" +

            "border-width: 1px;" +
            "border-radius: " + QString::number(width()/3) + "px;" +

            ((isBestScore&&!hideScore)?getBackgroundImageCSS():"") + "}");
    this->update();
}


void ScoreButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QFont &font = this->font();
    QPen pen(BLACK);
    pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    bool hideScore = learningMode && !learningShow;
    QString text = hideScore?"?":QString::number((int)score);
    QFontMetrics fm = QFontMetrics(font);
    int textWide = fm.width(text);
    int textHigh = fm.height();

    QPainterPath path;
    path.addText(this->width()/2 - textWide/2, this->height()/2 + textHigh*0.4, this->font(), text);
    painter.drawPath(path);
}


