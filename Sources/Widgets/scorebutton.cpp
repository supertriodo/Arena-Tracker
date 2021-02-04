#include "scorebutton.h"
#include "../themehandler.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent, ScoreSource scoreSource) : QLabel(parent)
{
    this->learningMode = false;
    this->learningShow = false;
    this->isBestScore = false;
    this->scoreSource = scoreSource;
    this->score = 0;
}


void ScoreButton::mousePressEvent(QMouseEvent *event)
{
    if(scoreSource == Score_HearthArena)
    {
        QDesktopServices::openUrl(QUrl(
            "https://www.heartharena.com/tierlist"
            ));
    }
    else if(scoreSource == Score_LightForge)
    {
        QDesktopServices::openUrl(QUrl(
            "http://thelightforge.com"
            ));
    }
    else if(scoreSource == Score_HSReplay)
    {
        emit showHSRwebPicks();
    }

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


void ScoreButton::getScoreColor(int &r, int &g, int &b, float score)
{
    int rating255 = 0;
    if(scoreSource == Score_HearthArena)        rating255 = std::max(std::min(static_cast<int>(score*2.55f), 255), 0);//0<-->100
    else if(scoreSource == Score_LightForge)    rating255 = std::max(std::min(static_cast<int>(score*2.55f), 255), 0);
    else if(scoreSource == Score_Heroes)        rating255 = std::max(std::min(static_cast<int>((score-45)/10*255), 255), 0);//45<-->55
    else if(scoreSource == Score_HSReplay)      rating255 = std::max(std::min(static_cast<int>((score-50)/10*255), 255), 0);//50<-->60
    r = std::min(255, (255 - rating255)*2);
    g = std::min(255, rating255*2);
    b = 0;
}


void ScoreButton::setScore(float score, bool isBestScore, int includedDecks)
{
    this->score = score;
    this->isBestScore = isBestScore;
    this->includedDecks = includedDecks;
    if(scoreSource == Score_HSReplay && includedDecks >= 0) this->setToolTip(QString::number(includedDecks) + " played");
    else    this->setToolTip("");
    draw();
}


void ScoreButton::draw()
{
    bool hideScore = learningMode && !learningShow;

    int r, g, b;
    getScoreColor(r, g, b, score);

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

    float drawScore = 0;
    if(scoreSource == Score_LightForge)         drawScore = static_cast<int>(score);
    else if(scoreSource == Score_HearthArena)   drawScore = static_cast<int>(score);
    else                                        drawScore = score;

    QFont font(LG_FONT);
    if( scoreSource == Score_Heroes ||
        scoreSource == Score_HSReplay)  font.setPixelSize(static_cast<int>(width()/3.5));
    else if(drawScore > 99)             font.setPixelSize(static_cast<int>(width()/3.2));
    else                                font.setPixelSize(static_cast<int>(width()/2.7));

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
        else                                        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrCloseFile()));
    }
    else
    {
        //Best Score background
        if(isBestScore)
        {
            if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haBestFile()));
            else if(scoreSource == Score_LightForge)    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfBestFile()));
            else                                        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrBestFile()));
        }

        //Not enough HSR decks
        if(scoreSource == Score_HSReplay && includedDecks >= 0 && includedDecks < MIN_HSR_DECKS)
        {
            float closeHeight = (1 - includedDecks/static_cast<float>(MIN_HSR_DECKS)) * 72 + 28;
            QRect source(0, 0, 128, closeHeight);
            QRect target(0, 0, width(), closeHeight/128*height());
            painter.drawPixmap(target, QPixmap(ThemeHandler::speedCloseFile()), source);
        }

        //Draw Score
        QString text = QString::number(static_cast<double>(drawScore));
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
        if(scoreSource == Score_Heroes || scoreSource == Score_HSReplay)
        {
            font.setPixelSize(static_cast<int>(width()/5.0));
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
        else                                        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrOpenFile()));

        //Best Score text
        if(isBestScore)
        {
            if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haTextFile()));
            else if(scoreSource == Score_LightForge)    painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfTextFile()));
            else                                        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrTextFile()));
        }
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


