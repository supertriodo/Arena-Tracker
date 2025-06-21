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
    tied = true;
    emit pDebug("Reset (powerState = noGame).", 0);
    emit pDebug("Reset (LoadingScreen = menu).", 0);
}


void GameWatcher::processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    Q_UNUSED(logSeek);

    switch(logComponent)
    {
        case logPower:
            processPower(line, numLine);
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
    emit arenaDeckRead();   //completeArenaDeck with draft file
    emit pDebug("End reading deck (arenaState = deckRead).", 0);
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
        if(prevMode == "GAMEPLAY")
        {
            if(spectating || loadingScreenState == menu || tied)
            {
                emit pDebug("CreateGameResult: Avoid spectator/tied game result.", 0);
            }
            else
            {
                createGameResult();
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
        else if(currMode == "GAME_MODE")
        {
            loadingScreenState = gameMode;
            emit pDebug("Entering GAME_MODE (loadingScreenState = gameMode).", numLine);
        }
        else if(currMode == "TOURNAMENT")
        {
            loadingScreenState = ranked;
            emit pDebug("Entering CASUAL/RANKED (loadingScreenState = ranked).", numLine);
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
    Q_UNUSED(line);
    Q_UNUSED(numLine);

//    if(powerState != noGame)   return;

//    //Definimos RANKED solo si venimos de loadScreen TOURNAMENT y
//    //acabamos de encontrar el WON pero aun no hemos createResult
//    if(loadingScreenState == casual && logSeekWon != -1 && line.contains("assetPath=rank_window"))
//    {
//        loadingScreenState = ranked;
//        emit pDebug("On RANKED (loadingScreenState = ranked).", numLine);
//    }
}


//Usar ticket arena/Comprar arena
//D 11:27:13.1732560 DraftManager.OnBegin - Got new draft deck with ID: 507495951
//D 11:27:13.1736650 SetDraftMode - DRAFTING
//Volver arena eleccion heroes ya comprada
//D 11:27:47.0196860 DraftManager.OnChoicesAndContents - Draft Deck ID: 507495951, Hero Card =
//D 11:27:47.0197460 SetDraftMode - DRAFTING
void GameWatcher::processArena(QString &line, qint64 numLine)
{
    //NEW ARENA - START DRAFT
    //[Arena] DraftManager.OnChosen(): hero=HERO_02 premium=STANDARD
    if(line.contains(QRegularExpression("DraftManager\\.OnChosen\\(\\): hero=HERO_(\\d+)"), match))
    {
        QString hero = match->captured(1);
        emit pDebug("New arena. Heroe: " + hero, numLine);
        emit newArena(hero); //(connect)Begin draft //(connect)resetDeckDontRead (arenaState = deckRead)
    }
    //DRAFTING PICK CARD
    //[Arena] Client chooses: Profesora violeta (NEW1_026)
    else if(line.contains(QRegularExpression("Client chooses: .* \\((\\w+)\\)"), match))
    {
        QString code = match->captured(1);
        emit pDebug("Pick card: " + code, numLine);
        emit pickCard(code);
    }
    //START READING DECK
    //[Arena] DraftManager.OnChoicesAndContents - Draft Deck ID: 472720132, Hero Card = HERO_02
    else if(line.contains(QRegularExpression(
                "DraftManager\\.OnChoicesAndContents - Draft Deck ID: \\d+, Hero Card = HERO_(\\d+)"), match))
    {
        QString hero = match->captured(1);
        emit pDebug("Found Hero Draft Deck. Heroe: " + hero, numLine);
        startReadingDeck();
        emit heroDraftDeck(hero);
    }
    //END READING DECK
    //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
    else if(line.contains("SetDraftMode - ACTIVE_DRAFT_DECK"))
    {
        emit pDebug("Found ACTIVE_DRAFT_DECK.", numLine);
        endReadingDeck();//completeArenaDeck with draft file
        emit activeDraftDeck(); //(connect)End draft/Show mechanics, debe estar detras de endReadingDeck
        //para primero completar el deck y luego mostrar la mechanics window del deck completo
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
    //COMPRAR ARENA -- VUELTA A SELECCION HEROE
    else if(line.contains(QRegularExpression(
                "DraftManager\\.OnBegin - Got new draft deck with ID: \\d+"), match) ||
            line.contains(QRegularExpression(
                            "DraftManager\\.OnChoicesAndContents - Draft Deck ID: \\d+, Hero Card ="), match))
    {
        emit pDebug("New arena: choosing heroe.", numLine);
        emit heroDraftDeck();//No hero
        emit arenaChoosingHeroe();
    }
    //SetDraftMode - DRAFTING
    else if(line.contains("SetDraftMode - DRAFTING"))
    {
        emit pDebug("Found SetDraftMode - DRAFTING.", numLine);
        emit continueDraft();   //(connect) continueDraft
    }
    //SetDraftMode - IN_REWARDS
    else if(line.contains("SetDraftMode - IN_REWARDS"))
    {
        emit pDebug("Found SetDraftMode - IN_REWARDS.", numLine);
        emit heroDraftDeck();//No hero
        emit inRewards();   //Remove mechanics window
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

void GameWatcher::processPower(QString &line, qint64 numLine)
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
                emit clearDrawList();
                emit pDebug("WARNING: Heroes/Players info missing (powerState = inGameState, mulliganDone = true)", 0, Warning);
            }
        }
    }
    switch(powerState)
    {
        case noGame:
            break;
        case heroType1State:
        case heroPower1State:
        case heroType2State:
            processPowerHero(line, numLine);
            break;
        case mulliganState:
            processPowerMulligan(line, numLine);
            break;
        case inGameState:
            processPowerInGame(line, numLine);
            break;
    }
}

void GameWatcher::processPowerHero(QString &line, qint64 numLine)
{
    if(powerState == heroPower1State)
    {
        if(line.contains(QRegularExpression("Creating ID=\\d+ CardID=(\\w+)"), match))
        {
            powerState = heroType2State;
            QString hp1 = match->captured(1);
            emit pDebug("Skip hero power 1: " + hp1 + " (powerState = heroType2State)", numLine);
        }
    }
    else// powerState == heroType1State || powerState == heroType2State
    {
        if(line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
        {
            if(powerState == heroType1State)
            {
                powerState = heroPower1State;
                hero1 = match->captured(1);
                emit pDebug("Found hero 1: " + hero1 + " (powerState = heroPower1State)", numLine);
            }
            else //if(powerState == heroType2State
            {
                powerState = mulliganState;
                hero2 = match->captured(1);
                emit pDebug("Found hero 2: " + hero2 + " (powerState = mulliganState)", numLine);
            }
        }
    }
}

void GameWatcher::processPowerMulligan(QString &line, qint64 numLine)
{
    //Jugador/Enemigo names, playerTag y firstPlayer
    //GameState.DebugPrintEntityChoices() - id=1 Player=UNKNOWN HUMAN PLAYER TaskList= ChoiceType=MULLIGAN CountMin=0 CountMax=3
    //GameState.DebugPrintEntityChoices() - id=2 Player=triodo#2541 TaskList= ChoiceType=MULLIGAN CountMin=0 CountMax=5
    if(line.contains(QRegularExpression(
                "GameState\\.DebugPrintEntityChoices\\(\\) - id=(\\d+) Player=(.*) TaskList=\\d* ChoiceType=MULLIGAN CountMin=0 CountMax=(\\d+)"
                  ), match))
    {
        QString player = match->captured(1);
        QString playerName = match->captured(2);
        QString numCards = match->captured(3);

        if(player.toInt() == 1)
        {
            emit pDebug("Found player 1: " + playerName, numLine);
            if(playerName != "UNKNOWN HUMAN PLAYER")
            {
                name1 = playerName;
                emitEnemyName();
            }
        }
        else if(player.toInt() == 2)
        {
            emit pDebug("Found player 2: " + playerName, numLine);
            if(playerName != "UNKNOWN HUMAN PLAYER")
            {
                name2 = playerName;
                emitEnemyName();
            }
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
        QString entityName = match->captured(1);
        //Player mulligan
        if(entityName == playerTag)
        {
            if(!mulliganPlayerDone)
            {
                emit pDebug("Player mulligan end.", numLine);
                mulliganPlayerDone = true;
                turn = 1;
                emit clearDrawList();

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

                //Revisamos Enemy name, por si cogio "UNKNOWN HUMAN PLAYER" al inicio, en cuyo caso name1/name2 = ""
                QString enemyName = (playerID == 1)?name2:name1;
                if(enemyName.isEmpty() && !entityName.isEmpty())
                {
                    if(playerID == 1)   name2 = entityName;
                    else                name1 = entityName;
                    emitEnemyName();
                }

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

        if  (tag == "ATK" || tag == "HEALTH" ||
            (lastShowEntity.trackAllTags && (tag == "DAMAGE" || tag == "DORMANT"))
            )
        {
            emit pDebug((lastShowEntity.isPlayer?QString("Player"):QString("Enemy")) + ": SHOW_TAG(" + tag + ")= " + value, numLine);
            if(lastShowEntity.id == -1)         emit pDebug("Show entity id missing.", numLine, DebugLevel::Error);
            else if(lastShowEntity.isPlayer)    emit playerBoardTagChange(lastShowEntity.id, "", tag, value);
            else                                emit enemyBoardTagChange(lastShowEntity.id, "", tag, value);
        }
        //En un futuro quizas haya que distinguir entre cambios en zone HAND o PLAY, por ahora son siempre cambios en PLAY

        //Tag avanzados para CHANGE_ENTITY - Updating Entity=
        //No son necesarios ya que al hacer el update entity emit minionCodeChange que cambiara
        //el codigo del minion leyendo del json todos sus atributos correctos.
//                    || (lastShowEntity.trackAllTags && (tag == "DAMAGE" || /*tag == "EXHAUSTED" ||*/
//                     tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" || tag == "RUSH" ||
//                     tag == "FROZEN" || tag == "WINDFURY" || tag == "AURA"))
    }
    else
    {
        //Reiniciamos lastShowEntity
        lastShowEntity.id = -1;

        //TAG_CHANGE jugadores conocido
        //D 10:48:46.1127070 PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=SerKolobok tag=RESOURCES value=3
        //D 20:08:22.6854340 PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=The Innkeeper tag=OVERLOAD_OWED value=2
        if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
            "Entity=(.*) tag=(\\w+) value=(\\w+)"
            ), match))
        {
            QString name = match->captured(1);
            QString tag = match->captured(2);
            QString value = match->captured(3);
            bool isPlayer = (name == playerTag);

            if(tag == "RESOURCES" || tag == "RESOURCES_USED" || tag == "CORPSES" || tag == "CORPSES_SPENT_THIS_GAME" ||
                tag == "CURRENT_SPELLPOWER" || tag == "OVERLOAD_OWED")
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
        //TAG_CHANGE Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=37 zone=HAND zonePos=6 cardId= player=2] tag=FORGE_REVEALED value=1
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
            else if(tag == "FORGE_REVEALED" && value.toInt() == 1)
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": TAG_CHANGE(" + tag + ")= " + value +
                                " -- Id: " + id, numLine);
                emit forgeHandCard(id.toInt());
            }
            else if(tag == "DAMAGE" || tag == "ATK" || tag == "HEALTH" || tag == "EXHAUSTED" ||
                    tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" || tag == "RUSH" ||
                    tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" ||
                    tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "AURA" ||
                    tag == "CANT_BE_DAMAGED" || tag == "SHOULDEXITCOMBAT" || tag == "ZONE" ||
                    tag == "LINKED_ENTITY" || tag == "DURABILITY" ||
                    tag == "COST" || tag == "REBORN" || tag == "DORMANT")
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
                    tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" || tag == "RUSH" ||
                    tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY" || tag == "SILENCED" ||
                    tag == "CONTROLLER" || tag == "TO_BE_DESTROYED" || tag == "AURA" ||
                    tag == "CANT_BE_DAMAGED" || tag == "SHOULDEXITCOMBAT" || tag == "ZONE" ||
                    tag == "LINKED_ENTITY" || tag == "DURABILITY" ||
                    tag == "COST" || tag == "REBORN" || tag == "DORMANT")
            {
                emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": MINION/CARD TAG_CHANGE(" + tag + ")=" + value +
                            " -- " + name + " -- Id: " + id, numLine);
                if(isPlayer)    emit playerBoardTagChange(id.toInt(), cardId, tag, value);
                else            emit enemyBoardTagChange(id.toInt(), cardId, tag, value);
            }
        }


        //Enemigo accion desconocida
        //BLOCK_START BlockType=PLAY Entity=[entityName=UNKNOWN ENTITY [cardType=INVALID] id=29 zone=HAND zonePos=2 cardId= player=1]
        //EffectCardId=System.Collections.Generic.List`1[System.String] EffectIndex=0 Target=0 SubOption=-1
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
            "Entity=\\[entityName=UNKNOWN ENTITY \\[cardType=INVALID\\] id=(\\d+) zone=\\w+ zonePos=\\d+ cardId= player=\\d+\\]"
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
            lastShowEntity.trackAllTags = false;
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
            lastShowEntity.trackAllTags = false;
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
            lastShowEntity.trackAllTags = true;
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
            lastShowEntity.trackAllTags = true;

            if(zone == "HAND")
            {
                if(isPlayer)    emit playerCardCodeChange(id.toInt(), newCardId);
            }
            else if(zone == "PLAY")
            {
                emit minionCodeChange(isPlayer, id.toInt(), newCardId);
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
            "EffectCardId=.* EffectIndex=-?\\d+ "
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
                    if(advanceTurn(isPlayer))       emit newTurn(isPlayerTurn, turnReal, enemyMinions);
                }
            }

            //Accion con objetivo en PLAY
            else if(zone2 == "PLAY")
            {
                //Jugador juega carta con objetivo en PLAY, No enemigo pq BlockType=PLAY es de entity desconocida para el enemigo
                if(blockType == "PLAY" && zone == "HAND")
                {
                    if(Utility::getTypeFromCode(cardId1) == SPELL)
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Spell obj played: " +
                                    name1 + " on target " + name2, numLine);
                        /*if(Utility::codeEqConstant(cardId2, MAD_SCIENTIST))
                        {
                            emit pDebug("Skip spell obj testing (Mad Scientist died).", 0);
                        }
                        else */if(isPlayer && isPlayerTurn)
                        {
                            if(Utility::isAHero(cardId2))   emit playerSpellObjHeroPlayed();
                            else                            emit playerSpellObjMinionPlayed();
                        }
                    }
                    else
                    {
                        emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Minion/weapon obj played: " +
                                    name1 + " target " + name2, numLine);
                        if(isPlayer && isPlayerTurn)
                        {
                            if(Utility::isAHero(cardId2))   emit playerBattlecryObjHeroPlayed();//Secreto Evasion
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

                    if(Utility::isAHero(cardId1))
                    {
                        if(Utility::isAHero(cardId2))
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (heroe)vs(heroe) " + name2, numLine);
                            if(isPlayer && isPlayerTurn)    emit playerAttack(true, true, playerMinions, id1.toInt(), id2.toInt());
                        }
                        else
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (heroe)vs(minion) " + name2, numLine);
                            /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                            {
                                emit pDebug("Saltamos comprobacion de secretos";
                            }
                            else */if(isPlayer && isPlayerTurn)    emit playerAttack(true, false, playerMinions, id1.toInt(), id2.toInt());
                        }
                    }
                    else
                    {
                        if(Utility::isAHero(cardId2))
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (minion)vs(heroe) " + name2, numLine);
                            if(isPlayer && isPlayerTurn)    emit playerAttack(false, true, playerMinions, id1.toInt(), id2.toInt());
                        }
                        else
                        {
                            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Attack: " +
                                        name1 + " (minion)vs(minion) " + name2, numLine);
                            /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                            {
                                emit pDebug("Saltamos comprobacion de secretos";
                            }
                            else */if(isPlayer && isPlayerTurn)    emit playerAttack(false, false, playerMinions, id1.toInt(), id2.toInt());
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
                    if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
                    emit enemyCardDraw(id.toInt(), turnReal);
                    if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
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

        //Jugador envia carta desconocida a deck
        else if(zoneTo == "FRIENDLY DECK")
        {
            //Carta devuelta al mazo en Mulligan
            if(zoneFrom == "FRIENDLY HAND")
            {
                emit pDebug("Player: Starting card returned. ID: " + id, numLine);
                emit playerReturnToDeck("", id.toInt());
                emit playerCardPlayed(id.toInt(), "", true, isPlayerTurn);
            }
            //Jugador, OUTSIDER desconocido a deck
            else
            {
                if(mulliganPlayerDone)
                {
                    emit pDebug("Player: Outsider unknown card to deck. ID: " + id, numLine);
                    emit playerReturnToDeck("", id.toInt());
                }
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

        //Enemigo esbirro muere
        else if(zoneFrom == "OPPOSING PLAY" && zoneTo != "OPPOSING PLAY")
        {
            if(enemyMinions>0)  enemyMinions--;
            emit pDebug("Enemy: Minion unknown removed from OPPOSING PLAY. ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
            if(zoneTo != "FRIENDLY PLAY")   emit enemyMinionZonePlayRemove(id.toInt());
        }

        //Jugador esbirro muere
        else if(zoneFrom == "FRIENDLY PLAY" && zoneTo != "FRIENDLY PLAY")
        {
            if(playerMinions>0) playerMinions--;
            emit pDebug("Player: Minion unknown removed from FRIENDLY PLAY. ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
            if(zoneTo != "OPPOSING PLAY")   emit playerMinionZonePlayRemove(id.toInt());
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
        else if(zoneTo == "OPPOSING HAND" && zoneFrom != "OPPOSING HAND")
        {
            if(mulliganEnemyDone)
            {
                //Mostramos todas las cartas marcadas como desveladas en el log y establecemos excepciones (isCheatingCard)
                bool advance = false;
                bool cheatingCard = isCheatingCard(zoneFrom);
                if(cheatingCard)    emit pDebug("Enemy: Known card to hand from deck (Hidden to avoid cheating): " + name + " ID: " + id, numLine);
                else                emit pDebug("Enemy: Known card to hand: " + name + " ID: " + id, numLine);
                if(zoneFrom == "OPPOSING DECK")
                {
                    advance = advanceTurn(false);
//                    if(!cheatingCard)   emit enemyKnownCardDraw(id.toInt(), cardId);
                }
                if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
                if(cheatingCard)                emit enemyCardDraw(id.toInt(), turnReal);
                else                            emit enemyCardDraw(id.toInt(), turnReal, false, cardId);
                if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
            }
            else
            {
                //Enemigo roba starting card
                emit pDebug("Enemy: Starting card drawn (Hidden to avoid cheating). ID: " + id, numLine);
                emit enemyCardDraw(id.toInt());
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
                if(advance && turnReal==1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
                emit playerCardToHand(id.toInt(), cardId, turnReal);
                if(advance && turnReal!=1)      emit newTurn(isPlayerTurn, turnReal, enemyMinions);
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
                emit pDebug("Found playerID: " + player, numLine);
                playerID = player.toInt();

                if(playerTag.isEmpty())
                {
                    playerTag = (playerID == 1)?name1:name2;
                    if(!playerTag.isEmpty())    emit pDebug("Found playerTag: " + playerTag, numLine);
                }

                emit enemyHero((playerID == 1)?hero2:hero1);
                emitEnemyName();

                secretHero = Utility::classLogNumber2classEnum((playerID == 1)?hero1:hero2);
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
                emit enemySecretStolen(id.toInt(), cardId, loadingScreenState);
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
                if(!isPlayerTurn)    emit enemySpellPlayed(id.toInt(), cardId);
            }
            //Enemigo juega esbirro
            else if(zoneTo == "OPPOSING PLAY")
            {
                if(Utility::getTypeFromCode(cardId) == MINION)//Evita locations
                {
                    emit pDebug("Enemy: Minion played: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
                }
                else
                {
                    emit pDebug("Enemy: Non minion played on board: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
                }
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
            if(advance)     emit newTurn(isPlayerTurn, turnReal, enemyMinions);
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
                if(advance)     emit newTurn(isPlayerTurn, turnReal, enemyMinions);
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
                if(isPlayerTurn)    emit playerSpellPlayed(id.toInt(), cardId);
            }
            //Jugador juega esbirro/location
            else if(zoneTo == "FRIENDLY PLAY")
            {
                if(Utility::getTypeFromCode(cardId) == MINION)//Evita locations
                {
                    emit pDebug("Player: Minion played: " + name + " ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
                    if(isPlayerTurn)    emit playerMinionPlayed(cardId, id.toInt(), playerMinions);
                }
                else
                {
                    emit pDebug("Player: Non minion played on board: " + name + " ID: " + id + " Minions: " + QString::number(playerMinions), numLine);
                }
            }
            //Jugador juega arma
            else if(zoneTo == "FRIENDLY PLAY (Weapon)")
            {
                emit pDebug("Player: Weapon played: " + name + " ID: " + id, numLine);
                if(isPlayerTurn)    emit playerWeaponPlayed(id.toInt(), cardId);
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

            emit playerCardPlayed(id.toInt(), cardId, discard, isPlayerTurn);
        }

        //Enemigo esbirro muere
        else if(zoneFrom == "OPPOSING PLAY" && zoneTo != "OPPOSING PLAY")
        {
            if(enemyMinions>0)  enemyMinions--;
            emit pDebug("Enemy: Minion removed from OPPOSING PLAY: " + name + " ID: " + id + " Minions: " + QString::number(enemyMinions), numLine);
            if(zoneTo != "FRIENDLY PLAY")   emit enemyMinionZonePlayRemove(id.toInt());

            if(zoneTo == "OPPOSING GRAVEYARD")
            {
                emit enemyMinionGraveyard(id.toInt(), cardId, isPlayerTurn, enemyMinions);
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
                emit pDebug("Player: New minion pos: " +
                            name + " >> " + zonePos + " Minions: " + QString::number(playerMinions), numLine);
                emit playerMinionPosChange(id.toInt(), zonePosInt);
            }
            //Enemigo esbirro cambia pos
            else
            {
                emit pDebug("Enemy: New minion pos: " +
                            name + " >> " + zonePos + " Minions: " + QString::number(enemyMinions), numLine);
                emit enemyMinionPosChange(id.toInt(), zonePosInt);
            }
        }
        Q_UNUSED(name);
    }
}


bool GameWatcher::isHeroPower(QString code)
{
    return (Utility::getTypeFromCode(code) == HERO_POWER);
}


void GameWatcher::createGameResult()
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

    emit newGameResult(gameResult, loadingScreenState);

    //Save player tag
    QString playerTagPreSharp = getNamePreSharp(playerTag);
    emit pDebug("Save playerName: " + playerTagPreSharp + "(" + playerTag + ")", 0);
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("playerName", playerTagPreSharp);
}


QString GameWatcher::getNamePreSharp(QString name)
{
    return name.split("#").first();
}


void GameWatcher::emitEnemyName()
{
    if(playerID != 0 && !name1.isEmpty() && !name2.isEmpty())
    {
        QString enemyTag = (playerID == 1)?name2:name1;
        emit pDebug("Found enemyTag: " + enemyTag, 0);
        emit enemyName(getNamePreSharp(enemyTag));
    }
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
    }
    return advance;
}


LoadingScreenState GameWatcher::getLoadingScreen()
{
    return this->loadingScreenState;
}


bool GameWatcher::isCheatingCard(const QString &zoneFrom)
{
    if(zoneFrom == "FRIENDLY HAND")             return false;
    if(zoneFrom.startsWith("OPPOSING PLAY"))    return false;
    if(zoneFrom.startsWith("FRIENDLY PLAY"))    return false;
    if(zoneFrom == "FRIENDLY SECRET")           return false;
    return true;
}


