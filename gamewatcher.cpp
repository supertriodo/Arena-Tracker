#include "gamewatcher.h"
#include <QtWidgets>

GameWatcher::GameWatcher(QObject *parent) : QObject(parent)
{
    gameState = noGame;
    arenaMode = false;
    deckRead = false;
    match = new QRegularExpressionMatch();

    QSettings settings("Arena Tracker", "Arena Tracker");
    this->playerTag = settings.value("playerTag", "").toString();
}


GameWatcher::~GameWatcher()
{
    delete match;
}


void GameWatcher::processLogLine(QString &line)
{
    if(line.startsWith("[Bob]"))
    {
        if(line.startsWith("[Bob] ---Register"))
        {
            if(line.startsWith("[Bob] ---RegisterScreenForge---"))
            {
                arenaMode = true;
//                qDebug() << "GameWatcher: "<< "Entrando en arena.";
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
                hero1.clear();
                hero2.clear();
                name1.clear();
                name2.clear();
                firstPlayer.clear();
                winnerPlayer.clear();

                if(gameState != noGame)
                {
                    qDebug() << "GameWatcher: "<< "ERROR: Juego incompleto.";
                    emit sendLog(tr("Log: WARNING:Discard game, info missing in log."));
                    gameState = noGame;
                }
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
//                qDebug() << "GameWatcher: "<< "Fuera de arena.";
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
        processGameLine(line);
    }
    else if(line.startsWith("[Zone]"))
    {
        if(gameState == winnerState)
        {
            if(line.contains(QRegularExpression(
                "\\[name=(.*) id=\\d+ zone=(\\w+) zonePos=0 cardId=(\\w+) player=\\d+\\] zone from FRIENDLY DECK ->"
                ), match))
            {
                if(match->captured(2) != "DECK")
                {
                    qDebug() << "GameWatcher: " << "Carta robada: " << match->captured(3) << " - " << match->captured(1);
                    emit cardDrawn(match->captured(3));
                }
            }

        }
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
                        QString card = match->captured(1);
                        if(!card.contains("HERO"))
                        {
                            qDebug() << "GameWatcher: "<< "Nueva carta.";
                            emit newDeckCard(card);
                        }
                    }
                }
            }
        }
    }
}


void GameWatcher::processGameLine(QString &line)
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
                gameState = winnerState;
            }
            else if(line.contains(QRegularExpression("Entity=(.+) tag=FIRST_PLAYER value=1"), match))
            {
                firstPlayer = match->captured(1);
            }
            break;
        case winnerState:
            if(line.contains(QRegularExpression("Entity=(.+) tag=PLAYSTATE value=WON"), match))
            {
                winnerPlayer = match->captured(1);
                gameState = noGame;
                createGameResult();
            }
            else if(line.contains(QRegularExpression(
                    "m_chosenEntities\\[\\d+\\]=\\[name=.* id=\\d+ zone=HAND zonePos=\\d+ cardId=(\\w+) player=\\d+\\]"
                    ), match))
            {
                qDebug() << "GameWatcher: " << "Carta Inicial robada: " << match->captured(1);
                emit cardDrawn(match->captured(1));
            }
            break;
        case inRewards:
            break;
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
