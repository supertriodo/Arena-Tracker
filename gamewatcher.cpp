#include "gamewatcher.h"
#include <QtWidgets>

GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    gameState = noGame;
    arenaMode = false;
    deckRead = false;
    mulliganEnemyDone = false;
    turn = turnReal = 0;
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
            }
            else if(line.startsWith("[Bob] ---RegisterProfileNotices---") ||
                    line.startsWith("[Bob] ---RegisterFriendChallenge---"))
            {
                if(gameState == inRewards)
                {
                    gameState = noGame;
                    qDebug() << "GameWatcher: "<< "Rewards completos.";
                    emit sendLog(tr("Log: New rewards."));
                    emit arenaRewardsComplete();
                }
            }
            else if(line.startsWith("[Bob] ---RegisterScreenEndOfGame---"))
            {
                gameState = noGame;

                if(arenaMode)
                {
                    emit endGame();
                }
            }
            else
            {
                arenaMode = false;
#ifdef QT_DEBUG
                arenaMode = true;//Testing
#endif
            }

            if(gameState == readingDeck)
            {
                deckRead = true;
                gameState = noGame;
                qDebug() << "GameWatcher: "<< "Final leer deck.";
                emit sendLog(tr("Log: Active deck read."));
            }
        }
    }
    else if(line.startsWith("[Rachelle]"))
    {
        if(line.contains(QRegularExpression("reward \\d=\\[")))
        {
            gameState = inRewards;
            qDebug() << "GameWatcher: "<< "Nuevo reward.";

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
        else if(line.contains(QRegularExpression("DraftManager\\.OnChosen.+ hero = HERO_(\\d+)"), match))
        {
            QString hero = match->captured(1);
            qDebug() << "GameWatcher: "<< "Nueva arena.";
            emit sendLog(tr("Log: New arena."));
            emit newArena(hero);
            deckRead = false;
        }
    }
    else if(line.startsWith("[Power]"))
    {
        processPower(line);
    }
    else if(line.startsWith("[Zone]"))
    {
        processZone(line);
    }
    else
    {
        if(!deckRead)
        {
            if(line.startsWith("[Ben]"))
            {
                if(line.startsWith("[Ben] SetDraftMode - ACTIVE_DRAFT_DECK"))
                {
                    gameState = readingDeck;
                    qDebug() << "GameWatcher: "<< "Inicio leer deck.";
                }
            }
            else if(line.startsWith("[Asset]"))
            {
                if(gameState == readingDeck)
                {
                    if(line.contains(QRegularExpression(
                            "CachedAsset\\.UnloadAssetObject.+ - unloading name=(\\w+) family=CardPrefab persistent=False"), match))
                    {
                        QString code = match->captured(1);
                        if(!code.contains("HERO"))
                        {
                            qDebug() << "GameWatcher: "<< "Nueva carta: " << code;
                            emit newDeckCard(code);
                        }
                    }
                }
            }
        }
    }
}


void GameWatcher::processPower(QString &line)
{
    switch(gameState)
    {
        case readingDeck:
        case noGame:
            if(line.contains("CREATE_GAME"))
            {
                if(gameState == readingDeck)
                {
                    deckRead = true;
                    gameState = noGame;
                    qDebug() << "GameWatcher: "<< "Final leer deck.";
                    emit sendLog(tr("Log: Active deck read."));
                }

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
                if(name2 == playerTag)  playerID = 2;
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
                if(name1 == playerTag)  playerID = 1;
                gameState = inGameState;
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
        qDebug() << "GameWatcher: " << "Jugador: Carta Inicial robada: " << match->captured(1);
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
            qDebug() << "GameWatcher: " << "Mulligan enemigo terminado.";
            mulliganEnemyDone = true;
            if(firstPlayer == playerTag)
            {
                //Convertir ultima carta en moneda enemiga
                emit lastHandCardIsCoin();
            }
        }
    }
    //Jugador juega carta con objetivo
    else if(line.contains(QRegularExpression(
        "ACTION_START Entity=\\[name=(.*) id=\\d+ zone=HAND zonePos=\\d+ cardId=(.*) player=(\\d+)\\] SubType=PLAY Index=0 "
        "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(.*) player=(\\d+)\\]"
        ), match))
    {
        if(match->captured(3).toInt() == playerID)
        {
            qDebug() << "GameWatcher: " << "Jugador: Carta obj jugada: " << match->captured(1) << "sobre" << match->captured(4);
            //emit juega carta con objetivo (hechizo o esbirro o arma)
        }
    }
    //Jugador ataca (esbirro/heroe VS esbirro/heroe)
    else if(line.contains(QRegularExpression(
        "ACTION_START Entity=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(.*) player=(\\d+)\\] SubType=ATTACK Index=-1 "
        "Target=\\[name=(.*) id=\\d+ zone=PLAY zonePos=\\d+ cardId=(.*) player=(\\d+)\\]"
        ), match))
    {
        if(match->captured(3).toInt() == playerID)
        {
            qDebug() << "GameWatcher: " << "Jugador: Ataca: " << match->captured(1) << "vs" << match->captured(4);

            if(match->captured(2).contains("HERO"))
            {
                if(match->captured(5).contains("HERO"))
                {
                    //emit ataque heroe vs heroe
                }
                else
                {
                    //emit ataque heroe vs esbirro
                }
            }
            else
            {
                if(match->captured(5).contains("HERO"))
                {
                    //emit ataque esbirro vs heroe
                }
                else
                {
                    //emit ataque esbirro vs esbirro
                }
            }
        }
    }
}


void GameWatcher::processZone(QString &line)
{
    if(gameState == inGameState)
    {
        //JUGADOR JUEGA
        //Jugador juega carta
        if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from FRIENDLY HAND -> (FRIENDLY PLAY)?\n"
            ), match))
        {
            //Jugador juega hechizo
            if(match->captured(3).isEmpty())
            {
                qDebug() << "GameWatcher: " << "Jugador: Hechizo jugado: " << match->captured(1);
                //emit juega hechizo
            }
            //Jugador juega esbirro/arma
            else
            {
                qDebug() << "GameWatcher: " << "Jugador: Esbirro/arma jugado: " << match->captured(1);
                //emit juega esbirro/arma
            }
        }

        //ENEMIGO JUEGA
        //Enemigo juega secreto o carta devuelta al mazo en Mulligan
        //[id=54 cardId= type=INVALID zone=SECRET zonePos=1 player=2] zone from OPPOSING HAND -> OPPOSING SECRET
        else if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=\\w+ zonePos=\\d+ player=\\d+\\] zone from OPPOSING HAND -> OPPOSING (SECRET|DECK)"
            ), match))
        {
            if(match->captured(2) == "SECRET")
            {
                qDebug() << "GameWatcher: " << "Enemigo: Secreto jugado. ID:" << match->captured(1);
                //emit juega secreto
            }
            else
            {
                qDebug() << "GameWatcher: " << "Enemigo: Carta inicial devuelta. ID:" << match->captured(1);
            }
            emit enemyCardPlayed(match->captured(1).toInt());
        }
        //Enemigo juega carta
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING HAND ->"
            ), match))
        {
            qDebug() << "GameWatcher: " << "Enemigo: Carta jugada: " << match->captured(1) << ". ID:" << match->captured(2);
            emit enemyCardPlayed(match->captured(2).toInt(), match->captured(3));
        }

        //ENEMIGO SECRETO DESVELADO
        //[name=Contrahechizo id=54 zone=GRAVEYARD zonePos=0 cardId=EX1_287 player=2] zone from OPPOSING SECRET -> OPPOSING GRAVEYARD
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING SECRET -> OPPOSING GRAVEYARD"
            ), match))
        {
            qDebug() << "GameWatcher: " << "Enemigo: Secreto desvelado: " << match->captured(1);
            //emit secreto desvelado
        }

        //JUGADOR ESBIRRO MUERE
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from FRIENDLY PLAY -> FRIENDLY GRAVEYARD"
            ), match))
        {
            qDebug() << "GameWatcher: " << "Jugador: Esbirro muerto: " << match->captured(1);
            //emit no necesario
        }

        //ENEMIGO ESBIRRO MUERE
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING PLAY -> OPPOSING GRAVEYARD"
            ), match))
        {
            qDebug() << "GameWatcher: " << "Enemigo: Esbirro muerto: " << match->captured(1);
            //emit muere esbirro
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
                qDebug() << "GameWatcher: " << "Jugador: Carta robada: " << match->captured(3) << " - " << match->captured(1);
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
            qDebug() << "GameWatcher: " << "Enemigo: Carta robada. ID:" << match->captured(1);
            emit enemyCardDraw(match->captured(1).toInt(), turnReal);
        }

        else if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=\\d+\\] zone from  -> OPPOSING HAND"
            ), match))
        {
            //Enemigo roba carta especial del vacio
            if(mulliganEnemyDone)
            {
                qDebug() << "GameWatcher: " << "Enemigo: Carta especial robada. ID:" << match->captured(1);
                emit enemyCardDraw(match->captured(1).toInt(), turnReal, true);
            }
            //Enemigo roba carta inicial
            else
            {
                qDebug() << "GameWatcher: " << "Enemigo: Carta inicial robada. ID:" << match->captured(1);
                emit enemyCardDraw(match->captured(1).toInt());
            }
        }
        //Enemigo roba carta conocida
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from .* -> OPPOSING HAND"
            ), match))
        {
            qDebug() << "GameWatcher: " << "Enemigo: Carta devuelta a la mano: " << match->captured(1) << ". ID:" << match->captured(2);
            emit enemyCardDraw(match->captured(2).toInt(), turnReal, false, match->captured(3));
        }
    }
}


QString GameWatcher::askPlayerTag(QString &playerName1, QString &playerName2)
{
    QMessageBox msgBox;
    msgBox.setText(tr("Who are you?"));
    msgBox.setWindowTitle(tr("Player Tag"));
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
        qDebug() << "GameWatcher: "<< "ERROR: PlayerTag no jugo en esta partida.";
        emit sendLog(tr("Log: WARNING:Registered game played without you.") + "" + playerTag);
        return;
    }

    gameResult.isFirst = (firstPlayer == playerTag);
    gameResult.isWinner = (winnerPlayer == playerTag);

    qDebug() << "GameWatcher: "<< "Nuevo juego de arena.";
    emit sendLog(tr("Log: New game."));

    emit newGameResult(gameResult);
}


void GameWatcher::setDeckRead()
{
    deckRead = true;
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
        qDebug() << endl << "GameWatcher: " << "Turno: " << QString::number(turn) << " " << (playerTurn?"Jugador":"Enemigo");
    }
}
