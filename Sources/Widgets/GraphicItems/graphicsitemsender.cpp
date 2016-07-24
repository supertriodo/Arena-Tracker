#include "graphicsitemsender.h"

GraphicsItemSender::GraphicsItemSender(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
}


void GraphicsItemSender::sendPlanCardEntered(QString code, QPoint rectCardTopLeft, QPoint rectCardBottomRight)
{
    rectCardTopLeft = ui->planGraphicsView->mapFromScene(rectCardTopLeft);
    rectCardTopLeft = ui->planGraphicsView->mapToGlobal(rectCardTopLeft);
    rectCardBottomRight = ui->planGraphicsView->mapFromScene(rectCardBottomRight);
    rectCardBottomRight = ui->planGraphicsView->mapToGlobal(rectCardBottomRight);

    QPoint planViewTopLeft = ui->planGraphicsView->mapToGlobal(QPoint(0,0));
    int planViewBottom = ui->planGraphicsView->mapToGlobal(QPoint(0,ui->planGraphicsView->height())).y();

    QRect rect(planViewTopLeft.x(), rectCardTopLeft.y(), ui->planGraphicsView->width(), rectCardBottomRight.y()-rectCardTopLeft.y());
    emit cardEntered(code, rect, planViewTopLeft.y(), planViewBottom);
}
