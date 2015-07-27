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
    if(gameState != drafting)
    {
        gameState = noGame;
        qDebug() << "GameWatcher: GameState = noGame";
    }
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
                    qDebug() << "GameWatcher: GameState = noGame";
                    qDebug() << "GameWatcher: "<< "Rewards completos.";
                    emit sendLog(tr("Log: New rewards."));
                    emit arenaRewardsComplete();
                }
            }
            else if(line.startsWith("[Bob] ---RegisterScreenEndOfGame---"))
            {
                gameState = noGame;
                qDebug() << "GameWatcher: GameState = noGame";

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
                endReadingDeck();
            }

            if(gameState == drafting)
            {
                qDebug() << "GameWatcher: " << "Pause draft.";
                emit pauseDraft();
            }
        }
    }
    else if(line.startsWith("[Rachelle]"))
    {
        if(line.contains(QRegularExpression("reward \\d=\\[")))
        {
            gameState = inRewards;
            qDebug() << "GameWatcher: GameState = inRewards";
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
        else if(line.contains(QRegularExpression("DraftManager\\.OnChosen.+ hero=HERO_(\\d+)"), match))
        {
            QString hero = match->captured(1);
            qDebug() << "GameWatcher: "<< "Nueva arena.";
            emit sendLog(tr("Log: New arena."));
            emit newArena(hero);
            deckRead = false;

            if(synchronized)
            {
                newDraft(hero);
            }
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
    else if(line.startsWith("[Ben]"))
    {
        if((gameState == drafting) && line.startsWith("[Ben] SetDraftMode - DRAFTING"))
        {
            qDebug() << "GameWatcher: " << "Resume draft.";
            emit resumeDraft();
        }
        else if(line.startsWith("[Ben] SetDraftMode - ACTIVE_DRAFT_DECK"))
        {
            if(gameState == drafting)
            {
                gameState = noGame;
                qDebug() << "GameWatcher: GameState = noGame";
                qDebug() << "GameWatcher: "<< "End draft.";
                emit endDraft();
            }

            if(!deckRead)
            {
                gameState = readingDeck;
                qDebug() << "GameWatcher: GameState = readingDeck";
                qDebug() << "GameWatcher: "<< "Inicio leer deck.";
            }
        }
    }
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
                qDebug() << "GameWatcher: Desechamos HERO";
                endReadingDeck();
                return;
            }
            //Hero powers
            if( code=="CS2_102" || code=="CS2_083b" || code=="CS2_034" ||
                code=="CS1h_001" || code=="CS2_056" || code=="CS2_101" ||
                code=="CS2_017" || code=="DS1h_292" || code=="CS2_049")
            {
                qDebug() << "GameWatcher: Desechamos HERO POWER";
                endReadingDeck();
                return;
            }
            emit newDeckCard(code);
        }
    }
}


void GameWatcher::newDraft(QString hero)
{
    gameState = drafting;
    qDebug() << "GameWatcher: GameState = drafting";
    qDebug() << "GameWatcher: "<< "Begin draft.";
    emit beginDraft(hero);
}


void GameWatcher::endReadingDeck()
{
    deckRead = true;
    gameState = noGame;
    qDebug() << "GameWatcher: GameState = noGame";
    qDebug() << "GameWatcher: "<< "Final leer deck.";
    emit sendLog(tr("Log: Active deck read."));
}


void GameWatcher::processPower(QString &line)
{
    switch(gameState)
    {
        case readingDeck:
        case drafting:
        case noGame:
            if(line.contains("CREATE_GAME"))
            {
                //Nunca ocurre (+seguridad)
                if(gameState == drafting)
                {
                    gameState = noGame;
                    qDebug() << "GameWatcher: GameState = noGame";
                    qDebug() << "GameWatcher: "<< "End draft.";
                    emit endDraft();
                }

                if(gameState == readingDeck)
                {
                    endReadingDeck();
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
                qDebug() << "GameWatcher: GameState = inGameState";
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
        qDebug() << "GameWatcher: Jugador: Carta Inicial robada:" << match->captured(1);
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
            qDebug() << "GameWatcher: Mulligan enemigo terminado.";
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
                    qDebug() << "GameWatcher: Jugador: Hechizo obj jugada:" <<
                                match->captured(1) << "sobre" << match->captured(4);
                    if(match->captured(5) == MAD_SCIENTIST)
                    {
                        qDebug() << "GameWatcher: Saltamos comprobacion de secretos";
                    }
                    else if(isPlayerTurn)    emit playerSpellObjPlayed();
                }
                else
                {
                    qDebug() << "GameWatcher: Jugador: Esbirro/arma obj jugada:" <<
                                match->captured(1) << "sobre" << match->captured(4);
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
                        qDebug() << "GameWatcher: Jugador: Ataca:" <<
                                    match->captured(1) << "(heroe)vs(heroe)" << match->captured(4);
                        if(isPlayerTurn)    emit playerAttack(true, true);
                    }
                    else
                    {
                        qDebug() << "GameWatcher: Jugador: Ataca:" <<
                                    match->captured(1) << "(heroe)vs(esbirro)" << match->captured(4);
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            qDebug() << "GameWatcher: Saltamos comprobacion de secretos";
                        }
                        else */if(isPlayerTurn)    emit playerAttack(true, false);
                    }
                }
                else
                {
                    if(match->captured(5).contains("HERO"))
                    {
                        qDebug() << "GameWatcher: Jugador: Ataca:" <<
                                    match->captured(1) << "(esbirro)vs(heroe)" << match->captured(4);
                        if(isPlayerTurn)    emit playerAttack(false, true);
                    }
                    else
                    {
                        qDebug() << "GameWatcher: Jugador: Ataca:" <<
                                    match->captured(1) << "(esbirro)vs(esbirro)" << match->captured(4);
                        /*if(match->captured(5) == MAD_SCIENTIST) //Son comprobaciones now de secretos
                        {
                            qDebug() << "GameWatcher: Saltamos comprobacion de secretos";
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
                    qDebug() << "GameWatcher: Enemigo: Carta inicial devuelta. ID:" << match->captured(1);
                }
            }
            if(match->captured(3) == "SECRET")
            {
                qDebug() << "GameWatcher: Enemigo: Secreto jugado. ID:" << match->captured(1);
                emit enemySecretPlayed(match->captured(1).toInt(), secretHero);
            }

        }
        //Enemigo juega carta
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING HAND -> (.*)\n"
            ), match))
        {
            //Enemigo juega hechizo
            if(match->captured(4).isEmpty())
            {
                qDebug() << "GameWatcher: Enemigo: Hechizo jugado:" << match->captured(1) << "ID:" << match->captured(2);
            }
            //Enemigo juega esbirro
            else if(synchronized && (match->captured(4) == "OPPOSING PLAY"))
            {
                enemyMinions++;
                qDebug() << "GameWatcher: Enemigo: Esbirro jugado:" << match->captured(1) << "ID:" << match->captured(2) << "Esbirros:" << enemyMinions;
            }
            //Enemigo juega arma
            else if(match->captured(4) == "OPPOSING PLAY (Weapon)")
            {
                qDebug() << "GameWatcher: Enemigo: Arma jugada:" << match->captured(1) << "ID:" << match->captured(2);
            }
            else if(match->captured(4) == "OPPOSING GRAVEYARD")
            {
                qDebug() << "GameWatcher: Enemigo: Carta descartada:" << match->captured(1) << "ID:" << match->captured(2);
            }

            emit enemyCardPlayed(match->captured(2).toInt(), match->captured(3));
        }

        //ENEMIGO SECRETO DESVELADO
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=\\w+ zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING SECRET ->"// OPPOSING GRAVEYARD"
            ), match))
        {
            qDebug() << "GameWatcher: Enemigo: Secreto desvelado:" << match->captured(1);
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
                qDebug() << "GameWatcher: Jugador: Carta robada:" << match->captured(1);
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
            qDebug() << "GameWatcher: Enemigo: Carta robada. ID:" << match->captured(1);
            emit enemyCardDraw(match->captured(1).toInt(), turnReal);
        }

        else if(line.contains(QRegularExpression(
            "\\[id=(\\d+) cardId= type=INVALID zone=HAND zonePos=\\d+ player=\\d+\\] zone from  -> OPPOSING HAND"
            ), match))
        {
            //Enemigo roba carta especial del vacio
            if(mulliganEnemyDone)
            {
                qDebug() << "GameWatcher: Enemigo: Carta especial robada. ID:" << match->captured(1);
                emit enemyCardDraw(match->captured(1).toInt(), turnReal, true);
            }
            //Enemigo roba carta inicial
            else
            {
                qDebug() << "GameWatcher: Enemigo: Carta inicial robada. ID:" << match->captured(1);
                emit enemyCardDraw(match->captured(1).toInt());
            }
        }
        //Enemigo roba carta conocida
        else if(line.contains(QRegularExpression(
            "\\[name=(.*) id=(\\d+) zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from .* -> OPPOSING HAND"
            ), match))
        {
            qDebug() << "GameWatcher: Enemigo: Carta devuelta a la mano:" << match->captured(1) << "ID:" << match->captured(2);
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
                    qDebug() << "GameWatcher: Jugador: Hechizo jugado:" << match->captured(1);
                    if(isPlayerTurn)    emit playerSpellPlayed();
                }
                //Jugador juega esbirro
                else if(match->captured(2) == "FRIENDLY PLAY")
                {
                    qDebug() << "GameWatcher: Jugador: Esbirro jugado:" << match->captured(1);
                    if(isPlayerTurn)    emit playerMinionPlayed();
                }
                //Jugador juega arma
                else
                {
                    qDebug() << "GameWatcher: Jugador: Arma jugada:" << match->captured(1);
                }
            }

            //JUGADOR ESBIRRO MUERE
    //        else if(line.contains(QRegularExpression(
    //            "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=\\w+ player=\\d+\\] zone from FRIENDLY PLAY -> FRIENDLY GRAVEYARD"
    //            ), match))
    //        {
    //            qDebug() << "GameWatcher: Jugador: Esbirro muerto: " << match->captured(1);
    //            //emit no necesario
    //        }

            //ENEMIGO ESBIRRO MUERE
            else if(line.contains(QRegularExpression(
                "\\[name=(.*) id=\\d+ zone=GRAVEYARD zonePos=\\d+ cardId=(\\w+) player=\\d+\\] zone from OPPOSING PLAY -> OPPOSING GRAVEYARD"
                ), match))
            {
                enemyMinions--;
                qDebug() << "GameWatcher: Enemigo: Esbirro muerto:" << match->captured(1) << "Esbirros:" << enemyMinions;

                if(isPlayerTurn)
                {
                    emit enemyMinionDead();
                    if(enemyMinionsAliveForAvenge == -1)
                    {
                        if(match->captured(2) == MAD_SCIENTIST)
                        {
                            qDebug() << "GameWatcher: Saltamos comprobacion de secretos";
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
                    qDebug() << "GameWatcher: Enemigo: Nueva posicion esbirro:" <<
                                match->captured(1) << ">>" << match->captured(2) << "Esbirros:" << enemyMinions;
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
        qDebug() << "GameWatcher: Avenge tested: Supervivientes:" << enemyMinionsAliveForAvenge;
    }
    else    qDebug() << "GameWatcher: Avenge not tested: Supervivientes:" << enemyMinionsAliveForAvenge;
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
        qDebug() << "GameWatcher: "<< "ERROR: PlayerTag no jugo en esta partida.";
        emit sendLog(tr("Log: WARNING:Registered game played without you.") + "" + playerTag);
        return;
    }

    gameResult.isFirst = (firstPlayer == playerTag);
    gameResult.isWinner = (winnerPlayer == playerTag);

    qDebug() << endl << "GameWatcher: "<< "Nuevo juego de arena.";
    emit sendLog(tr("Log: New game."));

    emit newGameResult(gameResult);
}


void GameWatcher::setDeckRead()
{
    deckRead = true;
    if(gameState == readingDeck)
    {
        endReadingDeck();
    }
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










