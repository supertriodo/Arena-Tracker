#include "plangraphicsview.h"
#include "herographicsitem.h"
#include <QtWidgets>

PlanGraphicsView::PlanGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setStyleSheet("QGraphicsView{background-color: transparent;}");

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    this->setScene(graphicsScene);
    this->reset();
    this->updateView(0);
}


void PlanGraphicsView::reset()
{
    this->zoom = this->targetZoom = 0;
    removeAll();
}


void PlanGraphicsView::removeAll()
{
    foreach(QGraphicsItem *item, this->scene()->items())
    {
        this->scene()->removeItem(item);
    }
}


void PlanGraphicsView::updateView(int minionsZone)
{
    const int wMinion = MinionGraphicsItem::WIDTH-5;//142
    const int hMinion = MinionGraphicsItem::HEIGHT-5;//184
    const int wHero = HeroGraphicsItem::WIDTH;//160
    const int hHero = HeroGraphicsItem::HEIGHT;//184

    this->scene()->setSceneRect(std::min(-wHero/2, -wMinion/2*minionsZone), -hMinion-hHero,
                                std::max(wHero, wMinion*minionsZone), (hMinion+hHero)*2);
    fitInViewSmooth();
}


void PlanGraphicsView::fitInViewSmooth()
{
    bool zooming = (targetZoom != zoom);
    QRectF boardRect = this->scene()->sceneRect();
    float zoomWidth = this->width()/boardRect.width();
    float zoomHeight = this->height()/boardRect.height();
    targetZoom = std::min(zoomWidth, zoomHeight);
    if(!zooming)    progressiveZoom();
}


void PlanGraphicsView::progressiveZoom()
{
    const float zoomDiff = targetZoom - zoom;
    float advanceZoom = (zoomDiff>0)?ZOOM_SPEED:-ZOOM_SPEED;
    advanceZoom += zoomDiff/25;
    zoom += advanceZoom;
    if(std::fabs(targetZoom - zoom) < ZOOM_SPEED)    zoom = targetZoom;
    else                QTimer::singleShot(20, this, SLOT(progressiveZoom()));

    QMatrix mtx;
    mtx.scale(zoom, zoom);
    this->setMatrix(mtx);
}


void PlanGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    QRectF boardRect = this->scene()->sceneRect();
    float zoomWidth = this->width()/boardRect.width();
    float zoomHeight = this->height()/boardRect.height();
    zoom = targetZoom = std::min(zoomWidth, zoomHeight);

    QMatrix mtx;
    mtx.scale(zoom, zoom);
    this->setMatrix(mtx);
}
