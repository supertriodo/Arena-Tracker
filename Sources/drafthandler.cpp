#include "drafthandler.h"
#include "mainwindow.h"
#include "themehandler.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

DraftHandler::DraftHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->deckRating = 0;
    this->numCaptured = 0;
    this->drafting = false;
    this->capturing = false;
    this->leavingArena = false;
    this->transparency = Opaque;
    this->draftScoreWindow = NULL;
    this->mouseInApp = false;
    this->draftMethod = All;

    for(int i=0; i<3; i++)
    {
        screenRects[i] = cv::Rect(0,0,0,0);
        cardDetected[i] = false;
    }

    createDraftItemCounters();
    completeUI();

    connect(&futureFindScreenRects, SIGNAL(finished()), this, SLOT(finishFindScreenRects()));

    //TODO
//    initSynergyCodes();
}

DraftHandler::~DraftHandler()
{
    deleteDraftScoreWindow();
    deleteDraftItemCounters();
}


void DraftHandler::createDraftItemCounters()
{
    horLayoutCardTypes = new QHBoxLayout();
    horLayoutRaces1 = new QHBoxLayout();
    horLayoutRaces2 = new QHBoxLayout();
    horLayoutMechanics1 = new QHBoxLayout();
    horLayoutMechanics2 = new QHBoxLayout();

    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("minionsCounter.png"));
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("spellsCounter.png"));
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap("weaponsCounter.png"));

    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(this, horLayoutRaces1, QPixmap("elementalRace.png"));
    raceCounters[V_BEAST] = new DraftItemCounter(this, horLayoutRaces1, QPixmap("beastRace.png"));
    raceCounters[V_MURLOC] = new DraftItemCounter(this, horLayoutRaces1, QPixmap("murlocRace.png"));
    raceCounters[V_DRAGON] = new DraftItemCounter(this, horLayoutRaces1, QPixmap("dragonRace.png"));

    raceCounters[V_PIRATE] = new DraftItemCounter(this, horLayoutRaces2, QPixmap("pirateRace.png"));
    raceCounters[V_MECHANICAL] = new DraftItemCounter(this, horLayoutRaces2, QPixmap("mechanicalRace.png"));
    raceCounters[V_DEMON] = new DraftItemCounter(this, horLayoutRaces2, QPixmap("demonRace.png"));
    raceCounters[V_TOTEM] = new DraftItemCounter(this, horLayoutRaces2, QPixmap("totemRace.png"));

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_AOE] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("aoeMechanic.png"));
    mechanicCounters[V_TAUNT] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("tauntMechanic.png"));
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap("drawMechanic.png"));

    mechanicCounters[V_PING] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("pingMechanic.png"));
    mechanicCounters[V_DAMAGE_DESTROY] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("damageMechanic.png"));
    mechanicCounters[V_REACH] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap("reachMechanic.png"));

    mechanicCounters[V_ENRAGED] = new DraftItemCounter(this);

    horLayoutCardTypes->addStretch();
    horLayoutRaces1->addStretch();
    horLayoutRaces2->addStretch();
    horLayoutMechanics1->addStretch();
    horLayoutMechanics2->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutCardTypes);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics1);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics2);
    ui->draftVerticalLayout->addLayout(horLayoutRaces1);
    ui->draftVerticalLayout->addLayout(horLayoutRaces2);
}


void DraftHandler::deleteDraftItemCounters()
{
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        delete cardTypeCounters[i];
    }
    delete []cardTypeCounters;

    for(int i=0; i<V_NUM_RACES; i++)
    {
        delete raceCounters[i];
    }
    delete []raceCounters;

    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        delete mechanicCounters[i];
    }
    delete []mechanicCounters;
}


void DraftHandler::completeUI()
{
    labelCard[0] = ui->labelCard1;
    labelCard[1] = ui->labelCard2;
    labelCard[2] = ui->labelCard3;
    labelLFscore[0] = ui->labelLFscore1;
    labelLFscore[1] = ui->labelLFscore2;
    labelLFscore[2] = ui->labelLFscore3;
    labelHAscore[0] = ui->labelHAscore1;
    labelHAscore[1] = ui->labelHAscore2;
    labelHAscore[2] = ui->labelHAscore3;
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


void DraftHandler::initHearthArenaTiers(const QString &heroString)
{
    hearthArenaTiers.clear();

    QFile jsonFile(Utility::extraPath() + "/hearthArena.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonNamesObject = jsonDoc.object().value(heroString).toObject();

    for(const QString &code: lightForgeTiers.keys())
    {
        QString name = Utility::cardEnNameFromCode(code);
        int score = jsonNamesObject.value(name).toInt();
        hearthArenaTiers[code] = score;
        if(score == 0)  emit pDebug("HearthArena missing: " + name);
    }

    emit pDebug("HearthArena Cards: " + QString::number(hearthArenaTiers.count()));
}


void DraftHandler::addCardHist(QString code, bool premium)
{
    QString fileNameCode = premium?(code + "_premium"): code;
    QFileInfo cardFile(Utility::hscardsPath() + "/" + fileNameCode + ".png");
    if(cardFile.exists())
    {
        cardsHist[fileNameCode] = getHist(fileNameCode);
    }
    else
    {
        //La bajamos de HearthHead
        emit checkCardImage(fileNameCode);
        cardsDownloading.append(fileNameCode);
    }
}


QMap<QString, LFtier> DraftHandler::initLightForgeTiers(const QString &heroString)
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

            if(hero == NULL || hero == heroString)
            {
                LFtier lfTier;
                lfTier.score = (int)jsonScoreObject.value("Score").toDouble();

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
                }
                lightForgeTiers[code] = lfTier;
            }
        }
    }

    emit pDebug("LightForge Cards: " + QString::number(lightForgeTiers.count()));
    return lightForgeTiers;
}


void DraftHandler::initSynergyCodes()
{
    synergyCodes.clear();

    QFile jsonFile(Utility::extraPath() + "/synergies.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonObject = jsonDoc.object();

    for(const QString &code: jsonObject.keys())
    {
        synergyCodes[code];
        QJsonArray synergies = jsonObject.value(code).toArray();
        for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
        {
            synergyCodes[code].append(it->toString());
        }
    }

    emit pDebug("Synergy Cards: " + QString::number(synergyCodes.count()));
}


void DraftHandler::initCodesAndHistMaps(QString &hero)
{
    cardsDownloading.clear();
    cardsHist.clear();

    startFindScreenRects();
    const QString heroString = Utility::heroString2FromLogNumber(hero);
    this->lightForgeTiers = initLightForgeTiers(heroString);
    initHearthArenaTiers(heroString);
    initSynergyCodes();

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
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(labelCard[i]);
    }

    updateBoxTitle();

    if(!alreadyDrafting)
    {
        //SizePreDraft
        MainWindow *mainWindow = ((MainWindow*)parent());
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("size", mainWindow->size());

        //Show Tab
        ui->tabWidget->insertTab(0, ui->tabDraft, QIcon(ThemeHandler::tabArenaFile()), "");
        ui->tabWidget->setTabToolTip(0, "Draft");

        //SizeDraft
        QSize sizeDraft = settings.value("sizeDraft", QSize(350, 400)).toSize();
        mainWindow->resize(sizeDraft);
        mainWindow->resizeTabWidgets();
    }

    ui->tabWidget->setCurrentWidget(ui->tabDraft);
}


void DraftHandler::clearLists(bool keepCounters)
{
    hearthArenaTiers.clear();
    lightForgeTiers.clear();
    synergyCodes.clear();
    cardsHist.clear();

    if(!keepCounters)
    {
        deckRating = 0;

        //Reset counters
        for(int i=0; i<V_NUM_TYPES; i++)
        {
            cardTypeCounters[i]->reset();
        }
        for(int i=0; i<V_NUM_RACES; i++)
        {
            raceCounters[i]->reset();
        }
        for(int i=0; i<V_NUM_MECHANICS; i++)
        {
            mechanicCounters[i]->reset();
        }
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

            //Clear guessed cards
            for(int i=0; i<3; i++)
            {
                cardDetected[i] = false;
                draftCardMaps[i].clear();
                bestMatchesMaps[i].clear();
            }
        }
        if(draftScoreWindow != NULL)    draftScoreWindow->hide();
    }
}


int DraftHandler::draftedCardsCount()
{
    int num = 0;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        num += cardTypeCounters[i]->count();
    }
    return num;
}


void DraftHandler::initCounters(QList<DeckCard> deckCardList)
{
    if(draftedCardsCount() > 0) return;

    for(DeckCard deckCard: deckCardList)
    {
        for(uint i=0; i<deckCard.total; i++)
        {
            updateCounters(deckCard);
        }
    }

    emit pDebug("Counters starts with " + QString::number(draftedCardsCount()) + " cards.");
}


void DraftHandler::beginDraft(QString hero, QList<DeckCard> deckCardList)
{
    bool alreadyDrafting = drafting;
    int heroInt = hero.toInt();
    if(heroInt<1 || heroInt>9)
    {
        emit pDebug("Begin draft of unknown hero: " + hero, Error);
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

    this->arenaHero = hero;
    this->drafting = true;
    this->leavingArena = false;
    this->justPickedCard = "";

    initCodesAndHistMaps(hero);
    initCounters(deckCardList);
    resetTab(alreadyDrafting);
}


void DraftHandler::endDraft()
{
    if(!drafting)    return;

    emit pLog(tr("Draft: ") + ui->groupBoxDraft->title());
    emit pDebug("End draft.");
    emit pLog(tr("Draft: Draft ended."));


    //SizeDraft
    MainWindow *mainWindow = ((MainWindow*)parent());
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("sizeDraft", mainWindow->size());

    //Hide Tab
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabDraft));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabArena));
    mainWindow->calculateMinimumWidth();

    //SizePreDraft
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    mainWindow->resize(size);

    //Upload or complete deck with assets
    //Set updateTime in log
    emit draftEnded();

    //Show Deck Score
    int numCards = draftedCardsCount();
    int deckScore = (numCards==0)?0:(int)(deckRating/numCards);
    emit showMessageProgressBar("Deck Score: " + QString::number(deckScore), 10000);

    clearLists(false);

    this->drafting = false;
    this->justPickedCard = "";

    deleteDraftScoreWindow();

}


void DraftHandler::deleteDraftScoreWindow()
{
    if(draftScoreWindow != NULL)
    {
        draftScoreWindow->close();
        delete draftScoreWindow;
        draftScoreWindow = NULL;
    }
}


void DraftHandler::newCaptureDraftLoop(bool delayed)
{
    if(!capturing && drafting &&
        screenFound() && cardsDownloading.isEmpty() &&
        !lightForgeTiers.empty() && !hearthArenaTiers.empty())
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

    if(leavingArena || !drafting ||
        !screenFound() || !cardsDownloading.isEmpty() ||
        lightForgeTiers.empty() || hearthArenaTiers.empty())
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

        DraftCard bestCards[3];
        getBestCards(bestCards);
        showNewCards(bestCards);
    }
    else
    {
        QTimer::singleShot(CAPTUREDRAFT_LOOP_TIME, this, SLOT(captureDraft()));
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
    double minMatch = numCaptured;
    for(DraftCard card: draftCardMaps.values())
    {
        double match = card.getSumQualityMatches();
        if(match < minMatch)    minMatch = card.getSumQualityMatches();
    }
    return minMatch/numCaptured;
}


void DraftHandler::buildBestMatchesMaps()
{
    for(int i=0; i<3; i++)
    {
        for(QString code: draftCardMaps[i].keys())
        {
            double match = draftCardMaps[i][code].getSumQualityMatches();
            bestMatchesMaps[i].insertMulti(match, code);
        }
    }
}


void DraftHandler::getBestCards(DraftCard bestCards[3])
{
    double bestMatch = numCaptured;
    int bestIndex = 0;
    QString bestCode;

    for(int i=0; i<3; i++)
    {
        double match = bestMatchesMaps[i].firstKey();
        QString code = bestMatchesMaps[i].first();
        if(match < bestMatch)
        {
            bestMatch = match;
            bestIndex = i;
            bestCode = code;
        }
    }

    CardRarity bestRarity = draftCardMaps[bestIndex][bestCode].getRarity();
    emit pDebug("");

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
                    QString::number(((int)(match/std::max(1,numCaptured)*1000))/1000.0);
            if(draftCardMaps[i][code].getRarity() == bestRarity)
            {
                bestCards[i] = draftCardMaps[i][code];
                emit pDebug("Choose: " + cardInfo);
                break;
            }
            else
            {
                emit pDebug("Skip: " + cardInfo + " (Different rarity)");
            }
        }
    }

    emit pDebug("(" + QString::number(draftedCardsCount()) + ") " +
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

    if(code=="0" || code=="1" || code=="2")
    {
        code = draftCards[code.toInt()].getCode();
    }

    DraftCard draftCard;
    for(int i=0; i<3; i++)
    {
        if(draftCards[i].getCode() == code)
        {
            draftCard = draftCards[i];
            updateBoxTitle(shownTierScores[i]);
            break;
        }
    }
    updateCounters(draftCard);

    //Clear cards and score
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(labelCard[i]);
        cardDetected[i] = false;
        draftCardMaps[i].clear();
        bestMatchesMaps[i].clear();
    }

    this->numCaptured = 0;
    if(draftScoreWindow != NULL)    draftScoreWindow->hideScores();

    emit pDebug("Pick card: " + code);
    emit pLog(tr("Draft:") + " (" + QString::number(draftedCardsCount()) + ")" + draftCard.getName());
    emit newDeckCard(code);
    this->justPickedCard = code;

    newCaptureDraftLoop(true);
}


void DraftHandler::updateCounters(DeckCard &deckCard)
{
    updateRaceCounters(deckCard);
    updateCardTypeCounters(deckCard);
    updateMechanicCounters(deckCard);
}


void DraftHandler::updateRaceCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    CardRace cardRace = deckCard.getRace();

    if(cardRace == MURLOC || isMurlocGen(code))         raceCounters[V_MURLOC]->increase(code);
    if(cardRace == DEMON || isDemonGen(code))           raceCounters[V_DEMON]->increase(code);
    if(cardRace == MECHANICAL || isMechGen(code))       raceCounters[V_MECHANICAL]->increase(code);
    if(cardRace == ELEMENTAL || isElementalGen(code))   raceCounters[V_ELEMENTAL]->increase(code);
    if(cardRace == BEAST || isBeastGen(code))           raceCounters[V_BEAST]->increase(code);
    if(cardRace == TOTEM || isTotemGen(code))           raceCounters[V_TOTEM]->increase(code);
    if(cardRace == PIRATE || isPirateGen(code))         raceCounters[V_PIRATE]->increase(code);
    if(cardRace == DRAGON || isDragonGen(code))         raceCounters[V_DRAGON]->increase(code);

    if(isMurlocSyn(code))   raceCounters[V_MURLOC]->increaseSyn(code);
    if(isDemonSyn(code))    raceCounters[V_DEMON]->increaseSyn(code);
    if(isMechSyn(code))     raceCounters[V_MECHANICAL]->increaseSyn(code);
    if(isElementalSyn(code))raceCounters[V_ELEMENTAL]->increaseSyn(code);
    if(isBeastSyn(code))    raceCounters[V_BEAST]->increaseSyn(code);
    if(isTotemSyn(code))    raceCounters[V_TOTEM]->increaseSyn(code);
    if(isPirateSyn(code))   raceCounters[V_PIRATE]->increaseSyn(code);
    if(isDragonSyn(code))   raceCounters[V_DRAGON]->increaseSyn(code);
}


void DraftHandler::updateCardTypeCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)       cardTypeCounters[V_SPELL]->increase(code);
    else if(isSpellGen(code))   cardTypeCounters[V_SPELL]->increase(code,false);

    if(cardType == WEAPON)      cardTypeCounters[V_WEAPON]->increase(code);
    else if(isWeaponGen(code))  cardTypeCounters[V_WEAPON]->increase(code,false);

    if(cardType == MINION)      cardTypeCounters[V_MINION]->increase(code);

    if(isSpellSyn(code))    cardTypeCounters[V_SPELL]->increaseSyn(code);
    if(isWeaponSyn(code))   cardTypeCounters[V_WEAPON]->increaseSyn(code);
}


void DraftHandler::updateMechanicCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();

    if(isDiscoverDrawGen(code))                                             mechanicCounters[V_DISCOVER_DRAW]->increase(code);
    if(isTaunt(code, mechanics))                                            mechanicCounters[V_TAUNT]->increase(code);
    else if(isTauntGen(code, referencedTags))                               mechanicCounters[V_TAUNT]->increase();
    if(isAoeGen(code))                                                      mechanicCounters[V_AOE]->increase(code);
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mechanicCounters[V_PING]->increase(code);
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)
            || isDestroyGen(code))                                          mechanicCounters[V_DAMAGE_DESTROY]->increase(code);
    if(isReachGen(code, mechanics, referencedTags, text, cardType))         mechanicCounters[V_REACH]->increase(code);
    if(isEnrageGen(code, mechanics, referencedTags))                        mechanicCounters[V_ENRAGED]->increase(code);

    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isEnrageSyn(code,text))                                              mechanicCounters[V_ENRAGED]->increaseSyn(code);
}


bool DraftHandler::isSpellSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  (text.contains("spell") && !text.contains("spell damage") && !text.contains("can't be targeted by spells"));
    }
}


bool DraftHandler::isWeaponSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("weapon") && !text.contains("opponent's weapon");
    }
}
bool DraftHandler::isMurlocSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("murlocSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("murloc");
    }
}
bool DraftHandler::isDemonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("demon");
    }
}
bool DraftHandler::isMechSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("mech");
    }
}
bool DraftHandler::isElementalSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("elemental");
    }
}
bool DraftHandler::isBeastSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("beast");
    }
}
bool DraftHandler::isTotemSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("totem");
    }
}
bool DraftHandler::isPirateSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("pirate");
    }
}
bool DraftHandler::isDragonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonSyn");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("dragon");
    }
}
bool DraftHandler::isEnrageSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageSyn");
    }
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("enemy") && !text.contains("random") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool DraftHandler::isPingSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingSyn");
    }
    return false;
}
bool DraftHandler::isAoeSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeSyn");
    }
    return false;
}
bool DraftHandler::isTauntSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntSyn");
    }
    return false;
}


bool DraftHandler::isSpellGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("spellGen");
    return false;
}
bool DraftHandler::isWeaponGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("weaponGen");
    return false;
}
bool DraftHandler::isMurlocGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("murlocGen");
    return false;
}
bool DraftHandler::isDemonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("demonGen");
    return false;
}
bool DraftHandler::isMechGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("mechGen");
    return false;
}
bool DraftHandler::isElementalGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("elementalGen");
    return false;
}
bool DraftHandler::isBeastGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("beastGen");
    return false;
}
bool DraftHandler::isTotemGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("totemGen");
    return false;
}
bool DraftHandler::isPirateGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("pirateGen");
    return false;
}
bool DraftHandler::isDragonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("dragonGen");
    return false;
}
bool DraftHandler::isDiscoverDrawGen(const QString &code)
{
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discoverGen") ||
                synergyCodes[code].contains("drawGen") ||
                synergyCodes[code].contains("toYourHandGen");
    }
    else if(mechanics.contains(QJsonValue("DISCOVER")) || referencedTags.contains(QJsonValue("DISCOVER")))
    {
        return true;
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("draw") ||
                (text.contains("to your hand") && !text.contains("return"));
    }
}
bool DraftHandler::isTaunt(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("taunt");
    }
    else if(mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool DraftHandler::isTauntGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGen");
    }
    else if(referencedTags.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//bool DraftHandler::isRestoreGen(const QString &code)
//{
//    if(synergyCodes.contains(code))
//    {
//        return synergyCodes[code].contains("restoreGen");
//    }
//    else
//    {
//        QString text = Utility::cardEnTextFromCode(code).toLower();
//        return  text.contains("restore");
//    }
//}
bool DraftHandler::isAoeGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeGen");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  (text.contains("all") || text.contains("adjacent")) &&
                    (text.contains("damage") ||
                        (text.contains("destroy") && text.contains("minions"))
                     );
    }
}
bool DraftHandler::isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("random") && !text.contains("hero"))
    {
        return true;
    }
    else if(attack != 1)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool DraftHandler::isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                              const QString &text, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("reachGen") || synergyCodes[code].contains("weaponGen") || synergyCodes[code].contains("stealthGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
            !text.contains("minion") && !text.contains("random") && !text.contains("to your hero"))
    {
        return true;
    }
    //Charge and stealth minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")) ||
            mechanics.contains(QJsonValue("STEALTH")) || referencedTags.contains(QJsonValue("STEALTH")))
        {
            return !text.contains("gain <b>charge</b>") && !text.contains("can't attack heroes");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool DraftHandler::isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                      const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageMinionsGen");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
            !text.contains("1 damage") && !text.contains("all") && !text.contains("random") && !text.contains("hero"))
    {
        return true;
    }
    else if(attack == 1)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool DraftHandler::isDestroyGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroyGen");
    }
    else
    {
        QString text = Utility::cardEnTextFromCode(code).toLower();
        return  text.contains("destroy") && text.contains("minion") &&
                !text.contains("all");
    }
}
bool DraftHandler::isEnrageGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageGen");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")) || referencedTags.contains(QJsonValue("ENRAGED")))
    {
        return true;
    }
    else
    {
        return false;
    }
}


int DraftHandler::normalizeLFscore(int score)
{
    return score - 40;
}


void DraftHandler::showNewCards(DraftCard bestCards[3])
{
    //Load cards
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i] = bestCards[i];
        draftCards[i].draw(labelCard[i]);
    }


    //LightForge
    int rating1 = normalizeLFscore(lightForgeTiers[bestCards[0].getCode()].score);
    int rating2 = normalizeLFscore(lightForgeTiers[bestCards[1].getCode()].score);
    int rating3 = normalizeLFscore(lightForgeTiers[bestCards[2].getCode()].score);
    int maxCard1 = lightForgeTiers[bestCards[0].getCode()].maxCard;
    int maxCard2 = lightForgeTiers[bestCards[1].getCode()].maxCard;
    int maxCard3 = lightForgeTiers[bestCards[2].getCode()].maxCard;
    showNewRatings(rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   maxCard1, maxCard2, maxCard3,
                   LightForge);


    //HearthArena
    rating1 = hearthArenaTiers[bestCards[0].getCode()];
    rating2 = hearthArenaTiers[bestCards[1].getCode()];
    rating3 = hearthArenaTiers[bestCards[2].getCode()];
    showNewRatings(rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   -1, -1, -1,
                   HearthArena);

    //Synergies
    QMap<QString,int> synergies[3];
    for(int i=0; i<3; i++)  getSynergies(bestCards[i], synergies[i]);
    draftScoreWindow->setSynergies(synergies);
}


void DraftHandler::getSynergies(DraftCard &draftCard, QMap<QString,int> &synergies)
{
    getCardTypeSynergies(draftCard, synergies);
    getRaceSynergies(draftCard, synergies);
    getMechanicSynergies(draftCard, synergies);
}


void DraftHandler::getCardTypeSynergies(DraftCard &draftCard, QMap<QString,int> &synergies)
{
    QString code = draftCard.getCode();
    CardType cardType = draftCard.getType();

    if(cardType == SPELL || isSpellGen(code))   cardTypeCounters[V_SPELL]->insertSynCards(synergies);
    if(cardType == WEAPON || isWeaponGen(code)) cardTypeCounters[V_WEAPON]->insertSynCards(synergies);

    if(isSpellSyn(code))    cardTypeCounters[V_SPELL]->insertCards(synergies);
    if(isWeaponSyn(code))   cardTypeCounters[V_WEAPON]->insertCards(synergies);
}


void DraftHandler::getRaceSynergies(DraftCard &draftCard, QMap<QString,int> &synergies)
{
    QString code = draftCard.getCode();
    CardRace cardRace = draftCard.getRace();

    if(cardRace == MURLOC || isMurlocGen(code))         raceCounters[V_MURLOC]->insertSynCards(synergies);
    if(cardRace == DEMON || isDemonGen(code))           raceCounters[V_DEMON]->insertSynCards(synergies);
    if(cardRace == MECHANICAL || isMechGen(code))       raceCounters[V_MECHANICAL]->insertSynCards(synergies);
    if(cardRace == ELEMENTAL || isElementalGen(code))   raceCounters[V_ELEMENTAL]->insertSynCards(synergies);
    if(cardRace == BEAST || isBeastGen(code))           raceCounters[V_BEAST]->insertSynCards(synergies);
    if(cardRace == TOTEM || isTotemGen(code))           raceCounters[V_TOTEM]->insertSynCards(synergies);
    if(cardRace == PIRATE || isPirateGen(code))         raceCounters[V_PIRATE]->insertSynCards(synergies);
    if(cardRace == DRAGON || isDragonGen(code))         raceCounters[V_DRAGON]->insertSynCards(synergies);

    if(isMurlocSyn(code))   raceCounters[V_MURLOC]->insertCards(synergies);
    if(isDemonSyn(code))    raceCounters[V_DEMON]->insertCards(synergies);
    if(isMechSyn(code))     raceCounters[V_MECHANICAL]->insertCards(synergies);
    if(isElementalSyn(code))raceCounters[V_ELEMENTAL]->insertCards(synergies);
    if(isBeastSyn(code))    raceCounters[V_BEAST]->insertCards(synergies);
    if(isTotemSyn(code))    raceCounters[V_TOTEM]->insertCards(synergies);
    if(isPirateSyn(code))   raceCounters[V_PIRATE]->insertCards(synergies);
    if(isDragonSyn(code))   raceCounters[V_DRAGON]->insertCards(synergies);
}


void DraftHandler::getMechanicSynergies(DraftCard &draftCard, QMap<QString,int> &synergies)
{
    QString code = draftCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = draftCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();

    if(isTaunt(code, mechanics))                                            mechanicCounters[V_TAUNT]->insertSynCards(synergies);
    if(isAoeGen(code))                                                      mechanicCounters[V_AOE]->insertSynCards(synergies);
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mechanicCounters[V_PING]->insertSynCards(synergies);
    if(isEnrageGen(code, mechanics, referencedTags))                        mechanicCounters[V_ENRAGED]->insertSynCards(synergies);

    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->insertCards(synergies);
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->insertCards(synergies);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->insertCards(synergies);
    if(isEnrageSyn(code, text))                                             mechanicCounters[V_ENRAGED]->insertCards(synergies);
}


void DraftHandler::updateBoxTitle(double cardRating)
{
    deckRating += cardRating;
    int numCards = draftedCardsCount();
    int actualRating = (numCards==0)?0:(int)(deckRating/numCards);
    ui->groupBoxDraft->setTitle(QString("Deck Score: " + QString::number(actualRating) +
                                        " (" + QString::number(numCards) + "/30)"));
}


void DraftHandler::showNewRatings(double rating1, double rating2, double rating3,
                                  double tierScore1, double tierScore2, double tierScore3,
                                  int maxCard1, int maxCard2, int maxCard3,
                                  DraftMethod draftMethod)
{
    double ratings[3] = {rating1,rating2,rating3};
    double tierScore[3] = {tierScore1, tierScore2, tierScore3};
    int maxCards[3] = {maxCard1, maxCard2, maxCard3};
    double maxRating = std::max(std::max(rating1,rating2),rating3);

    for(int i=0; i<3; i++)
    {
        //TierScore for deck average
        if(draftMethod == this->draftMethod || (this->draftMethod == All && draftMethod == HearthArena))
        {
            shownTierScores[i] = tierScore[i];
        }

        //Update score label
        if(draftMethod == LightForge)
        {
            labelLFscore[i]->setText(QString::number((int)ratings[i]) +
                                               (maxCards[i]!=-1?(" - MAX(" + QString::number(maxCards[i]) + ")"):""));
            if(maxRating == ratings[i])     highlightScore(labelLFscore[i], draftMethod);
        }
        else if(draftMethod == HearthArena)
        {
            labelHAscore[i]->setText(QString::number((int)ratings[i]) +
                                                (maxCards[i]!=-1?(" - MAX(" + QString::number(maxCards[i]) + ")"):""));
            if(maxRating == ratings[i])     highlightScore(labelHAscore[i], draftMethod);
        }
    }

    //Mostrar score
    if(draftScoreWindow != NULL)
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
    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), image.bytesPerLine());

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


QString DraftHandler::degoldCode(QString fileName)
{
    QString code = fileName;
    if(code.endsWith("_premium"))   code.chop(8);
    return code;
}


void DraftHandler::mapBestMatchingCodes(cv::MatND screenCardsHist[3])
{
    bool newCardsFound = false;

    for(int i=0; i<3; i++)
    {
        QMap<double, QString> bestMatchesMap;
        for(QMap<QString, cv::MatND>::const_iterator it=cardsHist.constBegin(); it!=cardsHist.constEnd(); it++)
        {
            double match = compareHist(screenCardsHist[i], it.value(), 3);
            QString code = it.key();
            bestMatchesMap.insertMulti(match, code);

            //Actualizamos DraftCardMaps con los nuevos resultados
            if(draftCardMaps[i].contains(code))
            {
                if(numCaptured == 0)    draftCardMaps[i][code].setQualityMatch(match);
                else                    draftCardMaps[i][code].addQualityMatch(match);
            }
        }

        //Incluimos en DraftCardMaps los mejores 5 matches, si no han sido ya actualizados por estar en el map.
        QList<double> bestMatchesList = bestMatchesMap.keys();
        for(int j=0; j<5 && j<bestMatchesList.count(); j++)
        {
            double match = bestMatchesList.at(j);
            QString code = bestMatchesMap[match];

            if(!draftCardMaps[i].contains(code))
            {
                newCardsFound = true;
                draftCardMaps[i].insert(code, DraftCard(degoldCode(code)));
                draftCardMaps[i][code].addQualityMatch(match + this->numCaptured);
            }
        }
    }


    //No empezamos a contar mientras sigan apareciendo nuevas cartas en las 5 mejores posiciones
    if(!(numCaptured == 0 && newCardsFound))
    {
        this->numCaptured++;
    }


#ifdef QT_DEBUG
    for(int i=0; i<3; i++)
    {
        qDebug()<<endl;
        for(QString code: draftCardMaps[i].keys())
        {
            DraftCard card = draftCardMaps[i][code];
            qDebug()<<"["<<i<<"]"<<code<<card.getName()<<" -- "<<
                      ((int)(card.getSumQualityMatches()*1000))/1000.0<<" -- "<<
                      ((int)(card.getSumQualityMatches()/std::max(1,numCaptured)*1000))/1000.0;
        }
    }
    qDebug()<<"Captured: "<<numCaptured<<endl;
#endif
}


cv::MatND DraftHandler::getHist(const QString &code)
{
    cv::Mat fullCard = cv::imread((Utility::hscardsPath() + "/" + code + ".png").toStdString(), CV_LOAD_IMAGE_COLOR);
    cv::Mat srcBase;
    if(code.endsWith("_premium"))   srcBase = fullCard(cv::Rect(57,71,80,80));
    else                            srcBase = fullCard(cv::Rect(60,71,80,80));
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
    if(!futureFindScreenRects.isRunning() && drafting)  futureFindScreenRects.setFuture(QtConcurrent::run(this, &DraftHandler::findScreenRects));
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
        }

        emit pDebug("Hearthstone arena screen detected on screen " + QString::number(screenIndex));

        createDraftScoreWindow(screenDetection.screenScale);
        newCaptureDraftLoop();
    }
}


ScreenDetection DraftHandler::findScreenRects()
{
    ScreenDetection screenDetection;

    std::vector<Point2f> templatePoints(6);
    templatePoints[0] = cvPoint(205,276); templatePoints[1] = cvPoint(205+118,276+118);
    templatePoints[2] = cvPoint(484,276); templatePoints[3] = cvPoint(484+118,276+118);
    templatePoints[4] = cvPoint(762,276); templatePoints[5] = cvPoint(762+118,276+118);


    QList<QScreen *> screens = QGuiApplication::screens();
    for(int screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;

        std::vector<Point2f> screenPoints = Utility::findTemplateOnScreen("arenaTemplate.png", screen,
                                                                          templatePoints, screenDetection.screenScale);
        if(screenPoints.empty())    continue;

        //Calculamos screenRect
        for(int i=0; i<3; i++)
        {
            screenDetection.screenRects[i]=cv::Rect(screenPoints[i*2], screenPoints[i*2+1]);
        }

        screenDetection.screenIndex = screenIndex;
        return screenDetection;
    }

    screenDetection.screenIndex = -1;
    return screenDetection;
}


void DraftHandler::createDraftScoreWindow(const QPointF &screenScale)
{
    deleteDraftScoreWindow();
    QPoint topLeft(screenRects[0].x * screenScale.x(), screenRects[0].y * screenScale.y());
    QPoint bottomRight(screenRects[2].x * screenScale.x() + screenRects[2].width * screenScale.x(),
            screenRects[2].y * screenScale.y() + screenRects[2].height * screenScale.y());
    QRect draftRect(topLeft, bottomRight);
    QSize sizeCard(screenRects[0].width * screenScale.x(), screenRects[0].height * screenScale.y());
    draftScoreWindow = new DraftScoreWindow((QMainWindow *)this->parent(), draftRect, sizeCard, screenIndex);
    draftScoreWindow->setLearningMode(this->learningMode);
    draftScoreWindow->setDraftMethod(this->draftMethod);

    connect(draftScoreWindow, SIGNAL(cardEntered(QString,QRect,int,int)),
            this, SIGNAL(overlayCardEntered(QString,QRect,int,int)));
    connect(draftScoreWindow, SIGNAL(cardLeave()),
            this, SIGNAL(overlayCardLeave()));

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
    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(24);
    ui->labelLFscore1->setFont(font);
    ui->labelLFscore2->setFont(font);
    ui->labelLFscore3->setFont(font);
    ui->labelHAscore1->setFont(font);
    ui->labelHAscore2->setFont(font);
    ui->labelHAscore3->setFont(font);

    font = QFont(ThemeHandler::defaultFont());
    font.setPixelSize(12);

    for(int i=0; i<3; i++)
    {
        if(labelLFscore[i]->styleSheet().contains("background-image"))      highlightScore(labelLFscore[i], LightForge);
        if(labelHAscore[i]->styleSheet().contains("background-image"))      highlightScore(labelHAscore[i], HearthArena);
        draftCards[i].draw(labelCard[i]);
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

        ui->groupBoxDraft->setStyleSheet("QGroupBox{border: 0px solid transparent; margin-top: 15px; " + ThemeHandler::bgWidgets() +
                                            " color: white;}"
                                         "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}");
    }
    else
    {
        ui->tabDraft->setAttribute(Qt::WA_NoBackground, false);
        ui->tabDraft->repaint();

        ui->groupBoxDraft->setStyleSheet("QGroupBox{border: 0px solid transparent; margin-top: 15px; " + ThemeHandler::bgWidgets() +
                                            " color: " + ThemeHandler::fgColor() + ";}"
                                         "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}");
    }

    //Update score labels
    clearScore(ui->labelLFscore1, LightForge, false);
    clearScore(ui->labelLFscore2, LightForge, false);
    clearScore(ui->labelLFscore3, LightForge, false);
    clearScore(ui->labelHAscore1, HearthArena, false);
    clearScore(ui->labelHAscore2, HearthArena, false);
    clearScore(ui->labelHAscore3, HearthArena, false);

    //Update race counters
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_RACES; i++)
    {
        raceCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


void DraftHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    setTransparency(this->transparency);
}


void DraftHandler::setShowDraftOverlay(bool value)
{
    this->showDraftOverlay = value;
    showOverlay();
}


void DraftHandler::showOverlay()
{
    if(this->draftScoreWindow != NULL)
    {
        if(this->showDraftOverlay)  this->draftScoreWindow->show();
        else                        this->draftScoreWindow->hide();
    }
}


void DraftHandler::setLearningMode(bool value)
{
    this->learningMode = value;
    if(this->draftScoreWindow != NULL)  draftScoreWindow->setLearningMode(value);

    updateScoresVisibility();
}


void DraftHandler::setDraftMethod(DraftMethod value)
{
    this->draftMethod = value;
    if(draftScoreWindow != NULL)    draftScoreWindow->setDraftMethod(value);

    updateScoresVisibility();
}


void DraftHandler::updateScoresVisibility()
{
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
        switch(draftMethod)
        {
            case All:
                for(int i=0; i<3; i++)
                {
                    labelLFscore[i]->show();
                    labelHAscore[i]->show();
                }
                break;
            case LightForge:
                for(int i=0; i<3; i++)
                {
                    labelLFscore[i]->show();
                    labelHAscore[i]->hide();
                }
                break;
            case HearthArena:
                for(int i=0; i<3; i++)
                {
                    labelLFscore[i]->hide();
                    labelHAscore[i]->show();
                }
                break;
            default:
                for(int i=0; i<3; i++)
                {
                    labelLFscore[i]->hide();
                    labelHAscore[i]->hide();
                }
                break;
        }
    }
}


void DraftHandler::redrawAllCards()
{
    if(!drafting)   return;

    for(int i=0; i<3; i++)
    {
        draftCards[i].draw(labelCard[i]);
    }

    if(draftScoreWindow != NULL)    draftScoreWindow->redrawSynergyCards();
}


void DraftHandler::updateTamCard(int value)
{
    ui->labelCard1->setMaximumHeight(value);
    ui->labelCard2->setMaximumHeight(value);
    ui->labelCard3->setMaximumHeight(value);
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


void DraftHandler::minimizeScoreWindow()
{
    if(this->draftScoreWindow != NULL)  draftScoreWindow->showMinimized();
}


void DraftHandler::deMinimizeScoreWindow()
{
    if(this->draftScoreWindow != NULL)  draftScoreWindow->setWindowState(Qt::WindowActive);
}


//Construir json de HearthArena
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
