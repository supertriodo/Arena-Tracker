#include "bombwindow.h"
#include <QtWidgets>

BombWindow::BombWindow(QWidget *parent, QGraphicsScene *graphicsScene) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    graphicsView = new PlanGraphicsView(this, graphicsScene, false);
    setCentralWidget(graphicsView);
    setMinimumSize(0,0);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff );
}


BombWindow::~BombWindow()
{
    delete graphicsView;
}


void BombWindow::setTheme(Theme value)
{
    if(value == ThemeBlack)
    {
        graphicsView->setStyleSheet("QGraphicsView{background-color: black;}");
    }
    else
    {
        graphicsView->setStyleSheet("QGraphicsView{background-color: #F0F0F0;}");
    }
}


void BombWindow::showAt(QRect rectTab)
{
    int height = rectTab.height();
    QRectF boardRect = graphicsView->getBoardRect();
    int width = boardRect.width() * height / boardRect.height();

    QPoint center = rectTab.center();
    QPoint topLeft = rectTab.topRight();
    if(!Utility::isLeftOfScreen(center))    topLeft.setX(rectTab.left() - width);

    resize(width, height);
    move(topLeft);
    show();
}
