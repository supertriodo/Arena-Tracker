#include "cardwindow.h"
#include "../utility.h"
#include <QtWidgets>

CardWindow::CardWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    cardLabel = new QLabel(this);
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
    float value = value_x10/10.0;
    setMinimumSize(0,0);
    resize(value*WCARD, value*HCARD);
}


void CardWindow::loadCard(QString code, QRect rectCard, int maxTop, int maxBottom, bool alignReverse)
{
    if(code.isEmpty() ||
        !QFileInfo(Utility::hscardsPath() + "/" + code + ".png").exists())
    {
        hide();
        return;
    }

    QPoint center = rectCard.center();
    int midX = center.x();
    int midY = center.y();
    int topScreen, bottomScreen, leftScreen, rightScreen;
    bool showAtLeft = false;

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect screenRect = screen->geometry();
        topScreen = screenRect.y();
        bottomScreen = topScreen + screenRect.height();
        leftScreen = screenRect.x();
        rightScreen = leftScreen + screenRect.width();

        if(midX < leftScreen || midX > rightScreen ||
                midY < topScreen || midY > bottomScreen) continue;

        if(midX-leftScreen > rightScreen-midX)  showAtLeft=true;
        else                                    showAtLeft=false;
        break;
    }

    int winWidth = width();
    int winHeight = height();

    int moveX, moveY;
    if(alignReverse)    showAtLeft = !showAtLeft;
    if(showAtLeft)  moveX = rectCard.left()-winWidth;
    else            moveX = rectCard.right();

    moveY = midY-winHeight/2;
    if((maxTop!=-1) && (moveY<maxTop)) moveY=maxTop;
    else if((maxBottom!=-1) && ((moveY+winHeight)>maxBottom))
    {
        if((maxBottom-winHeight)<maxTop)    moveY=maxTop;
        else                            moveY=maxBottom-winHeight;
    }

    move(moveX, moveY);
    cardLabel->setPixmap(QPixmap(Utility::hscardsPath() + "/" + code + ".png").copy(5,34,WCARD,HCARD)
                         .scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    show();
}
