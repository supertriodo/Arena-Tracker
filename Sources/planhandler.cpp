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
    connect(ui->planGraphicsView, SIGNAL(sizeChanged()),
            this, SLOT(updateViewCardZoneSpots()));
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


void PlanHandler::addMinion(bool friendly, MinionGraphicsItem *minion, int pos)
{
    this->lastMinionAdded = minion;
    this->lastMinionAddedTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly);
    minionsList->insert(pos, minion);
    updateMinionZoneSpots(friendly);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->addItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
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

    MinionGraphicsItem* minion = new MinionGraphicsItem(code, id, friendly, nowBoard->playerTurn);
    addMinion(friendly, minion, pos);
    if(this->lastTriggerId!=-1)     copyMinionToLastTurn(friendly, minion);
    else                                emit pDebug("Triggered minion creator not set.");
    emit checkCardImage(code, false);
}


void PlanHandler::copyMinionToLastTurn(bool friendly, MinionGraphicsItem *minion, int idCreator)
{
    if(turnBoards.empty())  return;

    emit pDebug("Triggered minion. Ids: " + QString::number(idCreator) + " --> " + QString::number(minion->getId()));

    MinionGraphicsItem *triggerMinion = NULL;
    if(idCreator == -1)     idCreator = this->lastTriggerId;
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

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->removeItem(minion);
        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
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
        emit pDebug("Trying to add a hero with an existing one. Force remove old one.", Warning);
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
        if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->removeItem(hero);
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


void PlanHandler::playerMinionTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->playerHero!=NULL && nowBoard->playerHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->playerHero->getId(), Addon::AddonNeutral);
        removeHero(nowBoard, true);
        addHero(true, code, id);
    }
    else    addTagChange(id, true, tag, value);
}


void PlanHandler::enemyMinionTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->enemyHero!=NULL && nowBoard->enemyHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->enemyHero->getId(), Addon::AddonNeutral);
        removeHero(nowBoard, false);
        addHero(false, code, id);
    }
    else    addTagChange(id, false, tag, value);
}


void PlanHandler::addTagChange(int id, bool friendly, QString tag, QString value)
{
    TagChange tagChange;
    tagChange.id = id;
    tagChange.friendly = friendly;
    tagChange.tag = tag;
    tagChange.value = value;

    bool healing = false;
    bool isDead = true;
    bool isHero = true;
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != NULL)
    {
        emit pDebug("Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        checkAtkHealthChange(minion, friendly, tag, value);
        healing = minion->processTagChange(tag, value);
        isDead = minion->isDead();
        isHero = false;
    }
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
    else
    {
        QTimer::singleShot(1000, this, SLOT(checkPendingTagChanges()));
        pendingTagChanges.append(tagChange);
        emit pDebug("Append Tag Change: Id: " + QString::number(id) + " - " + tag + " --> " + value +
                    " - " + QString::number(pendingTagChanges.count()));
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
    qDebug()<<"CHECK TAG LIST -->"<<pendingTagChanges.count();

    if(pendingTagChanges.isEmpty()) return;

    TagChange tagChange = pendingTagChanges.takeFirst();
    const int id = tagChange.id;
    const bool friendly = tagChange.friendly;
    const QString tag = tagChange.tag;
    const QString value = tagChange.value;

    bool healing = false;
    //Evita addons provocado por cambio de damage al morir(en el log los minion vuelven a damage 0 justo antes de morir)
    //Ejemplo Jefe de banda de diablillos ataca y mata a otro minion, el jefe produce un trigger que apareceria en el esbirro muerto.
    bool isDead = true;
    //Evita addons al perder un arma y cambiar el atk a 0
    //Ejemplo ataque con arma a grubashi y gasta el arma.
    bool isHero = true;
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != NULL)
    {
        emit pDebug("Pending Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        checkAtkHealthChange(minion, friendly, tag, value);
        healing = minion->processTagChange(tag, value);
        isDead = minion->isDead();
        isHero = false;
    }
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
    else    return;


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
    this->addWeaponAddonToLastTurn(true, code, id);
}


void PlanHandler::enemyWeaponZonePlayAdd(QString code, int id)
{
    this->addWeaponAddonToLastTurn(false, code, id);
}


void PlanHandler::addWeaponAddonToLastTurn(bool friendly, QString code, int id)
{
    setLastTriggerId("", "", -1, -1);

    if(turnBoards.empty())  return;

    Board *board = turnBoards.last();

    if(friendly)    addAddon(board->playerHero, code, id, Addon::AddonNeutral);
    else            addAddon(board->enemyHero, code, id, Addon::AddonNeutral);
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
    emit pDebug("Addon(" + QString::number(minion->getId()) + ")-->" + code);
    minion->addAddon(code, id, type, number);
    if(code != "FATIGUE")   emit checkCardImage(code, false);
}


bool PlanHandler::isAddonCommonValid(QString code)
{
    QList<QString> forbiddenAddonList;
    forbiddenAddonList.append(IMP_GANG_BOSS);
    forbiddenAddonList.append(DRAGON_EGG);
    forbiddenAddonList.append(ACOLYTE_OF_PAIN);
    forbiddenAddonList.append(GURUBASHI_BERSERKER);
    forbiddenAddonList.append(FROTHING_BERSEKER);
    forbiddenAddonList.append(LIGHTWARDEN);
    forbiddenAddonList.append(GOREHOWL);
    forbiddenAddonList.append(LOREWALKER_CHO);
    forbiddenAddonList.append(NERUBIAN_PROPHET);
    return !forbiddenAddonList.contains(code);
}


bool PlanHandler::isAddonHeroValid(QString code)
{
    QList<QString> forbiddenAddonList;
    forbiddenAddonList.append(ACIDMAW);
    return !forbiddenAddonList.contains(code) && isAddonCommonValid(code);
}


bool PlanHandler::isAddonMinionValid(QString code)
{
    QList<QString> forbiddenAddonList;
    forbiddenAddonList.append(WRATHGUARD);
    forbiddenAddonList.append(AXE_FLINGER);
    forbiddenAddonList.append(ARMORSMITH);
    forbiddenAddonList.append(EYE_FOR_AN_EYE);
    forbiddenAddonList.append(TRUESILVER_CHAMPION);
    return !forbiddenAddonList.contains(code) && isAddonCommonValid(code);
}


SecretHero PlanHandler::cardclassToSecrethero(CardClass cardClass)
{
    if(cardClass==MAGE)         return mage;
    else if(cardClass==HUNTER)  return hunter;
    else if(cardClass==PALADIN) return paladin;
    else                        return unknown;
}


void PlanHandler::playerSecretPlayed(int id, QString code)
{
    if(nowBoard->playerHero == NULL) return;
    DeckCard deckCard(code);
    nowBoard->playerHero->addSecret(id, cardclassToSecrethero(deckCard.getCardClass()));
}


void PlanHandler::enemySecretPlayed(int id, SecretHero secretHero)
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
}


void PlanHandler::playerSecretStolen(int id, QString code)
{
    if(nowBoard->enemyHero == NULL) return;
    SecretHero secretHero = nowBoard->enemyHero->getSecretHero(id);
    enemySecretRevealed(id, code);
    if(nowBoard->playerHero != NULL)    nowBoard->playerHero->addSecret(id, secretHero);
}


void PlanHandler::enemySecretStolen(int id, QString code)
{
    if(nowBoard->playerHero == NULL) return;
    SecretHero secretHero = nowBoard->playerHero->getSecretHero(id);
    playerSecretRevealed(id, code);
    enemySecretPlayed(id, secretHero);
}


void PlanHandler::updateViewCardZoneSpots()
{
    updateCardZoneSpots(true, viewBoard);
}


void PlanHandler::updateCardZoneSpots(bool friendly, Board *board)
{
    if(board == NULL)   board = nowBoard;
    QList<CardGraphicsItem *> *playerHandList = &board->playerHandList;
    for(int i=0; i<playerHandList->count(); i++)
    {
       playerHandList->at(i)->setZonePos(friendly, i, playerHandList->count(), ui->planGraphicsView->getSceneViewWidth());
    }
}


int PlanHandler::findCardPos(QList<CardGraphicsItem *> * cardsList, int id)
{
    for(int i=0; i<cardsList->count(); i++)
    {
        if(cardsList->at(i)->getId()==id) return i;
    }
    return -1;
}


//MinionGraphicsItem * PlanHandler::findMinion(bool friendly, int id, Board *board)
//{
//    QList<MinionGraphicsItem *> * minionsList = getMinionList(friendly, board);
//    int pos = findMinionPos(minionsList, id);
//    if(pos == -1)   return NULL;
//    else            return minionsList->at(pos);
//}


void PlanHandler::playerCardDraw(int id, QString code)
{
    CardGraphicsItem *card = new CardGraphicsItem(code, id);
    nowBoard->playerHandList.append(card);
    updateCardZoneSpots(true);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->addItem(card);
//        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
    }
    emit checkCardImage(code, false);
}


//void PlanHandler::enemyCardDraw(int id, QString code)
//{

//}


void PlanHandler::playerCardPlayed(int id, QString code)
{
    int pos = findCardPos(&nowBoard->playerHandList, id);

    if(pos == -1)
    {
        emit pDebug("Player card played not found: " + QString::number(id) + " -- " + code);
        return;
    }

    CardGraphicsItem *card = nowBoard->playerHandList.takeAt(pos);
    updateCardZoneSpots(true);

    if(viewBoard == nowBoard)
    {
        ui->planGraphicsView->scene()->removeItem(card);
//        ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
    }

    delete card;

    //Set played last board
    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    pos = findCardPos(&board->playerHandList, id);
    if(pos == -1)
    {
        emit pDebug("Player card played not found on last turn: " + QString::number(id) + " -- " + code);
        return;
    }
    board->playerHandList.at(pos)->setPlayed();
}


//void PlanHandler::enemyCardPlayed(int id, QString code)
//{

//}


void PlanHandler::newTurn(bool playerTurn, int numTurn)
{
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

    if(nowBoard->playerHero != NULL)      nowBoard->playerHero->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHero != NULL)      nowBoard->enemyHero->setPlayerTurn(playerTurn);


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

    foreach(CardGraphicsItem * card, nowBoard->playerHandList)
    {
        board->playerHandList.append(new CardGraphicsItem(card));
    }

    turnBoards.append(board);


    //Avanza en ultimo turno
    int prevTurn = turnBoards.count()-2;
    if(prevTurn>=0 && viewBoard==turnBoards[prevTurn])   showNextTurn();
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
        else                            emit pDebug("Trigger code is in the forbidden creator list: " + code, Warning);
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


bool PlanHandler::isLastTriggerValid(QString code)
{
    QList<QString> forbiddenCreatorList;
    forbiddenCreatorList.append(KNIFE_JUGGLER);
    forbiddenCreatorList.append(ADDLED_GRIZZLY);
    forbiddenCreatorList.append(DARKSHIRE_COUNCILMAN);
    forbiddenCreatorList.append(FROTHING_BERSEKER);
    forbiddenCreatorList.append(THE_SKELETON_KNIGHT);
    return !forbiddenCreatorList.contains(code);
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

    if(viewBoard->playerHero != NULL)   viewBoard->playerHero->checkDownloadedCode(code);
    if(viewBoard->enemyHero != NULL)    viewBoard->enemyHero->checkDownloadedCode(code);
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
    this->nowBoard->playerTurn = true;
    setLastTriggerId("", "", -1, -1);
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

    QString color;
    if(viewBoard->playerTurn)    color = "#008000";
    else                        color = "#8B0000";
    ui->planLabelTurn->setStyleSheet("QLabel {background-color: transparent; color: " + color + ";}");

    if(viewTurn==0) ui->planLabelTurn->setText("--");
    else            ui->planLabelTurn->setText("T" + QString::number((viewTurn+1)/2));
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
}


void PlanHandler::loadViewBoard()
{
    ui->planGraphicsView->removeAll();
    updateViewCardZoneSpots();

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

    foreach(ArrowGraphicsItem *arrow, viewBoard->arrows)
    {
        ui->planGraphicsView->scene()->addItem(arrow);
    }

    foreach(CardGraphicsItem *card, viewBoard->playerHandList)
    {
        ui->planGraphicsView->scene()->addItem(card);
    }

    ui->planGraphicsView->updateView(std::max(viewBoard->playerMinions.count(), viewBoard->enemyMinions.count()));
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

    //Temporal
    if(!turnBoards.isEmpty())
    {
        viewBoard = turnBoards.first();
        loadViewBoard();
        updateButtons();
    }
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


void PlanHandler::endGame(bool playerWon)
{
    addHeroDeadToLastTurn(playerWon);
    unlockPlanInterface();
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
