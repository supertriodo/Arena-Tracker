#include "drawcardhandler.h"
#include <QtWidgets>

DrawCardHandler::DrawCardHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->drawAnimating = false;
    this->drawDisappear = 10;

    completeUI();
}

DrawCardHandler::~DrawCardHandler()
{
    drawCardList.clear();
    ui->drawListWidget->clear();
}


void DrawCardHandler::completeUI()
{
    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->drawListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->drawListWidget->setFixedHeight(0);
    ui->drawListWidget->setMouseTracking(true);

    connect(ui->drawListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDrawCardEntered(QListWidgetItem*)));
}


void DrawCardHandler::adjustDrawSize()
{
    if(drawAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustDrawSize()));
        return;
    }

    int rowHeight = ui->drawListWidget->sizeHintForRow(0);
    int rows = drawCardList.count();
    int height = rows*rowHeight + 2*ui->drawListWidget->frameWidth();
//    int maxHeight = (ui->drawListWidget->height()+ui->enemyHandListWidget->height())*4/5;
//    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->drawListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->drawListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->drawListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->drawListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->drawAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearDrawAnimating()));
}


void DrawCardHandler::clearDrawAnimating()
{
    this->drawAnimating = false;
    if(drawCardList.empty())    ui->drawListWidget->setHidden(true);
}


void DrawCardHandler::newDrawCard(QString code, bool mulligan)
{
    DrawCard drawCard(code);
    drawCard.listItem = new QListWidgetItem();
    drawCardList.append(drawCard);
    ui->drawListWidget->addItem(drawCard.listItem);
    drawCard.draw();
    emit checkCardImage(code);
    ui->drawListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));

    if(!mulligan && this->drawDisappear>0)
    {
        QTimer::singleShot(this->drawDisappear*1000, this, SLOT(removeOldestDrawCard()));
    }
}


void DrawCardHandler::removeOldestDrawCard()
{
    if(drawCardList.empty())    return;
    DrawCard drawCard = drawCardList.takeFirst();
    ui->drawListWidget->removeItemWidget(drawCard.listItem);
    delete drawCard.listItem;
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));
}


void DrawCardHandler::playerCardToHand(int id, QString code, int turn)
{
    Q_UNUSED(id);
    if(this->drawDisappear>=0)          newDrawCard(code, turn==0);
}


void DrawCardHandler::clearDrawList()
{
    ui->drawListWidget->clear();
    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setFixedHeight(0);
    drawCardList.clear();
}


void DrawCardHandler::setDrawDisappear(int value)
{
    this->drawDisappear = value;
    clearDrawList();
}


void DrawCardHandler::findDrawCardEntered(QListWidgetItem * item)
{
    QString code = drawCardList[ui->drawListWidget->row(item)].getCode();

    QRect rectCard = ui->drawListWidget->visualItemRect(item);
    QPoint posCard = ui->drawListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int drawListTop = -1;
    int drawListBottom = ui->tabEnemy->mapToGlobal(QPoint(0,ui->tabEnemy->height())).y();
    emit cardEntered(code, globalRectCard, drawListTop, drawListBottom);
}


void DrawCardHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<DrawCard>::iterator it = drawCardList.begin(); it != drawCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->draw();
        }
    }
}


void DrawCardHandler::redrawClassCards()
{
    foreach(DrawCard drawCard, drawCardList)
    {
        drawCard.draw();
    }
}


void DrawCardHandler::redrawSpellWeaponCards()
{
    foreach(DrawCard drawCard, drawCardList)
    {
        CardType cardType = drawCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            drawCard.draw();
        }
    }
}


void DrawCardHandler::redrawAllCards()
{
    foreach(DrawCard drawCard, drawCardList)
    {
        drawCard.draw();
    }
}
