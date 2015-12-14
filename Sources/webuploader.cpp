#include "webuploader.h"
#include "utility.h"
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtWidgets>

WebUploader::WebUploader(QObject *parent) : QObject(parent)
{
    match = new QRegularExpressionMatch();
    webState = signup;
    arenaCurrentID = 0;
    lastArenaID = 0;
    deckInWeb = false;
    gameResultPostList = new QList<GameResultPost>();
    rewardsPost = NULL;
    playerEmail = password = "";
    isConnected = false;

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

//    connectWeb();
}

WebUploader::~WebUploader()
{
    delete networkManager;
    delete gameResultPostList;
    delete match;
}


void WebUploader::tryConnect(QString playerEmail, QString password)
{
    this->playerEmail = playerEmail;
    this->password = password;

    delete networkManager->cookieJar();
    networkManager->setCookieJar(new QNetworkCookieJar());

    webState = signup;
    connectWeb();
}


void WebUploader::connectWeb()
{
//    QString playerEmail, password;
//    readSettings(playerEmail, password);

    QURL postData;
    postData.addQueryItem("signin", "1");
    postData.addQueryItem("playerEmail", playerEmail);
    postData.addQueryItem("password", password);

    QNetworkRequest request(QUrl(WEB_URL + "index.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    postRequest(request, postData);

    emit pDebug("\nLogin web.");
}


//void WebUploader::readSettings(QString &playerEmail, QString &password)
//{
//    QSettings settings("Arena Tracker", "Arena Tracker");
//    playerEmail = settings.value("playerEmail", "").toString();
//    password = settings.value("password", "").toString();

//    if(playerEmail.isEmpty() || password.isEmpty())
//    {
//        askLoginData(playerEmail, password);
//    }
//}


//void WebUploader::askLoginData(QString &playerEmail, QString &password)
//{
//    bool ok;
//    playerEmail = QInputDialog::getText(0, tr("Arena Mastery Login"),
//                                         tr("User email:"), QLineEdit::Normal,
//                                         playerEmail, &ok);

//    if(ok)
//    {
//        password = QInputDialog::getText(0, tr("Arena Mastery Login"),
//                                             tr("Password:"), QLineEdit::Password,
//                                             password, &ok);
//    }

//    if(!ok)
//    {
//        playerEmail = "";
//        password = "";
//    }

//    QSettings settings("Arena Tracker", "Arena Tracker");
//    settings.setValue("playerEmail", playerEmail);
//    settings.setValue("password", password);
//}


bool WebUploader::uploadNewGameResult(GameResult &gameresult, QList<DeckCard> *deckCardList)
{
    if(arenaCurrentID == 0 && (webState != createArena))
    {
        emit pDebug("No arena in progress to upload to for the game.", Warning);
        emit pLog(tr("Web: WARNING:No arena in progress to upload to for the game.\n"
                        "(Create the arena manually in Arena Mastery and refresh the app)."));
        return false;
    }
    if(arenaCurrentHero.compare(gameresult.playerHero) != 0)
    {
        emit pDebug("Arena in progress has a different hero.", Warning);
        emit pLog(tr("Web: WARNING:Arena in progress has a different hero.\n"
                        "(Go to Arena Mastery, fix it and refresh the app)."));
        return false;
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
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
    emit pDebug("New game waiting for upload.");

    if(!deckInWeb && arenaCards.isEmpty() && deckCardList!=NULL)
    {
        createArenaCards(*deckCardList);
    }

    return true;
}


void WebUploader::uploadDeck(QList<DeckCard> *deckCardList)
{
    if(arenaCurrentID == 0 && (webState != createArena))
    {
        emit pDebug("No arena in progress to upload to for the deck.", Warning);
        emit pLog(tr("Web: WARNING:No arena in progress to upload to for the deck.\n"
                        "(Create the arena manually in Arena Mastery and refresh the app)."));
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
    }

    if(!deckInWeb && arenaCards.isEmpty() && deckCardList!=NULL)
    {
        createArenaCards(*deckCardList);
    }
}


void WebUploader::createArenaCards(QList<DeckCard> &deckCardList)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->total > 0)
        {
            QString name = Utility::cardEnNameFromCode(it->getCode());
            arenaCards.append(QString::number(it->total) + " " + name + "\n");
        }
    }

    emit pDebug("Building Deck string:\n" + arenaCards);
    emit pDebug("New deck waiting for upload.");
}


bool WebUploader::uploadNewArena(const QString &hero)
{
    if(arenaCurrentID != 0 || webState == createArena)
    {
        emit pDebug("Arena in progress when trying to upload a new one.", Warning);
        emit pLog(tr("Web: WARNING:Arena in progress when trying to upload a new one.\n"
                        "(Go to Arena Mastery, fix it and refresh the app)."));
        return false;
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
    }

    arenaCurrentHero = QString(hero);
    webState = createArena;

    emit pDebug("New arena waiting for upload.");
    return true;
}


bool WebUploader::uploadArenaRewards(ArenaRewards &arenaRewards)
{
    if(lastArenaID == 0 && (webState != createArena))
    {
        emit pDebug("No arena in progress to upload to for the rewards.", Warning);
        emit pLog(tr("Web: WARNING:No arena in progress to upload to for the rewards."));
        return false;
    }

    if(webState == complete && gameResultPostList->isEmpty())
    {
        //Reavivamos la conexion
        connectWeb();
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
        postData.addQueryItem("arena", QString::number(lastArenaID));
    }

    QNetworkRequest request(QUrl(WEB_URL + "arena_update_ajax.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");

    if(rewardsPost == NULL)
    {
        rewardsPost = new GameResultPost();
        rewardsPost->postData = postData;
        rewardsPost->request = request;
        emit pDebug("Rewards waiting for upload.");
    }
    else
    {
        emit pDebug("Old rewards waiting for upload when creating the new ones.", Error);
        return false;
    }


    return true;
}


void WebUploader::showWebState()
{
    switch(webState)
    {
        case signup:
            emit pDebug("ReplyFinished: signup");
            break;
        case checkArenaCurrentLoad:
            emit pDebug("ReplyFinished: checkArenaCurrentLoad");
            break;
        case loadArenaCurrent:
            emit pDebug("ReplyFinished: loadArenaCurrent");
            break;
        case createArena:
            emit pDebug("ReplyFinished: createArena");
            break;
        case rewardsSent:
            emit pDebug("ReplyFinished: rewardsSent");
            break;
        case checkArenaCurrentReload:
            emit pDebug("ReplyFinished: checkArenaCurrentReload");
            break;
        case gameResultSent:
            emit pDebug("ReplyFinished: gameResultSent");
            break;
        case reloadArenaCurrent:
            emit pDebug("ReplyFinished: reloadArenaCurrent");
            break;
        case getArenaDeck1:
            emit pDebug("ReplyFinished: getArenaDeck1");
            break;
        case getArenaDeck2:
            emit pDebug("ReplyFinished: getArenaDeck2");
            break;
        case complete:
            emit pDebug("ReplyFinished: complete");
            break;
    }
}


void WebUploader::replyFinished(QNetworkReply *reply)
{
    showWebState();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        if(webState == rewardsSent) webState = complete;
        if(webState == gameResultSent) webState = complete;

        emit pDebug("No internet access to Arena Mastery.", Error);
        emit pLog(tr("Web: No internet access to Arena Mastery."));
        emit synchronized();
        QTimer::singleShot(10000, this, SLOT(connectWeb()));
        return;
    }

    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
    { 
        if(webState == signup)
        {
            emit pDebug("Arena mastery sign up success.");
            emit pLog(tr("Web: Arena mastery sign up success."));

            //Si ya hemos cargado la ultima arena hacemos un reload
            if(!isConnected)    webState = checkArenaCurrentLoad;
            else                webState = checkArenaCurrentReload;

            isConnected = true;
            emit connectionTried(true);
        }
        else if(webState == createArena)
        {
            if(QString(reply->rawHeader("Location")).contains(QRegularExpression(
                "arena\\.php\\?arena=(\\d+)"
                ), match))
            {
                arenaCurrentID = match->captured(1).toInt();
                lastArenaID = arenaCurrentID;
                deckInWeb = false;
                emit pDebug("New arena uploaded(" + match->captured(1) + "). Heroe: " + arenaCurrentHero);
                emit pLog(tr("Web: New arena uploaded."));
                checkArenaReload();
            }
        }

        emit pDebug("Redirected to " + reply->rawHeader("Location"));
        networkManager->get(QNetworkRequest(QUrl(WEB_URL + reply->rawHeader("Location"))));
    }
    else if(webState == signup)
    {
        emit pDebug("Wrong username or password.", Warning);
        emit pLog(tr("Web: Arena mastery sign up failed. Wrong username or password."));
        isConnected = false;
        emit connectionTried(false);

//        QSettings settings("Arena Tracker", "Arena Tracker");
//        QString playerEmail = settings.value("playerEmail", "").toString();
//        QString password = settings.value("password", "").toString();

//        askLoginData(playerEmail, password);
//        if(!playerEmail.isEmpty() && !password.isEmpty())
//        {
//            connectWeb();
//        }
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

        emit pDebug("Check arena in progress.");

        if(QString(reply->readAll()).contains(QRegularExpression(
            "<a href=.(arena\\.php\\?arena=(\\d+)). class=.btn btn-lg btn-success startArena.>"
            ), match))
        {
            networkManager->get(QNetworkRequest(QUrl(WEB_URL + match->captured(1))));
            emit pDebug("Arena in progress found.");
            arenaCurrentID = match->captured(2).toInt();
            lastArenaID = arenaCurrentID;
            webState = loadArenaCurrent;
        }
        else
        {
            emit pDebug("There's no arena in progress.");
            emit noArenaFound();
            arenaCurrentID = 0;
            webState = complete;
            uploadNext();
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

            emit loadArenaCurrentFinished();    //ArenaHandler removeDuplicateArena()
            checkArenaReload();
        }
        else
        {
            emit pDebug("Failed analysing arena in web.", Error);
            emit pLog(tr("Web: ERROR: Failed analysing arena in web."));
            webState = complete;
            uploadNext();
        }
    }
    else if(webState == rewardsSent)
    {
        emit pDebug("Rewards uploaded.");
        emit pLog(tr("Web: New rewards uploaded."));

        delete rewardsPost;
        rewardsPost = NULL;
        webState = complete;
        uploadNext();
    }
    else if(webState == checkArenaCurrentReload)
    {
        emit pDebug("Check arena in progress.");

        if(QString(reply->readAll()).contains(QRegularExpression(
            "<a href=.(arena\\.php\\?arena=(\\d+)). class=.btn btn-lg btn-success startArena.>"
            ), match))
        {
            networkManager->get(QNetworkRequest(QUrl(WEB_URL + match->captured(1))));
            emit pDebug("Arena in progress found.");
            arenaCurrentID = match->captured(2).toInt();
            lastArenaID = arenaCurrentID;
            webState = reloadArenaCurrent;
        }
        else
        {
            emit pDebug("There's no arena in progress.");
            emit noArenaFound();
            arenaCurrentID = 0;
            webState = complete;
            uploadNext();
        }
    }
    else if(webState == gameResultSent)
    {
        emit pDebug("New game uploaded.");
        emit pLog(tr("Web: New game uploaded."));

        gameResultPostList->removeFirst();

        checkArenaReload();
    }
    else if(webState == reloadArenaCurrent)
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
            emit pDebug("Failed analysing arena in web.", Error);
            emit pLog(tr("Web: ERROR: Failed analysing arena in web."));
            webState = complete;
            uploadNext();
        }
    }
    else if(webState == createArena)
    {
        networkManager->get(QNetworkRequest(QUrl(WEB_URL + "arena.php?new=" + heroToWebNumber(arenaCurrentHero))));
        emit pDebug("New arena asked to web.");
    }
    else if(webState == getArenaDeck1)
    {
        if(arenaCurrentID != 0)
        {
            networkManager->get(QNetworkRequest(QUrl(WEB_URL + "arena.php?arena=" + QString::number(arenaCurrentID))));
            emit pDebug("Arena in progress found.");
            webState = getArenaDeck2;
        }
        else
        {
            emit pDebug("No arena in progress.");
            webState = complete;
            uploadNext();
        }
    }
    else if(webState == getArenaDeck2)
    {
        emit pDebug("Get arena deck.");
        QString html(reply->readAll());
        getArenaCards(html);
        webState = complete;
        uploadNext();
    }
    else if(webState == complete)
    {
        uploadNext();
    }
}


void WebUploader::uploadNext()
{
    emit pDebug("UploadNext.");

    if(!arenaCards.isEmpty())
    {
        uploadArenaCards();
    }
    else if(!gameResultPostList->isEmpty())
    {
        GameResultPost gamePost = gameResultPostList->first();

        if(!gamePost.postData.hasQueryItem("arena"))
        {
            gamePost.postData.addQueryItem("arena", QString::number(arenaCurrentID));
        }
        postRequest(gamePost.request, gamePost.postData);
        emit pDebug("New game sent to web.");
        webState = gameResultSent;
    }
    else if(rewardsPost != NULL)
    {
        if(!rewardsPost->postData.hasQueryItem("arena"))
        {
            rewardsPost->postData.addQueryItem("arena", QString::number(lastArenaID));
        }
        postRequest(rewardsPost->request, rewardsPost->postData);
        emit pDebug("New rewards sent to web.");
        webState = rewardsSent;
    }
    else
    {
        emit synchronized();
    }
}


void WebUploader::uploadArenaCards()
{
    QURL postData;
    postData.addQueryItem("batchImportPwn", "1");
    postData.addQueryItem("importReviewed", "1");
    postData.addQueryItem("importDataBlock", arenaCards);

    QNetworkRequest request(QUrl(WEB_URL + "import_cards.php?arena=" + QString::number(lastArenaID)));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    postRequest(request, postData);

    arenaCards = "";
    deckInWeb = true;
    emit pDebug("Complete deck uploaded.");
    emit pLog(tr("Web: Complete deck uploaded."));
}


void WebUploader::refresh()
{
    if(webState == complete && gameResultPostList->isEmpty())
    {
        connectWeb();
        webState = checkArenaCurrentReload;
    }
}


void WebUploader::checkArenaReload()
{
    networkManager->get(QNetworkRequest(QUrl(WEB_URL + "player.php")));
    emit pDebug("Reload player.php");
    webState = checkArenaCurrentReload;
}


void WebUploader::askArenaCards()
{
    if(webState == complete && gameResultPostList->isEmpty())
    {
        connectWeb();
        webState = getArenaDeck1;
    }
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
        arenaCurrentHero = Utility::heroToLogNumber(match->captured(1));


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
        emit pDebug("Read arena in progress: " + QString(match->captured(1)) +
                    " with " + QString::number(list.count()) + " games.");

        if(getCards)
        {
            getArenaCards(html);
        }
        return true;
    }
    else
    {
        return false;
    }
}


void WebUploader::getArenaCards(QString &html)
{
    //Ejemplo html
    //<li><a href='#deck' data-toggle='tab'>Cards: List & Info</a></li>

    if(html.contains("<li><a href='#deck' data-toggle='tab'>Cards: List & Info</a></li>"))
    {
        deckInWeb = true;
        emit newWebDeckCardList();
        emit pDebug("Start reading deck.");

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

            QString name = match.captured(2);
            QString code = Utility::cardEnCodeFromName(name);
            if(code.isEmpty())
            {
                emit pDebug("Code for card name not found in Json: " + name, Error);
                emit pLog(tr("JSon: ERROR: Code for card name not found in Json: ") + name);
            }
            else    emit newDeckCard(code, match.captured(1).toInt());
        }
        emit pDebug("End reading deck.");
        emit pLog(tr("Web: Active deck read."));
    }
    else
    {
        emit pDebug("No deck in web.");
    }
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
    gameResult.enemyHero = Utility::heroToLogNumber(match.captured(3));
    gameResult.isFirst = match.captured(4).startsWith("P");
    gameResult.isWinner = match.captured(1).startsWith("s");
    gameResult.enemyName = "";

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

