#include "drafthandler.h"
#include "mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

DraftHandler::DraftHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->cardsDownloading = 0;
    this->deckRating = 0;
    this->numCaptured = 0;
    this->drafting = false;
    this->capturing = false;
    this->leavingArena = false;
    this->transparency = Opaque;
    this->theme = ThemeWhite;
    this->draftScoreWindow = NULL;
    this->mouseInApp = false;
    this->draftMethod = All;

    for(int i=0; i<3; i++)
    {
        screenRects[i] = cv::Rect(0,0,0,0);
        cardDetected[i] = false;
    }

    createHearthArenaMentor();
    completeUI();

    connect(&futureFindScreenRects, SIGNAL(finished()), this, SLOT(finishFindScreenRects()));
    connect(&futureInitLightForgeTiers, SIGNAL(finished()), this, SLOT(finishInitLightForgeTiers()));
}

DraftHandler::~DraftHandler()
{
    delete hearthArenaMentor;
    deleteDraftScoreWindow();
}


void DraftHandler::completeUI()
{
    ui->textBrowserDraft->setFrameShape(QFrame::NoFrame);

    QFont font("Belwe Bd BT");
    font.setPixelSize(24);
    ui->labelLFscore1->setFont(font);
    ui->labelLFscore2->setFont(font);
    ui->labelLFscore3->setFont(font);
    ui->labelHAscore1->setFont(font);
    ui->labelHAscore2->setFont(font);
    ui->labelHAscore3->setFont(font);

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


void DraftHandler::createHearthArenaMentor()
{
    hearthArenaMentor = new HearthArenaMentor(this);
    connect(hearthArenaMentor, SIGNAL(newTip(QString,double,double,double,double,double,double,QString,QString,QString,int,int,int,DraftMethod)),
            this, SLOT(showNewRatings(QString,double,double,double,double,double,double,QString,QString,QString,int,int,int,DraftMethod)));
    connect(hearthArenaMentor, SIGNAL(pLog(QString)),
            this, SIGNAL(pLog(QString)));
    connect(hearthArenaMentor, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SIGNAL(pDebug(QString,DebugLevel,QString)));
}


void DraftHandler::initHearthArenaCodes(QString &hero)
{
    hearthArenaCodes.clear();

    QFile jsonHAFile(":Json/"+hero+".json");
    jsonHAFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonHAFile.readAll());
    jsonHAFile.close();
    QJsonObject jsonHACodes = jsonDoc.object();
    for(QJsonObject::const_iterator it=jsonHACodes.constBegin(); it!=jsonHACodes.constEnd(); it++)
    {
        QString code = it.value().toObject().value("image").toString();
        bool bannedInArena = it.value().toObject().value("bannedInArena").toBool();

        if(Utility::isFromStandardSet(code) && !bannedInArena)
        {
            hearthArenaCodes[code] = it.key().toInt();
            addCardHist(code, false);
            addCardHist(code, true);
        }
    }

    emit pDebug("HearthArena Cards: " + QString::number(hearthArenaCodes.count()));
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
        cardsDownloading++;
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

                lightForgeTiers[code] = lfTier;
            }
        }
    }

    return lightForgeTiers;
}


void DraftHandler::startInitLightForgeTiers(const QString &heroString)
{
    if(!futureInitLightForgeTiers.isRunning())  futureInitLightForgeTiers.setFuture(QtConcurrent::run(this, &DraftHandler::initLightForgeTiers, heroString));
}


void DraftHandler::finishInitLightForgeTiers()
{
    this->lightForgeTiers = futureInitLightForgeTiers.result();
    emit pDebug("LightForge Cards: " + QString::number(lightForgeTiers.count()));
    newCaptureDraftLoop();
}


void DraftHandler::initCodesAndHistMaps(QString &hero)
{
    cardsDownloading = 0;
    cardsHist.clear();

    startFindScreenRects();
    startInitLightForgeTiers(Utility::heroString2FromLogNumber(hero));
    initHearthArenaCodes(hero);

    //Wait for cards
    if(cardsDownloading==0) newCaptureDraftLoop();
    else
    {
        ui->progressBar->setMaximum(cardsDownloading);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(true);
    }
}


void DraftHandler::reHistDownloadedCardImage(const QString &code)
{
    if(cardsDownloading == 0)   return; //No hay drafting en proceso

    if(!code.isEmpty())  cardsHist[code] = getHist(code);
    cardsDownloading--;

    if(cardsDownloading==0)
    {
        ui->progressBar->setVisible(false);
        newCaptureDraftLoop();
    }
    else
    {
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
}


void DraftHandler::removeTabHero()
{
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabHero));
}


void DraftHandler::resetTab()
{
    for(int i=0; i<3; i++)
    {
        clearScore(labelLFscore[i], LightForge);
        clearScore(labelHAscore[i], HearthArena);
        draftCards[i].setCode("");
        draftCards[i].draw(labelCard[i]);
    }

    ui->textBrowserDraft->setText("");
    ui->groupBoxDraft->setTitle("");

    //SizePreDraft
    MainWindow *mainWindow = ((MainWindow*)parent());
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("size", mainWindow->size());

    //Show Tab
    mainWindow->resize(mainWindow->width() + 30, mainWindow->height());
    removeTabHero();
    ui->tabWidget->insertTab(0, ui->tabDraft, QIcon(":/Images/arena.png"), "");
    ui->tabWidget->setCurrentWidget(ui->tabDraft);
    mainWindow->calculateMinimumWidth();

    //SizeDraft
    QSize sizeDraft = settings.value("sizeDraft", QSize(350, 400)).toSize();
    mainWindow->resize(sizeDraft);
}


void DraftHandler::clearLists(bool keepDraftedCards)
{
    hearthArenaCodes.clear();
    lightForgeTiers.clear();
    cardsHist.clear();

    if(!keepDraftedCards)
    {
        draftedCards.clear();
        deckRating = 0;
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
            newCaptureDraftLoop();
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
        }
        this->draftScoreWindow->hide();
    }
}


void DraftHandler::initDraftedCards(QList<DeckCard> deckCardList)
{
    if(!draftedCards.isEmpty()) return;

    for(DeckCard card: deckCardList)
    {
        if(card.getCode().isEmpty())    continue;
        for(uint i=0; i<card.total; i++)
        {
            draftedCards.append(hearthArenaCodes[card.getCode()]);
        }
    }

    emit pDebug("DraftedCards starts with " + QString::number(draftedCards.count()) + " cards.");
}


void DraftHandler::beginDraft(QString hero, QList<DeckCard> deckCardList)
{
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

    resetTab();
    clearLists(true);

    this->arenaHero = hero;
    this->drafting = true;
    this->capturing = false;
    this->leavingArena = false;
    this->justPickedCard = "";

    initCodesAndHistMaps(hero);
    initDraftedCards(deckCardList);
    updateBoxTitle();
}


void DraftHandler::endDraft()
{
    removeTabHero();

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
        screenFound() && cardsDownloading==0 &&
        !lightForgeTiers.empty() && !hearthArenaCodes.empty())
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
    if(!drafting || !capturing)
    {
        capturing = false;
        return;
    }

    if(leavingArena)
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

    //Borrar
    qDebug()<<"Captured: "<<numCaptured<<endl;

    return (cardDetected[0] && cardDetected[1] && cardDetected[2]) || numCaptured > 30;
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

        //Borrar
        double match = bestMatchesMaps[i].firstKey();
        QString code = bestMatchesMaps[i].first();
        QString name = draftCardMaps[i][code].getName();
        qDebug()<<code<<name<<match/numCaptured;
    }
}


void DraftHandler::getBestCards(DraftCard bestCards[3])
{
    double bestMatch = numCaptured;
    int bestIndex;
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
    qDebug()<<"Best code:"<<bestCode<<bestMatch/numCaptured<<bestIndex;

    for(int i=0; i<3; i++)
    {
        for(QString code: bestMatchesMaps[i].values())
        {
            if(draftCardMaps[i][code].getRarity() == bestRarity)
            {
                bestCards[i] = draftCardMaps[i][code];
                qDebug()<<"Choose" << code << "same rarity.";
                break;
            }
            else
            {
                qDebug()<<"Skip" << code << "different rarity.";
            }
        }
    }
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

    if(hearthArenaCodes.contains(code))
    {
        draftedCards.push_back(hearthArenaCodes[code]);

        for(int i=0; i<3; i++)
        {
            if(draftCards[i].getCode() == code)
            {
                draftCard = draftCards[i];
                updateBoxTitle(shownTierScores[i]);
                break;
            }
        }
    }

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
    ui->textBrowserDraft->setText("");
    draftScoreWindow->hideScores();

    emit pDebug("Card picked: (" + QString::number(draftedCards.count()) + ")" + draftCard.getName());
    emit pLog(tr("Draft:") + " (" + QString::number(draftedCards.count()) + ")" + draftCard.getName());
    emit newDeckCard(code);
    this->justPickedCard = code;

    newCaptureDraftLoop(true);
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

    ui->textBrowserDraft->setText("");


    //LightForge
    int rating1 = lightForgeTiers[bestCards[0].getCode()].score;
    int rating2 = lightForgeTiers[bestCards[1].getCode()].score;
    int rating3 = lightForgeTiers[bestCards[2].getCode()].score;
    int maxCard1 = lightForgeTiers[bestCards[0].getCode()].maxCard;
    int maxCard2 = lightForgeTiers[bestCards[1].getCode()].maxCard;
    int maxCard3 = lightForgeTiers[bestCards[2].getCode()].maxCard;
    showNewRatings("", rating1, rating2, rating3,
                   rating1, rating2, rating3,
                   "", "", "",
                   maxCard1, maxCard2, maxCard3,
                   LightForge);


    //HearthArena
    int intCodes[3];
    for(int i=0; i<3; i++)
    {
        intCodes[i] = hearthArenaCodes[bestCards[i].getCode()];
    }

    hearthArenaMentor->askCardsRating(arenaHero, draftedCards, intCodes);
}


void DraftHandler::updateBoxTitle(double cardRating)
{
    deckRating += cardRating;
    int numCards = draftedCards.count();
    int actualRating = (numCards==0)?0:(int)(deckRating/numCards);
    ui->groupBoxDraft->setTitle(QString("DECK RATING: " + QString::number(actualRating) +
                                        " (" + QString::number(numCards) + "/30)"));
}


void DraftHandler::showNewRatings(QString tip, double rating1, double rating2, double rating3,
                                  double tierScore1, double tierScore2, double tierScore3,
                                  QString synergy1, QString synergy2, QString synergy3,
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
                                            " - (" + QString::number((int)tierScore[i]) + ")");
            if(maxRating == ratings[i])     highlightScore(labelHAscore[i], draftMethod);
        }
    }

    if(draftMethod == HearthArena)  ui->textBrowserDraft->setText(tip);

    //Mostrar score
    draftScoreWindow->setScores(rating1, rating2, rating3, synergy1, synergy2, synergy3, draftMethod);
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

    //Borrar
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

        createDraftScoreWindow();
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

        std::vector<Point2f> screenPoints = Utility::findTemplateOnScreen("arenaTemplate.png", screen, templatePoints);
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


void DraftHandler::createDraftScoreWindow()
{
    deleteDraftScoreWindow();
    QPoint topLeft(screenRects[0].x, screenRects[0].y);
    QPoint bottomRight(screenRects[2].x+screenRects[2].width,
            screenRects[2].y+screenRects[2].height);
    QRect draftRect(topLeft, bottomRight);
    QSize sizeCard(screenRects[0].width, screenRects[0].height);
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
                         QString(theme==ThemeBlack||(!mouseInApp && transparency == Transparent)?"white":"black") + ";"
                         "background-image: url(" + backgroundImage + "); background-repeat: no-repeat; background-position: center; }");
}


void DraftHandler::setTheme(Theme theme)
{
    this->theme = theme;

    for(int i=0; i<3; i++)
    {
        if(labelLFscore[i]->styleSheet().contains("background-image"))      highlightScore(labelLFscore[i], LightForge);
        if(labelHAscore[i]->styleSheet().contains("background-image"))      highlightScore(labelHAscore[i], HearthArena);
    }
}


void DraftHandler::setTransparency(Transparency value)
{
    this->transparency = value;

    if(!mouseInApp && transparency==Transparent)
    {
        ui->tabHero->setAttribute(Qt::WA_NoBackground);
        ui->tabHero->repaint();
        ui->tabDraft->setAttribute(Qt::WA_NoBackground);
        ui->tabDraft->repaint();

        ui->heroLabel->setStyleSheet("QLabel{background-color: transparent; color: white;}");
        ui->textBrowserDraft->setStyleSheet("QTextBrowser{background-color: transparent; color: white;}");
        ui->groupBoxDraft->setStyleSheet("QGroupBox{border: 0px solid transparent; margin-top: 15px; background-color: transparent; color: rgb(50,175,50);}"
                                         "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}");
    }
    else
    {
        ui->tabHero->setAttribute(Qt::WA_NoBackground, false);
        ui->tabHero->repaint();
        ui->tabDraft->setAttribute(Qt::WA_NoBackground, false);
        ui->tabDraft->repaint();

        ui->heroLabel->setStyleSheet("");
        ui->textBrowserDraft->setStyleSheet("");
        ui->groupBoxDraft->setStyleSheet("QGroupBox{border: 0px solid transparent; margin-top: 15px; background-color: transparent; color: rgb(50,175,50);}"
                                         "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}");
    }

    //Update score labels
    clearScore(ui->labelLFscore1, LightForge, false);
    clearScore(ui->labelLFscore2, LightForge, false);
    clearScore(ui->labelLFscore3, LightForge, false);
    clearScore(ui->labelHAscore1, HearthArena, false);
    clearScore(ui->labelHAscore2, HearthArena, false);
    clearScore(ui->labelHAscore3, HearthArena, false);
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

    updateTipVisibility();
    updateScoresVisibility();
}


void DraftHandler::setDraftMethod(DraftMethod value)
{
    this->draftMethod = value;
    if(draftScoreWindow != NULL)    draftScoreWindow->setDraftMethod(value);

    updateTipVisibility();
    updateScoresVisibility();
}


void DraftHandler::updateTipVisibility()
{
    if(!learningMode && (this->draftMethod == HearthArena || this->draftMethod == All))
    {
        ui->textBrowserDraft->show();
        ui->draftVerticalSpacer->changeSize(20, 20, QSizePolicy::Minimum, QSizePolicy::Preferred);
    }
    else
    {
        ui->textBrowserDraft->hide();
        ui->draftVerticalSpacer->changeSize(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    }
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
