#include "planhandler.h"
#include <QtWidgets>


PlanHandler::PlanHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->inGame = false;
    this->mouseInApp = false;
    this->lastMinionAdded = NULL;

    completeUI();
}


PlanHandler::~PlanHandler()
{

}


void PlanHandler::completeUI()
{
}


void PlanHandler::playerMinionZonePlayAdd(QString code, int id, int pos)
{
    addMinion(true, code, id, pos-1);
}


void PlanHandler::enemyMinionZonePlayAdd(QString code, int id, int pos)
{
    addMinion(false, code, id, pos-1);
}


void PlanHandler::addMinion(bool friendly, QString code, int id, int pos)
{
    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id);
    this->lastMinionAdded = minion;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);

    updateZonePos(friendly);
    ui->planGraphicsView->scene()->addItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));

    qDebug()<<"New minion pos"<<pos;
}


void PlanHandler::playerMinionZonePlayRemove(int id)
{
    removeMinion(true, id);
}


void PlanHandler::enemyMinionZonePlayRemove(int id)
{
    removeMinion(false, id);
}


void PlanHandler::removeMinion(bool friendly, int id)
{
    this->lastMinionAdded = NULL;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)
    {
        emit pDebug("Remove minion not found. Id: " + QString::number(id));
        return;
    }

    MinionGraphicsItem* minion = minionsList->takeAt(pos);
    updateZonePos(friendly);
    ui->planGraphicsView->scene()->removeItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));
    delete minion;
}


void PlanHandler::updateZonePos(bool friendly)
{
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    for(int i=0; i<minionsList->count(); i++)
    {
        minionsList->at(i)->setZonePos(friendly, i, minionsList->count());
    }
}


QList<MinionGraphicsItem *> * PlanHandler::getMinionList(bool friendly)
{
    if(friendly)    return &playerMinions;
    else            return &enemyMinions;
}


int PlanHandler::findMinionPos(QList<MinionGraphicsItem *> * minionsList, int id)
{
    for(int i=0; i<minionsList->count(); i++)
    {
        if(minionsList->at(i)->getId()==id) return i;
    }
    return -1;
}


void PlanHandler::playerMinionPosChange(int id, int pos)
{
    updateMinionPos(true, id, pos-1);
}


void PlanHandler::enemyMinionPosChange(int id, int pos)
{
    updateMinionPos(false, id, pos-1);
}


void PlanHandler::updateMinionPos(bool friendly, int id, int pos)
{
    if(this->lastMinionAdded == NULL) return;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int oldPos = findMinionPos(minionsList, id);

    if(oldPos == -1)
    {
        emit pDebug("Update pos minion not found. Id: " + QString::number(id), Error);
    }
    else if(minionsList->at(oldPos)!=this->lastMinionAdded)
    {
        qDebug()<<"Solo se puede cambiar pos del ultimo minion added.";
    }
    else
    {
        if(oldPos == pos)
        {
            qDebug()<<"Minion already in the pos"<<pos;
        }
        else if(pos >= minionsList->count())
        {
            emit pDebug("Update pos minion ouside list bounds. Pos: " + QString::number(pos) +
                        " Count: " + QString::number(minionsList->count()), Error);
        }
        else
        {
            qDebug()<<"Update minion pos"<<pos;
            minionsList->move(oldPos, pos);
            updateZonePos(friendly);
        }
    }
    this->lastMinionAdded = NULL;
}


void PlanHandler::updateTransparency()
{
    bool inTabPlan = ui->tabWidget->currentWidget() == ui->tabPlan;

    if(!mouseInApp && (transparency==Transparent || (inGame && transparency==AutoTransparent)))
    {
        ui->tabPlan->setAttribute(Qt::WA_NoBackground);
        ui->tabPlan->repaint();

        //Tambien nos hacemos cargo en transparency==Transparent para que se llame a MainWindowFade al empezar y terminar un juego
        if(inTabPlan && transparency!=Opaque)
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabPlan->setAttribute(Qt::WA_NoBackground, false);
        ui->tabPlan->repaint();

        if(inTabPlan && transparency==AutoTransparent)
        {
            emit needMainWindowFade(false);
        }
    }
}


void PlanHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void PlanHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}
