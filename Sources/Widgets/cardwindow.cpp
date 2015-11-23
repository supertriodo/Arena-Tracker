#include "cardwindow.h"
#include "../utility.h"
#include <QtWidgets>

CardWindow::CardWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    cardLabel = new QLabel(this);
    setCentralWidget(cardLabel);
    move(700, 400);
    resize(WCARD,HCARD);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


CardWindow::~CardWindow()
{
}


void CardWindow::LoadCard(QString code, QRect rectCard, int maxTop, int maxBottom)
{
    if(code.isEmpty() ||
        !QFileInfo(Utility::appPath() + "/HSCards/" + code + ".png").exists())
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

    int moveX, moveY;
    if(showAtLeft)  moveX = rectCard.left()-WCARD;
    else            moveX = rectCard.right();

    moveY = midY-HCARD/2;
    if((maxTop!=-1) && (moveY<maxTop)) moveY=maxTop;
    else if((maxBottom!=-1) && ((moveY+HCARD)>maxBottom))
    {
        if((maxBottom-HCARD)<maxTop)    moveY=maxTop;
        else                            moveY=maxBottom-HCARD;
    }

    move(moveX, moveY);
    cardLabel->setPixmap(QPixmap(Utility::appPath() + "/HSCards/" + code + ".png").copy(5,34,WCARD,HCARD));
    show();
}
