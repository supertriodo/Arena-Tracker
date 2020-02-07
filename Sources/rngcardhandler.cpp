#include "rngcardhandler.h"
#include <QtWidgets>

RngCardHandler::RngCardHandler(QObject *parent, Ui::Extended *ui, PlanHandler *planHandler) : QObject(parent)
{
    this->ui = ui;
    this->planHandler = planHandler;
    this->rngAnimating = false;
    this->showRngList = false;
    this->patreonVersion = false;

    completeUI();
}


RngCardHandler::~RngCardHandler()
{
    rngCardList.clear();
    ui->rngListWidget->clear();
    delete bombWindow;
}


void RngCardHandler::completeUI()
{
    ui->rngListWidget->setHidden(true);
    ui->rngListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->rngListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->rngListWidget->setFixedHeight(0);
    ui->rngListWidget->setMouseTracking(true);

    createBombWindow();
}


void RngCardHandler::createBombWindow()
{
    bombWindow = new BombWindow(static_cast<QWidget*>(this->parent()), ui->planGraphicsView->scene());
    connect(ui->rngListWidget,SIGNAL(leave()),
            bombWindow, SLOT(hide()));
    connect(ui->rngListWidget,SIGNAL(leave()),
            planHandler, SLOT(resetDeadProbs()));
    connect(ui->rngListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(rngCardEntered(QListWidgetItem*)));
}


void RngCardHandler::rngCardEntered(QListWidgetItem * item)
{
    RngCard rngCard = rngCardList[ui->rngListWidget->row(item)];
    QString code = rngCard.getCode();
    planHandler->showLastTurn();
    planHandler->resetDeadProbs();
    showBombWindow();
    planHandler->checkBomb(code);
}


void RngCardHandler::showBombWindow()
{
    QSize sizeTab = ui->tabEnemy->geometry().size();
    QPoint topLeft = ui->tabEnemy->mapToGlobal(ui->tabEnemy->geometry().topLeft());
    QRect rectTab(topLeft, sizeTab);
    bombWindow->showAt(rectTab);
}


void RngCardHandler::newRngCard(QString code, int id)
{
    RngCard rngCard(code);
    rngCard.id = id;
    rngCard.listItem = new QListWidgetItem();
    rngCardList.append(rngCard);
    ui->rngListWidget->addItem(rngCard.listItem);
    rngCard.draw();
    emit checkCardImage(code);
    updateShowRngList();
    QTimer::singleShot(10, this, SLOT(adjustRngSize()));
}


void RngCardHandler::removeRngCard(int id, QString code)
{
    (void) code;

    int i=0;
    for (QList<RngCard>::iterator it = rngCardList.begin(); it != rngCardList.end(); it++, i++)
    {
        if(it->id == id)
        {
            delete it->listItem;
            rngCardList.removeAt(i);
            QTimer::singleShot(10, this, SLOT(adjustRngSize()));
            return;
        }
    }
}


void RngCardHandler::adjustRngSize()
{
    if(rngAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustRngSize()));
        return;
    }

    int rowHeight = ui->rngListWidget->sizeHintForRow(0);
    int rows = rngCardList.count();
    int height = rows*rowHeight + 2*ui->rngListWidget->frameWidth();
//    int maxHeight = (ui->rngListWidget->height()+ui->enemyHandListWidget->height())*4/5;
//    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->rngListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->rngListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->rngListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->rngListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->rngAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearRngAnimating()));
}


void RngCardHandler::clearRngAnimating()
{
    this->rngAnimating = false;
    if(rngCardList.empty())    ui->rngListWidget->setHidden(true);
}


void RngCardHandler::setShowRngList(bool value)
{
    this->showRngList = value;
    updateShowRngList();
}


void RngCardHandler::setPremium(bool premium)
{
    this->patreonVersion = premium;
    updateShowRngList();
}


void RngCardHandler::updateShowRngList()
{
    if(!showRngList || !patreonVersion || rngCardList.empty())  ui->rngListWidget->setHidden(true);
    else                                                        ui->rngListWidget->setHidden(false);
}


void RngCardHandler::playerCardToHand(int id, QString code, int turn)
{
    Q_UNUSED(turn);
    if(planHandler->isCardBomb(code))   newRngCard(code, id);
}


void RngCardHandler::clearRngList()
{
    ui->rngListWidget->clear();
    ui->rngListWidget->setHidden(true);
    ui->rngListWidget->setFixedHeight(0);
    rngCardList.clear();
}


void RngCardHandler::setTheme()
{
    bombWindow->setTheme();
}


void RngCardHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<RngCard>::iterator it = rngCardList.begin(); it != rngCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->draw();
        }
    }
}


void RngCardHandler::redrawClassCards()
{
    foreach(RngCard rngCard, rngCardList)
    {
        if(rngCard.getCardClass()<9)
        {
            rngCard.draw();
        }
    }
}


void RngCardHandler::redrawSpellWeaponCards()
{
    foreach(RngCard rngCard, rngCardList)
    {
        CardType cardType = rngCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            rngCard.draw();
        }
    }
}


void RngCardHandler::redrawAllCards()
{
    foreach(RngCard rngCard, rngCardList)
    {
        rngCard.draw();
    }
}
