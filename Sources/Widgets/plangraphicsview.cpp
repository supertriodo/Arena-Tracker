#include "plangraphicsview.h"
#include <QDebug>

PlanGraphicsView::PlanGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setStyleSheet("QGraphicsView{background-color: transparent;}");

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    graphicsScene->setSceneRect(-200, -200, 400, 400);

    this->setScene(graphicsScene);
}


void PlanGraphicsView::updateView(int minionsZone)
{
    this->scene()->setSceneRect(-140/2*minionsZone, -180, 140*minionsZone, 185*2);
    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}


void PlanGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}
