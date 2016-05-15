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
    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, this->playerTurn);
    this->lastMinionAdded = minion;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);

    updateZoneSpots(friendly);
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
    //this->lastMinionAdded = NULL;//Puede causar problemas si se crea esbirro y se elimina esbirro a la vez y el orden es crea/elimina/cambia pos creado (shadow madness)

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)
    {
        emit pDebug("Remove minion not found. Id: " + QString::number(id));
        return;
    }

    MinionGraphicsItem* minion = minionsList->takeAt(pos);
    updateZoneSpots(friendly);
    ui->planGraphicsView->scene()->removeItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));
    delete minion;
}


void PlanHandler::updateZoneSpots(bool friendly)
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


MinionGraphicsItem * PlanHandler::findMinion(bool friendly, int id)
{
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)   return NULL;
    else            return minionsList->at(pos);
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
            updateZoneSpots(friendly);
        }
    }
    this->lastMinionAdded = NULL;
}


void PlanHandler::playerMinionTagChange(int id, QString tag, QString value)
{
    addTagChange(id, true, tag, value);
}


void PlanHandler::enemyMinionTagChange(int id, QString tag, QString value)
{
    addTagChange(id, false, tag, value);
}


void PlanHandler::addTagChange(int id, bool friendly, QString tag, QString value)
{
    qDebug()<<"AddTagChange"<<id<<tag<<value<<pendingTagChanges.count();
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    pendingTagChanges.append(tagChange);
    QTimer::singleShot(500, this, SLOT(checkPendingTagChanges()));
}


void PlanHandler::checkPendingTagChanges()
{
    int lastMinionFoundIndex = -1;
    for(int i=0; i<pendingTagChanges.count(); i++)
    {
        TagChange tagChange = pendingTagChanges.at(i);
        MinionGraphicsItem * minion = findMinion(tagChange.friendly, tagChange.id);

        if(minion != NULL)
        {
            lastMinionFoundIndex = i;
            minion->processTagChange(tagChange.tag, tagChange.value);
        }
    }

    for(int i=0; i<=lastMinionFoundIndex; i++)  pendingTagChanges.removeFirst();
}


void PlanHandler::newTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    foreach(MinionGraphicsItem * minion, playerMinions)
    {
        minion->setPlayerTurn(playerTurn);
    }
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
