#include "gamewatcher.h"
#include <QtWidgets>


GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    powerState = noGame;
    arenaState = noDeckRead;
    loadingScreenState = menu;
    mulliganEnemyDone = false;
    spectating = false;
    turn = turnReal = 0;
    logSeekCreate = -1;

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
    powerState = noGame;
    arenaState = noDeckRead;
    loadingScreenState = menu;
    spectating = false;
    logSeekCreate = -1;
    emit pDebug("Reset (powerState = noGame).", 0);
    emit pDebug("Reset (LoadingScreen = menu).", 0);
}


void GameWatcher::processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    if(logComponent == logLoadingScreen)
    {
        processLoadingScreen(line, numLine);
    }
    else if(logComponent == logArena)
    {
        processArena(line, numLine);
    }
    else if(logComponent == logPower)
    {
        processPower(line, numLine, logSeek);
    }
    else if(logComponent == logZone)
    {
        processZone(line, numLine);
    }
}


void GameWatcher::startReadingDeck()
{
    if(arenaState == deckRead) return;
    emit needResetDeck();    //resetDeck
    arenaState = readingDeck;
    emit pDebug("Start reading deck (arenaState = readingDeck).", 0);
}


void GameWatcher::endReadingDeck()
{
    if(arenaState != readingDeck)    return;
    arenaState = deckRead;
    emit pDebug("End reading deck (arenaState = deckRead).", 0);
    emit pLog(tr("Log: Active deck read."));
}


void GameWatcher::setDeckRead(bool value)
{
    if(value)
    {
        arenaState = deckRead;
        emit pDebug("SetDeckRead (arenaState = deckRead).", 0);
    }
    else
    {
        arenaState = noDeckRead;
        emit pDebug("SetDeckRead (arenaState = noDeckRead).", 0);
    }
}


void GameWatcher::processLoadingScreen(QString &line, qint64 numLine)
{
    //[LoadingScreen] LoadingScreen.OnSceneLoaded() - prevMode=HUB currMode=DRAFT
    if(line.contains(QRegularExpression("LoadingScreen\\.OnSceneLoaded\\(\\) *- *prevMode=(\\w+) *currMode=(\\w+)"), match))
    {
        QString prevMode = match->captured(1);
        QString currMode = match->captured(2);
        emit pDebug("\nLoadingScreen: " + prevMode + " -> " + currMode, numLine);

        if(currMode == "DRAFT")
        {
            loadingScreenState = arena;
            emit pDebug("Entering ARENA (loadingScreenState = arena).", numLine);

            if(prevMode == "HUB")
            {
                emit enterArena();//enterArena deckHandler
            }
        }
        else if(currMode == "HUB")
        {
            loadingScreenState = menu;
            emit pDebug("Entering MENU (loadingScreenState = menu).", numLine);

            if(prevMode == "DRAFT")
            {
                setDeckRead(false);
                emit leaveArena();//leaveArena deckHandler
            }
        }
        else if(currMode == "TOURNAMENT")
        {
            loadingScreenState = constructed;
            emit pDebug("Entering CONSTRUCTED (loadingScreenState = constructed).", numLine);
        }
        else if(currMode == "ADVENTURE")
        {
            loadingScreenState = adventure;
            emit pDebug("Entering ADVENTURE (loadingScreenState = adventure).", numLine);
        }
        else if(currMode == "TAVERN_BRAWL")
        {
            loadingScreenState = tavernBrawl;
            emit pDebug("Entering TAVERN (loadingScreenState = tavernBrawl).", numLine);
        }
    }
}


void GameWatcher::processArena(QString &line, qint64 numLine)
{
    //NEW ARENA
    //[Arena] DraftManager.OnChosen(): hero=HERO_02 premium=STANDARD
    if(line.contains(QRegularExpression("DraftManager\\.OnChosen\\(\\): hero=HERO_(\\d+)"), match))
    {
        QString hero = match->captured(1);
        emit pDebug("New arena. Heroe: " + hero, numLine);
        emit pLog(tr("Log: New arena."));
        emit newArena(hero); //(sync)Begin draft //(sync)resetDeckDontRead (arenaState = deckRead)
    }
    //END READING DECK
    //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
    else if(synchronized && line.contains("SetDraftMode - ACTIVE_DRAFT_DECK"))
    {
        emit pDebug("Found ACTIVE_DRAFT_DECK.", numLine);
        emit activeDraftDeck(); //End draft
        endReadingDeck();
    }
    //DRAFTING PICK CARD
    //[Arena] Client chooses: Profesora violeta (NEW1_026)
    else if(synchronized && line.contains(QRegularExpression("Client chooses: .* \\((\\w+)\\)"), match))
    {
        QString code = match->captured(1);
        if(!code.contains("HERO"))
        {
            emit pDebug("Pick card: " + code, numLine);
            emit pickCard(code);
        }
    }
    //START READING DECK
    //[Arena] DraftManager.OnChoicesAndContents - Draft Deck ID: 472720132, Hero Card = HERO_02
    else if(synchronized && line.contains(QRegularExpression(
                "DraftManager\\.OnChoicesAndContents - Draft Deck ID: \\d+, Hero Card = HERO_\\d+"), match))
    {
        emit pDebug("Found DraftManager.OnChoicesAndContents", numLine);
        startReadingDeck();
    }
    //READ DECK CARD
    //[Arena] DraftManager.OnChoicesAndContents - Draft deck contains card FP1_012
    else if(synchronized && (arenaState == readingDeck) &&
        line.contains(QRegularExpression(
            "DraftManager\\.OnChoicesAndContents - Draft deck contains card (\\w+)"), match))
    {
        QString code = match->captured(1);
        emit pDebug("Reading deck: " + code, numLine);
        emit newDeckCard(code);
    }
    //IN REWARDS
    //[Arena] SetDraftMode - IN_REWARDS
    else if(synchronized && line.contains("SetDraftMode - IN_REWARDS"))
    {
        emit pDebug("Found IN_REWARDS.", numLine);
        emit inRewards();   //Show rewards input
    }
}


void GameWatcher::processPower(QString &line, qint64 numLine, qint64 logSeek)
{
    //================== End Spectator Game ==================
    if(line.contains("End Spectator Game"))
    {
        emit pDebug("End Spectator Game.", numLine);
        spectating = false;

        if(powerState != noGame)
        {
            emit pDebug("WON not found (PowerState = noGame)", 0);
            powerState = noGame;
            emit endGame();
        }
    }

    if(powerState == noGame)
    {
        //================== Begin Spectating 1st player ==================
        //================== Start Spectator Game ==================
        if(line.contains("Begin Spectating") || line.contains("Start Spectator Game"))
        {
            emit pDebug("Start Spectator Game.", numLine);
            spectating = true;
        }
        else if(line.contains("CREATE_GAME"))
        {
            emit pDebug("\nFound CREATE_GAME (powerState = heroType1State)", numLine);
            emit pDebug("PlayerTag: " + playerTag, 0);
            logSeekCreate = logSeek;
            powerState = heroType1State;

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
            playerMinions = -2;
            enemyMinions = -2;
            enemyMinionsAliveForAvenge = -1;

            emit specialCardTrigger("", "");    //Evita Cartas createdBy en el mulligan de practica
            emit startGame();
        }
    }
    else
    {
        //Win state
        if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
        {
            powerState = noGame;
            emit pDebug("Found WON (powerState = noGame).", numLine);

            winnerPlayer = match->captured(1);

            if(spectating)
            {
                emit pDebug("CreateGameResult: Avoid spectator game result.", 0);
            }
            else
            {
                QString logFileName = createGameLog(logSeek);
                createGameResult(logFileName);
            }

            emit endGame();
        }
        //Turn
        else if(line.contains(QRegularExpression("Entity=GameEntity tag=TURN value=(\\d+)"
                ), match))
        {
            turn = match->captured(1).toInt();
            emit pDebug("Found TURN: " + match->captured(1), numLine);

            if(powerState != inGameState)
            {
                powerState = inGameState;
                emit pDebug("WARNING: Heroes/Players info missing (powerState = inGameState)", 0);
            }
        }
    }
    switch(powerState)
    {
        case noGame:
            break;
        case heroType1State:
        case heroType2State:
            if(powerState == heroType1State && line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero1 = match->captured(1);
                powerState = heroType2State;
                emit pDebug("Found hero 1: " + hero1 + " (powerState = heroType2State)", numLine);
            }
            else if(powerState == heroType2State && line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero2 = match->captured(1);
                if(spectating)
                {
                    powerState = inGameState;
                    turn = 1;
                    emit pDebug("Found hero 2: " + hero2 + " (powerState = inGameState)", numLine);
                }
                else
                {
                    powerState = playerName1State;
                    emit pDebug("Found hero 2: " + hero2 + " (powerState = playerName1State)", numLine);
                }

            }
        case playerName1State:
            if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYER_ID value=2"), match))
            {
                name2 = match->captured(1);
                if(name2 == playerTag)
                {
                    playerID = 2;
                    secretHero = getSecretHero(hero2, hero1);
                    emit enemyHero(hero1);
                }
                powerState = playerName2State;
                emit pDebug("Found player 2: " + name2 + " (powerState = playerName2State)", numLine);
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
                emit pDebug("Found First Player: " + firstPlayer, numLine);
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
                    emit enemyHero(hero2);
                }
                powerState = inGameState;
                emit pDebug("Found player 1: " + name1 + " (powerState = inGameState)", numLine);
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
                emit pDebug("Found First Player: " + firstPlayer, numLine);
            }
            break;
        case inGameState:
            processPowerInGame(line, numLine);
            break;
    }
}


bool GameWatcher::isHeroPower(QString code)
{
    if( code=="CS2_102" || code=="CS2_083b" || code=="CS2_034" ||
        code=="CS1h_001" || code=="CS2_056" || code=="CS2_101" ||
        code=="CS2_017" || code=="DS1h_292" || code=="CS2_049")
            return true;
    else    return false;
}


void GameWatcher::processPowerInGame(QString &line, qint64 numLine)
{
    if(synchronized)
    {
        //Jugador roba carta inicial
        if(line.contains(QRegularExpression(
                "m_chosenEntities\\[\\d+\\]=\\[name=.* id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\]"
                ), match))
        {
            QString cardId = match->captured(1);
            QString player = match->captured(2);

            emit pDebug("Player: Starting card drawn: " + cardId, numLine);

            //Descubrimos el playerID del jugador y su playerTag (si playerTag no estaba definido)
            if(playerID == 0)
            {
                playerID = player.toInt();

                if(playerID == 1)
                {
                    if(!spectating)  playerTag = name1;
                    secretHero = getSecretHero(hero1, hero2);
                }
                else if(playerID == 2)
                {
                    if(!spectating)  playerTag = name2;
                    secretHero = getSecretHero(hero2, hero1);
                }
                else
                {
                    playerID = 0;
                    emit pDebug("Read invalid PlayerID value: " + player, 0, Error);
                }

                if(!spectating)
                {
                    QSettings settings("Arena Tracker", "Arena Tracker");
                    settings.setValue("playerTag", playerTag);
                    emit pDebug("Defined playerID, secretHero and playerTag: " + playerTag, 0);
                }
                else
                {
                    emit pDebug("Defined playerID and secretHero in spectator game.", 0);
                }
            }

            emit playerCardDraw(match->captured(1));
        }
        //Enemigo roba carta inicial
        else if(line.contains(QRegularExpression(
                      "GameState\\.DebugPrintEntityChoices\\(\\) - *Entities\\[\\d+\\]="
                      "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=(\\d+)\\]"
                      ), match))
        {
            QString id = match->captured(1);
            QString player = match->captured(2);

            if(player.toInt() != playerID)
            {
                emit pDebug("Enemy: Starting card drawn. ID: " + id, numLine);
                emit enemyCardDraw(id.toInt());
            }
        }
        //Enemigo mulligan
        else if(line.contains(QRegularExpression("Entity=(.+) tag=MULLIGAN_STATE value=DONE"
                ), match))
        {
            if(!mulliganEnemyDone && match->captured(1) != playerTag)
            {
                if(firstPlayer == playerTag)
                {
                    //Convertir ultima carta en moneda enemiga
                    emit pDebug("Enemy: Coin created.", 0);
                    emit lastHandCardIsCoin();
                }
                emit pDebug("Enemy mulligan end. Minions: 0", numLine);
                mulliganEnemyDone = true;
                playerMinions = 0;
                enemyMinions = 0;
            }
        }

        //ULTIMO TRIGGER SPECIAL CARDS
        //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=POWER Entity=[name=Robo de ideas id=31 zone=PLAY zonePos=0 cardId=EX1_339 player=1]
        //EffectCardId= EffectIndex=-1 Target=0
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
            "Entity=\\[name=(.*) id=\\d+ zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] "
            "EffectCardId=\\w* EffectIndex=-?\\d+ Target="
            ), match))
        {
            QString blockType = match->captured(1);
            QString name = match->captured(2);
            QString cardId = match->captured(3);
            QString player = match->captured(4);

            emit pDebug("Trigger(" + blockType + "): " + name, numLine);
            emit specialCardTrigger(cardId, blockType);
            if(isHeroPower(cardId) && isPlayerTurn && player.toInt()==playerID)     emit playerHeroPower();
        }

        //Jugador/Enemigo accion con objetivo
        //GameState.DebugPrintPower() - BLOCK_START BlockType=ATTACK Entity=[name=Arquera elfa id=51 zone=PLAY zonePos=1 cardId=CS2_189 player=2]
        //EffectCardId= EffectIndex=-1 Target=[name=Acaparador de botín id=31 zone=PLAY zonePos=1 cardId=EX1_096 player=1]
        else if(line.contains(QRegularExpression(
            "GameState\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
            "Entity=\\[name=(.*) id=\\d+ zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] "
            "EffectCardId=\\w* EffectIndex=(-?\\d+) "
            "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
            ), match))
        {
            QString blockType = match->captured(1);
            QString name1 = match->captured(2);
            QString zone = match->captured(3);
            QString cardId1 = match->captured(4);
            QString player1 = match->captured(5);
            QString index = match->captured(6);
            QString name2 = match->captured(7);
            QString cardId2 = match->captured(8);

            bool isPlayer = (player1.toInt() == playerID);


            //Jugador/Enemigo juega carta con objetivo
            if(zone == "HAND" && blockType == "PLAY" && index == "0")
            {
                DeckCard deckCard(cardId1);
                if(deckCard.getType() == SPELL)
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Spell obj played: " +
                                name1 + " on target " + name2, numLine);
                    if(cardId2 == MAD_SCIENTIST)
                    {
                        emit pDebug("Skip spell obj testing (Mad Scientist died).", 0);
                    }
                    else if(isPlayer && isPlayerTurn)    emit playerSpellObjPlayed();
                }
                else
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Minion/weapon obj played: " +
                                name1 + " target " + name2, numLine);
                }
            }

            //Jugador/Enemigo ataca (esbirro/heroe VS esbirro/heroe)
            else if(zone == "PLAY" && blockType == "ATTACK" && index == "-1")
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
    //Carta desconocida (Enemigo)
    if(line.contains(QRegularExpression(
        "\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=\\d+\\] zone from (.*) -> OPPOSING (HAND|SECRET|DECK)"
        ), match))
    {
        QString id = match->captured(1);
        QString zoneFrom = match->captured(2);
        QString zoneToOpposing = match->captured(3);


        //Enemigo juega carta desconocida
        if(zoneFrom == "OPPOSING HAND")
        {
            emit pDebug("Enemy: Unknown card played. ID: " + id, numLine);
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

        //Enemigo roba carta desconocida
        else if(zoneToOpposing == "HAND")
        {
            //Enemigo roba carta de deck
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
            }
        }
    }


    //[Zone] ZoneChangeList.ProcessChanges() - id=3 local=True [name=Acólito de dolor id=10 zone=HAND zonePos=2 cardId=EX1_007 player=1]
    //zone from FRIENDLY HAND -> FRIENDLY PLAY
    //Carta conocida
    else if(line.contains(QRegularExpression(
        "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from (\\w+ \\w+(?: \\(Weapon\\))?)? -> (\\w+ \\w+(?: \\(Weapon\\))?)?"
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
            if(zoneFrom == "OPPOSING DECK")
            {
                advanceTurn(false);
                emit enemyKnownCardDraw(cardId);
            }
            emit pDebug("Enemy: Known card to hand: " + name + " ID: " + id, numLine);
            emit enemyCardDraw(id.toInt(), turnReal, false, cardId);
        }
        //Enemigo, nuevo minion en PLAY
        else if(zoneTo == "OPPOSING PLAY" && zoneFrom != "OPPOSING PLAY")
        {
            enemyMinions++;
            emit pDebug("Enemy: Minion moved to OPPOSING PLAY: " + name + " Minions: " + QString::number(enemyMinions), numLine);
        }
        //Jugador, nuevo minion en PLAY
        else if(zoneTo == "FRIENDLY PLAY" && zoneFrom != "FRIENDLY PLAY")
        {
            playerMinions++;
            emit pDebug("Player: Minion moved to FRIENDLY PLAY: " + name + " Minions: " + QString::number(playerMinions), numLine);
        }

        //Enemigo roba secreto (kezan mystic)
        if(zoneFrom == "FRIENDLY SECRET" && zoneTo == "OPPOSING SECRET")
        {
            emit pDebug("Enemy: Secret stolen: " + name + " ID: " + id, numLine);
            emit enemySecretStealed(id.toInt(), cardId);
        }

        //Enemigo juega carta conocida
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
                emit pDebug("Enemy: Minion played: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
            }
            //Enemigo juega arma
            else if(zoneTo == "OPPOSING PLAY (Weapon)")
            {
                emit pDebug("Enemy: Weapon played: " + name + " ID: " + id, numLine);
            }
            //Enemigo descarta carta
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

        //Enemigo roba carta overdraw
        else if(zoneFrom == "OPPOSING DECK" && zoneTo == "OPPOSING GRAVEYARD")
        {
            advanceTurn(false);
            emit pDebug("Enemy: Card overdraw: " + name, numLine);
            emit enemyKnownCardDraw(cardId);
        }

        //Jugador roba carta conocida
        else if(zoneFrom == "FRIENDLY DECK" && !zoneTo.isEmpty())
        {
            advanceTurn(true);
            emit pDebug("Player: Card drawn: " + name, numLine);
            emit playerCardDraw(cardId);
        }

        //Jugador juega carta conocida
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
                emit pDebug("Player: Minion played: " + name + " Minions: " + QString::number(playerMinions), numLine);
                if(isPlayerTurn)    emit playerMinionPlayed(playerMinions);
            }
            //Jugador juega arma
            else if(zoneTo == "FRIENDLY PLAY (Weapon)")
            {
                emit pDebug("Player: Weapon played: " + name, numLine);
            }
            //Jugador descarta carta
            else if(zoneTo == "FRIENDLY GRAVEYARD")
            {
                emit pDebug("Player: Card discarded: " + name, numLine);
            }
        }

        //Enemigo esbirro muere
        else if(zoneFrom == "OPPOSING PLAY" && zoneTo != "OPPOSING PLAY")
        {
            if(enemyMinions>0)  enemyMinions--;
            emit pDebug("Enemy: Minion removed from OPPOSING PLAY: " + name + " Minions: " + QString::number(enemyMinions), numLine);

            if(zoneTo == "OPPOSING GRAVEYARD" && isPlayerTurn)
            {
                emit enemyMinionDead(cardId);
                if(enemyMinionsAliveForAvenge == -1)
                {
                    if(cardId == MAD_SCIENTIST)
                    {
                        emit pDebug("Skip avenge testing (Mad Scientist died).", 0);
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
        else if(zoneFrom == "FRIENDLY PLAY" && zoneTo != "FRIENDLY PLAY")
        {
            if(playerMinions>0) playerMinions--;
            emit pDebug("Player: Minion removed from FRIENDLY PLAY: " + name + " Minions: " + QString::number(playerMinions), numLine);
        }
    }


    //Jugador/Enemigo esbirro cambia pos
    //No podemos usar zonePos= porque para los esbirros del jugador que pasan a una posicion mayor muestra su posicion origen
    //Todo comentado porque esta forma de contar el numero de esbirros puede producir errores.
    //Ej: Si un esbirro con deathrattle produce otro esbirro. Primero se cambia la pos de los esbirros a la dcha
    //y despues se genran los esbirros de deathrattle causando una suma erronea.
//    else if(line.contains(QRegularExpression(
//        "\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=\\w+ player=(\\d+)\\] pos from \\d+ -> (\\d+)"
//        ), match))
//    {
//        QString name = match->captured(1);
//        QString player = match->captured(2);
//        QString zonePos = match->captured(3);

//        //Jugador esbirro cambia pos
//        if(player.toInt() == playerID)
//        {
//            if(zonePos.toInt() > playerMinions) playerMinions = zonePos.toInt();
//            emit pDebug("Player: New minion pos: " +
//                        name + " >> " + zonePos + " Minions: " + QString::number(playerMinions), numLine);
//        }
//        //Enemigo esbirro cambia pos
//        else
//        {
//            if(zonePos.toInt() > enemyMinions) enemyMinions = zonePos.toInt();
//            emit pDebug("Enemy: New minion pos: " +
//                        name + " >> " + zonePos + " Minions: " + QString::number(enemyMinions), numLine);
//        }
//    }
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


void GameWatcher::createGameResult(QString logFileName)
{
    GameResult gameResult;

    if(playerID == 1)
    {
        gameResult.playerHero = hero1;
        gameResult.enemyHero = hero2;
        gameResult.enemyName = name2;
    }
    else if(playerID == 2)
    {
        gameResult.playerHero = hero2;
        gameResult.enemyHero = hero1;
        gameResult.enemyName = name1;
    }
    else
    {
        emit pDebug("CreateGameResult: PlayerID wasn't defined in the game.", 0, Error);
        return;
    }

    gameResult.isFirst = (firstPlayer == playerTag);
    gameResult.isWinner = (winnerPlayer == playerTag);

    emit newGameResult(gameResult, loadingScreenState, logFileName);
}


QString GameWatcher::createGameLog(qint64 logSeekWon)
{
    if(!copyGameLogs)
    {
        emit pDebug("Game log copy disabled.", 0);
        return "";
    }
    if(logSeekCreate == -1)
    {
        emit pDebug("Cannot create match log. Found WON but not CREATE_GAME", 0);
        return "";
    }

    QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh-mm");
    QString win = (winnerPlayer == playerTag)?"WIN":"LOSE";
    QString gameMode = Utility::getLoadingScreenString(loadingScreenState);
    QString playerHero, enemyHero;
    if(playerID == 1)
    {
        playerHero = Utility::heroStringFromLogNumber(hero1);
        enemyHero = Utility::heroStringFromLogNumber(hero2);
    }
    else
    {
        playerHero = Utility::heroStringFromLogNumber(hero2);
        enemyHero = Utility::heroStringFromLogNumber(hero1);
    }
    QString fileName = gameMode + " " + timeStamp + " " + playerHero + "vs" + enemyHero + " " + win + ".arenatracker";


    emit pDebug("Game log ready to be copied.", 0);
    emit gameLogComplete(logSeekCreate, logSeekWon, fileName);
    logSeekCreate = -1;

    return fileName;
}


void GameWatcher::advanceTurn(bool playerDraw)
{
    if(turnReal == turn)    return;

    bool playerTurn;
    if((firstPlayer==playerTag && turn%2==1) || (firstPlayer!=playerTag && turn%2==0))  playerTurn=true;
    else    playerTurn=false;

    //Al turno 1 dejamos que pase cualquiera asi dejamos el turno 0 para indicar cartas de mulligan
    //Solo avanza de turno al robar carta el jugador que le corresponde
    if(turn == 1 || playerDraw == playerTurn || spectating)
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


void GameWatcher::setSynchronized()
{
    this->synchronized = true;
}


LoadingScreenState GameWatcher::getLoadingScreen()
{
    return this->loadingScreenState;
}


void GameWatcher::setCopyGameLogs(bool value)
{
    this->copyGameLogs = value;
}



// RACHELLE PROCESS
//    else if(line.startsWith("[Rachelle]"))
//    {
//        if(line.contains(QRegularExpression("reward \\d=\\[")))
//        {
//            gameState = inRewards;
//            emit pDebug("New reward (GameState = inRewards).", numLine);

//            if(line.contains("BoosterPackRewardData"))
//            {
//                emit newArenaReward(0,0,true,false,false);
//            }
//            else if(line.contains("CardRewardData"))
//            {
//                if(line.contains(QRegularExpression("Premium=(STANDARD|GOLDEN)"), match))
//                {
//                    QString cardType = match->captured(1);
//                    if(cardType.compare("STANDARD") == 0)
//                    {
//                        emit newArenaReward(0,0,false,false,true);
//                    }
//                    else
//                    {
//                        emit newArenaReward(0,0,false,true,false);
//                    }

//                }
//            }
//            else if(line.contains(QRegularExpression("GoldRewardData: Amount=(\\d+)"), match))
//            {
//                QString gold = match->captured(1);
//                emit newArenaReward(gold.toInt(),0,false,false,false);
//            }
//            else if(line.contains(QRegularExpression("ArcaneDustRewardData: Amount=(\\d+)"), match))
//            {
//                QString dust = match->captured(1);
//                emit newArenaReward(0, dust.toInt(),false,false,false);
//            }
//        }
//    }

// ASSET PROCESS
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
//                emit pDebug("Discard HERO card", numLine);
//                endReadingDeck();
//                return;
//            }
//            //Hero powers
//            if( code=="CS2_102" || code=="CS2_083b" || code=="CS2_034" ||
//                code=="CS1h_001" || code=="CS2_056" || code=="CS2_101" ||
//                code=="CS2_017" || code=="DS1h_292" || code=="CS2_049")
//            {
//                emit pDebug("Discard HERO POWER", numLine);
//                endReadingDeck();
//                return;
//            }
//            emit pDebug("Read code: " + code, numLine);
//            emit newDeckCard(code);
//        }
//    }

//  BOB PROCESS
//    if(line.startsWith("[Bob]"))
//    {
//        if(line.startsWith("[Bob] ---Register"))
//        {
//            //Redundante en caso de que falle
//            //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
//            endReadingDeck();

//            if(line.startsWith("[Bob] ---RegisterScreenForge---"))
//            {
//                arenaMode = true;
//                emit pDebug("Entering arena.", numLine);
//                emit enterArena();
//            }
//            else if(line.startsWith("[Bob] ---RegisterProfileNotices---") ||
//                    line.startsWith("[Bob] ---RegisterFriendChallenge---"))
//            {
//            }
//            else if(line.startsWith("[Bob] ---RegisterScreenEndOfGame---"))
//            {
//                emit endGame();
//                gameState = noGame;
//                emit pDebug("Found ScreenEndOfGame (GameState = noGame).\n", numLine);
//            }
//            else
//            {
//                arenaMode = false;
//                emit pDebug("Leaving arena.", numLine);
//                emit leaveArena();
//            }
//        }
//    }

