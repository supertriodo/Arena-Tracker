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
            secretHero = unknown;
            playerMinions = 0;
            enemyMinions = 0;
            enemyMinionsAliveForAvenge = -1;

            emit specialCardTrigger("", "", -1);    //Evita Cartas createdBy en el mulligan de practica
            emit startGame();
        }
    }
    else
    {
        //Win state
        //PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=El tabernero tag=PLAYSTATE value=WON
        if(line.contains(QRegularExpression(
                            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
                            "Entity=(.+) tag=PLAYSTATE value=WON"), match))
        {
            powerState = noGame;
            emit pDebug("Found WON (powerState = noGame).", numLine);

            winnerPlayer = match->captured(1);

            if(spectating || loadingScreenState == menu)
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
        //PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=GameEntity tag=TURN value=12
        else if(line.contains(QRegularExpression(
                            "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
                            "Entity=GameEntity tag=TURN value=(\\d+)"
                ), match))
        {
            turn = match->captured(1).toInt();
            emit pDebug("Found TURN: " + match->captured(1), numLine);

            if(powerState != inGameState && turn > 1)
            {
                powerState = inGameState;
                mulliganEnemyDone = mulliganPlayerDone = true;
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
    //Jugador/Enemigo names y firstPlayer
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
        else    emit pDebug("Read invalid PlayerID value: " + player, numLine, Error);

        if(numCards == "3")
        {
            firstPlayer = playerName;
            emit pDebug("Found First Player: " + firstPlayer, numLine);
        }
    }

    //Jugador roba carta inicial
    //GameState.DebugPrintEntityChoices() -   Entities[0]=[name=Conjurador etéreo id=22 zone=HAND zonePos=1 cardId=LOE_003 player=1]
    else if(line.contains(QRegularExpression(
                "GameState\\.DebugPrintEntityChoices\\(\\) - *Entities\\[\\d+\\]="
                "\\[name=(.*) id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\]"
                  ), match))
    {
        QString cardName = match->captured(1);
        QString cardId = match->captured(2);
        QString player = match->captured(3);

        emit pDebug("Player: Starting card drawn: " + cardName, numLine);
        emit playerCardDraw(cardId, true);

        if(playerID == 0 || playerTag.isEmpty())
        {
            playerID = player.toInt();
            playerTag = (playerID == 1)?name1:name2;
            emit enemyHero((playerID == 1)?hero2:hero1);
            emit pDebug("Found playerID: " + player + " playerTag: " + playerTag, 0);
        }
    }

    //Enemigo roba carta inicial
    else if(line.contains(QRegularExpression(
                  "GameState\\.DebugPrintEntityChoices\\(\\) - *Entities\\[\\d+\\]="
                  "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=\\d+\\]"
                  ), match))
    {
        QString id = match->captured(1);

        emit pDebug("Enemy: Starting card drawn. ID: " + id, numLine);
        emit enemyCardDraw(id.toInt());
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
                emit clearDrawList(true);

                if(mulliganEnemyDone)
                {
                    turn = 1;
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

                if(mulliganPlayerDone)
                {
                    turn = 1;
                    powerState = inGameState;
                    emit pDebug("Mulligan phase end (powerState = inGameState)", numLine);
                }
            }
        }
    }
}


void GameWatcher::processPowerInGame(QString &line, qint64 numLine)
{
    //TAG_CHANGE desconocido
    //Secret hero
    //GameState.DebugPrintPower() -     TAG_CHANGE Entity=[id=43 cardId= type=INVALID zone=HAND zonePos=1 player=2] tag=CLASS value=PALADIN
    //PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=[id=81 cardId= type=INVALID zone=SETASIDE zonePos=0 player=2] tag=HEALTH value=11
    if(line.contains(QRegularExpression(
        "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
        "Entity=\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=(\\d+)\\] tag=(\\w+) value=(\\w+)"
        ), match))
    {
        QString id = match->captured(1);
        QString player = match->captured(2);
        QString tag = match->captured(3);
        QString value = match->captured(4);
        bool isPlayer = (player.toInt() == playerID);

        if(tag == "CLASS")
        {
            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Secret hero: " + value + " Id: " + id, numLine);
            if(value == "MAGE")         secretHero = mage;
            else if(value == "HUNTER")  secretHero = hunter;
            else if(value == "PALADIN") secretHero = paladin;
        }
        else if(tag == "DAMAGE" || tag == "ATK" || tag == "HEALTH" || tag == "EXHAUSTED" ||
                tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY")
        {
            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": TAG_CHANGE(" + tag + "): " + value + " Id: " + id, numLine);
            if(isPlayer)    emit playerMinionTagChange(id.toInt(), tag, value);
            else            emit enemyMinionTagChange(id.toInt(), tag, value);
        }
    }


    //TAG_CHANGE conocido
    //PowerTaskList aparece segundo pero hay acciones que no tienen GameState, como el damage del maestro del acero herido
    //GameState.DebugPrintPower() -         TAG_CHANGE Entity=[name=Déspota del templo id=36 zone=PLAY zonePos=1 cardId=EX1_623 player=2] tag=DAMAGE value=0
    //GameState.DebugPrintPower() -     TAG_CHANGE Entity=[name=Déspota del templo id=36 zone=PLAY zonePos=1 cardId=EX1_623 player=2] tag=ATK value=3
    else if(line.contains(QRegularExpression(
        "PowerTaskList\\.DebugPrintPower\\(\\) - *TAG_CHANGE "
        "Entity=\\[name=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=\\w+ player=(\\d+)\\] "
        "tag=(\\w+) value=(\\w+)"
        ), match))
    {
        QString name = match->captured(1);
        QString id = match->captured(2);
        QString zone = match->captured(3);
        QString player = match->captured(4);
        QString tag = match->captured(5);
        QString value = match->captured(6);
        bool isPlayer = (player.toInt() == playerID);


        if(tag == "DAMAGE" || tag == "ATK" || tag == "HEALTH" || tag == "EXHAUSTED" ||
                tag == "DIVINE_SHIELD" || tag == "STEALTH" || tag == "TAUNT" || tag == "CHARGE" ||
                tag == "ARMOR" || tag == "FROZEN" || tag == "WINDFURY")
        {
            emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": TAG_CHANGE(" + tag + ")=" + value + " -- " + name, numLine);
            if(isPlayer)    emit playerMinionTagChange(id.toInt(), tag, value);
            else            emit enemyMinionTagChange(id.toInt(), tag, value);
        }
    }


    //Jugador/Enemigo accion con objetivo
    //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=ATTACK Entity=[name=Jinete de lobos id=45 zone=PLAY zonePos=1 cardId=CS2_124 player=2]
    //EffectCardId= EffectIndex=-1 Target=[name=Jaina Valiente id=64 zone=PLAY zonePos=0 cardId=HERO_08 player=1]
    //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=TRIGGER Entity=[name=Trepadora embrujada id=12 zone=GRAVEYARD zonePos=0 cardId=FP1_002 player=1]
    //EffectCardId= EffectIndex=0 Target=0
    //PowerTaskList.DebugPrintPower() - BLOCK_START BlockType=POWER Entity=[name=Elemental de Escarcha id=43 zone=PLAY zonePos=1 cardId=EX1_283 player=2]
    //EffectCardId= EffectIndex=-1 Target=[name=Trituradora antigua de Sneed id=23 zone=PLAY zonePos=5 cardId=GVG_114 player=1]
    else if(line.contains(QRegularExpression(
        "PowerTaskList\\.DebugPrintPower\\(\\) - BLOCK_START BlockType=(\\w+) "
        "Entity=\\[name=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=(\\d+)\\] "
        "EffectCardId=\\w* EffectIndex=-?\\d+ "
        "Target=(?:\\[name=(.*) id=(\\d+) zone=(\\w+) zonePos=\\d+ cardId=(\\w+) player=\\d+\\])?"
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
        emit pDebug("Trigger(" + blockType + "): " + name1 + (name2.isEmpty()?"":" --> " + name2), numLine);
        emit specialCardTrigger(cardId1, blockType, id1.toInt());
        if(isHeroPower(cardId1) && isPlayerTurn && player1.toInt()==playerID)     emit playerHeroPower();


        //Accion con objetivo en PLAY
        if(zone2 == "PLAY")
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
                    else if(isPlayer && isPlayerTurn)    emit playerSpellObjPlayed();
                }
                else
                {
                    emit pDebug((isPlayer?QString("Player"):QString("Enemy")) + ": Minion/weapon obj played: " +
                                name1 + " target " + name2, numLine);
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
        "\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=\\d+\\] zone from "
        "(\\w+ \\w+(?: \\(Weapon\\))?)? -> (\\w+ \\w+(?: \\((?:Weapon|Hero|Hero Power)\\))?)?"
        ), match))
    {
        QString id = match->captured(1);
        QString zoneFrom = match->captured(2);
        QString zoneTo = match->captured(3);


        //Enemigo juega carta desconocida
        if(zoneFrom == "OPPOSING HAND")
        {
            emit enemyCardPlayed(id.toInt());

            //Carta devuelta al mazo en Mulligan
            if(zoneTo == "OPPOSING DECK")
            {
                emit pDebug("Enemy: Starting card returned. ID: " + id, numLine);
            }
            else
            {
                emit pDebug("Enemy: Unknown card played. ID: " + id, numLine);
            }
        }

        //Enemigo juega secreto
        if(zoneTo == "OPPOSING SECRET")
        {
            emit pDebug("Enemy: Secret played. ID: " + id, numLine);
            emit enemySecretPlayed(id.toInt(), secretHero);
        }

        //Enemigo roba carta desconocida
        else if(zoneTo == "OPPOSING HAND")
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
        "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=(\\d+) cardId=(\\w+) player=(\\d+)\\] zone from "
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
            if(zoneFrom == "FRIENDLY PLAY") emit playerMinionZonePlaySteal(id.toInt(), zonePos.toInt());
            else if(zoneFrom.isEmpty())     emit enemyMinionZonePlayAddTriggered(cardId, id.toInt(), zonePos.toInt());
            else                            emit enemyMinionZonePlayAdd(cardId, id.toInt(), zonePos.toInt());
        }
        //Jugador, nuevo minion en PLAY
        else if(zoneTo == "FRIENDLY PLAY" && zoneFrom != "FRIENDLY PLAY")
        {
            playerMinions++;
            emit pDebug("Player: Minion moved to FRIENDLY PLAY: " + name + " Minions: " + QString::number(playerMinions), numLine);
            if(zoneFrom == "OPPOSING PLAY") emit enemyMinionZonePlaySteal(id.toInt(), zonePos.toInt());
            else if(zoneFrom.isEmpty())     emit playerMinionZonePlayAddTriggered(cardId, id.toInt(), zonePos.toInt());
            else                            emit playerMinionZonePlayAdd(cardId, id.toInt(), zonePos.toInt());
        }
        //Enemigo, carga heroe
        else if(zoneTo == "OPPOSING PLAY (Hero)")
        {
            emit pDebug("Enemy: Hero moved to OPPOSING PLAY (Hero): " + name, numLine);
            emit enemyHeroZonePlayAdd(cardId, id.toInt());
        }
        //Jugador, carga heroe
        else if(zoneTo == "FRIENDLY PLAY (Hero)")
        {
            emit pDebug("Player: Hero moved to FRIENDLY PLAY (Hero): " + name, numLine);
            if(playerID == 0)
            {
                playerID = player.toInt();
                emit pDebug("Found playerID: " + player, numLine);
            }
            emit playerHeroZonePlayAdd(cardId, id.toInt());
        }
        //Enemigo, equipa arma
        else if(zoneTo == "OPPOSING PLAY (Weapon)" && zoneFrom != "OPPOSING GRAVEYARD")//Al reemplazar un arma por otra, la antigua va, vuelve y va a graveyard.
        {
            emit pDebug("Enemy: Weapon moved to OPPOSING PLAY (Weapon): " + name, numLine);
            emit enemyWeaponZonePlayAdd(cardId, id.toInt());
        }
        //Jugador, equipa arma
        else if(zoneTo == "FRIENDLY PLAY (Weapon)" && zoneFrom != "FRIENDLY GRAVEYARD")
        {
            emit pDebug("Player: Weapon moved to FRIENDLY PLAY (Weapon): " + name, numLine);
            emit playerWeaponZonePlayAdd(cardId, id.toInt());
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
            //Carta devuelta al mazo en Mulligan
            else if(zoneTo == "FRIENDLY DECK")
            {
                emit pDebug("Player: Starting card returned: " + name, numLine);
                emit playerReturnToDeck(cardId);
            }
        }

        //Enemigo esbirro muere
        else if(zoneFrom == "OPPOSING PLAY" && zoneTo != "OPPOSING PLAY")
        {
            if(enemyMinions>0)  enemyMinions--;
            emit pDebug("Enemy: Minion removed from OPPOSING PLAY: " + name + " Minions: " + QString::number(enemyMinions), numLine);
            if(zoneTo != "FRIENDLY PLAY")   emit enemyMinionZonePlayRemove(id.toInt());

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
            if(zoneTo != "OPPOSING PLAY")   emit playerMinionZonePlayRemove(id.toInt());
        }
    }


    //Jugador/Enemigo esbirro cambia pos
    //No podemos usar zonePos= porque para los esbirros del jugador que pasan a una posicion mayor muestra su posicion origen
    //Todo comentado porque esta forma de contar el numero de esbirros puede producir errores.
    //Ej: Si un esbirro con deathrattle produce otro esbirro. Primero se cambia la pos de los esbirros a la dcha
    //y despues se genran los esbirros de deathrattle causando una suma erronea.
    //id=7 local=True [name=Ingeniera novata id=25 zone=HAND zonePos=5 cardId=EX1_015 player=1] pos from 5 -> 3
    else if(line.contains(QRegularExpression(
        "\\[name=(.*) id=(\\d+) zone=(?:HAND|PLAY) zonePos=\\d+ cardId=\\w+ player=(\\d+)\\] pos from \\d+ -> (\\d+)"
        ), match))
    {
        QString name = match->captured(1);
        QString id = match->captured(2);
        QString player = match->captured(3);
        QString zonePos = match->captured(4);

        //Jugador esbirro cambia pos
        if(player.toInt() == playerID)
        {
//            emit pDebug("Player: New minion pos: " +
//                        name + " >> " + zonePos + " Minions: " + QString::number(playerMinions), numLine);
            emit playerMinionPosChange(id.toInt(), zonePos.toInt());
        }
        //Enemigo esbirro cambia pos
        else
        {
//            emit pDebug("Enemy: New minion pos: " +
//                        name + " >> " + zonePos + " Minions: " + QString::number(enemyMinions), numLine);
            emit enemyMinionPosChange(id.toInt(), zonePos.toInt());
        }
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

    //Al jugar contra la maquina puede que se lea antes el fin de turno que el robo del inicio del turno
    //if(turn > turnReal+1)   turn = turnReal+1;

    bool playerTurn;
    if((firstPlayer==playerTag && turn%2==1) || (firstPlayer!=playerTag && turn%2==0))  playerTurn=true;
    else    playerTurn=false;

    if(firstPlayer.isEmpty() || playerTag.isEmpty())    playerTurn = playerDraw;

    //Al turno 1 dejamos que pase cualquiera asi dejamos el turno 0 para indicar cartas de mulligan
    //Solo avanza de turno al robar carta el jugador que le corresponde
    if(turn == 1 || playerDraw == playerTurn || playerID == 0)
    {
        turnReal = turn;
        emit pDebug("\nTurn: " + QString::number(turn) + " " + (playerTurn?"Player":"Enemy"), 0);

        isPlayerTurn = playerTurn;

        if(playerDraw)      emit clearDrawList();
        emit newTurn(isPlayerTurn, turn);

        //Secret CSpirit test
        if(!isPlayerTurn && enemyMinions > 0)
        {
            emit pDebug("CSpirit tested. Minions: " + QString::number(enemyMinions), 0);
            emit cSpiritTested();
        }
    }
}


//SecretHero GameWatcher::getSecretHero(QString playerHero, QString enemyHero)
//{
//    SecretHero enemySecretHero = unknown;
//    SecretHero playerSecretHero = unknown;

//    if(enemyHero == QString("04"))  enemySecretHero = paladin;
//    else if(enemyHero == QString("05"))  enemySecretHero = hunter;
//    else if(enemyHero == QString("08"))  enemySecretHero = mage;

//    if(playerHero == QString("04"))  playerSecretHero = paladin;
//    else if(playerHero == QString("05"))  playerSecretHero = hunter;
//    else if(playerHero == QString("08"))  playerSecretHero = mage;

//    if(enemySecretHero != unknown)  return enemySecretHero;
//    else if(playerSecretHero != unknown)  return playerSecretHero;
//    else    return unknown;
//}


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

