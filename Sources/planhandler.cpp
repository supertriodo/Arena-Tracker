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
        hero->changeHero(code, id);
    }
    else
    {
        hero = new HeroGraphicsItem(code, id, friendly, nowBoard->playerTurn);

        if(viewBoard == nowBoard)
        {
            ui->planGraphicsView->scene()->addItem(hero);
            ui->planGraphicsView->updateView(std::max(nowBoard->playerMinions.count(), nowBoard->enemyMinions.count()));
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


void PlanHandler::playerMinionTagChange(int id, QString code, QString tag, QString value)
{
    if(tag == "LINKED_ENTITY" && !code.isEmpty() &&
        nowBoard->playerHero!=NULL && nowBoard->playerHero->getId() == value.toInt())
    {
        addAddonToLastTurn(code, id, nowBoard->playerHero->getId(), Addon::AddonNeutral);
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

    //Minions
    MinionGraphicsItem * minion = findMinion(friendly, id);
    if(minion != NULL)
    {
        emit pDebug("Tag Change Minion: Id: " + QString::number(id) + " - " + tag + " --> " + value);
        checkAtkHealthChange(minion, friendly, tag, value);
        healing = minion->processTagChange(tag, value);
        isDead = minion->isDead();
        isHero = false;
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

    weapon = new WeaponGraphicsItem(code, id, friendly);

    if(viewBoard == nowBoard)   ui->planGraphicsView->scene()->addItem(weapon);

    if(friendly)    nowBoard->playerWeapon = weapon;
    else            nowBoard->enemyWeapon = weapon;

    emit checkCardImage(code, false);
    setLastTriggerId("", "", -1, -1);

    //Add addon to last turn
    HeroGraphicsItem* hero = friendly?nowBoard->playerHero:nowBoard->enemyHero;
    if(hero != NULL)    addAddonToLastTurn(code, id, hero->getId(), Addon::AddonNeutral);
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
    HeroPowerGraphicsItem* heroPower = friendly?nowBoard->playerHeroPower:nowBoard->enemyHeroPower;

    if(heroPower != NULL)
    {
        HeroGraphicsItem* hero = friendly?nowBoard->playerHero:nowBoard->enemyHero;
        if(hero != NULL)    addAddonToLastTurn(code, id, hero->getId(), Addon::AddonNeutral);
        heroPower->changeHeroPower(code, id);
    }
    else
    {
        heroPower = new HeroPowerGraphicsItem(code, id, friendly, nowBoard->playerTurn);

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
    forbiddenAddonList.append(SNAKE_TRAP);
    forbiddenAddonList.append(ARMORED_WARHORSE);
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
    forbiddenAddonList.append(GLADIATORS_LONGBOW);
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

    revealEnemyCardPrevTurns(id, code);
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
    for(int i=0; i<cardsList->count(); i++)
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


void PlanHandler::playerCardDraw(int id, QString code, int turn)
{
    cardDraw(true, id, code, "", turn);
}


void PlanHandler::enemyCardDraw(int id, QString code, QString createdByCode, int turn)
{
    cardDraw(false, id, code, createdByCode, turn);
}


void PlanHandler::cardDraw(bool friendly, int id, QString code, QString createdByCode, int turn)
{
    CardGraphicsItem *card = new CardGraphicsItem(id, code, createdByCode, turn);
    getHandList(friendly)->append(card);
    updateCardZoneSpots(friendly);

    if(viewBoard == nowBoard)       ui->planGraphicsView->scene()->addItem(card);
    if(!code.isEmpty())             emit checkCardImage(code, false);
    if(!createdByCode.isEmpty())    emit checkCardImage(createdByCode, false);

    //Show card draw last turn
    if(turnBoards.empty())  return;
    Board *board = turnBoards.last();
    if(board->numTurn == turn)
    {
        CardGraphicsItem *drawCard = new CardGraphicsItem(card);
        //Evitamos mostrar cartas robadas en el turno 1 pq el mulligan da problemas
        if(turn != 1)   drawCard->setDraw();
        getHandList(friendly, board)->append(drawCard);
        updateCardZoneSpots(friendly, board);
        if(viewBoard == board)      ui->planGraphicsView->scene()->addItem(drawCard);
    }
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
            card->setCode(code);
            emit checkCardImage(code, false);
        }
    }
}


void PlanHandler::lastEnemyHandCardIsCoin()
{
    nowBoard->enemyHandList.last()->setCode(THE_COIN);
    revealEnemyCardPrevTurns(nowBoard->enemyHandList.last()->getId(), THE_COIN);
}


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

    if(nowBoard->playerHero != NULL)        nowBoard->playerHero->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHero != NULL)         nowBoard->enemyHero->setPlayerTurn(playerTurn);

    if(nowBoard->playerHeroPower != NULL)   nowBoard->playerHeroPower->setPlayerTurn(playerTurn);
    if(nowBoard->enemyHeroPower != NULL)    nowBoard->enemyHeroPower->setPlayerTurn(playerTurn);


    //Store nowBoard
    if(this->firstStoredTurn == 0)
    {
        this->firstStoredTurn = numTurn;
    }
    updateButtons();

    Board *board = new Board();
    board->playerTurn = playerTurn;
    board->numTurn = numTurn;

    if(nowBoard->playerHero == NULL)    board->playerHero = NULL;
    else                                board->playerHero = new HeroGraphicsItem(nowBoard->playerHero);
    if(nowBoard->enemyHero == NULL)     board->enemyHero = NULL;
    else                                board->enemyHero = new HeroGraphicsItem(nowBoard->enemyHero);

    if(nowBoard->playerHeroPower == NULL)   board->playerHeroPower = NULL;
    else                                    board->playerHeroPower = new HeroPowerGraphicsItem(nowBoard->playerHeroPower);
    if(nowBoard->enemyHeroPower == NULL)    board->enemyHeroPower = NULL;
    else                                    board->enemyHeroPower = new HeroPowerGraphicsItem(nowBoard->enemyHeroPower);

    if(nowBoard->playerWeapon == NULL)  board->playerWeapon = NULL;
    else                                board->playerWeapon = new WeaponGraphicsItem(nowBoard->playerWeapon);
    if(nowBoard->enemyWeapon == NULL)   board->enemyWeapon = NULL;
    else                                board->enemyWeapon = new WeaponGraphicsItem(nowBoard->enemyWeapon);

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

    foreach(CardGraphicsItem * card, nowBoard->enemyHandList)
    {
        board->enemyHandList.append(new CardGraphicsItem(card));
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
    forbiddenCreatorList.append(SWORD_OF_JUSTICE);
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
    ui->planGraphicsView->updateView(std::max(viewBoard->playerMinions.count(), viewBoard->enemyMinions.count()));
    updateViewCardZoneSpots();

    ui->planGraphicsView->scene()->addItem(viewBoard->playerHero);

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

    ui->planGraphicsView->scene()->addItem(viewBoard->enemyHero);
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
