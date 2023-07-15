#include "enemyranking.h"
#include "Sources/constants.h"
#include "../themehandler.h"
#include <QtWidgets>

EnemyRanking::EnemyRanking(QWidget *parent) : QLabel(parent)
{

}


void EnemyRanking::setEnemyRanking(const QString &tag, QList<EnemyRankingItem> enemyRankingItems)
{
    this->tag = tag;
    this->enemyRankingItems = enemyRankingItems;
    showEnemyRanking(40 + 30*enemyRankingItems.count());
    if(enemyRankingItems.isEmpty())
    {
        QTimer::singleShot(10000, this, [this] () {hideEnemyRanking();});
    }
}


void EnemyRanking::updateRankingItem(const QString &tag, EnemyRankingItem enemyRankingItem)
{
    if(this->tag != tag)    return;

    for(EnemyRankingItem &item : enemyRankingItems)
    {
        if(item.region == enemyRankingItem.region)
        {
            item.rank = enemyRankingItem.rank;
            item.rating = enemyRankingItem.rating;
            item.searchingTag = false;
            draw();
            return;
        }
    }
}


void EnemyRanking::showEnemyRanking(int endHeight)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(this->minimumHeight());
    animation->setEndValue(endHeight);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    animation = new QPropertyAnimation(this, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(this->minimumHeight());
    animation->setEndValue(endHeight);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    show();
}


void EnemyRanking::hideEnemyRanking()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(this->minimumHeight());
    animation->setEndValue(0);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished,
        [this]()
        {
            this->setFixedHeight(0);
            this->hide();
        });

    animation = new QPropertyAnimation(this, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(this->minimumHeight());
    animation->setEndValue(0);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}


QString EnemyRanking::getTag()
{
    return this->tag;
}


void EnemyRanking::draw()
{
    this->update();
}


void EnemyRanking::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPixmap canvas(width(), height());
    canvas.fill(ThemeHandler::themeColor1());
    QRect targetAll(0, 0, width(), height());
    drawPixmap(canvas, targetAll);

    QPainter painterObject(this);
    painterObject.drawPixmap(targetAll, canvas);
}


void EnemyRanking::drawPixmap(QPixmap &canvas, QRect &targetAll)
{
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QFont font(LG_FONT);
    int pixelSize = 30;
    font.setPixelSize(pixelSize);

    QPen pen(BLACK);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(WHITE);

    //Background
    painter.setOpacity(0.5);
    painter.drawPixmap(targetAll, QPixmap(":/Images/bgLeaderboard.png"));
    painter.setOpacity(1.0);

    //Draw Tag
    QString text;
    text = tag;
    Utility::shrinkText(font, text, pixelSize, width()*0.9);
    QFontMetrics fm = QFontMetrics(font);
    int textWide = fm.width(text);
    int textHigh = fm.height();

    QPainterPath path;
#ifdef Q_OS_WIN
    path.addText(this->width()/2 - textWide/2, 20 + textHigh*0.3, font, text);
#else
    path.addText(this->width()/2 - textWide/2, 20 + textHigh*0.4, font, text);
#endif
    painter.drawPath(path);


    int yPos = 20;
    for(const EnemyRankingItem &item : qAsConst(enemyRankingItems))
    {
        yPos += 30;

        painter.setBrush(item.searchingTag?ThemeHandler::themeColor2():WHITE);

        //Draw Rating
        text = QString::number(item.rating, 'g', 3);
        fm = QFontMetrics(font);
        textWide = fm.width(text);
        textHigh = fm.height();
        int ratingWide = textWide;

        path = QPainterPath();
    #ifdef Q_OS_WIN
        path.addText(this->width()*0.5 - textWide/2, yPos + textHigh*0.3, font, text);
    #else
        path.addText(this->width()*0.5 - textWide/2, yPos + textHigh*0.4, font, text);
    #endif
        painter.drawPath(path);


        //Draw Rank
        text = "#" + (item.rank>999?QString::number(item.rank/1000)+"k":QString::number(item.rank));
        fm = QFontMetrics(font);
        textWide = fm.width(text);
        textHigh = fm.height();

        path = QPainterPath();
    #ifdef Q_OS_WIN
        path.addText(this->width()*0.5 - ratingWide/2 - 20 - textWide, yPos + textHigh*0.3, font, text);
    #else
        path.addText(this->width()*0.5 - ratingWide/2 - 20 - textWide, yPos + textHigh*0.4, font, text);
    #endif
        painter.drawPath(path);


        //Draw Region
        text = item.region;
        fm = QFontMetrics(font);
        textWide = fm.width(text);
        textHigh = fm.height();

        path = QPainterPath();
    #ifdef Q_OS_WIN
        path.addText(this->width()*0.5 + ratingWide/2 + 20, yPos + textHigh*0.3, font, text);
    #else
        path.addText(this->width()*0.5 + ratingWide/2 + 20, yPos + textHigh*0.4, font, text);
    #endif
        painter.drawPath(path);
    }
}

