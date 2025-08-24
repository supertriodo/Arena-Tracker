#include "cardlistwindow.h"
#include "../Cards/secretcard.h"
#include "../utility.h"
#include <QtWidgets>

CardListWindow::CardListWindow(QWidget *parent, SecretsHandler *secretsHandler) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    this->secretsHandler = secretsHandler;

    QWidget *mainWidget = new QWidget(this);
    QHBoxLayout *hLayout = new QHBoxLayout(mainWidget);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    listWidget = new MoveListWidget(this);
    listWidget2 = new MoveListWidget(this);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hLayout->addWidget(listWidget);
    hLayout->addWidget(listWidget2);
    setCentralWidget(mainWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


CardListWindow::~CardListWindow()
{
}


void CardListWindow::clear()
{
    listWidget->clear();
    listWidget2->clear();
    listWidget2->hide();
}


void CardListWindow::loadSecret(int id, QRect &rectCard, int maxTop, int maxBottom, bool alignReverse)
{
    //Clear and delete items
    clear();

    //Get codes
    QStringList codes = secretsHandler->getSecretOptionCodes(id);
    if(codes.isEmpty())
    {
        hide();
        return;
    }

    for(const QString &code: qAsConst(codes))
    {
        SecretCard card(code);
        card.listItem = new QListWidgetItem();//Items son auto delete en clear()
        listWidget->addItem(card.listItem);
        card.draw();
    }

    int winWidth = listWidget->sizeHintForColumn(0);
    int winHeight = codes.count()*listWidget->sizeHintForRow(0) + 2*listWidget->frameWidth();
    setFixedSize(winWidth, winHeight);

    QPoint center = rectCard.center();
    bool showAtLeft = !Utility::isLeftOfScreen(center);

    int moveX, moveY;
    if(alignReverse)    showAtLeft = !showAtLeft;
    if(showAtLeft)  moveX = rectCard.left()-winWidth;
    else            moveX = rectCard.right();

    moveY = center.y()-winHeight/2;
    if((maxTop!=-1) && (moveY<maxTop)) moveY=maxTop;
    else if((maxBottom!=-1) && ((moveY+winHeight)>maxBottom))
    {
        if((maxBottom-winHeight)<maxTop)    moveY=maxTop;
        else                                moveY=maxBottom-winHeight;
    }

    move(moveX, moveY);
    show();
}


void CardListWindow::loadDraftItem(QList<SynergyCard> &synergyCardList, QRect &rectCard, int maxTop, int maxBottom, bool alignReverse)
{
    //Clear and delete items
    clear();

    if(synergyCardList.isEmpty())
    {
        hide();
        return;
    }

    for(SynergyCard &card: synergyCardList)
    {
        card.listItem = new QListWidgetItem();//Items son auto delete en clear()
        listWidget->addItem(card.listItem);
        card.draw();
    }

    int winWidth = listWidget->sizeHintForColumn(0);
    int winHeight = listWidget->count()*listWidget->sizeHintForRow(0) + 2*listWidget->frameWidth();
    setFixedSize(winWidth, winHeight);

    QPoint center = rectCard.center();
    bool showAtLeft = !Utility::isLeftOfScreen(center);

    int moveX, moveY;
    if(alignReverse)    showAtLeft = !showAtLeft;
    if(showAtLeft)  moveX = rectCard.left()-winWidth;
    else            moveX = rectCard.right();

    moveY = center.y()-winHeight/2;
    if((maxTop!=-1) && (moveY<maxTop)) moveY=maxTop;
    else if((maxBottom!=-1) && ((moveY+winHeight)>maxBottom))
    {
        if((maxBottom-winHeight)<maxTop)    moveY=maxTop;
        else                                moveY=maxBottom-winHeight;
    }

    move(moveX, moveY);
    show();
}


void CardListWindow::loadDraftOverlayItem(QList<SynergyCard> &synergyCardList, QPoint &originList, int maxLeft, int maxRight)
{
    //Clear and delete items
    clear();

    if(synergyCardList.isEmpty())
    {
        hide();
        return;
    }

    int doubleColumn = (synergyCardList.count()>=10);
    int i=0;
    for(SynergyCard &card: synergyCardList)
    {
        card.listItem = new QListWidgetItem();//Items son auto delete en clear()
        if(!doubleColumn || (i%2==0))
        {
            listWidget->addItem(card.listItem);
        }
        else
        {
            listWidget2->addItem(card.listItem);
        }
        card.draw();
        i++;
    }

    if(doubleColumn)    listWidget2->show();
    int lwWidth = listWidget->sizeHintForColumn(0);
    int lw2Width = listWidget2->sizeHintForColumn(0);
    int winWidth = doubleColumn?(lwWidth + lw2Width):(lwWidth);
    int winHeight = std::max(listWidget->count()*listWidget->sizeHintForRow(0) + 2*listWidget->frameWidth(),
                             listWidget2->count()*listWidget2->sizeHintForRow(0) + 2*listWidget2->frameWidth());
    setFixedSize(winWidth, winHeight);

    int moveX, moveY;
    moveX = originList.x() - winWidth/2;
    moveY = originList.y();

    if((maxLeft!=-1) && (moveX<maxLeft))                    moveX = maxLeft;
    else if((maxRight!=-1) && ((moveX+winWidth)>maxRight))  moveX = maxRight - winWidth;

    move(moveX, moveY);
    show();
}


void CardListWindow::enterEvent(QEvent * e)
{
    QMainWindow::enterEvent(e);
    hide();
}


void CardListWindow::leaveEvent(QEvent * e)
{
    QMainWindow::leaveEvent(e);
    hide();
}
