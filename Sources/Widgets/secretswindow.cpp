#include "secretswindow.h"
#include "../Cards/secretcard.h"
#include "../utility.h"
#include <QtWidgets>

SecretsWindow::SecretsWindow(QWidget *parent, SecretsHandler *secretsHandler) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    this->secretsHandler = secretsHandler;
    listWidget = new MoveListWidget(this);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(listWidget);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


SecretsWindow::~SecretsWindow()
{
}


void SecretsWindow::loadSecret(int id, QRect rectCard, int maxTop, int maxBottom, bool alignReverse)
{
    //Clear and delete items
    listWidget->clear();

    //Get codes
    QStringList codes = secretsHandler->getSecretOptionCodes(id);
    if(codes.isEmpty())
    {
        hide();
        return;
    }

    foreach(QString code, codes)
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


