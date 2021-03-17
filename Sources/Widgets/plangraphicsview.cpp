#include "plangraphicsview.h"
#include "GraphicItems/herographicsitem.h"
#include "GraphicItems/cardgraphicsitem.h"
#include "../themehandler.h"
#include <cmath>
#include <QtWidgets>

PlanGraphicsView::PlanGraphicsView(QWidget *parent, QGraphicsScene *graphicsScene, bool showCards) : QGraphicsView(parent)
{
    this->targetZoom = this->zoom = 0;
    this->showCards = showCards;

    if(graphicsScene == nullptr)
    {
        graphicsScene = new QGraphicsScene(this);
        graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    }

    this->setScene(graphicsScene);
    this->reset();
    if(showCards)   this->updateView(0);
}


void PlanGraphicsView::setTheme(bool standAlone)
{
    this->setStyleSheet("QGraphicsView{" + (standAlone?ThemeHandler::bgApp():ThemeHandler::bgWidgets()) + "}");
}


int PlanGraphicsView::getSceneViewWidth()
{
    return static_cast<int>(width()/targetZoom);
}


int PlanGraphicsView::getCardsViewHeight()
{
    const int hMinion = MinionGraphicsItem::HEIGHT-5;//184
    const int hHero = HeroGraphicsItem::HEIGHT;//184
    return static_cast<int>((height()/targetZoom - hMinion*2 -hHero*2)/2);
}


void PlanGraphicsView::reset()
{
    removeAll();
}


void PlanGraphicsView::removeAll()
{
    if(this->scene() == nullptr)   return;
    for(QGraphicsItem *item: (const QList<QGraphicsItem *>)this->scene()->items())
    {
        this->scene()->removeItem(item);
    }
}


void PlanGraphicsView::updateView(int minionsZone)
{
    const int wMinion = MinionGraphicsItem::WIDTH-5;//142
    const int hMinion = MinionGraphicsItem::HEIGHT-5;//184
    const int wHero = HeroGraphicsItem::WIDTH+290;//160
    const int hHero = HeroGraphicsItem::HEIGHT;//184
    const int hCards = 155;

    this->scene()->setSceneRect(std::min(-wHero/2, -wMinion/2*minionsZone), -hMinion-hHero-hCards,
                                std::max(wHero, wMinion*minionsZone), (hMinion+hHero+hCards)*2);
    fitInViewSmooth();
}


void PlanGraphicsView::fitInViewSmooth()
{
    bool zooming = !FLOATEQ(targetZoom, zoom);
    QRectF boardRect = this->scene()->sceneRect();
    float zoomWidth = static_cast<float>(this->width()/boardRect.width());
    float zoomHeight = static_cast<float>(this->height()/boardRect.height());
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
    mtx.scale(static_cast<double>(zoom), static_cast<double>(zoom));
    this->setMatrix(mtx);
}


void PlanGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    QRectF boardRect = getBoardRect();
    float zoomWidth = static_cast<float>(this->width()/boardRect.width());
    float zoomHeight = static_cast<float>(this->height()/boardRect.height());
    zoom = targetZoom = std::min(zoomWidth, zoomHeight);

    QMatrix mtx;
    mtx.scale(static_cast<double>(zoom), static_cast<double>(zoom));
    this->setMatrix(mtx);

    emit sizeChanged();
}


QRectF PlanGraphicsView::getBoardRect()
{
    if(showCards)   return this->scene()->sceneRect();
    else
    {
        const int hCards = 155;
        QRectF boardRect(this->scene()->sceneRect());
        boardRect.setTop(boardRect.top() + hCards);
        boardRect.setBottom(boardRect.bottom() - hCards);
        this->setSceneRect(boardRect);
        return boardRect;
    }
}

//void PlanGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    QGraphicsView::mouseDoubleClickEvent(event);
//    event->ignore();
//}
//void PlanGraphicsView::mouseMoveEvent(QMouseEvent *event)
//{
//    QGraphicsView::mouseMoveEvent(event);
//    event->ignore();
//}
//void PlanGraphicsView::mousePressEvent(QMouseEvent *event)
//{
//    QGraphicsView::mousePressEvent(event);
//    event->ignore();
//}
//void PlanGraphicsView::mouseReleaseEvent(QMouseEvent *event)
//{
//    QGraphicsView::mouseReleaseEvent(event);
//    event->ignore();
//}


void PlanGraphicsView::leaveEvent(QEvent * e)
{
    QGraphicsView::leaveEvent(e);
    emit leave();
}
