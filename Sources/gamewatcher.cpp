#include "gamewatcher.h"
#include <QtWidgets>


QMap<QString, QJsonObject> * GameWatcher::cardsJson;


GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    gameState = noGame;
    arenaMode = false;
//    deckRead = false;
    mulliganEnemyDone = false;
    turn = turnReal = 0;

    synchronized = false;
#ifdef QT_DEBUG
    synchronized = true;
#endif

    match = new QRegularExpressionMatch();

    QSettings settings("Arena Tracker", "Arena Tracker");
    this->playerTag = settings.value("playerTag", "").toString();
}


GameWatcher::~GameWatcher()
{
    delete match;
}


void GameWatcher::reset()
{
    gameState = noGame;
    emit pDebug("GameState = noGame");
    arenaMode = false;
}


void GameWatcher::processLogLine(QString line)
{
    if(line.startsWith("[Bob]"))
    {
        if(line.startsWith("[Bob] ---Register"))
        {
            if(line.startsWith("[Bob] ---RegisterScreenForge---"))
            {
                arenaMode = true;
                emit enterArena();
            }
            else if(line.startsWith("[Bob] ---RegisterProfileNotices---") ||
                    line.startsWith("[Bob] ---RegisterFriendChallenge---"))
            {
                if(gameState == inRewards)
                {
                    gameState = noGame;
                    emit pDebug("Rewards complete.");
                    emit pLog(tr("Log: New rewards."));
                    emit arenaRewardsComplete();
                }
            }
            else if(line.startsWith("[Bob] ---RegisterScreenEndOfGame---"))
            {
                gameState = noGame;
                emit pDebug("GameState = noGame");

                if(arenaMode)
                {
                    emit endGame();
                }
            }
            else
            {
                arenaMode = false;
                emit leaveArena();
#ifdef QT_DEBUG
                arenaMode = true;//Testing
#endif
            }

//            if(gameState == readingDeck)
//            {
//                endReadingDeck();
//            }
        }
    }
    else if(line.startsWith("[Rachelle]"))
    {
        if(line.contains(QRegularExpression("reward \\d=\\[")))
        {
            gameState = inRewards;
            emit pDebug("GameState = inRewards");
            emit pDebug("New reward.");

            if(line.contains("BoosterPackRewardData"))
            {
                emit newArenaReward(0,0,true,false,false);
            }
            else if(line.contains("CardRewardData"))
            {
                if(line.contains(QRegularExpression("Premium=(STANDARD|GOLDEN)"), match))
                {
                    QString cardType = match->captured(1);
                    if(cardType.compare("STANDARD") == 0)
                    {
                        emit newArenaReward(0,0,false,false,true);
                    }
                    else
                    {
                        emit newArenaReward(0,0,false,true,false);
                    }

                }
            }
            else if(line.contains(QRegularExpression("GoldRewardData: Amount=(\\d+)"), match))
            {
                QString gold = match->captured(1);
                emit newArenaReward(gold.toInt(),0,false,false,false);
            }
            else if(line.contains(QRegularExpression("ArcaneDustRewardData: Amount=(\\d+)"), match))
            {
                QString dust = match->captured(1);
                emit newArenaReward(0, dust.toInt(),false,false,false);
            }
        }
        //No ocurre en log
//        else if(line.contains(QRegularExpression("DraftManager\\.OnChosen.+ hero=HERO_(\\d+)"), match))
//        {
//            emit pDebug("Nueva arena.";
//            emit pLog(tr("Log: New arena."));
//            QString hero = match->captured(1);
//            emit newArena(hero);
//            deckRead = false;
//        }
    }
    else if(line.startsWith("[Power]"))
    {
        processPower(line);
    }
    else if(line.startsWith("[Zone]"))
    {
        processZone(line);
    }
    //No ocurre en log
//    else if(line.startsWith("[Ben]"))
//    {
//        if(line.startsWith("[Ben] SetDraftMode - DRAFTING"))
//        {
//            emit pDebug("Resume draft.";
//            emit resumeDraft();
//        }
//        else if(line.startsWith("[Ben] SetDraftMode - ACTIVE_DRAFT_DECK"))
//        {
//            emit activeDraftDeck();

    //Si queremos reactivar readingDeck hay que modificar newDeckCard
//            if(!deckRead)
//            {
//                gameState = readingDeck;
//                emit pDebug("GameState = readingDeck";
//                emit pDebug("Inicio leer deck.";
//            }
//        }
//    }
//    else if(line.startsWith("[Asset]"))
//    {
//        if((gameState == readingDeck) &&
//            line.contains(QRegularExpression(
//                "CachedAsset\\.UnloadAssetObject.+ - unloading name=(\\w+) family=CardPrefab persistent=False"), match))
//        {
//            QString code = match->captured(1);
//            //Hero portraits
//            if(code.contains("HERO"))
//            {
//                emit pDebug("Desechamos HERO";
//                endReadingDeck();
//                return;
//            }
//            //Hero powers
//            if( code=="CS2_102" || code=="CS2_083b" || code=="CS2_034" ||
//                code=="CS1h_001" || code=="CS2_056" || code=="CS2_101" ||
//                code=="CS2_017" || code=="DS1h_292" || code=="CS2_049")
//            {
//                emit pDebug("Desechamos HERO POWER";
//                endReadingDeck();
//                return;
//            }
//            emit newDeckCard(code);
//        }
//    }
}


//void GameWatcher::endReadingDeck()
//{
//    deckRead = true;
//    gameState = noGame;
//    emit pDebug("GameState = noGame";
//    emit pDebug("Final leer deck.";
//    emit sendLog(tr("Log: Active deck read."));
//}


//void GameWatcher::setDeckRead()
//{
//    deckRead = true;
//    if(gameState == readingDeck)
//    {
//        endReadingDeck();
//    }
//}


void GameWatcher::processPower(QString &line)
{
    switch(gameState)
    {
//        case readingDeck:
        case noGame:
            if(line.contains("CREATE_GAME"))
            {
//                if(gameState == readingDeck)
//                {
//                    endReadingDeck();
//                }

                if(arenaMode)
                {
                    gameState = heroType1State;
                    mulliganEnemyDone = false;
                    turn = turnReal = 0;

                    hero1.clear();
                    hero2.clear();
                    name1.clear();
                    name2.clear();
                    firstPlayer.clear();
                    winnerPlayer.clear();
                    playerID = 0;
                    secretHero = unknown;
                    enemyMinions = 0;
                    enemyMinionsAliveForAvenge = -1;
                    emit startGame();
                }
            }
            break;
        case heroType1State:
            if(line.contains(QRegularExpression("Creating ID=4 CardID=HERO_(\\d+)"), match))
            {
                hero1 = match->captured(1);
                gameState = heroType2State;
            }
            break;
        case heroType2State:
            if(line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero2 = match->captured(1);
                gameState = playerName1State;
            }
            break;
        case playerName1State:
            if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYER_ID value=2"), match))
            {
                name2 = match->captured(1);
                if(name2 == playerTag)
                {
                    playerID = 2;
                    secretHero = getSecretHero(hero2, hero1);
                }
                gameState = playerName2State;
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
            }
            break;
        case playerName2State:
            if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYER_ID value=1"), match))
            {
                name1 = match->captured(1);
                if(name1 == playerTag)
                {
                    playerID = 1;
                    secretHero = getSecretHero(hero1, hero2);
                }
                gameState = inGameState;
                emit pDebug("GameState = inGameState");
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
            }
            break;
        case inGameState:
            processPowerInGame(line);
            break;
        case inRewards:
            break;
    }
}


void GameWatcher::processPowerInGame(QString &line)
{
    //Win state
    if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
    {
        winnerPlayer = match->captured(1);
        createGameResult();
    }
    //Roba carta
    else if(line.contains(QRegularExpression(
            "m_chosenEntities\\[\\d+\\]=\\[name=.* id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
            ), match))
    {
        emit pDebug("Player: Starting card drawn: " + match->captured(1));
        emit playerCardDraw(match->captured(1));
    }
    //Turn
    else if(line.contains(QRegularExpression("Entity=GameEntity tag=TURN value=(\\d+)"
            ), match))
    {
        turn = match->captured(1).toInt();
    }
    //Enemy mulligan
    else if(line.contains(QRegularExpression("Entity=(.+) tag=MULLIGAN_STATE value=DONE"
            ), match))
    {
        if(match->captured(1) != playerTag)
        {
            emit pDebug("Enemy mulligan end.");
            mulliganEnemyDone = true;
            if(firstPlayer == playerTag)
            {
                //Convertir ultima carta en moneda enemiga
                emit lastHandCardIsCoin();
            }
        }
    }

    //SECRETOS
    else if(synchronized)
    {
        //Jugador juega carta con objetivo
        if(line.contains(QRegularExpression(
            "ACTION_START Entity=\\[name=(.*) id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] SubType=PLAY Index=0 "
            "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
            ), match))
        {
            if(match->captured(3).toInt() == playerID)
            {
                QString type = (*cardsJson)[match->captured(2)].value("type").toString();

                if(type == QString("Spell"))
                {
                    emit pDebug("Player: Spell obj played: " +
                                match->captured(1) + " target " + match->captured(4));
                    if(match->captured(5) == MAD_SCIENTIST)
                    {
                        emit pDebug("Skip secret testing.");
                    }
                    else if(isPlayerTurn)    emit playerSpellObjPlayed();
                }
                else
                {
                    emit pDebug("Player: Minion/weapon obj played: " +
                                match->captured(1) + " target " + match->captured(4));
                }
            }
        }

        //Jugador ataca (esbirro/heroe VS esbirro/heroe)
        else if(line.contains(QRegularExpression(
            "ACTION_START Entity=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] SubType=ATTACK Index=-1 "
            "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\]"
            ), match))
        {
            if(match->captured(3).toInt() == playerID)
            {
                if(match->captured(2).contains("HERO"))
                {
                    if(match->captured(5).contains("HERO"))
                    {
                        emit pDebug("Player: Attack: " +
                                    match->captured(1) + " (heroe)vs(heroe) " + match->captured(4));
                        if(isPlayerTurn)    emit playerAttack(true, true);
                    }
                    else
                    {
                        emit pDebug("Player: Attack: " +
                                    match->captured(1) + " (heroe)vs(minion) " + match->captured(4));
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            emit pDebug("Saltamos comprobacion de secretos";
                        }
                        else */if(isPlayerTurn)    emit playerAttack(true, false);
                    }
                }
                else
                {
                    if(match->captured(5).contains("HERO"))
                    {
                        emit pDebug("Player: Attack: " +
                                    match->captured(1) + " (minion)vs(heroe) " + match->captured(4));
                        if(isPlayerTurn)    emit playerAttack(false, true);
                    }
                    else
                    {
                        emit pDebug("Player: Attack: " +
                                    match->captured(1) + " (minion)vs(minion) " + match->captured(4));
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            emit pDebug("Saltamos comprobacion de secretos";
                        }
                        else */if(isPlayerTurn)    emit playerAttack(false, false);
                    }
                }
            }
        }
    }
}


void GameWatcher::processZone(QString &line)
{
    if(gameState == inGameState)
    {
        //ENEMIGO JUEGA
        //Enemigo juega secreto o carta devuelta al mazo en Mulligan
        if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=\\d+\\] zone from (.*) -> OPPOSING (SECRET|DECK)"
            ), match))
        {
            if(match->captured(2) == "OPPOSING HAND")
            {
                emit enemyCardPlayed(match->captured(1).toInt());
                if(match->captured(3) == "DECK")
                {
                    emit pDebug("Enemy: Starting card returned. ID: " + match->captured(1));
                }
            }
            if(match->captured(3) == "SECRET")
            {
                emit pDebug("Enemy: Secret played. ID: " + match->captured(1));
                emit enemySecretPlayed(match->captured(1).toInt(), secretHero);
            }
        }
        //Enemigo roba secreto (kezan mystic)
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=SECRET zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from FRIENDLY SECRET -> OPPOSING SECRET"
            ), match))
        {
            emit pDebug("Enemy: Secret stolen: " + match->captured(1) + " ID: " + match->captured(2));
            emit enemySecretStealed(match->captured(2).toInt(), match->captured(3));
        }
        //Enemigo juega carta
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING HAND -> (.*)\n"
            ), match))
        {
            //Enemigo juega hechizo
            if(match->captured(4).isEmpty())
            {
                emit pDebug("Enemy: Spell played: " + match->captured(1) + " ID: " + match->captured(2));
            }
            //Enemigo juega esbirro
            else if(synchronized && (match->captured(4) == "OPPOSING PLAY"))
            {
                enemyMinions++;
                emit pDebug("Enemy: Minion played: " + match->captured(1) + " ID: " + match->captured(2) + " Minions: " + enemyMinions);
            }
            //Enemigo juega arma
            else if(match->captured(4) == "OPPOSING PLAY (Weapon)")
            {
                emit pDebug("Enemy: Weapon played: " + match->captured(1) + " ID: " + match->captured(2));
            }
            else if(match->captured(4) == "OPPOSING GRAVEYARD")
            {
                emit pDebug("Enemy: Card discarded: " + match->captured(1) + " ID: " + match->captured(2));
            }

            emit enemyCardPlayed(match->captured(2).toInt(), match->captured(3));
        }

        //ENEMIGO SECRETO DESVELADO
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING SECRET ->"// OPPOSING GRAVEYARD"
            ), match))
        {
            emit pDebug("Enemy: Secret revealed: " + match->captured(1));
            emit enemySecretRevealed(match->captured(2).toInt(), match->captured(3));
        }

        //JUGADOR ROBA
        //Jugador roba carta
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=(\\w+) zonePos=0 cardId=(\\w+) player=\\d+\\] zone from FRIENDLY DECK ->"
            ), match))
        {
            if(match->captured(2) != "DECK")
            {
                advanceTurn(true);
                emit pDebug("Player: Card drawn: " + match->captured(1));
                emit playerCardDraw(match->captured(3));
            }
        }

        //ENEMIGO ROBA
        //Enemigo roba carta de deck
        else if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=\\d+\\] zone from OPPOSING DECK -> OPPOSING HAND"
            ), match))
        {
            advanceTurn(false);
            emit pDebug("Enemy: Card drawn. ID: " + match->captured(1));
            emit enemyCardDraw(match->captured(1).toInt(), turnReal);
        }

        else if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=\\d+\\] zone from  -> OPPOSING HAND"
            ), match))
        {
            //Enemigo roba carta especial del vacio
            if(mulliganEnemyDone)
            {
                emit pDebug("Enemy: Special card drawn. ID: " + match->captured(1));
                emit enemyCardDraw(match->captured(1).toInt(), turnReal, true);
            }
            //Enemigo roba carta inicial
            else
            {
                emit pDebug("Enemy: Starting card drawn. ID: " + match->captured(1));
                emit enemyCardDraw(match->captured(1).toInt());
            }
        }
        //Enemigo roba carta conocida
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from .* -> OPPOSING HAND"
            ), match))
        {
            emit pDebug("Enemy: Card returned to hand: " + match->captured(1) + " ID: " + match->captured(2));
            emit enemyCardDraw(match->captured(2).toInt(), turnReal, false, match->captured(3));
        }


        //SECRETOS
        else if(synchronized)
        {
            //JUGADOR JUEGA
            //Jugador juega carta
            if(line.contains(QRegularExpression(
                "\\[name=(.*) id=\\d+ zone=\\w+ zonePos=\\d+ cardId=\\w+ player=\\d+\\] zone from FRIENDLY HAND -> (FRIENDLY PLAY|FRIENDLY PLAY .Weapon.)?\n"
                ), match))
            {
                //Jugador juega hechizo
                if(match->captured(2).isEmpty())
                {
                    emit pDebug("Player: Spell played: " + match->captured(1));
                    if(isPlayerTurn)    emit playerSpellPlayed();
                }
                //Jugador juega esbirro
                else if(match->captured(2) == "FRIENDLY PLAY")
                {
                    emit pDebug("Player: Minion played: " + match->captured(1));
                    if(isPlayerTurn)    emit playerMinionPlayed();
                }
                //Jugador juega arma
                else
                {
                    emit pDebug("Player: Weapon played: " + match->captured(1));
                }
            }

            //JUGADOR ESBIRRO MUERE
    //        else if(line.contains(QRegularExpression(
    //            "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=\\w+ player=\\d+\\] zone from FRIENDLY PLAY -> FRIENDLY GRAVEYARD"
    //            ), match))
    //        {
    //            emit pDebug("Player: Esbirro muerto:  " + match->captured(1);
    //            //emit no necesario
    //        }

            //ENEMIGO ESBIRRO MUERE
            else if(line.contains(QRegularExpression(
                "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING PLAY -> OPPOSING GRAVEYARD"
                ), match))
            {
                enemyMinions--;
                emit pDebug("Enemy: Minion dead: " + match->captured(1) + " Minions: " + enemyMinions);

                if(isPlayerTurn)
                {
                    emit enemyMinionDead();
                    if(enemyMinionsAliveForAvenge == -1)
                    {
                        if(match->captured(2) == MAD_SCIENTIST)
                        {
                            emit pDebug("Skip secret testing.");
                        }
                        else
                        {
                            enemyMinionsAliveForAvenge = enemyMinions;
                            QTimer::singleShot(1000, this, SLOT(checkAvenge()));
                        }
                    }
                    else    enemyMinionsAliveForAvenge--;
                }
            }

            //ENEMIGO ESBIRRO CAMBIA POSICION
            else if(line.contains(QRegularExpression(
                "\\[name=(.*) id=\\d+ zone=PLAY zonePos=(\\d+) cardId=\\w+ player=(\\d+)\\] pos from"
                ), match))
            {
                if(match->captured(3).toInt() != playerID)
                {
                    if(match->captured(2).toInt() > enemyMinions) enemyMinions = match->captured(2).toInt();
                    emit pDebug("Enemy: New minion pos: " +
                                match->captured(1) + " >> " + match->captured(2) + " Minions: " + enemyMinions);
                }
            }
        }
    }
}


void GameWatcher::checkAvenge()
{
    if(enemyMinionsAliveForAvenge > 0)
    {
        emit avengeTested();
        emit pDebug("Avenge tested: Survivors: " + enemyMinionsAliveForAvenge);
    }
    else    emit pDebug("Avenge not tested: Survivors: " + enemyMinionsAliveForAvenge);
    enemyMinionsAliveForAvenge = -1;
}


QString GameWatcher::askPlayerTag(QString &playerName1, QString &playerName2)
{
    QMessageBox msgBox;
    msgBox.setText(tr("Who are you?"));
    msgBox.setWindowTitle(tr("Player Tag"));
    msgBox.setIcon(QMessageBox::Question);
    QPushButton *button1 = msgBox.addButton(playerName1, QMessageBox::ActionRole);
    QPushButton *button2 = msgBox.addButton(playerName2, QMessageBox::ActionRole);
    QPushButton *button3 = msgBox.addButton("None", QMessageBox::ActionRole);

    msgBox.exec();

    if (msgBox.clickedButton() == button1)
    {
        return playerName1;
    }
    else if (msgBox.clickedButton() == button2)
    {
        return playerName2;
    }
    else
    {
        (void)button3;
        return playerTag;
    }
}


void GameWatcher::createGameResult()
{
    if(playerTag != name1 && playerTag != name2)
    {
        playerTag = askPlayerTag(name1, name2);
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("playerTag", playerTag);
    }

    GameResult gameResult;

    //Volvemos a calcular playerHero y enemyHero en caso de cambio de playerTag
    if(name1 == playerTag)
    {
        gameResult.playerHero = hero1;
        gameResult.enemyHero = hero2;
    }
    else if(name2 == playerTag)
    {
        gameResult.playerHero = hero2;
        gameResult.enemyHero = hero1;
    }
    else
    {
        emit pDebug("PlayerTag didn't play this game.", Error);
        emit pLog(tr("Log: WARNING:Registered game played without you.") + "" + playerTag);
        return;
    }

    gameResult.isFirst = (firstPlayer == playerTag);
    gameResult.isWinner = (winnerPlayer == playerTag);

    emit pDebug("\nNew game.");
    emit pLog(tr("Log: New game."));

    emit newGameResult(gameResult);
}


void GameWatcher::advanceTurn(bool playerDraw)
{
    if(turnReal == turn)    return;

    bool playerTurn;
    if((firstPlayer==playerTag && turn%2==1) || (firstPlayer!=playerTag && turn%2==0))  playerTurn=true;
    else    playerTurn=false;

    //Al turno 1 dejamos que pase cualquiera asi dejamos el turno 0 para indicar cartas de mulligan
    //Solo avanza de turno al robar carta el jugador que le corresponde
    if(turn == 1 || playerDraw == playerTurn)
    {
        turnReal = turn;
        emit pDebug("\nTurn: " + QString::number(turn) + " " + (playerTurn?"Player":"Enemy"));

        if((firstPlayer==playerTag && turnReal%2==1) || (firstPlayer!=playerTag && turnReal%2==0))  isPlayerTurn=true;
        else    isPlayerTurn=false;

    }
}


SecretHero GameWatcher::getSecretHero(QString playerHero, QString enemyHero)
{
    SecretHero enemySecretHero = unknown;
    SecretHero playerSecretHero = unknown;

    if(enemyHero == QString("04"))  enemySecretHero = paladin;
    else if(enemyHero == QString("05"))  enemySecretHero = hunter;
    else if(enemyHero == QString("08"))  enemySecretHero = mage;

    if(playerHero == QString("04"))  playerSecretHero = paladin;
    else if(playerHero == QString("05"))  playerSecretHero = hunter;
    else if(playerHero == QString("08"))  playerSecretHero = mage;

    if(enemySecretHero != unknown)  return enemySecretHero;
    else if(playerSecretHero != unknown)  return playerSecretHero;
    else    return unknown;
}


void GameWatcher::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    GameWatcher::cardsJson = cardsJson;
}


void GameWatcher::setSynchronized()
{
    this->synchronized = true;
}










