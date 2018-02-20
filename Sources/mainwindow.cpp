#include "mainwindow.h"
#include "Widgets/ui_extended.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include "Widgets/cardwindow.h"
#include "versionchecker.h"
#include "themehandler.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::Extended)
{
    QFontDatabase::addApplicationFont(":Fonts/hsFont.ttf");
    QFontDatabase::addApplicationFont(":Fonts/LuckiestGuy.ttf");

    ui->setupUi(this);

    atLogFile = NULL;
    mouseInApp = false;
    deckWindow = NULL;
    arenaWindow = NULL;
    enemyWindow = NULL;
    enemyDeckWindow = NULL;
    planWindow = NULL;
    copyGameLogs = false;
    draftLogFile = "";
    cardHeight = -1;

    logLoader = NULL;
    gameWatcher = NULL;
    arenaHandler = NULL;
    cardDownloader = NULL;
    enemyHandHandler = NULL;
    draftHandler = NULL;
    deckHandler = NULL;
    enemyDeckHandler = NULL;
    secretsHandler = NULL;
    trackobotUploader = NULL;
    premiumHandler = NULL;

    createNetworkManager();
    createDataDir();
    createLogFile();
    completeUI();
    initCardsJson();
    downloadLightForgeJson();
    downloadHearthArenaVersion();
    downloadSynergiesVersion();
    downloadExtraFiles();
    downloadThemes();

    createTrackobotUploader();
    createCardDownloader();
    createPlanHandler();
    createEnemyHandHandler();//-->PlanHandler
    createEnemyDeckHandler();
    createDeckHandler();//-->EnemyDeckHandler
    createDraftHandler();//-->CardDownloader
    createSecretsHandler();//-->EnemyHandHandler
    createArenaHandler();//-->DeckHandler -->TrackobotUploader -->PlanHandler
    createGameWatcher();//-->A lot
    createLogLoader();//-->GameWatcher -->DraftHandler
    createCardWindow();//-->A lot
    createCardListWindow();//-->PlanHandler -->SecretsHandler -->DraftHandler
    createPremiumHandler();//-->ArenaHandler -->PlanHandler -->DraftHandler

    readSettings();
    checkGamesLogDir();
    checkFirstRunNewVersion();
    createVersionChecker();//Despues de createDataDir (removeHSDir) y checkFirstRunNewVersion() ya que reescribe el settings "runVersion"

    setAcceptDrops(true);

    QTimer::singleShot(1000, this, SLOT(init()));
}


MainWindow::~MainWindow()
{
    if(networkManager != NULL)      delete networkManager;
    if(premiumHandler != NULL)      delete premiumHandler;
    if(logLoader != NULL)           delete logLoader;
    if(gameWatcher != NULL)         delete gameWatcher;
    if(arenaHandler != NULL)        delete arenaHandler;
    if(cardDownloader != NULL)      delete cardDownloader;
    if(enemyDeckHandler != NULL)    delete enemyDeckHandler;
    if(enemyHandHandler != NULL)    delete enemyHandHandler;
    if(draftHandler != NULL)        delete draftHandler;
    if(deckHandler != NULL)         delete deckHandler;
    if(secretsHandler != NULL)      delete secretsHandler;
    if(trackobotUploader != NULL)   delete trackobotUploader;
    if(ui != NULL)                  delete ui;
    closeLogFile();
    QFontDatabase::removeAllApplicationFonts();
}


void MainWindow::init()
{
    spreadTransparency();
    trackobotUploader->checkAccount();
//    downloadAllArenaCodes();  //Connect en completeUI

#ifdef Q_OS_LINUX
    checkLinuxShortcut();
#endif

#ifdef QT_DEBUG
    test();
#endif
}


void MainWindow::createDetachWindow(int index, const QPoint& dropPoint)
{
    QWidget *paneWidget = ui->tabWidget->widget(index);
    createDetachWindow(paneWidget, dropPoint);
}


void MainWindow::createDetachWindow(QWidget *paneWidget, const QPoint& dropPoint)
{
    if(paneWidget != ui->tabArena && paneWidget != ui->tabEnemy && paneWidget != ui->tabDeck &&
            paneWidget != ui->tabEnemyDeck && paneWidget != ui->tabPlan)    return;

    DetachWindow *detachWindow = NULL;

    if(paneWidget == ui->tabArena)
    {
        detachWindow = new DetachWindow(paneWidget, "Games", this->transparency, dropPoint);
        arenaWindow = detachWindow;
    }
    else if(paneWidget == ui->tabEnemy)
    {
        detachWindow = new DetachWindow(paneWidget, "Hand", this->transparency, dropPoint);
        enemyWindow = detachWindow;
    }
    else if(paneWidget == ui->tabDeck)
    {
        detachWindow = new DetachWindow(paneWidget, "Deck", this->transparency, dropPoint);
        deckWindow = detachWindow;
        connect(deckWindow, SIGNAL(resized()),
                this, SLOT(spreadCorrectTamCard()));
    }
    else if(paneWidget == ui->tabEnemyDeck)
    {
        detachWindow = new DetachWindow(paneWidget, "Enemy Deck", this->transparency, dropPoint);
        enemyDeckWindow = detachWindow;
    }
    else /*if(paneWidget == ui->tabPlan)*/
    {
        detachWindow = new DetachWindow(paneWidget, "Replay", this->transparency, dropPoint);
        planWindow = detachWindow;
    }

    connect(ui->minimizeButton, SIGNAL(clicked()),
            detachWindow, SLOT(showMinimized()));
    connect(detachWindow, SIGNAL(closed(DetachWindow *, QWidget *)),
            this, SLOT(closedDetachWindow(DetachWindow *, QWidget *)));
    connect(detachWindow, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
    connect(detachWindow, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));

    updateMainUITheme();//Crea el fondo de la nueva ventana y sus botones
    resizeChecks();//Recoloca botones -X y reajusta tabBar size
    calculateMinimumWidth();//Recalcula minimumWidth de mainWindow
}


void MainWindow::closedDetachWindow(DetachWindow *detachWindow, QWidget *paneWidget)
{
    Q_UNUSED(paneWidget);
    disconnect(ui->minimizeButton, 0, detachWindow, 0);
    disconnect(detachWindow, 0, this, 0);

    if(detachWindow == deckWindow)      deckWindow = NULL;
    if(detachWindow == arenaWindow)     arenaWindow = NULL;
    if(detachWindow == enemyWindow)     enemyWindow = NULL;
    if(detachWindow == enemyDeckWindow) enemyDeckWindow = NULL;
    if(detachWindow == planWindow)
    {
        //Antes de hacer el close de la detach window se llama esta funcion.
        //Volver plan a su size normal si se cierra Plan
        resetSizePlan();
        planWindow = NULL;
    }

    updateMainUITheme();//Elimina el fondo de la ventana al unir la tab a mainWindow
    updateTabIcons();//Inserta el tab y los ordena
    resizeChecks();//Recoloca botones -X y reajusta tabBar size
    calculateMinimumWidth();//Recalcula minimumWidth de mainWindow
}


void MainWindow::resetDeckDontRead()
{
    resetDeck(true);
}


void MainWindow::resetDeck(bool deckRead)
{
    gameWatcher->setDeckRead(deckRead);
    deckHandler->reset();
}


QString MainWindow::getHSLanguage()
{
    QString lang = "";

    if(logLoader != NULL)
    {
        QDir dir(QFileInfo(logLoader->getLogConfigPath()).absolutePath() + "/Cache/UberText");
        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Time);


        switch (dir.count())
        {
        case 0:
            lang = "enUS";
            break;
        case 1:
            foreach(QString file, dir.entryList())
            {
                lang = file.mid(5,4);
            }
            break;
        default:
            QStringList files = dir.entryList();
            lang = files.takeFirst().mid(5,4);

            //Remove old languages files
            foreach(QString file, files)
            {
                dir.remove(file);
                pDebug(file + " removed.");
            }

            //Remove old image cards
            QDir dirHSCards(Utility::hscardsPath());
            dirHSCards.setFilter(QDir::Files);
            QStringList filters("*_*.png");
            dirHSCards.setNameFilters(filters);

            foreach(QString file, dirHSCards.entryList())
            {
                dirHSCards.remove(file);
                pDebug(file + " removed.");
            }

            break;
        }
    }


    if(lang != "enGB" && lang != "enUS" && lang != "esES" && lang != "esMX" &&
            lang != "deDE" && lang != "frFR" && lang != "itIT" &&
            lang != "plPL" && lang != "ptBR" && lang != "ruRU" &&
            lang != "koKR" && lang != "zhCN" && lang != "zhTW" &&
            lang != "jaJP" && lang != "thTH")
    {
        pDebug("Language: " + lang + " not supported. Using enUS.");
        pLog("Settings: Language " + lang + " not supported. Using enUS.");
        lang = "enUS";
    }
    else if(lang == "enGB")
    {
        pDebug("Language: " + lang + ". Using enUS.");
        pLog("Settings: Language " + lang + ". Using enUS.");
        lang = "enUS";
    }
    else
    {
        pDebug("Language: " + lang + ".");
        pLog("Settings: Language " + lang + ".");
    }

    cardDownloader->setLang(lang);
    return lang;
}


void MainWindow::createCardsJsonMap(QByteArray &jsonData)
{
    emit pDebug("Create Json Map.");

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = jsonDoc.array();
    foreach(QJsonValue jsonCard, jsonArray)
    {
        QJsonObject jsonCardObject = jsonCard.toObject();
        cardsJson[jsonCardObject.value("id").toString()] = jsonCardObject;
    }

    emit cardsJsonReady();
}


void MainWindow::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError)
    {
        emit pDebug(reply->url().toString() + " --> Failed. Retrying...");
        networkManager->get(QNetworkRequest(reply->url()));
    }
    else
    {
        QString fullUrl = reply->url().toString();
        QString endUrl = fullUrl.split("/").last();

        //Cards json
        if(endUrl == "cards.json")
        {
            if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
            {
                checkCardsJsonVersion(reply->rawHeader("Location"));
            }
            else
            {
                emit pDebug("Extra: Json Cards --> Download Success.");
                QSettings settings("Arena Tracker", "Arena Tracker");
                settings.setValue("cardsJsonVersion", fullUrl);
                QByteArray jsonData = reply->readAll();
                Utility::dumpOnFile(jsonData, Utility::extraPath() + "/cards.json");
                createCardsJsonMap(jsonData);
            }
        }
        //Light Forge json
        else if(fullUrl == LIGHTFORGE_JSON_URL)
        {
            emit pDebug("Extra: Json LightForge --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/lightForge.json");
        }
        //Hearth Arena version
        else if(endUrl == "haVersion.json")
        {
            int haVersion = QJsonDocument::fromJson(reply->readAll()).object().value("haVersion").toInt();
            downloadHearthArenaJson(haVersion);
        }
        //Hearth Arena json
        else if(endUrl == "hearthArena.json")
        {
            emit pDebug("Extra: Json HearthArena --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/hearthArena.json");
        }
        //Synergies version
        else if(endUrl == "synergiesVersion.json")
        {
            int synergiesVersion = QJsonDocument::fromJson(reply->readAll()).object().value("synergiesVersion").toInt();
            downloadSynergiesJson(synergiesVersion);
        }
        //Hearth Arena json
        else if(endUrl == "synergies.json")
        {
            emit pDebug("Extra: Json synergies --> Download Success.");
            QByteArray jsonData = reply->readAll();
            Utility::dumpOnFile(jsonData, Utility::extraPath() + "/synergies.json");
        }
        //Themes json
        else if(endUrl == "Themes.json")
        {
            QJsonObject jsonObject = QJsonDocument::fromJson(reply->readAll()).object();
            for(const QString &key: jsonObject.keys())
            {
                downloadTheme(key, jsonObject.value(key).toInt());
            }
        }
        //Theme zip
        else if(fullUrl.contains(THEMES_URL) && endUrl.endsWith(".zip"))
        {
            pDebug("Themes: " + endUrl + " --> Download Success.");
            QByteArray data = reply->readAll();
            Utility::dumpOnFile(data, Utility::themesPath() + "/" + endUrl);
            Utility::unZip(Utility::themesPath() + "/" + endUrl, Utility::themesPath());
            QFile zipFile(Utility::themesPath() + "/" + endUrl);
            zipFile.remove();

            QString theme = endUrl.left(endUrl.length()-4);
            if(ui->configComboTheme->findText(theme) == -1)
            {
                ui->configComboTheme->addItem(theme);
            }
            if(ThemeHandler::themeLoaded() == theme)
            {
                loadTheme(theme);
            }
            else if(ThemeHandler::themeLoaded().isEmpty() && theme == DEFAULT_THEME)
            {
                ui->configComboTheme->setCurrentText(theme);
                loadTheme(theme);
            }
        }
        //Extra files
        else
        {
            pDebug("Extra: " + endUrl + " --> Download Success.");
            QByteArray data = reply->readAll();
            Utility::dumpOnFile(data, Utility::extraPath() + "/" + endUrl);
        }
    }
}


void MainWindow::checkCardsJsonVersion(QString cardsJsonVersion)
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString storedCardsJsonVersion = settings.value("cardsJsonVersion", "").toString();
    QFile cardsJsonFile(Utility::extraPath() + "/cards.json");
    emit pDebug("Extra: Json Cards --> Latest version: " + cardsJsonVersion);
    emit pDebug("Extra: Json Cards --> Stored version: " + storedCardsJsonVersion);

    //Need download
    if(cardsJsonVersion != storedCardsJsonVersion || !cardsJsonFile.exists())
    {
        emit pDebug("Extra: Json Cards --> Download from: " + cardsJsonVersion);
        networkManager->get(QNetworkRequest(QUrl(cardsJsonVersion)));
    }
    //No download
    else
    {
        emit pDebug("Extra: Json Cards --> Use local cards.json");
        //Al crear el JsonMap en initCardsJson no se ejecuta downloadAllArenaCodes a traves de emit cardsJsonReady
        //porque draftHandler aun no esta definido, asi que lo volvemos a llamar aqui.
        emit cardsJsonReady();
    }
}


void MainWindow::setLocalLang()
{
    QString lang = getHSLanguage();
    Utility::setLocalLang(lang);
}


void MainWindow::initCardsJson()
{
    Utility::setCardsJson(&cardsJson);
    networkManager->get(QNetworkRequest(QUrl(JSON_CARDS_URL)));
    emit pDebug("Extra: Json Cards --> Trying: " + QString(JSON_CARDS_URL));

    //Load local cards.json (Incluso aunque haya una version nueva para bajar)
    QFile cardsJsonFile(Utility::extraPath() + "/cards.json");
    if(cardsJsonFile.exists())
    {
        if(!cardsJsonFile.open(QIODevice::ReadOnly))
        {
            emit pDebug("ERROR: Failed to open cards.json");
            return;
        }
        QByteArray jsonData = cardsJsonFile.readAll();
        cardsJsonFile.close();
        createCardsJsonMap(jsonData);
    }
}


void MainWindow::downloadLightForgeJson()
{
    networkManager->get(QNetworkRequest(QUrl(LIGHTFORGE_JSON_URL)));
    emit pDebug("Extra: Json LightForge --> Download from: " + QString(LIGHTFORGE_JSON_URL));
}


void MainWindow::createNetworkManager()
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}


void MainWindow::createVersionChecker()
{
    VersionChecker *versionChecker = new VersionChecker(this);
    connect(versionChecker, SIGNAL(startProgressBar(int, QString)),
            this, SLOT(startProgressBar(int, QString)));
    connect(versionChecker, SIGNAL(advanceProgressBar(int, QString)),
            this, SLOT(advanceProgressBar(int, QString)));
    connect(versionChecker, SIGNAL(showMessageProgressBar(QString, int)),
            this, SLOT(showMessageProgressBar(QString, int)));
    connect(versionChecker, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(versionChecker, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createPremiumHandler()
{
    premiumHandler = new PremiumHandler(this);
    connect(premiumHandler, SIGNAL(setPremium(bool)),
            arenaHandler, SLOT(setPremium(bool)));
    connect(premiumHandler, SIGNAL(setPremium(bool)),
            draftHandler, SLOT(setPremium(bool)));
    connect(premiumHandler, SIGNAL(setPremium(bool)),
            planHandler, SLOT(setPremium(bool)));
    connect(trackobotUploader, SIGNAL(connected(QString,QString)),
            premiumHandler, SLOT(checkPremium(QString,QString)));
    connect(trackobotUploader, SIGNAL(disconnected()),
            premiumHandler, SLOT(checkPremium()));
    connect(premiumHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(premiumHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createTrackobotUploader()
{
    trackobotUploader = new TrackobotUploader(this);
    connect(trackobotUploader, SIGNAL(startProgressBar(int, QString)),
            this, SLOT(startProgressBar(int, QString)));
    connect(trackobotUploader, SIGNAL(advanceProgressBar(int, QString)),
            this, SLOT(advanceProgressBar(int, QString)));
    connect(trackobotUploader, SIGNAL(showMessageProgressBar(QString)),
            this, SLOT(showMessageProgressBar(QString)));
    connect(trackobotUploader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(trackobotUploader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createDraftHandler()
{
    draftHandler = new DraftHandler(this, ui);
    connect(draftHandler, SIGNAL(startProgressBar(int, QString)),
            this, SLOT(startProgressBar(int, QString)));
    connect(draftHandler, SIGNAL(advanceProgressBar(int, QString)),
            this, SLOT(advanceProgressBar(int, QString)));
    connect(draftHandler, SIGNAL(showMessageProgressBar(QString, int)),
            this, SLOT(showMessageProgressBar(QString, int)));
    connect(draftHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(draftHandler, SIGNAL(showPremiumDialog()),
            this, SLOT(showPremiumDialog()));
    connect(draftHandler, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCardDraft(QString)));
    connect(draftHandler, SIGNAL(calculateMinimumWidth()),
            this, SLOT(calculateMinimumWidth()));

    connect(draftHandler, SIGNAL(downloadStarted()),
            cardDownloader, SLOT(setFastMode()));
    connect(draftHandler, SIGNAL(downloadEnded()),
            cardDownloader, SLOT(setSlowMode()));

    //Connect en logLoader
//    connect(draftHandler, SIGNAL(draftEnded()),
//            logLoader, SLOT(setUpdateTimeMax()));
//    connect(draftHandler, SIGNAL(draftStarted()),
//            logLoader, SLOT(setUpdateTimeMin()));

    connect(draftHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(draftHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));

    connect(ui->minimizeButton, SIGNAL(clicked()),
            draftHandler, SLOT(minimizeScoreWindow()));
}


void MainWindow::createSecretsHandler()
{
    secretsHandler = new SecretsHandler(this, ui, enemyHandHandler);
    connect(secretsHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(secretsHandler, SIGNAL(revealCreatedByCard(QString,QString,int)),
            enemyHandHandler, SLOT(revealCreatedByCard(QString,QString,int)));
    connect(secretsHandler, SIGNAL(isolatedSecret(int,QString)),
            planHandler, SLOT(enemyIsolatedSecret(int,QString)));
    connect(secretsHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(secretsHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createArenaHandler()
{
    arenaHandler = new ArenaHandler(this, deckHandler, trackobotUploader, planHandler, ui);
    connect(arenaHandler, SIGNAL(showMessageProgressBar(QString)),
            this, SLOT(showMessageProgressBar(QString)));
    connect(arenaHandler, SIGNAL(showPremiumDialog()),
            this, SLOT(showPremiumDialog()));
    connect(arenaHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(arenaHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createEnemyDeckHandler()
{
    enemyDeckHandler = new EnemyDeckHandler(this, ui);
    connect(enemyDeckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyDeckHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(enemyDeckHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(enemyDeckHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createDeckHandler()
{
    deckHandler = new DeckHandler(this, ui, enemyDeckHandler, planHandler);
    connect(deckHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(deckHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(deckHandler, SIGNAL(showMessageProgressBar(QString)),
            this, SLOT(showMessageProgressBar(QString)));
    connect(deckHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(deckHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
    connect(deckHandler, SIGNAL(deckSizeChanged()),
            this, SLOT(spreadCorrectTamCard()));

    deckHandler->loadDecks();
}


void MainWindow::createEnemyHandHandler()
{
    enemyHandHandler = new EnemyHandHandler(this, ui);
    connect(enemyHandHandler, SIGNAL(checkCardImage(QString)),
            this, SLOT(checkCardImage(QString)));
    connect(enemyHandHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(enemyHandHandler, SIGNAL(enemyCardDraw(int,QString,QString,int)),
            planHandler, SLOT(enemyCardDraw(int,QString,QString,int)));
    connect(enemyHandHandler, SIGNAL(enemyCardBuff(int,int,int)),
            planHandler, SLOT(enemyCardBuff(int,int,int)));
    connect(enemyHandHandler, SIGNAL(revealEnemyCard(int,QString)),
            planHandler, SLOT(revealEnemyCard(int,QString)));
    connect(planHandler, SIGNAL(heroTotalAttackChange(bool,int,int)),
            enemyHandHandler, SLOT(drawHeroTotalAttack(bool,int,int)));
    connect(enemyHandHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(enemyHandHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createPlanHandler()
{
    planHandler = new PlanHandler(this, ui);
    connect(planHandler, SIGNAL(checkCardImage(QString, bool)),
            this, SLOT(checkCardImage(QString, bool)));
    connect(planHandler, SIGNAL(needMainWindowFade(bool)),
            this, SLOT(fadeBarAndButtons(bool)));
    connect(planHandler, SIGNAL(showPremiumDialog()),
            this, SLOT(showPremiumDialog()));
    connect(planHandler, SIGNAL(swapSize(bool)),
            this, SLOT(swapSizePlan(bool)));
    connect(planHandler, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(planHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::swapSizePlan(bool sizePlan)
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QSize newSize;

    if(this->planWindow == NULL)
    {
        if(!sizePlan)
        {
            settings.setValue("sizePlan", this->size());
            newSize = settings.value("size", QSize(255, 600)).toSize();
        }
        else
        {
            settings.setValue("size", this->size());
            newSize = settings.value("sizePlan", QSize(400, 400)).toSize();
        }
        this->resize(newSize);
    }
    else
    {
        if(!sizePlan)
        {
            settings.setValue("sizePlan", planWindow->size());
            newSize = settings.value("sizeWindowReplay", QSize(255, 600)).toSize();
        }
        else
        {
            settings.setValue("sizeWindowReplay", planWindow->size());
            newSize = settings.value("sizePlan", QSize(400, 400)).toSize();
        }
        planWindow->resize(newSize);
    }
}


void MainWindow::createCardWindow()
{
    cardWindow = new CardWindow(this);
    connect(deckHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(enemyDeckHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(enemyHandHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(secretsHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));
    connect(draftHandler, SIGNAL(overlayCardEntered(QString, QRect, int, int, bool)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int, bool)));
    connect(planHandler, SIGNAL(cardEntered(QString, QRect, int, int)),
            cardWindow, SLOT(loadCard(QString, QRect, int, int)));

    connect(planHandler, SIGNAL(cardLeave()),
            cardWindow, SLOT(hide()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            cardWindow, SLOT(hide()));
    connect(ui->deckListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->enemyDeckListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->drawListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->enemyHandListWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(ui->secretsTreeWidget, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
    connect(draftHandler, SIGNAL(overlayCardLeave()),
            cardWindow, SLOT(hide()));
    connect(draftHandler, SIGNAL(draftStarted()),
            cardWindow, SLOT(hide()));
    connect(ui->planGraphicsView, SIGNAL(leave()),
            cardWindow, SLOT(hide()));
}


void MainWindow::createCardListWindow()
{
    cardListWindow = new CardListWindow(this, secretsHandler);
    connect(planHandler, SIGNAL(secretEntered(int,QRect&,int,int)),
            cardListWindow, SLOT(loadSecret(int,QRect&,int,int)));
    connect(draftHandler, SIGNAL(itemEnter(QList<DeckCard>&,QRect&,int,int)),
            cardListWindow, SLOT(loadDraftItem(QList<DeckCard>&,QRect&,int,int)));
    connect(draftHandler, SIGNAL(itemEnterOverlay(QList<DeckCard>&,QPoint&,int,int)),
            cardListWindow, SLOT(loadDraftOverlayItem(QList<DeckCard>&,QPoint&,int,int)));

    connect(planHandler, SIGNAL(cardLeave()),
            cardListWindow, SLOT(hide()));
    connect(draftHandler, SIGNAL(itemLeave()),
            cardListWindow, SLOT(hide()));
}


void MainWindow::createCardDownloader()
{
    cardDownloader = new HSCardDownloader(this);
    connect(cardDownloader, SIGNAL(downloaded(QString)),
            this, SLOT(redrawDownloadedCardImage(QString)));
    connect(cardDownloader, SIGNAL(missingOnWeb(QString)),
            this, SLOT(missingOnWeb(QString)));
    connect(cardDownloader, SIGNAL(allCardsDownloaded()),
            this, SLOT(allCardsDownloaded()));
    connect(cardDownloader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(cardDownloader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
}


void MainWindow::createGameWatcher()
{
    gameWatcher = new GameWatcher(this);

    connect(gameWatcher, SIGNAL(newArena(QString)),
            this, SLOT(resetDeckDontRead()));
    connect(gameWatcher, SIGNAL(needResetDeck()),
            this, SLOT(resetDeck()));
    connect(gameWatcher, SIGNAL(arenaDeckRead()),
            this, SLOT(completeArenaDeck()));
    connect(gameWatcher, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(gameWatcher, SIGNAL(pDebug(QString,qint64,DebugLevel,QString)),
            this, SLOT(pDebug(QString,qint64,DebugLevel,QString)));

    connect(gameWatcher, SIGNAL(newGameResult(GameResult, LoadingScreenState, QString, qint64)),
            arenaHandler, SLOT(newGameResult(GameResult, LoadingScreenState, QString, qint64)));
    connect(gameWatcher, SIGNAL(newArena(QString)),
            arenaHandler, SLOT(newArena(QString)));
    //Rewards input disabled with track-o-bot stats
//    connect(gameWatcher, SIGNAL(inRewards()),
//            arenaHandler, SLOT(showRewards()));

    connect(gameWatcher, SIGNAL(newDeckCard(QString)),
            deckHandler, SLOT(newDeckCardAsset(QString)));
    connect(gameWatcher, SIGNAL(playerCardDraw(QString, int)),
            deckHandler, SLOT(playerCardDraw(QString, int)));
    connect(gameWatcher, SIGNAL(playerCardToHand(int,QString,int)),
            deckHandler, SLOT(playerCardToHand(int,QString,int)));
    connect(gameWatcher, SIGNAL(playerCardPlayed(int,QString,bool)),
            deckHandler, SLOT(removeRngCard(int,QString)));
    connect(gameWatcher, SIGNAL(playerReturnToDeck(QString, int)),
            deckHandler, SLOT(returnToDeck(QString, int)));
    connect(gameWatcher, SIGNAL(clearDrawList(bool)),
            deckHandler, SLOT(clearDrawList(bool)));
    connect(gameWatcher, SIGNAL(startGame()),
            deckHandler, SLOT(lockDeckInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool,bool)),
            deckHandler, SLOT(unlockDeckInterface()));
    connect(gameWatcher, SIGNAL(enterArena()),
            deckHandler, SLOT(enterArena()));
    connect(gameWatcher, SIGNAL(leaveArena()),
            deckHandler, SLOT(leaveArena()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString, int, int)),
            deckHandler, SLOT(setLastCreatedByCode(QString, QString)));
    connect(gameWatcher, SIGNAL(coinIdFound(int)),
            deckHandler, SLOT(setFirstOutsiderId(int)));

    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString,bool)),
            enemyDeckHandler, SLOT(enemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretRevealed(int, QString)),
            enemyDeckHandler, SLOT(enemySecretRevealed(int, QString)));
    connect(gameWatcher, SIGNAL(enemyKnownCardDraw(int, QString)),
            enemyDeckHandler, SLOT(enemyKnownCardDraw(int, QString)));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyDeckHandler, SLOT(lockEnemyDeckInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool,bool)),
            enemyDeckHandler, SLOT(unlockEnemyDeckInterface()));
    connect(gameWatcher, SIGNAL(enemyHero(QString)),
            enemyDeckHandler, SLOT(setEnemyClass(QString)));
    connect(gameWatcher, SIGNAL(coinIdFound(int)),
            enemyDeckHandler, SLOT(setFirstOutsiderId(int)));

    connect(gameWatcher, SIGNAL(enemyCardDraw(int,int,bool,QString)),
            enemyHandHandler, SLOT(showEnemyCardDraw(int,int,bool,QString)));
    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString,bool)),
            enemyHandHandler, SLOT(hideEnemyCardPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(lastHandCardIsCoin()),
            enemyHandHandler, SLOT(lastHandCardIsCoin()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString, int, int)),
            enemyHandHandler, SLOT(setLastCreatedByCode(QString)));
    connect(gameWatcher, SIGNAL(buffHandCard(int)),
            enemyHandHandler, SLOT(buffHandCard(int)));
    connect(gameWatcher, SIGNAL(startGame()),
            enemyHandHandler, SLOT(lockEnemyInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool,bool)),
            enemyHandHandler, SLOT(unlockEnemyInterface()));

    connect(gameWatcher, SIGNAL(playerMinionZonePlayAdd(QString,int,int)),
            planHandler, SLOT(playerMinionZonePlayAdd(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayAdd(QString,int,int)),
            planHandler, SLOT(enemyMinionZonePlayAdd(QString,int,int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlayAddTriggered(QString,int,int)),
            planHandler, SLOT(playerMinionZonePlayAddTriggered(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayAddTriggered(QString,int,int)),
            planHandler, SLOT(enemyMinionZonePlayAddTriggered(QString,int,int)));
    connect(gameWatcher, SIGNAL(playerHeroZonePlayAdd(QString,int)),
            planHandler, SLOT(playerHeroZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(enemyHeroZonePlayAdd(QString,int)),
            planHandler, SLOT(enemyHeroZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(playerHeroPowerZonePlayAdd(QString,int)),
            planHandler, SLOT(playerHeroPowerZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(enemyHeroPowerZonePlayAdd(QString,int)),
            planHandler, SLOT(enemyHeroPowerZonePlayAdd(QString,int)));
    connect(gameWatcher, SIGNAL(playerWeaponZonePlayAdd(QString, int)),
            planHandler, SLOT(playerWeaponZonePlayAdd(QString, int)));
    connect(gameWatcher, SIGNAL(enemyWeaponZonePlayAdd(QString, int)),
            planHandler, SLOT(enemyWeaponZonePlayAdd(QString, int)));
    connect(gameWatcher, SIGNAL(playerWeaponZonePlayRemove(int)),
            planHandler, SLOT(playerWeaponZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(enemyWeaponZonePlayRemove(int)),
            planHandler, SLOT(enemyWeaponZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlayRemove(int)),
            planHandler, SLOT(playerMinionZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlayRemove(int)),
            planHandler, SLOT(enemyMinionZonePlayRemove(int)));
    connect(gameWatcher, SIGNAL(playerMinionZonePlaySteal(int,int)),
            planHandler, SLOT(playerMinionZonePlaySteal(int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionZonePlaySteal(int,int)),
            planHandler, SLOT(enemyMinionZonePlaySteal(int,int)));
    connect(gameWatcher, SIGNAL(playerMinionPosChange(int,int)),
            planHandler, SLOT(playerMinionPosChange(int,int)));
    connect(gameWatcher, SIGNAL(enemyMinionPosChange(int,int)),
            planHandler, SLOT(enemyMinionPosChange(int,int)));
    connect(gameWatcher, SIGNAL(playerCardTagChange(int,QString,QString,QString)),
            planHandler, SLOT(playerCardTagChange(int,QString,QString,QString)));
    connect(gameWatcher, SIGNAL(enemyCardTagChange(int,QString,QString,QString)),
            planHandler, SLOT(enemyCardTagChange(int,QString,QString,QString)));
    connect(gameWatcher, SIGNAL(playerMinionTagChange(int,QString,QString,QString)),
            planHandler, SLOT(playerMinionTagChange(int,QString,QString,QString)));
    connect(gameWatcher, SIGNAL(enemyMinionTagChange(int,QString,QString,QString)),
            planHandler, SLOT(enemyMinionTagChange(int,QString,QString,QString)));
    connect(gameWatcher, SIGNAL(unknownTagChange(QString,QString)),
            planHandler, SLOT(unknownTagChange(QString,QString)));
    connect(gameWatcher, SIGNAL(playerTagChange(QString,QString)),
            planHandler, SLOT(playerTagChange(QString,QString)));
    connect(gameWatcher, SIGNAL(enemyTagChange(QString,QString)),
            planHandler, SLOT(enemyTagChange(QString,QString)));
    connect(gameWatcher, SIGNAL(zonePlayAttack(QString, int,int)),
            planHandler, SLOT(zonePlayAttack(QString, int,int)));
    connect(gameWatcher, SIGNAL(playerSecretPlayed(int,QString)),
            planHandler, SLOT(playerSecretPlayed(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretPlayed(int,CardClass,LoadingScreenState)),
            planHandler, SLOT(enemySecretPlayed(int,CardClass)));
    connect(gameWatcher, SIGNAL(playerSecretRevealed(int,QString)),
            planHandler, SLOT(playerSecretRevealed(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretRevealed(int,QString)),
            planHandler, SLOT(enemySecretRevealed(int,QString)));
    connect(gameWatcher, SIGNAL(playerSecretStolen(int,QString)),
            planHandler, SLOT(playerSecretStolen(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretStolen(int,QString)),
            planHandler, SLOT(enemySecretStolen(int,QString)));
    connect(gameWatcher, SIGNAL(playerCardToHand(int,QString,int)),
            planHandler, SLOT(playerCardDraw(int,QString,int)));
    connect(gameWatcher, SIGNAL(playerCardPlayed(int,QString,bool)),
            planHandler, SLOT(playerCardPlayed(int,QString,bool)));
    connect(gameWatcher, SIGNAL(enemyCardPlayed(int,QString,bool)),
            planHandler, SLOT(enemyCardPlayed(int,QString,bool)));
    connect(gameWatcher, SIGNAL(playerCardCodeChange(int,QString)),
            planHandler, SLOT(playerCardCodeChange(int,QString)));
    connect(gameWatcher, SIGNAL(newTurn(bool, int)),
            planHandler, SLOT(newTurn(bool, int)));
    connect(gameWatcher, SIGNAL(logTurn()),
            planHandler, SLOT(resetLastPowerAddon()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString,QString,int, int)),
            planHandler, SLOT(setLastTriggerId(QString,QString,int, int)));
    connect(gameWatcher, SIGNAL(playerCardObjPlayed(QString,int,int)),
            planHandler, SLOT(playerCardObjPlayed(QString,int,int)));
    connect(gameWatcher, SIGNAL(enemyCardObjPlayed(QString,int,int)),
            planHandler, SLOT(enemyCardObjPlayed(QString,int,int)));
    connect(gameWatcher, SIGNAL(startGame()),
            planHandler, SLOT(lockPlanInterface()));
    connect(gameWatcher, SIGNAL(endGame(bool,bool)),
            planHandler, SLOT(endGame(bool,bool)));


    connect(gameWatcher, SIGNAL(endGame(bool,bool)),
            secretsHandler, SLOT(resetSecretsInterface()));
    connect(gameWatcher, SIGNAL(enemySecretPlayed(int,CardClass, LoadingScreenState)),
            secretsHandler, SLOT(secretPlayed(int,CardClass, LoadingScreenState)));
    connect(gameWatcher, SIGNAL(enemySecretStolen(int,QString)),
            secretsHandler, SLOT(secretStolen(int,QString)));
    connect(gameWatcher, SIGNAL(enemySecretRevealed(int, QString)),
            secretsHandler, SLOT(secretRevealed(int, QString)));
    connect(gameWatcher, SIGNAL(playerSecretStolen(int, QString)),
            secretsHandler, SLOT(secretRevealed(int, QString)));
    connect(gameWatcher, SIGNAL(playerSpellPlayed(QString)),
            secretsHandler, SLOT(playerSpellPlayed(QString)));
    connect(gameWatcher, SIGNAL(playerSpellObjMinionPlayed()),
            secretsHandler, SLOT(playerSpellObjMinionPlayed()));
    connect(gameWatcher, SIGNAL(playerSpellObjHeroPlayed()),
            secretsHandler, SLOT(playerSpellObjHeroPlayed()));
    connect(gameWatcher, SIGNAL(playerBattlecryObjHeroPlayed()),
            secretsHandler, SLOT(playerBattlecryObjHeroPlayed()));
    connect(gameWatcher, SIGNAL(playerMinionPlayed(QString, int)),
            secretsHandler, SLOT(playerMinionPlayed(QString, int)));
    connect(gameWatcher, SIGNAL(enemyMinionDead(QString)),
            secretsHandler, SLOT(enemyMinionDead(QString)));
    connect(gameWatcher, SIGNAL(avengeTested()),
            secretsHandler, SLOT(avengeTested()));
    connect(gameWatcher, SIGNAL(cSpiritTested()),
            secretsHandler, SLOT(cSpiritTested()));
    connect(gameWatcher, SIGNAL(playerAttack(bool,bool,int)),
            secretsHandler, SLOT(playerAttack(bool,bool,int)));
    connect(gameWatcher, SIGNAL(playerHeroPower()),
            secretsHandler, SLOT(playerHeroPower()));
    connect(gameWatcher, SIGNAL(specialCardTrigger(QString, QString, int, int)),
            secretsHandler, SLOT(resetLastMinionDead(QString, QString)));

    connect(gameWatcher, SIGNAL(newArena(QString)),
            draftHandler, SLOT(beginDraft(QString)));
    connect(gameWatcher, SIGNAL(activeDraftDeck()),
            draftHandler, SLOT(endDraft()));
    connect(gameWatcher, SIGNAL(startGame()),    //Salida alternativa de drafting (+seguridad)
            draftHandler, SLOT(endDraftDeleteMechanicsWindow()));
    connect(gameWatcher, SIGNAL(pickCard(QString)),
            draftHandler, SLOT(pickCard(QString)));
    connect(gameWatcher, SIGNAL(enterArena()),
            draftHandler, SLOT(enterArena()));
    connect(gameWatcher, SIGNAL(leaveArena()),
            draftHandler, SLOT(leaveArena()));
}


void MainWindow::createLogLoader()
{
    logLoader = new LogLoader(this);
    connect(logLoader, SIGNAL(logReset()),
            this, SLOT(logReset()));
    connect(logLoader, SIGNAL(newLogLineRead(LogComponent, QString,qint64,qint64)),
            gameWatcher, SLOT(processLogLine(LogComponent, QString,qint64,qint64)));
    connect(logLoader, SIGNAL(logConfigSet()),
            this, SLOT(setLocalLang()));
    connect(logLoader, SIGNAL(showMessageProgressBar(QString)),
            this, SLOT(showMessageProgressBar(QString)));
    connect(logLoader, SIGNAL(pLog(QString)),
            this, SLOT(pLog(QString)));
    connect(logLoader, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SLOT(pDebug(QString,DebugLevel,QString)));
    connect(gameWatcher, SIGNAL(gameLogComplete(qint64,qint64,QString)),
            logLoader, SLOT(copyGameLog(qint64,qint64,QString)));

    //Connect de draftHandler
    connect(draftHandler, SIGNAL(draftEnded()),
            logLoader, SLOT(setUpdateTimeMax()));
    connect(draftHandler, SIGNAL(draftStarted()),
            logLoader, SLOT(setUpdateTimeMin()));

    if(!logLoader->init())  QTimer::singleShot(1, this, SLOT(closeApp()));
}


void MainWindow::completeUI()
{
    ThemeHandler::defaultEmptyValues();
    ui->tabWidget->clear();//Rellenado en spreadTheme

    ui->tabWidgetH2 = new MoveTabWidget(this);
    ui->tabWidgetH2->hide();
    ui->gridLayout->addWidget(ui->tabWidgetH2, 0, 1);
    ui->tabWidgetH3 = new MoveTabWidget(this);
    ui->tabWidgetH3->hide();
    ui->gridLayout->addWidget(ui->tabWidgetH3, 0, 2);
    ui->tabWidgetV1 = new MoveTabWidget(this);
    ui->tabWidgetV1->hide();
    ui->tabWidgetV1->setTabBarAutoHide(true);
    ui->gridLayout->addWidget(ui->tabWidgetV1, 1, 0);

    ui->progressBar->setVisible(false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);

    ui->progressBarMini->setFixedHeight(8);
    ui->progressBarMini->setVisible(false);
    ui->progressBarMini->setMaximum(100);
    ui->progressBarMini->setValue(100);

    completeConfigTab();

    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(spreadMouseInApp()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(changingTabResetSizePlan()));
    connect(ui->tabWidget, SIGNAL(detachTab(int,QPoint)),
            this, SLOT(createDetachWindow(int,QPoint)));
    connect(this, SIGNAL(cardsJsonReady()),
            this, SLOT(downloadAllArenaCodes()));

#ifdef QT_DEBUG
    pLog(tr("MODE DEBUG"));
    pDebug("MODE DEBUG");
#endif

#ifdef Q_OS_WIN
    pLog(tr("Settings: Platform: Windows"));
    pDebug("Platform: Windows");
#endif

#ifdef Q_OS_MAC
    pLog(tr("Settings: Platform: Mac"));
    pDebug("Platform: Mac");
#endif

#ifdef Q_OS_LINUX
    #ifdef APPIMAGE
        pLog(tr("Settings: Platform: Linux AppImage"));
        pDebug("Platform: Linux AppImage");
    #else
        pLog(tr("Settings: Platform: Linux Static"));
        pDebug("Platform: Linux Static");
    #endif
#endif

    completeUITabNames();
    completeUIButtons();
}


void MainWindow::completeUITabNames()
{
    ui->tabArena->setObjectName("TabArena");
    ui->tabEnemy->setObjectName("TabEnemy");
    ui->tabDeck->setObjectName("TabDeck");
    ui->tabEnemyDeck->setObjectName("TabEnemyDeck");
    ui->tabPlan->setObjectName("TabPlan");
    ui->tabConfig->setObjectName("TabConfig");
}


void MainWindow::completeUIButtons()
{
    ui->closeButton = new QPushButton("", this);
    ui->closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->closeButton->setFlat(true);
    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(closeApp()));


    ui->minimizeButton = new QPushButton("", this);
    ui->minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->minimizeButton->setFlat(true);
    connect(ui->minimizeButton, SIGNAL(clicked()),
            this, SLOT(showMinimized()));

    ui->resizeButton = new ResizeButton(this);
    ui->resizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->resizeButton->resize(24, 24);
    ui->resizeButton->setIconSize(QSize(24, 24));
    ui->resizeButton->setFlat(true);
    connect(ui->resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));
}


void MainWindow::closeApp()
{
    //Check unsaved decks
    if(ui->deckButtonSave->isEnabled() && !deckHandler->askSaveDeck())   return;
    removeNonCompleteDraft();
    resetSizePlan();
    draftHandler->endDraftDeleteMechanicsWindow();
    close();
}


void MainWindow::initConfigTheme(QString theme)
{
    int index = ui->configComboTheme->findText(theme);
    if(index == -1)
    {
        theme = DEFAULT_THEME;
        ui->configComboTheme->setCurrentText(theme);
    }
    else
    {
        ui->configComboTheme->setCurrentIndex(index);
    }
    loadTheme(theme, true);
}


void MainWindow::initConfigTab(int tooltipScale, int cardHeight, bool autoSize,
                               bool showClassColor, bool showSpellColor, bool showManaLimits,
                               bool showTotalAttack, bool showRngList, int maxGamesLog,
                               bool normalizedLF, QString theme)
{
    //UI
    switch(transparency)
    {
        case Transparent:
            ui->configRadioTransparent->setChecked(true);
            break;
        case AutoTransparent:
            ui->configRadioAuto->setChecked(true);
            break;
        case Opaque:
            ui->configRadioOpaque->setChecked(true);
            break;
        case Framed:
            ui->configRadioFramed->setChecked(true);
            break;
        default:
            transparency = AutoTransparent;
            ui->configRadioAuto->setChecked(true);
            break;
    }

    initConfigTheme(theme);

    //Deck
    if(cardHeight<ui->configSliderCardSize->minimum() || cardHeight>ui->configSliderCardSize->maximum())  cardHeight = 35;
    if(ui->configSliderCardSize->value() == cardHeight)   updateTamCard(cardHeight);
    else    ui->configSliderCardSize->setValue(cardHeight);

    if(tooltipScale<ui->configSliderTooltipSize->minimum() || tooltipScale>ui->configSliderTooltipSize->maximum())  tooltipScale = 10;
    if(ui->configSliderTooltipSize->value() == tooltipScale) updateTooltipScale(tooltipScale);
    else ui->configSliderTooltipSize->setValue(tooltipScale);

    ui->configCheckAutoSize->setChecked(autoSize);

    ui->configCheckClassColor->setChecked(showClassColor);
    updateShowClassColor(showClassColor);

    ui->configCheckSpellColor->setChecked(showSpellColor);
    updateShowSpellColor(showSpellColor);

    ui->configCheckManaLimits->setChecked(showManaLimits);
    updateShowManaLimits(showManaLimits);

    //Hand
    //Slider            0  - Ns - 11
    //DrawDissapear     -1 - Ns - 0
    switch(this->drawDisappear)
    {
        case -1:
            ui->configSliderDrawTime->setValue(0);
            updateTimeDraw(0);
            break;
        case 0:
            ui->configSliderDrawTime->setValue(11);
            break;
        default:
            if(this->drawDisappear<-1 || this->drawDisappear>10)    this->drawDisappear = 5;
            ui->configSliderDrawTime->setValue(this->drawDisappear);
            break;
    }

    ui->configCheckTotalAttack->setChecked(showTotalAttack);
    updateShowTotalAttack(showTotalAttack);

    ui->configCheckRngList->setChecked(showRngList);
    updateShowRngList(showRngList);


    //Draft
    if(this->showDraftOverlay) ui->configCheckOverlay->setChecked(true);
    draftHandler->setShowDraftOverlay(this->showDraftOverlay);

    if(this->draftLearningMode) ui->configCheckLearning->setChecked(true);
    draftHandler->setLearningMode(this->draftLearningMode);

    if(normalizedLF)    ui->configCheckNormalizeLF->setChecked(true);
    updateDraftNormalizeLF(normalizedLF);

    switch(draftMethod)
    {
        case HearthArena:
            ui->configRadioHA->setChecked(true);
            break;
        case LightForge:
            ui->configRadioLF->setChecked(true);
            break;
        case All:
            ui->configRadioCombined->setChecked(true);
            break;
        default:
            draftMethod = HearthArena;
            ui->configRadioHA->setChecked(true);
            break;
    }
    spreadDraftMethod(draftMethod);

    //Zero To Heroes
    ui->configSliderZero->setValue(maxGamesLog);
    updateMaxGamesLog(maxGamesLog);
}


void MainWindow::moveInScreen(QPoint pos, QSize size)
{
    QRect appRect(pos, size);
    QPoint midPoint = appRect.center();

    QString message =
            "Window Pos: (" + QString::number(pos.x()) + "," + QString::number(pos.y()) +
            ") - Size: (" + QString::number(size.width()) + "," + QString::number(size.height()) +
            ") - Mid: (" + QString::number(midPoint.x()) + "," + QString::number(midPoint.y()) + ")";
    pDebug(message);

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();

        if(geometry.contains(midPoint))
        {
            message =
                    "Window in screen: (" + QString::number(geometry.left()) + "," + QString::number(geometry.top()) + "," +
                    QString::number(geometry.right()) + "," + QString::number(geometry.bottom()) + ")";
            pDebug(message);
            move(pos);
            return;
        }
    }

    message = "Window outside screens. Move to (0,0)";
    pDebug(message);
    move(QPoint(0,0));
}


void MainWindow::readSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QPoint pos;
    QSize size;

    pos = settings.value("pos", QPoint(0,0)).toPoint();
    size = settings.value("size", QSize(255, 600)).toSize();

    this->transparency = (Transparency)settings.value("transparent", AutoTransparent).toInt();
    QString theme = settings.value("theme", DEFAULT_THEME).toString();

    int cardHeight = settings.value("cardHeight", 35).toInt();
    this->drawDisappear = settings.value("drawDisappear", 5).toInt();
    this->showDraftOverlay = settings.value("showDraftOverlay", true).toBool();
    this->draftLearningMode = settings.value("draftLearningMode", false).toBool();
    bool normalizedLF = settings.value("draftNormalizedLF", false).toBool();
    this->draftMethod = (DraftMethod)settings.value("draftMethod", LightForge).toInt();
    int tooltipScale = settings.value("tooltipScale", 10).toInt();
    bool autoSize = settings.value("autoSize", true).toBool();
    bool showClassColor = settings.value("showClassColor", true).toBool();
    bool showSpellColor = settings.value("showSpellColor", true).toBool();
    bool showManaLimits = settings.value("showManaLimits", true).toBool();
    bool showTotalAttack = settings.value("showTotalAttack", true).toBool();
    bool showRngList = settings.value("showRngList", true).toBool();
    int maxGamesLog = settings.value("maxGamesLog", 15).toInt();

    initConfigTab(tooltipScale, cardHeight, autoSize, showClassColor, showSpellColor, showManaLimits, showTotalAttack, showRngList,
                  maxGamesLog, normalizedLF, theme);

    this->setAttribute(Qt::WA_TranslucentBackground, transparency!=Framed);
    this->showWindowFrame(transparency == Framed);
    this->show();
    this->setMinimumSize(100,200);  //El minimumSize inicial es incorrecto
    resize(size);
    moveInScreen(pos, size);
    calculateMinimumWidth();

    //Detach Windows
    if(settings.value("deckWindow", true).toBool())         createDetachWindow(ui->tabDeck);
    if(settings.value("arenaWindow", false).toBool())       createDetachWindow(ui->tabArena);
    if(settings.value("enemyWindow", false).toBool())       createDetachWindow(ui->tabEnemy);
    if(settings.value("enemyDeckWindow", false).toBool())   createDetachWindow(ui->tabEnemyDeck);
    if(settings.value("planWindow", false).toBool())        createDetachWindow(ui->tabPlan);
}


void MainWindow::writeSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("transparent", (int)this->transparency);
    settings.setValue("theme", ui->configComboTheme->currentText());
    settings.setValue("cardHeight", ui->configSliderCardSize->value());
    settings.setValue("drawDisappear", this->drawDisappear);
    settings.setValue("showDraftOverlay", this->showDraftOverlay);
    settings.setValue("draftLearningMode", this->draftLearningMode);
    settings.setValue("draftNormalizedLF", ui->configCheckNormalizeLF->isChecked());
    settings.setValue("draftMethod", (int)this->draftMethod);
    settings.setValue("tooltipScale", ui->configSliderTooltipSize->value());
    settings.setValue("autoSize", ui->configCheckAutoSize->isChecked());
    settings.setValue("showClassColor", ui->configCheckClassColor->isChecked());
    settings.setValue("showSpellColor", ui->configCheckSpellColor->isChecked());
    settings.setValue("showManaLimits", ui->configCheckManaLimits->isChecked());
    settings.setValue("showTotalAttack", ui->configCheckTotalAttack->isChecked());
    settings.setValue("showRngList", ui->configCheckRngList->isChecked());
    settings.setValue("maxGamesLog", ui->configSliderZero->value());
    settings.setValue("deckWindow", deckWindow!=NULL);
    settings.setValue("arenaWindow", arenaWindow!=NULL);
    settings.setValue("enemyWindow", enemyWindow!=NULL);
    settings.setValue("enemyDeckWindow", enemyDeckWindow!=NULL);
    settings.setValue("planWindow", planWindow!=NULL);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);

    hide();
    writeSettings();
    if(deckWindow != NULL)
    {
        deckWindow->close();
        deckWindow = NULL;
    }
    if(arenaWindow != NULL)
    {
        arenaWindow->close();
        arenaWindow = NULL;
    }
    if(enemyWindow != NULL)
    {
        enemyWindow->close();
        enemyWindow = NULL;
    }
    if(enemyDeckWindow != NULL)
    {
        enemyDeckWindow->close();
        enemyDeckWindow = NULL;
    }
    if(planWindow != NULL)
    {
        planWindow->close();
        planWindow = NULL;
    }
    event->accept();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPosition = event->globalPos() - dragPosition;
        int top = newPosition.y();
        int bottom = top + this->height();
        int left = newPosition.x();
        int right = left + this->width();
        int midX = (left + right)/2;
        int midY = (top + bottom)/2;

        const int stickyMargin = 10;

        foreach (QScreen *screen, QGuiApplication::screens())
        {
            if (!screen)    continue;
            QRect screenRect = screen->geometry();
            int topScreen = screenRect.y();
            int bottomScreen = topScreen + screenRect.height();
            int leftScreen = screenRect.x();
            int rightScreen = leftScreen + screenRect.width();

            if(midX < leftScreen || midX > rightScreen ||
                    midY < topScreen || midY > bottomScreen) continue;

            if(std::abs(top - topScreen) < stickyMargin)
            {
                newPosition.setY(topScreen);
            }
            else if(std::abs(bottom - bottomScreen) < stickyMargin)
            {
                newPosition.setY(bottomScreen - this->height());
            }
            if(std::abs(left - leftScreen) < stickyMargin)
            {
                newPosition.setX(leftScreen);
            }
            else if(std::abs(right - rightScreen) < stickyMargin)
            {
                newPosition.setX(rightScreen - this->width());
            }
            move(newPosition);
            event->accept();
            return;
        }

        move(newPosition);
        event->accept();
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Control)
    {
        if(event->modifiers()&Qt::ControlModifier)
        {
            if(event->key() == Qt::Key_R)       resetSettings();
            else if(event->key() == Qt::Key_1)  draftHandler->pickCard("0");
            else if(event->key() == Qt::Key_2)  draftHandler->pickCard("1");
            else if(event->key() == Qt::Key_3)  draftHandler->pickCard("2");
            else if(event->key() == Qt::Key_5)  draftHandler->endDraft();
#ifdef Q_OS_LINUX
            else if(event->key() == Qt::Key_S)  askLinuxShortcut();
#endif
#ifdef QT_DEBUG
            else if(event->key() == Qt::Key_D)  createDebugPack();
            else if(event->key() == Qt::Key_Z)
            {
                if(this->planWindow != NULL)
                {
                    planWindow->resize(QSize(960, 1080));
                    planWindow->move(1920, 0);
                }
                if(this->deckWindow != NULL)
                {
                    deckWindow->resize(QSize(deckWindow->width(), 1053));
                    deckWindow->move(0, 0);
                }
                this->resize(QSize(250, 540));
                this->move(2880, 0);
            }
            else if(event->key() == Qt::Key_8)  QtConcurrent::run(this->draftHandler, &DraftHandler::craftGoldenCopy, 0);
            else if(event->key() == Qt::Key_9)  QtConcurrent::run(this->draftHandler, &DraftHandler::craftGoldenCopy, 1);
            else if(event->key() == Qt::Key_0)  QtConcurrent::run(this->draftHandler, &DraftHandler::craftGoldenCopy, 2);
#endif
        }
    }
}


//Restaura ambas ventanas minimizadas
void MainWindow::changeEvent(QEvent * event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        if((windowState() & Qt::WindowMinimized) == 0)
        {
            if(deckWindow != NULL)      deckWindow->setWindowState(Qt::WindowActive);
            if(arenaWindow != NULL)     arenaWindow->setWindowState(Qt::WindowActive);
            if(enemyWindow != NULL)     enemyWindow->setWindowState(Qt::WindowActive);
            if(enemyDeckWindow != NULL) enemyDeckWindow->setWindowState(Qt::WindowActive);
            if(planWindow != NULL)      planWindow->setWindowState(Qt::WindowActive);
            if(draftHandler != NULL)    draftHandler->deMinimizeScoreWindow();
        }

    }
}


void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach(const QUrl &url, e->mimeData()->urls())
    {
        QString fileName = url.toLocalFile();
        pDebug("Dropped " + fileName);

        if(fileName.endsWith(".xls"))
        {
            if(askImportXls())
            {
                trackobotUploader->uploadXls(fileName);
            }
            break;
        }
        else if(fileName.endsWith(".track-o-bot"))
        {
            if(askImportAccount())
            {
                trackobotUploader->importAccount(fileName);
            }
            break;
        }
    }
}


bool MainWindow::askImportAccount()
{
    QString text =  "Do you want to use this new track-o-bot account"
                    "<br>as your default account?"
                    "<br><a href='https://www.youtube.com/watch?v=DfIat7UR7Tc'>VIDEO</a>";

    QMessageBox msgBox(this);
    msgBox.setText(text);
    msgBox.setWindowTitle("Import track-o-bot account?");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.exec();

    if(msgBox.result() == QMessageBox::Yes) return true;
    else                                    return false;
}


bool MainWindow::askImportXls()
{
    QString text =  "Do you want to upload all the games included"
                    "<br>on this XLS file to your track-o-bot account?"
                    "<br><br>Keep in mind the XLS needs to follow"
                    "<br>Arena Mastery export XLS format."
                    "<br><a href='https://www.youtube.com/watch?v=LOB1sBU1AOA'>VIDEO</a>";

    QMessageBox msgBox(this);
    msgBox.setText(text);
    msgBox.setWindowTitle("Upload XLS?");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.exec();

    if(msgBox.result() == QMessageBox::Yes) return true;
    else                                    return false;
}


void MainWindow::leaveEvent(QEvent * e)
{
    QMainWindow::leaveEvent(e);

    this->mouseInApp = false;
    spreadMouseInApp();

    if(arenaHandler != NULL)    arenaHandler->deselectRow();
}


void MainWindow::enterEvent(QEvent * e)
{
    QMainWindow::enterEvent(e);

    this->mouseInApp = true;
    spreadMouseInApp();
}


void MainWindow::spreadMouseInApp()
{
    QWidget *currentTab = ui->tabWidget->currentWidget();

    if(currentTab == ui->tabDeck)           deckHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabEnemy)     enemyHandHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabPlan)      planHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabEnemyDeck) enemyDeckHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabArena)     arenaHandler->setMouseInApp(mouseInApp);
    else if(currentTab == ui->tabDraft)     draftHandler->setMouseInApp(mouseInApp);
    else                                    updateOtherTabsTransparency();

    //Fade Bar
    if(transparency==Transparent)
    {
        if(mouseInApp)      fadeBarAndButtons(false);
        else                fadeBarAndButtons(true);
    }
    else if(transparency==AutoTransparent && currentTab != ui->tabDeck && currentTab != ui->tabEnemy && currentTab != ui->tabEnemyDeck)
    {
        fadeBarAndButtons(false);
    }
}


void MainWindow::changingTabResetSizePlan()
{
    if(planWindow == NULL)  resetSizePlan();
}


void MainWindow::resetSizePlan()
{
    if(planHandler->resetSizePlan())  swapSizePlan(false);
}


void MainWindow::resizeSlot(QSize size)
{
    resize(size);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resizeChecks();
    event->accept();
}


void MainWindow::resizeChecks()
{
    QWidget *widget = this->centralWidget();

    updateTabWidgetsTheme(false, true);

    int top = widget->pos().y();
    int bottom = top + widget->height();
    int left = widget->pos().x();
    int right = left + widget->width();

    resizeTopButtons(right - ThemeHandler::borderWidth(), top + ThemeHandler::borderWidth());
    ui->resizeButton->move(right-24, bottom-24);

    if(deckWindow == NULL) spreadCorrectTamCard(); //Solo es necesario si ui->tabDeck esta en mainWindow, debido a auto size
}


void MainWindow::resizeTopButtons(int right, int top)
{
    int limitWidth = ui->tabWidget->tabBar()->width() + BIG_BUTTONS_H;

    int buttonsWidth;
    bool smallButtons = (this->width() - ThemeHandler::borderWidth()*2) < limitWidth;
    if(smallButtons)
    {
        buttonsWidth = SMALL_BUTTONS_H;
        ui->closeButton->move(right-buttonsWidth, top);
        ui->minimizeButton->move(right-buttonsWidth, top+buttonsWidth);
    }
    else
    {
        buttonsWidth = 24;
        ui->closeButton->move(right-buttonsWidth, top);
        ui->minimizeButton->move(right-2*buttonsWidth, top);
    }

    ui->closeButton->resize(buttonsWidth, buttonsWidth);
    ui->closeButton->setIconSize(QSize(buttonsWidth, buttonsWidth));
    ui->minimizeButton->resize(buttonsWidth, buttonsWidth);
    ui->minimizeButton->setIconSize(QSize(buttonsWidth, buttonsWidth));
}


void MainWindow::moveTabTo(QWidget *widget, QTabWidget *tabWidget)
{
    QIcon icon;
    QString tooltip;
    if(widget == ui->tabArena)
    {
        icon = QIcon(ThemeHandler::tabGamesFile());
        tooltip = "Games";
    }
    else if(widget == ui->tabDeck)
    {
        icon = QIcon(ThemeHandler::tabDeckFile());
        tooltip = "Deck";
    }
    else if(widget == ui->tabEnemy)
    {
        icon = QIcon(ThemeHandler::tabHandFile());
        tooltip = "Hand";
    }
    else if(widget == ui->tabPlan)
    {
        icon = QIcon(ThemeHandler::tabPlanFile());
        tooltip = "Replay";
    }
    else if(widget == ui->tabEnemyDeck)
    {
        icon = QIcon(ThemeHandler::tabEnemyDeckFile());
        tooltip = "Enemy Deck";
    }
    else if(widget == ui->tabLog)
    {
        icon = QIcon(ThemeHandler::tabLogFile());
        tooltip = "Log";
    }
    else if(widget == ui->tabConfig)
    {
        icon = QIcon(ThemeHandler::tabConfigFile());
        tooltip = "Config";
    }
    tabWidget->addTab(widget, icon, "");
    tabWidget->setTabToolTip(tabWidget->count()-1, tooltip);
}


//Se llama al meter o sacar tabs en tabBar o al cambiar theme (por si tiene bordes)
void MainWindow::calculateMinimumWidth()
{
    //El menor ancho de una tab es 38 y el menor de los botones 19;
    int minWidth = ui->tabWidget->count()*38 + SMALL_BUTTONS_H + ThemeHandler::borderWidth()*2;
    this->setMinimumWidth(minWidth);
}


//Fija la anchura de la ventana de deck, enemyHand y enemyDeck.
void MainWindow::calculateCardWindowMinimumWidth(DetachWindow *detachWindow, bool hasBorders)
{
    if(detachWindow == NULL)    return;

    int deckWidth = ui->deckListWidget->sizeHintForColumn(0) + (hasBorders?2*ThemeHandler::borderWidth():0);
    if(detachWindow == deckWindow)      deckWindow->setFixedWidth(deckWidth);
    if(detachWindow == enemyWindow)     enemyWindow->setFixedWidth(deckWidth);
    if(detachWindow == enemyDeckWindow) enemyDeckWindow->setFixedWidth(deckWidth);
}


void MainWindow::removeNonCompleteDraft()
{
    //Remove old not-complete draft
    if(!draftLogFile.isEmpty())
    {
        //Check dir
        QFileInfo dirInfo(Utility::gameslogPath());
        if(!dirInfo.exists())
        {
            pDebug("Cannot remove non-complete draft Log. GamesLog dir doesn't exist.");
            return;
        }

        QDir dir(Utility::gameslogPath());
        dir.remove(draftLogFile);
        pDebug("Remove non-complete draft: " + draftLogFile);
        draftLogFile = "";
    }
}


void MainWindow::checkDraftLogLine(QString logLine, QString file)
{
    //New Draft
    if(file == "DraftHandler")
    {
        QRegularExpressionMatch match;
        if(logLine.contains(QRegularExpression("DraftHandler: Begin draft\\. Heroe: (\\d+)"), &match))
        {
            //Check dir
            QFileInfo dir(Utility::gameslogPath());
            if(!dir.exists())
            {
                pDebug("Cannot create draft Log. GamesLog dir doesn't exist.");
                return;
            }

            //Remove old not-complete draft
            removeNonCompleteDraft();

            QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh-mm");
            QString playerHero = Utility::heroStringFromLogNumber(match.captured(1));
            QString fileName = "DRAFT " + timeStamp + " " + playerHero + ".arenatracker";

            QFile logDraft(Utility::gameslogPath() + "/" + fileName);
            if(!logDraft.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                pDebug("Cannot create draft log file...", Error);
                pLog(tr("Log: ERROR:Cannot create draft log file..."));
                return;
            }

            QTextStream stream(&logDraft);
            stream << logLine << endl;
            logDraft.close();

            pDebug("Start DraftLog: " + fileName);
            draftLogFile = fileName;
            return;
        }
    }

    //Continue Draft
    bool copyLogLine = false;
    bool endDraftLog = false;

    if(!draftLogFile.isEmpty())
    {
        if(file == "DraftHandler")
        {
            if(logLine.contains("New codes") || logLine.contains("Pick card"))
            {
                copyLogLine = true;
            }
            else if(logLine.contains("End draft"))
            {
                copyLogLine = true;
                endDraftLog = true;
            }
        }

        if(copyLogLine)
        {
            QFile logDraft(Utility::gameslogPath() + "/" + draftLogFile);
            if(!logDraft.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                pDebug("Cannot open draft log file...", Error);
                pLog(tr("Log: ERROR:Cannot open draft log file..."));
                return;
            }

            QTextStream stream(&logDraft);
            stream << logLine << endl;
            logDraft.close();
        }
        if(endDraftLog)
        {
            pDebug("End DraftLog: " + draftLogFile);
            if(arenaHandler != NULL)    arenaHandler->linkDraftLogToArenaCurrent(draftLogFile);
            draftLogFile = "";
        }
    }
}


void MainWindow::pDebug(QString line, DebugLevel debugLevel, QString file)
{
    pDebug(line, 0, debugLevel, file);
}


void MainWindow::pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file)
{
    (void)debugLevel;
    QString logLine = "";
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    while(line.length() > 0 && line[0]==QChar('\n'))
    {
        line.remove(0, 1);
        logLine += '\n';
    }

    if(!line.isEmpty())
    {
        logLine += timeStamp + " - " + file;
        if(numLine > 0) logLine += "(" + QString::number(numLine) + ")";
        logLine += ": " + line;
    }

    qDebug().noquote() << logLine;

    if(atLogFile != NULL)
    {
        QTextStream stream(atLogFile);
        stream << logLine << endl;
    }

    if(copyGameLogs)    checkDraftLogLine(logLine, file);
}


void MainWindow::pLog(QString line)
{
    ui->logTextEdit->append(line);
}


void MainWindow::logReset()
{
    deckHandler->unlockDeckInterface();
    deckHandler->leaveArena();
    enemyHandHandler->unlockEnemyInterface();
    gameWatcher->reset();
}


bool MainWindow::checkCardImage(QString code, bool isHero)
{
    QFileInfo cardFile(Utility::hscardsPath() + "/" + code + ".png");

    if(!cardFile.exists())
    {
        //La bajamos de HearthHead
        cardDownloader->downloadWebImage(code, isHero);
        return false;
    }
    return true;
}


void MainWindow::redrawDownloadedCardImage(QString code)
{
    deckHandler->redrawDownloadedCardImage(code);
    enemyDeckHandler->redrawDownloadedCardImage(code);
    enemyHandHandler->redrawDownloadedCardImage(code);
    planHandler->redrawDownloadedCardImage(code);
    secretsHandler->redrawDownloadedCardImage(code);
    draftHandler->reHistDownloadedCardImage(code);
    if(!allCardsDownloadList.isEmpty())     this->updateProgressAllCardsDownload(code);
}


void MainWindow::missingOnWeb(QString code)
{
    draftHandler->reHistDownloadedCardImage(code, true);
    if(!allCardsDownloadList.isEmpty())     this->updateProgressAllCardsDownload(code);
}


void MainWindow::resetSettings()
{
    int ret = QMessageBox::warning(this, tr("Reset settings"),
                                   tr("Do you want to reset Arena Tracker settings?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logsDirPath", "");
        settings.setValue("logConfig", "");
        settings.setValue("playerTag", "");
        settings.setValue("sizeDraft", QSize(255, 600));
        settings.setValue("shortcutAsked", false);

        resize(QSize(255, 600));
        move(QPoint(0,0));
        close();

        //Write detachWindows settings after they are closed
        settings.setValue("deckWindow", false);
        settings.setValue("arenaWindow", false);
        settings.setValue("enemyWindow", false);
        settings.setValue("enemyDeckWindow", false);
        settings.setValue("planWindow", false);
    }
}


void MainWindow::createLogFile()
{
    atLogFile = new QFile(Utility::dataPath() + "/ArenaTrackerLog.txt");
    if(atLogFile->exists())  atLogFile->remove();
    if(!atLogFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        pDebug("Failed to create Arena Tracker log on disk.", Error);
        pLog(tr("File: ERROR: Failed to create Arena Tracker log on disk."));
        atLogFile = NULL;
    }
}


void MainWindow::closeLogFile()
{
    if(atLogFile == NULL)   return;
    atLogFile->close();
    delete atLogFile;
    atLogFile = NULL;
}


void MainWindow::createDataDir()
{
    Utility::createDir(Utility::dataPath());
//    removeHSCards();//Redownload HSCards en esta version
    if(Utility::createDir(Utility::hscardsPath()))
    {
        //Necesitamos bajar todas las cartas
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("allCardsDownloaded", false);
    }
    Utility::createDir(Utility::gameslogPath());
    Utility::createDir(Utility::extraPath());
    Utility::createDir(Utility::themesPath());

    pDebug("Path Arena Tracker Dir: " + Utility::dataPath());
}


void MainWindow::downloadExtraFiles()
{
    QFileInfo file;

    file = QFileInfo(Utility::extraPath() + "/arenaTemplate.png");
    if(!file.exists())  networkManager->get(QNetworkRequest(QUrl(EXTRA_URL + QString("/arenaTemplate.png"))));

    file = QFileInfo(Utility::extraPath() + "/icon.png");
    if(!file.exists())  networkManager->get(QNetworkRequest(QUrl(IMAGES_URL + QString("/icon.png"))));
}


void MainWindow::downloadHearthArenaVersion()
{
    networkManager->get(QNetworkRequest(QUrl(HA_URL + QString("/haVersion.json"))));
}


void MainWindow::downloadHearthArenaJson(int version)
{
    bool needDownload = false;
    QSettings settings("Arena Tracker", "Arena Tracker");
    int storedVersion = settings.value("haVersion", 0).toInt();

    QFileInfo fileInfo(Utility::extraPath() + "/hearthArena.json");
    if(!fileInfo.exists())          needDownload = true;
    if(version != storedVersion)    needDownload = true;

    emit pDebug("Extra: Json HearthArena: Local(" + QString::number(storedVersion) + ") - "
                        "Web(" + QString::number(version) + ")" + (!needDownload?" up-to-date":""));

    if(needDownload)
    {
        if(fileInfo.exists())
        {
            QFile file(Utility::extraPath() + "/hearthArena.json");
            file.remove();
            emit pDebug("Extra: Json HearthArena removed.");
        }

        settings.setValue("haVersion", version);
        networkManager->get(QNetworkRequest(QUrl(HA_URL + QString("/hearthArena.json"))));
        emit pDebug("Extra: Json HearthArena --> Download from: " + QString(HA_URL) + QString("/hearthArena.json"));
    }
}


void MainWindow::downloadSynergiesVersion()
{
    networkManager->get(QNetworkRequest(QUrl(SYNERGIES_URL + QString("/synergiesVersion.json"))));
}


void MainWindow::downloadSynergiesJson(int version)
{
    bool needDownload = false;
    QSettings settings("Arena Tracker", "Arena Tracker");
    int storedVersion = settings.value("synergiesVersion", 0).toInt();

    QFileInfo fileInfo(Utility::extraPath() + "/synergies.json");
    if(!fileInfo.exists())          needDownload = true;
    if(version != storedVersion)    needDownload = true;

    emit pDebug("Extra: Json Synergies: Local(" + QString::number(storedVersion) + ") - "
                        "Web(" + QString::number(version) + ")" + (!needDownload?" up-to-date":""));

    if(needDownload)
    {
        if(fileInfo.exists())
        {
            QFile file(Utility::extraPath() + "/synergies.json");
            file.remove();
            emit pDebug("Extra: Json Synergies removed.");
        }

        settings.setValue("synergiesVersion", version);
        networkManager->get(QNetworkRequest(QUrl(SYNERGIES_URL + QString("/synergies.json"))));
        emit pDebug("Extra: Json Synergies --> Download from: " + QString(SYNERGIES_URL) + QString("/synergies.json"));
    }
}


void MainWindow::downloadThemes()
{
    networkManager->get(QNetworkRequest(QUrl(THEMES_URL + QString("/Themes.json"))));
}


void MainWindow::downloadTheme(QString theme, int version)
{
    bool needDownload = false;
    QSettings settings("Arena Tracker", "Arena Tracker");
    int storedVersion = settings.value(theme + "Theme", 0).toInt();

    QFileInfo dirInfo(Utility::themesPath() + "/" + theme);
    if(!dirInfo.exists())           needDownload = true;
    if(version != storedVersion)    needDownload = true;

    emit pDebug("Themes: " + theme + ": Local(" + QString::number(storedVersion) + ") - "
                        "Web(" + QString::number(version) + ")" + (!needDownload?" up-to-date":""));

    if(needDownload)
    {
        if(dirInfo.exists())
        {
            QDir dir(Utility::themesPath() + "/" + theme);
            dir.removeRecursively();
            emit pDebug("Themes: " + Utility::themesPath() + "/" + theme + " removed.");
        }

        settings.setValue(theme + "Theme", version);
        networkManager->get(QNetworkRequest(QUrl(QString(THEMES_URL) + "/" + theme + ".zip")));
        emit pDebug("Themes: " + theme + ".zip --> Download from: " + THEMES_URL);
    }
}


void MainWindow::removeHSCards()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runVersion = settings.value("runVersion", "").toString();

    if(runVersion != VERSION)
    {
        QDir cardsDir = QDir(Utility::hscardsPath());
        cardsDir.removeRecursively();
        emit pDebug(Utility::hscardsPath() + " removed.");
    }
}


void MainWindow::checkLinuxShortcut()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    bool shortcutAsked = settings.value("shortcutAsked", false).toBool();

    if(!shortcutAsked)
    {
#ifdef APPIMAGE
        QFile appFile(Utility::dataPath() + "/ArenaTracker.Linux.AppImage");
        if(appFile.exists())
        {
            settings.setValue("shortcutAsked", true);
            createLinuxShortcut();
            showMessageAppImageShortcut();
        }
#else
        settings.setValue("shortcutAsked", true);
        askLinuxShortcut();
#endif
    }
}


void MainWindow::askLinuxShortcut()
{
    int answer = QMessageBox::question(this, tr("Create shortcut?"), tr("Do you want to create a desktop shortcut\nand a menu item for Arena Tracker?"),
                             QMessageBox::Yes, QMessageBox::No);
    if(answer == QMessageBox::Yes)
    {
        createLinuxShortcut();
    }
}


void MainWindow::showMessageAppImageShortcut()
{
    QMessageBox::information(this, tr("Use the shorcut"),
                    tr("Arena Tracker AppImage has been copied to \n(~/.local/share/Arena Tracker) and a new shortcut has been created "
                       "in your desktop linked to that AppImage."
                       "\n\nFrom now on you should run Arena Tracker from that shortcut."
                       "\nYou can also remove the AppImage you downloaded."),
                    QMessageBox::Ok);
}


void MainWindow::createLinuxShortcut()
{
#ifdef APPIMAGE
    QString appImagePath = Utility::dataPath() + "/ArenaTracker.Linux.AppImage";
#else
    QString appImagePath = Utility::appPath() + "/ArenaTracker";
#endif

    //Menu Item shortcut
    QFile shortcutFile(QDir::homePath() + "/.local/share/applications/Arena Tracker.desktop");
    if(shortcutFile.exists())   shortcutFile.remove();
    if(!shortcutFile.open(QIODevice::WriteOnly))
    {
        emit pDebug("ERROR: Cannot create Arena Tracker.desktop", Error);
        emit pLog(tr("Log: ERROR:Arena Tracker.desktop"));
        return;
    }
    shortcutFile.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);

    QTextStream out(&shortcutFile);

    out << "[Desktop Entry]" << endl;
    out << "Comment=" << endl;
    out << "Terminal=false" << endl;
    out << "Name=Arena Tracker" << endl;
    out << "Type=Application" << endl;
    out << "Exec=\"" + appImagePath + "\"" << endl;
    out << "Icon=" + Utility::extraPath() + "/icon.png" << endl;

    shortcutFile.close();

    //Desktop shortcut
    QString desktopShorcutFilename = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/Arena Tracker.desktop";
    QFile desktopShortcut(desktopShorcutFilename);
    if(desktopShortcut.exists())    desktopShortcut.remove();
    shortcutFile.copy(desktopShorcutFilename);

    emit pDebug("Desktop and menu shorcut created pointing to " + appImagePath);
    emit pLog("Shortcut: Desktop and menu shorcut created pointing to " + appImagePath);
}


void MainWindow::checkGamesLogDir()
{
    QFileInfo dirInfo(Utility::gameslogPath());
    if(!dirInfo.exists())
    {
        pDebug("Cannot check GamesLog dir. Dir doesn't exist.");
        return;
    }

    int maxGamesLog = ui->configSliderZero->value();
    if(maxGamesLog == 100)
    {
        pDebug("GamesLog: Keep ALL.");
        maxGamesLog = std::numeric_limits<int>::max();
    }
    else    pDebug("GamesLog: Keep recent " + QString::number(maxGamesLog) + ".");

    QDir dir(Utility::gameslogPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QStringList filterName;
    filterName << "*.arenatracker";
    dir.setNameFilters(filterName);

    QStringList files = dir.entryList();
    int indexDraft = files.indexOf(QRegularExpression("DRAFT.*"));
    pDebug("Last arena DRAFT: " + (indexDraft==-1?QString("Not Found"):files[indexDraft]));

    bool homelessArenaGames = true;//Evita mostrar juegos de arena sin draft
    for(int i=files.length()-1; i>=0; i--)
    {
        QString file = files[i];
        //Current arena draft or kept draft
        if((file.startsWith("DRAFT") && i < maxGamesLog) || i == indexDraft)
        {
            emit pDebug("Show Arena: " + file);
            homelessArenaGames = false;
            arenaHandler->showArenaLog(file);
        }
        //Current arena game or kept other games
        else if(((file.startsWith("ARENA") && i < indexDraft) || (i < maxGamesLog)) &&
                !(file.startsWith("ARENA") && homelessArenaGames))
        {
            emit pDebug("Show GameResut: " + file);
            arenaHandler->showGameResultLog(file);
        }
        else
        {
            dir.remove(file);
            pDebug(file + " removed.");
        }
    }

    ui->arenaTreeWidget->collapseAll();
}


void MainWindow::completeArenaDeck()
{
    if(arenaHandler == NULL)    return;

    QString arenaCurrentGameLog = arenaHandler->getArenaCurrentDraftLog();
    if(arenaCurrentGameLog.isEmpty())
    {
        pDebug("Completing Arena Deck: No draft log.");
    }
    else
    {
        pDebug("Completing Arena Deck: " + arenaCurrentGameLog);

        if(deckHandler != NULL) deckHandler->completeArenaDeck(arenaCurrentGameLog);
    }
}


//Config Tab
void MainWindow::addDraftMenu(QPushButton *button)
{
    QMenu *newArenaMenu = new QMenu(button);

    QSignalMapper* mapper = new QSignalMapper(button);

    for(int i=0; i<9; i++)
    {
        QAction *action = newArenaMenu->addAction(Utility::getHeroName(i));
        mapper->setMapping(action, Utility::getHeroName(i));
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    }

    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(confirmNewArenaDraft(QString)));

    button->setMenu(newArenaMenu);
}


void MainWindow::confirmNewArenaDraft(QString hero)
{
    int ret = QMessageBox::question(this, tr("New arena: ") + hero,
                                   "Make sure you have already picked " + hero + " in hearthstone. "
                                   "You shouldn't move hearthstone window until the end of the draft.\n\n"
                                   "Do you want to continue?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Ok)
    {
        pDebug("Manual draft: " + hero);
        pLog(tr("Menu: Force draft: ") + hero);
        QString heroLog = Utility::heroToLogNumber(hero);
        draftHandler->beginDraft(heroLog, deckHandler->getDeckCardList());
    }
}


void MainWindow::transparentAlways()
{
    spreadTransparency(Transparent);
}


void MainWindow::transparentAuto()
{
    spreadTransparency(AutoTransparent);
}


void MainWindow::transparentNever()
{
    spreadTransparency(Opaque);
}


void MainWindow::transparentFramed()
{
    spreadTransparency(Framed);
}


void MainWindow::spreadTransparency()
{
    spreadTransparency(this->transparency);
}


void MainWindow::spreadTransparency(Transparency newTransparency)
{
    this->transparency = newTransparency;

    bool kindOfTransparent = (transparency==Transparent || transparency==AutoTransparent);
    deckHandler->setTransparency(
                (this->deckWindow!=NULL && kindOfTransparent)?
                    Transparent:transparency);
    enemyDeckHandler->setTransparency(
                (this->enemyDeckWindow!=NULL && kindOfTransparent)?
                    Transparent:transparency);
    enemyHandHandler->setTransparency(
                (this->enemyWindow!=NULL && kindOfTransparent)?
                    Transparent:transparency);
    planHandler->setTransparency(transparency);
    arenaHandler->setTransparency(transparency);
    draftHandler->setTransparency(transparency);
    updateOtherTabsTransparency();

    showWindowFrame(transparency == Framed);

    if(arenaWindow != NULL)
    {
        arenaWindow->showWindowFrame(transparency == Framed);
        updateDetachWindowTheme(ui->tabArena);
    }
    if(enemyWindow != NULL)
    {
        enemyWindow->showWindowFrame(transparency == Framed);
        updateDetachWindowTheme(ui->tabEnemy);
    }
    if(deckWindow != NULL)
    {
        deckWindow->showWindowFrame(transparency == Framed);
        updateDetachWindowTheme(ui->tabDeck);
    }
    if(enemyDeckWindow != NULL)
    {
        enemyDeckWindow->showWindowFrame(transparency == Framed);
        updateDetachWindowTheme(ui->tabEnemyDeck);
    }
    if(planWindow != NULL)
    {
        planWindow->showWindowFrame(transparency == Framed);
        updateDetachWindowTheme(ui->tabPlan);
    }
}


void MainWindow::showWindowFrame(bool showFrame)
{
    if(showFrame)
    {
        this->setWindowFlags(Qt::Window|Qt::WindowStaysOnTopHint);
    }
    else
    {
        this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
    this->show();
}


//Update Config and Log tabs transparency
void MainWindow::updateOtherTabsTransparency()
{
    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabLog->setAttribute(Qt::WA_NoBackground);
        ui->tabLog->repaint();
        ui->tabConfig->setAttribute(Qt::WA_NoBackground);
        ui->tabConfig->repaint();

        QString groupBoxCSS =
                "QGroupBox {border: 2px solid " + ThemeHandler::themeColor2() + "; border-radius: 5px; margin-top: 5px; " +
                    ThemeHandler::bgWidgets() + " color: white;}"
                "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}";
        ui->configBoxActions->setStyleSheet(groupBoxCSS);
        ui->configBoxUI->setStyleSheet(groupBoxCSS);
        ui->configBoxDeck->setStyleSheet(groupBoxCSS);
        ui->configBoxHand->setStyleSheet(groupBoxCSS);
        ui->configBoxDraft->setStyleSheet(groupBoxCSS);
        ui->configBoxZero->setStyleSheet(groupBoxCSS);
        ui->configBoxDraftMethod->setStyleSheet(groupBoxCSS);
        ui->configBoxDraftExtra->setStyleSheet(groupBoxCSS);

        QString labelCSS = "QLabel {background-color: transparent; color: white;}";
        ui->configLabelDeckNormal->setStyleSheet(labelCSS);
        ui->configLabelDeckNormal2->setStyleSheet(labelCSS);
        ui->configLabelDeckTooltip->setStyleSheet(labelCSS);
        ui->configLabelDeckTooltip2->setStyleSheet(labelCSS);
        ui->configLabelDrawTime->setStyleSheet(labelCSS);
        ui->configLabelDrawTimeValue->setStyleSheet(labelCSS);
        ui->configLabelZero->setStyleSheet(labelCSS);
        ui->configLabelZero2->setStyleSheet(labelCSS);
        ui->configLabelTheme->setStyleSheet(labelCSS);

        QString radioCSS = "QRadioButton {background-color: transparent; color: white;}";
        ui->configRadioTransparent->setStyleSheet(radioCSS);
        ui->configRadioAuto->setStyleSheet(radioCSS);
        ui->configRadioOpaque->setStyleSheet(radioCSS);
        ui->configRadioFramed->setStyleSheet(radioCSS);
        ui->configRadioHA->setStyleSheet(radioCSS);
        ui->configRadioLF->setStyleSheet(radioCSS);
        ui->configRadioCombined->setStyleSheet(radioCSS);

        QString checkCSS = "QCheckBox {background-color: transparent; color: white;}";
        ui->configCheckClassColor->setStyleSheet(checkCSS);
        ui->configCheckSpellColor->setStyleSheet(checkCSS);
        ui->configCheckOverlay->setStyleSheet(checkCSS);
        ui->configCheckLearning->setStyleSheet(checkCSS);
        ui->configCheckNormalizeLF->setStyleSheet(checkCSS);
        ui->configCheckAutoSize->setStyleSheet(checkCSS);
        ui->configCheckManaLimits->setStyleSheet(checkCSS);
        ui->configCheckTotalAttack->setStyleSheet(checkCSS);
        ui->configCheckRngList->setStyleSheet(checkCSS);

        ui->logTextEdit->setStyleSheet("QTextEdit{" + ThemeHandler::bgWidgets() + " color: white;}");
    }
    else
    {
        ui->tabLog->setAttribute(Qt::WA_NoBackground, false);
        ui->tabLog->repaint();
        ui->tabConfig->setAttribute(Qt::WA_NoBackground, false);
        ui->tabConfig->repaint();

        ui->configBoxActions->setStyleSheet("");
        ui->configBoxUI->setStyleSheet("");
        ui->configBoxDeck->setStyleSheet("");
        ui->configBoxHand->setStyleSheet("");
        ui->configBoxDraft->setStyleSheet("");
        ui->configBoxZero->setStyleSheet("");
        ui->configBoxDraftMethod->setStyleSheet("");
        ui->configBoxDraftExtra->setStyleSheet("");


        ui->configLabelDeckNormal->setStyleSheet("");
        ui->configLabelDeckNormal2->setStyleSheet("");
        ui->configLabelDeckTooltip->setStyleSheet("");
        ui->configLabelDeckTooltip2->setStyleSheet("");
        ui->configLabelDrawTime->setStyleSheet("");
        ui->configLabelDrawTimeValue->setStyleSheet("");
        ui->configLabelZero->setStyleSheet("");
        ui->configLabelZero2->setStyleSheet("");
        ui->configLabelTheme->setStyleSheet("");

        ui->configRadioTransparent->setStyleSheet("");
        ui->configRadioAuto->setStyleSheet("");
        ui->configRadioOpaque->setStyleSheet("");
        ui->configRadioFramed->setStyleSheet("");
        ui->configRadioHA->setStyleSheet("");
        ui->configRadioLF->setStyleSheet("");
        ui->configRadioCombined->setStyleSheet("");

        ui->configCheckClassColor->setStyleSheet("");
        ui->configCheckSpellColor->setStyleSheet("");
        ui->configCheckOverlay->setStyleSheet("");
        ui->configCheckLearning->setStyleSheet("");
        ui->configCheckNormalizeLF->setStyleSheet("");
        ui->configCheckAutoSize->setStyleSheet("");
        ui->configCheckManaLimits->setStyleSheet("");
        ui->configCheckTotalAttack->setStyleSheet("");
        ui->configCheckRngList->setStyleSheet("");

        ui->logTextEdit->setStyleSheet("");
    }
}


void MainWindow::fadeBarAndButtons(bool fadeOut)
{
    if(fadeOut)
    {
        bool inTabEnemy = ui->tabWidget->currentWidget() == ui->tabEnemy;
        if(inTabEnemy && enemyHandHandler->isEmpty())
        {
            Utility::fadeInWidget(ui->tabWidget->tabBar());
            Utility::fadeInWidget(ui->tabWidgetH2->tabBar());
            Utility::fadeInWidget(ui->tabWidgetH3->tabBar());
        }
        else
        {
            Utility::fadeOutWidget(ui->tabWidget->tabBar());
            Utility::fadeOutWidget(ui->tabWidgetH2->tabBar());
            Utility::fadeOutWidget(ui->tabWidgetH3->tabBar());
        }
        Utility::fadeOutWidget(ui->minimizeButton);
        Utility::fadeOutWidget(ui->closeButton);
        Utility::fadeOutWidget(ui->resizeButton);
    }
    else
    {
        Utility::fadeInWidget(ui->tabWidget->tabBar());
        Utility::fadeInWidget(ui->tabWidgetH2->tabBar());
        Utility::fadeInWidget(ui->tabWidgetH3->tabBar());
        Utility::fadeInWidget(ui->minimizeButton);
        Utility::fadeInWidget(ui->closeButton);
        Utility::fadeInWidget(ui->resizeButton);
    }

    updateTabWidgetsTheme(fadeOut, false);//Si usamos un theme con bordes los oculta
}


void MainWindow::redrawAllGames()
{
    emit pDebug("Redraw all games.");
    arenaHandler->clearAllGames();

    QFileInfo dirInfo(Utility::gameslogPath());
    if(!dirInfo.exists())
    {
        pDebug("Cannot check GamesLog dir. Dir doesn't exist.");
        return;
    }

    QDir dir(Utility::gameslogPath());
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QStringList filterName;
    filterName << "*.arenatracker";
    dir.setNameFilters(filterName);

    QStringList files = dir.entryList();

    for(int i=files.length()-1; i>=0; i--)
    {
        QString file = files[i];
        if(file.startsWith("DRAFT"))
        {
            emit pDebug("Show Arena: " + file);
            arenaHandler->showArenaLog(file);
        }
        else
        {
            emit pDebug("Show GameResut: " + file);
            arenaHandler->showGameResultLog(file);
        }
    }
}


void MainWindow::spreadTheme(bool redrawAllGames)
{
    updateMainUITheme();
    updateTabIcons();
    arenaHandler->setTheme();
    deckHandler->setTheme();
    draftHandler->setTheme();
    planHandler->setTheme();
    enemyHandHandler->setTheme();
    deckHandler->redrawAllCards();
    draftHandler->redrawAllCards();
    enemyDeckHandler->redrawAllCards();
    enemyHandHandler->redrawAllCards();
    if(redrawAllGames) this->redrawAllGames();
    resizeChecks();//Recoloca botones -X
    calculateMinimumWidth();//Si hay borde cambia el minimumWidth
}


void MainWindow::updateTabIcons()
{
    QWidget * currentTab = ui->tabWidget->currentWidget();

    ui->tabWidget->hide();
    if(arenaWindow == NULL)     moveTabTo(ui->tabArena, ui->tabWidget);
    if(enemyWindow == NULL)     moveTabTo(ui->tabEnemy, ui->tabWidget);
    if(deckWindow == NULL)      moveTabTo(ui->tabDeck, ui->tabWidget);
    if(enemyDeckWindow == NULL) moveTabTo(ui->tabEnemyDeck, ui->tabWidget);
    if(planWindow == NULL)      moveTabTo(ui->tabPlan, ui->tabWidget);
    moveTabTo(ui->tabConfig, ui->tabWidget);
    ui->tabWidget->show();

    ui->tabWidget->setCurrentWidget(currentTab);
}


void MainWindow::updateTabWidgetsTheme(bool transparent, bool resizing)
{
    int maxWidth = ui->tabWidget->width() - ThemeHandler::borderWidth()*2 - SMALL_BUTTONS_H;
    ui->tabWidget->setTheme("left", maxWidth, resizing, transparent);

    if(!resizing)
    {
        ui->tabWidgetH2->setTheme("center", ui->tabWidgetH2->width(), resizing, transparent);
        ui->tabWidgetH3->setTheme("center", ui->tabWidgetH3->width(), resizing, transparent);
        ui->tabWidgetV1->setTheme("left", ui->tabWidgetV1->width(), resizing, transparent);
    }
}


void MainWindow::updateMainUITheme()
{
    QFont font(ThemeHandler::defaultFont());
    font.setPixelSize(15);
    QApplication::setFont(font);
    font.setPixelSize(16);
    ui->progressBar->setFont(font);

    updateTabWidgetsTheme(false, false);
    updateButtonsTheme();

    QString mainCSS = "";
    mainCSS +=
            "QMenu {background: " + ThemeHandler::bgMenuColor() + "; color: " + ThemeHandler::fgMenuColor() + ";}"
            "QMenu::item {padding: 2px 25px 2px 20px;border: 1px solid transparent;}"
            "QMenu::item:selected {background-color: " + ThemeHandler::bgSelectedItemMenuColor() + "; "
                "color: " + ThemeHandler::fgSelectedItemMenuColor() + "; "
                "border-color: " + ThemeHandler::bgSelectedItemMenuColor() + ";}"

            "QScrollBar:vertical {background-color: transparent; border: 2px solid " + ThemeHandler::themeColor2() + "; "
                "width: 15px; margin: 15px 0px 15px 0px;}"
            "QScrollBar::handle:vertical {background: " + ThemeHandler::themeColor1() + "; min-height: 20px;}"
            "QScrollBar::add-line:vertical {border: 2px solid " + ThemeHandler::themeColor2() + ";background: " + ThemeHandler::themeColor1() + "; "
                "height: 15px; subcontrol-position: bottom; subcontrol-origin: margin;}"
            "QScrollBar::sub-line:vertical {border: 2px solid " + ThemeHandler::themeColor2() + ";background: " + ThemeHandler::themeColor1() + "; "
                "height: 15px; subcontrol-position: top; subcontrol-origin: margin;}"
            "QScrollBar:up-arrow:vertical, QScrollBar::down-arrow:vertical {border: 2px solid " + ThemeHandler::themeColor1() + "; "
                "width: 3px; height: 3px; background: " + ThemeHandler::themeColor2() + ";}"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none;}"

            "QProgressBar {border: 2px solid " + ThemeHandler::borderProgressBarColor() + "; color: " + ThemeHandler::fgProgressBarColor() + "; "
                "background-color: " + ThemeHandler::bgProgressBarColor() + ";}"
            "QProgressBar::chunk {background-color: " + ThemeHandler::chunkProgressBarColor() + ";}"

            "QDialog {" + ThemeHandler::bgApp() + ";}"
            "QPushButton {background: " + ThemeHandler::themeColor1() + "; color: " + ThemeHandler::fgColor() + ";}"
            "QToolTip {border: 2px solid " + ThemeHandler::borderTooltipColor() + "; border-radius: 2px; "
                "color: " + ThemeHandler::fgTooltipColor() + "; background: " + ThemeHandler::bgTooltipColor() + ";}"

            "QGroupBox {border: 2px solid " + ThemeHandler::themeColor2() + "; border-radius: 5px; "
                "margin-top: 5px; " + ThemeHandler::bgWidgets() + " color: " + ThemeHandler::fgColor() + ";}"
            "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}"
            "QLabel {background-color: transparent; color: " + ThemeHandler::fgColor() + ";}"
            "QTextBrowser {" + ThemeHandler::bgWidgets() + " color: " + ThemeHandler::fgColor() + ";}"
            "QRadioButton {background-color: transparent; color: " + ThemeHandler::fgColor() + ";}"
            "QCheckBox {background-color: transparent; color: " + ThemeHandler::fgColor() + ";}"
            "QTextEdit{" + ThemeHandler::bgWidgets() + " color: " + ThemeHandler::fgColor() + ";}"

            "QLineEdit {border: 2px solid " + ThemeHandler::borderLineEditColor() + ";border-radius: 5px; "
                "background: " + ThemeHandler::bgLineEditColor() + "; color: " + ThemeHandler::fgLineEditColor() + "; "
                "selection-background-color: " + ThemeHandler::bgSelectionLineEditColor() + "; "
                "selection-color: " + ThemeHandler::fgSelectionLineEditColor() + ";}"

            "QComboBox {background: " + ThemeHandler::bgMenuColor() + "; color: " + ThemeHandler::fgMenuColor() + "; "
                "selection-background-color: " + ThemeHandler::bgSelectedItemMenuColor() + ";"
                "selection-color: "+ ThemeHandler::fgSelectedItemMenuColor() +";}"
            "QComboBox QAbstractItemView{background: " + ThemeHandler::bgMenuColor() + "; "
                "color: " + ThemeHandler::fgMenuColor() + "; "
                "selection-background-color: " + ThemeHandler::bgSelectedItemMenuColor() + "; "
                "selection-color: "+ ThemeHandler::fgSelectedItemMenuColor() +";}"
            ;

    this->setStyleSheet(mainCSS);
    updateAllDetachWindowTheme(mainCSS);
}


void MainWindow::updateAllDetachWindowTheme(const QString &mainCSS)
{
    if(arenaWindow != NULL)
    {
        arenaWindow->setStyleSheet(mainCSS);
        arenaWindow->spreadTheme();
    }
    if(enemyWindow != NULL)
    {
        enemyWindow->setStyleSheet(mainCSS);
        enemyWindow->spreadTheme();
    }
    if(deckWindow != NULL)
    {
        deckWindow->setStyleSheet(mainCSS);
        deckWindow->spreadTheme();
    }
    if(enemyDeckWindow != NULL)
    {
        enemyDeckWindow->setStyleSheet(mainCSS);
        enemyDeckWindow->spreadTheme();
    }
    if(planWindow != NULL)
    {
        planWindow->setStyleSheet(mainCSS);
        planWindow->spreadTheme();
    }

    updateDetachWindowTheme(ui->tabArena);
    updateDetachWindowTheme(ui->tabEnemy);
    updateDetachWindowTheme(ui->tabDeck);
    updateDetachWindowTheme(ui->tabEnemyDeck);
    updateDetachWindowTheme(ui->tabPlan);
}


void MainWindow::updateDetachWindowTheme(QWidget *paneWidget)
{
    DetachWindow *detachWindow;
    QString paneWidgetName;
    bool showThemeBackground = false;
    bool paneBorder;

    if(paneWidget == ui->tabArena)
    {
            detachWindow = arenaWindow;
            paneWidgetName = "TabArena";
            paneBorder = true;
            showThemeBackground = (detachWindow != NULL &&
                    (transparency == Framed || transparency == Opaque || transparency == AutoTransparent));
    }
    else if(paneWidget == ui->tabEnemy)
    {
            detachWindow = enemyWindow;
            paneWidgetName = "TabEnemy";
            paneBorder = false;
            showThemeBackground = (detachWindow != NULL &&
                    (transparency == Framed || transparency == Opaque));
    }
    else if(paneWidget == ui->tabDeck)
    {
            detachWindow = deckWindow;
            paneWidgetName = "TabDeck";
            paneBorder = false;
            showThemeBackground = (detachWindow != NULL &&
                    (transparency == Framed || transparency == Opaque));
    }
    else if(paneWidget == ui->tabEnemyDeck)
    {
            detachWindow = enemyDeckWindow;
            paneWidgetName = "TabEnemyDeck";
            paneBorder = false;
            showThemeBackground = (detachWindow != NULL &&
                    (transparency == Framed || transparency == Opaque));
    }
    else if(paneWidget == ui->tabPlan)
    {
            detachWindow = planWindow;
            paneWidgetName = "TabPlan";
            paneBorder = true;
            showThemeBackground = (detachWindow != NULL &&
                    (transparency == Framed || transparency == Opaque || transparency == AutoTransparent));
    }

    if(showThemeBackground)
    {
        paneWidget->setStyleSheet("QWidget#" + paneWidgetName + " { " +
            ThemeHandler::bgApp() + ThemeHandler::borderApp(false) +" }");
        paneWidget->layout()->setContentsMargins(ThemeHandler::borderWidth() + (paneBorder?10:0),
                                                 ThemeHandler::borderWidth() + (paneBorder?10:0),
                                                 ThemeHandler::borderWidth() + (paneBorder?10:0),
                                                 ThemeHandler::borderWidth() + (paneBorder?10:0));
        calculateCardWindowMinimumWidth(detachWindow, true);
    }
    else
    {
        paneWidget->setStyleSheet("");
        if(detachWindow == NULL)
        {
            paneWidget->layout()->setContentsMargins((paneBorder?10:0), 40 + (paneBorder?5:0),
                                                     (paneBorder?10:0), (paneBorder?10:0));
        }
        else
        {
            paneWidget->layout()->setContentsMargins((paneBorder?10:0), (paneBorder?10:0),
                                                     (paneBorder?10:0), (paneBorder?10:0));
            calculateCardWindowMinimumWidth(detachWindow, false);
        }
    }
}


void MainWindow::updateButtonsTheme()
{
    ui->closeButton->setStyleSheet("QPushButton {background: " + ThemeHandler::bgTopButtonsColor() + "; border: none;}"
                                   "QPushButton:hover {background: " + ThemeHandler::hoverTopButtonsColor() + ";}");
    ui->minimizeButton->setStyleSheet("QPushButton {background: " + ThemeHandler::bgTopButtonsColor() + "; border: none;}"
                                      "QPushButton:hover {background: " + ThemeHandler::hoverTopButtonsColor() + ";}");

    ui->closeButton->setIcon(QIcon(ThemeHandler::buttonCloseFile()));
    ui->minimizeButton->setIcon(QIcon(ThemeHandler::buttonMinimizeFile()));
    ui->configButtonForceDraft->setIcon(QIcon(ThemeHandler::buttonForceDraftFile()));

    QList<QAction *> actions = ui->configButtonForceDraft->menu()->actions();
    for(int i=0; i<actions.count(); i++)
    {
        actions[i]->setIcon(QIcon(ThemeHandler::heroFile(Utility::getHeroLogNumber(i))));
    }

    ui->resizeButton->setIcon(QIcon(ThemeHandler::buttonResizeFile()));
}


int MainWindow::getAutoTamCard()
{
    int numCards = deckHandler->getNumCardRows();
    int deckHeight = ui->tabDeck->height();
    if(this->deckWindow == NULL)   deckHeight -= 40;

    if(numCards > 0)    return deckHeight/numCards;
    else                return -1;
}


int MainWindow::getTamCard()
{
    bool autoSize = ui->configCheckAutoSize->isChecked();

    int tamCardSlider = ui->configSliderCardSize->value();

    if(autoSize)
    {
        int tamCardAuto = getAutoTamCard();
        if(tamCardAuto == -1)   return tamCardSlider;
        else                    return std::min(tamCardAuto, tamCardSlider);
    }
    else
    {
        return tamCardSlider;
    }
}


void MainWindow::spreadTamCard(int value)
{
    if(value < ui->configSliderCardSize->minimum()) value = ui->configSliderCardSize->minimum();
    if(this->cardHeight == value) return;

    this->cardHeight = value;
    DeckCard::setCardHeight(value);

    if(deckHandler != NULL)
    {
        deckHandler->updateIconSize(value);
        deckHandler->redrawAllCards();
    }

    if(enemyDeckHandler != NULL)    enemyDeckHandler->redrawAllCards();
    if(secretsHandler != NULL)      secretsHandler->redrawAllCards();
    if(enemyHandHandler != NULL)
    {
        enemyHandHandler->redrawAllCards();
        enemyHandHandler->redrawTotalAttack();
    }

    if(draftHandler != NULL)
    {
        draftHandler->updateTamCard();
        draftHandler->redrawAllCards();
    }

    bool windowsWithBorders = (transparency == Framed || transparency == Opaque);
    if(deckWindow != NULL)      calculateCardWindowMinimumWidth(deckWindow, windowsWithBorders);
    if(enemyWindow != NULL)     calculateCardWindowMinimumWidth(enemyWindow, windowsWithBorders);
    if(enemyDeckWindow != NULL) calculateCardWindowMinimumWidth(enemyDeckWindow, windowsWithBorders);
}


void MainWindow::spreadCorrectTamCard()
{
    spreadTamCard(getTamCard());
}


void MainWindow::updateTamCard(int value)
{
    spreadCorrectTamCard();

    QString labelText = QString::number(value) + " px";
    ui->configSliderCardSize->setToolTip(labelText);
    ui->configLabelDeckNormal2->setText(labelText);
}


void MainWindow::updateTooltipScale(int value)
{
    cardWindow->scale(value);

    QString labelText;
    if(value < 10)  labelText = "OFF";
    else            labelText = "x"+QString::number(value/10.0);
    ui->configSliderTooltipSize->setToolTip(labelText);
    ui->configLabelDeckTooltip2->setText(labelText);
}


void MainWindow::updateShowClassColor(bool checked)
{
    DeckCard::setDrawClassColor(checked);
    deckHandler->redrawClassCards();
    secretsHandler->redrawClassCards();
    enemyHandHandler->redrawClassCards();
    draftHandler->redrawAllCards();
    enemyDeckHandler->redrawClassCards();
}


void MainWindow::updateShowSpellColor(bool checked)
{
    DeckCard::setDrawSpellWeaponColor(checked);
    deckHandler->redrawSpellWeaponCards();
    secretsHandler->redrawSpellWeaponCards();
    enemyHandHandler->redrawSpellWeaponCards();
    draftHandler->redrawAllCards();
    enemyDeckHandler->redrawSpellWeaponCards();
}


void MainWindow::updateShowManaLimits(bool checked)
{
    deckHandler->setShowManaLimits(checked);
}


//Valores drawDisappear:
//  -1  No show
//  0   Turn
//  n   Ns
void MainWindow::updateTimeDraw(int value)
{
    //Slider            0  - Ns - 11
    //DrawDissapear     -1 - Ns - 0

    QString labelText;

    switch(value)
    {
        case 0:
            this->drawDisappear = -1;
            labelText = "No";
            break;
        case 11:
            this->drawDisappear = 0;
            labelText = "Turn";
            break;
        default:
            this->drawDisappear = value;
            labelText = QString::number(value) + "s";
            break;
    }

    ui->configLabelDrawTimeValue->setText(labelText);
    ui->configSliderDrawTime->setToolTip(labelText);

    deckHandler->setDrawDisappear(this->drawDisappear);
}


void MainWindow::updateShowTotalAttack(bool checked)
{
    enemyHandHandler->setShowAttackBar(checked);
}


void MainWindow::updateShowRngList(bool checked)
{
    deckHandler->setShowRngList(checked);
}


void MainWindow::toggleShowDraftOverlay()
{
    this->showDraftOverlay = !this->showDraftOverlay;
    draftHandler->setShowDraftOverlay(this->showDraftOverlay);
}


void MainWindow::toggleDraftLearningMode()
{
    this->draftLearningMode = !this->draftLearningMode;
    draftHandler->setLearningMode(this->draftLearningMode);
}


void MainWindow::updateDraftNormalizeLF(bool checked)
{
    draftHandler->setNormalizedLF(checked);
}


void MainWindow::draftMethodHA()
{
    spreadDraftMethod(HearthArena);
}


void MainWindow::draftMethodLF()
{
    spreadDraftMethod(LightForge);
}


void MainWindow::draftMethodCombined()
{
    spreadDraftMethod(All);
}


void MainWindow::spreadDraftMethod(DraftMethod draftMethod)
{
    this->draftMethod = draftMethod;
    draftHandler->setDraftMethod(draftMethod);
}


void MainWindow::updateMaxGamesLog(int value)
{
    if(value == 0)
    {
        copyGameLogs = true;//false;//Siempre copiamos los logs de la sesion actual para poder completar el DRAFT actual.
    }
    else
    {
        copyGameLogs = true;
    }
    gameWatcher->setCopyGameLogs(copyGameLogs);

    QString labelText;
    if(value == 100)
    {
        labelText = "ALL";
    }
    else if(value == 0)
    {
        labelText = "MIN";
    }
    else
    {
        labelText = QString::number(ui->configSliderZero->value());
    }

    ui->configLabelZero2->setText(labelText);
    ui->configSliderZero->setToolTip(labelText);
}


void MainWindow::completeConfigComboTheme()
{
    QDir themesDir(Utility::themesPath());
    for(const QFileInfo &themeFI : themesDir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot))
    {
        ui->configComboTheme->addItem(themeFI.fileName());
    }

    ui->configComboTheme->setEditable(false);

    connect(ui->configComboTheme, SIGNAL(activated(QString)),
            this, SLOT(loadTheme(QString)));
}


void MainWindow::completeConfigTab()
{
    //Cambiar en Designer margenes/spacing de nuevos configBox a 5-9-5-9/5
    //Actions
    addDraftMenu(ui->configButtonForceDraft);
    //connect en createDeckHandler

    //UI
    connect(ui->configRadioTransparent, SIGNAL(clicked()), this, SLOT(transparentAlways()));
    connect(ui->configRadioAuto, SIGNAL(clicked()), this, SLOT(transparentAuto()));
    connect(ui->configRadioOpaque, SIGNAL(clicked()), this, SLOT(transparentNever()));
    connect(ui->configRadioFramed, SIGNAL(clicked()), this, SLOT(transparentFramed()));

    completeConfigComboTheme();

    //Deck
    connect(ui->configSliderCardSize, SIGNAL(valueChanged(int)), this, SLOT(updateTamCard(int)));
    connect(ui->configSliderTooltipSize, SIGNAL(valueChanged(int)), this, SLOT(updateTooltipScale(int)));
    connect(ui->configCheckAutoSize, SIGNAL(clicked()), this, SLOT(spreadCorrectTamCard()));
    connect(ui->configCheckClassColor, SIGNAL(clicked(bool)), this, SLOT(updateShowClassColor(bool)));
    connect(ui->configCheckSpellColor, SIGNAL(clicked(bool)), this, SLOT(updateShowSpellColor(bool)));
    connect(ui->configCheckManaLimits, SIGNAL(clicked(bool)), this, SLOT(updateShowManaLimits(bool)));

    //Hand
    connect(ui->configSliderDrawTime, SIGNAL(valueChanged(int)), this, SLOT(updateTimeDraw(int)));
    connect(ui->configCheckTotalAttack, SIGNAL(clicked(bool)), this, SLOT(updateShowTotalAttack(bool)));
    connect(ui->configCheckRngList, SIGNAL(clicked(bool)), this, SLOT(updateShowRngList(bool)));

    //Draft
    connect(ui->configCheckOverlay, SIGNAL(clicked()), this, SLOT(toggleShowDraftOverlay()));
    connect(ui->configCheckLearning, SIGNAL(clicked()), this, SLOT(toggleDraftLearningMode()));
    connect(ui->configCheckNormalizeLF, SIGNAL(clicked(bool)), this, SLOT(updateDraftNormalizeLF(bool)));
    connect(ui->configRadioHA, SIGNAL(clicked()), this, SLOT(draftMethodHA()));
    connect(ui->configRadioLF, SIGNAL(clicked()), this, SLOT(draftMethodLF()));
    connect(ui->configRadioCombined, SIGNAL(clicked()), this, SLOT(draftMethodCombined()));

    //Zero To Heroes
    connect(ui->configSliderZero, SIGNAL(valueChanged(int)), this, SLOT(updateMaxGamesLog(int)));


    completeHighResConfigTab();
}


void MainWindow::completeHighResConfigTab()
{
    int screenHeight = getScreenHighest();
    if(screenHeight < 1000) return;

    int maxCard = (int)(screenHeight/1000.0*50);
    maxCard -= maxCard%5;
    ui->configSliderCardSize->setMaximum(maxCard);

    int maxTooltip = (int)(screenHeight/1000.0*15);
    maxTooltip -= maxTooltip%5;
    ui->configSliderTooltipSize->setMaximum(maxTooltip);
}


int MainWindow::getScreenHighest()
{
    int height = 0;

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();
        if(geometry.height()>height)    height = geometry.height();
    }
    return height;
}


LoadingScreenState MainWindow::getLoadingScreen()
{
    if(gameWatcher != NULL) return gameWatcher->getLoadingScreen();
    else                    return menu;
}


void MainWindow::createDebugPack()
{
    QString timeStamp = QDateTime::currentDateTime().toString("MMMM-d hh-mm-ss");
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/ATbugs/" + timeStamp;
    QDir dir(dirPath);
    dir.mkpath(dirPath);

    QList<QScreen *> screens = QGuiApplication::screens();
    for(int screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;

        QRect rect = screen->geometry();
        QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();
        image.save(dirPath + "/screenshot.png");

        cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), image.bytesPerLine());
        cv::resize(mat, mat, cv::Size(1280, 720));
        cv::imshow("Screenshot", mat);
    }

    QFile atLog(Utility::dataPath() + "/ArenaTrackerLog.txt");
    atLog.copy(dirPath + "/ArenaTrackerLog.txt");

    QString hsLogsPath = logLoader->getLogsDirPath();
    QFile arenaLog(hsLogsPath + "/Arena.log");
    arenaLog.copy(dirPath + "/Arena.log");
    QFile loadingScreenLog(hsLogsPath + "/LoadingScreen.log");
    loadingScreenLog.copy(dirPath + "/LoadingScreen.log");
    QFile powerLog(hsLogsPath + "/Power.log");
    powerLog.copy(dirPath + "/Power.log");
    QFile zoneLog(hsLogsPath + "/Zone.log");
    zoneLog.copy(dirPath + "/Zone.log");

    emit pDebug("Bug pack " + dirPath + " created.");
}


void MainWindow::showMessageProgressBar(QString text, int hideDelay)
{
    ui->progressBar->setFormat(text);

    if(ui->progressBar->value() != ui->progressBar->maximum())
    {
        emit pDebug("Progress bar message received while counting. " + text, Warning);
        if(!ui->progressBar->isVisible())   showProgressBar(false);
    }
    else
    {
        if(!ui->progressBar->isVisible())   showProgressBar(true);
        QTimer::singleShot(hideDelay, this, SLOT(hideProgressBar()));
    }
}


void MainWindow::startProgressBar(int maximum, QString text)
{
    ui->progressBar->setMaximum(maximum);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat(text);

    if(!ui->progressBar->isVisible())   showProgressBar(false);
}


void MainWindow::advanceProgressBar(int remaining, QString text)
{
    if(remaining <= 0)
    {
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
    else
    {
        if(remaining > ui->progressBar->maximum())  ui->progressBar->setMaximum(remaining);
        ui->progressBar->setValue(ui->progressBar->maximum()-remaining);
    }
    if(!text.isEmpty())     ui->progressBar->setFormat(text);
}


void MainWindow::showProgressBar(bool animated)
{
    if(ui->progressBar->isVisible())
    {
        pDebug("Trying to show progress bar already shown.", Warning);
        return;
    }

    ui->progressBar->setVisible(true);

    if(animated)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(ui->progressBar, "maximumHeight");
        animation->setDuration(ANIMATION_TIME);
        animation->setStartValue(0);
        animation->setEndValue(ui->progressBar->height());
        animation->setEasingCurve(SHOW_EASING_CURVE);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
    else
    {
        ui->progressBar->setMaximumHeight(16777215);
    }
}


void MainWindow::hideProgressBar()
{
    if(ui->progressBar->value() != ui->progressBar->maximum())
    {
        pDebug("Trying to hide progress bar while counting.", Warning);
        return;
    }

    if(!ui->progressBar->isVisible())
    {
        pDebug("Trying to hide progress bar already hidden.", Warning);
        return;
    }

    QPropertyAnimation *animation = new QPropertyAnimation(ui->progressBar, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->progressBar->height());
    animation->setEndValue(0);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(
        animation, &QPropertyAnimation::finished,
        [=]()
        {
            if(ui->progressBar->value() != ui->progressBar->maximum())
            {
                pDebug("Finish hiding progress bar while counting.", Warning);
                ui->progressBar->setVisible(true);
            }
            else
            {
                ui->progressBar->setVisible(false);
            }
            ui->progressBar->setMaximumHeight(16777215);
        }
    );
}


void MainWindow::startProgressBarMini(int maximum)
{
    ui->progressBarMini->setMaximum(maximum);
    ui->progressBarMini->setMinimum(0);
    ui->progressBarMini->setValue(0);
    ui->progressBarMini->setVisible(true);
}


void MainWindow::hideProgressBarMini()
{
    advanceProgressBarMini(0);
}


void MainWindow::advanceProgressBarMini(int remaining)
{
    if(remaining <= 0)
    {
        ui->progressBarMini->setVisible(false);
        ui->progressBarMini->setValue(ui->progressBarMini->maximum());
    }
    else
    {
        if(remaining > ui->progressBarMini->maximum())  ui->progressBarMini->setMaximum(remaining);
        ui->progressBarMini->setValue(ui->progressBarMini->maximum()-remaining);
    }
}


void MainWindow::checkFirstRunNewVersion()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runVersion = settings.value("runVersion", "").toString();

    if(runVersion != VERSION)
    {
        settings.setValue("allCardsDownloaded", false);
    }
}


void MainWindow::downloadAllArenaCodes()
{
    if(draftHandler == NULL)    return;

    QSettings settings("Arena Tracker", "Arena Tracker");
    bool allCardsDownloaded = settings.value("allCardsDownloaded", false).toBool();
    if(allCardsDownloaded)
    {
        emit pDebug("All arena cards were already downloaded.");
    }
    else
    {
        emit pDebug("Downloading all arena cards.");
        allCardsDownloadList.clear();
        QStringList codeList = draftHandler->getAllArenaCodes();
        for(const QString code: codeList)
        {
            if(!checkCardImage(code))
            {
                allCardsDownloadList.append(code);
            }
            if(!checkCardImage(code + "_premium"))
            {
                allCardsDownloadList.append(code + "_premium");
            }
        }

        if(allCardsDownloadList.isEmpty())  this->allCardsDownloaded();
        else
        {
            startProgressBarMini(allCardsDownloadList.count());
            showMessageProgressBar("Downloading cards...");
        }
    }
}


void MainWindow::updateProgressAllCardsDownload(QString code)
{
    if(allCardsDownloadList.removeOne(code))
    {
        advanceProgressBarMini(allCardsDownloadList.count());
    }
}


void MainWindow::allCardsDownloaded()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    bool allCardsDownloaded = settings.value("allCardsDownloaded", false).toBool();
    if(!allCardsDownloaded)
    {
        settings.setValue("allCardsDownloaded", true);
        allCardsDownloadList.clear();
        hideProgressBarMini();
        showMessageProgressBar("All cards downloaded");
        emit pDebug("All arena cards have been downloaded.");
    }
}


void MainWindow::loadTheme(QString theme, bool initTheme)
{
    if(ThemeHandler::loadTheme(theme))
    {
        spreadTheme(!initTheme);
        if(!initTheme)  showMessageProgressBar("Theme " + theme + " loaded");
    }
    else
    {
        if(initTheme)   spreadTheme(!initTheme);
        else            showMessageProgressBar("Theme " + theme + " invalid");
    }
}


void MainWindow::showPremiumDialog()
{
    QMessageBox msgBox(this);
    msgBox.setText("Becoming a patron (3$+) will let you activate the premium version of Arena Tracker, "
                   "which implements some extra features: (replays, planning, synergies and draft mechanics overview). "
                   "<a href='https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/More.md'>Learn more...</a>"
                   "<br><br>If you are already a patron use the \"Unlock premium\" button and "
                   "type the e-mail address that appears in your patron profile. "
                   "Your version will be upgraded in less than 24 hours."
                   "<br><br>Thanks for your support!");
    msgBox.setWindowTitle(tr("Premium"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setIcon(QMessageBox::Information);
    QPushButton *button1 = msgBox.addButton("Become a patron", QMessageBox::ActionRole);
    QPushButton *button2 = msgBox.addButton("Unlock premium", QMessageBox::ActionRole);
    QPushButton *button3 = msgBox.addButton("Cancel", QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton() == button1)
    {
        QDesktopServices::openUrl(QUrl(
            "https://www.patreon.com/triodo"
            ));
    }
    else if(msgBox.clickedButton() == button2)
    {
        bool ok;
        QString email = QInputDialog::getText(this, tr("Unlock premium"),
                                                tr("Patron e-mail:"), QLineEdit::Normal,
                                                "", &ok);
        if(ok)
        {
            premiumHandler->unlockPremium(email);
            showMessageProgressBar("Premium request sent");
        }
    }
    else if(msgBox.clickedButton() == button3)
    {
    }
}


void MainWindow::test()
{
//    testPlan();
    QTimer::singleShot(1000, this, SLOT(testDelay()));
}


void MainWindow::testPlan()
{
    planHandler->playerMinionZonePlayAdd("AT_003", 1, 1);
    planHandler->enemyMinionZonePlayAdd("AT_042t2", 2, 1);
    planHandler->playerMinionZonePlayAdd("CS1_042", 3, 1);
    planHandler->playerMinionZonePlayAdd(FLAMEWAKER, 5, 1);
    planHandler->playerMinionZonePlayAdd(FLAMEWAKER, 6, 1);
    planHandler->playerMinionZonePlayAdd(FLAMEWAKER, 7, 1);
    planHandler->enemyMinionZonePlayAdd("EX1_020", 4, 1);
    planHandler->enemyMinionZonePlayAdd(FLAMEWAKER, 7, 1);
    planHandler->playerHeroZonePlayAdd("HERO_08", 11);
    planHandler->enemyHeroZonePlayAdd("HERO_09", 12);
    planHandler->playerHeroPowerZonePlayAdd("CS1h_001", 13);

    planHandler->newTurn(true, 1);
    planHandler->playerCardDraw(22, "EX1_384",2);
    planHandler->playerCardDraw(23, "OG_116",2);
    planHandler->playerCardDraw(21, "GVG_090",2);
    planHandler->playerCardDraw(21, "EX1_082",2);
    planHandler->playerCardDraw(24, "EX1_277",2);
    planHandler->playerCardDraw(41, "GVG_004",2);
    planHandler->playerCardDraw(44, "BRM_002",2);
    planHandler->playerCardDraw(45, "GVG_050",2);
    planHandler->zonePlayAttack("AT_003",1,2);
    planHandler->zonePlayAttack("AT_003",3,2);
    planHandler->zonePlayAttack("AT_003",11,4);
    planHandler->playerSecretPlayed(25, "EX1_611");
    planHandler->playerSecretPlayed(26, "EX1_594");
    planHandler->playerSecretPlayed(27, "EX1_294");
    planHandler->playerSecretPlayed(28, "EX1_130");
    planHandler->enemySecretPlayed(29, MAGE);

    planHandler->newTurn(false, 2);
    planHandler->enemyMinionZonePlayAdd("AT_007", 5, 1);
    planHandler->zonePlayAttack("AT_003",12,11);
    planHandler->zonePlayAttack("AT_003",12,11);
    planHandler->setLastTriggerId("", "FATIGUE", 0, 0);
    planHandler->playerMinionTagChange(11, "", "DAMAGE", "1");
    planHandler->enemyCardObjPlayed("EX1_020", 4, 1);
    planHandler->setLastTriggerId("CS2_034", "TRIGGER", 134, -1);
    planHandler->playerMinionTagChange(1, "","DAMAGE", "1");
//    planHandler->playerMinionTagChange(93, "BRM_027h", "LINKED_ENTITY", "11");
    planHandler->playerMinionZonePlayRemove(1);
    planHandler->playerMinionZonePlayRemove(3);
    planHandler->enemyCardDraw(22, "AT_003", "",2);
    planHandler->enemyCardDraw(23, "CS1_042", "",2);
    planHandler->enemyCardDraw(21, "", "",2);
    planHandler->enemyCardDraw(21, "", "",32);
    planHandler->enemyCardDraw(24, "AT_002", "",2);
    planHandler->playerSecretRevealed(25, "EX1_611");
    planHandler->playerSecretRevealed(26, "EX1_594");
    planHandler->playerSecretRevealed(27, "EX1_294");
    planHandler->playerSecretRevealed(28, "EX1_130");

    planHandler->newTurn(true, 3);
    planHandler->enemyIsolatedSecret(29, "EX1_136");
    planHandler->enemySecretPlayed(30, MAGE);

}


void MainWindow::testSynergies()
{
//    draftHandler->debugSynergiesSet("GVG");
//    draftHandler->debugSynergiesCode("LOOT_520");
//    draftHandler->testSynergies();
}


void MainWindow::testDelay()
{
//    qDebug()<<Utility::getCardAttribute("GVG_030", "set").toString();
//    testSynergies();
}



//TODDO
//Dech mechanics redone with colors
//Prox version rebajar cartas
//Primer deck mechanic dorado
//Hacer el delay para buscar tag change 1sec no mas, sino se puede pasar a otro turno
//Hallazeal the ascended solo puede poner addons en heroes
//Incluir dano y healing en el addon en replay
//Anciano prohibido es coste 4 para mana avg, repasar todas las cartas de coste 0/1/2/3
//Repasar cartas wild como nueva exp (king's elek) carta robada como BY:
//Actualizar synergiesVersion

//Twitch integration
//HSReplay support

//Group similar outsider cards (purple text cards on your deck) so they don't take so much space in your deck window.
//Specify where are enemy secrets played coming from, like BY: Cabalist's Tome. In case they are generated by other cards.
//A Graveyard Tab to be able to know exactly what died on each side and how often. This could be useful for reanimation mechanics (Onyx Bishop) and other ones like Tomb Lurker.
//Add checkboxes to config tab to select which tabs will be shown in the main window and which ones will be hidden.
//Divide the overlay checkbox in config tab used during the draft in two separate checkboxes: One for deck mechanics overlay (over the cards) and other for scores and synergies overlay (below the cards).
//Mostrar draft mechanics overlay al volver a arena para saber como es nuestro mazo
//Mostrar BY: cards en mano enemiga conocidas como la carta especifica. P.Ej: BY: Gilded gargoyle es siempre moneda
//Show cards drawn and discarded when your hand is full on replay tab.
//Add a countdown option to let you know how much time you have left until your turn ends.


//REPLAY BUGS
//Mandar a pending tag changes durante 5 segundos, carta robada por mana blind no se pone a 0 mana. Aceptable

//Cambios al ataque de un arma en el turno del otro jugador no crean addons ya que el ataque del heroe estara oculto. Aceptable

//Renuncia a la oscuridad muestra como jugadas las cartas sustituidas. Van a zone vacia como los hechizos asi que no se puede distinguir. Aceptable
//Mismo problema con Experimentador gnomo al convertir un esbirro en pollo.

//Al robar un minion de un zone con auras, aparecera un addon extra en el minion robado, al cambiar su ATK/HEALTH.
//El addon es de la fuente que lo robo. Aceptable

//Viejo ojosombrio incrementa su ATK al aparecer otros murlocs, Si los murlocs nuevos son TRIGGERED,
//el addon sobre ojosombrio sera incorrecto. Aceptable

//Efectos que cambien el max vida pondran addons de vida incorrectos, igualdad. Aceptable
//Dificil de arreglar, se cambia el damage antes del health.
//Al morir stormwind champion, apareceran addons de vida de lo que lo mato en el resto de minions heridos de la zona.

//Al lanzar la maldicion del brujo la carta se roba y se juega como hechizo en el enemigo
//ZoneChangeList.ProcessChanges() - id=87 local=False [id=152 cardId= type=INVALID zone=HAND zonePos=6 player=2] zone from  -> OPPOSING HAND
//ZoneChangeList.ProcessChanges() - id=87 local=False [name=¡Maldito! id=152 zone=HAND zonePos=6 cardId=LOE_007t player=2] zone from OPPOSING HAND ->

//Se produce entre el PLAY y el POWER
//PowerTaskList.DebugPrintPower() -     TAG_CHANGE Entity=[name=Jaina Valiente id=64 zone=PLAY zonePos=0 cardId=HERO_08 player=1] tag=HEAVILY_ARMORED value=1
//GameWatcher(41192): Trigger(TRIGGER): Eremita Cho

//El minion copiado por mirror entity no tiene las modificaciones a ataque o vida ya que estas no estan en append el suficiente tiempo.


//SPECTATOR GAMES
//Si empiezan desde el principio todo correcto. A veces las cartas iniciales no apareceran en la draw list, se debe a que a veces vienen del vacio en lugar del DECK.
//Por lo tanto no seran restadas del mazo, y si son devueltas en el mulligan apareceran como OUTSIDERS
//Si empiezan a medias faltara: name1, name2, playerTag, firstPlayer

//BUGS CONOCIDOS
//Tab Config ScrollArea slider transparent CSS
//Solo mode da problemas con las cartas iniciales en el enemigo, son de turn 1 y no hay moneda.
//Baron seboso (Blubber baron) no tiene atk/health correctos en el replay ya que modifica sus atributos en mano y no usa TAG_CHANGE ARMS_DEALING
//Acechador solitario (Forlorn Stalker), los minions que buffan tienen atk/health correctos por la misma razon.
//Cartas a mano sin code, arreglado obteniendo el code del nombre:
//id=10 local=False [name=Clériga de Villanorte id=55 zone=HAND zonePos=1 cardId= player=2] zone from OPPOSING PLAY -> OPPOSING HAND
//Comadreja aprece como OUTSIDER y OUTSIDER BY en tu deck, se debe a que va a tu deck 2 veces una como conocida y otra como desconocida.


//NUEVAS CARTAS
//Update Json cartas --> Automatico
//Update Json LF tierlist --> Automatico
//Update Json HA tierlist --> HATLsed.sh
//--Update secrets
//--Update bombing cards --> PlanHandler::isCardBomb
//--Update ARMS_DEALING cards != 1 --> EnemyHandHandler::getCardBuff
//--Update cartas que dan mana inmediato --> CardGraphicsItem::getManaSpent
//--Update Utility::isFromStandardSet(QString code) --> LOOTAPALOOZA
//--Update cartas que roban un tipo especifico de carta (Curator) --> EnemyHandHandler::isDrawSpecificCards
//--Update cartas que roban una carta y la clonan (Mimic Pod) --> EnemyHandHandler::isClonerCard
//--Update cartas que en la practica tienen un coste diferente --> SynergyHandler::getCorrectedCardMana
//--Update synergies.json

//STANDARD CYCLE
//Remove secrets rotating out
//Actualizar Utility::isFromStandardSet(code)

//NUEVOS HEROES
//Evitar Asset hero powers (GameWatcher 201)
//Incluir nuevo hero power en isHeroPower(QString code) de GameWatcher
//Nuevo Json hearthArena
//Nuevo start draft menu

//NUEVOS BACKGROUND
//Coger el color de una parte clara de un carta de clase
//Colores->Colorear...(4 opcion por abajo)
//Colores->Tono y saturacion...(2 opcion) Luminosidad +50

//NUEVOS CONTROLES CONFIG TAB
//readSettings --> Cargar valores
//initConfigTab --> Actualizar UI con valores cargados
//updateOtherTabsTransparency --> CSS nuevos controles
//completeConfigTab --> Connect controles - funciones y crear funciones
//writeSettings --> Guardar valores

//NUEVA HEBRA
//QFutureWatcher<QString> futureFUNCION;
//connect(&futureFUNCION, SIGNAL(finished()), this, SLOT(finishFUNCION()));
//void DeckHandler::startFUNCION()
//{
//    if(!futureFUNCION.isRunning()) futureFUNCION.setFuture(QtConcurrent::run(this, &DeckHandler::FUNCION));
//}
//void DeckHandler::finishFUNCION()
//{
//    QString message = futureFUNCION.result();
//    emit pDebug(message);
//}

