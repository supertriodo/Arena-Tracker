#include "gamewatcher.h"
#include <QtWidgets>


QMap<QString, QJsonObject> * GameWatcher::cardsJson;


GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    gameState = noGame;
    arenaMode = false;
    deckRead = false;
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
    emit pDebug("GameState = noGame", 0);
    arenaMode = false;
}


bool GameWatcher::findClasp(QString &line)
{
    int index = line.indexOf('[');

    if(index == -1)
    {
        return false;
    }
    else
    {
        line.remove(0, index);
        qDebug()<<line;
        return true;
    }
}


void GameWatcher::processLogLine(QString line, qint64 numLine)
{
    if(line.startsWith("[Bob]"))
    {
        if(line.startsWith("[Bob] ---Register"))
        {
            if(gameState == readingDeck)
            {
                endReadingDeck();
            }

            if(line.startsWith("[Bob] ---RegisterScreenForge---"))
            {
                arenaMode = true;
                emit pDebug("Entering arena.", numLine);
                emit enterArena();

                if(synchronized && !deckRead && gameState == noGame)
                {
                    startReadingDeck();
                }
            }
            else if(line.startsWith("[Bob] ---RegisterProfileNotices---") ||
                    line.startsWith("[Bob] ---RegisterFriendChallenge---"))
            {
                if(gameState == inRewards)
                {
                    gameState = noGame;
                    emit pDebug("Rewards complete.", numLine);
                    emit pLog(tr("Log: New rewards."));
                    emit arenaRewardsComplete();
                }
            }
            else if(line.startsWith("[Bob] ---RegisterScreenEndOfGame---"))
            {
                gameState = noGame;
                emit pDebug("GameState = noGame", numLine);

                if(arenaMode)
                {
                    emit endGame();
                }
            }
            else
            {
                arenaMode = false;
                emit pDebug("Leaving arena.", numLine);
                emit leaveArena();
            }
        }
    }
    else if(line.startsWith("[Rachelle]"))
    {
        if(line.contains(QRegularExpression("reward \\d=\\[")))
        {
            gameState = inRewards;
            emit pDebug("GameState = inRewards", numLine);
            emit pDebug("New reward.", numLine);

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
        processPower(line, numLine);
    }
    else if(line.startsWith("[Zone]"))
    {
        processZone(line, numLine);
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
    else if(line.startsWith("[Asset]"))
    {
        if((gameState == readingDeck) &&
            line.contains(QRegularExpression(
                "CachedAsset\\.UnloadAssetObject.+ - unloading name=(\\w+) family=CardPrefab persistent=False"), match))
        {
            QString code = match->captured(1);
            //Hero portraits
            if(code.contains("HERO"))
            {
                emit pDebug("Discard HERO card", numLine);
                endReadingDeck();
                return;
            }
            //Hero powers
            if( code=="CS2_102" || code=="CS2_083b" || code=="CS2_034" ||
                code=="CS1h_001" || code=="CS2_056" || code=="CS2_101" ||
                code=="CS2_017" || code=="DS1h_292" || code=="CS2_049")
            {
                emit pDebug("Discard HERO POWER", numLine);
                endReadingDeck();
                return;
            }
            emit pDebug("Read code: " + code, numLine);
            emit newDeckCard(code);
        }
    }
}


void GameWatcher::startReadingDeck()
{
    if(gameState != noGame || deckRead) return;
    gameState = readingDeck;
    emit pDebug("GameState = readingDeck", 0);
    emit pDebug("Start reading deck.", 0);
}


void GameWatcher::endReadingDeck()
{
    if(gameState != readingDeck)    return;
    deckRead = true;
    gameState = noGame;
    emit pDebug("GameState = noGame", 0);
    emit pDebug("End reading deck.", 0);
    emit pLog(tr("Log: Active deck read."));
}


void GameWatcher::setDeckRead(bool value)
{
    deckRead = value;
    if(deckRead && gameState == readingDeck)
    {
        endReadingDeck();
    }
}


void GameWatcher::processPower(QString &line, qint64 numLine)
{
    switch(gameState)
    {
        case readingDeck:
        case noGame:
            if(line.contains("CREATE_GAME"))
            {
                if(gameState == readingDeck)
                {
                    endReadingDeck();
                }

                gameState = heroType1State;
                emit pDebug("GameState = heroType1State", numLine);
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
            break;
        case heroType1State:
            if(line.contains(QRegularExpression("Creating ID=4 CardID=HERO_(\\d+)"), match))
            {
                hero1 = match->captured(1);
                gameState = heroType2State;
                emit pDebug("GameState = heroType2State", numLine);
            }
            break;
        case heroType2State:
            if(line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero2 = match->captured(1);
                gameState = playerName1State;
                emit pDebug("GameState = playerName1State", numLine);
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
                emit pDebug("GameState = playerName2State", numLine);
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
                emit pDebug("GameState = inGameState\n", numLine);
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
            }
            break;
        case inGameState:
            processPowerInGame(line, numLine);
            break;
        case inRewards:
            break;
    }
}


void GameWatcher::processPowerInGame(QString &line, qint64 numLine)
{
    //Win state
    if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
    {
        winnerPlayer = match->captured(1);
        createGameResult();

        gameState = noGame;
        emit pDebug("GameState = noGame", numLine);
    }
    //Turn
    else if(line.contains(QRegularExpression("Entity=GameEntity tag=TURN value=(\\d+)"
            ), match))
    {
        turn = match->captured(1).toInt();
    }
    else if(synchronized)
    {
        //Jugador roba carta inicial
        if(line.contains(QRegularExpression(
                "m_chosenEntities\\[\\d+\\]=\\[name=.* id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
                ), match))
        {
            emit pDebug("Player: Starting card drawn: " + match->captured(1), numLine);
            emit playerCardDraw(match->captured(1));
        }
        //Enemigo mulligan
        else if(line.contains(QRegularExpression("Entity=(.+) tag=MULLIGAN_STATE value=DONE"
                ), match))
        {
            if(!mulliganEnemyDone && match->captured(1) != playerTag)
            {
                emit pDebug("Enemy mulligan end.", numLine);
                mulliganEnemyDone = true;
                if(firstPlayer == playerTag)
                {
                    //Convertir ultima carta en moneda enemiga
                    emit lastHandCardIsCoin();
                }
            }
        }

        //SECRETOS
        //Jugador accion con objetivo
        else if(line.contains(QRegularExpression(
            "GameState\\.DebugPrintPower\\(\\) - ACTION_START "
            "Entity=\\[name=(.*) id=\\d+ zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] "
            "BlockType=(\\w+) Index=(-?\\d+) "
            "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
            ), match))
        {
            QString name1 = match->captured(1);
            QString zone = match->captured(2);
            QString cardId1 = match->captured(3);
            QString player1 = match->captured(4);
            QString subType = match->captured(5);
            QString index = match->captured(6);
            QString name2 = match->captured(7);
            QString cardId2 = match->captured(8);

            bool isPlayer = (player1.toInt() == playerID);


            //Jugador juega carta con objetivo
            if(zone == "HAND" && subType == "PLAY" && index == "0")
            {
                QString type = (*cardsJson)[cardId1].value("type").toString();

                if(type == QString("Spell"))
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Spell obj played: " +
                                name1 + " on target " + name2, numLine);
                    if(cardId2 == MAD_SCIENTIST)
                    {
                        emit pDebug("Skip secret testing.", 0);
                    }
                    else if(isPlayer && isPlayerTurn)    emit playerSpellObjPlayed();
                }
                else
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Minion/weapon obj played: " +
                                name1 + " target " + name2, numLine);
                }
            }

            //Jugador ataca (esbirro/heroe VS esbirro/heroe)
            else if(zone == "PLAY" && subType == "ATTACK" && index == "-1")
            {
                if(cardId1.contains("HERO"))
                {
                    if(cardId2.contains("HERO"))
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                    name1 + " (heroe)vs(heroe) " + name2, numLine);
                        if(isPlayer && isPlayerTurn)    emit playerAttack(true, true);
                    }
                    else
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                    name1 + " (heroe)vs(minion) " + name2, numLine);
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            emit pDebug("Saltamos comprobacion de secretos";
                        }
                        else */if(isPlayer && isPlayerTurn)    emit playerAttack(true, false);
                    }
                }
                else
                {
                    if(cardId2.contains("HERO"))
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                    name1 + " (minion)vs(heroe) " + name2, numLine);
                        if(isPlayer && isPlayerTurn)    emit playerAttack(false, true);
                    }
                    else
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                    name1 + " (minion)vs(minion) " + name2, numLine);
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            emit pDebug("Saltamos comprobacion de secretos";
                        }
                        else */if(isPlayer && isPlayerTurn)    emit playerAttack(false, false);
                    }
                }
            }
        }
    }
}


void GameWatcher::processZone(QString &line, qint64 numLine)
{
    if(gameState == inGameState && synchronized)
    {
        //Carta desconocida
        if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=\\d+\\] zone from (.*) -> OPPOSING (HAND|SECRET|DECK)"
            ), match))
        {
            QString id = match->captured(1);
            QString zoneFrom = match->captured(2);
            QString zoneToOpposing = match->captured(3);


            //Enemigo juega carta
            if(zoneFrom == "OPPOSING HAND")
            {
                emit enemyCardPlayed(id.toInt());

                //Carta devuelta al mazo en Mulligan
                if(zoneToOpposing == "DECK")
                {
                    emit pDebug("Enemy: Starting card returned. ID: " + id, numLine);
                }
            }

            //Enemigo juega secreto
            if(zoneToOpposing == "SECRET")
            {
                emit pDebug("Enemy: Secret played. ID: " + id, numLine);
                emit enemySecretPlayed(id.toInt(), secretHero);
            }

            //Enemigo roba carta de deck
            else if(zoneToOpposing == "HAND")
            {
                if(zoneFrom == "OPPOSING DECK")
                {
                    advanceTurn(false);
                    emit pDebug("Enemy: Card drawn. ID: " + id, numLine);
                    emit enemyCardDraw(id.toInt(), turnReal);
                }

                else if(zoneFrom.isEmpty())
                {
                    //Enemigo roba carta especial del vacio
                    if(mulliganEnemyDone)
                    {
                        emit pDebug("Enemy: Special card drawn. ID: " + id, numLine);
                        emit enemyCardDraw(id.toInt(), turnReal, true);
                    }
                    //Enemigo roba carta inicial
                    else
                    {
                        emit pDebug("Enemy: Starting card drawn. ID: " + id, numLine);
                        emit enemyCardDraw(id.toInt());
                    }
                }
            }
        }


        //Carta conocida
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from (.*) -> (.*)\n"
            ), match))
        {
            QString name = match->captured(1);
            QString id = match->captured(2);
            QString cardId = match->captured(3);
            QString zoneFrom = match->captured(4);
            QString zoneTo = match->captured(5);


            //Enemigo roba carta conocida
            if(zoneTo == "OPPOSING HAND")
            {
                if(zoneFrom == "OPPOSING DECK")     advanceTurn(false);
                emit pDebug("Enemy: Known card to hand: " + name + " ID: " + id, numLine);
                emit enemyCardDraw(id.toInt(), turnReal, false, cardId);
            }

            //Enemigo roba secreto (kezan mystic)
            if(zoneFrom == "FRIENDLY SECRET" && zoneTo == "OPPOSING SECRET")
            {
                emit pDebug("Enemy: Secret stolen: " + name + " ID: " + id, numLine);
                emit enemySecretStealed(id.toInt(), cardId);
            }

            //Enemigo juega carta
            else if(zoneFrom == "OPPOSING HAND")
            {
                //Enemigo juega hechizo
                if(zoneTo.isEmpty())
                {
                    emit pDebug("Enemy: Spell played: " + name + " ID: " + id, numLine);
                }
                //Enemigo juega esbirro
                else if(zoneTo == "OPPOSING PLAY")
                {
                    enemyMinions++;
                    emit pDebug("Enemy: Minion played: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
                }
                //Enemigo juega arma
                else if(zoneTo == "OPPOSING PLAY (Weapon)")
                {
                    emit pDebug("Enemy: Weapon played: " + name + " ID: " + id, numLine);
                }
                else if(zoneTo == "OPPOSING GRAVEYARD")
                {
                    emit pDebug("Enemy: Card discarded: " + name + " ID: " + id, numLine);
                }

                emit enemyCardPlayed(id.toInt(), cardId);
            }

            //Enemigo secreto desvelado
            else if(zoneFrom == "OPPOSING SECRET")
            {
                emit pDebug("Enemy: Secret revealed: " + name, numLine);
                emit enemySecretRevealed(id.toInt(), cardId);
            }

            //Jugador roba carta
            else if(zoneFrom == "FRIENDLY DECK" && !zoneTo.isEmpty())
            {
                advanceTurn(true);
                emit pDebug("Player: Card drawn: " + name, numLine);
                emit playerCardDraw(cardId);
            }


            //SECRETOS
            //Jugador juega carta
            else if(zoneFrom == "FRIENDLY HAND")
            {
                //Jugador juega hechizo
                if(zoneTo.isEmpty())
                {
                    emit pDebug("Player: Spell played: " + name, numLine);
                    if(isPlayerTurn)    emit playerSpellPlayed();
                }
                //Jugador juega esbirro
                else if(zoneTo == "FRIENDLY PLAY")
                {
                    emit pDebug("Player: Minion played: " + name, numLine);
                    if(isPlayerTurn)    emit playerMinionPlayed();
                }
                //Jugador juega arma
                else if(zoneTo == "FRIENDLY PLAY (Weapon)")
                {
                    emit pDebug("Player: Weapon played: " + name, numLine);
                }
            }

            //Enemigo esbirro muere
            else if(zoneFrom == "OPPOSING PLAY" && zoneTo == "OPPOSING GRAVEYARD")
            {
                enemyMinions--;
                emit pDebug("Enemy: Minion dead: " + name + " Minions: " + QString::number(enemyMinions), numLine);

                if(isPlayerTurn)
                {
                    emit enemyMinionDead();
                    if(enemyMinionsAliveForAvenge == -1)
                    {
                        if(cardId == MAD_SCIENTIST)
                        {
                            emit pDebug("Skip secret testing.", 0);
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

            //Jugador esbirro muere
            else if(zoneFrom == "FRIENDLY PLAY" && zoneTo == "FRIENDLY GRAVEYARD")
            {
                emit pDebug("Player: Minion dead: " + name, numLine);
            }
        }


        //Enemigo esbirro cambia pos
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=PLAY zonePos=(\\d+) cardId=\\w+ player=(\\d+)\\] pos from"
            ), match))
        {
            if(match->captured(3).toInt() != playerID)
            {
                if(match->captured(2).toInt() > enemyMinions) enemyMinions = match->captured(2).toInt();
                emit pDebug("Enemy: New minion pos: " +
                            match->captured(1) + " >> " + match->captured(2) + " Minions: " + QString::number(enemyMinions), numLine);
            }
        }
    }
}


void GameWatcher::checkAvenge()
{
    if(enemyMinionsAliveForAvenge > 0)
    {
        emit avengeTested();
        emit pDebug("Avenge tested: Survivors: " + QString::number(enemyMinionsAliveForAvenge), 0);
    }
    else    emit pDebug("Avenge not tested: Survivors: " + QString::number(enemyMinionsAliveForAvenge), 0);
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

    emit pDebug("\nNew game.", 0);
    emit pLog(tr("Log: New game."));

    emit newGameResult(gameResult, this->arenaMode);
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
        emit pDebug("\nTurn: " + QString::number(turn) + " " + (playerTurn?"Player":"Enemy"), 0);

        isPlayerTurn = playerTurn;

        if(isPlayerTurn)    emit playerTurnStart();

        if(synchronized && !isPlayerTurn && enemyMinions > 0)
        {
            emit pDebug("CSpirit tested. Minions: " + QString::number(enemyMinions), 0);
            emit cSpiritTested();
        }
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










