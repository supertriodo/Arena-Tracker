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


void SecretsHandler::setArenaSets(QStringList &arenaSets)
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
    int rows = 0;

    for(int i=0; i<activeSecretList.count(); i++)
    {
        rows += activeSecretList[i].children.count();
    }

    int height = rows*rowHeight + 2*ui->secretsListWidget->frameWidth();
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
    knownSecretPlayed(id, Utility::getClassFromCode(code), code, loadingScreenState);
}


void SecretsHandler::secretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState)
{
    HandCard *handCard = enemyHandHandler->getHandCard(id);

    if(handCard != nullptr)
    {
        QString code = handCard->getCode();
        QString createdByCode = handCard->getCreatedByCode();

        //Secreto conocido
        if(!code.isEmpty() && Utility::isASecret(code))
        {
            knownSecretPlayed(id, hero, code, loadingScreenState);
        }
        //Pocion de polimorfia
        else if(createdByCode == KABAL_CHEMIST)
        {
            knownSecretPlayed(id, hero, POTION_OF_POLIMORPH, loadingScreenState);
        }
        //Deck Card
        else
        {
            unknownSecretPlayed(id, hero, loadingScreenState);
        }
    }
    else
    {
        unknownSecretPlayed(id, hero, loadingScreenState);
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


void SecretsHandler::unknownSecretPlayedAddOption(QString code, bool inArena, ActiveSecret &activeSecret, QString manaText)
{
    if
    (
        (inArena && isFromArenaSets(code)) ||
        (!inArena && (Utility::isFromStandardSet(code) || (showWildSecrets && patreonVersion)))
    )
    {
        activeSecret.children.append(SecretCard(code, manaText));
    }
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
        for(const QString &code: secretsByPickrate[hero])
        {
            unknownSecretPlayedAddOption(code, inArena, activeSecret,
                                         (cardsPickratesMap == nullptr) ? "-" :
                                            QString::number((int)round(cardsPickratesMap[hero][code]))+"%");
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
                unknownSecretPlayedAddOption(AUTODEFENSE_MATRIX, inArena, activeSecret);
                unknownSecretPlayedAddOption(AVENGE, inArena, activeSecret);
                unknownSecretPlayedAddOption(REDEMPTION, inArena, activeSecret);
                unknownSecretPlayedAddOption(REPENTANCE, inArena, activeSecret);
                unknownSecretPlayedAddOption(NEVER_SURRENDER, inArena, activeSecret);
                unknownSecretPlayedAddOption(SACRED_TRIAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(EYE_FOR_AN_EYE, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(GETAWAY_KODO, inArena, activeSecret);
                unknownSecretPlayedAddOption(COMPETITIVE_SPIRIT, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(HIDDEN_WISDOM, inArena, activeSecret);
            break;

            case HUNTER:
                //COMMON
                unknownSecretPlayedAddOption(FREEZING_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(EXPLOSIVE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(BEAR_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(SNIPE, inArena, activeSecret);
                unknownSecretPlayedAddOption(PRESSURE_PLATE, inArena, activeSecret);
                unknownSecretPlayedAddOption(DART_TRAP, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(PACK_TACTICS, inArena, activeSecret);
                unknownSecretPlayedAddOption(WANDERING_MONSTER, inArena, activeSecret);
                unknownSecretPlayedAddOption(VENOMSTRIKE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(CAT_TRICK, inArena, activeSecret);
                unknownSecretPlayedAddOption(MISDIRECTION, inArena, activeSecret);
                unknownSecretPlayedAddOption(HIDDEN_CACHE, inArena, activeSecret);
                //EPIC
                unknownSecretPlayedAddOption(SNAKE_TRAP, inArena, activeSecret);
                unknownSecretPlayedAddOption(RAT_TRAP, inArena, activeSecret);
            break;

            case MAGE:
                //COMMON
                unknownSecretPlayedAddOption(NETHERWIND_PORTAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(MIRROR_ENTITY, inArena, activeSecret);
                unknownSecretPlayedAddOption(FROZEN_CLONE, inArena, activeSecret);
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
                //EPIC
                unknownSecretPlayedAddOption(SPLITTING_IMAGE, inArena, activeSecret);
                unknownSecretPlayedAddOption(SPELLBENDER, inArena, activeSecret);
                unknownSecretPlayedAddOption(ICE_BLOCK, inArena, activeSecret);
            break;

            case ROGUE:
                //COMMON
                unknownSecretPlayedAddOption(DIRTY_TRICKS, inArena, activeSecret);
                unknownSecretPlayedAddOption(SUDDEN_BETRAYAL, inArena, activeSecret);
                unknownSecretPlayedAddOption(CHEAT_DEATH, inArena, activeSecret);
                //RARE
                unknownSecretPlayedAddOption(AMBUSH, inArena, activeSecret);
                //EPIC
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


void SecretsHandler::unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState)
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

    activeSecret->activeSecretIds.append(activeSecretId);

    updateShowSecrets();
    adjustSize();
}


void SecretsHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        for(QList<SecretCard>::iterator it2 = it->children.begin(); it2 != it->children.end(); it2++)
        {
            if(it2->getCode() == code)    it2->draw();
        }
    }
}


void SecretsHandler::redrawClassCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        foreach(SecretCard secretCard, activeSecret.children)
        {
            if(secretCard.getCardClass()<NUM_HEROS)   secretCard.draw();
        }
    }
}


void SecretsHandler::redrawSpellWeaponCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        foreach(SecretCard secretCard, activeSecret.children)
        {
            CardType cardType = secretCard.getType();
            if(cardType == SPELL || cardType == WEAPON)   secretCard.draw();
        }
    }
}


void SecretsHandler::redrawAllCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        foreach(SecretCard secretCard, activeSecret.children)
        {
            secretCard.draw();
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

    CardClass hero = Utility::getClassFromCode(code);
    for(int i=0; i<activeSecretList.count(); i++)
    {
        if(activeSecretList[i].hero == hero)
        {
            //Eliminamos la id
            for(int j=0; j<activeSecretList[i].activeSecretIds.count(); j++)
            {
                if(activeSecretList[i].activeSecretIds[j].id == id)
                {
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
    if(code == GETAWAY_KODO && !lastMinionDead.isEmpty())       emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(code == MANA_BIND && !lastSpellPlayed.isEmpty())    emit revealCreatedByCard(lastSpellPlayed, code, 1);
    else if(code == FROZEN_CLONE && !lastMinionPlayed.isEmpty())emit revealCreatedByCard(lastMinionPlayed, code, 2);
    else if(code == CHEAT_DEATH && !lastMinionDead.isEmpty())   emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(code == DDUPLICATE && !lastMinionDead.isEmpty())    emit revealCreatedByCard(lastMinionDead, code, 2);
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


void SecretsHandler::discardSecretOptionNow(QString code)
{
    emit pDebug("Option discarded: " + code);

    CardClass hero = Utility::getClassFromCode(code);
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        if(it->hero == hero)
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
void SecretsHandler::playerSpellPlayed(QString code)
{
    if(lastSpellPlayed.isEmpty())    lastSpellPlayed = code;

    discardSecretOptionNow(COUNTERSPELL);
    discardSecretOptionNow(MANA_BIND);
    discardSecretOptionNow(NETHERWIND_PORTAL);

    discardSecretOptionNow(CAT_TRICK);
    discardSecretOptionNow(PRESSURE_PLATE);

    discardSecretOptionNow(NEVER_SURRENDER);

    discardSecretOptionNow(DIRTY_TRICKS);
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
    discardSecretOptionNow(EXPLOSIVE_RUNES);

    discardSecretOptionNow(SNIPE);
    discardSecretOptionNow(HIDDEN_CACHE);

    if(playerMinions>3)
    {
        discardSecretOptionNow(SACRED_TRIAL);
        discardSecretOption(REPENTANCE);//Ocultado por SACRED_TRIAL
    }
    else    discardSecretOptionNow(REPENTANCE);

    discardSecretOptionNow(AMBUSH);
}


void SecretsHandler::enemyMinionGraveyard(int id, QString code, bool isPlayerTurn)
{
    Q_UNUSED(id);

    if(!isPlayerTurn)   return;

    if(lastMinionDead.isEmpty())    lastMinionDead = code;

    discardSecretOptionNow(DDUPLICATE);
    discardSecretOptionNow(EFFIGY);
    discardSecretOptionNow(REDEMPTION);
    discardSecretOptionNow(GETAWAY_KODO);

    discardSecretOptionNow(CHEAT_DEATH);
}


void SecretsHandler::avengeTested()
{
    discardSecretOptionNow(AVENGE);
}


void SecretsHandler::handOfSalvationTested()
{
    discardSecretOptionNow(HAND_OF_SALVATION);
}


void SecretsHandler::_3CardsPlayedTested()
{
    discardSecretOptionNow(RAT_TRAP);
    discardSecretOptionNow(HIDDEN_WISDOM);
}


void SecretsHandler::cSpiritTested()
{
    discardSecretOptionNow(COMPETITIVE_SPIRIT);
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
void SecretsHandler::playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions)
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

            discardSecretOptionNow(NOBLE_SACRIFICE);
            discardSecretOptionNow(AUTODEFENSE_MATRIX);

            discardSecretOptionNow(SPLITTING_IMAGE);

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

            discardSecretOptionNow(EXPLOSIVE_TRAP);//No necesita objetivo
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(FREEZING_TRAP);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOption(MISDIRECTION);//Ocultado por FREEZING_TRAP y EXPLOSIVE_TRAP
            discardSecretOptionNow(WANDERING_MONSTER);//No necesita objetivo

            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);

            if(playerMinions > 1)   discardSecretOptionNow(SUDDEN_BETRAYAL);
            discardSecretOption(EVASION);//Ocultado por SUDDEN_BETRAYAL
        }
        //Minion -> minion
        else
        {
            discardSecretOptionNow(FREEZING_TRAP);
            discardSecretOptionNow(VENOMSTRIKE_TRAP);
            discardSecretOptionNow(SNAKE_TRAP);
            discardSecretOptionNow(PACK_TACTICS);

            discardSecretOptionNow(NOBLE_SACRIFICE);
            discardSecretOptionNow(AUTODEFENSE_MATRIX);

            discardSecretOptionNow(SPLITTING_IMAGE);

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
                              << SACRED_TRIAL << EYE_FOR_AN_EYE << GETAWAY_KODO << COMPETITIVE_SPIRIT << HIDDEN_WISDOM;

    secretsByPickrate[HUNTER] << FREEZING_TRAP << EXPLOSIVE_TRAP << BEAR_TRAP << SNIPE << PRESSURE_PLATE << DART_TRAP
                              << PACK_TACTICS << WANDERING_MONSTER << VENOMSTRIKE_TRAP << CAT_TRICK << MISDIRECTION << HIDDEN_CACHE
                              << SNAKE_TRAP << RAT_TRAP;

    secretsByPickrate[MAGE] << NETHERWIND_PORTAL << MIRROR_ENTITY << FROZEN_CLONE << DDUPLICATE << FLAME_WARD << ICE_BARRIER
                            << EXPLOSIVE_RUNES << POTION_OF_POLIMORPH << EFFIGY << VAPORIZE << COUNTERSPELL << MANA_BIND
                            << SPLITTING_IMAGE << SPELLBENDER << ICE_BLOCK;

    secretsByPickrate[ROGUE] << DIRTY_TRICKS << SUDDEN_BETRAYAL << CHEAT_DEATH << AMBUSH << BAMBOOZLE << EVASION;
}


void SecretsHandler::sortSecretsByPickrate(const QMap<QString, float> cardsPickratesMap[])
{
    qSort(secretsByPickrate[PALADIN].begin(), secretsByPickrate[PALADIN].end(), [=](const QString &code1, const QString &code2)
    {
        return cardsPickratesMap[PALADIN][code1] > cardsPickratesMap[PALADIN][code2];
    });

    qSort(secretsByPickrate[HUNTER].begin(), secretsByPickrate[HUNTER].end(), [=](const QString &code1, const QString &code2)
    {
        return cardsPickratesMap[HUNTER][code1] > cardsPickratesMap[HUNTER][code2];
    });

    qSort(secretsByPickrate[MAGE].begin(), secretsByPickrate[MAGE].end(), [=](const QString &code1, const QString &code2)
    {
        return cardsPickratesMap[MAGE][code1] > cardsPickratesMap[MAGE][code2];
    });

    qSort(secretsByPickrate[ROGUE].begin(), secretsByPickrate[ROGUE].end(), [=](const QString &code1, const QString &code2)
    {
        return cardsPickratesMap[ROGUE][code1] > cardsPickratesMap[ROGUE][code2];
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






