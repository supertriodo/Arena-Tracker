#include "webuploader.h"
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtWidgets>


WebUploader::WebUploader(QObject *parent, QMap<QString, QJsonObject> *cardsJson) : QObject(parent)
{
    match = new QRegularExpressionMatch();
    webState = signup;
    arenaCurrentID = 0;
    deckInWeb = false;
    gameResultPostList = new QList<GameResultPost>();
    rewardsPost = NULL;
    this->cardsJson = cardsJson;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    connectWeb();
}

WebUploader::~WebUploader()
{
    delete networkManager;
    delete gameResultPostList;
    delete match;
}


void WebUploader::connectWeb()
{
    QString playerEmail, password;
    readSettings(playerEmail, password);

    QURL postData;
    postData.addQueryItem("signin", "1");
    postData.addQueryItem("playerEmail", playerEmail);
    postData.addQueryItem("password", password);

    QNetworkRequest request(QUrl(WEB_URL + "index.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    postRequest(request, postData);
}


void WebUploader::readSettings(QString &playerEmail, QString &password)
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    playerEmail = settings.value("playerEmail", "").toString();
    password = settings.value("password", "").toString();

    if(playerEmail.isEmpty() || password.isEmpty())
    {
        askLoginData(playerEmail, password);
    }
}


void WebUploader::askLoginData(QString &playerEmail, QString &password)
{
    bool ok;
    playerEmail = QInputDialog::getText(0, tr("Arena Mastery Login"),
                                         tr("User email:"), QLineEdit::Normal,
                                         playerEmail, &ok);

    if(ok)
    {
        password = QInputDialog::getText(0, tr("Arena Mastery Login"),
                                             tr("Password:"), QLineEdit::Password,
                                             password, &ok);
    }

    if(!ok)
    {
        playerEmail = "";
        password = "";
    }

    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("playerEmail", playerEmail);
    settings.setValue("password", password);
}


bool WebUploader::uploadNewGameResult(GameResult &gameresult, QList<DeckCard> *deckCardList)
{
    if(arenaCurrentID == 0 && (webState != createArena))
    {
        qDebug() << "WebUploader: " << "WARNING: Uploading juego sin haber arena en progreso en la web.";
        emit sendLog(tr("Web: WARNING:No arena in progress to upload to for the game.\n"
                        "(Create the arena manually in Arena Mastery and refresh the app)."));
        return false;
    }
    if(arenaCurrentHero.compare(gameresult.playerHero) != 0)
    {
        qDebug() << "WebUploader: " << "WARNING: Uploading juego de un heroe distinto al de la arena en progreso.";
        emit sendLog(tr("Web: WARNING:Arena in progress has a different hero.\n"
                        "(Go to Arena Mastery, fix it and refresh the app)."));
        return false;
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
        qDebug() << "WebUploader: " << "Reavivamos la conexion.";
    }

    QURL postData;
    postData.addQueryItem("action", "new");
    postData.addQueryItem("opClassId", heroToWebNumber(gameresult.enemyHero));
    postData.addQueryItem("result", gameresult.isWinner?"1":"0");
    postData.addQueryItem("play", gameresult.isFirst?"1":"0");
    if(webState != createArena)
    {
        postData.addQueryItem("arena", QString::number(arenaCurrentID));
    }

    QNetworkRequest request(QUrl(WEB_URL + "arena_gameupdate_ajax.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");

    GameResultPost gamePost;
    gamePost.postData = postData;
    gamePost.request = request;
    gameResultPostList->append(gamePost);
    qDebug() << "WebUploader: " << "Juego nuevo esperando para upload.";

    if(!deckInWeb && arenaCards.isEmpty() && deckCardList!=NULL)
    {
        createArenaCards(*deckCardList);
    }

    return true;
}


void WebUploader::createArenaCards(QList<DeckCard> &deckCardList)
{
    for (QList<DeckCard>::const_iterator it = deckCardList.cbegin(); it != deckCardList.cend(); it++)
    {
        if(it->total > 0)
        {
            QString name = (*cardsJson)[it->code].value("name").toString();
            arenaCards.append(QString::number(it->total) + " " + name + "\n");
        }
    }

    qDebug() << "Building Deck string: "<< endl << arenaCards;
}


bool WebUploader::uploadNewArena(const QString &hero)
{
    if(arenaCurrentID != 0 || webState == createArena)
    {
        qDebug() << "WebUploader: " << "WARNING: Uploading nueva Arena con una en progreso en la web.";
        emit sendLog(tr("Web: WARNING:Arena in progress.\n"
                        "(Go to Arena Mastery, fix it and refresh the app)."));
        return false;
    }

//    //Reavivamos la conexion
//    connectWeb();
//    qDebug() << "WebUploader: " << "Reavivamos la conexion.";

    networkManager->get(QNetworkRequest(QUrl(WEB_URL + "arena.php?new=" + heroToWebNumber(hero))));
    arenaCurrentHero = QString(hero);
    webState = createArena;
    qDebug() << "WebUploader: " << "Arena nueva pedida a la web.";
    return true;
}


bool WebUploader::uploadArenaRewards(ArenaRewards &arenaRewards)
{
    if(arenaCurrentID == 0 && (webState != createArena))
    {
        qDebug() << "WebUploader: " << "WARNING: Uploading rewards sin haber arena en progreso en la web.";
        emit sendLog(tr("Web: WARNING:No arena in progress to upload to for the rewards."));
        return false;
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
        qDebug() << "WebUploader: " << "Reavivamos la conexion.";
    }

    QURL postData;
    postData.addQueryItem("action", "rewards");
    postData.addQueryItem("rewards", "1");
    postData.addQueryItem("gold", QString::number(arenaRewards.gold));
    postData.addQueryItem("dust", QString::number(arenaRewards.dust));
    postData.addQueryItem("card", QString::number(arenaRewards.goldCards));
    postData.addQueryItem("reg", QString::number(arenaRewards.plainCards));
    postData.addQueryItem("pack", QString::number(arenaRewards.packs));
    if(webState != createArena)
    {
        postData.addQueryItem("arena", QString::number(arenaCurrentID));
    }

    QNetworkRequest request(QUrl(WEB_URL + "arena_update_ajax.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");

    if(rewardsPost == NULL)
    {
        rewardsPost = new GameResultPost();
        rewardsPost->postData = postData;
        rewardsPost->request = request;
        qDebug() << "WebUploader: " << "Rewards esperando para upload.";
    }
    else
    {
        qDebug() << "WebUploader: " << "ERROR: Existian rewards pendientes al crear nuevos.";
        return false;
    }


    return true;
}


void WebUploader::replyFinished(QNetworkReply *reply)
{
    qDebug() << "WebUploader: " << "ReplyFinished. " + QString::number(webState);
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "WebUploader: " << "ERROR: accediendo a Arena Mastery.";
        emit sendLog(tr("Web: No internet access to Arena Mastery."));
        return;
    }

    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
    { 
        if(webState == signup)
        {
            qDebug() << "WebUploader: " << "Sign up en arena Mastery correcto.";
            emit sendLog(tr("Web: Arena mastery sign up success."));
            webState = checkArenaCurrentLoad;
        }
        else if(webState == createArena)
        {
            if(QString(reply->rawHeader("Location")).contains(QRegularExpression(
                "arena\\.php\\?arena=(\\d+)"
                ), match))
            {
                arenaCurrentID = match->captured(1).toInt();
                deckInWeb = false;
                qDebug() << "WebUploader: " << "Arena nueva uploaded(" << match->captured(1) << "). Heroe: " << arenaCurrentHero;
                emit sendLog(tr("Web: New arena uploaded."));
                webState = complete;
            }
        }

        qDebug() << "WebUploader: " << "Redirigido a " << reply->rawHeader("Location");
        networkManager->get(QNetworkRequest(QUrl(WEB_URL + reply->rawHeader("Location"))));
    }
    else if(webState == signup)
    {
        qDebug() << "WebUploader: " << "WARNING: Usuario o password incorrecto.";

        QSettings settings("Arena Tracker", "Arena Tracker");
        QString playerEmail = settings.value("playerEmail", "").toString();
        QString password = settings.value("password", "").toString();

        askLoginData(playerEmail, password);
        if(!playerEmail.isEmpty() && !password.isEmpty())
        {
            connectWeb();
        }
    }
    else if(webState == checkArenaCurrentLoad)
    {
        //Ejemplo html
        //Arena en progreso
        //<a href="arena.php?arena=482647" class="btn btn-lg btn-success startArena">Continue your arena
        //        <span class="glyphicon glyphicon-chevron-right"></span><br>
        //        </a>

        //Sin arena en progreso
        //<button class="btn btn-lg btn-success startArena" data-toggle="modal" data-target="#newArena">
        //        <span class="glyphicon glyphicon-plus"></span> Enter a new arena
        //        </button>

        qDebug() << "WebUploader: " << "Check arena actual";

        if(QString(reply->readAll()).contains(QRegularExpression(
            "<a href=.(arena\\.php\\?arena=(\\d+)). class=.btn btn-lg btn-success startArena.>"
            ), match))
        {
            networkManager->get(QNetworkRequest(QUrl(WEB_URL + match->captured(1))));
            qDebug() << "WebUploader: " << "Existe una arena en progreso.";
            arenaCurrentID = match->captured(2).toInt();
            webState = loadArenaCurrent;
        }
        else
        {
            qDebug() << "WebUploader: " << "Ninguna arena en progreso.";
            arenaCurrentID = 0;
            webState = complete;
            emit synchronized();
            emit noArenaFound();
        }
    }
    else if(webState == loadArenaCurrent)
    {
        QList<GameResult> list;
        if(getArenaCurrentAndGames(reply, list, true))
        {
            emit loadedArena(arenaCurrentHero);

            while(!list.empty())
            {
                emit loadedGameResult(list.first());
                list.removeFirst();
            }

            webState = complete;
            emit synchronized();
        }
        else
        {
            qDebug() << "WebUploader: " << "ERROR: al buscar la arena en progreso.";
        }
    }
    else if(webState == checkArenaCurrentReload1)
    {
        checkArenaCurrentReload();
    }
    else if(webState == checkArenaCurrentReload2)
    {
        qDebug() << "WebUploader: " << "Check arena actual";

        if(QString(reply->readAll()).contains(QRegularExpression(
            "<a href=.(arena\\.php\\?arena=(\\d+)). class=.btn btn-lg btn-success startArena.>"
            ), match))
        {
            networkManager->get(QNetworkRequest(QUrl(WEB_URL + match->captured(1))));
            qDebug() << "WebUploader: " << "Existe una arena en progreso.";
            arenaCurrentID = match->captured(2).toInt();
            webState = reloadArenaCurrent2;
        }
        else
        {
            qDebug() << "WebUploader: " << "Ninguna arena en progreso.";
            arenaCurrentID = 0;
            webState = complete;
            emit noArenaFound();
        }
    }
    else if(webState == reloadArenaCurrent1)
    {
        networkManager->get(QNetworkRequest(QUrl(WEB_URL + "arena.php?arena=" + QString::number(arenaCurrentID))));
        qDebug() << "WebUploader: " << "Recargamos la arena en progreso: " + QString::number(arenaCurrentID);
        webState = reloadArenaCurrent2;
    }
    else if(webState == reloadArenaCurrent2)
    {
        QList<GameResult> list;
        if(getArenaCurrentAndGames(reply, list))
        {
            emit reloadedArena(arenaCurrentHero);

            while(!list.empty())
            {
                emit reloadedGameResult(list.first());
                list.removeFirst();
            }

            webState = complete;
            uploadNext();
        }
        else
        {
            qDebug() << "WebUploader: " << "ERROR: al buscar la arena en progreso.";
        }
    }
    else if(webState == complete)
    {
        qDebug() << "WebUploader: " << "UploadNext.";
        uploadNext();
    }
}


void WebUploader::uploadNext()
{
    if(!arenaCards.isEmpty())
    {
        uploadArenaCards();
    }
    else if(!gameResultPostList->isEmpty())
    {
        GameResultPost gamePost = gameResultPostList->first();
        gameResultPostList->removeFirst();

        if(!gamePost.postData.hasQueryItem("arena"))
        {
            gamePost.postData.addQueryItem("arena", QString::number(arenaCurrentID));
        }
        postRequest(gamePost.request, gamePost.postData);
        qDebug() << "WebUploader: " << "Juego nuevo uploaded.";
        emit sendLog(tr("Web: New game uploaded."));
        webState = reloadArenaCurrent1;
    }
    else if(rewardsPost != NULL)
    {
        if(!rewardsPost->postData.hasQueryItem("arena"))
        {
            rewardsPost->postData.addQueryItem("arena", QString::number(arenaCurrentID));
        }
        postRequest(rewardsPost->request, rewardsPost->postData);
        qDebug() << "WebUploader: " << "Rewards uploaded.";
        emit sendLog(tr("Web: New rewards uploaded."));
        webState = checkArenaCurrentReload1;

        delete rewardsPost;
        rewardsPost = NULL;
    }
}


void WebUploader::uploadArenaCards()
{
    QURL postData;
    postData.addQueryItem("batchImportPwn", "1");
    postData.addQueryItem("importReviewed", "1");
    postData.addQueryItem("importDataBlock", arenaCards);

    QNetworkRequest request(QUrl(WEB_URL + "import_cards.php?arena=" + QString::number(arenaCurrentID)));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    postRequest(request, postData);

    arenaCards = "";
    deckInWeb = true;
    qDebug() << "WebUploader: " << "Complete deck uploaded.";
    emit sendLog(tr("Web: Complete deck uploaded."));
}


void WebUploader::checkArenaCurrentReload()
{
    networkManager->get(QNetworkRequest(QUrl(WEB_URL + "player.php")));
    qDebug() << "WebUploader: " << "Recargamos player";
    webState = checkArenaCurrentReload2;
}

bool WebUploader::getArenaCurrentAndGames(QNetworkReply *reply, QList<GameResult> &list, bool getCards)
{
    //Ejemplo html
    //<div class='col-md-5' id='nameDate'>
    //<h1><img src='images/32icon-dru.gif' /> Druid Arena</h1>

    QString html(reply->readAll());
    if(html.contains(QRegularExpression(
        "<div.*id='nameDate'>\n<h1>.*"
        "(Druid|Hunter|Mage|Paladin|Priest|Rogue|Shaman|Warlock|Warrior)"
        " Arena</h1>"
        ), match))
    {
        arenaCurrentHero = heroToLogNumber(match->captured(1));


        //Ejemplo html
        //<div id='gameListArea'><ul class='list-group'>
        //<a name='gameList'></a><li class='list-group-item match-list'><span class='glyphicon glyphicon-remove fail matchStatus pull-left'></span> <span class='helpText matchSupLabel'>Game 2:</span><a href='#' onClick="showGame(2771882)" data-toggle='modal' data-target='#showGame'><span class='matchLabel'>vs Mage</span> </a><span class='helpText'>(<span class='glyphicon glyphicon-plus'></span> Extra card)</span><div class='pull-right'><a onClick="showGame(2771882)" data-toggle='modal' data-target='#showGame' class='btn btn-xs btn-info'><span class='glyphicon glyphicon-paperclip'></span> Game notes</a>  <a onClick="deleteGame(2771882)" href='#' class='btn btn-xs btn-danger'><span class='glyphicon glyphicon-trash'></span></a></div></li>
        //                       <li class='list-group-item match-list'><span class='glyphicon glyphicon-ok success matchStatus pull-left'></span> <span class='helpText matchSupLabel'>Game 1:</span><a href='#' onClick="showGame(2771880)" data-toggle='modal' data-target='#showGame'><span class='matchLabel'>vs Rogue</span> </a><span class='helpText'>(<span class='glyphicon glyphicon-play-circle'></span> Played first)</span><div class='pull-right'><a onClick="showGame(2771880)" data-toggle='modal' data-target='#showGame' class='btn btn-xs btn-info'><span class='glyphicon glyphicon-paperclip'></span> Game notes</a>  <a onClick="deleteGame(2771880)" href='#' class='btn btn-xs btn-danger'><span class='glyphicon glyphicon-trash'></span></a></div></li>
        //</ul>
        //</div></div>

        QRegularExpression re(
            "<li.*>.*(fail|success).*Game (\\d+).*"
            "vs (Druid|Hunter|Mage|Paladin|Priest|Rogue|Shaman|Warlock|Warrior).*"
            "(Extra card|Played first).*</li>");
        QRegularExpressionMatchIterator reIterator = re.globalMatch(html);

        while (reIterator.hasNext())
        {
            QRegularExpressionMatch match = reIterator.next();
            list.prepend(createGameResult(match, arenaCurrentHero));
        }
        qDebug() << "WebUploader: " << "Leida arena en progreso: " << QString(match->captured(1)) <<
                    " con " << QString::number(list.count()) << " juegos";

        if(getCards)
        {
            GetArenaCards(html);
        }
        return true;
    }
    else
    {
        return false;
    }
}


void WebUploader::GetArenaCards(QString &html)
{
    //Ejemplo html
    //<li><a href='#deck' data-toggle='tab'>Cards: List & Info</a></li>

    if(html.contains("<li><a href='#deck' data-toggle='tab'>Cards: List & Info</a></li>"))
    {
        deckInWeb = true;
        emit newWebDeckCardList();
        qDebug() << "WebUploader: "<< "Inicio leer deck.";

        //Ejemplo html carta
        //<li id='374' class='list-group-item' data-name='1' data-cost='3' data-total='1' data-remaining='1' data-any='1'>
        //<span style='display: inline-block;'>(3) <a href='http://www.hearthpwn.com/cards/428' onClick='return false;'>Acolyte of Pain</a>
        //</span> (<span id='remaining-374' style='font-weight:bold;'>1</span> copy)</li>
        QRegularExpression re(
            "<li id='\\d+' class='list-group-item' data-name='\\d+' data-cost='\\d+' data-total='(\\d+)' data-remaining='\\d+' data-any='\\d+'>"
            "<span style='display: inline-block;'>.*<a href=.*onClick=.*>(.+)</a> "
            "</span>.*</li>");
        QRegularExpressionMatchIterator reIterator = re.globalMatch(html);

        while (reIterator.hasNext())
        {
            QRegularExpressionMatch match = reIterator.next();
            emit newDeckCard(codeFromName(match.captured(2)), match.captured(1).toInt());
            qDebug() << "WebUploader: "<< "Nueva carta:" << match.captured(1) << match.captured(2);
        }
        qDebug() << "WebUploader: "<< "Final leer deck.";
        emit sendLog(tr("Web: Active deck read."));
    }
}


QString WebUploader::codeFromName(QString name)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toString() == name)
        {
            if(!it->value("cost").isUndefined())    return it.key();
        }
    }
    qDebug() << "WebUploader: " << "ERROR: No se encuentra en Json codigo de carta: " << name;
    emit sendLog(tr("JSon: ERROR: Code for card name not found in Json: ") + name);
    return "";
}


QString WebUploader::heroToLogNumber(const QString &hero)
{
    if(hero.compare("Druid")==0)        return QString("06");
    else if(hero.compare("Hunter")==0)  return QString("05");
    else if(hero.compare("Mage")==0)    return QString("08");
    else if(hero.compare("Paladin")==0) return QString("04");
    else if(hero.compare("Priest")==0)  return QString("09");
    else if(hero.compare("Rogue")==0)   return QString("03");
    else if(hero.compare("Shaman")==0)  return QString("02");
    else if(hero.compare("Warlock")==0) return QString("07");
    else if(hero.compare("Warrior")==0) return QString("01");
    else return QString();
}


QString WebUploader::heroToWebNumber(const QString &hero)
{
    if(hero.compare("06")==0)           return QString("1");
    else if(hero.compare("05")==0)      return QString("2");
    else if(hero.compare("08")==0)      return QString("3");
    else if(hero.compare("04")==0)      return QString("4");
    else if(hero.compare("09")==0)      return QString("5");
    else if(hero.compare("03")==0)      return QString("6");
    else if(hero.compare("02")==0)      return QString("7");
    else if(hero.compare("07")==0)      return QString("8");
    else if(hero.compare("01")==0)      return QString("9");
    else return QString();
}


GameResult WebUploader::createGameResult(const QRegularExpressionMatch &match, const QString &arenaCurrentHero)
{
    GameResult gameResult;

    gameResult.playerHero = arenaCurrentHero;
    gameResult.enemyHero = heroToLogNumber(match.captured(3));
    gameResult.isFirst = match.captured(4).startsWith("P");
    gameResult.isWinner = match.captured(1).startsWith("s");

    return gameResult;
}


void WebUploader::postRequest(QNetworkRequest request, QURL postData)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
#else
    networkManager->post(request, postData.encodedQuery());
#endif
}

