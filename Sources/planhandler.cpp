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
    qDebug()<<"NEW MINION --> id"<<id<<"pos"<<pos;

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, this->playerTurn);
    addMinion(friendly, minion, pos);
    emit checkCardImage(code);
}


void PlanHandler::addMinion(bool friendly, MinionGraphicsItem* minion, int pos)
{
    this->lastMinionAdded = minion;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);

    updateZoneSpots(friendly);
    ui->planGraphicsView->scene()->addItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));
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
    qDebug()<<"REMOVE MINION --> id"<<id;

    MinionGraphicsItem* minion = takeMinion(friendly, id);
    if(minion == NULL)  emit pDebug("Remove minion not found. Id: " + QString::number(id));
    else                delete minion;
}


MinionGraphicsItem * PlanHandler::takeMinion(bool friendly, int id)
{
    this->lastMinionAdded = NULL;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)   return NULL;

    MinionGraphicsItem* minion = minionsList->takeAt(pos);
    updateZoneSpots(friendly);
    ui->planGraphicsView->scene()->removeItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));

    return minion;
}


void PlanHandler::playerMinionZonePlaySteal(int id, int pos)
{
    stealMinion(true, id, pos);
}


void PlanHandler::enemyMinionZonePlaySteal(int id, int pos)
{
    stealMinion(false, id, pos);
}


void PlanHandler::stealMinion(bool friendly, int id, int pos)
{
    qDebug()<<"STEAL MINION --> id"<<id<<"pos"<<pos;

    MinionGraphicsItem* minion = takeMinion(friendly, id);
    if(minion == NULL)  emit pDebug("Steal minion not found. Id: " + QString::number(id));
    else
    {
        addMinion(!friendly, minion, pos);
        minion->changeZone(this->playerTurn);
    }
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
        qDebug()<<"Solo se puede cambiar pos del ultimo minion added."<<id<<"-->"<<pos;
    }
    else
    {
        if(oldPos == pos)
        {
            qDebug()<<"POSITION --> Minion already in the pos"<<id<<"-->"<<pos;
        }
        else if(pos >= minionsList->count())
        {
            emit pDebug("Update pos minion ouside list bounds. Pos: " + QString::number(pos) +
                        " Count: " + QString::number(minionsList->count()), Error);
        }
        else
        {
            qDebug()<<"POSITION --> "<<id<<"-->"<<pos;
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
//    qDebug()<<"AddTagChange"<<id<<tag<<value<<pendingTagChanges.count();
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    MinionGraphicsItem * minion = findMinion(tagChange.friendly, tagChange.id);
    if(minion != NULL)
    {
        emit pDebug("Tag Change: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        minion->processTagChange(tagChange.tag, tagChange.value);
    }
    else
    {
        emit pDebug("Append Tag Change: Id: " + QString::number(id) + " - " + tag + " --> " + value +
                    " - " + QString::number(pendingTagChanges.count()));
        pendingTagChanges.append(tagChange);
        QTimer::singleShot(500, this, SLOT(checkPendingTagChanges()));
    }
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


void PlanHandler::redrawDownloadedCardImage(QString code)
{
    foreach(MinionGraphicsItem * minion, playerMinions)
    {
        if(minion->getCode() == code)   minion->update();
    }
    foreach(MinionGraphicsItem * minion, enemyMinions)
    {
        if(minion->getCode() == code)   minion->update();
    }
}


void PlanHandler::reset()
{
    while(!playerMinions.empty())
    {
        MinionGraphicsItem* minion = playerMinions.takeFirst();
        ui->planGraphicsView->scene()->removeItem(minion);
        delete minion;
    }

    while(!enemyMinions.empty())
    {
        MinionGraphicsItem* minion = enemyMinions.takeFirst();
        ui->planGraphicsView->scene()->removeItem(minion);
        delete minion;
    }

    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));
    pendingTagChanges.clear();
    this->lastMinionAdded = NULL;
}


void PlanHandler::lockPlanInterface()
{
    this->inGame = true;
    updateTransparency();

    reset();
}


void PlanHandler::unlockPlanInterface()
{
    this->inGame = false;
    updateTransparency();
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
