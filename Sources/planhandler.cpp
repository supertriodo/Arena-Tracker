#include "planhandler.h"
#include <QtWidgets>


PlanHandler::PlanHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->inGame = false;
    this->mouseInApp = false;
    this->lastMinionAdded = NULL;
    this->playerHero = NULL;
    this->enemyHero = NULL;

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
    emit checkCardImage(code, false);
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
    if(minion == NULL)  emit pDebug("Remove minion not found. Id: " + QString::number(id), Warning);
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


void PlanHandler::playerHeroZonePlayAdd(QString code, int id)
{
    addHero(true, code, id);
}


void PlanHandler::enemyHeroZonePlayAdd(QString code, int id)
{
    addHero(false, code, id);
}


void PlanHandler::addHero(bool friendly, QString code, int id)
{
    qDebug()<<"NEW HERO --> id"<<id;
    HeroGraphicsItem* hero = friendly?playerHero:enemyHero;

    if(hero != NULL)
    {
        emit pDebug("Trying to add a hero with an existing one. Force remove old one.", Warning);
        removeHero(friendly, hero->getId());
    }

    hero = new HeroGraphicsItem(code, id, friendly, this->playerTurn);
    ui->planGraphicsView->scene()->addItem(hero);

    if(friendly)    playerHero = hero;
    else            enemyHero = hero;

    emit checkCardImage(code, true);
}


void PlanHandler::removeHero(bool friendly, int id)
{
    qDebug()<<"REMOVE HERO --> id"<<id;

    HeroGraphicsItem* hero = friendly?playerHero:enemyHero;
    if(hero == NULL || hero->getId() != id)
    {
        emit pDebug("Remove hero not found. Id: " + QString::number(id), Warning);
    }
    else
    {
        ui->planGraphicsView->scene()->removeItem(hero);
        delete hero;

        if(friendly)    playerHero = NULL;
        else            enemyHero = NULL;
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
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    MinionGraphicsItem * minion = findMinion(tagChange.friendly, tagChange.id);
    if(minion != NULL)
    {
        emit pDebug("Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        minion->processTagChange(tagChange.tag, tagChange.value);
    }
    else if(playerHero->getId() == tagChange.id)
    {
        emit pDebug("Tag Change Player Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        playerHero->processTagChange(tagChange.tag, tagChange.value);
    }
    else if(enemyHero->getId() == tagChange.id)
    {
        emit pDebug("Tag Change Enemy Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        enemyHero->processTagChange(tagChange.tag, tagChange.value);
    }
    else
    {
        QTimer::singleShot(1000, this, SLOT(checkPendingTagChanges()));
        pendingTagChanges.append(tagChange);
        emit pDebug("Append Tag Change: Id: " + QString::number(id) + " - " + tag + " --> " + value +
                    " - " + QString::number(pendingTagChanges.count()));
    }
}


void PlanHandler::checkPendingTagChanges()
{
    qDebug()<<"CHECK TAG LIST -->"<<pendingTagChanges.count();

    if(pendingTagChanges.isEmpty()) return;

    TagChange tagChange = pendingTagChanges.takeFirst();
    MinionGraphicsItem * minion = findMinion(tagChange.friendly, tagChange.id);
    if(minion != NULL)                              minion->processTagChange(tagChange.tag, tagChange.value);
    else if(playerHero->getId() == tagChange.id)    playerHero->processTagChange(tagChange.tag, tagChange.value);
    else if(enemyHero->getId() == tagChange.id)     enemyHero->processTagChange(tagChange.tag, tagChange.value);
}


void PlanHandler::newTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    foreach(MinionGraphicsItem * minion, playerMinions)
    {
        minion->setPlayerTurn(playerTurn);
    }

    if(playerHero != NULL)      playerHero->setPlayerTurn(playerTurn);
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

    if(playerHero != NULL && playerHero->getCode() == code)   playerHero->update();
    if(enemyHero != NULL && enemyHero->getCode() == code)     enemyHero->update();
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

    if(playerHero != NULL)  removeHero(true, playerHero->getId());
    if(enemyHero != NULL)   removeHero(false, enemyHero->getId());

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
