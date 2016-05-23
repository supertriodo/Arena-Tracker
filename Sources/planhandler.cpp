#include "planhandler.h"
#include <QtWidgets>


PlanHandler::PlanHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->inGame = false;
    this->mouseInApp = false;
    this->nowBoard = new Board();
    this->nowBoard->playerHero = NULL;
    this->nowBoard->enemyHero = NULL;
    this->nowBoard->numTurn = 0;
    reset();
    completeUI();
}


PlanHandler::~PlanHandler()
{
    delete nowBoard;
}


void PlanHandler::completeUI()
{
    connect(ui->planButtonPrev, SIGNAL(clicked()),
            this, SLOT(showPrevTurn()));
    connect(ui->planButtonNext, SIGNAL(clicked()),
            this, SLOT(showNextTurn()));
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

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn);
    addMinion(friendly, minion, pos);
    emit checkCardImage(code, false);
}


void PlanHandler::addMinion(bool friendly, MinionGraphicsItem* minion, int pos)
{
    this->lastMinionAdded = minion;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);
    updateZoneSpots(friendly);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->addItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
    }
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

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->removeItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
    }

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
        minion->changeZone(nowBoard->playerTurn);
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
    HeroGraphicsItem* hero = friendly?nowBoard->playerHero:nowBoard->enemyHero;

    if(hero != NULL)
    {
        emit pDebug("Trying to add a hero with an existing one. Force remove old one.", Warning);
        if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->removeItem(hero);
        removeHero(nowBoard, friendly);
    }

    hero = new HeroGraphicsItem(code, id, friendly, nowBoard->playerTurn);
    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->addItem(hero);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
    }

    if(friendly)    nowBoard->playerHero = hero;
    else            nowBoard->enemyHero = hero;

    emit checkCardImage(code, true);
}


void PlanHandler::removeHero(Board *board, bool friendly)
{
    HeroGraphicsItem* hero = friendly?board->playerHero:board->enemyHero;
    if(hero == NULL)
    {
        emit pDebug("Remove hero NULL.", Warning);
    }
    else
    {
        delete hero;

        if(friendly)    board->playerHero = NULL;
        else            board->enemyHero = NULL;
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
    if(friendly)    return &nowBoard->playerMinions;
    else            return &nowBoard->enemyMinions;
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
    else if(nowBoard->playerHero->getId() == tagChange.id)
    {
        emit pDebug("Tag Change Player Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->playerHero->processTagChange(tagChange.tag, tagChange.value);
    }
    else if(nowBoard->enemyHero->getId() == tagChange.id)
    {
        emit pDebug("Tag Change Enemy Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->enemyHero->processTagChange(tagChange.tag, tagChange.value);
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
    else if(nowBoard->playerHero->getId() == tagChange.id)    nowBoard->playerHero->processTagChange(tagChange.tag, tagChange.value);
    else if(nowBoard->enemyHero->getId() == tagChange.id)     nowBoard->enemyHero->processTagChange(tagChange.tag, tagChange.value);
}


void PlanHandler::newTurn(bool playerTurn, int numTurn)
{
    //Update nowBoard
    nowBoard->playerTurn = playerTurn;

    foreach(MinionGraphicsItem * minion, nowBoard->playerMinions)
    {
        minion->setPlayerTurn(playerTurn);
    }

    if(nowBoard->playerHero != NULL)      nowBoard->playerHero->setPlayerTurn(playerTurn);


    //Store nowBoard
    if(this->firstStoredTurn == 0)
    {
        this->firstStoredTurn = numTurn;
        updateButtons();
    }

    Board *board = new Board();
    board->playerTurn = playerTurn;
    board->numTurn = numTurn;

    if(nowBoard->playerHero == NULL)    board->playerHero = NULL;
    else                                board->playerHero = new HeroGraphicsItem(nowBoard->playerHero);
    if(nowBoard->enemyHero == NULL)     board->enemyHero = NULL;
    else                                board->enemyHero = new HeroGraphicsItem(nowBoard->enemyHero);

    foreach(MinionGraphicsItem * minion, nowBoard->playerMinions)
    {
        board->playerMinions.append(new MinionGraphicsItem(minion));
    }

    foreach(MinionGraphicsItem * minion, nowBoard->enemyMinions)
    {
        board->enemyMinions.append(new MinionGraphicsItem(minion));
    }

    turnBoards.append(board);
}


void PlanHandler::redrawDownloadedCardImage(QString code)
{
    foreach(MinionGraphicsItem * minion, nowBoard->playerMinions)
    {
        if(minion->getCode() == code)   minion->update();
    }
    foreach(MinionGraphicsItem * minion, nowBoard->enemyMinions)
    {
        if(minion->getCode() == code)   minion->update();
    }

    if(nowBoard->playerHero != NULL && nowBoard->playerHero->getCode() == code)   nowBoard->playerHero->update();
    if(nowBoard->enemyHero != NULL && nowBoard->enemyHero->getCode() == code)     nowBoard->enemyHero->update();
}


void PlanHandler::resetBoard(Board *board)
{
    while(!board->playerMinions.empty())
    {
        MinionGraphicsItem* minion = board->playerMinions.takeFirst();
        delete minion;
    }

    while(!board->enemyMinions.empty())
    {
        MinionGraphicsItem* minion = board->enemyMinions.takeFirst();
        delete minion;
    }

    if(board->playerHero != NULL)  removeHero(board, true);
    if(board->enemyHero != NULL)   removeHero(board, false);
}


void PlanHandler::reset()
{
    emit pDebug("Clear all boards.");
    ui->planGraphicsView->reset();
    pendingTagChanges.clear();
    this->lastMinionAdded = NULL;
    this->viewBoard = nowBoard;
    this->firstStoredTurn = 0;
    updateButtons();

    resetBoard(nowBoard);
    while(!turnBoards.empty())
    {
        Board *board = turnBoards.takeFirst();
        resetBoard(board);
        delete board;
    }
}


void PlanHandler::updateButtons()
{
    int viewTurn = viewBoard->numTurn;
    bool nextEnabled = viewTurn != 0;
    bool prevEnabled = (firstStoredTurn!=0 && viewTurn==0) || viewTurn>firstStoredTurn;
    ui->planButtonNext->setEnabled(nextEnabled);
    ui->planButtonPrev->setEnabled(prevEnabled);
}


void PlanHandler::showNextTurn()
{
    int viewTurn = viewBoard->numTurn;
    if(viewTurn == 0)
    {
        emit pDebug("Moving to next turn when in nowBoard. Next Turn button should be disabled.", Error);
        updateButtons();
        return;
    }
    int countTurns = turnBoards.count();
    int lastTurn = firstStoredTurn + countTurns - 1;
    if(viewTurn < lastTurn) viewBoard = turnBoards[viewTurn+1-firstStoredTurn];
    else                    viewBoard = nowBoard;

    loadViewBoard();
    updateButtons();
    qDebug()<<nowBoard->numTurn<<countTurns<<firstStoredTurn<<lastTurn<<endl
           <<viewTurn<<viewBoard->numTurn;
}


void PlanHandler::showPrevTurn()
{
    int viewTurn = viewBoard->numTurn;
    if(viewTurn == firstStoredTurn)
    {
        emit pDebug("Moving to prev turn with no prev turn. Prev Turn button should be disabled.", Error);
        updateButtons();
        return;
    }
    int countTurns = turnBoards.count();
    int lastTurn = firstStoredTurn + countTurns - 1;
    if(viewTurn == 0)       viewBoard = turnBoards[lastTurn-firstStoredTurn];
    else                    viewBoard = turnBoards[viewTurn-1-firstStoredTurn];

    loadViewBoard();
    updateButtons();
    qDebug()<<nowBoard->numTurn<<countTurns<<firstStoredTurn<<lastTurn<<endl
           <<viewTurn<<viewBoard->numTurn;
}


void PlanHandler::loadViewBoard()
{
    ui->planGraphicsView->removeAll();

    ui->planGraphicsView->scene()->addItem(viewBoard->playerHero);
    ui->planGraphicsView->scene()->addItem(viewBoard->enemyHero);

    foreach(MinionGraphicsItem *minion, viewBoard->playerMinions)
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    foreach(MinionGraphicsItem *minion, viewBoard->enemyMinions)
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    ui->planGraphicsView->updateView(std::max(viewBoard->playerMinions.count(), viewBoard->enemyMinions.count()));
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
