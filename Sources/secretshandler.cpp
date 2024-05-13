#include "secretshandler.h"
#include <QtWidgets>

SecretsHandler::SecretsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler, PlanHandler *planHandler) : QObject(parent)
{
    this->ui = ui;
    this->enemyHandHandler = enemyHandHandler;
    this->planHandler = planHandler;
    this->patreonVersion = false;
    this->secretsAnimating = false;
    this->showSecrets = true;
    this->showWildSecrets = false;
    this->lastMinionDead = "";
    this->lastMinionPlayed = "";
    this->lastSpellPlayed = "";
    this->playerCardsDrawn = 0;
    this->enemyMinionsDeadThisTurn = 0;
    this->enemyMinionsAliveForAvenge = -1;
    this->playerCardsPlayedThisTurn = 0;
    this->isPlayerTurn = false;
    this->cardsPickratesMap = nullptr;

    completeUI();
    createSecretsByPickrate();
}

SecretsHandler::~SecretsHandler()
{

}


void SecretsHandler::completeUI()
{
    ui->secretsListWidget->setHidden(true);
    ui->secretsListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->secretsListWidget->setMouseTracking(true);
    ui->secretsListWidget->setFixedHeight(0);

    connect(ui->secretsListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findSecretCardEntered(QListWidgetItem*)));
}


void SecretsHandler::setArenaSets(QStringList arenaSets)
{
    this->arenaSets = arenaSets;
}


void SecretsHandler::resetLastMinionDead(QString code, QString subType)
{
    (void) code;
    //Duplica el primer esbirro que muera despues de una accion del usuario (!TRIGGER)
    if(subType != "TRIGGER")
    {
        this->lastMinionDead.clear();

        //El ManaBind es rebelado justo antes del POWER del hechizo lanzado
        if(subType != "PLAY")   this->lastSpellPlayed.clear();
    }
    //No podemos resetear lasMinionPlayed porque entre que se invoca el minion y se desvela el secreto ocurren
    //los tres subType POWER/PLAY/TRIGGER
}


void SecretsHandler::adjustSize()
{
    if(secretsAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustSize()));
        return;
    }

    int rowHeight = ui->secretsListWidget->sizeHintForRow(0);
    int height = ui->secretsListWidget->count()*rowHeight + 2*ui->secretsListWidget->frameWidth();
    int maxHeight = (ui->secretsListWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->secretsListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->secretsListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->secretsListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->secretsListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->secretsAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearSecretsAnimating()));
}


void SecretsHandler::clearSecretsAnimating()
{
    this->secretsAnimating = false;
    if(activeSecretList.empty())    ui->secretsListWidget->setHidden(true);
}


void SecretsHandler::secretStolen(int id, QString code, LoadingScreenState loadingScreenState)
{
    QList<CardClass> classList = Utility::getClassFromCode(code);
    knownSecretPlayed(id, classList.first(), code, loadingScreenState);
}


void SecretsHandler::secretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState)
{
    HandCard *handCard = enemyHandHandler->getHandCard(id);

    if(handCard != nullptr)
    {
        QString code = handCard->getCode();
        QString createdByCode = handCard->getCreatedByCode();

        //Secreto conocido
        if(!code.isEmpty())
        {
            if(Utility::isASecret(code))    knownSecretPlayed(id, hero, code, loadingScreenState);
            else                            return;
        }
        //Pocion de polimorfia
        else if(Utility::codeEqConstant(createdByCode, KABAL_CHEMIST))
        {
            knownSecretPlayed(id, hero, POTION_OF_POLIMORPH, loadingScreenState);
        }
        //Deck Card
        else
        {
            unknownSecretPlayed(id, hero, loadingScreenState, createdByCode);
        }
    }
    //Created by some TRIGGER
    else
    {
        unknownSecretPlayed(id, hero, loadingScreenState, enemyHandHandler->getLastCreatedByCode());
    }
}


void SecretsHandler::knownSecretPlayed(int id, CardClass hero, QString code, LoadingScreenState loadingScreenState)
{
    emit pDebug("Secret known played. Code: " + code);

    //No puede haber dos secretos iguales discard en los demas
    //Eliminar de las opciones, secretos que ya hemos reducido a 1 opcion
    //No es necesario ya que visualmente esa opcion tiene que estar activa y cuando se desvele ese secreto se eliminara como opcion de los demas

    bool inArena = (loadingScreenState == arena);

    ActiveSecret *activeSecret = getActiveSecret(hero, inArena);
    ActiveSecretId activeSecretId;
    activeSecretId.id = id;
    activeSecretId.codes.append(code);
    activeSecret->activeSecretIds.append(activeSecretId);

    for(QList<SecretCard>::iterator it = activeSecret->children.begin(); it != activeSecret->children.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->remaining = it->total = 1;
        }
        it->draw();
    }

    emit isolatedSecret(id, code);

    updateShowSecrets();
    adjustSize();
}


bool SecretsHandler::isFromArenaSets(QString code)
{
    QString cardSet = Utility::getCardAttribute(code, "set").toString();
    return arenaSets.contains(cardSet);
}


bool SecretsHandler::unknownSecretPlayedAddOption(QString code, bool inArena, ActiveSecret &activeSecret, QString manaText)
{
    QString coreCode = "CORE_" + code;
    if
    (
        (inArena && isFromArenaSets(coreCode)) ||
        (!inArena && Utility::isFromStandardSet(coreCode))
    )
    {
        emit pDebug("Secret option: " + coreCode);
        activeSecret.children.append(SecretCard(coreCode, manaText));
        return true;
    }
    else if
    (
        (inArena && isFromArenaSets(code)) ||
        (!inArena && (Utility::isFromStandardSet(code) || (showWildSecrets && patreonVersion)))
    )
    {
        emit pDebug("Secret option: " + code);
        activeSecret.children.append(SecretCard(code, manaText));
        return true;
    }
    return false;
}


//Devuelve o crea las opciones de secretos del heroe. No modifica su numero (total/remaining) ni rellena su id + codes
ActiveSecret * SecretsHandler::getActiveSecret(CardClass hero, bool inArena)
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        if(hero == activeSecret.hero)
        {
            return &activeSecret;
        }
    }

    ActiveSecret activeSecret;
    activeSecret.hero = hero;

    //Opciones ordenadas por pickrate en arena
    if(inArena)
    {
        for(const QString &code: qAsConst(secretsByPickrate[hero]))
        {
            QString text;
            if(cardsPickratesMap == nullptr)
            {
                text = "-";
            }
            else
            {
                text = QString::number((int)round(cardsPickratesMap[hero][update2HSRcode(code, hero)]))+"%";
            }
            unknownSecretPlayedAddOption(code, inArena, activeSecret, text);
        }
    }
    //Opciones ordenadas por rareza
    else
    {
        switch(hero)
        {
        //NUEVO SECRETO 1 - construido - si dudamos ponlo primero
            case PALADIN:
                //if(loadingScreenState == arena && !discover) activeSecret.children.append(SecretCard(HAND_OF_SALVATION));//Arena event
                //COMMON
                unknownSecretPlayedAddOption(NOBLE_SACRIFICE, inArena, activeSecret);
                unknownSecretPlayedAddOption(JUDGMENT_OF_JUSTICE, inArena, activeSecret);
                unknownSecretPlayedAddOption(AUTODEFENSE_MATRIX, inArena, activeSecret);
                unknownSecretPlayedAddOption(AVENGE, inArena, activeSecret);
                unknownSecretPlayedAddOption(REDEMPTION, inArena, activeSecret);
                unknownSecretPlayedAddOption(REPENTANCE, inArena, activeSecret);
                unknownSecretPlayedAddOption(NEVER_SURRENDER, inArena, activeSecret);
                unknownSecretPlayedAddOption(SACRED_TRIAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(GALLOPING_SAVIOR, inArena, activeSecret);
                unknownSecretPlayedAddOption(EYE_FOR_AN_EYE, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(GETAWAY_KODO, inArena, activeSecret);
                unknownSecretPlayedAddOption(COMPETITIVE_SPIRIT, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(RECKONING, inArena, activeSecret);
                unknownSecretPlayedAddOption(HIDDEN_WISDOM, inArena, activeSecret);
                unknownSecretPlayedAddOption(OH_MY_YOGG, inArena, activeSecret);
            break;

            case HUNTER:
                //COMMON
                unknownSecretPlayedAddOption(FREEZING_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(EXPLOSIVE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(BAIT_AND_SWITCH, inArena, activeSecret);
                unknownSecretPlayedAddOption(BEAR_TRAP, inArena, activeSecret);
                if(!unknownSecretPlayedAddOption(SNIPE, inArena, activeSecret)) unknownSecretPlayedAddOption(SNIPE2, inArena, activeSecret);
                unknownSecretPlayedAddOption(ZOMBEEEES, inArena, activeSecret);
                unknownSecretPlayedAddOption(PRESSURE_PLATE, inArena, activeSecret);
                unknownSecretPlayedAddOption(DART_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(OPEN_THE_CAGES, inArena, activeSecret);
                unknownSecretPlayedAddOption(MOTION_DENIED, inArena, activeSecret);
                unknownSecretPlayedAddOption(BARGAIN_BIN, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(PACK_TACTICS, inArena, activeSecret);
                unknownSecretPlayedAddOption(WANDERING_MONSTER, inArena, activeSecret);
                unknownSecretPlayedAddOption(VENOMSTRIKE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(CAT_TRICK, inArena, activeSecret);
                unknownSecretPlayedAddOption(MISDIRECTION, inArena, activeSecret);
                unknownSecretPlayedAddOption(HIDDEN_CACHE, inArena, activeSecret);
                unknownSecretPlayedAddOption(HIDDEN_MEANING, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(EMERGENCY_MANEUVERS, inArena, activeSecret);
                unknownSecretPlayedAddOption(SNAKE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(ICE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(RAT_TRAP, inArena, activeSecret);
            break;

            case MAGE:
                //COMMON
                unknownSecretPlayedAddOption(NETHERWIND_PORTAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(MIRROR_ENTITY, inArena, activeSecret);
                unknownSecretPlayedAddOption(FROZEN_CLONE, inArena, activeSecret);
                unknownSecretPlayedAddOption(OASIS_ALLY, inArena, activeSecret);
                unknownSecretPlayedAddOption(DDUPLICATE, inArena, activeSecret);
                unknownSecretPlayedAddOption(FLAME_WARD, inArena, activeSecret);
                unknownSecretPlayedAddOption(ICE_BARRIER, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(EXPLOSIVE_RUNES, inArena, activeSecret);
                unknownSecretPlayedAddOption(POTION_OF_POLIMORPH, inArena, activeSecret);
                unknownSecretPlayedAddOption(EFFIGY, inArena, activeSecret);
                unknownSecretPlayedAddOption(VAPORIZE, inArena, activeSecret);
                unknownSecretPlayedAddOption(COUNTERSPELL, inArena, activeSecret);
                unknownSecretPlayedAddOption(MANA_BIND, inArena, activeSecret);
                unknownSecretPlayedAddOption(OBJECTION, inArena, activeSecret);
                unknownSecretPlayedAddOption(SUMMONING_WARD, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(AZERITE_VEIN, inArena, activeSecret);
                unknownSecretPlayedAddOption(VENGEFUL_VISAGE, inArena, activeSecret);
                unknownSecretPlayedAddOption(SPLITTING_IMAGE, inArena, activeSecret);
                unknownSecretPlayedAddOption(SPELLBENDER, inArena, activeSecret);
                unknownSecretPlayedAddOption(RIGGED_FAIRE_GAME, inArena, activeSecret);
                unknownSecretPlayedAddOption(ICE_BLOCK, inArena, activeSecret);
            break;

            case ROGUE:
                //COMMON
                unknownSecretPlayedAddOption(STICKY_SITUATION, inArena, activeSecret);
                unknownSecretPlayedAddOption(DOUBLE_CROSS, inArena, activeSecret);
                unknownSecretPlayedAddOption(DIRTY_TRICKS, inArena, activeSecret);
                unknownSecretPlayedAddOption(SUDDEN_BETRAYAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(CHEAT_DEATH, inArena, activeSecret);
                unknownSecretPlayedAddOption(PLAGIARIZE, inArena, activeSecret);
                unknownSecretPlayedAddOption(PERJURY, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(SHADOW_CLONE, inArena, activeSecret);
                unknownSecretPlayedAddOption(AMBUSH, inArena, activeSecret);
                unknownSecretPlayedAddOption(SHENANIGANS, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(KIDNAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(BAMBOOZLE, inArena, activeSecret);
                unknownSecretPlayedAddOption(EVASION, inArena, activeSecret);
            break;

            default:
            break;
        }
    }

    //Creamos los QListWidgetItem
    for(QList<SecretCard>::iterator it = activeSecret.children.begin(); it != activeSecret.children.end(); it++)
    {
        it->remaining = it->total = 0;
        it->listItem = new QListWidgetItem(ui->secretsListWidget);
        emit checkCardImage(it->getCode());
    }

    activeSecretList.append(activeSecret);
    return &activeSecretList.last();
}


void SecretsHandler::unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState, QString createdByCode)
{
    emit pDebug("Secret unknown played. Hero: " + QString::number(hero));

    bool inArena = (loadingScreenState == arena);

    ActiveSecret *activeSecret = getActiveSecret(hero, inArena);
    ActiveSecretId activeSecretId;
    activeSecretId.id = id;

    for(QList<SecretCard>::iterator it = activeSecret->children.begin(); it != activeSecret->children.end(); it++)
    {
        activeSecretId.codes.append(it->getCode());
        it->remaining = it->total = 1;
        it->draw();
    }

    //Mostramos secretos generados por otras cartas
    if(!createdByCode.isEmpty())
    {
        activeSecretId.parent.setCreatedByCode(createdByCode);
        activeSecretId.parent.listItem = new QListWidgetItem();
        ui->secretsListWidget->insertItem(0, activeSecretId.parent.listItem);
        activeSecretId.parent.draw();
        emit checkCardImage(createdByCode);
    }

    activeSecret->activeSecretIds.append(activeSecretId);

    updateShowSecrets();
    adjustSize();
}


void SecretsHandler::redrawDownloadedCardImage(QString code)
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        for(SecretCard &secretCard: activeSecret.children)
        {
            if(secretCard.getCode() == code)    secretCard.draw();
        }
        for(ActiveSecretId &activeSecretId: activeSecret.activeSecretIds)
        {
            if(activeSecretId.parent.getCreatedByCode() == code)     activeSecretId.parent.draw();
        }
    }
}


void SecretsHandler::redrawClassCards()
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        for(SecretCard &secretCard: activeSecret.children)
        {
            secretCard.draw();
        }
        for(ActiveSecretId &activeSecretId: activeSecret.activeSecretIds)
        {
            activeSecretId.parent.draw();
        }
    }
}


void SecretsHandler::redrawSpellWeaponCards()
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        for(SecretCard &secretCard: activeSecret.children)
        {
            CardType cardType = secretCard.getType();
            if(cardType == SPELL || cardType == WEAPON)   secretCard.draw();
        }
        for(ActiveSecretId &activeSecretId: activeSecret.activeSecretIds)
        {
            activeSecretId.parent.draw();
        }
    }
}


void SecretsHandler::redrawAllCards()
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        for(SecretCard &secretCard: activeSecret.children)
        {
            secretCard.draw();
        }
        for(ActiveSecretId &activeSecretId: activeSecret.activeSecretIds)
        {
            activeSecretId.parent.draw();
        }
    }
}


void SecretsHandler::resetSecretsInterface()
{
    ui->secretsListWidget->setHidden(true);
    ui->secretsListWidget->clear();
    ui->secretsListWidget->setFixedHeight(0);
    activeSecretList.clear();
    secretTestQueue.clear();
    magneticPlayedQueue.clear();
}


void SecretsHandler::secretRevealed(int id, QString code)
{
    emit pDebug("Secret revealed: " + code);

    QList<CardClass> cardClass = Utility::getClassFromCode(code);
    for(int i=0; i<activeSecretList.count(); i++)
    {
        if(activeSecretList[i].hero == cardClass[0] || (cardClass.count()>1 && activeSecretList[i].hero == cardClass[1]))
        {
            //Eliminamos la id
            for(int j=0; j<activeSecretList[i].activeSecretIds.count(); j++)
            {
                if(activeSecretList[i].activeSecretIds[j].id == id)
                {
                    if(activeSecretList[i].activeSecretIds[j].parent.listItem != nullptr)
                    {
                        delete activeSecretList[i].activeSecretIds[j].parent.listItem;
                        activeSecretList[i].activeSecretIds[j].parent.listItem = nullptr;
                    }
                    activeSecretList[i].activeSecretIds.removeAt(j);
                    break;
                }
            }

            //Borramos el activeSecret si esta vacio
            if(activeSecretList[i].activeSecretIds.isEmpty())
            {
                for(SecretCard &secretCard: activeSecretList[i].children)
                {
                    delete secretCard.listItem;
                }
                activeSecretList.removeAt(i);
                adjustSize();
                break;
            }
            //Descartamos la opcion de secreto y recalculamos las opciones restantes
            else
            {
                discardSecretOptionNow(code);

                for(SecretCard &secretCard: activeSecretList[i].children)
                {
                    if(secretCard.remaining > 0)
                    {
                        bool codeFound = false;
                        QString scode = secretCard.getCode();
                        for(ActiveSecretId &activeSecretId: activeSecretList[i].activeSecretIds)
                        {
                            if(activeSecretId.codes.contains(scode))
                            {
                                codeFound = true;
                                break;
                            }
                        }

                        if(!codeFound)
                        {
                            secretCard.remaining = secretCard.total = 0;
                            secretCard.draw();
                        }
                    }
                }
            }
        }
    }


    for(int i=0; i<secretTestQueue.count(); i++)
    {
        secretTestQueue[i].secretRevealedLastSecond = true;
    }


    //NUEVO SECRETO 2 - cartas a mano
    //Reveal cards in Hand
    if(Utility::codeEqConstant(code, GETAWAY_KODO) && !lastMinionDead.isEmpty())       emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(Utility::codeEqConstant(code, MANA_BIND) && !lastSpellPlayed.isEmpty())    emit revealCreatedByCard(lastSpellPlayed, code, 1);
    else if(Utility::codeEqConstant(code, FROZEN_CLONE) && !lastMinionPlayed.isEmpty())emit revealCreatedByCard(lastMinionPlayed, code, 2);
    else if(Utility::codeEqConstant(code, CHEAT_DEATH) && !lastMinionDead.isEmpty())   emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(Utility::codeEqConstant(code, DDUPLICATE) && !lastMinionDead.isEmpty())    emit revealCreatedByCard(lastMinionDead, code, 2);
    else if(Utility::codeEqConstant(code, ICE_TRAP) && !lastSpellPlayed.isEmpty())     emit revealCreatedByCard(lastSpellPlayed, code, 1);
}


void SecretsHandler::discardSecretOptionDelay()
{
    if(secretTestQueue.isEmpty())   return;

    SecretTest secretTest = secretTestQueue.dequeue();
    if(secretTest.secretRevealedLastSecond)
    {
        emit pDebug("Option not discarded: " + secretTest.code + " (A secret revealed)");
        return;
    }

    discardSecretOptionNow(secretTest.code);
}


void SecretsHandler::discardSecretOptionNow(const QString &code)
{
    QString otherCode = Utility::otherCodeConstant(code);
    discardSecretOptionNow2(code);
    discardSecretOptionNow2(otherCode);
}


void SecretsHandler::discardSecretOptionNow2(const QString &code)
{
    emit pDebug("Option discarded: " + code);

    QList<CardClass> cardClass = Utility::getClassFromCode(code);
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        if(it->hero == cardClass[0] || (cardClass.count()>1 && it->hero == cardClass[1]))
        {
            for(SecretCard &secretCard: it->children)
            {
                if(secretCard.getCode() == code)
                {
                    secretCard.remaining = secretCard.total = 0;
                    secretCard.draw();
                    break;
                }
            }

            for(ActiveSecretId &activeSecretId: it->activeSecretIds)
            {
                activeSecretId.codes.removeOne(code);

                if(activeSecretId.codes.count() == 1)
                {
                    emit isolatedSecret(activeSecretId.id, activeSecretId.codes.first());
                }

                //Eliminar de las opciones, secretos que ya hemos reducido a 1 opcion
                //No es necesario ya que visualmente esa opcion tiene que estar activa y cuando se desvele ese secreto se eliminara como opcion de los demas
            }
        }
    }
}


void SecretsHandler::discardSecretOption(QString code, int delay)
{
    if(activeSecretList.isEmpty()){}
    else if(activeSecretList.count() == 1)
    {
        discardSecretOptionNow(code);
    }
    else
    {
        SecretTest secretTest;
        secretTest.code = code;
        secretTest.secretRevealedLastSecond = false;
        secretTestQueue.enqueue(secretTest);

        QTimer::singleShot(delay, this, SLOT(discardSecretOptionDelay()));
    }
}


//NUEVO SECRETO 3 condicion trigger
void SecretsHandler::playerWeaponPlayed(int id, QString code)
{
    Q_UNUSED(id);
    Q_UNUSED(code);

    discardSecretOptionNow(BARGAIN_BIN);
}

void SecretsHandler::playerSpellPlayed(int id, QString code)
{
    Q_UNUSED(id);

    if(lastSpellPlayed.isEmpty())    lastSpellPlayed = code;

    discardSecretOptionNow(COUNTERSPELL);
    discardSecretOptionNow(MANA_BIND);
    discardSecretOptionNow(NETHERWIND_PORTAL);

    discardSecretOptionNow(CAT_TRICK);
    discardSecretOptionNow(PRESSURE_PLATE);
    discardSecretOptionNow(ICE_TRAP);
    discardSecretOptionNow(BARGAIN_BIN);

    discardSecretOptionNow(NEVER_SURRENDER);
    discardSecretOptionNow(OH_MY_YOGG);

    discardSecretOptionNow(DIRTY_TRICKS);
    discardSecretOptionNow(STICKY_SITUATION);
}


void SecretsHandler::playerSpellObjMinionPlayed()
{
    discardSecretOption(SPELLBENDER);//Ocultado por COUNTERSPELL
}


void SecretsHandler::playerSpellObjHeroPlayed()
{
    discardSecretOptionNow(EVASION);
}


void SecretsHandler::playerBattlecryObjHeroPlayed()
{
    discardSecretOptionNow(EVASION);
}


void SecretsHandler::playerHeroPower()
{
    discardSecretOptionNow(DART_TRAP);
}


void SecretsHandler::newTurn(bool isPlayerTurn, int numTurn, int enemyMinions)
{
    this->isPlayerTurn = isPlayerTurn;
    this->playerCardsDrawn = 0;
    this->enemyMinionsDeadThisTurn = 0; //Hand of salvation testing
    this->playerCardsPlayedThisTurn = 0; //Rat trap/Hidden wisdom testing
    if(!isPlayerTurn)
    {
        discardSecretOptionNow(PLAGIARIZE);
        discardSecretOptionNow(PERJURY);

        if(enemyMinions > 0)
        {
            discardSecretOptionNow(COMPETITIVE_SPIRIT);
            discardSecretOptionNow(SUMMONING_WARD);
        }
        if(enemyMinions > 1)    discardSecretOptionNow(OPEN_THE_CAGES);

        if(planHandler->isEnemyHeroHealthChanged()) discardSecretOptionNow(RIGGED_FAIRE_GAME);
    }

    Q_UNUSED(numTurn)
}


void SecretsHandler::playerAllManaSpent()
{
    discardSecretOptionNow(DOUBLE_CROSS);
    discardSecretOptionNow(HIDDEN_MEANING);
}


void SecretsHandler::playerCardDraw()
{
    this->playerCardsDrawn++;
    if(isPlayerTurn && playerCardsDrawn>1) discardSecretOptionNow(SHENANIGANS);
}


void SecretsHandler::playerMinionPlayed(QString code, int id, int playerMinions)
{
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    if(mechanics.contains(QJsonValue("MODULAR")))
    {
        //Workaround for magnetic minions discarding secrets like mirror entity when played on magnetize mode
        //https://github.com/supertriodo/Arena-Tracker/issues/112
        MagneticPlayed magneticPlayed;
        magneticPlayed.code = code;
        magneticPlayed.id = id;
        magneticPlayed.playerMinions = playerMinions;
        magneticPlayedQueue.enqueue(magneticPlayed);

        QTimer::singleShot(3000, this, SLOT(playerMinionPlayedDelay()));
    }
    else
    {
        playerMinionPlayedNow(code, playerMinions);
    }
}


void SecretsHandler::playerMinionPlayedDelay()
{
    if(magneticPlayedQueue.isEmpty())   return;

    MagneticPlayed magneticPlayed = magneticPlayedQueue.dequeue();
    if(planHandler->isMinionOnBoard(true, magneticPlayed.id))
    {
        playerMinionPlayedNow(magneticPlayed.code, magneticPlayed.playerMinions);
    }
    else
    {
        emit pDebug("Minion played secrets not discarded (Magnetized minion played)");
    }
}


void SecretsHandler::playerMinionPlayedNow(QString code, int playerMinions)
{
    lastMinionPlayed = code;

    discardSecretOptionNow(FROZEN_CLONE);//No necesita objetivo
    discardSecretOption(MIRROR_ENTITY);//Ocultado por EXPLOSIVE_RUNES
    discardSecretOption(POTION_OF_POLIMORPH);//Ocultado por EXPLOSIVE_RUNES
    discardSecretOption(EXPLOSIVE_RUNES);//Ocultado por OBJECTION
    discardSecretOption(OBJECTION);//Ocultado por EXPLOSIVE_RUNES

    discardSecretOption(SNIPE);//Ocultado por ZOMBEEEES
    discardSecretOption(SNIPE2);
    discardSecretOption(ZOMBEEEES);//Ocultado por SNIPE
    discardSecretOptionNow(HIDDEN_CACHE);//No necesita objetivo
    discardSecretOptionNow(BARGAIN_BIN);//No necesita objetivo

    if(playerMinions>3)
    {
        discardSecretOptionNow(SACRED_TRIAL);
        discardSecretOption(REPENTANCE);//Ocultado por SACRED_TRIAL
    }
    else    discardSecretOptionNow(REPENTANCE);

    discardSecretOptionNow(AMBUSH);//No necesita objetivo
    discardSecretOptionNow(KIDNAP);
}


void SecretsHandler::enemyMinionGraveyard(int id, QString code, bool isPlayerTurn, int enemyMinions)
{
    Q_UNUSED(id);

    if(!isPlayerTurn)   return;

    if(lastMinionDead.isEmpty())    lastMinionDead = code;

    checkHandOfSalvation();
    checkAvenge(enemyMinions);

    discardSecretOptionNow(DDUPLICATE);
    discardSecretOptionNow(EFFIGY);
    discardSecretOptionNow(REDEMPTION);
    discardSecretOptionNow(GETAWAY_KODO);

    discardSecretOptionNow(CHEAT_DEATH);

    discardSecretOptionNow(EMERGENCY_MANEUVERS);
}


void SecretsHandler::checkHandOfSalvation()
{
    enemyMinionsDeadThisTurn++;
    if(enemyMinionsDeadThisTurn > 1)
    {
        emit pDebug("Hand of salvation tested: This turn died: " + QString::number(enemyMinionsDeadThisTurn));
        discardSecretOptionNow(HAND_OF_SALVATION);
    }
}


void SecretsHandler::checkAvenge(int enemyMinions)
{
    if(enemyMinionsAliveForAvenge == -1)
    {
        enemyMinionsAliveForAvenge = enemyMinions;
        QTimer::singleShot(1000, this, SLOT(checkAvengeDelay()));
    }
    else    enemyMinionsAliveForAvenge--;
}


void SecretsHandler::checkAvengeDelay()
{
    if(enemyMinionsAliveForAvenge > 0)
    {
        discardSecretOptionNow(AVENGE);
        emit pDebug("Avenge tested: Survivors: " + QString::number(enemyMinionsAliveForAvenge));
    }
    else    emit pDebug("Avenge not tested: Survivors: " + QString::number(enemyMinionsAliveForAvenge));
    enemyMinionsAliveForAvenge = -1;
}


void SecretsHandler::playerCardPlayed(int id, QString code, bool discard, bool isPlayerTurn)
{
    if(isPlayerTurn && !discard)
    {
        playerCardsPlayedThisTurn++;

        //Secrets 3+ cards
        if(playerCardsPlayedThisTurn > 2)
        {
            discardSecretOptionNow(GALLOPING_SAVIOR);
            discardSecretOptionNow(HIDDEN_WISDOM);

            discardSecretOptionNow(RAT_TRAP);
            discardSecretOptionNow(MOTION_DENIED);
        }

        //Play card just draw
        if(planHandler->isRecentCard(id))   discardSecretOptionNow(AZERITE_VEIN);
    }

    Q_UNUSED(id);
    Q_UNUSED(code);
}


/*
 * http://hearthstone.gamepedia.com/Secret
 *
 * If a Secret removes the specific target for another Secret which was already triggered, the second Secret will not take effect,
 * since it now lacks a target. For example, if Freezing Trap removes the minion which would have been the target of Misdirection,
 * the Misdirection will not trigger, since it no longer has a target.
 *
 * Note that this rule only applies for Secrets which require specific targets; Secrets such as Explosive Trap and Snake Trap do not require targets,
 * and will always take effect once triggered, even if the original trigger minion has been removed from play.
 */
void SecretsHandler::playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions, int id1, int id2)
{
    if(isHeroFrom)
    {
        //Hero -> hero
        if(isHeroTo)
        {
            discardSecretOptionNow(ICE_BARRIER);

            discardSecretOptionNow(EXPLOSIVE_TRAP);//No necesita objetivo
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(MISDIRECTION);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOptionNow(WANDERING_MONSTER);//No necesita objetivo

            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);

            discardSecretOptionNow(EVASION);
        }
        //Hero -> minion
        else
        {
            discardSecretOptionNow(VENOMSTRIKE_TRAP);
            discardSecretOptionNow(SNAKE_TRAP);
            discardSecretOptionNow(PACK_TACTICS);
            discardSecretOptionNow(BAIT_AND_SWITCH);

            discardSecretOptionNow(NOBLE_SACRIFICE);
            discardSecretOptionNow(AUTODEFENSE_MATRIX);

            discardSecretOptionNow(SPLITTING_IMAGE);
            discardSecretOptionNow(OASIS_ALLY);

            discardSecretOptionNow(BAMBOOZLE);
        }
    }
    else
    {
        //Minion -> hero
        if(isHeroTo)
        {
            discardSecretOption(FLAME_WARD);//Ocultado por VAPORIZE
            discardSecretOption(VAPORIZE);//Ocultado por FLAME_WARD
            discardSecretOptionNow(ICE_BARRIER);//No necesita objetivo
            discardSecretOption(VENGEFUL_VISAGE);//Ocultado por VAPORIZE

            discardSecretOptionNow(EXPLOSIVE_TRAP);//No necesita objetivo
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(FREEZING_TRAP);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOption(MISDIRECTION);//Ocultado por FREEZING_TRAP y EXPLOSIVE_TRAP
            discardSecretOptionNow(WANDERING_MONSTER);//No necesita objetivo

            discardSecretOptionNow(NOBLE_SACRIFICE);
            discardSecretOptionNow(JUDGMENT_OF_JUSTICE);
            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            if(planHandler->isReckoningTested(isHeroTo, id1, id2))  discardSecretOption(RECKONING);//Ocultado por NOBLE_SACRIFICE (si el atacante tiene <= 2 health pq muere)

            if(playerMinions > 1)   discardSecretOptionNow(SUDDEN_BETRAYAL);
            discardSecretOption(EVASION);//Ocultado por SUDDEN_BETRAYAL
            discardSecretOption(SHADOW_CLONE);//Ocultado por SUDDEN_BETRAYAL
        }
        //Minion -> minion
        else
        {
            discardSecretOptionNow(FREEZING_TRAP);
            discardSecretOptionNow(VENOMSTRIKE_TRAP);
            discardSecretOptionNow(SNAKE_TRAP);
            discardSecretOptionNow(PACK_TACTICS);
            discardSecretOptionNow(BAIT_AND_SWITCH);

            discardSecretOptionNow(NOBLE_SACRIFICE);
            discardSecretOptionNow(JUDGMENT_OF_JUSTICE);
            discardSecretOptionNow(AUTODEFENSE_MATRIX);
            if(planHandler->isReckoningTested(isHeroTo, id1, id2))  discardSecretOption(RECKONING);//Ocultado por AUTODEFENSE_MATRIX

            discardSecretOptionNow(SPLITTING_IMAGE);
            discardSecretOptionNow(OASIS_ALLY);

            discardSecretOptionNow(BAMBOOZLE);
        }
    }
}


void SecretsHandler::findSecretCardEntered(QListWidgetItem * item)
{
    QString code = "";

    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end() && code.isEmpty(); it++)
    {
        for(SecretCard &secretCard: it->children)
        {
            if(secretCard.listItem == item)
            {
                code = secretCard.getCode();
                break;
            }
        }
        for(ActiveSecretId &activeSecretId: it->activeSecretIds)
        {
            if(activeSecretId.parent.listItem == item)
            {
                code = activeSecretId.parent.getCode();
                if(code.isEmpty())  code = activeSecretId.parent.getCreatedByCode();
                break;
            }
        }
    }

    QRect rectCard = ui->secretsListWidget->visualItemRect(item);
    QPoint posCard = ui->secretsListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int secretListTop = ui->secretsListWidget->mapToGlobal(QPoint(0,0)).y();
    int secretListBottom = ui->secretsListWidget->mapToGlobal(QPoint(0,ui->secretsListWidget->height())).y();
    emit cardEntered(code, globalRectCard, secretListTop, secretListBottom);
}


QStringList SecretsHandler::getSecretOptionCodes(int id)
{
    for(ActiveSecret &activeSecret: activeSecretList)
    {
        for(ActiveSecretId &activeSecretId: activeSecret.activeSecretIds)
        {
            if(activeSecretId.id == id)
            {
                return activeSecretId.codes;
            }
        }
    }
    return QStringList();
}


void SecretsHandler::setPremium(bool premium)
{
    this->patreonVersion = premium;
}


void SecretsHandler::setShowWildSecrets(bool value)
{
    this->showWildSecrets = value;
}


void SecretsHandler::setShowSecrets(bool value)
{
    this->showSecrets = value;
    updateShowSecrets();
}


void SecretsHandler::updateShowSecrets()
{
    if(!showSecrets || activeSecretList.empty())    ui->secretsListWidget->setHidden(true);
    else                                            ui->secretsListWidget->setHidden(false);
}


void SecretsHandler::setCardsPickratesMap(QMap<QString, float> cardsPickratesMap[])
{
    this->cardsPickratesMap = cardsPickratesMap;
}


//NUEVO SECRETO 4 arena - ponlo al final
void SecretsHandler::createSecretsByPickrate()
{
    secretsByPickrate[PALADIN] << NOBLE_SACRIFICE << AUTODEFENSE_MATRIX << AVENGE << REDEMPTION << REPENTANCE << NEVER_SURRENDER
                               << SACRED_TRIAL << EYE_FOR_AN_EYE << GETAWAY_KODO << COMPETITIVE_SPIRIT << HIDDEN_WISDOM
                               << OH_MY_YOGG << RECKONING << GALLOPING_SAVIOR << JUDGMENT_OF_JUSTICE;

    secretsByPickrate[HUNTER] << FREEZING_TRAP << EXPLOSIVE_TRAP << BEAR_TRAP << SNIPE << SNIPE2 << PRESSURE_PLATE << DART_TRAP
                              << PACK_TACTICS << WANDERING_MONSTER << VENOMSTRIKE_TRAP << CAT_TRICK << MISDIRECTION << HIDDEN_CACHE
                              << SNAKE_TRAP << RAT_TRAP << OPEN_THE_CAGES << ICE_TRAP << EMERGENCY_MANEUVERS << MOTION_DENIED
                              << ZOMBEEEES << HIDDEN_MEANING << BAIT_AND_SWITCH << BARGAIN_BIN;

    secretsByPickrate[MAGE] << NETHERWIND_PORTAL << MIRROR_ENTITY << FROZEN_CLONE << DDUPLICATE << FLAME_WARD << ICE_BARRIER
                            << EXPLOSIVE_RUNES << POTION_OF_POLIMORPH << EFFIGY << VAPORIZE << COUNTERSPELL << MANA_BIND
                            << SPLITTING_IMAGE << SPELLBENDER << ICE_BLOCK << RIGGED_FAIRE_GAME << OASIS_ALLY << VENGEFUL_VISAGE
                            << OBJECTION << AZERITE_VEIN << SUMMONING_WARD;

    secretsByPickrate[ROGUE] << DIRTY_TRICKS << SUDDEN_BETRAYAL << CHEAT_DEATH << AMBUSH << BAMBOOZLE << EVASION << PLAGIARIZE
                             << SHADOW_CLONE << SHENANIGANS << KIDNAP << STICKY_SITUATION << DOUBLE_CROSS << PERJURY;
}


QString SecretsHandler::update2HSRcode(QString code, CardClass cardClass)
{
    if(!cardsPickratesMap[cardClass].contains(code))
    {
        if(code.startsWith("CORE_") &&
            cardsPickratesMap[cardClass].contains(code.mid(5)))         code = code.mid(5);
        else if(cardsPickratesMap[cardClass].contains("CORE_" + code))  code = "CORE_" + code;
    }
    return code;
}


void SecretsHandler::sortSecretsByPickrate(const QMap<QString, float> cardsPickratesMap[])
{
    qSort(secretsByPickrate[PALADIN].begin(), secretsByPickrate[PALADIN].end(), [=](const QString &code1, const QString &code2)
    {
        QString code1m = update2HSRcode(code1, PALADIN);
        QString code2m = update2HSRcode(code2, PALADIN);
        return cardsPickratesMap[PALADIN][code1m] > cardsPickratesMap[PALADIN][code2m];
    });

    qSort(secretsByPickrate[HUNTER].begin(), secretsByPickrate[HUNTER].end(), [=](const QString &code1, const QString &code2)
    {
        QString code1m = update2HSRcode(code1, HUNTER);
        QString code2m = update2HSRcode(code2, HUNTER);
        return cardsPickratesMap[HUNTER][code1m] > cardsPickratesMap[HUNTER][code2m];
    });

    qSort(secretsByPickrate[MAGE].begin(), secretsByPickrate[MAGE].end(), [=](const QString &code1, const QString &code2)
    {
        QString code1m = update2HSRcode(code1, MAGE);
        QString code2m = update2HSRcode(code2, MAGE);
        return cardsPickratesMap[MAGE][code1m] > cardsPickratesMap[MAGE][code2m];
    });

    qSort(secretsByPickrate[ROGUE].begin(), secretsByPickrate[ROGUE].end(), [=](const QString &code1, const QString &code2)
    {
        QString code1m = update2HSRcode(code1, ROGUE);
        QString code2m = update2HSRcode(code2, ROGUE);
        return cardsPickratesMap[ROGUE][code1m] > cardsPickratesMap[ROGUE][code2m];
    });
}

/* Secrets interactions. Kobolds & Catacombs update.
Hey guys!
It's the time for the test of new secrets. In order to update Arena Tracker I need to play with all the combinations of secrets to know in which order they trigger and which one overlap with any other.

Secrets, same as deathrattle, trigger in the same order you played them. If a secret X rely on a target and that target was eliminated by a previous secret Y, triggered by the same action, then the secret X won't trigger.

The result of this is a bunch of data that may be of interest for some of you, so here it is:


Sudden Betrayal and Evasion:

Sudden Betrayal will only trigger when you have 2+ minions. If it triggers, Evasion will never be revealed as your hero doesn't receive any damage.


Explosive Runes, Frozen Clone, Mirror Entity and Potion of Polimorph:

Frozen Clone will always trigger as it doesn't rely on the target to be alive.

Explosive Runes will always trigger as neither of the other 3 secrets will remove the target (Potion of Polimorph just morph it).

Mirror Entity and Potion of Polimorph won't trigger if Explosive Runes killed the minion before.

If Explosive Runes triggers but don't kill the minion, Mirror Entity will copy the damaged minion.

If Potion of Polimorph triggers first, Mirror Entity will copy a 1/1 sheep, Frozen Clone will create two copies of a 1/1 sheep and Explosive Runes will deal 1 damage to the sheep and the rest to the opponent hero.


Freezing Trap, Explosive Trap, Missdirection and Wandering Monster:

Explosive Trap and Wandering Monster will always trigger as they don't rely on the target to be alive.

Freezing Trap won't trigger if Explosive Trap killed the minion before.

Misdirection won't trigger if Explosive Trap or Freezing Trap killed the minion before.
*/






