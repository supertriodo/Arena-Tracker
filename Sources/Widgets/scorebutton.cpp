#include "scorebutton.h"
#include "../themehandler.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent, ScoreSource scoreSource, bool normalizedLF) : QLabel(parent)
{
    this->normalizedLF = normalizedLF;
    this->learningMode = false;
    this->learningShow = false;
    this->isBestScore = false;
    this->scoreSource = scoreSource;
    this->score = 0;
}


void ScoreButton::mousePressEvent(QMouseEvent *event)
{
    QDesktopServices::openUrl(QUrl(
        "https://hsreplay.net"
        ));
    QLabel::mousePressEvent(event);
}


void ScoreButton::leaveEvent(QEvent * e)
{
    if(learningMode)
    {
        emit spreadLearningShow(false);
    }
    QLabel::leaveEvent(e);
}


void ScoreButton::enterEvent(QEvent * e)
{
    if(learningMode)
    {
        emit spreadLearningShow(true);
    }
    QLabel::enterEvent(e);
}


void ScoreButton::setLearningShow(bool value)
{
    learningShow = value;
    draw();
}


void ScoreButton::setLearningMode(bool value)
{
    this->learningMode = value;
    draw();
}


void ScoreButton::setNormalizedLF(bool value)
{
    this->normalizedLF = value;
    //No necesita draw porque al cambiar normalized en draftHandler el vuelve a mostrar los scores lo que en cascada causara un draw aqui.
}


void ScoreButton::getScoreColor(int &r, int &g, int &b, double score)
{
    int rating255 = 0;
    if(scoreSource == Score_HearthArena)        rating255 = std::max(std::min((int)(score*2.55), 255), 0);//0<-->100
    else if(scoreSource == Score_LightForge)    rating255 = std::max(std::min((int)(score*2.55), 255), 0);
    else if(scoreSource == Score_Heroes)        rating255 = std::max(std::min((int)((score-45)/10*255), 255), 0);//45<-->55
    r = std::min(255, (255 - rating255)*2);
    g = std::min(255, rating255*2);
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
    getScoreColor(r, g, b, (scoreSource == Score_LightForge)?Utility::normalizeLF(score, true):score);

    QString rgb = "rgb("+ QString::number(r) +","+ QString::number(g) +","+ QString::number(b) +")";
    QString rgbMid = "rgb("+ QString::number(r/4) +","+ QString::number(g/4) +","+ QString::number(0) +")";
    QString gradientCSS = "qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                          "stop: 0 " + rgbMid + ", "
                          "stop: 0.5 " + rgb + ", "
                          "stop: 1 " + rgbMid + ");";

    QString backgroundColor;
    if(!isEnabled())    backgroundColor = "grey;";
    else if(hideScore)  backgroundColor = "black;";
    else                backgroundColor = gradientCSS;
    this->setStyleSheet(
            "QLabel{background-color: " + backgroundColor +
            "border-style: solid; border-color: transparent;" +
            "border-width: " + QString::number(width()/3) + "px;" +
            "border-radius: " + QString::number(width()/2) + "px;}");
    this->update();
}


void ScoreButton::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPixmap canvas(width(), height());
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    float drawScore;
    if(scoreSource == Score_LightForge)         drawScore = (int)Utility::normalizeLF(score, this->normalizedLF);
    else if(scoreSource == Score_HearthArena)   drawScore = (int)score;
    else if(scoreSource == Score_Heroes)        drawScore = score;

    QFont font(LG_FONT);
    if(scoreSource == Score_Heroes) font.setPixelSize(width()/3.5);
    else if(drawScore > 99)         font.setPixelSize(width()/3.2);
    else                            font.setPixelSize(width()/2.7);

    QPen pen(BLACK);
    pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    QRect targetAll(0, 0, width(), height());
    bool hideScore = learningMode && !learningShow;
    if(hideScore)
    {
        if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haCloseFile()));
        else if(scoreSource == Score_LightForge)    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfCloseFile()));
        else if(scoreSource == Score_Heroes)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfCloseFile()));
    }
    else
    {
        if(isBestScore)
        {
            if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haBestFile()));
            else if(scoreSource == Score_LightForge)    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfBestFile()));
            else if(scoreSource == Score_Heroes)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfBestFile()));
        }

        //Draw Score
        QString text = QString::number(drawScore);
        QFontMetrics fm = QFontMetrics(font);
        int textWide = fm.width(text);
        int textHigh = fm.height();

        QPainterPath path;
#ifdef Q_OS_WIN
        path.addText(this->width()/2 - textWide/2, this->height()/2 + textHigh*0.3, font, text);
#else
        path.addText(this->width()/2 - textWide/2, this->height()/2 + textHigh*0.4, font, text);
#endif
        painter.drawPath(path);

        //Draw heroe winrate %
        if(scoreSource == Score_Heroes)
        {
            font.setPixelSize(width()/4.5);
            QString text = "%";
            QFontMetrics fm = QFontMetrics(font);
            int textWide = fm.width(text);
            int textHigh = fm.height();

            QPainterPath path;
#ifdef Q_OS_WIN
            path.addText(this->width()/2 - textWide/2, this->height()*0.68 + textHigh*0.3, font, text);
#else
            path.addText(this->width()/2 - textWide/2, this->height()*0.68 + textHigh*0.4, font, text);
#endif
            painter.drawPath(path);
        }

        if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haOpenFile()));
        else if(scoreSource == Score_LightForge)    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfOpenFile()));
        else if(scoreSource == Score_Heroes)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfOpenFile()));
    }

    QPainter painterObject(this);
    if(isEnabled())
    {
        painterObject.drawPixmap(targetAll, canvas);
    }
    else
    {
        QIcon icon(canvas);
        painterObject.drawPixmap(targetAll, icon.pixmap(width(), height(), QIcon::Disabled, QIcon::On));
    }
}


