#include "gamewatcher.h"
#include <QtWidgets>


GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    reset();
    match = new QRegularExpressionMatch();
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
    mulliganEnemyDone = mulliganPlayerDone = false;
    turn = turnReal = 0;
    spectating = false;
    logSeekCreate = -1;
    logSeekWon = -1;
    tied = true;
    emit pDebug("Reset (powerState = noGame).", 0);
    emit pDebug("Reset (LoadingScreen = menu).", 0);
}


void GameWatcher::processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    switch(logComponent)
    {
        case logPower:
            processPower(line, numLine, logSeek);
        break;
        case logZone:
            processZone(line, numLine);
        break;
        case logLoadingScreen:
            processLoadingScreen(line, numLine);
        break;
        case logArena:
            processArena(line, numLine);
        break;
        case logAsset:
            processAsset(line, numLine);
        break;
        case logInvalid:
            emit pDebug("Unknown log component read.", Warning);
        break;
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
    emit arenaDeckRead();
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

        //Create result, avoid first run
        if(prevMode == "GAMEPLAY" && logSeekCreate != -1 && logSeekWon != -1)
        {
            if(spectating || loadingScreenState == menu || tied)
            {
                emit pDebug("CreateGameResult: Avoid spectator/tied game result.", 0);
            }
            else
            {
                QString logFileName = createGameLog();
                createGameResult(logFileName);
            }
            spectating = false;
        }

        if(currMode == "DRAFT")
        {
            loadingScreenState = arena;
            emit pDebug("Entering ARENA (loadingScreenState = arena).", numLine);

            if(prevMode == "HUB" || prevMode == "FRIENDLY")
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
            loadingScreenState = casual;
            emit pDebug("Entering CASUAL/RANKED (loadingScreenState = casual).", numLine);
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
        else if(currMode == "FRIENDLY")
        {
            loadingScreenState = friendly;
            emit pDebug("Entering FRIENDLY (loadingScreenState = friendly).", numLine);

            if(prevMode == "DRAFT")
            {
                setDeckRead(false);
                emit leaveArena();//leaveArena deckHandler
            }
        }
    }
}


void GameWatcher::processAsset(QString &line, qint64 numLine)
{
    if(powerState != noGame)   return;

    //Definimos RANKED solo si venimos de loadScreen TOURNAMENT y
    //acabamos de encontrar el WON pero aun no hemos createResult
    if(loadingScreenState == casual && logSeekWon != -1 && line.contains("assetPath=rank_window"))
    {
        loadingScreenState = ranked;
        emit pDebug("On RANKED (loadingScreenState = ranked).", numLine);
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
    else if(line.contains("SetDraftMode - ACTIVE_DRAFT_DECK"))
    {
        emit pDebug("Found ACTIVE_DRAFT_DECK.", numLine);
        emit activeDraftDeck(); //End draft
        endReadingDeck();
    }
    //DRAFTING PICK CARD
    //[Arena] Client chooses: Profesora violeta (NEW1_026)
    else if(line.contains(QRegularExpression("Client chooses: .* \\((\\w+)\\)"), match))
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
    else if(line.contains(QRegularExpression(
                "DraftManager\\.OnChoicesAndContents - Draft Deck ID: \\d+, Hero Card = HERO_\\d+"), match))
    {
        emit pDebug("Found DraftManager.OnChoicesAndContents", numLine);
        startReadingDeck();
    }
    //READ DECK CARD
    //[Arena] DraftManager.OnChoicesAndContents - Draft deck contains card FP1_012
    else if((arenaState == readingDeck) && line.contains(QRegularExpression(
            "DraftManager\\.OnChoicesAndContents - Draft deck contains card (\\w+)"), match))
    {
        QString code = match->captured(1);
        emit pDebug("Reading deck: " + code, numLine);
        emit newDeckCard(code);
    }
    //IN REWARDS
    //[Arena] SetDraftMode - IN_REWARDS
    else if(line.contains("SetDraftMode - IN_REWARDS"))
    {
        emit pDebug("Found IN_REWARDS.", numLine);
        emit inRewards();   //Show rewards input
    }
}


//Ejemplo spectate
//11:22:01 - GameWatcher(1): Start Spectator Game.
//11:22:01 - GameWatcher(3): Found CREATE_GAME (powerState = heroType1State)
//11:22:04 - GameWatcher(22): LoadingScreen: HUB -> GAMEPLAY
//Juego arena
//20:58:25 - GameWatcher(27229): Found WON (powerState = noGame): Dappo
//20:58:35 - GameWatcher(27255): End Spectator Game.
//20:58:36 - GameWatcher(346): LoadingScreen: GAMEPLAY -> HUB
//20:58:36 - GameWatcher: CreateGameResult: Avoid spectator/tied game result.
//20:58:36 - GameWatcher(346): Entering MENU (loadingScreenState = menu).
//Nuevo juego arena
//20:58:54 - GameWatcher(27256): Start Spectator Game.
//20:58:56 - GameWatcher(27258): Found CREATE_GAME (powerState = heroType1State)
//20:58:56 - GameWatcher(376): LoadingScreen: HUB -> GAMEPLAY

void GameWatcher::processPower(QString &line, qint64 numLine, qint64 logSeek)
{
    //================== End Spectator Game ==================
    if(line.contains("End Spectator Game"))
    {
        emit pDebug("End Spectator Game.", numLine);
//        spectating = false;//Se pondra a false despues de haberse creado el resultado en LoadingScreen: GAMEPLAY -> HUB

        if(powerState != noGame)
        {
            emit pDebug("WON not found (PowerState = noGame)", 0);
            powerState = noGame;
            emit endGame();
        }
    }

    //================== Begin Spectating 1st player ==================
    //================== Start Spectator Game ==================
    else if(line.contains("Begin Spectating") || line.contains("Start Spectator Game"))
    {
        emit pDebug("Start Spectator Game.", numLine);
        spectating = true;
    }
    //Create game
    //GameState.DebugPrintPower() - CREATE_GAME
    else if(line.contains("GameState.DebugPrintPower() - CREATE_GAME"))
    {
        if(powerState != noGame)
        {
            emit pDebug("WON not found (PowerState = noGame)", 0);
            powerState = noGame;
            emit endGame();
        }

        emit pDebug("\nFound CREATE_GAME (powerState = heroType1State)", numLine);
        logSeekCreate = logSeek;
        powerState = heroType1State;

        mulliganEnemyDone = mulliganPlayerDone = false;
        turn = turnReal = 0;

        hero1.clear();
        hero2.clear();
        name1.clear();
        name2.clear();
        firstPlayer.clear();
        winnerPlayer.clear();
        playerID = 0;
        playerTag.clear();
        secretHero = INVALID_CLASS;
        playerMinions = 0;
        enemyMinions = 0;
        enemyMinionsAliveForAvenge = -1;
        enemyMinionsDeadThisTurn = 0;
        playerCardsPlayedThisTurn = 0;
        startGameEpoch = QDateTime::currentMSecsSinceEpoch()/1000;
        tied = true;//Si no se encuentra WON no se llamara a createGameResult() pq tried sigue siendo true

        emit specialCardTrigger("", "", -1, -1);    //Evita Cartas createdBy en el mulligan de practica
        emit startGame();
    }

    if(powerState != noGame)
    {
        //Win state
        //PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=El tabernero tag=PLAYSTATE value=WON
        if(line.contains(QRegularExpression(
                            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
                            "Entity=(.+) tag=PLAYSTATE value=(WON|TIED)"), match))
        {
            winnerPlayer = match->captured(1);
            tied = (match->captured(2) == "TIED");
            powerState = noGame;
            logSeekWon = logSeek;
            if(tied)    emit pDebug("Found TIED (powerState = noGame)", numLine);
            else        emit pDebug("Found WON (powerState = noGame): " + winnerPlayer + (playerTag.isEmpty()?" - Unknown winner":""), numLine);

            bool playerWon = !tied && (winnerPlayer == playerTag);
            emit endGame(playerWon, playerTag.isEmpty());
        }
        //Turn
        //PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=GameEntity tag=TURN value=12
        else if(line.contains(QRegularExpression(
                            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
                            "Entity=GameEntity tag=TURN value=(\\d+)"
                ), match))
        {
            turn = match->captured(1).toInt();
            emit logTurn();
            emit pDebug("Found TURN: " + match->captured(1), numLine);

            if(powerState != inGameState && turn > 1)
            {
                powerState = inGameState;
                mulliganEnemyDone = mulliganPlayerDone = true;
                emit clearDrawList(true);
                emit pDebug("WARNING: Heroes/Players info missing (powerState = inGameState, mulliganDone = true)", 0, Warning);
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
                powerState = mulliganState;
                emit pDebug("Found hero 2: " + hero2 + " (powerState = mulliganState)", numLine);
            }
        case mulliganState:
            processPowerMulligan(line, numLine);
            break;
        case inGameState:
            processPowerInGame(line, numLine);
            break;
    }
}


void GameWatcher::processPowerMulligan(QString &line, qint64 numLine)
{
    //Jugador/Enemigo names, playerTag y firstPlayer
    //GameState.DebugPrintEntityChoices() - id=1 Player=fayatime TaskList=3 ChoiceType=MULLIGAN CountMin=0 CountMax=5
    //GameState.DebugPrintEntityChoices() - id=2 Player=Винсент TaskList=4 ChoiceType=MULLIGAN CountMin=0 CountMax=3
    if(line.contains(QRegularExpression(
                "GameState\\.DebugPrintEntityChoices\\(\\) - id=(\\d+) Player=(.*) TaskList=\\d+ ChoiceType=MULLIGAN CountMin=0 CountMax=(\\d+)"
                  ), match))
    {
        QString player = match->captured(1);
        QString playerName = match->captured(2);
        QString numCards = match->captured(3);

        if(player.toInt() == 1)
        {
            name1 = playerName;
            emit pDebug("Found player 1: " + name1, numLine);
        }
        else if(player.toInt() == 2)
        {
            name2 = playerName;
            emit pDebug("Found player 2: " + name2, numLine);
        }
        else    emit pDebug("Read invalid PlayerID value: " + player, numLine, DebugLevel::Error);

        //No se usa. El playerID se calcula (junto al playerTag) al cargar el retrato del heroe en processZone.
        if(playerTag.isEmpty() && playerID == player.toInt())
        {
            playerTag = (playerID == 1)?name1:name2;
            emit pDebug("Found playerTag: " + playerTag, numLine);
        }

        if(numCards == "3")
        {
            firstPlayer = playerName;
            emit pDebug("Found First Player: " + firstPlayer, numLine);
        }
    }


    //MULLIGAN DONE
    //GameState.DebugPrintPower() -     TAG_CHANGE Entity=fayatime tag=MULLIGAN_STATE value=DONE
    //GameState.DebugPrintPower() -     TAG_CHANGE Entity=Винсент tag=MULLIGAN_STATE value=DONE
    else if(line.contains(QRegularExpression("Entity=(.+) tag=MULLIGAN_STATE value=DONE"
            ), match))
    {
        //Player mulligan
        if(match->captured(1) == playerTag)
        {
            if(!mulliganPlayerDone)
            {
                emit pDebug("Player mulligan end.", numLine);
                mulliganPlayerDone = true;
                turn = 1;
                emit clearDrawList(true);

                if(mulliganEnemyDone)
                {
                    //turn = 1;
                    powerState = inGameState;
                    emit pDebug("Mulligan phase end (powerState = inGameState)", numLine);
                }
            }
        }
        //Enemy mulligan
        else
        {
            if(!mulliganEnemyDone)
            {
                if(firstPlayer == playerTag)
                {
                    //Convertir ultima carta en moneda enemiga
                    emit pDebug("Enemy: Coin created.", 0);
                    emit lastHandCardIsCoin();
                }
                emit pDebug("Enemy mulligan end.", numLine);
                mulliganEnemyDone = true;
                turn = 1;

                if(mulliganPlayerDone)
                {
                    //turn = 1;
                    powerState = inGameState;
                    emit pDebug("Mulligan phase end (powerState = inGameState)", numLine);
                }
            }
        }
    }


    //GameState.DebugPrintEntityChoices() -   Entities[4]=[entityName=La moneda id=68 zone=HAND zonePos=5 cardId=GAME_005 player=2]
    //GameState.DebugPrintEntityChoices() -   Entities[4]=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=68 zone=HAND zonePos=5 cardId= player=1]
    else if(line.contains(QRegularExpression(
                "GameState\\.DebugPrintEntityChoices\\(\\) - *"
                "Entities\\[4\\]=\\[entityName=.* id=(\\d+) zone=HAND zonePos=5 cardId=.* player=\\d+\\]"
                  ), match))
    {
        QString id = match->captured(1);
        emit pDebug("Coin ID: " + id, numLine);
        emit coinIdFound(id.toInt());
    }
}


void GameWatcher::processPowerInGame(QString &line, qint64 numLine)
{
    //SHOW_ENTITY tag
    //tag=HEALTH value=1
    if(line.contains(QRegularExpression(
        "PowerTaskList\\.DebugPrintPower\\(\\) - *"
        "tag=(\\w+) value=(\\w+)"
        ), match))
    {
        QString tag = match->captured(1);
        QString value = match->captured(2);

        if(tag == "ATK" || tag == "HEALTH")
        {
            emit pDebug((lastShowEntity.isPlayer?QString("Player"):QString("Enemy")) + ": SHOW_TAG(" + tag + ")= " + value, numLine);
            if(lastShowEntity.id == -1)         emit pDebug("Show entity id missing.", DebugLevel::Error);
            else if(lastShowEntity.isPlayer)    emit playerBoardTagChange(lastShowEntity.id, "", tag, value);
            else                                emit enemyBoardTagChange(lastShowEntity.id, "", tag, value);
        }
        //En un futuro quizas haya que distinguir entre cambios en zone HAND o PLAY, por ahora son siempre cambios en PLAY
    }
    else
    {
        //Reiniciamos lastShowEntity
        lastShowEntity.id = -1;

        //TAG_CHANGE jugadores conocido
        //D 10:48:46.1127070 PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=SerKolobok tag=RESOURCES value=3
        if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
            "Entity=(.*) tag=(\\w+) value=(\\w+)"
            ), match))
        {
            QString name = match->captured(1);
            QString tag = match->captured(2);
            QString value = match->captured(3);
            bool isPlayer = (name == playerTag);

            if(tag == "RESOURCES" || tag == "RESOURCES_USED" || tag == "CURRENT_SPELLPOWER")
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": TAG_CHANGE(" + tag + ")= " + value +
                            " -- Name: " + name, numLine);
                if(!playerTag.isEmpty())
                {
                    if(isPlayer)    emit playerTagChange(tag, value);
                    else            emit enemyTagChange(tag, value);
                }
                else
                {
                    emit unknownTagChange(tag, value);
                }
            }
        }


        //TAG_CHANGE desconocido
        //TAG_CHANGE Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=49 zone=HAND zonePos=3 cardId= player=2] tag=CLASS value=MAGE
        //TAG_CHANGE Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=37 zone=HAND zonePos=2 cardId= player=2] tag=CLASS value=MAGE
        if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
            "Entity=\\[entityName=UNKNOWN ENTITY \\[cardType=INVALID\\] id=(\\d+) zone=\\w+ zonePos=\\d+ cardId= player=(\\d+)\\] "
            "tag=(\\w+) value=(\\w+)"
            ), match))
        {
            QString id = match->captured(1);
            QString player = match->captured(2);
            QString tag = match->captured(3);
            QString value = match->captured(4);
            bool isPlayer = (player.toInt() == playerID);


            if(tag == "CLASS")
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Secret hero = " + value +
                            " -- Id: " + id, numLine);
                if(value == "MAGE")         secretHero = MAGE;
                else if(value == "HUNTER")  secretHero = HUNTER;
                else if(value == "PALADIN") secretHero = PALADIN;
                else if(value == "ROGUE")   secretHero = ROGUE;
            }
            //Justo antes de jugarse ARMS_DEALING se pone a 0, si no lo evitamos el minion no se actualizara desde la carta
            else if(tag == "ARMS_DEALING" && value.toInt() != 0)
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": TAG_CHANGE(" + tag + ")= " + value +
                            " -- Id: " + id, numLine);
                emit buffHandCard(id.toInt());
            }
            else if(tag == "DAMAGE" || tag == "ATK" || tag == "HEALTH" || tag == "EXHAUSTED" ||
                    tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                    tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" ||
                    tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "AURA" ||
                    tag == "CANT_BE_DAMAGED" || tag == "SHOULDEXITCOMBAT" || tag == "ZONE" ||
                    tag == "LINKED_ENTITY" || tag == "DURABILITY" ||
                    tag == "COST")
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": MINION/CARD TAG_CHANGE(" + tag + ")= " + value +
                            " -- Id: " + id, numLine);
                if(isPlayer)    emit playerBoardTagChange(id.toInt(), "", tag, value);
                else            emit enemyBoardTagChange(id.toInt(), "", tag, value);
            }
        }

        //TAG_CHANGE conocido
        //PowerTaskList aparece segundo pero hay acciones que no tienen GameState, como el damage del maestro del acero herido
        //GameState.DebugPrintPower() -         TAG_CHANGE Entity=[entityName=Déspota del templo id=36 zone=PLAY zonePos=1 cardId=EX1_623 player=2] tag=DAMAGE value=0
        //GameState.DebugPrintPower() -     TAG_CHANGE Entity=[entityName=Déspota del templo id=36 zone=PLAY zonePos=1 cardId=EX1_623 player=2] tag=ATK value=3
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
            "Entity=\\[entityName=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=(\\w*) player=(\\d+)\\] "
            "tag=(\\w+) value=(\\w+)"
            ), match))
        {
            QString name = match->captured(1);
            QString id = match->captured(2);
            QString zone = match->captured(3);
            QString cardId = match->captured(4);
            QString player = match->captured(5);
            QString tag = match->captured(6);
            QString value = match->captured(7);
            bool isPlayer = (player.toInt() == playerID);
            Q_UNUSED(zone);


            if(tag == "DAMAGE" || tag == "ATK" || tag == "HEALTH" || tag == "EXHAUSTED" ||
                    tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                    tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" ||
                    tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "AURA" ||
                    tag == "CANT_BE_DAMAGED" || tag == "SHOULDEXITCOMBAT" || tag == "ZONE" ||
                    tag == "LINKED_ENTITY" || tag == "DURABILITY" ||
                    tag == "COST")
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": MINION/CARD TAG_CHANGE(" + tag + ")=" + value +
                            " -- " + name + " -- Id: " + id, numLine);
                if(isPlayer)    emit playerBoardTagChange(id.toInt(), cardId, tag, value);
                else            emit enemyBoardTagChange(id.toInt(), cardId, tag, value);
            }
        }


        //Enemigo accion desconocida
        //BLOCK_START BlockType=PLAY Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=49 zone=HAND zonePos=3 cardId= player=2]
        //EffectCardId= EffectIndex=0 Target=0
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
            "Entity=\\[entityName=UNKNOWN ENTITY \\[cardType=INVALID\\] id=(\\d+) zone=\\w+ zonePos=\\d+ cardId= player=\\d+\\] "
            "EffectCardId=\\w* EffectIndex=-?\\d+ "
            "Target="
            ), match))
        {
            QString blockType = match->captured(1);
            QString id = match->captured(2);

            //ULTIMO TRIGGER SPECIAL CARDS, con o sin objetivo
            emit pDebug("Trigger(" + blockType + ") desconocido. Id: " + id, numLine);
            emit specialCardTrigger("", blockType, id.toInt(), -1);
        }


        //SHOW_ENTITY conocido
        //SHOW_ENTITY - Updating Entity=[entityName=Maestra de secta id=50 zone=DECK zonePos=0 cardId= player=2] CardID=EX1_595
        //SHOW_ENTITY - Updating Entity=[entityName=Turbocerdo con pinchos id=18 zone=DECK zonePos=0 cardId=CFM_688 player=1] CardID=CFM_688
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *SHOW_ENTITY - Updating "
            "Entity=\\[entityName=.* id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=\\w* player=(\\d+)\\] "
            "CardID=\\w+"
            ), match))
        {
            QString id = match->captured(1);
            QString player = match->captured(2);
            bool isPlayer = (player.toInt() == playerID);

            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": SHOW_ENTITY -- Id: " + id, numLine);
            lastShowEntity.id = id.toInt();
            lastShowEntity.isPlayer = isPlayer;
        }


        //SHOW_ENTITY desconocido
        //SHOW_ENTITY - Updating Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=58 zone=HAND zonePos=3 cardId= player=2] CardID=EX1_011
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *SHOW_ENTITY - Updating "
            "Entity=\\[entityName=UNKNOWN ENTITY \\[cardType=INVALID\\] id=(\\d+) zone=\\w+ zonePos=\\d+ cardId= player=(\\d+)\\] "
            "CardID=\\w+"
            ), match))
        {
            QString id = match->captured(1);
            QString player = match->captured(2);
            bool isPlayer = (player.toInt() == playerID);

            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": SHOW_ENTITY -- Id: " + id, numLine);
            lastShowEntity.id = id.toInt();
            lastShowEntity.isPlayer = isPlayer;
        }


        //FULL_ENTITY conocido
        //FULL_ENTITY - Updating [entityName=Recluta Mano de Plata id=95 zone=PLAY zonePos=3 cardId=CS2_101t player=2] CardID=CS2_101t
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *FULL_ENTITY - Updating "
            "\\[entityName=.* id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=\\w* player=(\\d+)\\] "
            "CardID=\\w+"
            ), match))
        {
            QString id = match->captured(1);
            QString player = match->captured(2);
            bool isPlayer = (player.toInt() == playerID);

            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": FULL_ENTITY -- Id: " + id, numLine);
            lastShowEntity.id = id.toInt();
            lastShowEntity.isPlayer = isPlayer;
        }


        //CHANGE_ENTITY conocido
        //CHANGE_ENTITY - Updating Entity=[entityName=Aullavísceras id=53 zone=HAND zonePos=3 cardId=EX1_411 player=2] CardID=OG_031
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *CHANGE_ENTITY - Updating Entity="
            "\\[entityName=.* id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=\\w* player=(\\d+)\\] "
            "CardID=(\\w+)"
            ), match))
        {
            QString id = match->captured(1);
            QString zone = match->captured(2);
            QString player = match->captured(3);
            QString newCardId = match->captured(4);
            bool isPlayer = (player.toInt() == playerID);

            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": CHANGE_ENTITY -- Id: " + id +
                        " to Code: " + newCardId + " in Zone: " + zone, numLine);
            lastShowEntity.id = id.toInt();
            lastShowEntity.isPlayer = isPlayer;

            if(isPlayer && zone == "HAND")
            {
                emit playerCardCodeChange(id.toInt(), newCardId);
            }
        }


        //Jugador/Enemigo accion con/sin objetivo
        //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=ATTACK Entity=[entityName=Jinete de lobos id=45 zone=PLAY zonePos=1 cardId=CS2_124 player=2]
        //EffectCardId= EffectIndex=-1 Target=[entityName=Jaina Valiente id=64 zone=PLAY zonePos=0 cardId=HERO_08 player=1]
        //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=TRIGGER Entity=[entityName=Trepadora embrujada id=12 zone=GRAVEYARD zonePos=0 cardId=FP1_002 player=1]
        //EffectCardId= EffectIndex=0 Target=0
        //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=POWER Entity=[entityName=Elemental de Escarcha id=43 zone=PLAY zonePos=1 cardId=EX1_283 player=2]
        //EffectCardId= EffectIndex=-1 Target=[entityName=Trituradora antigua de Sneed id=23 zone=PLAY zonePos=5 cardId=GVG_114 player=1]
        //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=FATIGUE Entity=[entityName=Malfurion Tempestira id=76 zone=PLAY zonePos=0 cardId=HERO_06 player=1]
        //EffectCardId= EffectIndex=0 Target=0
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
            "Entity=\\[entityName=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] "
            "EffectCardId=\\w* EffectIndex=-?\\d+ "
            "Target=(?:\\[entityName=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=\\d+\\])?"
            ), match))
        {
            QString blockType = match->captured(1);
            QString name1 = match->captured(2);
            QString id1 = match->captured(3);
            QString zone = match->captured(4);
            QString cardId1 = match->captured(5);
            QString player1 = match->captured(6);
            QString name2 = match->captured(7);
            QString id2 = match->captured(8);
            QString zone2 = match->captured(9);
            QString cardId2 = match->captured(10);
            bool isPlayer = (player1.toInt() == playerID);


            //ULTIMO TRIGGER SPECIAL CARDS, con o sin objetivo
            emit pDebug("Trigger(" + blockType + "): " + name1 + " (" + cardId1 + ")" +
                        (name2.isEmpty()?"":" --> " + name2 + " (" + cardId2 + ")"), numLine);
            emit specialCardTrigger(cardId1, blockType, id1.toInt(), id2.isEmpty()?-1:id2.toInt());
            if(isHeroPower(cardId1) && isPlayerTurn && player1.toInt()==playerID)     emit playerHeroPower();


            //Accion sin objetivo
            if(zone2.isEmpty())
            {
                if(blockType == "FATIGUE" && zone == "PLAY")
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Fatigue damage.", numLine);
                    if(advanceTurn(isPlayer))       emit newTurn(isPlayerTurn, turnReal);
                }
            }

            //Accion con objetivo en PLAY
            else if(zone2 == "PLAY")
            {
                //Jugador juega carta con objetivo en PLAY, No enemigo pq BlockType=PLAY es de entity desconocida para el enemigo
                if(blockType == "PLAY" && zone == "HAND")
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
                        else if(isPlayer && isPlayerTurn)
                        {
                            if(cardId2.startsWith("HERO"))  emit playerSpellObjHeroPlayed();
                            else                            emit playerSpellObjMinionPlayed();
                        }
                    }
                    else
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Minion/weapon obj played: " +
                                    name1 + " target " + name2, numLine);
                        if(isPlayer && isPlayerTurn)
                        {
                            if(cardId2.startsWith("HERO"))  emit playerBattlecryObjHeroPlayed();//Secreto Evasion
//                            else                            emit playerBattlecryObjMinionPlayed();//No se usa aun
                        }
                    }
                }

                //Jugador/enemigo causa accion con objetivo en PLAY
                else if(blockType == "POWER")
                {
                    if(isPlayer)    emit playerCardObjPlayed(cardId1, id1.toInt(), id2.toInt());
                    else            emit enemyCardObjPlayed(cardId1, id1.toInt(), id2.toInt());
                }

                //Jugador/Enemigo ataca (esbirro/heroe VS esbirro/heroe)
                else if(blockType == "ATTACK" && zone == "PLAY")
                {
                    emit zonePlayAttack(cardId1, id1.toInt(), id2.toInt());

                    if(cardId1.contains("HERO"))
                    {
                        if(cardId2.contains("HERO"))
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (heroe)vs(heroe) " + name2, numLine);
                            if(isPlayer && isPlayerTurn)    emit playerAttack(true, true, playerMinions);
                        }
                        else
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (heroe)vs(minion) " + name2, numLine);
                            /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                            {
                                emit pDebug("Saltamos comprobacion de secretos";
                            }
                            else */if(isPlayer && isPlayerTurn)    emit playerAttack(true, false, playerMinions);
                        }
                    }
                    else
                    {
                        if(cardId2.contains("HERO"))
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (minion)vs(heroe) " + name2, numLine);
                            if(isPlayer && isPlayerTurn)    emit playerAttack(false, true, playerMinions);
                        }
                        else
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (minion)vs(minion) " + name2, numLine);
                            /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                            {
                                emit pDebug("Saltamos comprobacion de secretos";
                            }
                            else */if(isPlayer && isPlayerTurn)    emit playerAttack(false, false, playerMinions);
                        }
                    }
                }
            }
        }
    }
}


void GameWatcher::processZone(QString &line, qint64 numLine)
{
    if(powerState == noGame)   return;

    //Carta desconocida
    //[entityName=UNKNOWN ENTITY [cardType=INVALID] id=69 zone=HAND zonePos=0 cardId= player=2] zone from OPPOSING DECK -> OPPOSING HAND
    if(line.contains(QRegularExpression(
        "\\[entityName=UNKNOWN ENTITY \\[cardType=INVALID\\] id=(\\d+) zone=\\w+ zonePos=\\d+ cardId= player=\\d+\\] zone from "
        "(\\w+ \\w+(?: \\(Weapon\\))?)? -> (\\w+ \\w+(?: \\((?:Weapon|Hero|Hero Power)\\))?)?"
        ), match))
    {
        QString id = match->captured(1);
        QString zoneFrom = match->captured(2);
        QString zoneTo = match->captured(3);


        //Enemigo juega secreto
        if(zoneTo == "OPPOSING SECRET")
        {
            emit pDebug("Enemy: Secret played. ID: " + id, numLine);
            emit enemySecretPlayed(id.toInt(), secretHero, loadingScreenState);
        }

        //Enemigo roba carta desconocida
        else if(zoneTo == "OPPOSING HAND")
        {
            if(mulliganEnemyDone)
            {
                //Enemigo roba carta de deck
                if(zoneFrom == "OPPOSING DECK")
                {
                    bool advance = advanceTurn(false);
                    emit pDebug("Enemy: Card drawn. ID: " + id, numLine);
                    if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal);
                    emit enemyCardDraw(id.toInt(), turnReal);
                    if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal);
                }
                //Enemigo roba carta especial del vacio
                else if(zoneFrom.isEmpty())
                {
                    emit pDebug("Enemy: Special card drawn. ID: " + id, numLine);
                    emit enemyCardDraw(id.toInt(), turnReal, true);
                }
            }
            else
            {
                //Enemigo roba starting card
                emit pDebug("Enemy: Starting card drawn. ID: " + id, numLine);
                emit enemyCardDraw(id.toInt());
            }
        }

        //Jugador, OUTSIDER desconocido a deck
        else if(zoneTo == "FRIENDLY DECK")
        {
            if(mulliganPlayerDone)
            {
                emit pDebug("Player: Outsider unknown card to deck. ID: " + id, numLine);
                emit playerReturnToDeck("", id.toInt());
            }
        }


        //Enemigo juega carta desconocida
        if(zoneFrom == "OPPOSING HAND")
        {
            //Carta devuelta al mazo en Mulligan
            if(zoneTo == "OPPOSING DECK")
            {
                emit pDebug("Enemy: Starting card returned. ID: " + id, numLine);
                emit enemyCardPlayed(id.toInt(), "", true);
            }
            else
            {
                emit pDebug("Enemy: Unknown card played. ID: " + id, numLine);
                emit enemyCardPlayed(id.toInt());
            }
        }

        //Jugador, elimina OUTSIDER desconocido de deck
        else if(zoneFrom == "FRIENDLY DECK")
        {
            emit pDebug("Player: Unknown card removed from deck. ID: " + id, numLine);
            emit playerCardDraw("", id.toInt());
        }
    }


    //Carta conocida
    //[entityName=Shellshifter id=32 zone=HAND zonePos=0 cardId=UNG_101 player=1] zone from FRIENDLY DECK -> FRIENDLY HAND
    //[entityName=Shellshifter id=32 zone=DECK zonePos=0 cardId= player=1] zone from FRIENDLY HAND -> FRIENDLY DECK
    else if(line.contains(QRegularExpression(
        "\\[entityName=(.*) id=(\\d+) zone=\\w+ zonePos=(\\d+) cardId=(\\w*) player=(\\d+)\\] zone from "
        "(\\w+ \\w+(?: \\(Weapon\\))?)? -> (\\w+ \\w+(?: \\((?:Weapon|Hero|Hero Power)\\))?)?"
        ), match))
    {
        QString name = match->captured(1);
        QString id = match->captured(2);
        QString zonePos = match->captured(3);
        QString cardId = match->captured(4);
        QString player = match->captured(5);
        QString zoneFrom = match->captured(6);
        QString zoneTo = match->captured(7);

        if(cardId.isEmpty())    cardId = Utility::cardLocalCodeFromName(name);


        //Jugador juega secreto
        if(zoneTo == "FRIENDLY SECRET" && zoneFrom != "OPPOSING SECRET")
        {
            emit pDebug("Player: Secret played: " + name + " ID: " + id, numLine);
            emit playerSecretPlayed(id.toInt(), cardId);
        }

        //Enemigo roba carta conocida
        else if(zoneTo == "OPPOSING HAND" && zoneFrom != "OPPOSING HAND" && mulliganEnemyDone)
        {
            bool advance = false;
            if(zoneFrom == "OPPOSING DECK")
            {
                emit pDebug("Enemy: Known card to hand from deck (Hidden to avoid cheating): " + name + " ID: " + id, numLine);
                advance = advanceTurn(false);
                //Bug HS, aun no deberiamos conocer esta carta, no queremos que vaya a enemy deck tab hasta que sea jugada
                //emit enemyKnownCardDraw(id.toInt(), cardId);

                if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal);
                //Bug HS, aun no deberiamos conocer esta carta, no queremos que se muestre en hand tab
                emit enemyCardDraw(id.toInt(), turnReal);
                if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal);
            }
            else
            {
                emit pDebug("Enemy: Known card to hand: " + name + " ID: " + id, numLine);
                emit enemyCardDraw(id.toInt(), turnReal, false, cardId);
            }
        }

        //Jugador roba carta conocida
        else if(zoneTo == "FRIENDLY HAND")
        {
            if(mulliganPlayerDone)//Evita que las cartas iniciales creen un nuevo Board en PlanHandler al ser robadas
            {
                emit pDebug("Player: Known card to hand: " + name + " ID: " + id, numLine);
                bool advance = false;
                if(zoneFrom == "FRIENDLY DECK")
                {
                    advance = advanceTurn(true);
                }
                if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal);
                emit playerCardToHand(id.toInt(), cardId, turnReal);
                if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal);
            }
            else
            {
                emit pDebug("Player: Starting card to hand: " + name + " ID: " + id, numLine);
                emit playerCardToHand(id.toInt(), cardId, 0);
            }
        }

        //Jugador, OUTSIDER a deck
        else if(zoneTo == "FRIENDLY DECK" && zoneFrom != "FRIENDLY DECK" && zoneFrom != "FRIENDLY HAND")
        {
            emit pDebug("Player: Outsider card to deck: " + name + " ID: " + id, numLine);
            emit playerReturnToDeck(cardId, id.toInt());
        }

        //Enemigo, nuevo minion en PLAY
        else if(zoneTo == "OPPOSING PLAY" && zoneFrom != "OPPOSING PLAY")
        {
            enemyMinions++;
            emit pDebug("Enemy: Minion moved to OPPOSING PLAY: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
            if(zoneFrom == "FRIENDLY PLAY") emit playerMinionZonePlaySteal(id.toInt(), zonePos.toInt());
            else if(zoneFrom.isEmpty())     emit enemyMinionZonePlayAddTriggered(cardId, id.toInt(), zonePos.toInt());
            else                            emit enemyMinionZonePlayAdd(cardId, id.toInt(), zonePos.toInt());
        }

        //Jugador, nuevo minion en PLAY
        else if(zoneTo == "FRIENDLY PLAY" && zoneFrom != "FRIENDLY PLAY")
        {
            playerMinions++;
            emit pDebug("Player: Minion moved to FRIENDLY PLAY: " + name + " ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
            if(zoneFrom == "OPPOSING PLAY") emit enemyMinionZonePlaySteal(id.toInt(), zonePos.toInt());
            else if(zoneFrom.isEmpty())     emit playerMinionZonePlayAddTriggered(cardId, id.toInt(), zonePos.toInt());
            else                            emit playerMinionZonePlayAdd(cardId, id.toInt(), zonePos.toInt());
        }

        //Enemigo, carga heroe
        else if(zoneTo == "OPPOSING PLAY (Hero)")
        {
            emit pDebug("Enemy: Hero moved to OPPOSING PLAY (Hero): " + name + " ID: " + id, numLine);
            emit enemyHeroZonePlayAdd(cardId, id.toInt());
        }

        //Jugador, carga heroe
        else if(zoneTo == "FRIENDLY PLAY (Hero)")
        {
            emit pDebug("Player: Hero moved to FRIENDLY PLAY (Hero): " + name + " ID: " + id, numLine);
            if(playerID == 0)
            {
                playerID = player.toInt();
                emit enemyHero((playerID == 1)?hero2:hero1);
                emit pDebug("Found playerID: " + player, numLine);

                secretHero = Utility::heroFromLogNumber((playerID == 1)?hero1:hero2);

                if(playerTag.isEmpty())
                {
                    playerTag = (playerID == 1)?name1:name2;
                    if(!playerTag.isEmpty())    emit pDebug("Found playerTag: " + playerTag, numLine);
                }
            }
            emit playerHeroZonePlayAdd(cardId, id.toInt());
        }

        //Enemigo, carga hero power
        else if(zoneTo == "OPPOSING PLAY (Hero Power)")
        {
            emit pDebug("Enemy: Hero Power moved to OPPOSING PLAY (Hero Power): " + name + " ID: " + id, numLine);
            emit enemyHeroPowerZonePlayAdd(cardId, id.toInt());
        }

        //Jugador, carga hero power
        else if(zoneTo == "FRIENDLY PLAY (Hero Power)")
        {
            emit pDebug("Player: Hero Power moved to FRIENDLY PLAY (Hero Power): " + name + " ID: " + id, numLine);
            emit playerHeroPowerZonePlayAdd(cardId, id.toInt());
        }

        //Enemigo, equipa arma
        else if(zoneTo == "OPPOSING PLAY (Weapon)" && zoneFrom != "OPPOSING GRAVEYARD")//Al reemplazar un arma por otra, la antigua va, vuelve y va a graveyard.
        {
            emit pDebug("Enemy: Weapon moved to OPPOSING PLAY (Weapon): " + name + " ID: " + id, numLine);
            emit enemyWeaponZonePlayAdd(cardId, id.toInt());
        }

        //Jugador, equipa arma
        else if(zoneTo == "FRIENDLY PLAY (Weapon)" && zoneFrom != "FRIENDLY GRAVEYARD")
        {
            emit pDebug("Player: Weapon moved to FRIENDLY PLAY (Weapon): " + name + " ID: " + id, numLine);
            emit playerWeaponZonePlayAdd(cardId, id.toInt());
        }




        if(zoneFrom == "FRIENDLY SECRET")
        {
            //Enemigo roba secreto (kezan mystic)
            if(zoneTo == "OPPOSING SECRET")
            {
                emit pDebug("Enemy: Secret stolen: " + name + " ID: " + id, numLine);
                emit enemySecretStolen(id.toInt(), cardId);
            }
            //Jugador secreto desvelado
            else
            {
                emit pDebug("Player: Secret revealed: " + name + " ID: " + id, numLine);
                emit playerSecretRevealed(id.toInt(), cardId);
            }
        }

        else if(zoneFrom == "OPPOSING SECRET")
        {
            //Jugador roba secreto (kezan mystic)
            if(zoneTo == "FRIENDLY SECRET")
            {
                emit pDebug("Player: Secret stolen: " + name + " ID: " + id, numLine);
                emit playerSecretStolen(id.toInt(), cardId);
            }
            //Enemigo secreto desvelado
            else if(zoneTo != "OPPOSING SECRET")
            {
                emit pDebug("Enemy: Secret revealed: " + name + " ID: " + id, numLine);
                emit enemySecretRevealed(id.toInt(), cardId);
            }
        }

        //Enemigo juega carta conocida
        else if(zoneFrom == "OPPOSING HAND" && zoneTo != "OPPOSING HAND")
        {
            bool discard = false;

            //Enemigo juega hechizo
            if(zoneTo.isEmpty() || zoneTo == "OPPOSING SECRET")
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
            //Enemigo juega heroe
            else if(zoneTo == "OPPOSING PLAY (Hero)")
            {
                emit pDebug("Enemy: Hero played: " + name + " ID: " + id, numLine);
            }
            //Enemigo descarta carta
            else if(zoneTo == "OPPOSING GRAVEYARD")
            {
                emit pDebug("Enemy: Card discarded: " + name + " ID: " + id, numLine);
                discard = true;
            }
            //Carta devuelta al mazo en Mulligan, Leviatan de llamas
            else if(zoneTo == "OPPOSING DECK")
            {
                emit pDebug("Enemy: Starting card returned: " + name + " ID: " + id, numLine);
                discard = true;
            }
            else
            {
                emit pDebug("Enemy: Card moved from hand: " + name + " ID: " + id, numLine, Warning);
                discard = true;
            }

            emit enemyCardPlayed(id.toInt(), cardId, discard);
        }

        //Enemigo roba carta overdraw/recruit
        else if(zoneFrom == "OPPOSING DECK" && (zoneTo == "OPPOSING GRAVEYARD" || zoneTo == "OPPOSING PLAY"))
        {
            bool advance = advanceTurn(false);
            emit pDebug("Enemy: Card from deck skipped hand (overdraw/recruit): " + name + " ID: " + id, numLine);
            emit enemyKnownCardDraw(id.toInt(), cardId);
            if(advance)     emit newTurn(isPlayerTurn, turnReal);
        }

        //Jugador roba carta conocida
        //Maldición ancestral hace un FRIENDLY DECK --> EMPTY justo despues de meterse en el mazo. CONTRA
        //Joust mechanics mueven la carta mostrada a EMPTY. CONTRA. En deckHandler se evita que la carta se incluya en el mazo (al ser un outsider)
        //Dragon's Fury se mete como outsider en el mazo EMPTY --> FRIENDLY DECK y sale despues de hacer el joust FRIENDLY DECK --> EMPTY. Puesto que AT lo mete en el deck es necesario sacarlo. PRO
        //El mono de Elise vacia el deck a empty y lo rellena de legendarias. PRO
        //Explorar un'goro vacia el deck a empty y lo rellena de explorar un'goro. PRO
        else if(zoneFrom == "FRIENDLY DECK" && zoneTo != "FRIENDLY DECK")
        {
            emit pDebug("Player: Card drawn: " + name + " ID: " + id, numLine);
            //El avance de turno ocurre generalmente en (zoneTo == "FRIENDLY HAND") pero en el caso de overdraw ocurrira aqui.
            if(mulliganPlayerDone)//Evita que las cartas iniciales creen un nuevo Board en PlanHandler al ser robadas
            {
                bool advance = advanceTurn(true);
                if(advance)     emit newTurn(isPlayerTurn, turnReal);
            }
            emit playerCardDraw(cardId, id.toInt());
        }

        //Jugador juega carta conocida
        else if(zoneFrom == "FRIENDLY HAND")
        {
            bool discard = false;

            //Jugador juega hechizo
            if(zoneTo.isEmpty() || zoneTo == "FRIENDLY SECRET")//En spectator mode los secretos van FRIENDLY HAND --> FRIENDLY SECRET
            {
                emit pDebug("Player: Spell played: " + name + " ID: " + id, numLine);
                if(isPlayerTurn)    emit playerSpellPlayed(cardId);
            }
            //Jugador juega esbirro
            else if(zoneTo == "FRIENDLY PLAY")
            {
                emit pDebug("Player: Minion played: " + name + " ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
                if(isPlayerTurn)    emit playerMinionPlayed(cardId, playerMinions);
            }
            //Jugador juega arma
            else if(zoneTo == "FRIENDLY PLAY (Weapon)")
            {
                emit pDebug("Player: Weapon played: " + name + " ID: " + id, numLine);
            }
            //Jugador juega heroe
            else if(zoneTo == "FRIENDLY PLAY (Hero)")
            {
                emit pDebug("Player: Hero played: " + name + " ID: " + id, numLine);
            }
            //Jugador descarta carta
            else if(zoneTo == "FRIENDLY GRAVEYARD")
            {
                emit pDebug("Player: Card discarded: " + name + " ID: " + id, numLine);
                discard = true;
            }
            //Carta devuelta al mazo en Mulligan
            //(Quizas sea util verificar que estamos en el turno 1 o puede dar problemas en el futuro)
            else if(zoneTo == "FRIENDLY DECK")
            {
                emit pDebug("Player: Starting card returned: " + name + " ID: " + id, numLine);
                emit playerReturnToDeck(cardId, id.toInt());
                discard = true;
            }
            else
            {
                emit pDebug("Player: Card moved from hand: " + name + " ID: " + id, numLine, Warning);
                discard = true;
            }

            emit playerCardPlayed(id.toInt(), cardId, discard);

            if(isPlayerTurn && !discard)
            {
                //Rat trap/Hidden wisdom control
                playerCardsPlayedThisTurn++;
                if(playerCardsPlayedThisTurn > 2)
                {
                    emit pDebug("Rat trap/Hidden wisdom tested: This turn cards played: " + QString::number(playerCardsPlayedThisTurn), 0);
                    emit _3CardsPlayedTested();
                }
            }
        }

        //Enemigo esbirro muere
        else if(zoneFrom == "OPPOSING PLAY" && zoneTo != "OPPOSING PLAY")
        {
            if(enemyMinions>0)  enemyMinions--;
            emit pDebug("Enemy: Minion removed from OPPOSING PLAY: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
            if(zoneTo != "FRIENDLY PLAY")   emit enemyMinionZonePlayRemove(id.toInt());

            if(zoneTo == "OPPOSING GRAVEYARD")
            {
                emit enemyMinionGraveyard(id.toInt(), cardId, isPlayerTurn);
                if(isPlayerTurn)
                {
                    //Avenge control
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

                    //Hand of salvation control
                    enemyMinionsDeadThisTurn++;
                    if(enemyMinionsDeadThisTurn > 1)
                    {
                        if(cardId == MAD_SCIENTIST)
                        {
                            emit pDebug("Skip Hand of salvation testing (Mad Scientist died).", 0);
                        }
                        else
                        {
                            emit pDebug("Hand of salvation tested: This turn died: " + QString::number(enemyMinionsDeadThisTurn), 0);
                            emit handOfSalvationTested();
                        }
                    }
                }
            }
        }

        //Jugador esbirro muere
        else if(zoneFrom == "FRIENDLY PLAY" && zoneTo != "FRIENDLY PLAY")
        {
            if(playerMinions>0) playerMinions--;
            emit pDebug("Player: Minion removed from FRIENDLY PLAY: " + name + " ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
            if(zoneTo != "OPPOSING PLAY")   emit playerMinionZonePlayRemove(id.toInt());

            if(zoneTo == "FRIENDLY GRAVEYARD")  emit playerMinionGraveyard(id.toInt(), cardId);
        }

        //Enemigo, deshecha arma
        else if(zoneFrom == "OPPOSING PLAY (Weapon)")
        {
            emit pDebug("Enemy: Weapon moved from OPPOSING PLAY (Weapon): " + name + " ID: " + id, numLine);
            emit enemyWeaponZonePlayRemove(id.toInt());

            if(zoneTo == "OPPOSING GRAVEYARD")  emit enemyWeaponGraveyard(id.toInt(), cardId);
        }

        //Jugador, deshecha arma
        else if(zoneFrom == "FRIENDLY PLAY (Weapon)")
        {
            emit pDebug("Player: Weapon moved from FRIENDLY PLAY (Weapon): " + name + " ID: " + id, numLine);
            emit playerWeaponZonePlayRemove(id.toInt());

            if(zoneTo == "FRIENDLY GRAVEYARD")  emit playerWeaponGraveyard(id.toInt(), cardId);
        }
    }


    //Jugador/Enemigo esbirro cambia pos
    //No podemos usar zonePos= porque para los esbirros del jugador que pasan a una posicion mayor muestra su posicion origen
    //Toddo comentado porque esta forma de contar el numero de esbirros puede producir errores.
    //Ej: Si un esbirro con deathrattle produce otro esbirro. Primero se cambia la pos de los esbirros a la dcha
    //y despues se genran los esbirros de deathrattle causando una suma erronea.
    //id=7 local=True [entityName=Ingeniera novata id=25 zone=HAND zonePos=5 cardId=EX1_015 player=1] pos from 5 -> 3
    else if(line.contains(QRegularExpression(
        "\\[entityName=(.*) id=(\\d+) zone=(?:HAND|PLAY) zonePos=\\d+ cardId=\\w+ player=(\\d+)\\] pos from \\d+ -> (\\d+)"
        ), match))
    {
        QString name = match->captured(1);
        QString id = match->captured(2);
        QString player = match->captured(3);
        QString zonePos = match->captured(4);
        int zonePosInt = zonePos.toInt();

        if(zonePosInt>0)//En patch 8.2.0 los esbirros se mueven siempre a 0 y luego a su posicion
        {
            //Jugador esbirro cambia pos
            if(player.toInt() == playerID)
            {
    //            emit pDebug("Player: New minion pos: " +
    //                        name + " >> " + zonePos + " Minions: " + QString::number(playerMinions), numLine);
                emit playerMinionPosChange(id.toInt(), zonePosInt);
            }
            //Enemigo esbirro cambia pos
            else
            {
    //            emit pDebug("Enemy: New minion pos: " +
    //                        name + " >> " + zonePos + " Minions: " + QString::number(enemyMinions), numLine);
                emit enemyMinionPosChange(id.toInt(), zonePosInt);
            }
        }
    }
}


bool GameWatcher::isHeroPower(QString code)
{
    return (Utility::getTypeFromCode(code) == HERO_POWER);
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
        emit pDebug("CreateGameResult: PlayerID wasn't defined in the game.", 0, DebugLevel::Error);
        return;
    }

    gameResult.isFirst = (firstPlayer == playerTag);
    gameResult.isWinner = (winnerPlayer == playerTag);

    emit newGameResult(gameResult, loadingScreenState, logFileName, startGameEpoch);
}


QString GameWatcher::createGameLog()
{
    if(!copyGameLogs)
    {
        emit pDebug("Game log copy disabled.", 0);
        return "";
    }
    if(logSeekCreate == -1)
    {
        emit pDebug("Cannot create match log. Not found CREATE_GAME", 0);
        return "";
    }

    if(logSeekWon == -1)
    {
        emit pDebug("Cannot create match log. Not found WON ", 0);
        return "";
    }

    QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh-mm");
    QString win = (winnerPlayer == playerTag)?"WIN":"LOSE";
    QString coin = (firstPlayer == playerTag)?"FIRST":"COIN";
    QString gameMode = Utility::getLoadingScreenToString(loadingScreenState);
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
    QString fileName = gameMode + " " + timeStamp + " " + playerHero + "vs" + enemyHero + " " + win + " " + coin + ".arenatracker";


    emit pDebug("Game log ready to be copied.", 0);
    emit gameLogComplete(logSeekCreate, logSeekWon, fileName);
    logSeekCreate = -1;
    logSeekWon = -1;

    return fileName;
}


bool GameWatcher::advanceTurn(bool playerDraw)
{
    if(turnReal == turn)    return false;

    //Al jugar contra la maquina puede que se lea antes el fin de turno que el robo del inicio del turno
    //if(turn > turnReal+1)   turn = turnReal+1;

    bool playerTurn;
    if((firstPlayer==playerTag && turn%2==1) || (firstPlayer!=playerTag && turn%2==0))  playerTurn=true;
    else    playerTurn=false;

    if(firstPlayer.isEmpty() || playerTag.isEmpty())    playerTurn = playerDraw;

    //Al turno 1 dejamos que pase cualquiera asi dejamos el turno 0 para indicar cartas de mulligan
    //Solo avanza de turno al robar carta el jugador que le corresponde
    bool advance = (turn == 1 || playerDraw == playerTurn || playerID == 0);
    if(advance)
    {
        turnReal = turn;
        emit pDebug("\nTurn: " + QString::number(turn) + " " + (playerTurn?"Player":"Enemy"), 0);

        isPlayerTurn = playerTurn;

        if(playerDraw)      emit clearDrawList();

        emit specialCardTrigger("", "", -1, -1);    //Evita Cartas createdBy en las cartas recien robadas al empezar el turno

        //Secret CSpirit test
        if(!isPlayerTurn && enemyMinions > 0)
        {
            emit pDebug("CSpirit tested. Minions: " + QString::number(enemyMinions), 0);
            emit cSpiritTested();
        }

        enemyMinionsDeadThisTurn = 0; //Hand of salvation testing
        playerCardsPlayedThisTurn = 0; //Rat trap/Hidden wisdom testing
    }
    return advance;
}


LoadingScreenState GameWatcher::getLoadingScreen()
{
    return this->loadingScreenState;
}


void GameWatcher::setCopyGameLogs(bool value)
{
    this->copyGameLogs = value;
}


