#include "drafthandler.h"
#include "themehandler.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

DraftHandler::DraftHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->deckRatingHA = this->deckRatingLF = 0;
    this->deckRatingHSR = 0;
    this->numCaptured = 0;
    this->extendedCapture = false;
    this->drafting = false;
    this->heroDrafting = false;
    this->capturing = false;
    this->leavingArena = false;
    this->transparency = Opaque;
    this->draftHeroWindow = nullptr;
    this->draftScoreWindow = nullptr;
    this->draftMechanicsWindow = nullptr;
    this->synergyHandler = nullptr;
    this->mouseInApp = false;
    this->draftMethodHA = false;
    this->draftMethodLF = true;
    this->draftMethodHSR = false;
    this->draftMethodAvgScore = LightForge;
    this->normalizedLF = true;
    this->twitchHandler = nullptr;

    for(int i=0; i<3; i++)
    {
        screenRects[i] = cv::Rect(0,0,0,0);
        cardDetected[i] = false;
    }

    createScoreItems();
    createSynergyHandler();
    buildHeroCodesList();
    completeUI();

    connect(&futureFindScreenRects, SIGNAL(finished()), this, SLOT(finishFindScreenRects()));
}

DraftHandler::~DraftHandler()
{
    deleteDraftHeroWindow();
    deleteDraftScoreWindow();
    deleteDraftMechanicsWindow();
    deleteTwitchHandler();
    if(synergyHandler != nullptr)  delete synergyHandler;
}


void DraftHandler::createScoreItems()
{
    int width = 80;
    lavaButton = new LavaButton(ui->tabDraft, 3, 5.5);
    lavaButton->setFixedHeight(width);
    lavaButton->setFixedWidth(width);
    lavaButton->reset();
    lavaButton->setToolTip("Deck weight");
    lavaButton->hide();

    scoreButtonLF = new ScoreButton(ui->tabDraft, Score_LightForge, false);
    scoreButtonLF->setFixedHeight(width);
    scoreButtonLF->setFixedWidth(width);
    scoreButtonLF->setScore(0, true);
    scoreButtonLF->setToolTip("LightForge deck average");
    scoreButtonLF->hide();

    scoreButtonHA = new ScoreButton(ui->tabDraft, Score_HearthArena, false);
    scoreButtonHA->setFixedHeight(width);
    scoreButtonHA->setFixedWidth(width);
    scoreButtonHA->setScore(0, true);
    scoreButtonHA->setToolTip("HearthArena deck average");
    scoreButtonHA->hide();

    scoreButtonHSR = new ScoreButton(ui->tabDraft, Score_HSReplay, false);
    scoreButtonHSR->setFixedHeight(width);
    scoreButtonHSR->setFixedWidth(width);
    scoreButtonHSR->setScore(0, true);
    scoreButtonHSR->setToolTip("HSReplay winrate deck average");
    scoreButtonHSR->hide();

    QHBoxLayout *scoresLayout = new QHBoxLayout();
    scoresLayout->addWidget(lavaButton);
    scoresLayout->addWidget(scoreButtonLF);
    scoresLayout->addWidget(scoreButtonHA);
    scoresLayout->addWidget(scoreButtonHSR);

    ui->draftVerticalLayout->addLayout(scoresLayout);
    ui->draftVerticalLayout->addSpacing(10);
}


void DraftHandler::createSynergyHandler()
{
    this->synergyHandler = new SynergyHandler(this->parent(),ui);
    connect(synergyHandler, SIGNAL(itemEnter(QList<DeckCard>&,QRect&,int,int)),
            this, SIGNAL(itemEnter(QList<DeckCard>&,QRect&,int,int)));
    connect(synergyHandler, SIGNAL(itemLeave()),
            this, SIGNAL(itemLeave()));
    connect(synergyHandler, SIGNAL(pLog(QString)),
            this, SIGNAL(pLog(QString)));
    connect(synergyHandler, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SIGNAL(pDebug(QString,DebugLevel,QString)));
}


void DraftHandler::completeUI()
{
    setPremium(false);

    comboBoxCard[0] = ui->comboBoxCard1;
    comboBoxCard[1] = ui->comboBoxCard2;
    comboBoxCard[2] = ui->comboBoxCard3;
    labelLFscore[0] = ui->labelLFscore1;
    labelLFscore[1] = ui->labelLFscore2;
    labelLFscore[2] = ui->labelLFscore3;
    labelHAscore[0] = ui->labelHAscore1;
    labelHAscore[1] = ui->labelHAscore2;
    labelHAscore[2] = ui->labelHAscore3;

    for(int i=0; i<3; i++)
    {
        comboBoxCard[i]->setFocusPolicy(Qt::NoFocus);
    }

    connect(ui->refreshDraftButton, SIGNAL(clicked(bool)),
                this, SLOT(refreshCapturedCards()));
}


void DraftHandler::setPremium(bool premium)
{
    if(drafting)    return;
    this->patreonVersion = premium;

    if(premium)
    {
        ui->labelDeckScore->show();
    }
    else
    {
        ui->labelDeckScore->hide();
    }
}


void DraftHandler::connectAllComboBox()
{
    for(int i=0; i<3; i++)
    {
        connect(comboBoxCard[i], SIGNAL(currentIndexChanged(int)),
                this, SLOT(comboBoxChanged()));
        comboBoxCard[i]->setEnabled(true);
    }
    ui->refreshDraftButton->setEnabled(true);
}


void DraftHandler::clearAndDisconnectAllComboBox()
{
    for(int i=0; i<3; i++)
    {
        comboBoxCard[i]->setEnabled(false);
        clearAndDisconnectComboBox(i);
    }
    ui->refreshDraftButton->setEnabled(false);
}


void DraftHandler::clearAndDisconnectComboBox(int index)
{
    disconnect(comboBoxCard[index], nullptr, nullptr, nullptr);
    comboBoxCard[index]->clear();
}


QStringList DraftHandler::getAllArenaCodes()
{
    QStringList codeList;

    QFile jsonFile(Utility::extraPath() + "/lightForge.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    const QJsonArray jsonCardsArray = jsonDoc.object().value("Cards").toArray();
    for(QJsonValue jsonCard: jsonCardsArray)
    {
        QJsonObject jsonCardObject = jsonCard.toObject();
        QString code = jsonCardObject.value("CardId").toString();
        codeList.append(code);
    }

    return codeList;
}


QStringList DraftHandler::getAllHeroCodes()
{
    return heroCodesList;
}


void DraftHandler::initHearthArenaTiers(const QString &heroString, const bool multiClassDraft)
{
    hearthArenaTiers.clear();

    QFile jsonFile(Utility::extraPath() + "/hearthArena.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    if(multiClassDraft)
    {
        QJsonObject heroJsonObject = jsonDoc.object().value(heroString).toObject();
        QJsonObject othersJsonObject[8];
        QString allHeroes[] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
        for(int j=0, i=0; j<9; j++)
        {
            if(heroString != allHeroes[j])   othersJsonObject[i++] = jsonDoc.object().value(allHeroes[j]).toObject();
        }
        for(const QString &code: lightForgeTiers.keys())
        {
            QString name = Utility::cardEnNameFromCode(code);
            if(heroJsonObject.contains(name))   hearthArenaTiers[code] = heroJsonObject.value(name).toInt();
            else
            {
                for(int i=0; i<8; i++)
                {
                    if(othersJsonObject[i].contains(name))
                    {
                        hearthArenaTiers[code] = othersJsonObject[i].value(name).toInt();
                        break;
                    }
                }
            }

            if(hearthArenaTiers[code] == 0)  emit pDebug("HearthArena missing: " + name);
        }
    }
    else
    {
        QJsonObject jsonNamesObject = jsonDoc.object().value(heroString).toObject();
        for(const QString &code: lightForgeTiers.keys())
        {
            QString name = Utility::cardEnNameFromCode(code);
            int score = jsonNamesObject.value(name).toInt();
            hearthArenaTiers[code] = score;
            if(score == 0)  emit pDebug("HearthArena missing: " + name);
        }
        emit pDebug("HearthArena Cards: " + QString::number(jsonNamesObject.count()));
    }
}


void DraftHandler::addCardHist(QString code, bool premium, bool isHero)
{
    //Evitamos golden cards de cartas no colleccionables
    if(premium &&
        (!Utility::hasGoldenImage(code) ||
         !Utility::getCardAttribute(code, "collectible").toBool())) return;

    QString fileNameCode = premium?(code + "_premium"): code;
    QFileInfo cardFile(Utility::hscardsPath() + "/" + fileNameCode + ".png");
    if(cardFile.exists())
    {
        cardsHist[fileNameCode] = getHist(fileNameCode);
    }
    else
    {
        //La bajamos de github/hearthSim
        emit checkCardImage(fileNameCode, isHero);
        cardsDownloading.append(fileNameCode);
    }
}


QMap<QString, LFtier> DraftHandler::initLightForgeTiers(const QString &heroString, const bool multiClassDraft)
{
    QMap<QString, LFtier> lightForgeTiers;

    QFile jsonFile(Utility::extraPath() + "/lightForge.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    const QJsonArray jsonCardsArray = jsonDoc.object().value("Cards").toArray();
    for(QJsonValue jsonCard: jsonCardsArray)
    {
        QJsonObject jsonCardObject = jsonCard.toObject();
        QString code = jsonCardObject.value("CardId").toString();

        const QJsonArray jsonScoresArray = jsonCardObject.value("Scores").toArray();
        for(QJsonValue jsonScore: jsonScoresArray)
        {
            QJsonObject jsonScoreObject = jsonScore.toObject();
            QString hero = jsonScoreObject.value("Hero").toString();

            if(multiClassDraft || hero == nullptr || hero == heroString)
            {
                LFtier lfTier;
                lfTier.score = static_cast<int>(jsonScoreObject.value("Score").toDouble());

                if(jsonScoreObject.value("StopAfterFirst").toBool())
                {
                    lfTier.maxCard = 1;
                }
                else if(jsonScoreObject.value("StopAfterSecond").toBool())
                {
                    lfTier.maxCard = 2;
                }
                else
                {
                    lfTier.maxCard = -1;
                }

                if(!lightForgeTiers.contains(code))
                {
                    addCardHist(code, false);
                    addCardHist(code, true);

                    //En multiclass guardaremos el primer score que aparezca
                    //En cartas neutrales sera el hero == nullptr
                    //En cartas de clase sera la clase especifica
                    if(multiClassDraft) lightForgeTiers[code] = lfTier;
                }
                //En uniclass guardaremos el ultimo score que aparezca que sera el de la clase del draft
                if(!multiClassDraft)    lightForgeTiers[code] = lfTier;
            }
        }
    }

    emit pDebug("LightForge Cards: " + QString::number(lightForgeTiers.count()));
    return lightForgeTiers;
}


void DraftHandler::initCodesAndHistMaps(QString hero)
{
    cardsDownloading.clear();
    cardsHist.clear();

    if(drafting)
    {
        startFindScreenRects();

        const QString heroString = Utility::heroString2FromLogNumber(hero);
        this->lightForgeTiers = initLightForgeTiers(heroString, MULTICLASS_ARENA);
        initHearthArenaTiers(heroString, MULTICLASS_ARENA);
        synergyHandler->initSynergyCodes();
    }
    else //if(heroDrafting)
    {
        QTimer::singleShot(1000, this, SLOT(startFindScreenRects()));

        for(const QString &code: heroCodesList)     addCardHist(code, false, true);
    }

    //Wait for cards
    if(cardsDownloading.isEmpty())
    {
        newCaptureDraftLoop();
    }
    else
    {
        emit startProgressBar(cardsDownloading.count(), "Downloading cards...");
        emit downloadStarted();
    }
}


void DraftHandler::reHistDownloadedCardImage(const QString &fileNameCode, bool missingOnWeb)
{
    if(!cardsDownloading.contains(fileNameCode)) return; //No forma parte del drafting

    if(!fileNameCode.isEmpty() && !missingOnWeb)  cardsHist[fileNameCode] = getHist(fileNameCode);
    cardsDownloading.removeOne(fileNameCode);
    emit advanceProgressBar(cardsDownloading.count(), fileNameCode.split("_premium").first() + " downloaded");
    if(cardsDownloading.isEmpty())
    {
        emit showMessageProgressBar("All cards downloaded");
        emit downloadEnded();
        newCaptureDraftLoop();
    }
}


void DraftHandler::resetTab(bool alreadyDrafting)
{
    clearAndDisconnectAllComboBox();
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(comboBoxCard[i]);
        comboBoxCard[i]->setCurrentIndex(0);
    }

    if(!alreadyDrafting)
    {
        //SizePreDraft
        QMainWindow *mainWindow = static_cast<QMainWindow*>(parent());
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("size", mainWindow->size());

        //Show Tab
        ui->tabWidget->insertTab(0, ui->tabDraft, QIcon(ThemeHandler::tabArenaFile()), "");
        ui->tabWidget->setTabToolTip(0, "Draft");

        //Reset scores
        scoreButtonLF->setNormalizedLF(normalizedLF);
        synergyHandler->setHidden(!patreonVersion);
        lavaButton->setHidden(!patreonVersion);
        scoreButtonHA->setEnabled(false);
        scoreButtonLF->setEnabled(false);
        scoreButtonHSR->setEnabled(false);
        lavaButton->setEnabled(false);
        lavaButton->reset();
        updateDeckScore();//Basicamente para updateLabelDeckScore
        updateScoresVisibility();
        updateAvgScoresVisibility();

        //SizeDraft
        QSize sizeDraft = settings.value("sizeDraft", QSize(350, 400)).toSize();
        mainWindow->resize(sizeDraft);
        emit calculateMinimumWidth();
    }

    ui->tabWidget->setCurrentWidget(ui->tabDraft);
}


void DraftHandler::clearLists(bool keepCounters)
{
    clearAndDisconnectAllComboBox();
    synergyHandler->clearLists(keepCounters);//keepCounters = beginDraft
    hearthArenaTiers.clear();
    lightForgeTiers.clear();
    cardsHist.clear();

    if(!keepCounters)//endDraft
    {
        deckRatingHA = deckRatingLF = 0;
        deckRatingHSR = 0;
    }

    for(int i=0; i<3; i++)
    {
        screenRects[i]=cv::Rect(0,0,0,0);
        cardDetected[i] = false;
        draftCardMaps[i].clear();
        bestMatchesMaps[i].clear();
    }

    screenIndex = -1;
    numCaptured = 0;
    extendedCapture = false;
}


void DraftHandler::enterArena()
{
    if(drafting)
    {
        showOverlay();
        if(draftCards[0].getCode().isEmpty())
        {
            newCaptureDraftLoop(true);
        }
    }
}


void DraftHandler::leaveArena()
{
    if(drafting)
    {
        if(capturing)
        {
            this->leavingArena = true;
            this->numCaptured = 0;
            this->extendedCapture = false;

            //Clear guessed cards
            for(int i=0; i<3; i++)
            {
                cardDetected[i] = false;
                draftCardMaps[i].clear();
                bestMatchesMaps[i].clear();
            }
        }
        if(draftScoreWindow != nullptr)        draftScoreWindow->hide();
        if(draftMechanicsWindow != nullptr)    draftMechanicsWindow->hide();
    }
    else if(heroDrafting)   endHeroDraft();
    else    deleteDraftMechanicsWindow();
}


void DraftHandler::beginDraft(QString hero, QList<DeckCard> deckCardList)
{
    if(heroDrafting)   endHeroDraft();

    bool alreadyDrafting = drafting;
    int heroInt = hero.toInt();
    if(heroInt<1 || heroInt>9)
    {
        emit pDebug("Begin draft of unknown hero: " + hero, DebugLevel::Error);
        emit pLog(tr("Draft: ERROR: Started draft of unknown hero ") + hero);
        return;
    }
    else
    {
        emit pDebug("Begin draft. Heroe: " + hero);
        emit pLog(tr("Draft: New draft started."));
    }

    //Set updateTime in log / Hide card Window
    emit draftStarted();

    clearLists(true);

    this->arenaHero = Utility::heroFromLogNumber(hero);
    this->drafting = true;
    this->leavingArena = false;
    this->justPickedCard = "";

    initCodesAndHistMaps(hero);
    resetTab(alreadyDrafting);
    initSynergyCounters(deckCardList);
    createTwitchHandler();
}


void DraftHandler::createTwitchHandler()
{
    if(TwitchHandler::isWellConfigured())
    {
        this->twitchHandler = new TwitchHandler(this);
        connect(twitchHandler, SIGNAL(connectionOk(bool)),
                this, SLOT(twitchHandlerConnectionOk(bool)));
        connect(twitchHandler, SIGNAL(voteUpdate(int,int,int)),
                this, SLOT(twitchHandlerVoteUpdate(int,int,int)));
    }
}


void DraftHandler::deleteTwitchHandler()
{
    if(twitchHandler != nullptr)
    {
        twitchHandler->deleteLater();
        twitchHandler = nullptr;
    }
}


void DraftHandler::twitchHandlerConnectionOk(bool ok)
{
    if(ok)
    {
        if(draftScoreWindow != nullptr && TwitchHandler::isActive())   draftScoreWindow->showTwitchScores();
        if(draftHeroWindow != nullptr && TwitchHandler::isActive())    draftHeroWindow->showTwitchScores();
    }
    else
    {
        deleteTwitchHandler();
    }
}


void DraftHandler::twitchHandlerVoteUpdate(int vote1, int vote2, int vote3)
{
    if(draftScoreWindow != nullptr)    draftScoreWindow->setTwitchScores(vote1, vote2, vote3);
    if(draftHeroWindow != nullptr)     draftHeroWindow->setTwitchScores(vote1, vote2, vote3);
}


void DraftHandler::updateTwitchChatVotes()
{
    if(draftScoreWindow != nullptr)
    {
        if(twitchHandler != nullptr && twitchHandler->isConnectionOk() &&
                TwitchHandler::isActive())  draftScoreWindow->showTwitchScores();
        else                                draftScoreWindow->showTwitchScores(false);
    }

    if(draftHeroWindow != nullptr)
    {
        if(twitchHandler != nullptr && twitchHandler->isConnectionOk() &&
                TwitchHandler::isActive())  draftHeroWindow->showTwitchScores();
        else                                draftHeroWindow->showTwitchScores(false);
    }
}


void DraftHandler::initSynergyCounters(QList<DeckCard> &deckCardList)
{
    if(deckCardList.count() == 1 || synergyHandler->draftedCardsCount() > 0 || !patreonVersion)  return;

    if(!lavaButton->isEnabled())
    {
        scoreButtonHA->setEnabled(true);
        scoreButtonLF->setEnabled(true);
        scoreButtonHSR->setEnabled(true);
        lavaButton->setEnabled(true);
    }

    QStringList spellList, minionList, weaponList,
                aoeList, tauntList, survivabilityList, drawList,
                pingList, damageList, destroyList, reachList;
    int tdraw, ttoYourHand, tdiscover;
    tdraw = ttoYourHand = tdiscover = 0;
    for(DeckCard &deckCard: deckCardList)
    {
        if(deckCard.getType() == INVALID_TYPE)  continue;
        QString code = deckCard.getCode();
        for(int i=0; i<deckCard.total; i++)
        {
            int draw, toYourHand, discover;
            synergyHandler->updateCounters(deckCard, spellList, minionList, weaponList,
                           aoeList, tauntList, survivabilityList, drawList,
                           pingList, damageList, destroyList, reachList,
                           draw, toYourHand, discover);
            tdraw += draw;
            ttoYourHand += toYourHand;
            tdiscover += discover;

            deckRatingHA += hearthArenaTiers[code];
            deckRatingLF += lightForgeTiers[code].score;
            deckRatingHSR += cardsWinratesMap[this->arenaHero][code];
        }
    }

    int numCards = synergyHandler->draftedCardsCount();
    lavaButton->setValue(synergyHandler->getManaCounterCount(), numCards, tdraw, ttoYourHand, tdiscover);

    updateDeckScore();
    emit pDebug("Counters starts with " + QString::number(numCards) + " cards.");
}


void DraftHandler::endDraft()
{
    if(!drafting)    return;

    emit pLog(tr("Draft: ") + ui->labelDeckScore->text());
    emit pDebug("End draft.");
    emit pLog(tr("Draft: Draft ended."));


    //SizeDraft
    QMainWindow *mainWindow = static_cast<QMainWindow*>(parent());
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("sizeDraft", mainWindow->size());

    //Hide Tab
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabDraft));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabArena));
    emit calculateMinimumWidth();

    //SizePreDraft
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    mainWindow->resize(size);

    //Upload or complete deck with assets
    //Set updateTime in log
    emit draftEnded();

    //Show Deck Score
    if(patreonVersion)
    {
        int numCards = synergyHandler->draftedCardsCount();
        int deckScoreHA = (numCards==0)?0:static_cast<int>(deckRatingHA/numCards);
        int deckScoreLFNormalized = (numCards==0)?0:static_cast<int>(Utility::normalizeLF((deckRatingLF/numCards), this->normalizedLF));
        float deckScoreHSR = (numCards==0)?0:static_cast<float>(round(static_cast<double>(deckRatingHSR/numCards * 10))/10.0);
        showMessageDeckScore(deckScoreLFNormalized, deckScoreHA, deckScoreHSR);
    }

    clearLists(false);

    this->drafting = false;
    this->justPickedCard = "";

    deleteDraftScoreWindow();
    deleteTwitchHandler();
}


void DraftHandler::endDraftDeleteMechanicsWindow()
{
    endDraft();
    deleteDraftMechanicsWindow();
}


void DraftHandler::deleteDraftHeroWindow()
{
    if(draftHeroWindow != nullptr)
    {
        draftHeroWindow->close();
        delete draftHeroWindow;
        draftHeroWindow = nullptr;
        emit overlayCardLeave();
    }
}


void DraftHandler::deleteDraftScoreWindow()
{
    if(draftScoreWindow != nullptr)
    {
        draftScoreWindow->close();
        delete draftScoreWindow;
        draftScoreWindow = nullptr;
        emit overlayCardLeave();
    }
}


void DraftHandler::deleteDraftMechanicsWindow()
{
    if(draftMechanicsWindow != nullptr)
    {
        draftMechanicsWindow->close();
        delete draftMechanicsWindow;
        draftMechanicsWindow = nullptr;
        emit itemLeave();
    }
}


void DraftHandler::newCaptureDraftLoop(bool delayed)
{
    if(!capturing && screenFound() && cardsDownloading.isEmpty() &&
        ((drafting && !lightForgeTiers.empty() && !hearthArenaTiers.empty()) || heroDrafting))
    {
        capturing = true;

        if(delayed)                 QTimer::singleShot(CAPTUREDRAFT_START_TIME, this, SLOT(captureDraft()));
        else                        captureDraft();
    }
}


//Screen Rects detectados
void DraftHandler::captureDraft()
{
    justPickedCard = "";

    bool missingTierLists = drafting && (lightForgeTiers.empty() || hearthArenaTiers.empty());
    if((!drafting && !heroDrafting) || missingTierLists ||
        leavingArena || !screenFound() || !cardsDownloading.isEmpty())
    {
        leavingArena = false;
        capturing = false;
        return;
    }

    cv::MatND screenCardsHist[3];
    if(!getScreenCardsHist(screenCardsHist))
    {
        capturing = false;
        return;
    }
    mapBestMatchingCodes(screenCardsHist);

    if(areCardsDetected())
    {
        capturing = false;
        buildBestMatchesMaps();

        if(drafting)
        {
            DraftCard bestCards[3];
            getBestCards(bestCards);
            showNewCards(bestCards);
        }
        else// if(heroDrafting)
        {
            showNewHeroes();
        }
    }
    else
    {
        if(numCaptured == 0)    QTimer::singleShot(CAPTUREDRAFT_LOOP_TIME_FADING, this, SLOT(captureDraft()));
        else                    QTimer::singleShot(CAPTUREDRAFT_LOOP_TIME, this, SLOT(captureDraft()));
    }
}


bool DraftHandler::areCardsDetected()
{
    for(int i=0; i<3; i++)
    {
        if(!cardDetected[i] && (numCaptured > 2) &&
            (getMinMatch(draftCardMaps[i]) < (CARD_ACCEPTED_THRESHOLD + numCaptured*CARD_ACCEPTED_THRESHOLD_INCREASE)))
        {
            cardDetected[i] = true;
        }
    }

    return (cardDetected[0] && cardDetected[1] && cardDetected[2]);
}


double DraftHandler::getMinMatch(const QMap<QString, DraftCard> &draftCardMaps)
{
    double minMatch = 1;
    for(DraftCard card: draftCardMaps.values())
    {
        double match = card.getBestQualityMatches();
        if(match < minMatch)    minMatch = match;
    }
    return minMatch;
}


void DraftHandler::buildBestMatchesMaps()
{
    if(drafting)
    {
        for(int i=0; i<3; i++)
        {
            QMap<double, QString> bestMatchesDups;
            for(QString code: draftCardMaps[i].keys())
            {
                double match = draftCardMaps[i][code].getBestQualityMatches();
                bestMatchesDups.insertMulti(match, code);
            }

            comboBoxCard[i]->clear();
            QStringList insertedCodes;
            for(const QString &code: bestMatchesDups.values())
            {
                if(!insertedCodes.contains(degoldCode(code)))
                {
                    double match = draftCardMaps[i][code].getBestQualityMatches();
                    bestMatchesMaps[i].insertMulti(match, code);
                    draftCardMaps[i][code].draw(comboBoxCard[i]);
                    insertedCodes.append(degoldCode(code));
                }
            }
        }
    }
    else// if(heroDrafting)
    {
        for(int i=0; i<3; i++)
        {
            for(QString code: draftCardMaps[i].keys())
            {
                double match = draftCardMaps[i][code].getBestQualityMatches();
                bestMatchesMaps[i].insertMulti(match, code);
            }
        }
    }
}

//Distingue grupos de legendarias de no legendarias
//En los eventos las cartas legendarias introducidas no tienen rareza legendaria, para ellas no analizaremos rarezas
CardRarity DraftHandler::getBestRarity()
{
    CardRarity rarity[3];
    for(int i=0; i<3; i++)
    {
        QString code = bestMatchesMaps[i].first();
        //No restringimos rarezas si hay cartas unicas de arena (no colleccionables) (que no tienen rareza)
        if(!Utility::getCardAttribute(degoldCode(code), "collectible").toBool())    return INVALID_RARITY;
        rarity[i] = draftCardMaps[i][code].getRarity();
    }

//    if(rarity[0] == rarity[1] || rarity[0] == rarity[2])    return rarity[0];
//    else if(rarity[1] == rarity[2])                         return rarity[1];
//    else
    {
        double bestMatch = 1;
        int bestIndex = 0;

        for(int i=0; i<3; i++)
        {
            double match = bestMatchesMaps[i].firstKey();
            if(match < bestMatch)
            {
                bestMatch = match;
                bestIndex = i;
            }
        }

        return rarity[bestIndex];
    }
}


void DraftHandler::getBestCards(DraftCard bestCards[3])
{
    CardRarity bestRarity = getBestRarity();

    for(int i=0; i<3; i++)
    {
        QList<double> bestMatchesList = bestMatchesMaps[i].keys();
        QList<QString> bestCodesList = bestMatchesMaps[i].values();
        for(int j=0; j<bestMatchesList.count(); j++)
        {
            double match = bestMatchesList[j];
            QString code = bestCodesList[j];
            QString name = draftCardMaps[i][code].getName();
            QString cardInfo = code + " " + name + " " +
                    QString::number(static_cast<int>(match*1000)/1000.0);
            if( (bestRarity == INVALID_RARITY) ||
                (bestRarity != LEGENDARY && draftCardMaps[i][code].getRarity() != LEGENDARY) ||
                (bestRarity == LEGENDARY && draftCardMaps[i][code].getRarity() == LEGENDARY))
            {
                bestCards[i] = draftCardMaps[i][code];
                comboBoxCard[i]->setCurrentIndex(j);
                emit pDebug("Choose: " + cardInfo);
                break;
            }
            else
            {
                emit pDebug("Skip: " + cardInfo + " (Wrong rarity)");
            }
        }
        if(bestCards[i].getCode().isEmpty() && !bestCodesList.isEmpty())
        {
            bestCards[i] = draftCardMaps[i][bestCodesList.first()];
        }
    }

    connectAllComboBox();
    emit pDebug("(" + QString::number(synergyHandler->draftedCardsCount()) + ") " +
                bestCards[0].getCode() + "/" + bestCards[1].getCode() +
                "/" + bestCards[2].getCode() + " New codes.");
}


void DraftHandler::pickCard(QString code)
{
    if(!drafting || justPickedCard==code)
    {
        emit pDebug("WARNING: Duplicate pick code detected: " + code);
        return;
    }

    bool delayCapture = true;
    if(code=="0" || code=="1" || code=="2")
    {
        code = draftCards[code.toInt()].getCode();
        delayCapture = false;
    }

    if(patreonVersion)
    {
        if(!lavaButton->isEnabled())
        {
            scoreButtonHA->setEnabled(true);
            scoreButtonLF->setEnabled(true);
            scoreButtonHSR->setEnabled(true);
            lavaButton->setEnabled(true);
        }

        DraftCard draftCard;
        int cardIndex;
        for(cardIndex=0; cardIndex<3; cardIndex++)
        {
            if(draftCards[cardIndex].getCode() == code)
            {
                draftCard = draftCards[cardIndex];
                break;
            }
        }

        if(cardIndex > 2)   draftCard = DraftCard(code);

        QStringList spellList, minionList, weaponList,
                    aoeList, tauntList, survivabilityList, drawList,
                    pingList, damageList, destroyList, reachList;
        int draw, toYourHand, discover;
        synergyHandler->updateCounters(draftCard, spellList, minionList, weaponList,
                                       aoeList, tauntList, survivabilityList, drawList,
                                       pingList, damageList, destroyList, reachList,
                                       draw, toYourHand, discover);

        int numCards = synergyHandler->draftedCardsCount();
        lavaButton->setValue(synergyHandler->getManaCounterCount(), numCards, draw, toYourHand, discover);
        updateDeckScore(hearthArenaTiers[code], lightForgeTiers[code].score, cardsWinratesMap[this->arenaHero][code]);
        if(draftMechanicsWindow != nullptr)
        {
            draftMechanicsWindow->updateCounters(spellList, minionList, weaponList,
                                                 aoeList, tauntList, survivabilityList, drawList,
                                                 pingList, damageList, destroyList, reachList);
            draftMechanicsWindow->updateManaCounter(synergyHandler->getCorrectedCardMana(draftCard), numCards);
            draftMechanicsWindow->updateDeckWeight(numCards, draw, toYourHand, discover);
        }
    }

    //Clear cards and score
    clearAndDisconnectAllComboBox();
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(comboBoxCard[i]);
        comboBoxCard[i]->setCurrentIndex(0);
        cardDetected[i] = false;
        draftCardMaps[i].clear();
        bestMatchesMaps[i].clear();
    }

    this->numCaptured = 0;
    this->extendedCapture = false;
    if(draftScoreWindow != nullptr)    draftScoreWindow->hideScores();

    emit pDebug("Pick card: " + code);
    emit newDeckCard(code);
    this->justPickedCard = code;

    newCaptureDraftLoop(delayCapture);
}


void DraftHandler::refreshCapturedCards()
{
    if(!drafting)
    {
        return;
    }

    //Clear cards and score
    clearAndDisconnectAllComboBox();
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(comboBoxCard[i]);
        comboBoxCard[i]->setCurrentIndex(0);
        cardDetected[i] = false;
        draftCardMaps[i].clear();
        bestMatchesMaps[i].clear();
    }

    this->numCaptured = 0;
    this->extendedCapture = true;
    if(draftScoreWindow != nullptr)    draftScoreWindow->hideScores();

    newCaptureDraftLoop(false);
}


void DraftHandler::showNewCards(DraftCard bestCards[3])
{
    //Load cards
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i] = bestCards[i];
    }

    QString codes[3] = {bestCards[0].getCode(), bestCards[1].getCode(), bestCards[2].getCode()};

    //LightForge
    int rating1 = lightForgeTiers[codes[0]].score;
    int rating2 = lightForgeTiers[codes[1]].score;
    int rating3 = lightForgeTiers[codes[2]].score;
    int maxCard1 = lightForgeTiers[codes[0]].maxCard;
    int maxCard2 = lightForgeTiers[codes[1]].maxCard;
    int maxCard3 = lightForgeTiers[codes[2]].maxCard;
    showNewRatings(rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   maxCard1, maxCard2, maxCard3,
                   LightForge);


    //HearthArena
    rating1 = hearthArenaTiers[codes[0]];
    rating2 = hearthArenaTiers[codes[1]];
    rating3 = hearthArenaTiers[codes[2]];
    showNewRatings(rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   -1, -1, -1,
                   HearthArena);

    //HSReplay
    float frating1 = cardsWinratesMap[this->arenaHero][codes[0]];
    float frating2 = cardsWinratesMap[this->arenaHero][codes[1]];
    float frating3 = cardsWinratesMap[this->arenaHero][codes[2]];
    showNewRatings(frating1, frating2, frating3,
                   frating1, frating2, frating3,
                   -1, -1, -1,
                   HSReplay);

    //Twitch Handler
    if(this->twitchHandler != nullptr)
    {
        twitchHandler->reset();

        if(TwitchHandler::isActive())
        {
            QString pickTag = TwitchHandler::getPickTag();
            twitchHandler->sendMessage((patreonVersion?QString("["+QString::number(synergyHandler->draftedCardsCount()+1)+"/30] -- "):QString("")) +
                                       "(" + pickTag + "1) " + bestCards[0].getName() +
                                       " / (" + pickTag + "2) " + bestCards[1].getName() +
                                       " / (" + pickTag + "3) " + bestCards[2].getName());
        }
    }


    if(patreonVersion)
    {
        if(draftScoreWindow != nullptr)
        {
            for(int i=0; i<3; i++)
            {
                QMap<QString, int> synergies;
                QMap<QString, int> mechanicIcons;
                synergyHandler->getSynergies(bestCards[i], synergies, mechanicIcons);
                draftScoreWindow->setSynergies(i, synergies, mechanicIcons);
            }
        }
    }
}


void DraftHandler::comboBoxChanged()
{
    DraftCard bestCards[3];

    for(int i=0; i<3; i++)
    {
        int comboBoxIndex = comboBoxCard[i]->currentIndex();
        QList<QString> bestCodes = bestMatchesMaps[i].values();
        int count = bestCodes.count();
        if(comboBoxIndex >= count || comboBoxIndex < 0) return;
        QString code = bestCodes[comboBoxIndex];
        bestCards[i] = draftCardMaps[i][code];
    }

    if(draftScoreWindow != nullptr)    draftScoreWindow->hideScores(true);
    showNewCards(bestCards);
}


void DraftHandler::updateDeckScore(float cardRatingHA, float cardRatingLF, float cardRatingHSR)
{
    if(!patreonVersion) return;

    int numCards = synergyHandler->draftedCardsCount();
    deckRatingHA += cardRatingHA;
    deckRatingLF += cardRatingLF;
    deckRatingHSR += cardRatingHSR;
    int deckScoreHA = (numCards==0)?0:static_cast<int>(deckRatingHA/numCards);
    int deckScoreLF = (numCards==0)?0:static_cast<int>(deckRatingLF/numCards);
    int deckScoreLFNormalized = (numCards==0)?0:static_cast<int>(Utility::normalizeLF((deckRatingLF/numCards), this->normalizedLF));
    float deckScoreHSR = (numCards==0)?0:static_cast<float>(round(static_cast<double>(deckRatingHSR/numCards * 10))/10.0);
    updateLabelDeckScore(deckScoreLFNormalized, deckScoreHA, deckScoreHSR, numCards);
    scoreButtonLF->setScore(deckScoreLF, true);
    scoreButtonHA->setScore(deckScoreHA, true);
    scoreButtonHSR->setScore(deckScoreHSR, true);

    if(draftMechanicsWindow != nullptr)    draftMechanicsWindow->setScores(deckScoreHA, deckScoreLF, deckScoreHSR);
}


void DraftHandler::updateLabelDeckScore(int deckScoreLFNormalized, int deckScoreHA, float deckScoreHSR, int numCards)
{
    QString scoreText = "";
    if(draftMethodLF)   scoreText += " LF: " + QString::number(deckScoreLFNormalized);
    if(draftMethodHA)
    {
        if(!scoreText.isEmpty())    scoreText += " --";
        scoreText += " HA: " + QString::number(deckScoreHA);
    }
    scoreText += " (" + QString::number(numCards) + "/30)";
    ui->labelDeckScore->setText(scoreText);
    //TODO draftMethodHSR
}


void DraftHandler::showMessageDeckScore(int deckScoreLFNormalized, int deckScoreHA, float deckScoreHSR)
{
    QString scoreText = "";
    if(draftMethodLF)   scoreText += "LF: " + QString::number(deckScoreLFNormalized);
    if(draftMethodHA)
    {
        if(!scoreText.isEmpty())    scoreText += " -- ";
        scoreText += "HA: " + QString::number(deckScoreHA);
    }

    if(!scoreText.isEmpty())    emit showMessageProgressBar(scoreText, 10000);
    //TODO draftMethodHSR
}


void DraftHandler::showNewRatings(float rating1, float rating2, float rating3,
                                  float tierScore1, float tierScore2, float tierScore3,
                                  int maxCard1, int maxCard2, int maxCard3,
                                  DraftMethod draftMethod)
{
    float ratings[3] = {rating1,rating2,rating3};
    float tierScore[3] = {tierScore1, tierScore2, tierScore3};
    int maxCards[3] = {maxCard1, maxCard2, maxCard3};
    float maxRating = std::max(std::max(rating1,rating2),rating3);
    Q_UNUSED(tierScore);

    for(int i=0; i<3; i++)
    {
        //TODO draftMethodHSR en labels
        //Update score label
        if(draftMethod == LightForge)
        {
            labelLFscore[i]->setText(QString::number(static_cast<int>(Utility::normalizeLF(ratings[i], this->normalizedLF))) +
                                               (maxCards[i]!=-1?(" - MAX(" + QString::number(maxCards[i]) + ")"):""));
            if(FLOATEQ(maxRating, ratings[i]))  highlightScore(labelLFscore[i], draftMethod);
        }
        else if(draftMethod == HearthArena)
        {
            labelHAscore[i]->setText(QString::number(static_cast<int>(ratings[i])) +
                                                (maxCards[i]!=-1?(" - MAX(" + QString::number(maxCards[i]) + ")"):""));
            if(FLOATEQ(maxRating, ratings[i]))  highlightScore(labelHAscore[i], draftMethod);
        }
    }

    //Mostrar score
    if(draftScoreWindow != nullptr)
    {
        draftScoreWindow->setScores(rating1, rating2, rating3, draftMethod);
    }
}


bool DraftHandler::getScreenCardsHist(cv::MatND screenCardsHist[3])
{
    QList<QScreen *> screens = QGuiApplication::screens();
    if(screenIndex >= screens.count() || screenIndex < 0)  return false;
    QScreen *screen = screens[screenIndex];
    if (!screen) return false;

    QRect rect = screen->geometry();
    QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();
    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), static_cast<ulong>(image.bytesPerLine()));

    cv::Mat screenCapture = mat.clone();

    cv::Mat bigCards[3];
    bigCards[0] = screenCapture(screenRects[0]);
    bigCards[1] = screenCapture(screenRects[1]);
    bigCards[2] = screenCapture(screenRects[2]);


//#ifdef QT_DEBUG
//    cv::imshow("Card1", bigCards[0]);
//    cv::imshow("Card2", bigCards[1]);
//    cv::imshow("Card3", bigCards[2]);
//#endif

    for(int i=0; i<3; i++)  screenCardsHist[i] = getHist(bigCards[i]);
    return true;
}


bool DraftHandler::isGoldCode(QString fileName)
{
    return fileName.endsWith("_premium");
}


QString DraftHandler::degoldCode(QString fileName)
{
    QString code = fileName;
    if(code.endsWith("_premium"))   code.chop(8);
    return code;
}


void DraftHandler::mapBestMatchingCodes(cv::MatND screenCardsHist[3])
{
    bool newCardsFound = false;
    const int numCandidates = (extendedCapture?CAPTURE_EXTENDED_CANDIDATES:CAPTURE_MIN_CANDIDATES);

    for(int i=0; i<3; i++)
    {
        QMap<double, QString> bestMatchesMap;
        for(QMap<QString, cv::MatND>::const_iterator it=cardsHist.constBegin(); it!=cardsHist.constEnd(); it++)
        {
            double match = compareHist(screenCardsHist[i], it.value(), 3);
            QString code = it.key();
            bestMatchesMap.insertMulti(match, code);

            //Actualizamos DraftCardMaps con los nuevos resultados
            if((numCaptured != 0) && draftCardMaps[i].contains(code))
            {
                draftCardMaps[i][code].setBestQualityMatch(match, false);
            }
        }

        //Incluimos en DraftCardMaps los mejores 7 matches, si no han sido ya actualizados por estar en el map.
        QList<double> bestMatchesList = bestMatchesMap.keys();
        for(int j=0; j<numCandidates && j<bestMatchesList.count(); j++)
        {
            double match = bestMatchesList.at(j);
            QString code = bestMatchesMap[match];

            if(!draftCardMaps[i].contains(code))
            {
                newCardsFound = true;
                draftCardMaps[i].insert(code, DraftCard(degoldCode(code)));
                if(numCaptured != 0)    draftCardMaps[i][code].setBestQualityMatch(match, true);
            }
        }
    }


    //No empezamos a contar mientras sigan apareciendo nuevas cartas en las 7 mejores posiciones
    if(numCaptured != 0 || !newCardsFound)
    {
        if(numCaptured == 0)
        {
            for(int i=0; i<3; i++)  draftCardMaps[i].clear();
        }

        this->numCaptured++;
    }


//#ifdef QT_DEBUG
//    for(int i=0; i<3; i++)
//    {
//        qDebug()<<endl;
//        for(QString code: draftCardMaps[i].keys())
//        {
//            DraftCard card = draftCardMaps[i][code];
//            qDebug()<<"["<<i<<"]"<<code<<card.getName()<<" -- "<<
//                      (static_cast<int>(card.getBestQualityMatches()*1000))/1000.0;
//        }
//    }
//    qDebug()<<"Captured: "<<numCaptured<<endl;
//#endif
}


cv::MatND DraftHandler::getHist(const QString &code)
{
    cv::Mat fullCard = cv::imread((Utility::hscardsPath() + "/" + code + ".png").toStdString(), CV_LOAD_IMAGE_COLOR);
    cv::Mat srcBase;
    if(drafting)
    {
        if(code.endsWith("_premium"))   srcBase = fullCard(cv::Rect(57,71,80,80));
        else                            srcBase = fullCard(cv::Rect(60,71,80,80));
    }
    else //if(heroDrafting)
    {
        srcBase = fullCard(cv::Rect(75,201,160,160));
//#ifdef QT_DEBUG
//        cv::imshow(code.toStdString(), srcBase);
//#endif
    }
    return getHist(srcBase);
}


cv::MatND DraftHandler::getHist(cv::Mat &srcBase)
{
    cv::Mat hsvBase;

    /// Convert to HSV
    cvtColor( srcBase, hsvBase, cv::COLOR_BGR2HSV );

    /// Using 50 bins for hue and 60 for saturation
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };

    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };

    // Use the o-th and 1-st channels
    int channels[] = { 0, 1 };

    /// Calculate the histograms for the HSV images
    cv::MatND histBase;
    calcHist( &hsvBase, 1, channels, cv::Mat(), histBase, 2, histSize, ranges, true, false );
    normalize( histBase, histBase, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

    return histBase;
}


bool DraftHandler::screenFound()
{
    if(screenIndex != -1)   return true;
    else                    return false;
}


void DraftHandler::startFindScreenRects()
{
    if(!futureFindScreenRects.isRunning() &&
            (drafting || heroDrafting))  futureFindScreenRects.setFuture(QtConcurrent::run(this, &DraftHandler::findScreenRects));
}


void DraftHandler::finishFindScreenRects()
{
    ScreenDetection screenDetection = futureFindScreenRects.result();

    if(screenDetection.screenIndex == -1)
    {
        this->screenIndex = -1;
        emit pDebug("Hearthstone arena screen not found. Retrying...");
        QTimer::singleShot(CAPTUREDRAFT_LOOP_FLANN_TIME, this, SLOT(startFindScreenRects()));
    }
    else
    {
        this->screenIndex = screenDetection.screenIndex;
        for(int i=0; i<3; i++)
        {
            this->screenRects[i] = screenDetection.screenRects[i];
//#ifdef QT_DEBUG
//            qDebug()<<"[" + QString::number(i) + "]"<<screenRects[i].x<<screenRects[i].y<<screenRects[i].width<<screenRects[i].height;
//#endif
        }

        emit pDebug("Hearthstone arena screen detected on screen " + QString::number(screenIndex));

        createDraftWindows(screenDetection.screenScale);
        newCaptureDraftLoop();
    }
}


ScreenDetection DraftHandler::findScreenRects()
{
    ScreenDetection screenDetection;

    std::vector<Point2f> templatePoints(6);
    if(drafting)
    {
        templatePoints[0] = cvPoint(205,276); templatePoints[1] = cvPoint(205+118,276+118);
        templatePoints[2] = cvPoint(484,276); templatePoints[3] = cvPoint(484+118,276+118);
        templatePoints[4] = cvPoint(762,276); templatePoints[5] = cvPoint(762+118,276+118);
    }
    else// if(heroDrafting)
    {
        templatePoints[0] = cvPoint(182,332); templatePoints[1] = cvPoint(182+152,332+152);
        templatePoints[2] = cvPoint(453,332); templatePoints[3] = cvPoint(453+152,332+152);
        templatePoints[4] = cvPoint(724,332); templatePoints[5] = cvPoint(724+152,332+152);
    }


    QList<QScreen *> screens = QGuiApplication::screens();
    for(int screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;

        std::vector<Point2f> screenPoints = Utility::findTemplateOnScreen(drafting?"arenaTemplate.png":"heroesTemplate.png", screen,
                                                                          templatePoints, screenDetection.screenScale);
        if(screenPoints.empty())    continue;

        //Calculamos screenRect
        for(int i=0; i<3; i++)
        {
            screenDetection.screenRects[i]=cv::Rect(screenPoints[static_cast<ulong>(i*2)], screenPoints[static_cast<ulong>(i*2+1)]);
        }

        screenDetection.screenIndex = screenIndex;
        return screenDetection;
    }

    screenDetection.screenIndex = -1;
    return screenDetection;
}


void DraftHandler::beginHeroDraft()
{
    emit pDebug("Begin hero draft.");

    clearLists(false);
    this->heroDrafting = true;
    this->leavingArena = false;

    initCodesAndHistMaps();
    createTwitchHandler();
}


void DraftHandler::endHeroDraft()
{
    if(!heroDrafting)    return;

    emit pDebug("End hero draft.");

    clearLists(false);

    this->heroDrafting = false;
    deleteDraftHeroWindow();
    deleteTwitchHandler();
}


void DraftHandler::showNewHeroes()
{
    float scores[3];
    for(int i=0; i<3; i++)
    {
        double match = bestMatchesMaps[i].firstKey();
        QString code = bestMatchesMaps[i].first();
        QString name = draftCardMaps[i][code].getName();
        QString cardInfo = code + " " + name + " " +
                QString::number(static_cast<int>(match*1000)/1000.0);
        emit pDebug("Choose: " + cardInfo);

        QString HSRkey = Utility::getCardAttribute(code, "cardClass").toString();
        scores[i] = heroWinratesMap[HSRkey];
    }
    if(draftHeroWindow != nullptr)     draftHeroWindow->setScores(scores[0], scores[1], scores[2]);

    //Twitch Handler
    if(this->twitchHandler != nullptr)
    {
        twitchHandler->reset();

        if(TwitchHandler::isActive())
        {
            QString pickTag = TwitchHandler::getPickTag();
            twitchHandler->sendMessage("(" + pickTag + "1) " + draftCardMaps[0][bestMatchesMaps[0].first()].getName() +
                                       " / (" + pickTag + "2) " + draftCardMaps[1][bestMatchesMaps[1].first()].getName() +
                                       " / (" + pickTag + "3) " + draftCardMaps[2][bestMatchesMaps[2].first()].getName());
        }
    }
}


void DraftHandler::initDraftMechanicsWindowCounters()
{
    int numCards = synergyHandler->draftedCardsCount();

    if(numCards == 0 || !patreonVersion || draftMechanicsWindow == nullptr)    return;

    QStringList spellList, minionList, weaponList,
                aoeList, tauntList, survivabilityList, drawList,
                pingList, damageList, destroyList, reachList;
    int draw, toYourHand, discover;
    int manaCounter = synergyHandler->getCounters(spellList, minionList, weaponList,
                                                  aoeList, tauntList, survivabilityList, drawList,
                                                  pingList, damageList, destroyList, reachList,
                                                  draw, toYourHand, discover);
    draftMechanicsWindow->updateCounters(spellList, minionList, weaponList,
                                         aoeList, tauntList, survivabilityList, drawList,
                                         pingList, damageList, destroyList, reachList);
    draftMechanicsWindow->updateManaCounter(manaCounter, numCards);
    draftMechanicsWindow->updateDeckWeight(numCards, draw, toYourHand, discover);
    updateDeckScore();
}


void DraftHandler::createDraftWindows(const QPointF &screenScale)
{
    deleteDraftHeroWindow();
    deleteDraftScoreWindow();
    deleteDraftMechanicsWindow();
    QPoint topLeft(static_cast<int>(screenRects[0].x * screenScale.x()), static_cast<int>(screenRects[0].y * screenScale.y()));
    QPoint bottomRight(static_cast<int>(screenRects[2].x * screenScale.x() + screenRects[2].width * screenScale.x()),
            static_cast<int>(screenRects[2].y * screenScale.y() + screenRects[2].height * screenScale.y()));
    QRect draftRect(topLeft, bottomRight);
    QSize sizeCard(static_cast<int>(screenRects[0].width * screenScale.x()), static_cast<int>(screenRects[0].height * screenScale.y()));

    if(drafting)
    {
        draftScoreWindow = new DraftScoreWindow(static_cast<QMainWindow *>(this->parent()), draftRect, sizeCard, screenIndex, this->normalizedLF);
        draftScoreWindow->setLearningMode(this->learningMode);
        draftScoreWindow->setDraftMethod(this->draftMethodHA, this->draftMethodLF, this->draftMethodHSR && patreonVersion);

        connect(draftScoreWindow, SIGNAL(cardEntered(QString,QRect,int,int)),
                this, SIGNAL(overlayCardEntered(QString,QRect,int,int)));
        connect(draftScoreWindow, SIGNAL(cardLeave()),
                this, SIGNAL(overlayCardLeave()));

        if(twitchHandler != nullptr && twitchHandler->isConnectionOk() && TwitchHandler::isActive())   draftScoreWindow->showTwitchScores();

        draftMechanicsWindow = new DraftMechanicsWindow(static_cast<QMainWindow *>(this->parent()), draftRect, sizeCard, screenIndex,
                                                        patreonVersion, this->normalizedLF);
        draftMechanicsWindow->setDraftMethodAvgScore(draftMethodAvgScore);
        initDraftMechanicsWindowCounters();

        connect(draftMechanicsWindow, SIGNAL(itemEnter(QList<DeckCard>&,QPoint&,int,int)),
                this, SIGNAL(itemEnterOverlay(QList<DeckCard>&,QPoint&,int,int)));
        connect(draftMechanicsWindow, SIGNAL(itemLeave()),
                this, SIGNAL(itemLeave()));
        connect(draftMechanicsWindow, SIGNAL(showPremiumDialog()),
                this, SIGNAL(showPremiumDialog()));
    }
    else// if(heroDrafting)
    {
        draftHeroWindow = new DraftHeroWindow(static_cast<QMainWindow *>(this->parent()), draftRect, sizeCard, screenIndex);
        if(twitchHandler != nullptr && twitchHandler->isConnectionOk() && TwitchHandler::isActive())   draftHeroWindow->showTwitchScores();
    }

    showOverlay();
}


void DraftHandler::clearScore(QLabel *label, DraftMethod draftMethod, bool clearText)
{
    if(clearText)   label->setText("");
    else if(label->styleSheet().contains("background-image"))
    {
        highlightScore(label, draftMethod);
        return;
    }

    if(!mouseInApp && transparency == Transparent)
    {
        label->setStyleSheet("QLabel {background-color: transparent; color: white;}");
    }
    else
    {
        label->setStyleSheet("");
    }
}


void DraftHandler::highlightScore(QLabel *label, DraftMethod draftMethod)
{
    QString backgroundImage = "";
    if(draftMethod == LightForge)           backgroundImage = ":/Images/bgScoreLF.png";
    else if(draftMethod == HearthArena)     backgroundImage = ":/Images/bgScoreHA.png";
    label->setStyleSheet("QLabel {background-color: transparent; color: " +
                         QString((!mouseInApp && transparency == Transparent)?"white":ThemeHandler::fgColor()) + ";"
                         "background-image: url(" + backgroundImage + "); background-repeat: no-repeat; background-position: center; }");
}


void DraftHandler::setTheme()
{
    if(draftMechanicsWindow != nullptr)    draftMechanicsWindow->setTheme();
    synergyHandler->setTheme();

    ui->refreshDraftButton->setIcon(QIcon(ThemeHandler::buttonDraftRefreshFile()));

    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(24);
    ui->labelLFscore1->setFont(font);
    ui->labelLFscore2->setFont(font);
    ui->labelLFscore3->setFont(font);
    ui->labelHAscore1->setFont(font);
    ui->labelHAscore2->setFont(font);
    ui->labelHAscore3->setFont(font);

    for(int i=0; i<3; i++)
    {
        if(labelLFscore[i]->styleSheet().contains("background-image"))      highlightScore(labelLFscore[i], LightForge);
        if(labelHAscore[i]->styleSheet().contains("background-image"))      highlightScore(labelHAscore[i], HearthArena);
    }

    //Change Arena draft icon
    int index = ui->tabWidget->indexOf(ui->tabDraft);
    if(index >= 0)  ui->tabWidget->setTabIcon(index, QIcon(ThemeHandler::tabArenaFile()));
}


void DraftHandler::setTransparency(Transparency value)
{
    this->transparency = value;

    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabDraft->setAttribute(Qt::WA_NoBackground);
        ui->tabDraft->repaint();

        ui->labelDeckScore->setStyleSheet("QLabel {background-color: transparent; color: white;}");
    }
    else
    {
        ui->tabDraft->setAttribute(Qt::WA_NoBackground, false);
        ui->tabDraft->repaint();

        ui->labelDeckScore->setStyleSheet("");
    }

    //Update score labels
    clearScore(ui->labelLFscore1, LightForge, false);
    clearScore(ui->labelLFscore2, LightForge, false);
    clearScore(ui->labelLFscore3, LightForge, false);
    clearScore(ui->labelHAscore1, HearthArena, false);
    clearScore(ui->labelHAscore2, HearthArena, false);
    clearScore(ui->labelHAscore3, HearthArena, false);

    //Update race counters
    synergyHandler->setTransparency(transparency, mouseInApp);
}


void DraftHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    setTransparency(this->transparency);
}


void DraftHandler::setShowDraftScoresOverlay(bool value)
{
    this->showDraftScoresOverlay = value;
    showOverlay();
}


void DraftHandler::setShowDraftMechanicsOverlay(bool value)
{
    this->showDraftMechanicsOverlay = value;
    showOverlay();
}


void DraftHandler::showOverlay()
{
    if(this->draftHeroWindow != nullptr)
    {
        this->draftHeroWindow->show();
    }
    if(this->draftScoreWindow != nullptr)
    {
        if(showDraftScoresOverlay)  this->draftScoreWindow->show();
        else                        this->draftScoreWindow->hide();
    }

    if(this->draftMechanicsWindow != nullptr)
    {
        if(showDraftMechanicsOverlay && patreonVersion) this->draftMechanicsWindow->show();
        else                                            this->draftMechanicsWindow->hide();
    }
}


void DraftHandler::setLearningMode(bool value)
{
    this->learningMode = value;
    if(this->draftScoreWindow != nullptr)  draftScoreWindow->setLearningMode(value);

    updateScoresVisibility();
}


void DraftHandler::setDraftMethodAvgScore(DraftMethod draftMethodAvgScore)
{
    this->draftMethodAvgScore = draftMethodAvgScore;

    if(!isDrafting())   return;
    if(draftMechanicsWindow != nullptr)    draftMechanicsWindow->setDraftMethodAvgScore(draftMethodAvgScore);

    updateAvgScoresVisibility();
}


void DraftHandler::setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR)
{
    this->draftMethodHA = draftMethodHA;
    this->draftMethodLF = draftMethodLF;
    this->draftMethodHSR = draftMethodHSR;

    if(!isDrafting())   return;
    if(draftScoreWindow != nullptr)        draftScoreWindow->setDraftMethod(draftMethodHA, draftMethodLF, draftMethodHSR && patreonVersion);

    updateDeckScore();//Basicamente para updateLabelDeckScore
    updateScoresVisibility();
}


void DraftHandler::updateScoresVisibility()
{
    //TODO draftMethodHSR
    if(learningMode)
    {
        for(int i=0; i<3; i++)
        {
            labelLFscore[i]->hide();
            labelHAscore[i]->hide();
        }
    }
    else
    {
        for(int i=0; i<3; i++)
        {
            labelHAscore[i]->setVisible(draftMethodHA);
            labelLFscore[i]->setVisible(draftMethodLF);
        }
    }
}


void DraftHandler::updateMinimumHeight()
{
    ui->tabDraft->setMinimumHeight(ui->tabDraft->sizeHint().height());
}


void DraftHandler::updateAvgScoresVisibility()
{
    scoreButtonLF->hide();
    scoreButtonHA->hide();
    scoreButtonHSR->hide();

    if(patreonVersion)
    {
        switch(draftMethodAvgScore)
        {
            case LightForge:
                scoreButtonLF->show();
            break;
            case HearthArena:
                scoreButtonHA->show();
            break;
            case HSReplay:
                scoreButtonHSR->show();
            break;
            default:
            break;
        }
    }
}


void DraftHandler::redrawAllCards()
{
    if(!drafting)   return;

    for(int i=0; i<3; i++)
    {
        int currentIndex = comboBoxCard[i]->currentIndex();
        clearAndDisconnectComboBox(i);
        for(const QString &code: bestMatchesMaps[i].values())
        {
            draftCardMaps[i][code].draw(comboBoxCard[i]);
        }
        comboBoxCard[i]->setCurrentIndex(currentIndex);
    }

    if(draftScoreWindow != nullptr)    draftScoreWindow->redrawSynergyCards();
    connectAllComboBox();
}


void DraftHandler::updateTamCard()
{
    ui->comboBoxCard1->setIconSize(QSize(DeckCard::getCardWidth(), DeckCard::getCardHeight()));
    ui->comboBoxCard2->setIconSize(QSize(DeckCard::getCardWidth(), DeckCard::getCardHeight()));
    ui->comboBoxCard3->setIconSize(QSize(DeckCard::getCardWidth(), DeckCard::getCardHeight()));
}


void DraftHandler::craftGoldenCopy(int cardIndex)
{
    QString code = draftCards[cardIndex].getCode();
    if(!drafting || code.isEmpty())  return;

    //Lanza script
    QProcess p;
    QStringList params;

    params << QDir::toNativeSeparators(Utility::extraPath() + "/goldenCrafter.py");
    params << Utility::removeAccents(draftCards[cardIndex].getName());//Card Name

    emit pDebug("Start script:\n" + params.join(" - "));

#ifdef Q_OS_WIN
    p.start("python", params);
#else
    p.start("python3", params);
#endif
    p.waitForFinished(-1);
}


bool DraftHandler::isDrafting()
{
    return this->drafting;
}


void DraftHandler::setNormalizedLF(bool value)
{
    this->normalizedLF = value;
    if(!isDrafting())   return;

    if(this->draftScoreWindow != nullptr)      draftScoreWindow->setNormalizedLF(value);
    if(this->draftMechanicsWindow != nullptr)  draftMechanicsWindow->setNormalizedLF(value);
    scoreButtonLF->setNormalizedLF(value);

    //Re Show new ratings
    int rating1 = lightForgeTiers[draftCards[0].getCode()].score;
    int rating2 = lightForgeTiers[draftCards[1].getCode()].score;
    int rating3 = lightForgeTiers[draftCards[2].getCode()].score;
    int maxCard1 = lightForgeTiers[draftCards[0].getCode()].maxCard;
    int maxCard2 = lightForgeTiers[draftCards[1].getCode()].maxCard;
    int maxCard3 = lightForgeTiers[draftCards[2].getCode()].maxCard;
    showNewRatings(rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   maxCard1, maxCard2, maxCard3,
                   LightForge);

    //Re UpdateDeckScore
    updateDeckScore();
}


void DraftHandler::minimizeScoreWindow()
{
    if(this->draftHeroWindow != nullptr)                                                       draftHeroWindow->showMinimized();
    if(this->draftScoreWindow != nullptr && showDraftScoresOverlay)                            draftScoreWindow->showMinimized();
    if(this->draftMechanicsWindow != nullptr && showDraftMechanicsOverlay && patreonVersion)   draftMechanicsWindow->showMinimized();
}


void DraftHandler::deMinimizeScoreWindow()
{
    if(this->draftHeroWindow != nullptr)                                                       draftHeroWindow->setWindowState(Qt::WindowActive);
    if(this->draftScoreWindow != nullptr && showDraftScoresOverlay)                            draftScoreWindow->setWindowState(Qt::WindowActive);
    if(this->draftMechanicsWindow != nullptr && showDraftMechanicsOverlay && patreonVersion)   draftMechanicsWindow->setWindowState(Qt::WindowActive);
}


void DraftHandler::debugSynergiesSet(const QString &set, bool onlyCollectible)
{
    synergyHandler->debugSynergiesSet(set, onlyCollectible);
}


void DraftHandler::debugSynergiesCode(const QString &code)
{
    synergyHandler->debugSynergiesCode(code);
}


void DraftHandler::checkSynergyCodes()
{
    synergyHandler->checkSynergyCodes();
}


void DraftHandler::testSynergies()
{
    synergyHandler->testSynergies();
}


void DraftHandler::initSynergyCodes()
{
    synergyHandler->initSynergyCodes();
}


void DraftHandler::setHeroWinratesMap(QMap<QString, float> &heroWinratesMap)
{
    this->heroWinratesMap = heroWinratesMap;
}


void DraftHandler::setCardsWinratesMap(QMap<QString, float> cardsWinratesMap[9])
{
    this->cardsWinratesMap = cardsWinratesMap;
}


void DraftHandler::buildHeroCodesList()
{
    heroCodesList.append("HERO_01");
    heroCodesList.append("HERO_01a");
    heroCodesList.append("HERO_02");
    heroCodesList.append("HERO_02a");
    heroCodesList.append("HERO_02b");
    heroCodesList.append("HERO_03");
    heroCodesList.append("HERO_03a");
    heroCodesList.append("HERO_04");
    heroCodesList.append("HERO_04a");
    heroCodesList.append("HERO_04b");
    heroCodesList.append("HERO_04c");
    heroCodesList.append("HERO_05");
    heroCodesList.append("HERO_05a");
    heroCodesList.append("HERO_06");
    heroCodesList.append("HERO_06a");
    heroCodesList.append("HERO_07");
    heroCodesList.append("HERO_07a");
    heroCodesList.append("HERO_07b");
    heroCodesList.append("HERO_08");
    heroCodesList.append("HERO_08a");
    heroCodesList.append("HERO_08b");
    heroCodesList.append("HERO_09");
    heroCodesList.append("HERO_09a");
    heroCodesList.append("HERO_09b");

    //Si añadimos uno necesitamos borrar todas las cartas y forzar un redownload
    //ya que si alguien ha necesitado el retrato mientras no estaba en el github
    //se habra bajado el retrato basico con el codigo del especifico y
    //no se detectara el nuevo retrato entre los heroes de arena
}


//Construir json de HearthArena (Ya no lo usamos)
//1) Copiar line (var cards = ...)
//EL RESTO LO HACE EL SCRIPT
//2) Eliminar al principio ("\")
//3) Eliminar al final (\"";)
//4) Eliminar (\\\\\\\") Problemas con " en descripciones.
//(Ancien Spirit - Chaman)
//(Explorer's Hat - Hunter)
//(Soul of the forest - Druid)
//5) Eliminar todas las (\)

//Heroes
//01) Warrior
//02) Shaman
//03) Rogue
//04) Paladin
//05) Hunter
//06) Druid
//07) Warlock
//08) Mage
//09) Priest
