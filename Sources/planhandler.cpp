#include "planhandler.h"
#include "mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>


PlanHandler::PlanHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->inGame = false;
    this->mouseInApp = false;
    this->sizePlan = false;
    this->nowBoard = new Board();
    this->nowBoard->playerHero = NULL;
    this->nowBoard->enemyHero = NULL;
    this->nowBoard->numTurn = 0;
    this->viewBoard = NULL;
    this->futureBoard = NULL;
    this->futureBombs = NULL;
    this->selectedMinion = NULL;
    this->selectedCode = "";
    reset();
    completeUI();
    createGraphicsItemSender();
}


PlanHandler::~PlanHandler()
{
    while(!turnBoards.empty())
    {
        Board *board = turnBoards.takeFirst();
        if(viewBoard != board)  resetBoard(board);
        delete board;
    }

    if(viewBoard != nowBoard)  resetBoard(nowBoard);
    delete nowBoard;

    delete graphicsItemSender;

    if(futureBombs != NULL)
    {
        delete futureBombs;
        futureBombs = NULL;
    }
}


void PlanHandler::completeUI()
{
    QFont font("Belwe Bd BT");
    font.setPixelSize(24);
    font.setBold(true);
    font.setKerning(true);
    ui->planLabelTurn->setFont(font);
    QFontMetrics fm(font);
    int textWide = fm.width("T99");
    ui->planLabelTurn->setFixedWidth(textWide);

    connect(ui->planButtonPrev, SIGNAL(clicked()),
            this, SLOT(showPrevTurn()));
    connect(ui->planButtonNext, SIGNAL(clicked()),
            this, SLOT(showNextTurn()));
    connect(ui->planButtonFirst, SIGNAL(clicked()),
            this, SLOT(showFirstTurn()));
    connect(ui->planButtonLast, SIGNAL(clicked()),
            this, SLOT(showLastTurn()));
    connect(ui->planTurnSlider, SIGNAL(valueChanged(int)),
            this, SLOT(showSliderTurn(int)));
    connect(ui->planButtonResize, SIGNAL(clicked()),
            this, SLOT(resizePlan()));
    connect(ui->planGraphicsView, SIGNAL(sizeChanged()),
            this, SLOT(updateViewCardZoneSpots()));
}


void PlanHandler::createGraphicsItemSender()
{
    graphicsItemSender = new GraphicsItemSender(this, ui);
    connect(graphicsItemSender, SIGNAL(cardPress(CardGraphicsItem*,Qt::MouseButton)),
            this, SLOT(cardPress(CardGraphicsItem*,Qt::MouseButton)));
    connect(graphicsItemSender, SIGNAL(heroPowerPress(HeroPowerGraphicsItem*,Qt::MouseButton)),
            this, SLOT(heroPowerPress(HeroPowerGraphicsItem*,Qt::MouseButton)));
    connect(graphicsItemSender, SIGNAL(minionPress(MinionGraphicsItem*,Qt::MouseButton)),
            this, SLOT(minionPress(MinionGraphicsItem*,Qt::MouseButton)));
    connect(graphicsItemSender, SIGNAL(heroPress(HeroGraphicsItem*,Qt::MouseButton)),
            this, SLOT(heroPress(HeroGraphicsItem*,Qt::MouseButton)));
    connect(graphicsItemSender, SIGNAL(minionWheel(MinionGraphicsItem*,bool)),
            this, SLOT(minionWheel(MinionGraphicsItem*,bool)));
    connect(graphicsItemSender, SIGNAL(heroWheel(HeroGraphicsItem*,bool)),
            this, SLOT(heroWheel(HeroGraphicsItem*,bool)));
    connect(graphicsItemSender, SIGNAL(cardEntered(QString,QRect,int,int)),
            this, SLOT(showCardTooltip(QString,QRect,int,int)));
    connect(graphicsItemSender, SIGNAL(secretEntered(int,QRect,int,int)),
            this, SIGNAL(secretEntered(int,QRect,int,int)));
    connect(graphicsItemSender, SIGNAL(cardLeave()),
            this, SIGNAL(cardLeave()));
    connect(graphicsItemSender, SIGNAL(resetDeadProbs()),
            this, SLOT(resetDeadProbs()));
    connect(graphicsItemSender, SIGNAL(checkBomb(QString)),
            this, SLOT(checkBomb(QString)));
    connect(graphicsItemSender, SIGNAL(heroTotalAttackChange(bool,int,int)),
            this, SIGNAL(heroTotalAttackChange(bool,int,int)));
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

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);
    updateMinionFromCard(minion);
    addMinion(friendly, minion, pos);
    emit checkCardImage(code, false);
}


void PlanHandler::updateMinionFromCard(MinionGraphicsItem * minion)
{
    CardGraphicsItem * card = findCard(minion->isFriendly(), minion->getId());
    if(card == NULL)    emit pDebug("Minion not found in hand when ckecking its stats. Id: " + QString::number(minion->getId()), Warning);
    else
    {
        minion->updateStatsFromCard(card);
    }
}


void PlanHandler::addMinion(bool friendly, MinionGraphicsItem *minion, int pos)
{
    this->lastMinionAdded = minion;
    this->lastMinionAddedTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);
    updateMinionZoneSpots(friendly);
    updateMinionsAttack(friendly);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->addItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
        updateViewCardZoneSpots();
    }
}


void PlanHandler::updateMinionsAttack(bool friendly, Board *board)
{
    int minionsAttack = 0;
    int minionsMaxAttack = 0;
    foreach(MinionGraphicsItem *minion, *getMinionList(friendly, board))
    {
        minionsAttack += minion->getPotencialDamage(false);
        if(board == NULL)   minionsMaxAttack += minion->getPotencialDamage(true);
    }

    HeroGraphicsItem *hero = getHero(friendly, board);
    if(hero != NULL)
    {
        hero->setMinionsAttack(minionsAttack, board == NULL);
        if(board == NULL)   hero->setMinionsMaxAttack(minionsMaxAttack);
    }
}


void PlanHandler::playerMinionZonePlayAddTriggered(QString code, int id, int pos)
{
    addMinionTriggered(true, code, id, pos-1);
}


void PlanHandler::enemyMinionZonePlayAddTriggered(QString code, int id, int pos)
{
    addMinionTriggered(false, code, id, pos-1);
}


void PlanHandler::addMinionTriggered(bool friendly, QString code, int id, int pos)
{
    qDebug()<<"NEW MINION TRIGGERED--> id"<<id<<"pos"<<pos;

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);
    addMinion(friendly, minion, pos);
    if(this->lastTriggerId!=-1)         copyMinionToLastTurn(friendly, minion);
    else                                emit pDebug("Triggered minion creator not set.");
    emit checkCardImage(code, false);
}


void PlanHandler::copyMinionToLastTurn(bool friendly, MinionGraphicsItem *minion, int idCreator)
{
    if(turnBoards.empty())  return;

    MinionGraphicsItem *triggerMinion = NULL;
    if(idCreator == -1)     idCreator = this->lastTriggerId;
    emit pDebug("Triggered minion. Ids: " + QString::number(idCreator) + " --> " + QString::number(minion->getId()));

    Board *board = turnBoards.last();
    QList<MinionGraphicsItem *> *minionsList = getMinionList(friendly, board);
    int pos = findMinionPos(minionsList, idCreator);

    //El padre es amigo
    if(pos != -1)
    {
        MinionGraphicsItem *creatorMinion = minionsList->at(pos);
        triggerMinion = new MinionGraphicsItem(minion, true);
        minionsList->insert(pos+1, triggerMinion);
        addReinforceToLastTurn(creatorMinion, triggerMinion, board);
    }
    else
    {
        QList<MinionGraphicsItem *> *opMinionsList = getMinionList(!friendly, board);
        pos = findMinionPos(opMinionsList, idCreator);

        //El padre es enemigo
        if(pos != -1)
        {
            MinionGraphicsItem *creatorMinion = opMinionsList->at(pos);
            triggerMinion = new MinionGraphicsItem(minion, true);
            minionsList->append(triggerMinion);
            addReinforceToLastTurn(creatorMinion, triggerMinion, board);
        }
        else
        {
            HeroGraphicsItem *hero = getHero(friendly, board);

            //El padre es un secreto amigo
            if(hero!=NULL && hero->isYourSecret(idCreator))
            {
                triggerMinion = new MinionGraphicsItem(minion, true);
                minionsList->append(triggerMinion);
                addReinforceToLastTurn(hero, triggerMinion, board);
            }

            //El padre no esta en el board
            else
            {
                emit pDebug("Triggered minion creator is not on the board. Ids: " +
                            QString::number(idCreator) + " --> " + QString::number(minion->getId()));
            }
        }
    }

    //Update Board
    if(triggerMinion != NULL)
    {
        updateMinionZoneSpots(friendly, board);

        if(viewBoard == board)
        {
            ui->planGraphicsView->scene()->addItem(triggerMinion);
            ui->planGraphicsView->updateView(std::max(board->playerMinions.count(), board->enemyMinions.count()));
            updateViewCardZoneSpots();
        }
    }
}


void PlanHandler::addReinforceToLastTurn(MinionGraphicsItem *parent, MinionGraphicsItem *child, Board *board)
{
    ArrowGraphicsItem *reinforce = new ArrowGraphicsItem(ArrowGraphicsItem::reinforcement);
    reinforce->setEnd(true, parent);
    reinforce->setEnd(false, child);

    board->arrows.prepend(reinforce);
    if(viewBoard == board)  ui->planGraphicsView->scene()->addItem(reinforce);
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


MinionGraphicsItem * PlanHandler::takeMinion(bool friendly, int id, bool stolen)
{
    this->lastMinionAdded = NULL;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)   return NULL;

    MinionGraphicsItem* minion = minionsList->takeAt(pos);
    updateMinionZoneSpots(friendly);
    updateMinionsAttack(friendly);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->removeItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
        updateViewCardZoneSpots();
    }

    //Marcar como dead minion en ultimo turno
    if(!turnBoards.empty())
    {
        Board *board = turnBoards.last();
        minionsList = getMinionList(friendly, board);
        pos = findMinionPos(minionsList, id);
        if(pos != -1)
        {
            minionsList->at(pos)->setDead(true);
            if(stolen)  minionsList->at(pos)->setId(-id);//Impide addons sobre el, en lugar de su copia en la otra zone.
        }
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

    MinionGraphicsItem* minion = takeMinion(friendly, id, true);
    if(minion == NULL)  emit pDebug("Steal minion not found. Id: " + QString::number(id));
    else
    {
        addMinion(!friendly, minion, pos);
        minion->changeZone();

        //Engrave roba el esbirro en el log, y luego lo mata, lo evitamos. Usamos lastPowerAddon.code pq lastTrigger no guarda code.
        if(this->lastPowerAddon.code != ENGRAVE)
        {
            copyMinionToLastTurn(!friendly, minion, -id);
        }
        else
        {
            emit pDebug("Avoid steal minion by Engrave.");
        }
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
        hero->changeHero(code, id);
    }
    else
    {
        hero = new HeroGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);

        if(viewBoard == nowBoard)
        {
            ui->planGraphicsView->scene()->addItem(hero);
            ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
            updateViewCardZoneSpots();
        }

        if(friendly)    nowBoard->playerHero = hero;
        else            nowBoard->enemyHero = hero;
    }
    emit checkCardImage(code, true);
}


void PlanHandler::removeHero(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
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


void PlanHandler::updateMinionZoneSpots(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly, board);
    for(int i=0; i<minionsList->count(); i++)
    {
        minionsList->at(i)->setZonePos(friendly, i, minionsList->count());
    }

    if(viewBoard == board)
    {
        foreach(ArrowGraphicsItem * arrow, board->arrows)
        {
            arrow->prepareGeometryChange();
            arrow->update();
        }
    }
}


QList<MinionGraphicsItem *> * PlanHandler::getMinionList(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    if(friendly)    return &board->playerMinions;
    else            return &board->enemyMinions;
}


HeroGraphicsItem * PlanHandler::getHero(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    if(friendly)    return board->playerHero;
    else            return board->enemyHero;
}


int PlanHandler::findMinionPos(QList<MinionGraphicsItem *> * minionsList, int id)
{
    for(int i=0; i<minionsList->count(); i++)
    {
        if(minionsList->at(i)->getId()==id) return i;
    }
    return -1;
}


MinionGraphicsItem * PlanHandler::findMinion(bool friendly, int id, Board *board)
{
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly, board);
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
    if(!isLastMinionAddedValid()) return;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int oldPos = findMinionPos(minionsList, id);

    if(oldPos == -1)
    {
        emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos) +
                    ". Id not found.", Warning);
    }
    else if(minionsList->at(oldPos)!=this->lastMinionAdded)
    {
        emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos) +
                    ". Not last minion added.");
    }
    else
    {
        if(oldPos == pos)
        {
            emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos) +
                        ". Minion already in place.");
        }
        else if(pos >= minionsList->count())
        {
            emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos) +
                        ". Minion ouside list bounds. Count: " + QString::number(minionsList->count()), Error);
        }
        else
        {
            emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos));
            minionsList->move(oldPos, pos);
            updateMinionZoneSpots(friendly);
        }
    }
    this->lastMinionAdded = NULL;
}


bool PlanHandler::isLastMinionAddedValid()
{
    if(this->lastMinionAdded == NULL)   return false;

    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((now - this->lastMinionAddedTime) > 1000)
    {
        emit pDebug("POSITION(" + QString::number(this->lastMinionAdded->getId()) + ") Avoid OLD.");
        this->lastMinionAdded = NULL;
        return false;
    }
    return true;
}


void PlanHandler::playerCardTagChange(int id, QString code, QString tag, QString value)
{
    Q_UNUSED(code);
    cardTagChange(id, true, tag, value);
}


void PlanHandler::enemyCardTagChange(int id, QString code, QString tag, QString value)
{
    Q_UNUSED(code);
    cardTagChange(id, false, tag, value);
}


void PlanHandler::cardTagChange(int id, bool friendly, QString tag, QString value)
{
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    CardGraphicsItem *card = findCard(friendly, id);
    if(card == NULL)
    {
        QTimer::singleShot(1000, this, SLOT(checkCardPendingTagChanges()));
        cardPendingTagChanges.append(tagChange);
        emit pDebug("Append Tag Change Card: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        return;
    }

    emit pDebug("Tag Change Card: Id: " + QString::number(id) + " - " + tag + " --> " + value);
    card->processTagChange(tag, value);
    cardTagChangePrevTurn(id, friendly, tag, value);

    if(friendly && tag == "COST")
    {
        showManaPlayableCardsAuto();
    }
}


void PlanHandler::checkCardPendingTagChanges()
{
    if(cardPendingTagChanges.isEmpty()) return;

    TagChange tagChange = cardPendingTagChanges.takeFirst();
    const int id = tagChange.id;
    const bool friendly = tagChange.friendly;
    const QString tag = tagChange.tag;
    const QString value = tagChange.value;

    CardGraphicsItem *card = findCard(friendly, id);
    if(card == NULL)
    {
        //Try to find the minion on the board
        emit pDebug("Pending Tag Change Card, id not found, looking into minions: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        addMinionTagChange(id, friendly, tag, value);
        return;
    }

    emit pDebug("Pending Tag Change Card: Id: " + QString::number(id) + " - " + tag + " --> " + value);
    card->processTagChange(tag, value);
    cardTagChangePrevTurn(id, friendly, tag, value);

    if(friendly && tag == "COST")
    {
        showManaPlayableCardsAuto();
    }
}


void PlanHandler::cardTagChangePrevTurn(int id, bool friendly, QString tag, QString value)
{
    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();
    if(board->playerTurn != friendly)   return;

    CardGraphicsItem *card = findCard(friendly, id, board);
    if(card != NULL)
    {
        if(tag == "COST")   card->reduceCost(value.toInt());
        else                card->processTagChange(tag, value);
    }
}


void PlanHandler::playerMinionTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->playerHero!=NULL && nowBoard->playerHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->playerHero->getId(), Addon::AddonNeutral);
        addHero(true, code, id);
    }
    else    addMinionTagChange(id, true, tag, value);
}


void PlanHandler::enemyMinionTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->enemyHero!=NULL && nowBoard->enemyHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->enemyHero->getId(), Addon::AddonNeutral);
        addHero(false, code, id);
    }
    else    addMinionTagChange(id, false, tag, value);
}


bool PlanHandler::updateInPendingTagChange(int id, QString tag, QString value)
{
    for(QList<TagChange>::iterator it = pendingTagChanges.begin(); it != pendingTagChanges.end(); it++)
    {
        if(it->id == id && it->tag == tag)
        {
            emit pDebug("Pending Tag Change updated: Id: " + QString::number(id) + " - " + tag + " --> " + value);
            it->value = value;
            return true;
        }
    }
    return false;
}


void PlanHandler::addMinionTagChange(int id, bool friendly, QString tag, QString value)
{
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    bool healing = false;
    bool isDead = true;
    bool isHero = true;

    //Update if is in pendingTagChanges
    if(updateInPendingTagChange(id, tag, value)) return;

    //Minions
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != NULL)
    {
        emit pDebug("Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        checkAtkHealthChange(minion, friendly, tag, value);
        healing = minion->processTagChange(tag, value);
        isDead = minion->isDead();
        isHero = false;
        if(tag == "ATK" || tag == "EXHAUSTED" || tag == "WINDFURY" || tag == "FROZEN")      updateMinionsAttack(friendly);
    }

    //Heroes
    else if(friendly && nowBoard->playerHero!=NULL && nowBoard->playerHero->getId() == id)
    {
        emit pDebug("Tag Change Player Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        healing = nowBoard->playerHero->processTagChange(tag, value);
        isDead = nowBoard->playerHero->isDead();
        isHero = true;
    }
    else if(!friendly && nowBoard->enemyHero!=NULL && nowBoard->enemyHero->getId() == id)
    {
        emit pDebug("Tag Change Enemy Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        healing = nowBoard->enemyHero->processTagChange(tag, value);
        isDead = nowBoard->enemyHero->isDead();
        isHero = true;
    }

    //Heroe Powers
    else if(friendly && nowBoard->playerHeroPower!=NULL && nowBoard->playerHeroPower->getId() == id)
    {
        emit pDebug("Tag Change Player Hero Power: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->playerHeroPower->processTagChange(tag, value);
        if(tag == "EXHAUSTED" && value == "1" && !turnBoards.empty())   turnBoards.last()->playerHeroPower->processTagChange(tag, value);
        return;
    }
    else if(!friendly && nowBoard->enemyHeroPower!=NULL && nowBoard->enemyHeroPower->getId() == id)
    {
        emit pDebug("Tag Change Enemy Hero Power: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->enemyHeroPower->processTagChange(tag, value);
        if(tag == "EXHAUSTED" && value == "1" && !turnBoards.empty())   turnBoards.last()->enemyHeroPower->processTagChange(tag, value);
        return;
    }

    //Weapons
    else if(friendly && nowBoard->playerWeapon!=NULL && nowBoard->playerWeapon->getId() == id)
    {
        emit pDebug("Tag Change Player Weapon: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->playerWeapon->processTagChange(tag, value);
        return;
    }
    else if(!friendly && nowBoard->enemyWeapon!=NULL && nowBoard->enemyWeapon->getId() == id)
    {
        emit pDebug("Tag Change Enemy Weapon: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->enemyWeapon->processTagChange(tag, value);
        return;
    }

    //Pending tag change
    else
    {
        //Si un minion llega a la mano y es jugado rapido puede que reciba el ZONE = HAND y aparezca como muerto.
        if(tag == "ZONE")
        {
            emit pDebug("Zone Tag Change not appended: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        }
        else
        {
            QTimer::singleShot(1000, this, SLOT(checkPendingTagChanges()));
            pendingTagChanges.append(tagChange);
            emit pDebug("Append Tag Change: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        }
        return;
    }


    if(!isDead && isLastPowerAddonValid(tag, value, id, friendly, isHero, healing))
    {
        if(tag == "DAMAGE" || tag == "ARMOR" || tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "SHOULDEXITCOMBAT" ||
            (tag == "DIVINE_SHIELD" && value == "0"))
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, id, healing?Addon::AddonLife:Addon::AddonDamage);

            //Evita que un efecto que quita la armadura y hace algo de damage aparezca 2 veces
            if(isHero && tag == "ARMOR" && value == "0")
            {
                this->lastArmorRemoverIds.idAddon = this->lastPowerAddon.id;
                this->lastArmorRemoverIds.idHero = id;
                emit pDebug("Last armor remover set.");
            }
        }
        else if(
                   tag == "ATK" || tag == "HEALTH" || tag == "ZONE" ||
                   tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                   tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" || tag == "AURA" || tag == "CANT_BE_DAMAGED"
               )
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, id, Addon::AddonNeutral);
        }
    }
}


void PlanHandler::checkPendingTagChanges()
{
    if(pendingTagChanges.isEmpty()) return;

    TagChange tagChange = pendingTagChanges.takeFirst();
    const int id = tagChange.id;
    const bool friendly = tagChange.friendly;
    const QString tag = tagChange.tag;
    const QString value = tagChange.value;

    bool healing = false;
    bool isDead = true;
    bool isHero = true;

    //Minions
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != NULL)
    {
        emit pDebug("Pending Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        checkAtkHealthChange(minion, friendly, tag, value);
        healing = minion->processTagChange(tag, value);
        isDead = minion->isDead();
        isHero = false;
        if(tag == "ATK" || tag == "EXHAUSTED" || tag == "WINDFURY" || tag == "FROZEN")      updateMinionsAttack(friendly);
    }

    //Heroes
    else if(friendly && nowBoard->playerHero!=NULL && nowBoard->playerHero->getId() == id)
    {
        emit pDebug("Pending Tag Change Player Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        healing = nowBoard->playerHero->processTagChange(tag, value);
        isDead = nowBoard->playerHero->isDead();
        isHero = true;
    }
    else if(!friendly && nowBoard->enemyHero!=NULL && nowBoard->enemyHero->getId() == id)
    {
        emit pDebug("Pending Tag Change Enemy Hero: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        healing = nowBoard->enemyHero->processTagChange(tag, value);
        isDead = nowBoard->enemyHero->isDead();
        isHero = true;
    }

    //Heroe Powers
    else if(friendly && nowBoard->playerHeroPower!=NULL && nowBoard->playerHeroPower->getId() == id)
    {
        emit pDebug("Pending Tag Change Player Hero Power: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->playerHeroPower->processTagChange(tag, value);
        if(tag == "EXHAUSTED" && value == "1" && !turnBoards.empty())   turnBoards.last()->playerHeroPower->processTagChange(tag, value);
        return;
    }
    else if(!friendly && nowBoard->enemyHeroPower!=NULL && nowBoard->enemyHeroPower->getId() == id)
    {
        emit pDebug("Pending Tag Change Enemy Hero Power: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->enemyHeroPower->processTagChange(tag, value);
        if(tag == "EXHAUSTED" && value == "1" && !turnBoards.empty())   turnBoards.last()->enemyHeroPower->processTagChange(tag, value);
        return;
    }

    //Weapons
    else if(friendly && nowBoard->playerWeapon!=NULL && nowBoard->playerWeapon->getId() == id)
    {
        emit pDebug("Pending Tag Change Player Weapon: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->playerWeapon->processTagChange(tag, value);
        return;
    }
    else if(!friendly && nowBoard->enemyWeapon!=NULL && nowBoard->enemyWeapon->getId() == id)
    {
        emit pDebug("Pending Tag Change Enemy Weapon: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        nowBoard->enemyWeapon->processTagChange(tag, value);
        return;
    }

    else
    {
        emit pDebug("Pending Tag Change, id not found: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        return;
    }


    if(!isDead && isLastPowerAddonValid(tag, value, id, friendly, isHero, healing))
    {
        if(tag == "DAMAGE" || tag == "ARMOR" || tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "SHOULDEXITCOMBAT" ||
            (tag == "DIVINE_SHIELD" && value == "0"))
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, id, healing?Addon::AddonLife:Addon::AddonDamage);

            //Evita que un efecto que quita la armadura y hace algo de damage aparezca 2 veces
            if(isHero && tag == "ARMOR" && value == "0")
            {
                this->lastArmorRemoverIds.idAddon = this->lastPowerAddon.id;
                this->lastArmorRemoverIds.idHero = id;
                emit pDebug("Last armor remover set.");
            }
        }
        else if(
                   tag == "ATK" || tag == "HEALTH" || tag == "ZONE" ||
                   tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                   tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" || tag == "AURA" || tag == "CANT_BE_DAMAGED"
               )
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, id, Addon::AddonNeutral);
        }
    }
}


bool PlanHandler::isLastPowerAddonValid(QString tag, QString value, int idTarget, bool friendly, bool isHero, bool healing)
{
    if(this->lastPowerAddon.id == -1)   return false;

    //Evita addons al perder un arma y cambiar el atk a 0
    if(isHero && tag == "ATK" && value == "0")
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid ATK to 0 when losing a weapon.");
        return false;
    }

    //Evita addons por perder el frozen al final del turno
    if(tag == "FROZEN" && value == "0")
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid FROZEN lost in end turn.");
        return false;
    }

    //Evita minions a ZONE PLAY
    if(tag == "ZONE" && value == "PLAY")
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid minion moved to PLAY.");
        return false;
    }

    //Evita que un efecto que quita la armadura y hace algo de damage aparezca 2 veces
    if(isHero && tag == "DAMAGE" && !healing &&
            this->lastArmorRemoverIds.idAddon == this->lastPowerAddon.id &&
            this->lastArmorRemoverIds.idHero == idTarget)
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid duplicate ARMOR+DAMAGE addons on hero.");
        this->lastArmorRemoverIds.idAddon = -1;
        return false;
    }

    //Evita addons por cambio de ATK/HEALTH provocados por AURAS
    if(!isHero && (tag == "ATK" || tag == "HEALTH") && areThereAuras(friendly))
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid ATK/HEALTH with auras.");
        return false;
    }

    //Evita ENCHANTMENT
    if(Utility::getCardAtribute(this->lastPowerAddon.code, "type").toString() == "ENCHANTMENT")
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid ENCHANTMENT.");
        return false;
    }

    //Evita forbidden addons minion
    if(!isHero && !isAddonMinionValid(this->lastPowerAddon.code))
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid invalid minion code.");
        return false;
    }

    //Evita forbidden addons heroe
    if(isHero && !isAddonHeroValid(this->lastPowerAddon.code))
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid invalid hero code.");
        return false;
    }    

    //Evita old TRIGGERS
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((now - this->lastPowerTime) > 8000)
    {
        emit pDebug("Addon(" + QString::number(idTarget) + ")-->" + this->lastPowerAddon.code + " Avoid OLD.");
        this->lastPowerAddon.id = -1;
        return false;
    }

    return true;
}


void PlanHandler::checkAtkHealthChange(MinionGraphicsItem * minion, bool friendly, QString tag, QString value)
{
    if(minion == NULL || minion->isDead())  return;

    if(tag == "ATK")
    {
        if(turnBoards.empty())  return;

        MinionGraphicsItem * minionLastTurn = findMinion(friendly, minion->getId(), turnBoards.last());
        if(minionLastTurn == NULL)  return;

        int attack = minion->getAttack();
        int newAttack = value.toInt();

        if(newAttack > attack)          minionLastTurn->setChangeAttack(MinionGraphicsItem::ChangePositive);
        else if(newAttack < attack)     minionLastTurn->setChangeAttack(MinionGraphicsItem::ChangeNegative);
    }
    else if(tag == "HEALTH")
    {
        if(turnBoards.empty())  return;

        MinionGraphicsItem * minionLastTurn = findMinion(friendly, minion->getId(), turnBoards.last());
        if(minionLastTurn == NULL)  return;

        int health = minion->getHealth();
        int newHealth = value.toInt();

        if(newHealth > health)          minionLastTurn->setChangeHealth(MinionGraphicsItem::ChangePositive);
        else if(newHealth < health)     minionLastTurn->setChangeHealth(MinionGraphicsItem::ChangeNegative);
    }
}


void PlanHandler::playerTagChange(QString tag, QString value)
{
    addTagChange(true, tag, value);
}


void PlanHandler::enemyTagChange(QString tag, QString value)
{
    addTagChange(false, tag, value);
}


void PlanHandler::unknownTagChange(QString tag, QString value)
{
    addTagChange(!nowBoard->playerTurn, tag, value);
    if(this->firstStoredTurn == 0)  addTagChange(nowBoard->playerTurn, tag, value);
}


void PlanHandler::addTagChange(bool friendly, QString tag, QString value)
{
    HeroGraphicsItem *hero = getHero(friendly, NULL);
    if(hero == NULL)        return;

    if(tag == "RESOURCES")
    {
        hero->setResources(value.toInt());
        if(friendly)    showManaPlayableCardsAuto();
    }
    else if(tag == "RESOURCES_USED")
    {
        hero->setResourcesUsed(value.toInt());
        //        13:08:35 - GameWatcher(11790): Player: TAG_CHANGE(RESOURCES)= 5 -- Name: triodo
        //        13:08:35 - GameWatcher(11791): Player: TAG_CHANGE(RESOURCES_USED)= 0 -- Name: triodo
        //El reinicio de los recursos del jugador se hace al final del turno enemigo por eso
        //forzamos a que el de RESOURCES_USED sea para el turno actual del jugador.
        if(friendly)    showManaPlayableCards(nowBoard);
    }
    else if(tag == "CURRENT_SPELLPOWER")
    {
        hero->setSpellDamage(value.toInt());
    }
}


bool PlanHandler::findAttackPoint(ArrowGraphicsItem *attack, bool isFrom, int id, Board *board)
{
    if(board->playerHero!=NULL && board->playerHero->getId() == id)
    {
        attack->setEnd(isFrom, board->playerHero);
    }
    else if(board->enemyHero!=NULL && board->enemyHero->getId() == id)
    {
        attack->setEnd(isFrom, board->enemyHero);
    }
    else
    {
        QList<MinionGraphicsItem *> * minionsList = getMinionList(true, board);
        int pos = findMinionPos(minionsList, id);
        if(pos != -1 && !minionsList->at(pos)->isDead())
        {
            attack->setEnd(isFrom, minionsList->at(pos));
        }
        else
        {
            minionsList = getMinionList(false, board);
            pos = findMinionPos(minionsList, id);
            if(pos != -1 && !minionsList->at(pos)->isDead())
            {
                attack->setEnd(isFrom, minionsList->at(pos));
            }
            else
            {
                emit pDebug("Attack section not found or dead. Id: " + QString::number(id), Warning);
                return false;
            }
        }
    }
    return true;
}


bool PlanHandler::appendAttack(ArrowGraphicsItem *attack, Board *board)
{
    int fromId = attack->getEnd(true)->getId();
    int toId = attack->getEnd(false)->getId();

    foreach(ArrowGraphicsItem *arrow, board->arrows)
    {
        if(arrow->getEnd(true)->getId() == fromId && arrow->getEnd(false)->getId() == toId)
        {
            arrow->increaseNumAttacks();
            arrow->update();
            return false;
        }
    }

    board->arrows.append(attack);
    return true;
}


void PlanHandler::zonePlayAttack(QString code, int id1, int id2)
{
    if(turnBoards.empty())  return;

    ArrowGraphicsItem *attack = new ArrowGraphicsItem();
    Board *board = turnBoards.last();

    //To
    if(findAttackPoint(attack, false, id2, board))
    {
        //From
        if(findAttackPoint(attack, true, id1, board))
        {
            if(board->playerTurn == attack->isFriendly())
            {
                if(appendAttack(attack, board))
                {
                    if(viewBoard == board)  ui->planGraphicsView->scene()->addItem(attack);
                }
                else    delete attack;
            }
            else
            {
                emit pDebug("Attack registered in the wrong turn.", Warning);
                delete attack;
            }
        }
        //Ataque con carga de minion recien jugado
        else
        {
            addAddon(attack->getEnd(false), code, id1, Addon::AddonDamage);
            delete attack;
        }
    }
    else
    {
        delete attack;
    }
}


void PlanHandler::playerWeaponZonePlayAdd(QString code, int id)
{
    addWeapon(true, code, id);
}


void PlanHandler::enemyWeaponZonePlayAdd(QString code, int id)
{
    addWeapon(false, code, id);
}


void PlanHandler::addWeapon(bool friendly, QString code, int id)
{
    qDebug()<<"NEW WEAPON --> id"<<id;
    WeaponGraphicsItem* weapon = friendly?nowBoard->playerWeapon:nowBoard->enemyWeapon;

    if(weapon != NULL)
    {
        emit pDebug("Trying to add a weapon with an existing one. Force remove old one.", Warning);
        removeWeapon(friendly);
    }

    weapon = new WeaponGraphicsItem(code, id, friendly, graphicsItemSender);
    HeroGraphicsItem * heroNow = getHero(friendly);
    if(heroNow != NULL)    heroNow->setHeroWeapon(weapon);

    if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->addItem(weapon);

    if(friendly)    nowBoard->playerWeapon = weapon;
    else            nowBoard->enemyWeapon = weapon;

    emit checkCardImage(code, false);
    setLastTriggerId("", "", -1, -1);

    //Add addon to last turn
    HeroGraphicsItem* heroLast = friendly?nowBoard->playerHero:nowBoard->enemyHero;
    if(heroLast != NULL)    addAddonToLastTurn(code, id, heroLast->getId(), Addon::AddonNeutral);
}


void PlanHandler::playerWeaponZonePlayRemove(int id)
{
    removeWeapon(true, id);
    killWeaponLastTurn(true, id);
}


void PlanHandler::enemyWeaponZonePlayRemove(int id)
{
    removeWeapon(false, id);
    killWeaponLastTurn(false, id);
}


void PlanHandler::removeWeapon(bool friendly, int id, Board *board)
{
    if(board == NULL)   board = nowBoard;
    WeaponGraphicsItem* weapon = friendly?board->playerWeapon:board->enemyWeapon;
    if(weapon == NULL)
    {
        emit pDebug("Trying to remove weapon NULL.", Warning);
    }
    else if(id == -1 || weapon->getId() == id)
    {
        HeroGraphicsItem * heroNow = getHero(friendly, board);
        if(heroNow != NULL)    heroNow->setHeroWeapon();
        delete weapon;

        if(friendly)    board->playerWeapon = NULL;
        else            board->enemyWeapon = NULL;
    }
    else
    {
        emit pDebug("Trying to remove a weapon different that equipped.", Warning);
    }
}


void PlanHandler::killWeaponLastTurn(bool friendly, int id)
{
    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();
    WeaponGraphicsItem* weapon = friendly?board->playerWeapon:board->enemyWeapon;
    if(weapon == NULL)
    {
        emit pDebug("Trying to kill weapon NULL in last turn.", Warning);
    }
    else if(weapon->getId() == id)
    {
        weapon->setDead(true);
    }
    else
    {
        emit pDebug("Trying to kill a weapon different that equipped in last turn.", Warning);
    }
}


void PlanHandler::playerHeroPowerZonePlayAdd(QString code, int id)
{
    addHeroPower(true, code, id);
}


void PlanHandler::enemyHeroPowerZonePlayAdd(QString code, int id)
{
    addHeroPower(false, code, id);
}


void PlanHandler::addHeroPower(bool friendly, QString code, int id)
{
    qDebug()<<"NEW HERO POWER --> id"<<id;
    //Avoid fancy Hero Powers codes
    if(code.endsWith("_H1"))    code = code.left(code.length()-3);

    HeroPowerGraphicsItem* heroPower = friendly?nowBoard->playerHeroPower:nowBoard->enemyHeroPower;

    if(heroPower != NULL)
    {
        HeroGraphicsItem* hero = friendly?nowBoard->playerHero:nowBoard->enemyHero;
        if(hero != NULL)    addAddonToLastTurn(code, id, hero->getId(), Addon::AddonNeutral);
        heroPower->changeHeroPower(code, id);
    }
    else
    {
        heroPower = new HeroPowerGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);

        if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->addItem(heroPower);

        if(friendly)    nowBoard->playerHeroPower = heroPower;
        else            nowBoard->enemyHeroPower = heroPower;
    }
    emit checkCardImage(code, false);
}


void PlanHandler::removeHeroPower(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    HeroPowerGraphicsItem* heroPower = friendly?board->playerHeroPower:board->enemyHeroPower;
    if(heroPower == NULL)
    {
        emit pDebug("Trying to remove Hero Power NULL.", Warning);
    }
    else
    {
        delete heroPower;

        if(friendly)    board->playerHeroPower = NULL;
        else            board->enemyHeroPower = NULL;
    }
}


void PlanHandler::playerCardObjPlayed(QString code, int id1, int id2)
{
    if(nowBoard->playerTurn)    addAddonToLastTurn(code, id1, id2, Addon::AddonNeutral);
    else                        emit pDebug("Minion addon registered in the wrong turn.");
}


void PlanHandler::enemyCardObjPlayed(QString code, int id1, int id2)
{
    if(!nowBoard->playerTurn)   addAddonToLastTurn(code, id1, id2, Addon::AddonNeutral);
    else                        emit pDebug("Minion addon registered in the wrong turn.");
}


void PlanHandler::addAddonToLastTurn(QString code, int id1, int id2, Addon::AddonType type, int number)
{
    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();

    if(board->playerHero!=NULL && board->playerHero->getId() == id2)
    {
        addAddon(board->playerHero, code, id1, type, number);
    }
    else if(board->enemyHero!=NULL && board->enemyHero->getId() == id2)
    {
        addAddon(board->enemyHero, code, id1, type, number);
    }
    else
    {
        QList<MinionGraphicsItem *> * minionsList = getMinionList(true, board);
        int pos = findMinionPos(minionsList, id2);
        if(pos != -1)
        {
            MinionGraphicsItem *targetMinion = minionsList->at(pos);
            addAddon(targetMinion, code, id1, type, number);
        }
        else
        {
            minionsList = getMinionList(false, board);
            pos = findMinionPos(minionsList, id2);
            if(pos != -1)
            {
                MinionGraphicsItem *targetMinion = minionsList->at(pos);
                addAddon(targetMinion, code, id1, type, number);
            }
            else
            {
                emit pDebug("Addon(" + QString::number(id2) + ")-->" + code + " Minion id not found.", Warning);
            }
        }
    }
}


bool PlanHandler::areThereAuras(bool friendly)
{
    //Check now board
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    foreach(MinionGraphicsItem *minion, *minionsList)
    {
        if(minion->isAura())    return true;
    }

    //Check last turn board
    if(turnBoards.empty())  return false;
    Board *board = turnBoards.last();
    minionsList = getMinionList(friendly, board);
    foreach(MinionGraphicsItem *minion, *minionsList)
    {
        if(minion->isAura())    return true;
    }

    return false;
}


void PlanHandler::addAddon(MinionGraphicsItem *minion, QString code, int id, Addon::AddonType type, int number)
{
    //Avoid fancy Hero Powers codes
    if(code.endsWith("_H1"))    code = code.left(code.length()-3);

    emit pDebug("Addon(" + QString::number(minion->getId()) + ")-->" + code);
    minion->addAddon(code, id, type, number);
    if(code != "FATIGUE")   emit checkCardImage(code, false);
}


void PlanHandler::playerSecretPlayed(int id, QString code)
{
    if(nowBoard->playerHero == NULL) return;
    DeckCard deckCard(code);
    nowBoard->playerHero->addSecret(id, deckCard.getCardClass());
}


void PlanHandler::enemySecretPlayed(int id, CardClass secretHero)
{
    if(nowBoard->enemyHero == NULL) return;
    nowBoard->enemyHero->addSecret(id, secretHero);
}


void PlanHandler::playerSecretRevealed(int id, QString code)
{
    if(nowBoard->playerHero == NULL) return;
    nowBoard->playerHero->removeSecret(id);

    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    board->playerHero->showSecret(id, code);
}


void PlanHandler::enemySecretRevealed(int id, QString code)
{
    if(nowBoard->enemyHero == NULL) return;
    nowBoard->enemyHero->removeSecret(id);

    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    board->enemyHero->showSecret(id, code);

    revealEnemyCardPrevTurns(id, code);
}


void PlanHandler::enemyIsolatedSecret(int id, QString code)
{
    if(nowBoard->enemyHero == NULL) return;
    nowBoard->enemyHero->showSecret(id, code);
}


void PlanHandler::playerSecretStolen(int id, QString code)
{
    if(nowBoard->enemyHero == NULL) return;
    CardClass secretHero = nowBoard->enemyHero->getSecretHero(id);
    enemySecretRevealed(id, code);
    if(nowBoard->playerHero != NULL)    nowBoard->playerHero->addSecret(id, secretHero);
}


void PlanHandler::enemySecretStolen(int id, QString code)
{
    if(nowBoard->playerHero == NULL) return;
    CardClass secretHero = nowBoard->playerHero->getSecretHero(id);
    playerSecretRevealed(id, code);
    enemySecretPlayed(id, secretHero);
}


void PlanHandler::updateViewCardZoneSpots()
{
    updateCardZoneSpots(true, viewBoard);
    updateCardZoneSpots(false, viewBoard);
}


void PlanHandler::updateCardZoneSpots(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    QList<CardGraphicsItem *> *handList = getHandList(friendly, board);
    int viewWidth = ui->planGraphicsView->getSceneViewWidth();
    int cardHeightShow = ui->planGraphicsView->getCardsViewHeight();
    for(int i=0; i<handList->count(); i++)
    {
        handList->at(i)->setZonePos(friendly, i, handList->count(), viewWidth, cardHeightShow);
    }
}


int PlanHandler::findCardPos(QList<CardGraphicsItem *> * cardsList, int id)
{
    //Hacemos la busqueda de derecha a izquierda.
    //En el turno mulligan puede ser que este la misma carta (id) dos veces en la mano pq la hayas hecho mulligan y robada en turno 1.
    //Queremos que devuelva la que no ha sido descartada (la mas a la derecha).
    for(int i=cardsList->count()-1; i>=0; i--)
    {
        if(cardsList->at(i)->getId()==id) return i;
    }
    return -1;
}


QList<CardGraphicsItem *> * PlanHandler::getHandList(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    if(friendly)    return &board->playerHandList;
    else            return &board->enemyHandList;
}


CardGraphicsItem * PlanHandler::findCard(bool friendly, int id, Board *board)
{
    QList<CardGraphicsItem *> * handList = getHandList(friendly, board);
    int pos = findCardPos(handList, id);
    if(pos == -1)   return NULL;
    else            return handList->at(pos);
}


void PlanHandler::revealEnemyCard(int id, QString code)
{
    CardGraphicsItem *card = findCard(false, id);
    if(card != NULL)
    {
        card->changeCode(code);
        emit checkCardImage(code, false);
    }
    revealEnemyCardPrevTurns(id, code);
}


void PlanHandler::playerCardDraw(int id, QString code, int turn)
{
    cardDraw(true, id, code, "", turn);

    //Al robar una carta la volvemos transparente si no se puede jugar
    //Queremos que las cartas en el mulligan se vean bien claras
    if(turn != 0)
    {
        showManaPlayableCardsAuto();
    }
}


void PlanHandler::enemyCardDraw(int id, QString code, QString createdByCode, int turn)
{
    cardDraw(false, id, code, createdByCode, turn);
}


CardGraphicsItem * PlanHandler::cardDraw(bool friendly, int id, QString code, QString createdByCode, int turn)
{
    CardGraphicsItem *card = new CardGraphicsItem(id, code, createdByCode, turn, friendly, graphicsItemSender);
    getHandList(friendly)->append(card);
    updateCardZoneSpots(friendly);

    if(viewBoard == nowBoard)       ui->planGraphicsView->scene()->addItem(card);
    if(!code.isEmpty())             emit checkCardImage(code, false);
    if(!createdByCode.isEmpty())    emit checkCardImage(createdByCode, false);

    //Show card draw last turn
    if(!turnBoards.empty())
    {
        Board *board = turnBoards.last();
        if(board->numTurn == turn)
        {
            CardGraphicsItem *drawCard = findCard(friendly, id, board);

            if(drawCard == NULL || drawCard->isDiscard())
            {
                drawCard = new CardGraphicsItem(card);
                drawCard->setDraw();
                getHandList(friendly, board)->append(drawCard);
                updateCardZoneSpots(friendly, board);
                if(viewBoard == board)      ui->planGraphicsView->scene()->addItem(drawCard);
            }
            else
            {
                //Battlecry cancelado y vuelve a la mano
                drawCard->setPlayed(false);
            }
        }
    }

    return card;
}


void PlanHandler::enemyCardBuff(int id, int buffAttack, int buffHealth)
{
    CardGraphicsItem * card = findCard(false, id);

    if(card == NULL)
    {
        emit pDebug("ERROR: CardGraphicsItem not found for buffing. Id: " + QString::number(id));
        return;
    }

    card->addBuff(buffAttack, buffHealth);
}


void PlanHandler::playerCardPlayed(int id, QString code, bool discard)
{
    cardPlayed(true, id, code, discard);
}


void PlanHandler::enemyCardPlayed(int id, QString code, bool discard)
{
    cardPlayed(false, id, code, discard);
}


void PlanHandler::cardPlayed(bool friendly, int id, QString code, bool discard)
{
    QList<CardGraphicsItem *> *handList = getHandList(friendly);
    int pos = findCardPos(handList, id);

    if(pos == -1)
    {
        emit pDebug((friendly?"Player":"Enemy") + QString(" card played not found: ") + QString::number(id) + " -- " + code);
    }
    else
    {
        CardGraphicsItem *card = handList->takeAt(pos);
        updateCardZoneSpots(friendly);

        if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->removeItem(card);

        delete card;
    }

    //Set played/discard last board
    if(!turnBoards.empty())
    {
        Board *board = turnBoards.last();
        CardGraphicsItem *card = findCard(friendly, id, board);

        if(card == NULL)
        {
            emit pDebug((friendly?"Player":"Enemy") + QString(" card played not found on last turn: ") + QString::number(id) + " -- " + code);
        }
        else
        {
            if(discard)     card->setDiscard();
            else            card->setPlayed();
        }
    }

    //Reveal in all turns
    if(!friendly && !code.isEmpty())
    {
        revealEnemyCardPrevTurns(id, code);
    }
}


void PlanHandler::revealEnemyCardPrevTurns(int id, QString code)
{
    foreach(Board *board, turnBoards)
    {
        CardGraphicsItem *card = findCard(false, id, board);
        if(card != NULL)
        {
            card->changeCode(code);
            emit checkCardImage(code, false);
        }
    }
}


void PlanHandler::fixTurn1Card()
{
    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    QList<CardGraphicsItem *> *cardList = getHandList(board->playerTurn, board);
    if(!cardList->isEmpty())     cardList->last()->setDraw(false);
}


void PlanHandler::playerCardCodeChange(int id, QString newCode)
{
    CardGraphicsItem *card = findCard(true, id);

    if(card != NULL)
    {
        emit pDebug("Player card Id: " + QString::number(id) + " changed Code: " + card->getCode() + " --> " + newCode);
        card->changeCode(newCode);
        emit checkCardImage(newCode, false);

//No es necesario ya que se hace justo antes de cambiar de turno, y en este ya se llama.
//Ademas esto ocurre justa despues de cambiar los RESOURCES del jugador pero antes de cambiar el turno, asi que el Auto
//no serviria ya que haria el calculo como si fuese el turno enemigo pero con los nuevos RESOURCES
//        showManaPlayableCardsAuto();
    }
    else
    {
        emit pDebug("Player card Id: " + QString::number(id) + " not found in player hand.");
    }
}


void PlanHandler::newTurn(bool playerTurn, int numTurn)
{
    if(numTurn == 2)    fixTurn1Card();

    //Update nowBoard
    nowBoard->playerTurn = playerTurn;

    foreach(MinionGraphicsItem * minion, nowBoard->playerMinions)
    {
        minion->setPlayerTurn(playerTurn);
    }

    foreach(MinionGraphicsItem * minion, nowBoard->enemyMinions)
    {
        minion->setPlayerTurn(playerTurn);
    }

    if(nowBoard->playerHero != NULL)        nowBoard->playerHero->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHero != NULL)         nowBoard->enemyHero->setPlayerTurn(playerTurn);

    if(nowBoard->playerHeroPower != NULL)   nowBoard->playerHeroPower->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHeroPower != NULL)    nowBoard->enemyHeroPower->setPlayerTurn(playerTurn);

    if(this->firstStoredTurn == 0)
    {
        this->firstStoredTurn = numTurn;

        if(numTurn > 5 && nowBoard->playerHero->getResources() == 1)
        {
            nowBoard->playerHero->setResources(10);
            nowBoard->enemyHero->setResources(10);
        }
    }

    //Store nowBoard
    turnBoards.append(copyBoard(nowBoard, numTurn));
    showManaPlayableCardsAuto();
    updateTurnSliderRange();

    //Avanza en now board
    if(viewBoard==nowBoard || viewBoard==futureBoard)   showLastTurn();
}


Board * PlanHandler::copyBoard(Board *origBoard, int numTurn, bool copySecretCodes)
{
    Board *board = new Board();
    board->playerTurn = origBoard->playerTurn;
    board->numTurn = numTurn;

    if(origBoard->playerHero == NULL)    board->playerHero = NULL;
    else                                board->playerHero = new HeroGraphicsItem(origBoard->playerHero);
    if(origBoard->enemyHero == NULL)     board->enemyHero = NULL;
    else                                board->enemyHero = new HeroGraphicsItem(origBoard->enemyHero, copySecretCodes);//Lo usamos al crear el futureBoard, solo es necesario para el enemigo ya que los secretos amigos de nowBoard nunca estaran desvelados ni isolated.

    if(origBoard->playerHeroPower == NULL)   board->playerHeroPower = NULL;
    else                                    board->playerHeroPower = new HeroPowerGraphicsItem(origBoard->playerHeroPower);
    if(origBoard->enemyHeroPower == NULL)    board->enemyHeroPower = NULL;
    else                                    board->enemyHeroPower = new HeroPowerGraphicsItem(origBoard->enemyHeroPower);

    if(origBoard->playerWeapon == NULL)  board->playerWeapon = NULL;
    else                                board->playerWeapon = new WeaponGraphicsItem(origBoard->playerWeapon);
    if(origBoard->enemyWeapon == NULL)   board->enemyWeapon = NULL;
    else                                board->enemyWeapon = new WeaponGraphicsItem(origBoard->enemyWeapon);

    foreach(MinionGraphicsItem * minion, origBoard->playerMinions)
    {
        board->playerMinions.append(new MinionGraphicsItem(minion));
    }

    foreach(MinionGraphicsItem * minion, origBoard->enemyMinions)
    {
        board->enemyMinions.append(new MinionGraphicsItem(minion));
    }

    foreach(CardGraphicsItem * card, origBoard->playerHandList)
    {
        board->playerHandList.append(new CardGraphicsItem(card));
    }

    foreach(CardGraphicsItem * card, origBoard->enemyHandList)
    {
        board->enemyHandList.append(new CardGraphicsItem(card));
    }

    return board;
}


//Evita addons provocado por ocultar/aparecer el arma al final del turno
void PlanHandler::resetLastPowerAddon()
{
    emit pDebug("Reset lastPowerAddon.");
    this->lastPowerAddon.id = -1;
}


void PlanHandler::setLastTriggerId(QString code, QString blockType, int id, int idTarget)
{
    if(blockType == "TRIGGER" || blockType == "JOUST")
    {
        if(isLastTriggerValid(code))    this->lastTriggerId = id;
        else                            emit pDebug("Trigger creator code is in the forbidden list: " + code, Warning);
        this->lastPowerAddon.code = code;
        this->lastPowerAddon.id = id;
        this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    else if(blockType == "POWER")
    {
        this->lastTriggerId = idTarget;
        this->lastPowerAddon.code = code;
        this->lastPowerAddon.id = id;
        this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    else if(blockType == "FATIGUE")
    {
        this->lastTriggerId = -1;
        this->lastPowerAddon.code = "FATIGUE";
        this->lastPowerAddon.id = id;
        this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    else
    {
        this->lastTriggerId = -1;
        this->lastPowerAddon.id = -1;
    }
    this->lastArmorRemoverIds.idAddon = -1;
}


void PlanHandler::redrawDownloadedCardImage(QString code)
{
    foreach(MinionGraphicsItem * minion, viewBoard->playerMinions)
    {
        minion->checkDownloadedCode(code);
    }
    foreach(MinionGraphicsItem * minion, viewBoard->enemyMinions)
    {
        minion->checkDownloadedCode(code);
    }
    foreach(CardGraphicsItem * card, viewBoard->playerHandList)
    {
        card->checkDownloadedCode(code);
    }
    foreach(CardGraphicsItem * card, viewBoard->enemyHandList)
    {
        card->checkDownloadedCode(code);
    }

    if(viewBoard->playerHero != NULL)   viewBoard->playerHero->checkDownloadedCode(code);
    if(viewBoard->enemyHero != NULL)    viewBoard->enemyHero->checkDownloadedCode(code);

    if(viewBoard->playerHeroPower != NULL)  viewBoard->playerHeroPower->checkDownloadedCode(code);
    if(viewBoard->enemyHeroPower != NULL)   viewBoard->enemyHeroPower->checkDownloadedCode(code);

    if(viewBoard->playerWeapon != NULL) viewBoard->playerWeapon->checkDownloadedCode(code);
    if(viewBoard->enemyWeapon != NULL)  viewBoard->enemyWeapon->checkDownloadedCode(code);
}


void PlanHandler::createFutureBoard()
{
    futureBoard = copyBoard(nowBoard, 0, true);
    futureBoard->playerHero->setShowAllInfo();
    futureBoard->enemyHero->setShowAllInfo();
    viewBoard = futureBoard;
    loadViewBoard();
    ui->planButtonLast->setIcon(QIcon(":Images/refresh.png"));
    ui->planButtonLast->setEnabled(true);
    emit cardLeave();   //Hide cards tooltips
    showManaPlayableCards(futureBoard);
}


int PlanHandler::getPotentialMana(Board *board)
{
    int mana = board->playerHero->getAvailableResources();
    foreach(CardGraphicsItem* card, board->playerHandList)
    {
        if(!card->isPlayed())
        {
            int manaCard = card->getManaSpent();
            if(manaCard < 0)    mana -= manaCard;
        }
    }

    return mana;
}


void PlanHandler::showManaPlayableCards(Board *board)
{
    int mana = getPotentialMana(board);
    foreach(CardGraphicsItem* card, board->playerHandList)
    {
        if(!card->isPlayed())   card->showManaPlayable(mana);
    }

    if(board->playerHeroPower != NULL && !board->playerHeroPower->isExausted())
    {
        board->playerHeroPower->showManaPlayable(mana);
    }
}


int PlanHandler::getPotentialManaNextTurn()
{
    int mana = nowBoard->playerHero->getResources() + 1;
    foreach(CardGraphicsItem* card, nowBoard->playerHandList)
    {
        int manaCard = card->getManaSpent();
        if(manaCard < 0)    mana -= manaCard;
    }

    return min(10, mana);
}


void PlanHandler::showManaPlayableCardsNextTurn()
{
    int mana = getPotentialManaNextTurn();
    foreach(CardGraphicsItem* card, nowBoard->playerHandList)
    {
        card->showManaPlayable(mana);
    }

    if(nowBoard->playerHeroPower != NULL && !nowBoard->playerHeroPower->isExausted())
    {
        nowBoard->playerHeroPower->showManaPlayable(mana);
    }
}


void PlanHandler::showManaPlayableCardsAuto()
{
    if(!nowBoard->playerTurn)   showManaPlayableCardsNextTurn();
    else                        showManaPlayableCards(nowBoard);
}


void PlanHandler::cardPress(CardGraphicsItem* card, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)           return;
    if(mouseButton != Qt::LeftButton)   return;

    bool doToggle = false;
    if(futureBoard == NULL)
    {
        if(nowBoard->playerHandList.contains(card))
        {
            createFutureBoard();

            //Get card
            QList<CardGraphicsItem *> * handList = getHandList(true, futureBoard);
            int pos = findCardPos(handList, card->getId());
            if(pos == -1)
            {
                emit pDebug("ERROR: Clicked card not found in just created futureBoard.", Error);
                return;
            }

            card = handList->at(pos);
            doToggle = true;
        }
    }
    else
    {
        if(futureBoard->playerHandList.contains(card))
        {
            doToggle = true;
        }
    }

    if(doToggle && !card->isTransparent())
    {
        card->togglePlayed();
        selectedCode = (card->isPlayed()?card->getCode():"");
        futureBoard->playerHero->addResourcesUsed(card->isPlayed()?card->getManaSpent():-card->getManaSpent());
        showManaPlayableCards(futureBoard);
    }
}


void PlanHandler::heroPowerPress(HeroPowerGraphicsItem* heroPower, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(mouseButton != Qt::LeftButton)   return;

    bool doToggle = false;
    if(futureBoard == NULL)
    {
        if(nowBoard->playerHeroPower == heroPower)
        {
            createFutureBoard();

            //Get hero power
            heroPower = futureBoard->playerHeroPower;
            doToggle = true;
        }
    }
    else
    {
        if(futureBoard->playerHeroPower == heroPower)
        {
            doToggle = true;
        }
    }

    if(doToggle && !heroPower->isTransparent())
    {
        heroPower->toggleExausted();
        selectedCode = (heroPower->isExausted()?heroPower->getCode():"");
        futureBoard->playerHero->addResourcesUsed(heroPower->isExausted()?2:-2);
        showManaPlayableCards(futureBoard);
    }
}


void PlanHandler::minionPress(MinionGraphicsItem* minion, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(minion->isDead())  return;

    if(futureBoard == NULL)
    {
        if(nowBoard->playerMinions.contains(minion) && minion->getAttack()>0)
        {
            createFutureBoard();

            //Get minion
            QList<MinionGraphicsItem *> * minionList = getMinionList(true, futureBoard);
            int pos = findMinionPos(minionList, minion->getId());
            if(pos == -1)
            {
                emit pDebug("ERROR: Clicked minion not found in just created futureBoard.", Error);
                return;
            }

            minion = minionList->at(pos);
            selectedMinion = minion;
            selectedMinion->selectMinion();
        }
    }
    else
    {
        //Show addon
        if(mouseButton != Qt::LeftButton)
        {
            if(!selectedCode.isEmpty())
            {
                minion->addPlanningAddon(selectedCode, Addon::AddonNeutral);
            }
        }
        else
        {
            //Select minion
            if(selectedMinion == NULL)
            {
                if(futureBoard->playerMinions.contains(minion) && minion->getAttack()>0)
                {
                    selectedCode = "";
                    selectedMinion = minion;
                    selectedMinion->selectMinion();
                }
            }
            //Show attack
            else
            {
                if(futureBoard->playerMinions.contains(minion) && minion->getAttack()>0)
                {
                    selectedCode = "";

                    //Deselect minion
                    if(selectedMinion == minion)
                    {
                        selectedMinion->selectMinion(false);
                        selectedMinion = NULL;
                    }
                    else
                    {
                        selectedMinion->selectMinion(false);
                        selectedMinion = minion;
                        selectedMinion->selectMinion();
                    }
                }
                else if(futureBoard->enemyMinions.contains(minion))
                {
                    //Show attack
                    ArrowGraphicsItem *attack = new ArrowGraphicsItem();
                    attack->setEnd(true, selectedMinion);//From
                    attack->setEnd(false, minion);//To
                    if(appendAttack(attack, futureBoard))
                    {
                        if(viewBoard == futureBoard)    ui->planGraphicsView->scene()->addItem(attack);
                    }
                    else    delete attack;

                    //Damage minions
                    selectedMinion->damagePlanningMinion(minion->getAttack());
                    minion->damagePlanningMinion(selectedMinion->getAttack());

                    selectedMinion->setExausted();
                    if(!selectedMinion->isHero())   updateMinionsAttack(true, futureBoard);
                    selectedMinion->selectMinion(false);
                    selectedMinion = NULL;
                }
            }
        }
    }
}


void PlanHandler::heroPress(HeroGraphicsItem* hero, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(hero->isDead())  return;

    if(futureBoard == NULL)
    {
        if(nowBoard->playerHero == hero && hero->getAttack()>0)
        {
            createFutureBoard();

            //Get hero
            hero = futureBoard->playerHero;
            selectedMinion = hero;
            selectedMinion->selectMinion();
        }
    }
    else
    {
        //Show addon
        if(mouseButton != Qt::LeftButton)
        {
            if(!selectedCode.isEmpty())
            {
                hero->addPlanningAddon(selectedCode, Addon::AddonNeutral);
            }
        }
        else
        {
            //Select hero
            if(selectedMinion == NULL)
            {
                if(futureBoard->playerHero == hero && hero->getAttack()>0)
                {
                    selectedCode = "";
                    selectedMinion = hero;
                    selectedMinion->selectMinion();
                }
            }
            //Show attack
            else
            {
                if(futureBoard->playerHero == hero && hero->getAttack()>0)
                {
                    selectedCode = "";

                    //Deselect minion
                    if(selectedMinion == hero)
                    {
                        selectedMinion->selectMinion(false);
                        selectedMinion = NULL;
                    }
                    else
                    {
                        selectedMinion->selectMinion(false);
                        selectedMinion = hero;
                        selectedMinion->selectMinion();
                    }
                }
                else if(futureBoard->enemyHero == hero)
                {
                    //Show attack
                    ArrowGraphicsItem *attack = new ArrowGraphicsItem();
                    attack->setEnd(true, selectedMinion);//From
                    attack->setEnd(false, hero);//To
                    if(appendAttack(attack, futureBoard))
                    {
                        if(viewBoard == futureBoard)    ui->planGraphicsView->scene()->addItem(attack);
                    }
                    else    delete attack;

                    //Damage minions
                    hero->damagePlanningMinion(selectedMinion->getAttack());

                    selectedMinion->setExausted();
                    if(!selectedMinion->isHero())   updateMinionsAttack(true, futureBoard);
                    selectedMinion->selectMinion(false);
                    selectedMinion = NULL;
                }
            }
        }
    }
}


void PlanHandler::minionWheel(MinionGraphicsItem* minion, bool up)
{
    if(!selectedCode.isEmpty())
    {
        minion->addPlanningAddon(selectedCode, (up?Addon::AddonLife:Addon::AddonDamage));
    }
}


void PlanHandler::heroWheel(HeroGraphicsItem* hero, bool up)
{
    if(!selectedCode.isEmpty())
    {
        hero->addPlanningAddon(selectedCode, (up?Addon::AddonLife:Addon::AddonDamage));
    }
}


void PlanHandler::showCardTooltip(QString code, QRect rectCard, int maxTop, int maxBottom)
{
    //Avoid card tooltips in future board
    if(viewBoard != futureBoard)    emit cardEntered(code, rectCard, maxTop, maxBottom);
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

    while(!board->arrows.empty())
    {
        ArrowGraphicsItem* arrow = board->arrows.takeFirst();
        delete arrow;
    }

    while(!board->playerHandList.empty())
    {
        CardGraphicsItem* card = board->playerHandList.takeFirst();
        delete card;
    }

    while(!board->enemyHandList.empty())
    {
        CardGraphicsItem* card = board->enemyHandList.takeFirst();
        delete card;
    }

    if(board->playerHero != NULL)  removeHero(true, board);
    if(board->enemyHero != NULL)   removeHero(false, board);

    if(board->playerHeroPower != NULL)  removeHeroPower(true, board);
    if(board->enemyHeroPower != NULL)   removeHeroPower(false, board);

    if(board->playerWeapon != NULL) removeWeapon(true, -1, board);
    if(board->enemyWeapon != NULL)  removeWeapon(false, -1, board);
}


void PlanHandler::reset()
{
    emit pDebug("Clear all boards.");
    ui->planGraphicsView->reset();
    pendingTagChanges.clear();
    this->lastMinionAdded = NULL;
    this->viewBoard = nowBoard;
    this->firstStoredTurn = 0;
    this->nowBoard->playerTurn = true;
    setLastTriggerId("", "", -1, -1);

    if(futureBoard != NULL)     deleteFutureBoard();
    resetBoard(nowBoard);
    while(!turnBoards.empty())
    {
        Board *board = turnBoards.takeFirst();
        resetBoard(board);
        delete board;
    }

    updateTurnSliderRange();

    ui->planButtonLast->setEnabled(false);
    ui->planButtonNext->setEnabled(false);
    ui->planButtonPrev->setEnabled(false);
    ui->planButtonFirst->setEnabled(false);
}


void PlanHandler::deleteFutureBoard()
{
    resetBoard(futureBoard);
    delete futureBoard;
    futureBoard = NULL;
    selectedMinion = NULL;
    selectedCode = "";
    ui->planButtonLast->setIcon(QIcon(":Images/planLast.png"));
}


void PlanHandler::updateTurnSliderRange()
{
    //Despues de un reset min/max son 0. Al definir el primer turno se definira el min a firstStoredTurn.
    //Esto cambiara el value y cargara al primer turno. Queremos que se siga mostrando nowBoard.
    bool showNowBoard = false;
    if(ui->planTurnSlider->maximum() == 0)  showNowBoard = true;
    ui->planTurnSlider->setMinimum(firstStoredTurn);
    ui->planTurnSlider->setMaximum(firstStoredTurn + turnBoards.count());
    if(showNowBoard)    showLastTurn();
}


void PlanHandler::updateTurnLabel()
{
    int viewTurn = viewBoard->numTurn;

    QString color;
    if(viewBoard->playerTurn)    color = "#008000";
    else                        color = "#8B0000";
    ui->planLabelTurn->setStyleSheet("QLabel {background-color: transparent; color: " + color + ";}");

    if(viewBoard == nowBoard)           ui->planLabelTurn->setText("--");
    else if(viewBoard == futureBoard)   ui->planLabelTurn->setText("++");
    else                                ui->planLabelTurn->setText("T" + QString::number((viewTurn+1)/2));
}


void PlanHandler::showSliderTurn(int turn)
{
    int lastTurn = firstStoredTurn + turnBoards.count() - 1;
    if(turn > lastTurn)     viewBoard = nowBoard;
    else                    viewBoard = turnBoards[turn-firstStoredTurn];

    loadViewBoard();

    bool prevEnabled = ui->planTurnSlider->minimum() != turn;
    bool nextEnabled = ui->planTurnSlider->maximum() != turn;
    ui->planButtonLast->setEnabled(nextEnabled);
    ui->planButtonNext->setEnabled(nextEnabled);
    ui->planButtonPrev->setEnabled(prevEnabled);
    ui->planButtonFirst->setEnabled(prevEnabled);

    if(futureBoard != NULL)     deleteFutureBoard();
}


void PlanHandler::showFirstTurn()
{
    ui->planTurnSlider->setValue(firstStoredTurn);
}


void PlanHandler::showLastTurn()
{
    if(ui->planTurnSlider->value() == firstStoredTurn + turnBoards.count())
    {
        showSliderTurn(firstStoredTurn + turnBoards.count());
    }
    else
    {
        ui->planTurnSlider->setValue(firstStoredTurn + turnBoards.count());
    }
}


void PlanHandler::showPrevTurn()
{
    ui->planTurnSlider->setValue(ui->planTurnSlider->value()-1);
}


void PlanHandler::showNextTurn()
{
    ui->planTurnSlider->setValue(ui->planTurnSlider->value()+1);
}


void PlanHandler::loadViewBoard()
{
    ui->planGraphicsView->removeAll();
    ui->planGraphicsView->updateView(std::max(viewBoard->playerMinions.count(), viewBoard->enemyMinions.count()));
    updateViewCardZoneSpots();

    foreach(MinionGraphicsItem *minion, viewBoard->playerMinions)
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    foreach(MinionGraphicsItem *minion, viewBoard->enemyMinions)
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    foreach(ArrowGraphicsItem *arrow, viewBoard->arrows)
    {
        ui->planGraphicsView->scene()->addItem(arrow);
    }

    foreach(CardGraphicsItem *card, viewBoard->playerHandList)
    {
        ui->planGraphicsView->scene()->addItem(card);
    }

    foreach(CardGraphicsItem *card, viewBoard->enemyHandList)
    {
        ui->planGraphicsView->scene()->addItem(card);
    }

    if(viewBoard->playerHeroPower != NULL)  ui->planGraphicsView->scene()->addItem(viewBoard->playerHeroPower);
    if(viewBoard->enemyHeroPower != NULL)   ui->planGraphicsView->scene()->addItem(viewBoard->enemyHeroPower);

    if(viewBoard->playerWeapon != NULL) ui->planGraphicsView->scene()->addItem(viewBoard->playerWeapon);
    if(viewBoard->enemyWeapon != NULL)  ui->planGraphicsView->scene()->addItem(viewBoard->enemyWeapon);

    if(viewBoard->playerHero !=NULL)    ui->planGraphicsView->scene()->addItem(viewBoard->playerHero);
    if(viewBoard->enemyHero !=NULL)     ui->planGraphicsView->scene()->addItem(viewBoard->enemyHero);

    updateTurnLabel();
}


void PlanHandler::lockPlanInterface()
{
    emit pDebug("Lock plan interface.");

    this->inGame = true;
    updateTransparency();

    reset();
}


void PlanHandler::unlockPlanInterface()
{
    emit pDebug("Unlock plan interface.");

    this->inGame = false;
    updateTransparency();

    if(!turnBoards.isEmpty())   showFirstTurn();
}


void PlanHandler::addHeroDeadToLastTurn(bool playerWon)
{
    if(playerWon)
    {
        if(nowBoard->enemyHero!=NULL)  nowBoard->enemyHero->setDead(true);
    }
    else
    {
        if(nowBoard->playerHero!=NULL) nowBoard->playerHero->setDead(true);
    }

    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();

    if(playerWon)
    {
        if(board->enemyHero!=NULL)  board->enemyHero->setDead(true);
    }
    else
    {
        if(board->playerHero!=NULL) board->playerHero->setDead(true);
    }
}


bool PlanHandler::getWinner()
{
    if(nowBoard->playerHero != NULL && nowBoard->playerHero->getRemainingHealth()<=0)   return false;
    if(nowBoard->enemyHero != NULL && nowBoard->enemyHero->getRemainingHealth()<=0)     return true;
    return !nowBoard->playerTurn;
}


void PlanHandler::endGame(bool playerWon, bool playerUnknown)
{
    if(playerUnknown)   playerWon = getWinner();
    addHeroDeadToLastTurn(playerWon);
    unlockPlanInterface();
}


bool PlanHandler::isSizePlan()
{
    return sizePlan;
}


bool PlanHandler::resetSizePlan()
{
    bool oldSizePlan = sizePlan;
    sizePlan = false;
    return oldSizePlan;
}


void PlanHandler::resizePlan(bool toggleSizePlan)
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    MainWindow *mainWindow = ((MainWindow*)parent());
    QSize newSize;
    if(toggleSizePlan)      sizePlan = !sizePlan;

    if(!sizePlan)
    {
        settings.setValue("sizePlan", mainWindow->size());
        newSize = settings.value("size", QSize(350, 400)).toSize();
    }
    else
    {
        settings.setValue("size", mainWindow->size());
        newSize = settings.value("sizePlan", QSize(400, 400)).toSize();
    }

    mainWindow->resize(newSize);
}


void PlanHandler::updateTransparency()
{
    bool inTabPlan = ui->tabWidget->currentWidget() == ui->tabPlan;

    if(!mouseInApp && transparency==Transparent)
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


void PlanHandler::resetDeadProbs()
{
    if(nowBoard->playerHero != NULL)    nowBoard->playerHero->setDeadProb();
    if(nowBoard->enemyHero != NULL)     nowBoard->enemyHero->setDeadProb();

    foreach(MinionGraphicsItem *minion, *getMinionList(true))    minion->setDeadProb();
    foreach(MinionGraphicsItem *minion, *getMinionList(false))   minion->setDeadProb();

    if(futureBombs != NULL)
    {
        delete futureBombs;
        futureBombs = NULL;
    }
}


void PlanHandler::checkBomb(QString code)
{
    if(code.isEmpty() || viewBoard!=nowBoard || futureBombs != NULL)    return;

    bool playerIn;
    int missiles;
    if(!isCardBomb(code, playerIn, missiles))   return;

    //Targets
    if(nowBoard->enemyHero == NULL)             return;
    HeroGraphicsItem *enemyHero = nowBoard->enemyHero;
    QList<MinionGraphicsItem *> *enemyMinions = getMinionList(false);

    HeroGraphicsItem *playerHero = NULL;
    QList<MinionGraphicsItem *> *playerMinions = NULL;
    if(playerIn)
    {
        if(nowBoard->playerHero == NULL)        return;
        playerHero = nowBoard->playerHero;
        playerMinions = getMinionList(true);
    }

    //Targets List
    QList<int> targets;
    targets.append(enemyHero->getHitsToDie());
    foreach(MinionGraphicsItem *minion, *enemyMinions)          targets.append(minion->getHitsToDie());

    if(playerIn)
    {
        targets.append(playerHero->getHitsToDie());
        foreach(MinionGraphicsItem *minion, *playerMinions)     targets.append(minion->getHitsToDie());
    }

    //Get dead probs
    futureBombs = new QFuture<QList<float> >(QtConcurrent::run(this, &PlanHandler::bombDeads, targets, missiles));
    QTimer::singleShot(10, this, SLOT(setDeadProbs()));
}


void PlanHandler::setDeadProbs()
{
    if(futureBombs == NULL)     return;

    if(futureBombs->isFinished())
    {
        QList<float> deadProbs = futureBombs->result();
        delete futureBombs;
        futureBombs = NULL;

        HeroGraphicsItem *enemyHero = nowBoard->enemyHero;
        QList<MinionGraphicsItem *> *enemyMinions = getMinionList(false);
        enemyHero->setDeadProb(deadProbs.takeFirst());
        foreach(MinionGraphicsItem *minion, *enemyMinions)          minion->setDeadProb(deadProbs.takeFirst());

        if(!deadProbs.isEmpty())
        {
            HeroGraphicsItem *playerHero = nowBoard->playerHero;
            QList<MinionGraphicsItem *> *playerMinions = getMinionList(true);
            playerHero->setDeadProb(deadProbs.takeFirst());
            foreach(MinionGraphicsItem *minion, *playerMinions)     minion->setDeadProb(deadProbs.takeFirst());
        }
    }
    else
    {
        QTimer::singleShot(100, this, SLOT(setDeadProbs()));
    }
}


QList<float> PlanHandler::bombDeads(QList<int> targets, int missiles)
{
    QMap<QString, float> states;
    states[encodeBombState(targets)] = 1;
    for(int i=0; i<missiles; i++)           states = bomb(states);

    QList<float> deadProbs;
    for(int i=0; i<targets.count(); i++)    deadProbs.append(0);

    foreach(QString state, states.keys())
    {
        float prob = states[state];
        QList<int> targets = decodeBombState(state);
        for(int i=0; i<targets.count(); i++)
        {
            if(targets[i] == 0)     deadProbs[i] += prob;
        }
    }

    return deadProbs;
}


QList<int> PlanHandler::decodeBombState(QString state)
{
    QList<int> targets;
    foreach(QString targetString, state.split(":"))  targets.append(targetString.toInt());
    return targets;
}


QString PlanHandler::encodeBombState(QList<int> targets)
{
    QStringList targetsString;
    foreach(int target, targets)      targetsString.append(QString::number(target));
    return targetsString.join(":");
}


QMap<QString, float> PlanHandler::bomb(QMap<QString, float> &oldStates)
{
    QMap<QString, float> newStates;

    foreach(QString oldState, oldStates.keys())
    {
        float oldProb = oldStates[oldState];
        QList<int> oldTargets = decodeBombState(oldState);

        int livingTargets = 0;
        for(int i=0; i<oldTargets.count(); i++)
        {
            if(oldTargets[i] > 0)   livingTargets++;
        }

        //Apply an attack to each living targets
        for(int i=0; i<oldTargets.count(); i++)
        {
            if(oldTargets[i] > 0)
            {
                QList<int> newTargets(oldTargets);
                newTargets[i]--;
                QString newState = encodeBombState(newTargets);

                //Update probabilities
                float addProb = newStates.contains(newState)?newStates[newState]:0;
                newStates[newState] = addProb + oldProb/livingTargets;
            }
        }
    }

    return newStates;
}


int PlanHandler::flamewakersOnBoard()
{
    int num = 0;
    QList<MinionGraphicsItem *> *playerMinions = getMinionList(true);
    foreach(MinionGraphicsItem *minion, *playerMinions)
    {
        if(minion->getCode() == FLAMEWAKER)     num++;
    }
    return num;
}


bool PlanHandler::isMechOnBoard()
{
    QList<MinionGraphicsItem *> *playerMinions = getMinionList(true);
    foreach(MinionGraphicsItem *minion, *playerMinions)
    {
        if(DeckCard(minion->getCode()).getRace() == MECHANICAL)     return true;
    }
    return false;
}


//Card bombs
bool PlanHandler::isCardBomb(QString code)
{
    if((code == MAD_BOMBER) || (code == MADDER_BOMBER) || (code == SPREADING_MADNESS) ||
            (code == ARCANE_MISSILES) || (code == AVENGING_WRATH) || (code == GOBLIN_BLASTMAGE))
    {
        return true;
    }
    return false;
}


bool PlanHandler::isCardBomb(QString code, bool &playerIn, int &missiles)
{
    missiles = 0;
    playerIn = false;

    if(code == MAD_BOMBER)
    {
        missiles = 3;
        playerIn = true;
    }
    else if(code == MADDER_BOMBER)
    {
        missiles = 6;
        playerIn = true;
    }
    else if(code == SPREADING_MADNESS)
    {
        missiles = 9;
        playerIn = true;
    }
    else if(code == ARCANE_MISSILES)
    {
        missiles = 3;
        playerIn = false;
    }
    else if(code == AVENGING_WRATH)
    {
        missiles = 8;
        playerIn = false;
    }
    else if(code == GOBLIN_BLASTMAGE && isMechOnBoard())
    {
        missiles = 4;
        playerIn = false;
    }

    if(DeckCard(code).getType() == SPELL)
    {
        //SpellDamage
        HeroGraphicsItem *hero = getHero(true, NULL);
        if(hero != NULL)            missiles += hero->getSpellDamage();

        //Flamewakers, evitamos con SPREADING_MADNESS (!playerIn)
        if(!playerIn)
        {
            int flamewakers = flamewakersOnBoard();
            if(flamewakers > 0)     missiles += flamewakers * 2;
        }
    }

    if(missiles > 0)    return true;
    else                return false;
}


//Card exceptions
bool PlanHandler::isAddonCommonValid(QString code)
{
    if(code == IMP_GANG_BOSS)           return false;
    if(code == DRAGON_EGG)              return false;
    if(code == ACOLYTE_OF_PAIN)         return false;
    if(code == GURUBASHI_BERSERKER)     return false;
    if(code == FROTHING_BERSEKER)       return false;
    if(code == LIGHTWARDEN)             return false;
    if(code == GOREHOWL)                return false;
    if(code == LOREWALKER_CHO)          return false;
    if(code == NERUBIAN_PROPHET)        return false;
    if(code == SNAKE_TRAP)              return false;
    if(code == ARMORED_WARHORSE)        return false;
    if(code == DARKSHIRE_COUNCILMAN)    return false;
    if(code == POLLUTED_HOARDER)        return false;
    return true;
}


bool PlanHandler::isAddonHeroValid(QString code)
{
    if(code == ACIDMAW)                 return false;
    if(code == SIEGE_ENGINE)            return false;
    return isAddonCommonValid(code);
}


bool PlanHandler::isAddonMinionValid(QString code)
{
    if(code == WRATHGUARD)              return false;
    if(code == AXE_FLINGER)             return false;
    if(code == ARMORSMITH)              return false;
    if(code == EYE_FOR_AN_EYE)          return false;
    if(code == TRUESILVER_CHAMPION)     return false;
    if(code == GLADIATORS_LONGBOW)      return false;
    return isAddonCommonValid(code);
}


bool PlanHandler::isLastTriggerValid(QString code)
{
    if(code == KNIFE_JUGGLER)           return false;
    if(code == ADDLED_GRIZZLY)          return false;
    if(code == DARKSHIRE_COUNCILMAN)    return false;
    if(code == FROTHING_BERSEKER)       return false;
    if(code == THE_SKELETON_KNIGHT)     return false;
    if(code == SWORD_OF_JUSTICE)        return false;
    return true;
}
