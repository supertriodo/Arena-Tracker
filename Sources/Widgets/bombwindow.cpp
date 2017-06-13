#include "bombwindow.h"
#include "../themehandler.h"
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


void BombWindow::setTheme()
{
    graphicsView->setStyleSheet("QGraphicsView{" + ThemeHandler::bgApp() + ";}");
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
