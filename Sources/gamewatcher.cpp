#include "gamewatcher.h"
#include <QtWidgets>


GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    gameState = noGame;
    loadingScreen = menu;
    deckRead = false;
    mulliganEnemyDone = false;
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
    gameState = noGame;
    loadingScreen = menu;
    deckRead = false;
    logSeekCreate = -1;
    emit pDebug("Reset (GameState = noGame).", 0);
    emit pDebug("Reset (LoadingScreen = menu).", 0);
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
        return true;
    }
}


void GameWatcher::processLogLine(QString line, qint64 numLine, qint64 logSeek)
{
    if(line.startsWith("[LoadingScreen]"))
    {
        processLoadingScreen(line, numLine);
    }
    else if(line.startsWith("[Arena]"))
    {
        processArena(line, numLine);
    }
    else if(line.startsWith("[Power]"))
    {
        processPower(line, numLine, logSeek);
    }
    else if(line.startsWith("[Zone]"))
    {
        processZone(line, numLine);
    }
}


void GameWatcher::startReadingDeck()
{
    if(gameState != noGame || deckRead) return;
    gameState = readingDeck;
    emit pDebug("Start reading deck (GameState = readingDeck).", 0);
    emit needResetDeck();    //resetDeck
}


void GameWatcher::endReadingDeck()
{
    if(gameState != readingDeck)    return;
    deckRead = true;
    gameState = noGame;
    emit pDebug("End reading deck (GameState = noGame)(DeckRead = true).", 0);
    emit pLog(tr("Log: Active deck read."));
}


void GameWatcher::setDeckRead(bool value)
{
    emit pDebug(QString("SetDeckRead (DeckRead = ") + (value?"true":"false") + ")", 0);
    deckRead = value;
    if(deckRead && gameState == readingDeck)
    {
        endReadingDeck();
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

        //Redundante en caso de que falle
        //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
        endReadingDeck();

        if(prevMode == "GAMEPLAY")
        {
            emit endGame();
            gameState = noGame;
            emit pDebug("Quitting GAMEPLAY (GameState = noGame).", numLine);
        }

        if(currMode == "DRAFT")
        {
            loadingScreen = arena;
            emit pDebug("Entering ARENA (LoadingScreen = arena).", numLine);

            if(prevMode == "HUB")
            {
                emit enterArena();//enterArena deckHandler
            }
        }
        else if(currMode == "HUB")
        {
            loadingScreen = menu;
            emit pDebug("Entering MENU (LoadingScreen = menu).", numLine);

            if(prevMode == "DRAFT")
            {
                deckRead = false;
                emit pDebug("(deckRead = false)", 0);
                emit leaveArena();//leaveArena deckHandler
            }
        }
        else if(currMode == "TOURNAMENT")
        {
            loadingScreen = constructed;
            emit pDebug("Entering CONSTRUCTED (LoadingScreen = constructed).", numLine);
        }
        else if(currMode == "ADVENTURE")
        {
            loadingScreen = adventure;
            emit pDebug("Entering ADVENTURE (LoadingScreen = adventure).", numLine);
        }
        else if(currMode == "TAVERN_BRAWL")
        {
            loadingScreen = tavernBrawl;
            emit pDebug("Entering TAVERN (LoadingScreen = tavernBrawl).", numLine);
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
        emit newArena(hero); //(sync)Begin draft //(sync)resetDeckDontRead (deckRead = true)
    }
    //END READING DECK
    //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
    else if(synchronized && line.startsWith("[Arena] SetDraftMode - ACTIVE_DRAFT_DECK"))
    {
        emit pDebug("Found ACTIVE_DRAFT_DECK (GameState = noGame).", numLine);
        emit activeDraftDeck(); //End draft
        endReadingDeck();

        //Redundante en caso de que falle ScreenEndOfGame
        emit endGame();
        gameState = noGame;
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
    else if(synchronized && (gameState == readingDeck) &&
        line.contains(QRegularExpression(
            "DraftManager\\.OnChoicesAndContents - Draft deck contains card (\\w+)"), match))
    {
        QString code = match->captured(1);
        emit pDebug("Reading deck: " + code, numLine);
        emit newDeckCard(code);
    }
    //IN REWARDS
    //[Arena] SetDraftMode - IN_REWARDS
    else if(synchronized && line.startsWith("[Arena] SetDraftMode - IN_REWARDS"))
    {
        emit pDebug("Found IN_REWARDS.", numLine);
        emit inRewards();   //Show rewards input
    }
}


void GameWatcher::processPower(QString &line, qint64 numLine, qint64 logSeek)
{
    switch(gameState)
    {
        case readingDeck:
        case noGame:
            //[Power] ================== Start Spectator Game ==================
            if(line.contains(QRegularExpression("Start Spectator Game"), match))
            {
                loadingScreen = spectator;
                emit pDebug("Entering SPECTATOR.", numLine);
            }
            else if(line.startsWith("[Power] GameState.DebugPrintPower() - CREATE_GAME"))
            {
                //Redundante en caso de que falle
                //[Arena] SetDraftMode - ACTIVE_DRAFT_DECK
                endReadingDeck();

                emit pDebug("\nFound CREATE_GAME (GameState = heroType1State)", numLine);
                emit pDebug("PlayerTag: " + playerTag, 0);
                logSeekCreate = logSeek;
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
                playerMinions = 0;
                enemyMinions = 0;
                enemyMinionsAliveForAvenge = -1;

                emit startGame();
                emit specialCardTrigger("", "");    //Evita Cartas createdBy en el mulligan de practica
            }
            break;
        case heroType1State:
        case heroType2State:
            if(gameState == heroType1State && line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero1 = match->captured(1);
                gameState = heroType2State;
                emit pDebug("Found hero 1: " + hero1 + " (GameState = heroType2State)", numLine);
            }
            else if(gameState == heroType2State && line.contains(QRegularExpression("Creating ID=\\d+ CardID=HERO_(\\d+)"), match))
            {
                hero2 = match->captured(1);
                if(loadingScreen == spectator)  gameState = inGameState;
                else                            gameState = playerName1State;
                emit pDebug("Found hero 2: " + hero2 + " (GameState = playerName1State)", numLine);
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
                gameState = playerName2State;
                emit pDebug("Found player 2: " + name2 + " (GameState = playerName2State)", numLine);
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
                gameState = inGameState;
                emit pDebug("Found player 1: " + name1 + " (GameState = inGameState)", numLine);
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
                emit pDebug("Found First Player: " + firstPlayer, numLine);
            }
            break;
        case inGameState:
            processPowerInGame(line, numLine, logSeek);
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


void GameWatcher::processPowerInGame(QString &line, qint64 numLine, qint64 logSeek)
{
    //Win state
    if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
    {
        gameState = noGame;
        emit pDebug("Found WON (GameState = noGame).", numLine);

        winnerPlayer = match->captured(1);
        createGameResult();
        createGameLog(logSeek + line.length());
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
                    if(loadingScreen != spectator)  playerTag = name1;
                    secretHero = getSecretHero(hero1, hero2);
                }
                else if(playerID == 2)
                {
                    if(loadingScreen != spectator)  playerTag = name2;
                    secretHero = getSecretHero(hero2, hero1);
                }
                else
                {
                    playerID = 0;
                    emit pDebug("Read invalid PlayerID value: " + player, 0, Error);
                }

                if(loadingScreen != spectator)
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
                emit pDebug("Enemy mulligan end.", numLine);
                mulliganEnemyDone = true;
            }
        }

        //ULTIMO TRIGGER SPECIAL CARDS
        //[Power] PowerTaskList.DebugPrintPower() - ACTION_START BlockType=ATTACK Entity=[name=Garrosh Grito Infernal id=64 zone=PLAY zonePos=0 cardId=HERO_01 player=1]
        //EffectCardId= EffectIndex=-1 Target=[name=Uther el Iluminado id=66 zone=PLAY zonePos=0 cardId=HERO_04 player=2]
        else if(line.contains(QRegularExpression(
            "PowerTaskList\\.DebugPrintPower\\(\\) - ACTION_START BlockType=(\\w+) "
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
        //[Power] GameState.DebugPrintPower() - ACTION_START BlockType=ATTACK Entity=[name=Garrosh Grito Infernal id=64 zone=PLAY zonePos=0 cardId=HERO_01 player=1]
        //EffectCardId= EffectIndex=-1 Target=[name=Uther el Iluminado id=66 zone=PLAY zonePos=0 cardId=HERO_04 player=2]
        else if(line.contains(QRegularExpression(
            "GameState\\.DebugPrintPower\\(\\) - ACTION_START BlockType=(\\w+) "
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
    if(gameState == inGameState && synchronized)
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
                    enemyMinions++;
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
                    playerMinions++;
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
            else if(zoneFrom == "OPPOSING PLAY" && zoneTo == "OPPOSING GRAVEYARD")
            {
                enemyMinions--;
                emit pDebug("Enemy: Minion dead: " + name + " Minions: " + QString::number(enemyMinions), numLine);

                if(isPlayerTurn)
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
            else if(zoneFrom == "FRIENDLY PLAY" && zoneTo == "FRIENDLY GRAVEYARD")
            {
                playerMinions--;
                emit pDebug("Player: Minion dead: " + name + " Minions: " + QString::number(playerMinions), numLine);
            }
        }


        //Jugador/Enemigo esbirro cambia pos
        //No podemos usar zonePos= porque para los esbirros del jugador que pasan a una posicion mayor muestra su posicion origen
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=\\w+ player=(\\d+)\\] pos from \\d+ -> (\\d+)"
            ), match))
        {
            QString name = match->captured(1);
            QString player = match->captured(2);
            QString zonePos = match->captured(3);

            //Jugador esbirro cambia pos
            if(player.toInt() == playerID)
            {
                if(zonePos.toInt() > playerMinions) playerMinions = zonePos.toInt();
                emit pDebug("Player: New minion pos: " +
                            name + " >> " + zonePos + " Minions: " + QString::number(playerMinions), numLine);
            }
            //Enemigo esbirro cambia pos
            else
            {
                if(zonePos.toInt() > enemyMinions) enemyMinions = zonePos.toInt();
                emit pDebug("Enemy: New minion pos: " +
                            name + " >> " + zonePos + " Minions: " + QString::number(enemyMinions), numLine);
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


void GameWatcher::createGameResult()
{
    if(loadingScreen == spectator)
    {
        emit pDebug("CreateGameResult: Avoid spectator game result.", 0);
        return;
    }

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

    emit newGameResult(gameResult, loadingScreen);
}


void GameWatcher::createGameLog(qint64 logSeekWon)
{
    if(logSeekCreate == -1)
    {
        emit pDebug("Cannot create match log. Found WON but not CREATE_GAME", 0);
        return;
    }

    QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh:mm");
    QString win = (winnerPlayer == playerTag)?"WIN":"LOSE";
    QString gameMode = Utility::getLoadingScreenString(loadingScreen);
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


void GameWatcher::setSynchronized()
{
    this->synchronized = true;
}


LoadingScreen GameWatcher::getLoadingScreen()
{
    return this->loadingScreen;
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

