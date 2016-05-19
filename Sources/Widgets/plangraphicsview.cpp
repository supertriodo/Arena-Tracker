#include "plangraphicsview.h"
#include "herographicsitem.h"
#include <QDebug>

PlanGraphicsView::PlanGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setStyleSheet("QGraphicsView{background-color: transparent;}");

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setScene(graphicsScene);
    updateView(0);
}


void PlanGraphicsView::updateView(int minionsZone)
{
    const int wMinion = MinionGraphicsItem::WIDTH-5;//142
    const int hMinion = MinionGraphicsItem::HEIGHT-5;//184
    const int wHero = HeroGraphicsItem::WIDTH;//160
    const int hHero = HeroGraphicsItem::HEIGHT;//184

    this->scene()->setSceneRect(std::min(-wHero/2, -wMinion/2*minionsZone), -hMinion-hHero,
                                std::max(wHero, wMinion*minionsZone), (hMinion+hHero)*2);
    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}


void PlanGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}
