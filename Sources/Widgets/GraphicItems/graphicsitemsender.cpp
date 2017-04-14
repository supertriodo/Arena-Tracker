#include "graphicsitemsender.h"
#include "cardgraphicsitem.h"


GraphicsItemSender::GraphicsItemSender(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->lastCode = "";
    this->lastId = -1;
}


void GraphicsItemSender::sendPlanCardEntered(QString code, QPoint rectCardTopLeft, QPoint rectCardBottomRight)
{
    if(code == lastCode)    return;
    lastCode = code;

    int maxTop, maxBottom;
    QRect rect = getRectCard(rectCardTopLeft, rectCardBottomRight, maxTop, maxBottom);
    emit cardEntered(code, rect, maxTop, maxBottom);
}


void GraphicsItemSender::sendPlanSecretEntered(int id, QPoint rectCardTopLeft, QPoint rectCardBottomRight)
{
    if(id == lastId)    return;
    lastId = id;

    int maxTop, maxBottom;
    QRect rect = getRectCard(rectCardTopLeft, rectCardBottomRight, maxTop, maxBottom);
    emit secretEntered(id, rect, maxTop, maxBottom);
}


QRect GraphicsItemSender::getRectCard(QPoint rectCardTopLeft, QPoint rectCardBottomRight, int &maxTop, int &maxBottom)
{
    rectCardTopLeft = ui->planGraphicsView->mapFromScene(rectCardTopLeft);
    rectCardTopLeft = ui->planGraphicsView->mapToGlobal(rectCardTopLeft);
    rectCardBottomRight = ui->planGraphicsView->mapFromScene(rectCardBottomRight);
    rectCardBottomRight = ui->planGraphicsView->mapToGlobal(rectCardBottomRight);

    QPoint planViewTopLeft = ui->planGraphicsView->mapToGlobal(QPoint(0,0));
    int planViewBottom = ui->planGraphicsView->mapToGlobal(QPoint(0,ui->planGraphicsView->height())).y();

    QRect rect(planViewTopLeft.x(), rectCardTopLeft.y(), ui->planGraphicsView->width(), rectCardBottomRight.y()-rectCardTopLeft.y());
    maxTop = planViewTopLeft.y();
    maxBottom = planViewBottom;
    return rect;
}


void GraphicsItemSender::sendPlanCardLeave()
{
    lastCode = "";
    lastId = -1;
    emit cardLeave();
}


void GraphicsItemSender::sendResetDeadProbs()
{
    emit resetDeadProbs();
}


void GraphicsItemSender::sendCheckBomb(QString code)
{
    emit checkBomb(code);
}


void GraphicsItemSender::sendHeroTotalAttackChange(bool friendly, int totalAttack, int totalMaxAttack)
{
    emit heroTotalAttackChange(friendly, totalAttack, totalMaxAttack);
}
