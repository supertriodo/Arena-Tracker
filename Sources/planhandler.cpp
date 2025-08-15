#include "planhandler.h"
#include "themehandler.h"
#include "Synergies/mechaniccounter.h"
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
    this->nowBoard->playerHero = nullptr;
    this->nowBoard->enemyHero = nullptr;
    this->nowBoard->numTurn = 0;
    this->viewBoard = nullptr;
    this->futureBoard = nullptr;
    this->selectedMinion = nullptr;
    this->selectedCode = "";
    reset();
    completeUI();
    createGraphicsItemSender();

    connect(&futureBombs, SIGNAL(finished()), this, SLOT(setDeadProbs()));
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
}


void PlanHandler::completeUI()
{
    planPatreonButton = new QPushButton(ui->tabPlan);
    planPatreonButton->setFlat(true);
    planPatreonButton->setIcon(QIcon(":/Images/becomePatreon.png"));
    planPatreonButton->setIconSize(QSize(217, 51));
    planPatreonButton->setToolTip("Unlock Replays and Planning becoming a patron (3€)");

    ui->verticalLayoutPlan->insertWidget(0, planPatreonButton);

    connect(planPatreonButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));

    setPremium(false);

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


void PlanHandler::setPremium(bool premium)
{
    if(premium)
    {
        ui->planGraphicsView->show();
        ui->planButtonPrev->show();
        ui->planButtonNext->show();
        ui->planButtonFirst->show();
        ui->planButtonLast->show();
        ui->planButtonResize->show();
        ui->planTurnSlider->show();
        ui->planLabelTurn->show();

        this->planPatreonButton->hide();
    }
    else
    {
        ui->planGraphicsView->hide();
        ui->planButtonPrev->hide();
        ui->planButtonNext->hide();
        ui->planButtonFirst->hide();
        ui->planButtonLast->hide();
        ui->planButtonResize->hide();
        ui->planTurnSlider->hide();
        ui->planLabelTurn->hide();

        this->planPatreonButton->show();
    }
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
    connect(graphicsItemSender, SIGNAL(secretEntered(int,QRect&,int,int)),
            this, SIGNAL(secretEntered(int,QRect&,int,int)));
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

    //We can have 8 minions during a magnetize for some seconds
    if(getMinionList(friendly)->count() > 7)
    {
        qDebug()<<"AVOID - NUM MINIONS = " << getMinionList(friendly)->count();
        return;
    }

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);
    updateMinionFromCard(minion);
    addMinion(friendly, minion, pos);
    emit checkCardImage(code, false);

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addMinionTagChange(tagChange, minion);
    }
    pendingTagChanges.remove(id);
}


void PlanHandler::updateMinionFromCard(MinionGraphicsItem * minion)
{
    CardGraphicsItem * card = findCard(minion->isFriendly(), minion->getId());
    if(card == nullptr)    emit pDebug("Minion not found in hand when ckecking its stats. Id: " + QString::number(minion->getId()), Warning);
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
    if(pos < 0)                     pos = 0;
    if(pos > minionsList->size())   pos = minionsList->size();
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
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(friendly, board))
    {
        minionsAttack += minion->getPotencialDamage(false);
        if(board == nullptr)   minionsMaxAttack += minion->getPotencialDamage(true);
    }

    HeroGraphicsItem *hero = getHero(friendly, board);
    if(hero != nullptr)
    {
        hero->setMinionsAttack(minionsAttack, board == nullptr);
        if(board == nullptr)   hero->setMinionsMaxAttack(minionsMaxAttack);
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
    qDebug()<<"NEW MINION TRIGGERED --> id"<<id<<"pos"<<pos;

    if(getMinionList(friendly)->count() >= 7)
    {
        qDebug()<<"AVOID - NUM MINIONS = " << getMinionList(friendly)->count();
        return;
    }

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn, graphicsItemSender);
    addMinion(friendly, minion, pos);
    emit checkCardImage(code, false);

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addMinionTagChange(tagChange, minion);
    }
    pendingTagChanges.remove(id);

    if(this->lastTriggerId!=-1)         copyMinionToLastTurn(friendly, minion);
    else                                emit pDebug("Triggered minion creator not set.");
}


void PlanHandler::copyMinionToLastTurn(bool friendly, MinionGraphicsItem *minion, int idCreator)
{
    if(turnBoards.empty())  return;

    MinionGraphicsItem *triggerMinion = nullptr;
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
            if(hero != nullptr && hero->isYourSecret(idCreator))
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
    if(triggerMinion != nullptr)
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
    if(minion == nullptr)  emit pDebug("Remove minion not found. Id: " + QString::number(id), Warning);
    else                delete minion;
}


MinionGraphicsItem * PlanHandler::takeMinion(bool friendly, int id, bool stolen)
{
    this->lastMinionAdded = nullptr;

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    int pos = findMinionPos(minionsList, id);
    if(pos == -1)   return nullptr;

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
    if(minion == nullptr)  emit pDebug("Steal minion not found. Id: " + QString::number(id));
    else
    {
        addMinion(!friendly, minion, pos);
        minion->changeZone();

        //Engrave roba el esbirro en el log, y luego lo mata, lo evitamos. Usamos lastPowerAddon.code pq lastTrigger no guarda code.
        if(Utility::codeEqConstant(this->lastPowerAddon.code, ENGRAVE) ||
            Utility::codeEqConstant(this->lastPowerAddon.code, PSYCHIC_SCREAM))
        {
            emit pDebug("Avoid steal minion by Engrave/Psychic scream.");
        }
        else
        {
            copyMinionToLastTurn(!friendly, minion, -id);
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

    if(hero != nullptr)
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
    if(code.startsWith("HERO_"))    emit checkCardImage(code, true);
    else                            emit checkCardImage("HERO_" + code, true);

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addHeroTagChange(tagChange);
    }
    pendingTagChanges.remove(id);
}


void PlanHandler::removeHero(bool friendly, Board *board)
{
    if(board == nullptr)   board = nowBoard;
    HeroGraphicsItem* hero = friendly?board->playerHero:board->enemyHero;
    if(hero == nullptr)
    {
        emit pDebug("Remove hero NULL.", Warning);
    }
    else
    {
        delete hero;

        if(friendly)    board->playerHero = nullptr;
        else            board->enemyHero = nullptr;
    }
}


void PlanHandler::updateMinionZoneSpots(bool friendly, Board *board)
{
    if(board == nullptr)   board = nowBoard;
    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly, board);
    for(int i=0; i<minionsList->count(); i++)
    {
        minionsList->at(i)->setZonePos(friendly, i, minionsList->count());
    }

    if(viewBoard == board)
    {
        for(ArrowGraphicsItem *arrow: qAsConst(board->arrows))
        {
            arrow->prepareGeometryChange();
            arrow->update();
        }
    }
}


QList<MinionGraphicsItem *> * PlanHandler::getMinionList(bool friendly, Board *board)
{
    if(board == nullptr)   board = nowBoard;
    if(friendly)    return &board->playerMinions;
    else            return &board->enemyMinions;
}


HeroGraphicsItem * PlanHandler::getHero(bool friendly, Board *board)
{
    if(board == nullptr)   board = nowBoard;
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
    if(pos == -1)   return nullptr;
    else            return minionsList->at(pos);
}


bool PlanHandler::isMinionOnBoard(bool friendly, int id, Board *board)
{
    return findMinion(friendly, id, board) != nullptr;
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
        else if(pos >= minionsList->count() || pos < 0)
        {
            emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos) +
                        ". Minion ouside list bounds. Count: " + QString::number(minionsList->count()), DebugLevel::Error);
        }
        else
        {
            emit pDebug((friendly?QString("Player"):QString("Enemy")) + ": POSITION(" + QString::number(id) + ")=" +QString::number(pos));
            minionsList->move(oldPos, pos);
            updateMinionZoneSpots(friendly);
        }
    }
    this->lastMinionAdded = nullptr;
}


bool PlanHandler::isLastMinionAddedValid()
{
    if(this->lastMinionAdded == nullptr)   return false;

    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((now - this->lastMinionAddedTime) > 1000)
    {
        emit pDebug("POSITION(" + QString::number(this->lastMinionAdded->getId()) + ") Avoid OLD.");
        this->lastMinionAdded = nullptr;
        return false;
    }
    return true;
}


void PlanHandler::cardTagChangePrevTurn(int id, bool friendly, QString tag, QString value)
{
    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();
    if(board->playerTurn != friendly)   return;

    CardGraphicsItem *card = findCard(friendly, id, board);
    if(card != nullptr)
    {
        if(tag == "COST")   card->reduceCost(value.toInt());
        else                card->processTagChange(tag, value);
    }
}


void PlanHandler::playerBoardTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->playerHero != nullptr && nowBoard->playerHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->playerHero->getId(), Addon::AddonNeutral);
        addHero(true, code, id);
    }
    else    addBoardTagChange(id, true, tag, value);
}


void PlanHandler::enemyBoardTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->enemyHero != nullptr && nowBoard->enemyHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->enemyHero->getId(), Addon::AddonNeutral);
        addHero(false, code, id);
    }
    else    addBoardTagChange(id, false, tag, value);
}


bool PlanHandler::updateInPendingTagChange(int id, QString tag, QString value)
{
    for(QMap<int,TagChange>::iterator it = pendingTagChanges.begin(); it != pendingTagChanges.end(); it++)
    {
        if(it->id == id && it->tag == tag)
        {
            emit pDebug("Mapped Tag Change updated: Id: " + QString::number(id) + " - " + tag + " --> " + value);
            it->value = value;
            return true;
        }
    }
    return false;
}


void PlanHandler::addCardTagChange(const TagChange &tagChange, CardGraphicsItem *card)
{
    emit pDebug("Tag Change Card: Id: " + QString::number(tagChange.id) + " - " + tagChange.tag + " --> " + tagChange.value);
    card->processTagChange(tagChange.tag, tagChange.value);
    cardTagChangePrevTurn(tagChange.id, tagChange.friendly, tagChange.tag, tagChange.value);

    if(tagChange.friendly && tagChange.tag == "COST")
    {
        showManaPlayableCardsAuto();
    }
}


void PlanHandler::addMinionTagChange(const TagChange &tagChange, MinionGraphicsItem * minion)
{
    emit pDebug("Tag Change Minion: Id: " + QString::number(tagChange.id) + " - " + tagChange.tag + " --> " + tagChange.value);

    //Usamos el tag change de reborn a 0 para marcar el lastTriggerId y asi mostrar flecha de reborn a reborn en replay.
    if(tagChange.tag == "REBORN")
    {
        if(tagChange.value == "0")
        {
            emit pDebug("REBORN lastTriggerId = " + QString::number(tagChange.id));
            this->lastTriggerId = tagChange.id;
        }
        return;
    }

    checkAtkHealthChange(minion, tagChange.friendly, tagChange.tag, tagChange.value);
    bool healing = minion->processTagChange(tagChange.tag, tagChange.value);
    bool isDead = minion->isDead();
    bool isHero = false;
    if(tagChange.tag == "ATK" || tagChange.tag == "EXHAUSTED" || tagChange.tag == "WINDFURY" || tagChange.tag == "FROZEN" ||
        tagChange.tag == "RUSH" || tagChange.tag == "DORMANT" || tagChange.tag == "LAUNCHPAD" || tagChange.tag == "LOCATION_ACTION_COOLDOWN")
    {
        updateMinionsAttack(tagChange.friendly);
    }

    addAddonTagChange(tagChange, healing, isDead, isHero);
}


void PlanHandler::addHeroTagChange(const TagChange &tagChange)
{
    HeroGraphicsItem* hero = (tagChange.friendly?nowBoard->playerHero:nowBoard->enemyHero);

    emit pDebug("Tag Change " + QString(tagChange.friendly?"Player":"Enemy") + " Hero: Id: " +
                QString::number(tagChange.id) + " - " + tagChange.tag + " --> " + tagChange.value);
    bool healing = hero->processTagChange(tagChange.tag, tagChange.value);
    bool isDead = hero->isDead();
    bool isHero = true;

    addAddonTagChange(tagChange, healing, isDead, isHero);
}


void PlanHandler::addHeroPowerTagChange(const TagChange &tagChange)
{
    HeroPowerGraphicsItem* heroPower = (tagChange.friendly?nowBoard->playerHeroPower:nowBoard->enemyHeroPower);

    emit pDebug("Tag Change " + QString(tagChange.friendly?"Player":"Enemy") + " Hero Power: Id: " +
                QString::number(tagChange.id) + " - " + tagChange.tag + " --> " + tagChange.value);
    heroPower->processTagChange(tagChange.tag, tagChange.value);

    if(tagChange.tag == "EXHAUSTED" && tagChange.value == "1" && !turnBoards.empty())
    {
        if(tagChange.friendly)  turnBoards.last()->playerHeroPower->processTagChange(tagChange.tag, tagChange.value);
        else                    turnBoards.last()->enemyHeroPower->processTagChange(tagChange.tag, tagChange.value);
    }
}


void PlanHandler::addWeaponTagChange(const TagChange &tagChange)
{
    WeaponGraphicsItem* weapon = (tagChange.friendly?nowBoard->playerWeapon:nowBoard->enemyWeapon);

    emit pDebug("Tag Change " + QString(tagChange.friendly?"Player":"Enemy") + " Weapon: Id: " +
                QString::number(tagChange.id) + " - " + tagChange.tag + " --> " + tagChange.value);
    weapon->processTagChange(tagChange.tag, tagChange.value);
}


void PlanHandler::addAddonTagChange(const TagChange &tagChange, bool healing, bool isDead, bool isHero)
{
    if(!isDead && isLastPowerAddonValid(tagChange.tag, tagChange.value, tagChange.id, tagChange.friendly, isHero, healing))
    {
        if(tagChange.tag == "DAMAGE" || tagChange.tag == "ARMOR" || tagChange.tag == "CONTROLLER" || tagChange.tag == "TO_BE_DESTROYED" ||
            tagChange.tag == "SHOULDEXITCOMBAT" || (tagChange.tag == "DIVINE_SHIELD" && tagChange.value == "0"))
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, tagChange.id, healing?Addon::AddonLife:Addon::AddonDamage);

            //Evita que un efecto que quita la armadura y hace algo de damage aparezca 2 veces
            if(isHero && tagChange.tag == "ARMOR" && tagChange.value == "0")
            {
                this->lastArmorRemoverIds.idAddon = this->lastPowerAddon.id;
                this->lastArmorRemoverIds.idHero = tagChange.id;
                emit pDebug("Last armor remover set.");
            }
        }
        else if(
                    tagChange.tag == "ATK" || tagChange.tag == "HEALTH" || tagChange.tag == "ZONE" ||
                    tagChange.tag == "DIVINE_SHIELD" || tagChange.tag == "STEALTH" || tagChange.tag == "TAUNT" ||
                    tagChange.tag == "CHARGE" || tagChange.tag == "RUSH" || tagChange.tag == "FROZEN" || tagChange.tag == "WINDFURY" ||
                    tagChange.tag == "SILENCED" || tagChange.tag == "AURA" || tagChange.tag == "CANT_BE_DAMAGED"
               )
        {
            addAddonToLastTurn(this->lastPowerAddon.code, this->lastPowerAddon.id, tagChange.id, Addon::AddonNeutral);
        }
    }
}


void PlanHandler::addBoardTagChange(int id, bool friendly, QString tag, QString value)
{
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    //Update if is in pendingTagChanges
    if(updateInPendingTagChange(id, tag, value)) return;

    //Cards
    CardGraphicsItem *card = findCard(friendly, id);
    if(card != nullptr)
    {
        if(tag == "COST" || tag == "ATK" || tag == "HEALTH")
        {
            addCardTagChange(tagChange, card);
            return;
        }
    }

    //Minions
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != nullptr && tag != "COST")
    {
        addMinionTagChange(tagChange, minion);
    }

    //Heroes
    else if(friendly && nowBoard->playerHero != nullptr && nowBoard->playerHero->getId() == id)
    {
        addHeroTagChange(tagChange);
    }
    else if(!friendly && nowBoard->enemyHero != nullptr && nowBoard->enemyHero->getId() == id)
    {
        addHeroTagChange(tagChange);
    }

    //Heroe Powers
    else if(friendly && nowBoard->playerHeroPower != nullptr && nowBoard->playerHeroPower->getId() == id)
    {
        addHeroPowerTagChange(tagChange);
    }
    else if(!friendly && nowBoard->enemyHeroPower != nullptr && nowBoard->enemyHeroPower->getId() == id)
    {
        addHeroPowerTagChange(tagChange);
    }

    //Weapons
    else if(friendly && nowBoard->playerWeapon != nullptr && nowBoard->playerWeapon->getId() == id)
    {
        addWeaponTagChange(tagChange);
    }
    else if(!friendly && nowBoard->enemyWeapon != nullptr && nowBoard->enemyWeapon->getId() == id)
    {
        addWeaponTagChange(tagChange);
    }

    //Pending tag change
    else
    {
        //Si un minion llega a la mano y es jugado rapido puede que reciba el ZONE = HAND y aparezca como muerto.
        if(tag == "ZONE")
        {
            emit pDebug("Zone Tag Change not appended: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        }
        //Usamos el tag change de reborn a 0 para marcar el lastTriggerId y asi mostrar flecha de reborn a reborn en replay.
        //Al aparecer el nuevo reborn tambien hay un tag change del nuevo, que podemos evitar si no hacemos pending tag change.
        else if(tag == "REBORN")
        {
            emit pDebug("Reborn Tag Change not appended: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        }
        else
        {
            pendingTagChanges.insertMulti(id, tagChange);
            emit pDebug("Tag Change Mapped: Id: " + QString::number(id) + " - " + tag + " --> " + value);
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
    if(Utility::getCardAttribute(this->lastPowerAddon.code, "type").toString() == "ENCHANTMENT")
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
    if(minion == nullptr || minion->isDead())  return;

    if(tag == "ATK")
    {
        if(turnBoards.empty())  return;

        MinionGraphicsItem * minionLastTurn = findMinion(friendly, minion->getId(), turnBoards.last());
        if(minionLastTurn == nullptr)  return;

        int attack = minion->getAttack();
        int newAttack = value.toInt();

        //Swaps pone ATK y HEALTH a 0 y luego los valores finales, lo que crea signos - confusos.
        if(attack == 0 || newAttack == 0)  return;

        if(newAttack > attack)          minionLastTurn->setChangeAttack(MinionGraphicsItem::ChangePositive);
        else if(newAttack < attack)     minionLastTurn->setChangeAttack(MinionGraphicsItem::ChangeNegative);
    }
    else if(tag == "HEALTH")
    {
        if(turnBoards.empty())  return;

        MinionGraphicsItem * minionLastTurn = findMinion(friendly, minion->getId(), turnBoards.last());
        if(minionLastTurn == nullptr)  return;

        int health = minion->getHealth();
        int newHealth = value.toInt();

        if(health == 0 || newHealth == 0)  return;

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
    HeroGraphicsItem *hero = getHero(friendly, nullptr);
    if(hero == nullptr)        return;

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
        if(friendly)
        {
            showManaPlayableCards(nowBoard);
            if(hero->getAvailableResources() == 0)  emit playerAllManaSpent();
        }
    }
    else if(tag == "CURRENT_SPELLPOWER")
    {
        hero->setSpellDamage(value.toInt());
    }
    else if(tag == "CORPSES")
    {
        hero->setCorpses(value.toInt());
    }
    else if(tag == "CORPSES_SPENT_THIS_GAME")
    {
        hero->setCorpsesUsed(value.toInt());
    }
}


bool PlanHandler::findAttackPoint(ArrowGraphicsItem *attack, bool isFrom, int id, Board *board)
{
    if(board->playerHero != nullptr && board->playerHero->getId() == id)
    {
        attack->setEnd(isFrom, board->playerHero);
    }
    else if(board->enemyHero != nullptr && board->enemyHero->getId() == id)
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

    for(ArrowGraphicsItem *arrow: qAsConst(board->arrows))
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

    if(weapon != nullptr)
    {
        emit pDebug("Trying to add a weapon with an existing one. Force remove old one.", Warning);
        removeWeapon(friendly);
    }

    weapon = new WeaponGraphicsItem(code, id, friendly, graphicsItemSender);
    updateWeaponFromCard(weapon);
    HeroGraphicsItem * heroNow = getHero(friendly);
    if(heroNow != nullptr)    heroNow->setHeroWeapon(weapon);

    if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->addItem(weapon);

    if(friendly)    nowBoard->playerWeapon = weapon;
    else            nowBoard->enemyWeapon = weapon;

    emit checkCardImage(code, false);
    setLastTriggerId("", "", -1, -1);

    //Add addon to last turn
    HeroGraphicsItem* heroLast = friendly?nowBoard->playerHero:nowBoard->enemyHero;
    if(heroLast != nullptr)    addAddonToLastTurn(code, id, heroLast->getId(), Addon::AddonNeutral);

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addWeaponTagChange(tagChange);
    }
    pendingTagChanges.remove(id);
}


void PlanHandler::updateWeaponFromCard(WeaponGraphicsItem * weapon)
{
    CardGraphicsItem * card = findCard(weapon->isFriendly(), weapon->getId());
    if(card == nullptr)    emit pDebug("Weapon not found in hand when ckecking its stats. Id: " + QString::number(weapon->getId()), Warning);
    else
    {
        weapon->updateStatsFromCard(card);
    }
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
    if(board == nullptr)   board = nowBoard;
    WeaponGraphicsItem* weapon = friendly?board->playerWeapon:board->enemyWeapon;
    if(weapon == nullptr)
    {
        emit pDebug("Trying to remove weapon NULL.", Warning);
    }
    else if(id == -1 || weapon->getId() == id)
    {
        HeroGraphicsItem * heroNow = getHero(friendly, board);
        if(heroNow != nullptr)    heroNow->setHeroWeapon();
        delete weapon;

        if(friendly)    board->playerWeapon = nullptr;
        else            board->enemyWeapon = nullptr;
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
    if(weapon == nullptr)
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
    qDebug()<<"NEW HERO POWER --> id"<<id<<code;
    //Avoid fancy Hero Powers codes
    if(code.endsWith("_H1"))    code = code.left(code.length()-3);

    HeroPowerGraphicsItem* heroPower = friendly?nowBoard->playerHeroPower:nowBoard->enemyHeroPower;

    if(heroPower != nullptr)
    {
        HeroGraphicsItem* hero = friendly?nowBoard->playerHero:nowBoard->enemyHero;
        if(hero != nullptr)    addAddonToLastTurn(code, id, hero->getId(), Addon::AddonNeutral);
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

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addHeroPowerTagChange(tagChange);
    }
    pendingTagChanges.remove(id);
}


void PlanHandler::removeHeroPower(bool friendly, Board *board)
{
    if(board == nullptr)   board = nowBoard;
    HeroPowerGraphicsItem* heroPower = friendly?board->playerHeroPower:board->enemyHeroPower;
    if(heroPower == nullptr)
    {
        emit pDebug("Trying to remove Hero Power NULL.", Warning);
    }
    else
    {
        delete heroPower;

        if(friendly)    board->playerHeroPower = nullptr;
        else            board->enemyHeroPower = nullptr;
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

    if(board->playerHero != nullptr && board->playerHero->getId() == id2)
    {
        addAddon(board->playerHero, code, id1, type, number);
    }
    else if(board->enemyHero != nullptr && board->enemyHero->getId() == id2)
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
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(friendly))
    {
        if(minion->isAura())    return true;
    }

    //Check last turn board
    if(turnBoards.empty())  return false;
    Board *board = turnBoards.last();
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(friendly, board))
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
    if(nowBoard->playerHero == nullptr) return;
    if(!Utility::isASecret(code))       return;
    QList<CardClass> secretClassList = Utility::getClassFromCode(code);
    CardClass secretClass;
    if(secretClassList.isEmpty())   secretClass = INVALID_CLASS;
    else                            secretClass = secretClassList.first();

    if(secretClassList.count() > 1)
    {
        for(const CardClass &cardClass: (const QList<CardClass>)nowBoard->playerHero->getCardClass())
        {
            if(secretClassList.contains(cardClass))
            {
                secretClass = cardClass;
                break;
            }
        }
    }
    nowBoard->playerHero->addSecret(id, secretClass);
}


void PlanHandler::enemySecretPlayed(int id, CardClass secretHero)
{
    if(nowBoard->enemyHero == nullptr) return;
    nowBoard->enemyHero->addSecret(id, secretHero);
}


void PlanHandler::playerSecretRevealed(int id, QString code)
{
    if(nowBoard->playerHero == nullptr) return;
    nowBoard->playerHero->removeSecret(id);

    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    board->playerHero->showSecret(id, code);
}


void PlanHandler::enemySecretRevealed(int id, QString code)
{
    if(nowBoard->enemyHero == nullptr) return;
    nowBoard->enemyHero->removeSecret(id);

    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    board->enemyHero->showSecret(id, code);

    revealEnemyCardPrevTurns(id, code);
}


void PlanHandler::enemyIsolatedSecret(int id, QString code)
{
    if(nowBoard->enemyHero == nullptr) return;
    nowBoard->enemyHero->showSecret(id, code);
}


void PlanHandler::playerSecretStolen(int id, QString code)
{
    if(nowBoard->enemyHero == nullptr) return;
    CardClass secretHero = nowBoard->enemyHero->getSecretHero(id);
    enemySecretRevealed(id, code);
    if(nowBoard->playerHero != nullptr)    nowBoard->playerHero->addSecret(id, secretHero);
}


void PlanHandler::enemySecretStolen(int id, QString code)
{
    if(nowBoard->playerHero == nullptr) return;
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
    if(board == nullptr)   board = nowBoard;
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
    if(board == nullptr)   board = nowBoard;
    if(friendly)    return &board->playerHandList;
    else            return &board->enemyHandList;
}


CardGraphicsItem * PlanHandler::findCard(bool friendly, int id, Board *board)
{
    QList<CardGraphicsItem *> * handList = getHandList(friendly, board);
    int pos = findCardPos(handList, id);
    if(pos == -1)   return nullptr;
    else            return handList->at(pos);
}


bool PlanHandler::isRecentCard(int id)
{
    if(turnBoards.isEmpty())    return false;
    Board *board = turnBoards.last();
    CardGraphicsItem *card = findCard(true, id, board);
    if(card == nullptr)         return false;
    return (card->numTurn() == board->numTurn);
}


void PlanHandler::revealEnemyCard(int id, QString code)
{
    CardGraphicsItem *card = findCard(false, id);
    if(card != nullptr)
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
    QStringList mechanics;
    if(!code.isEmpty())
    {
        if(MechanicCounter::isAoeGen(code, ""))                                                  mechanics += "aoeMechanic.png";
        if(MechanicCounter::isDestroyGen(code, QJsonArray(), ""))                                mechanics += "destroyMechanic.png";
        if(MechanicCounter::isDamageMinionsGen(code, QJsonArray(), QJsonArray(), "", MINION, 0)) mechanics += "damageMechanic.png";
    }

    CardGraphicsItem *card = new CardGraphicsItem(id, code, createdByCode, turn, friendly, graphicsItemSender, mechanics);
    getHandList(friendly)->append(card);
    updateCardZoneSpots(friendly);

    if(viewBoard == nowBoard)       ui->planGraphicsView->scene()->addItem(card);
    if(!code.isEmpty())             emit checkCardImage(code, false);
    if(!createdByCode.isEmpty())    emit checkCardImage(createdByCode, false);

    //Pending Tag Changes
    const QList<TagChange> tagChangeList = pendingTagChanges.values(id);
    for(const TagChange &tagChange: tagChangeList)
    {
        addCardTagChange(tagChange, card);
    }
    pendingTagChanges.remove(id);

    //Show card draw last turn
    if(!turnBoards.empty())
    {
        Board *board = turnBoards.last();
        if(board->numTurn == turn)
        {
            CardGraphicsItem *drawCard = findCard(friendly, id, board);

            if(drawCard == nullptr || drawCard->isDiscard())
            {
                drawCard = new CardGraphicsItem(card, false);
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

    if(card == nullptr)
    {
        emit pDebug("ERROR: CardGraphicsItem not found for buffing. Id: " + QString::number(id));
        return;
    }

    card->addBuff(buffAttack, buffHealth);
}


void PlanHandler::enemyCardForge(int id)
{
    CardGraphicsItem * card = findCard(false, id);

    if(card == nullptr)
    {
        emit pDebug("ERROR: CardGraphicsItem not found for forge. Id: " + QString::number(id));
        return;
    }

    card->forge();
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

        if(card == nullptr)
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
    for(Board *board: qAsConst(turnBoards))
    {
        CardGraphicsItem *card = findCard(false, id, board);
        if(card != nullptr)
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


void PlanHandler::fixLastEchoCard()
{
    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    QList<CardGraphicsItem *> *cardList = getHandList(board->playerTurn, board);

    if(cardList->isEmpty()) return;
    CardGraphicsItem *card = cardList->last();
    QString code = card->getCode();
    if(card->isDraw() && card->isPlayed() && !code.isEmpty() &&
            (Utility::getCardAttribute(code, "mechanics").toArray().contains(QJsonValue("ECHO")) ||
             Utility::codeEqConstant(code, UNSTABLE_EVOLUTION_TOKEN)))
    {
        cardList->removeLast();
        updateCardZoneSpots(board->playerTurn, board);
        if(viewBoard == board)      ui->planGraphicsView->scene()->removeItem(card);
        delete card;
    }
}


void PlanHandler::playerCardCodeChange(int id, QString newCode)
{
    CardGraphicsItem *card = findCard(true, id);

    if(card != nullptr)
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


void PlanHandler::minionCodeChange(bool friendly, int id, QString newCode)
{
    MinionGraphicsItem *minion = findMinion(friendly, id);

    if(minion != nullptr)
    {
        emit pDebug((friendly?QString("Player"): QString("Enemy")) + " minion Id: " + QString::number(id) +
                    " changed Code: " + minion->getCode() + " --> " + newCode);
        minion->changeCode(newCode);
        emit checkCardImage(newCode, false);
    }
    else
    {
        emit pDebug((friendly?QString("Player"): QString("Enemy")) + " minion Id: " + QString::number(id) + " not found in board.");
    }
}


//Test secreto Reckoning
bool PlanHandler::isReckoningTested(bool isHeroTo, int id1, int id2)
{
    MinionGraphicsItem * minion1 = findMinion(true, id1);
    if(minion1 == nullptr)  return false;
    int attack1 = minion1->getAttack();
    if(attack1 < 3) return false;
    //Minion -> hero
    if(isHeroTo)    return true;
    //Minion -> minion
    else
    {
        MinionGraphicsItem * minion2 = findMinion(false, id2);
        if(minion2 == nullptr)  return false;
        int attack2 = minion2->getAttack();
        int health1 = minion1->getHitsToDie(attack2);
        if(health1 > attack2)   return true;
        else                    return false;
    }
}


//Test secreto de no danar al rival en tu turno
bool PlanHandler::isEnemyHeroHealthChanged()
{
    if(turnBoards.count()<2)    return false;
    int nowHealth = turnBoards.last()->enemyHero->getArmorHealth();
    int prevHealth = turnBoards[turnBoards.count()-2]->enemyHero->getArmorHealth();
    if(nowHealth == prevHealth) return true;
    return false;
}


void PlanHandler::newTurn(bool playerTurn, int numTurn)
{
    if(numTurn == 2)    fixTurn1Card();
    fixLastEchoCard();

    //Update nowBoard
    nowBoard->playerTurn = playerTurn;

    for(MinionGraphicsItem *minion: qAsConst(nowBoard->playerMinions))
    {
        minion->setPlayerTurn(playerTurn);
    }

    for(MinionGraphicsItem *minion: qAsConst(nowBoard->enemyMinions))
    {
        minion->setPlayerTurn(playerTurn);
    }

    if(nowBoard->playerHero != nullptr)        nowBoard->playerHero->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHero != nullptr)         nowBoard->enemyHero->setPlayerTurn(playerTurn);

    if(nowBoard->playerHeroPower != nullptr)   nowBoard->playerHeroPower->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHeroPower != nullptr)    nowBoard->enemyHeroPower->setPlayerTurn(playerTurn);

    if(this->firstStoredTurn == 0)
    {
        this->firstStoredTurn = numTurn;

        if(numTurn > 5 && nowBoard->playerHero != nullptr && nowBoard->playerHero->getResources() == 1)
        {
            if(nowBoard->playerHero != nullptr) nowBoard->playerHero->setResources(10);
            if(nowBoard->enemyHero != nullptr)  nowBoard->enemyHero->setResources(10);
        }
    }
    //Mantenemos todos los tag changes entre turnos, asi las mecanicas anadidas a cartas como dark gifts, se pueden poner en los minions al invocarlos turnos despues.
    // pendingTagChanges.clear();

    //Store nowBoard
    turnBoards.append(copyBoard(nowBoard, numTurn));
    showManaPlayableCardsAuto();
    updateTurnSliderRange();

    //Avanza en now board
    if(viewBoard==nowBoard || viewBoard==futureBoard)   showLastTurn();

#ifdef QT_DEBUG
    if(DEBUG_REPLAY_AUTO_ADVANCE)   showNextTurn();//Auto avanzar turno para testing
#endif
}


Board * PlanHandler::copyBoard(Board *origBoard, int numTurn, bool copySecretCodes)
{
    Board *board = new Board();
    board->playerTurn = origBoard->playerTurn;
    board->numTurn = numTurn;

    if(origBoard->playerHero == nullptr)       board->playerHero = nullptr;
    else                                    board->playerHero = new HeroGraphicsItem(origBoard->playerHero);
    if(origBoard->enemyHero == nullptr)        board->enemyHero = nullptr;
    else                                    board->enemyHero = new HeroGraphicsItem(origBoard->enemyHero, copySecretCodes);//Lo usamos al crear el futureBoard, solo es necesario para el enemigo ya que los secretos amigos de nowBoard nunca estaran desvelados ni isolated.

    if(origBoard->playerHeroPower == nullptr)  board->playerHeroPower = nullptr;
    else                                    board->playerHeroPower = new HeroPowerGraphicsItem(origBoard->playerHeroPower);
    if(origBoard->enemyHeroPower == nullptr)   board->enemyHeroPower = nullptr;
    else                                    board->enemyHeroPower = new HeroPowerGraphicsItem(origBoard->enemyHeroPower);

    if(origBoard->playerWeapon == nullptr)     board->playerWeapon = nullptr;
    else
    {
        board->playerWeapon = new WeaponGraphicsItem(origBoard->playerWeapon);
        if(board->playerHero != nullptr)       board->playerHero->setHeroWeapon(board->playerWeapon, false);
    }
    if(origBoard->enemyWeapon == nullptr)      board->enemyWeapon = nullptr;
    else
    {
        board->enemyWeapon = new WeaponGraphicsItem(origBoard->enemyWeapon);
        if(board->enemyHero != nullptr)       board->enemyHero->setHeroWeapon(board->enemyWeapon, false);
    }

    for(MinionGraphicsItem *minion: qAsConst(origBoard->playerMinions))
    {
        board->playerMinions.append(new MinionGraphicsItem(minion));
    }

    for(MinionGraphicsItem *minion: qAsConst(origBoard->enemyMinions))
    {
        board->enemyMinions.append(new MinionGraphicsItem(minion));
    }

    bool showMechanics = (numTurn==0);
    for(CardGraphicsItem *card: qAsConst(origBoard->playerHandList))
    {
        board->playerHandList.append(new CardGraphicsItem(card, showMechanics));
    }

    for(CardGraphicsItem *card: qAsConst(origBoard->enemyHandList))
    {
        board->enemyHandList.append(new CardGraphicsItem(card, showMechanics));
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
        if(isLastTriggerValid(code))
        {
            emit pDebug("lastTriggerId = " + QString::number(id));
            this->lastTriggerId = id;
        }
        else
        {
            emit pDebug("Trigger creator code is in the forbidden list: " + code, Warning);
            this->lastTriggerId = -1;
        }

        if(code.isEmpty())
        {
            this->lastPowerAddon.id = -1;
        }
        else
        {
            this->lastPowerAddon.code = code;
            this->lastPowerAddon.id = id;
            this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        }
    }
    else if(blockType == "POWER")
    {
        this->lastTriggerId = (idTarget!=-1?idTarget:id);
        emit pDebug("lastTriggerId = " + QString::number(this->lastTriggerId));

        if(code.isEmpty())
        {
            this->lastPowerAddon.id = -1;
        }
        else
        {
            this->lastPowerAddon.code = code;
            this->lastPowerAddon.id = id;
            this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        }
    }
    else if(blockType == "FATIGUE")
    {
        emit pDebug("lastTriggerId reset.");
        this->lastTriggerId = -1;
        this->lastPowerAddon.code = "FATIGUE";
        this->lastPowerAddon.id = id;
        this->lastPowerTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    else
    {
        emit pDebug("lastTriggerId reset.");
        this->lastTriggerId = -1;
        this->lastPowerAddon.id = -1;
    }
    this->lastArmorRemoverIds.idAddon = -1;
}


void PlanHandler::redrawDownloadedCardImage(QString code)
{
    for(MinionGraphicsItem *minion: qAsConst(viewBoard->playerMinions))
    {
        minion->checkDownloadedCode(code);
    }
    for(MinionGraphicsItem *minion: qAsConst(viewBoard->enemyMinions))
    {
        minion->checkDownloadedCode(code);
    }
    for(CardGraphicsItem *card: qAsConst(viewBoard->playerHandList))
    {
        card->checkDownloadedCode(code);
    }
    for(CardGraphicsItem *card: qAsConst(viewBoard->enemyHandList))
    {
        card->checkDownloadedCode(code);
    }

    if(viewBoard->playerHero != nullptr)   viewBoard->playerHero->checkDownloadedCode(code);
    if(viewBoard->enemyHero != nullptr)    viewBoard->enemyHero->checkDownloadedCode(code);

    if(viewBoard->playerHeroPower != nullptr)  viewBoard->playerHeroPower->checkDownloadedCode(code);
    if(viewBoard->enemyHeroPower != nullptr)   viewBoard->enemyHeroPower->checkDownloadedCode(code);

    if(viewBoard->playerWeapon != nullptr) viewBoard->playerWeapon->checkDownloadedCode(code);
    if(viewBoard->enemyWeapon != nullptr)  viewBoard->enemyWeapon->checkDownloadedCode(code);
}


void PlanHandler::createFutureBoard()
{
    futureBoard = copyBoard(nowBoard, 0, true);
    if(futureBoard->playerHero != nullptr)  futureBoard->playerHero->setShowAllInfo();
    if(futureBoard->enemyHero != nullptr)   futureBoard->enemyHero->setShowAllInfo();
    viewBoard = futureBoard;
    loadViewBoard();
    ui->planButtonLast->setIcon(QIcon(ThemeHandler::buttonPlanRefreshFile()));
    ui->planButtonLast->setEnabled(true);
    emit cardLeave();   //Hide cards tooltips
    showManaPlayableCards(futureBoard);
}


int PlanHandler::getPotentialMana(Board *board)
{
    int mana = board->playerHero->getAvailableResources();
    for(CardGraphicsItem *card: qAsConst(board->playerHandList))
    {
        if(!card->isPlayed())
        {
            int manaCard = card->getManaSpent(false);
            if(manaCard < 0)    mana -= manaCard;
        }
    }

    return mana;
}


void PlanHandler::showManaPlayableCards(Board *board)
{
    int mana = getPotentialMana(board);
    for(CardGraphicsItem *card: qAsConst(board->playerHandList))
    {
        if(!card->isPlayed())   card->showManaPlayable(mana);
    }

    if(board->playerHeroPower != nullptr && !board->playerHeroPower->isExausted())
    {
        board->playerHeroPower->showManaPlayable(mana);
    }
}


int PlanHandler::getPotentialManaNextTurn()
{
    int mana = nowBoard->playerHero->getResources() + 1;
    for(CardGraphicsItem *card: qAsConst(nowBoard->playerHandList))
    {
        int manaCard = card->getManaSpent(false);
        if(manaCard < 0)    mana -= manaCard;
    }

    return min(10, mana);
}


void PlanHandler::showManaPlayableCardsNextTurn()
{
    int mana = getPotentialManaNextTurn();
    for(CardGraphicsItem *card: qAsConst(nowBoard->playerHandList))
    {
        card->showManaPlayable(mana);
    }

    if(nowBoard->playerHeroPower != nullptr && !nowBoard->playerHeroPower->isExausted())
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
    if(futureBoard == nullptr)
    {
        if(nowBoard->playerHandList.contains(card))
        {
            createFutureBoard();

            //Get card
            QList<CardGraphicsItem *> * handList = getHandList(true, futureBoard);
            int pos = findCardPos(handList, card->getId());
            if(pos == -1)
            {
                emit pDebug("ERROR: Clicked card not found in just created futureBoard.", DebugLevel::Error);
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
        futureBoard->playerHero->addResourcesUsed(card->isPlayed()?card->getManaSpent(true):-card->getManaSpent(true));
        showManaPlayableCards(futureBoard);
    }
}


void PlanHandler::heroPowerPress(HeroPowerGraphicsItem* heroPower, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(mouseButton != Qt::LeftButton)   return;

    bool doToggle = false;
    if(futureBoard == nullptr)
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
        int cost = heroPower->getCost();
        futureBoard->playerHero->addResourcesUsed(heroPower->isExausted()?cost:-cost);
        showManaPlayableCards(futureBoard);
    }
}


void PlanHandler::minionPress(MinionGraphicsItem* minion, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(minion->isDead())  return;

    if(futureBoard == nullptr)
    {
        if(nowBoard->playerMinions.contains(minion) && minion->getAttack()>0)
        {
            createFutureBoard();

            //Get minion
            QList<MinionGraphicsItem *> * minionList = getMinionList(true, futureBoard);
            int pos = findMinionPos(minionList, minion->getId());
            if(pos == -1)
            {
                emit pDebug("ERROR: Clicked minion not found in just created futureBoard.", DebugLevel::Error);
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
            if(selectedMinion == nullptr)
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
                        selectedMinion = nullptr;
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
                    selectedMinion = nullptr;
                }
            }
        }
    }
}


void PlanHandler::heroPress(HeroGraphicsItem* hero, Qt::MouseButton mouseButton)
{
    if(!nowBoard->playerTurn)   return;
    if(hero->isDead())  return;

    if(futureBoard == nullptr)
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
            if(selectedMinion == nullptr)
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
                        selectedMinion = nullptr;
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
                    selectedMinion = nullptr;
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

    if(board->playerHero != nullptr)  removeHero(true, board);
    if(board->enemyHero != nullptr)   removeHero(false, board);

    if(board->playerHeroPower != nullptr)  removeHeroPower(true, board);
    if(board->enemyHeroPower != nullptr)   removeHeroPower(false, board);

    if(board->playerWeapon != nullptr) removeWeapon(true, -1, board);
    if(board->enemyWeapon != nullptr)  removeWeapon(false, -1, board);
}


void PlanHandler::reset()
{
    emit pDebug("Clear all boards.");
    ui->planGraphicsView->reset();
    pendingTagChanges.clear();
    this->lastMinionAdded = nullptr;
    this->viewBoard = nowBoard;
    this->firstStoredTurn = 0;
    this->nowBoard->playerTurn = true;
    setLastTriggerId("", "", -1, -1);

    if(futureBoard != nullptr)     deleteFutureBoard();
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


void PlanHandler::setTheme()
{
    if(futureBoard == nullptr) ui->planButtonLast->setIcon(QIcon(ThemeHandler::buttonPlanLastFile()));
    else                    ui->planButtonLast->setIcon(QIcon(ThemeHandler::buttonPlanRefreshFile()));
    ui->planButtonNext->setIcon(QIcon(ThemeHandler::buttonPlanNextFile()));
    ui->planButtonPrev->setIcon(QIcon(ThemeHandler::buttonPlanPrevFile()));
    ui->planButtonFirst->setIcon(QIcon(ThemeHandler::buttonPlanFirstFile()));
    ui->planButtonResize->setIcon(QIcon(ThemeHandler::buttonPlanResizeFile()));

    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(24);
    ui->planLabelTurn->setFont(font);
    QFontMetrics fm(font);
    int textWide = fm.width("T99");
    ui->planLabelTurn->setFixedWidth(textWide);

    ui->planGraphicsView->setTheme(false);
}


void PlanHandler::deleteFutureBoard()
{
    resetBoard(futureBoard);
    delete futureBoard;
    futureBoard = nullptr;
    selectedMinion = nullptr;
    selectedCode = "";
    ui->planButtonLast->setIcon(QIcon(ThemeHandler::buttonPlanLastFile()));
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
    if(viewBoard->playerTurn)   color = GREEN_H;
    else                        color = SOFT_RED_H;
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

    if(futureBoard != nullptr)     deleteFutureBoard();
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

    for(MinionGraphicsItem *minion: qAsConst(viewBoard->playerMinions))
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    for(MinionGraphicsItem *minion: qAsConst(viewBoard->enemyMinions))
    {
        ui->planGraphicsView->scene()->addItem(minion);
    }

    for(ArrowGraphicsItem *arrow: qAsConst(viewBoard->arrows))
    {
        ui->planGraphicsView->scene()->addItem(arrow);
    }

    for(CardGraphicsItem *card: qAsConst(viewBoard->playerHandList))
    {
        ui->planGraphicsView->scene()->addItem(card);
    }

    for(CardGraphicsItem *card: qAsConst(viewBoard->enemyHandList))
    {
        ui->planGraphicsView->scene()->addItem(card);
    }

    if(viewBoard->playerHeroPower != nullptr)  ui->planGraphicsView->scene()->addItem(viewBoard->playerHeroPower);
    if(viewBoard->enemyHeroPower != nullptr)   ui->planGraphicsView->scene()->addItem(viewBoard->enemyHeroPower);

    if(viewBoard->playerWeapon != nullptr) ui->planGraphicsView->scene()->addItem(viewBoard->playerWeapon);
    if(viewBoard->enemyWeapon != nullptr)  ui->planGraphicsView->scene()->addItem(viewBoard->enemyWeapon);

    if(viewBoard->playerHero != nullptr)    ui->planGraphicsView->scene()->addItem(viewBoard->playerHero);
    if(viewBoard->enemyHero != nullptr)     ui->planGraphicsView->scene()->addItem(viewBoard->enemyHero);

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
        if(nowBoard->enemyHero != nullptr)  nowBoard->enemyHero->setDead(true);
    }
    else
    {
        if(nowBoard->playerHero != nullptr) nowBoard->playerHero->setDead(true);
    }

    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();

    if(playerWon)
    {
        if(board->enemyHero != nullptr)  board->enemyHero->setDead(true);
    }
    else
    {
        if(board->playerHero != nullptr) board->playerHero->setDead(true);
    }
}


bool PlanHandler::getWinner()
{
    if(nowBoard->playerHero != nullptr && nowBoard->playerHero->getRemainingHealth()<=0)   return false;
    if(nowBoard->enemyHero != nullptr && nowBoard->enemyHero->getRemainingHealth()<=0)     return true;
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


void PlanHandler::resizePlan()
{
    sizePlan = !sizePlan;
    emit swapSize(sizePlan);
}


void PlanHandler::updateTransparency()
{
    bool inTabPlan = ui->tabWidget->currentWidget() == ui->tabPlan;

    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabPlan->setAttribute(Qt::WA_NoBackground);
        ui->tabPlan->repaint();

        //Tambien nos hacemos cargo en transparency==Transparent para que se llame a MainWindowFade al empezar y terminar un juego
        if(inTabPlan && (transparency == Transparent || transparency == AutoTransparent))
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
    if(nowBoard->playerHero != nullptr)    nowBoard->playerHero->setDeadProb();
    if(nowBoard->enemyHero != nullptr)     nowBoard->enemyHero->setDeadProb();

    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(true))    minion->setDeadProb();
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(false))   minion->setDeadProb();

    if(futureBombs.isRunning())     abortFutureBombs = true;
}


void PlanHandler::checkBomb(QString code)
{
    if(code.isEmpty() || viewBoard!=nowBoard || nowBoard->enemyHero == nullptr || nowBoard->playerHero == nullptr)  return;

    bool playerIn, onlyMinions;
    int missiles, missileDamage;
    if(!isCardBomb(code, playerIn, onlyMinions, missiles, missileDamage) || missiles==0)    return;

    //Targets
    QList<int> targets;

    if(!onlyMinions)    targets.append(nowBoard->enemyHero->getHitsToDie(missileDamage));
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(false))  targets.append(minion->getHitsToDie(missileDamage));
    if(playerIn)
    {
        if(!onlyMinions)    targets.append(nowBoard->playerHero->getHitsToDie(missileDamage));
        for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(true))   targets.append(minion->getHitsToDie(missileDamage));
    }
    if(targets.isEmpty())   return;

    //Get dead probs
    futureBombs.setFuture(QtConcurrent::run(this, &PlanHandler::bombDeads, targets, playerIn, onlyMinions, missiles, missileDamage));
    abortFutureBombs = false;
}


void PlanHandler::setDeadProbs()
{
    if(abortFutureBombs)    return;

    DeadProbs dp = futureBombs.result();
    QList<float> &deadProbs = dp.dp;
    bool playerIn = dp.playerIn;
    bool onlyMinions = dp.onlyMinions;

    if(!onlyMinions)    nowBoard->enemyHero->setDeadProb(deadProbs.takeFirst());
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(false))  minion->setDeadProb(deadProbs.takeFirst());

    if(playerIn)
    {
        if(!onlyMinions)    nowBoard->playerHero->setDeadProb(deadProbs.takeFirst());
        for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(true))   minion->setDeadProb(deadProbs.takeFirst());
    }
}


DeadProbs PlanHandler::bombDeads(QList<int> targets, bool playerIn, bool onlyMinions, int missiles, int missileDamage)
{
    QMap<QString, float> states;
    states[encodeBombState(targets)] = 1;
    for(int i=0; i<missiles; i++)           states = bomb(states, missileDamage);

    DeadProbs dp;
    QList<float> &deadProbs = dp.dp;
    for(int i=0; i<targets.count(); i++)    deadProbs.append(0);

    const QList<QString> stateList = states.keys();
    for(const QString &state: stateList)
    {
        float prob = states[state];
        QList<int> targets = decodeBombState(state);
        for(int i=0; i<targets.count(); i++)
        {
            if(targets[i] == 0)     deadProbs[i] += prob;
        }
    }

    dp.playerIn = playerIn;
    dp.onlyMinions = onlyMinions;
    return dp;
}


QMap<QString, float> PlanHandler::bomb(QMap<QString, float> &oldStates, int missileDamage)
{
    QMap<QString, float> newStates;

    const QList<QString> oldStateList = oldStates.keys();
    for(const QString &oldState: oldStateList)
    {
        float oldProb = oldStates[oldState];
        QList<int> oldTargets = decodeBombState(oldState);

        int livingTargets = 0;
        for(int i=0; i<oldTargets.count(); i++)
        {
            if(oldTargets[i] > 0)   livingTargets++;
        }

        //Nada vivo, copiamos estado
        if(livingTargets == 0)
        {
            newStates[oldState] = oldProb;
        }

        //Apply an attack to each living targets
        for(int i=0; i<oldTargets.count(); i++)
        {
            if(oldTargets[i] > 0)
            {
                QList<int> newTargets(oldTargets);
                newTargets[i]-=missileDamage;
                if(newTargets[i]<0) newTargets[i]=0;
                QString newState = encodeBombState(newTargets);

                //Update probabilities
                float addProb = newStates.contains(newState)?newStates[newState]:0;
                newStates[newState] = addProb + oldProb/livingTargets;
            }
        }
    }

    return newStates;
}


QList<int> PlanHandler::decodeBombState(QString state)
{
    QList<int> targets;
    for(const QString &targetString: (const QStringList)state.split(":"))  targets.append(targetString.toInt());
    return targets;
}


QString PlanHandler::encodeBombState(QList<int> targets)
{
    QStringList targetsString;
    for(int target: targets)    targetsString.append(QString::number(target));
    return targetsString.join(":");
}


int PlanHandler::flamewakersOnBoard()
{
    int num = 0;
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(true))
    {
        if(Utility::codeEqConstant(minion->getCode(), FLAMEWAKER))      num++;
    }
    return num;
}


int PlanHandler::numRaceOnBoard(CardRace cardRace)
{
    int numRace = 0;
    for(MinionGraphicsItem *minion: (const QList<MinionGraphicsItem *>)*getMinionList(true))
    {
        if(Utility::getRaceFromCode(minion->getCode()).contains(cardRace))  numRace++;
    }
    return numRace;
}


//Card bombs
bool PlanHandler::isCardBomb(QString code)
{
    bool playerIn, onlyMinions;
    int missiles, missileDamage;
    return isCardBomb(code, playerIn, onlyMinions, missiles, missileDamage);
}


bool PlanHandler::isCardBomb(QString code, bool &playerIn, bool &onlyMinions, int &missiles, int &missileDamage)
{
    missiles = 0;
    missileDamage = 1;
    playerIn = false;
    onlyMinions = false;

    //ALL
    if(Utility::codeEqConstant(code, MAD_BOMBER))
    {
        missiles = 3;
        playerIn = true;
    }
    else if(Utility::codeEqConstant(code, MADDER_BOMBER))
    {
        missiles = 6;
        playerIn = true;
    }
    else if(Utility::codeEqConstant(code, SPREADING_MADNESS) || Utility::codeEqConstant(code, SPREADING_MADNESS2))
    {
        missiles = 13;
        playerIn = true;
    }
    else if(Utility::codeEqConstant(code, MADDEST_BOMBER))
    {
        missiles = 12;
        playerIn = true;
    }
    //ALL minions
    else if(Utility::codeEqConstant(code, MINEFIELD))
    {
        missiles = 5;
        playerIn = true;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, SCAVENGING_SHIVARRA))
    {
        missiles = 6;
        playerIn = true;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, BAKING_SODA_VOLCANO))
    {
        missiles = 10;
        playerIn = true;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, VOLCANO))
    {
        missiles = 15;
        playerIn = true;
        onlyMinions = true;
    }
    //Enemy
    else if(Utility::codeEqConstant(code, SEABREEZE_CHALICE) || Utility::codeEqConstant(code, SEABREEZE_CHALICE_1) || Utility::codeEqConstant(code, SEABREEZE_CHALICE_2))
    {
        missiles = 2;
    }
    else if(Utility::codeEqConstant(code, ARCANE_MISSILES) || Utility::codeEqConstant(code, KOBOLD_APPRENTICE) || Utility::codeEqConstant(code, HORIZONS_EDGE))
    {
        missiles = 3;
    }
    else if(Utility::codeEqConstant(code, TRIPLEWICK_TRICKSTER))
    {
        missiles = 3;
        missileDamage = 2;
    }
    else if(Utility::codeEqConstant(code, GREATER_ARCANE_MISSILES))
    {
        missiles = 3;
        missileDamage = 3;
    }
    else if(Utility::codeEqConstant(code, BEATBOXER))
    {
        missiles = 4;
    }
    else if(Utility::codeEqConstant(code, NAGRAND_SLAM))
    {
        missiles = 4;
        missileDamage = 3;
    }
    else if(Utility::codeEqConstant(code, CINDERSTORM))
    {
        missiles = 5;
    }
    else if(Utility::codeEqConstant(code, PRIESTESS_OF_FURY) || Utility::codeEqConstant(code, SOUL_BARRAGE))
    {
        missiles = 6;
    }
    else if(Utility::codeEqConstant(code, TAINTED_REMNANT))
    {
        missiles = 7;
    }
    else if(Utility::codeEqConstant(code, AVENGING_WRATH))
    {
        missiles = 8;
    }
    else if(Utility::codeEqConstant(code, MASK_OF_CTHUN) || Utility::codeEqConstant(code, SUNSET_VOLLEY))
    {
        missiles = 10;
    }
    //Enemy minions
    else if(Utility::codeEqConstant(code, DART_THROW))
    {
        missiles = 2;
        missileDamage = 2;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, MALFUNCTION))
    {
        missiles = 3;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, FIREBRAND) || Utility::codeEqConstant(code, DEVOURING_PLAGUE))
    {
        missiles = 4;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, FULLBLOWN_EVIL) || Utility::codeEqConstant(code, MORTAL_ERADICATION) ||
               Utility::codeEqConstant(code, XB488_DISPOSALBOT) || Utility::codeEqConstant(code, XB488_DISPOSALBOT_FORGED)
               )
    {
        missiles = 5;
        onlyMinions = true;
    }
    else if(Utility::codeEqConstant(code, RENO_THE_RELICOLOGIST) || Utility::codeEqConstant(code, DONT_STAND_IN_THE_FIRE))
    {
        missiles = 10;
        onlyMinions = true;
    }
    //Extra
    else if(Utility::codeEqConstant(code, GOBLIN_BLASTMAGE) || Utility::codeEqConstant(code, GOBLIN_BLASTMAGE2))
    {
        if(numRaceOnBoard(MECHANICAL)>0)
        {
            missiles = 6;
        }
    }
    else if(Utility::codeEqConstant(code, CANNON_BARRAGE))
    {
        missiles = numRaceOnBoard(PIRATE) + 1;
        missileDamage = 3;
    }
    else if(Utility::codeEqConstant(code, DEFIAS_BLASTFISHER))
    {
        missiles = numRaceOnBoard(BEAST) + 1;
        missileDamage = 2;
    }
    else if(Utility::codeEqConstant(code, JUNGLE_GYM))
    {
        missiles = numRaceOnBoard(BEAST) + 1;
    }
    else if(Utility::codeEqConstant(code, METEOROLOGIST) || Utility::codeEqConstant(code, BLASTMAGE_MINER))
    {
        missiles = getHandList(true)->count() - 1;
    }
    else if(Utility::codeEqConstant(code, TROGG_GEMTOSSER))
    {
        missiles = nowBoard->playerHero->getResources();
    }
    else if(Utility::codeEqConstant(code, DARK_SKIES))
    {
        missiles = getHandList(true)->count();
        playerIn = true;
        onlyMinions = true;
    }
    else
    {
        return false;
    }

    if(Utility::getTypeFromCode(code) == SPELL)
    {
        //SpellDamage
        HeroGraphicsItem *hero = getHero(true, nullptr);
        if(hero != nullptr)
        {
            if(missileDamage == 1)  missiles += hero->getSpellDamage();
            else                    missileDamage += hero->getSpellDamage();
        }
    }

    return true;
}


QJsonArray PlanHandler::getJsonCardHistory()
{
    QJsonArray cardHistory;

    for(const Board *board: (const QList<Board *>)turnBoards)
    {
        QJsonObject item;
        item[ "turn" ] = (board->numTurn + 1)/2;
        item[ "player" ] = board->playerTurn?"me":"opponent";

        for(CardGraphicsItem *card: (board->playerTurn?board->playerHandList:board->enemyHandList))
        {
            if(card->isPlayed())
            {
                item[ "card_id" ] = card->getCode();
                cardHistory.append(item);
            }
        }
    }

    return cardHistory;
}




//Card exceptions
//Cartas que no pueden poner addons ni en otros esbirros ni en heroes
bool PlanHandler::isAddonCommonValid(const QString &code)
{
    Q_UNUSED(code);
    return true;
}


//Cartas que no pueden poner addons en heroes
bool PlanHandler::isAddonHeroValid(const QString &code)
{
    return isAddonCommonValid(code);
}


//Cartas que no pueden poner addons en otros esbirros
bool PlanHandler::isAddonMinionValid(const QString &code)
{
    return isAddonCommonValid(code);
}


//Cartas que no pueden crear otros esbirros
bool PlanHandler::isLastTriggerValid(const QString &code)
{
    Q_UNUSED(code);
    return true;
}
