#include "plangraphicsview.h"
#include "herographicsitem.h"
#include "cardgraphicsitem.h"
#include <cmath>
#include <QtWidgets>

PlanGraphicsView::PlanGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->targetZoom = this->zoom = 0;
    this->setStyleSheet("QGraphicsView{background-color: transparent;}");

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    this->setScene(graphicsScene);
    this->reset();
    this->updateView(0);
}


int PlanGraphicsView::getSceneViewWidth()
{
    return width()/targetZoom;
}


int PlanGraphicsView::getCardsViewHeight()
{
    const int hMinion = MinionGraphicsItem::HEIGHT-5;//184
    const int hHero = HeroGraphicsItem::HEIGHT;//184
    return (height()/targetZoom - hMinion*2 -hHero*2)/2;
}


void PlanGraphicsView::reset()
{
    removeAll();
}


void PlanGraphicsView::removeAll()
{
    if(this->scene() == NULL)   return;
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
    const int hCards = 155;

    this->scene()->setSceneRect(std::min(-wHero/2, -wMinion/2*minionsZone), -hMinion-hHero-hCards,
                                std::max(wHero, wMinion*minionsZone), (hMinion+hHero+hCards)*2);
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
    if(std::abs(targetZoom - zoom) < ZOOM_SPEED)    zoom = targetZoom;
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

    emit sizeChanged();
}


void PlanGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
    event->ignore();
}
void PlanGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    event->ignore();
}
void PlanGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    event->ignore();
}
void PlanGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    event->ignore();
}
