#include "scorebutton.h"
#include "../themehandler.h"
#include <QtWidgets>

float ScoreButton::heroScores[NUM_HEROS] = {0};
int ScoreButton::playerRuns[NUM_HEROS] = {0}, ScoreButton::playerWins[NUM_HEROS] = {0}, ScoreButton::playerLost[NUM_HEROS] = {0};
float ScoreButton::minHeroScore, ScoreButton::maxHeroScore;

ScoreButton::ScoreButton(QWidget *parent, ScoreSource scoreSource, int classOrder) : QLabel(parent)
{
    this->learningMode = false;
    this->learningShow = false;
    this->bestScoreOpacity = 0;
    this->scoreSource = scoreSource;
    this->score = 0;
    this->includedDecks = -1;
    setClassOrder(classOrder);
}


void ScoreButton::mousePressEvent(QMouseEvent *event)
{
    if(scoreSource == Score_HearthArena)
    {
        QDesktopServices::openUrl(QUrl(
            "https://www.heartharena.com/tierlist"
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


void ScoreButton::setHeroScores(float heroScores[NUM_HEROS])
{
    minHeroScore = 100;
    maxHeroScore = 0;
    for(int i=0; i<NUM_HEROS; i++)
    {
        ScoreButton::heroScores[i] = heroScores[i];
        minHeroScore = std::min(heroScores[i], minHeroScore);
        maxHeroScore = std::max(heroScores[i], maxHeroScore);
    }
}
void ScoreButton::setPlayerRuns(int playerRuns[NUM_HEROS])
{
    for(int i=0; i<NUM_HEROS; i++)  ScoreButton::playerRuns[i] = playerRuns[i];
}
void ScoreButton::setPlayerWins(int playerWins[NUM_HEROS])
{
    for(int i=0; i<NUM_HEROS; i++)  ScoreButton::playerWins[i] = playerWins[i];
}
void ScoreButton::setPlayerLost(int playerLost[NUM_HEROS])
{
    for(int i=0; i<NUM_HEROS; i++)  ScoreButton::playerLost[i] = playerLost[i];
}


void ScoreButton::addRun(int classOrder, int wins, int lost)
{
    if(classOrder == -1)    return;
    ScoreButton::playerWins[classOrder] += wins;
    ScoreButton::playerLost[classOrder] += lost;
    ScoreButton::playerRuns[classOrder]++;
}


float ScoreButton::getHeroScore(int classOrder)
{
    if(classOrder == -1)    return 0;
    return ScoreButton::heroScores[classOrder];
}
float ScoreButton::getPlayerRun(int classOrder)
{
    if(classOrder == -1)    return 0;
    return ScoreButton::playerRuns[classOrder];
}
float ScoreButton::getPlayerWinrate(int classOrder)
{
    if(classOrder == -1)    return 0;
    int wins = ScoreButton::playerWins[classOrder];
    int lost = ScoreButton::playerLost[classOrder];
    if((wins+lost) > 0) return (wins*100/static_cast<float>(wins+lost));
    return 0;
}


void ScoreButton::setClassOrder(int classOrder)
{
    if(classOrder>=0 && classOrder<NUM_HEROS)   this->classOrder = classOrder;
    else                                        this->classOrder = -1;
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
    if(scoreSource == Score_HearthArena)
    {//0<-->100
        rating255 = std::max(std::min(static_cast<int>(score*2.55f), 255), 0);
    }
    else if(scoreSource == Score_HSReplay)
    {
        if(classOrder == -1)
        {//50<-->60
            rating255 = std::max(std::min(static_cast<int>((score-50)/10*255), 255), 0);
        }
        else
        {//heroWR*9+50/10<-->+10
            rating255 = std::max(std::min(static_cast<int>((score-((getHeroScore(classOrder)*9+50)/10))/10*255), 255), 0);
        }
    }
    else if(scoreSource == Score_Heroes || scoreSource == Score_Heroes_Player)
    {//minHeroScore<-->maxHeroScore
        rating255 = std::max(std::min(static_cast<int>((score-minHeroScore)/(maxHeroScore-minHeroScore)*255), 255), 0);
    }
    else
    {//0<-->100
        rating255 = std::max(std::min(static_cast<int>(score*2.55f), 255), 0);
    }

    r = std::min(255, (255 - rating255)*2);
    g = std::min(255, rating255*2);
    b = 0;
}


void ScoreButton::setScore(float score, float bestScore, int includedDecks)
{
    this->score = score;

    if(scoreSource == Score_HSReplay || scoreSource == Score_Heroes || scoreSource == Score_Heroes_Player)
    {
            bestScoreOpacity = (1 - (bestScore - score));
    }
    else    bestScoreOpacity = (1 - ((bestScore - score)/10.0));
    bestScoreOpacity = std::fmax(0, std::fmin(1, bestScoreOpacity));
    if(bestScoreOpacity>0)              bestScoreOpacity = 0.5 + (bestScoreOpacity/2.0);

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
    QRect targetAll(0, 0, width(), height());
    drawPixmap(canvas, targetAll);

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


void ScoreButton::drawPixmap(QPixmap &canvas, QRect &targetAll, bool bigFont)
{
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    float drawScore = 0;
    if(scoreSource == Score_HearthArena || bigFont) drawScore = static_cast<int>(score);
    else                                            drawScore = score;

    QFont font(LG_FONT);
    if(bigFont)
    {
        const float k = 0.85;
        if(drawScore > 99)                      font.setPixelSize(static_cast<int>(width()/(3.2*k)));
        else                                    font.setPixelSize(static_cast<int>(width()/(2.7*k)));
    }
    else
    {
        if( scoreSource == Score_Heroes || scoreSource == Score_Heroes_Player ||
                scoreSource == Score_HSReplay)  font.setPixelSize(static_cast<int>(width()/3.5));
        else if(drawScore > 99)                 font.setPixelSize(static_cast<int>(width()/3.2));
        else                                    font.setPixelSize(static_cast<int>(width()/2.7));
    }

    QPen pen(BLACK);
    if(bigFont) pen.setWidth(2);
    else        pen.setWidth(font.pixelSize()/20);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    bool hideScore = learningMode && !learningShow;
    if(hideScore)
    {
        if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haCloseFile()));
        else if(scoreSource == Score_Heroes_Player) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfCloseFile()));
        else/* if(scoreSource == Score_HSReplay)*/  painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrCloseFile()));
    }
    else
    {
        //Best Score background
        if(bestScoreOpacity==1)
        {
            if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haBestFile()));
            else if(scoreSource == Score_Heroes_Player) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfBestFile()));
            else/* if(scoreSource == Score_HSReplay)*/  painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrBestFile()));
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
        QString text;
        if(scoreSource == Score_Heroes_Player && drawScore == 0)    text = "--";
        else                                                        text = QString::number(drawScore, 'g', 3);
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
        if(!bigFont && (scoreSource == Score_Heroes || scoreSource == Score_HSReplay || scoreSource == Score_Heroes_Player))
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

            //Runs
            if(scoreSource == Score_Heroes_Player && classOrder != -1)
            {
                QString text = QString::number(getPlayerRun(classOrder));
                int textWide = fm.width(text);
                QPainterPath path;
        #ifdef Q_OS_WIN
                path.addText(this->width()/2 - textWide/2, this->height()*0.3 + textHigh*0.3, font, text);
        #else
                path.addText(this->width()/2 - textWide/2, this->height()*0.3 + textHigh*0.4, font, text);
        #endif
                painter.drawPath(path);
            }
        }

        if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haOpenFile()));
        else if(scoreSource == Score_Heroes_Player) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::lfOpenFile()));
        else/* if(scoreSource == Score_HSReplay)*/  painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrOpenFile()));

        //Best Score text
        if(bestScoreOpacity>0)
        {
            painter.setOpacity(bestScoreOpacity);
            if(scoreSource == Score_HearthArena)        painter.drawPixmap(targetAll, QPixmap(ThemeHandler::haTextFile()));
            else if(scoreSource == Score_Heroes_Player) painter.drawPixmap(targetAll, QPixmap(ThemeHandler::youTextFile()));
            else/* if(scoreSource == Score_HSReplay)*/  painter.drawPixmap(targetAll, QPixmap(ThemeHandler::hsrTextFile()));
            painter.setOpacity(1.0);
        }

        //Class icon
        if((scoreSource == Score_Heroes || scoreSource == Score_Heroes_Player) && classOrder != -1)
        {
            int iconWidth = width()*0.30;

            painter.drawPixmap(width()*0.35, height()*0.7, iconWidth, iconWidth,
                               QPixmap(ThemeHandler::heroFile(classOrder)));
        }
    }
}


QIcon ScoreButton::scoreIcon(ScoreSource scoreSource, float score, int size)
{
    ScoreButton scoreButton(nullptr, scoreSource, -1);
    scoreButton.setFixedSize(size, size);
    scoreButton.score = score;

    int r, g, b;
    scoreButton.getScoreColor(r, g, b, score);

    QPixmap canvas(size, size);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);
    painter.setBrush(QColor(r, g, b));
    painter.drawEllipse(QPoint(size/2,size/2), size/3, size/3);
    painter.end();

    QRect targetAll(0, 0, size, size);
    scoreButton.drawPixmap(canvas, targetAll, true);

    QIcon icon(canvas.copy(size/8, size/8, size*0.75, size*0.75));
    return icon;
}
