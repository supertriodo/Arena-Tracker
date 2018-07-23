#include "cardwindow.h"
#include "../utility.h"
#include <QtWidgets>

CardWindow::CardWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    cardLabel = new QLabel(this);
    alwaysHidden = false;
    setCentralWidget(cardLabel);
    setMinimumSize(0,0);
    resize(WCARD,HCARD);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


CardWindow::~CardWindow()
{
}


void CardWindow::scale(int value_x10)
{
    if(value_x10 < 10)
    {
        alwaysHidden = true;
        hide();
    }
    else
    {
        alwaysHidden = false;
        float value = value_x10/10.0f;
        setMinimumSize(0,0);
        resize(static_cast<int>(value*WCARD), static_cast<int>(value*HCARD));
    }
}


void CardWindow::loadCard(QString code, QRect rectCard, int maxTop, int maxBottom, bool alignReverse)
{
    if(alwaysHidden || code.isEmpty() ||
        !QFileInfo(Utility::hscardsPath() + "/" + code + ".png").exists())
    {
        hide();
        return;
    }

    QPoint center = rectCard.center();
    bool showAtLeft = !Utility::isLeftOfScreen(center);

    int winWidth = width();
    int winHeight = height();

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
    cardLabel->setPixmap(QPixmap(Utility::hscardsPath() + "/" + code + ".png").copy(5,34,WCARD,HCARD)
                         .scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    show();
}


void CardWindow::enterEvent(QEvent * e)
{
    QMainWindow::enterEvent(e);
    hide();
}


void CardWindow::leaveEvent(QEvent * e)
{
    QMainWindow::leaveEvent(e);
    hide();
}
