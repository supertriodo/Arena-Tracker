#include "drafthandler.h"
#include "mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

DraftHandler::DraftHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->cardsDownloading = 0;
    this->captureLoop = false;
    this->deckRating = 0;
    this->nextCount = 0;
    this->drafting = false;
    this->transparency = Opaque;
    this->draftScoreWindow = NULL;
    this->mouseInApp = false;

    for(int i=0; i<3; i++)
    {
        screenRects[i]=cv::Rect(0,0,0,0);
    }

    createHearthArenaMentor();
    completeUI();
}

DraftHandler::~DraftHandler()
{
    delete hearthArenaMentor;
}


void DraftHandler::completeUI()
{
    ui->textBrowserDraft->setFrameShape(QFrame::NoFrame);
    ui->radioButtonDraft1->setIconSize(10*CARD_SIZE);
    ui->radioButtonDraft2->setIconSize(10*CARD_SIZE);
    ui->radioButtonDraft3->setIconSize(10*CARD_SIZE);
    ui->radioButtonDraft1->setStyleSheet("QRadioButton::indicator {width: 0px;height: 0px;}");
    ui->radioButtonDraft2->setStyleSheet("QRadioButton::indicator {width: 0px;height: 0px;}");
    ui->radioButtonDraft3->setStyleSheet("QRadioButton::indicator {width: 0px;height: 0px;}");

    QFont font("Belwe Bd BT");
    font.setPointSize(18);
    ui->labelDraft1->setFont(font);
    ui->labelDraft2->setFont(font);
    ui->labelDraft3->setFont(font);

    draftCards[0].radioItem = ui->radioButtonDraft1;
    draftCards[1].radioItem = ui->radioButtonDraft2;
    draftCards[2].radioItem = ui->radioButtonDraft3;
    draftCards[0].scoreItem = ui->labelDraft1;
    draftCards[1].scoreItem = ui->labelDraft2;
    draftCards[2].scoreItem = ui->labelDraft3;
}


void DraftHandler::createHearthArenaMentor()
{
    hearthArenaMentor = new HearthArenaMentor(this);
    connect(hearthArenaMentor, SIGNAL(newTip(QString,double,double,double,double,double,double,QString,QString,QString)),
            this, SLOT(showNewRatings(QString,double,double,double,double,double,double,QString,QString,QString)));
    connect(hearthArenaMentor, SIGNAL(pLog(QString)),
            this, SIGNAL(pLog(QString)));
    connect(hearthArenaMentor, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SIGNAL(pDebug(QString,DebugLevel,QString)));
}


void DraftHandler::initCodesAndHistMaps(QString &hero)
{
    cardsDownloading = 0;
    hearthArenaCodes.clear();
    cardsHist.clear();


    QFile jsonFile(":Json/"+hero+".json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonAllCodes = jsonDoc.object();
    for(QJsonObject::const_iterator it=jsonAllCodes.constBegin(); it!=jsonAllCodes.constEnd(); it++)
    {
        QString code = it.value().toObject().value("image").toString();
        hearthArenaCodes[code] = it.key().toInt();

        QFileInfo cardFile(Utility::appPath() + "/HSCards/" + code + ".png");
        if(cardFile.exists())
        {
            cardsHist[code] = getHist(code);
        }
        else
        {
            //La bajamos de HearthHead
            emit checkCardImage(code);
            cardsDownloading++;
        }
    }
    emit pDebug("Num histograms BD:" + QString::number(hearthArenaCodes.count()));

    if(cardsDownloading==0) resumeDraft();
    else
    {
        ui->progressBar->setMaximum(cardsDownloading);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(true);
    }
}


void DraftHandler::reHistDownloadedCardImage(QString &code)
{
    if(cardsDownloading == 0)   return; //No hay drafting en proceso

    cardsHist[code] = getHist(code);
    cardsDownloading--;

    if(cardsDownloading==0)
    {
        ui->progressBar->setVisible(false);
        resumeDraft();
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
        clearScore(draftCards[i].scoreItem);
        draftCards[i].scoreItem->hide();
        draftCards[i].setCode("");
        draftCards[i].draw();
    }

    //Clear synergies
    ui->textDraft1->setText("Detecting Cards...");
    ui->textDraft2->setText("Detecting Cards...");
    ui->textDraft3->setText("Detecting Cards...");
    ui->textBrowserDraft->setText("");
    ui->groupBoxDraft->setTitle("");

    //SizePreDraft
    MainWindow *mainWindow = ((MainWindow*)parent());
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("size", mainWindow->size());

    //Show Tab
    removeTabHero();
    ui->tabWidget->insertTab(0, ui->tabDraft, QIcon(":/Images/arena.png"), "");
    ui->tabWidget->setCurrentWidget(ui->tabDraft);
    mainWindow->calculateMinimumWidth();

    //SizeDraft
    QSize sizeDraft = settings.value("sizeDraft", QSize(350, 400)).toSize();
    mainWindow->resize(sizeDraft);
}


void DraftHandler::clearLists()
{
    hearthArenaCodes.clear();
    cardsHist.clear();
    draftedCards.clear();

    for(int i=0; i<3; i++)
    {
        codesCandidates[i] = "";
        screenRects[i]=cv::Rect(0,0,0,0);
    }

    deckRating = 0;
    nextCount = 0;
}


void DraftHandler::beginDraft(QString hero)
{
    //Si ya hay un draft en proceso (captureDraft == true) se reiniciaran las listas
    //y al hacer resumeDraft no se crearara otro loop ya que hay uno.

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
    clearLists();

    this->arenaHero = hero;
    this->drafting = true;

    //Thread for initHistMaps
    QFuture<void> future = QtConcurrent::run(this, &DraftHandler::initCodesAndHistMaps, hero);
    ((MainWindow*)parent())->repaint();
    future.waitForFinished();
}


void DraftHandler::resumeDraft()
{
    if(!drafting)       return;
    if(captureLoop) //Force draft con un draft activo
    {
        //return;   //Util si tenemos loopInfinitos
    }

    captureLoop = true;

    emit pDebug("Resume draft.");
//    emit pLog(tr("Draft: Draft resumed."));

    QTimer::singleShot(CAPTUREDRAFT_START_TIME, this, SLOT(captureDraft()));
}


void DraftHandler::pauseDraft()
{
    if(!drafting)       return;
    captureLoop = false;

    emit pDebug("Pause draft.");
//    emit pLog(tr("Draft: Draft paused."));
}


void DraftHandler::endDraft()
{
    removeTabHero();

    if(!drafting)    return;

    emit pLog(tr("Draft: ") + ui->groupBoxDraft->title());
    emit pDebug("End draft.");
    emit pLog(tr("Draft: Draft ended."));

    //Guardamos ultima carta
//    for(int i=0; i<3; i++)
//    {
//        if(draftCards[i].radioItem->isChecked() && !draftCards[i].getCode().isEmpty())
//        {
//            pickCard(draftCards[i]);
//        }
//    }

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

    clearLists();

    this->captureLoop = false;
    this->drafting = false;

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


void DraftHandler::captureDraft()
{
    if(!captureLoop)    return;
    if(!drafting)   //Nunca ocurre(+seguridad)
    {
        captureLoop = false;
        return;
    }

    if(screenRectsFound() || findScreenRects())
    {
        cv::MatND screenCardsHist[3];
        getScreenCardsHist(screenCardsHist);

        QString codes[3];
        getBestMatchingCodes(screenCardsHist, codes);

        if(areNewCards(codes))  showNewCards(codes);
        else                    QTimer::singleShot(CAPTUREDRAFT_LOOP_TIME, this, SLOT(captureDraft()));
    }
    else
    {
        QTimer::singleShot(CAPTUREDRAFT_LOOP_FLANN_TIME, this, SLOT(captureDraft()));
    }
}


bool DraftHandler::areNewCards(QString codes[3])
{
    //Util en avance automatico
    /*if((codes[0]==draftCards[0].getCode() && codes[1]==draftCards[1].getCode()) ||
        (codes[0]==draftCards[0].getCode() && codes[2]==draftCards[2].getCode()) ||
        (codes[1]==draftCards[1].getCode() && codes[2]==draftCards[2].getCode()))
    {
        emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                    codes[0] + "/" + codes[1] + "/" + codes[2] +
                    " Same codes.");
        resetCodesCandidates();
        nextCount = 0;
        return false;
    }
    else */if(codes[0]=="" || codes[1]=="" || codes[2]=="")
    {
        emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                    codes[0] + "/" + codes[1] + "/" + codes[2] +
                    " Blank code.");
        //Salida alternativa 10 blancos
        //Util en avance automatico
//        if(draftedCards.count()>=29)
//        {
//            if(nextCount < 10)
//            {
//                nextCount++;
//                emit pDebug("Ending draft - " + QString::number(nextCount));
//            }
//            else                endDraft();
//        }
        return false;
    }
    else if(!areSameRarity(codes))
    {
        resetCodesCandidates();
        nextCount = 0;
        return false;
    }
    else if(codes[0]!=codesCandidates[0] ||
            codes[1]!=codesCandidates[1] ||
            codes[2]!=codesCandidates[2])
    {
        emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                    codes[0] + "/" + codes[1] + "/" + codes[2] +
                    " New candidates.");
        for(int i=0; i<3; i++)
        {
            codesCandidates[i]=codes[i];
        }
        nextCount = 0;
        return false;
    }
    else if(nextCount < 3)
    {
        nextCount++;
        emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                    codes[0] + "/" + codes[1] + "/" + codes[2] +
                    " New candidates - " + QString::number(nextCount));
        return false;
    }
    else
    {
        emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                    codes[0] + "/" + codes[1] + "/" + codes[2] +
                    " New codes.");
        resetCodesCandidates();
        nextCount = 0;
        return true;
    }
}


void DraftHandler::resetCodesCandidates()
{
    for(int i=0; i<3; i++)
    {
        codesCandidates[i]="";
    }
}


bool DraftHandler::areSameRarity(QString codes[3])
{
    enum Rarity {common, rare, epic, legendary, noInit};
    Rarity raritySample = noInit;

    for(int i=0; i<3; i++)
    {
        QString rarityS = (*cardsJson)[codes[i]].value("rarity").toString();
        Rarity rarity = noInit;

        if(rarityS == "Free")               rarity = common;
        else if(rarityS == "Common")        rarity = common;
        else if(rarityS == "Rare")          rarity = rare;
        else if(rarityS == "Epic")          rarity = epic;
        else if(rarityS == "Legendary")     rarity = legendary;

        if(raritySample == noInit)  raritySample = rarity;

        if(raritySample != rarity)
        {
            emit pDebug("(" + QString::number(draftedCards.count()) + ") " +
                        codes[0] + "/" + codes[1] + "/" + codes[2] +
                        " Different rarity codes: " + QString::number(raritySample) + "/" + QString::number(rarity));
            return false;
        }
    }

    return true;
}


void DraftHandler::pickCard(QString code)
{
    if(!drafting)
    {
        captureLoop = false;
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
                updateBoxTitle(draftCard.tierScore);
                break;
            }
        }
    }

    //Clear cards and score
    for(int i=0; i<3; i++)
    {
        clearScore(draftCards[i].scoreItem);
        draftCards[i].scoreItem->hide();
        draftCards[i].setCode("");
        draftCards[i].draw();
    }

    //Clear synergies
    ui->textDraft1->setText("Detecting Cards...");
    ui->textDraft2->setText("Detecting Cards...");
    ui->textDraft3->setText("Detecting Cards...");
    ui->textBrowserDraft->setText("");

    draftScoreWindow->hideScores();

    emit pDebug("Card picked: (" + QString::number(draftedCards.count()) + ")" + draftCard.getName());
    emit pLog(tr("Draft:") + " (" + QString::number(draftedCards.count()) + ")" + draftCard.getName());
    emit newDeckCard(code);

    QTimer::singleShot(CAPTUREDRAFT_START_TIME, this, SLOT(captureDraft()));
}


void DraftHandler::showNewCards(QString codes[3])
{
    int intCodes[3];
    for(int i=0; i<3; i++)
    {
//        if(draftCards[i].radioItem->isChecked() && !draftCards[i].getCode().isEmpty())
//        {
//            pickCard(draftCards[i]);
//        }

        clearScore(draftCards[i].scoreItem);
        draftCards[i].setCode(codes[i]);
        draftCards[i].draw();
        intCodes[i] = hearthArenaCodes[codes[i]];
    }

    //Clear synergies
    ui->textDraft1->setText("");
    ui->textDraft2->setText("");
    ui->textDraft3->setText("");
    ui->textBrowserDraft->setText("");


    hearthArenaMentor->askCardsRating(arenaHero, draftedCards, intCodes);
}


void DraftHandler::updateBoxTitle(double cardRating)
{
    deckRating += cardRating;
    int numCards = draftedCards.count();
    int actualRating = (int)(deckRating/numCards);
    ui->groupBoxDraft->setTitle(QString("DECK RATING: " + QString::number(actualRating) +
                                        " (" + QString::number(numCards) + "/30)"));
}


void DraftHandler::showNewRatings(QString tip, double rating1, double rating2, double rating3,
                                  double tierScore1, double tierScore2, double tierScore3,
                                  QString synergy1, QString synergy2, QString synergy3)
{
    double ratings[3] = {rating1,rating2,rating3};
    double tierScore[3] = {tierScore1, tierScore2, tierScore3};
    double maxRating = std::max(std::max(rating1,rating2),rating3);

    for(int i=0; i<3; i++)
    {
        draftCards[i].score = ratings[i];
        draftCards[i].tierScore = tierScore[i];
        draftCards[i].scoreItem->setText(QString::number((int)ratings[i]) +
                                        " -- (" + QString::number((int)tierScore[i]) + ")");
        if(maxRating == ratings[i])     highlightScore(draftCards[i].scoreItem);

        if(learningMode)    draftCards[i].scoreItem->hide();
        else                draftCards[i].scoreItem->show();
    }

    if(!learningMode)    ui->textBrowserDraft->setText(tip);


    //Mostrar sinergies
    QString synergies[3] = {synergy1,synergy2, synergy3};
    QTextBrowser *texts[3] = {ui->textDraft1,ui->textDraft2,ui->textDraft3};

    for(int i=0; i<3; i++)
    {
        QString text = synergies[i];
        texts[i]->setText(text);
    }


    //Mostrar score
    draftScoreWindow->setScores(rating1, rating2, rating3, synergy1, synergy2, synergy3);
}


void DraftHandler::getScreenCardsHist(cv::MatND screenCardsHist[3])
{
    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens[screenIndex];
    if (!screen) return;

    QRect rect = screen->geometry();
    QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();
    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), image.bytesPerLine());

    cv::Mat screenCapture = mat.clone();

    cv::Mat bigCards[3];
    bigCards[0] = screenCapture(screenRects[0]);
    bigCards[1] = screenCapture(screenRects[1]);
    bigCards[2] = screenCapture(screenRects[2]);

    cv::Mat screenCards[3];
    cv::resize(bigCards[0], screenCards[0], cv::Size(80, 80));
    cv::resize(bigCards[1], screenCards[1], cv::Size(80, 80));
    cv::resize(bigCards[2], screenCards[2], cv::Size(80, 80));

//#ifdef QT_DEBUG
//    cv::imshow("Card1", screenCards[0]);
//    cv::imshow("Card2", screenCards[1]);
//    cv::imshow("Card3", screenCards[2]);
//#endif

    for(int i=0; i<3; i++)  screenCardsHist[i] = getHist(screenCards[i]);
}


void DraftHandler::getBestMatchingCodes(cv::MatND screenCardsHist[3], QString codes[3])
{
    double bestMatch[3];
    QString bestCodes[3];

    for(int i=0; i<3; i++)
    {
        //Init best
        bestCodes[i] = cardsHist.firstKey();
        bestMatch[i] = compareHist(screenCardsHist[i], cardsHist.first(), 3);

        for(QMap<QString, cv::MatND>::const_iterator it=cardsHist.constBegin(); it!=cardsHist.constEnd(); it++)
        {
            double match = compareHist(screenCardsHist[i], it.value(), 3);

            //Gana menor
            if(bestMatch[i] > match)
            {
                bestMatch[i] = match;
                bestCodes[i] = it.key();
            }
        }
    }


    //Minimo umbral
    for(int i=0; i<3; i++)
    {
        if(bestMatch[i] < 0.5)   codes[i] = bestCodes[i];
    }
}


cv::MatND DraftHandler::getHist(QString &code)
{
    cv::Mat fullCard = cv::imread((Utility::appPath() + "/HSCards/" + code + ".png").toStdString(), CV_LOAD_IMAGE_COLOR);
    cv::Mat srcBase = fullCard(cv::Rect(60,71,80,80));
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


bool DraftHandler::screenRectsFound()
{
    if(screenRects[0].width != 0)   return true;
    else                            return false;
}


bool DraftHandler::findScreenRects()
{
    std::vector<Point2f> templatePoints(6);
    templatePoints[0] = cvPoint(205,276); templatePoints[1] = cvPoint(205+118,276+118);
    templatePoints[2] = cvPoint(484,276); templatePoints[3] = cvPoint(484+118,276+118);
    templatePoints[4] = cvPoint(762,276); templatePoints[5] = cvPoint(762+118,276+118);


    QList<QScreen *> screens = QGuiApplication::screens();
    for(screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;

        std::vector<Point2f> screenPoints = Utility::findTemplateOnScreen("arenaTemplate.png", screen, templatePoints);
        if(screenPoints.empty())    continue;

        //Calculamos screenRect
        for(int i=0; i<3; i++)
        {
            screenRects[i]=cv::Rect(screenPoints[i*2], screenPoints[i*2+1]);
            emit pDebug("ScreenRect: " +
                        QString::number(screenRects[i].x) + "/" +
                        QString::number(screenRects[i].y) + "/" +
                        QString::number(screenRects[i].width) + "/" +
                        QString::number(screenRects[i].height));
        }

        //Creamos draftScoreWindow
        deleteDraftScoreWindow();
        QPoint topLeft(screenRects[0].x, screenRects[0].y);
        QPoint bottomRight(screenRects[2].x+screenRects[2].width,
                screenRects[2].y+screenRects[2].height);
        QRect draftRect(topLeft, bottomRight);
        QSize sizeCard(screenRects[0].width, screenRects[0].height);
        draftScoreWindow = new DraftScoreWindow((QMainWindow *)this->parent(), draftRect, sizeCard, screenIndex);
        draftScoreWindow->setLearningMode(this->learningMode);

        connect(draftScoreWindow, SIGNAL(cardEntered(QString,QRect,int,int)),
                this, SIGNAL(overlayCardEntered(QString,QRect,int,int)));
        connect(draftScoreWindow, SIGNAL(cardLeave()),
                this, SIGNAL(overlayCardLeave()));

        showOverlay();

        return true;
    }
    return false;
}


//void DraftHandler::selectMouseCard()
//{
//    QList<QScreen *> screens = QGuiApplication::screens();
//    QScreen *screen = screens[screenIndex];
//    if (!screen) return;

//    int xMouse = QCursor::pos(screen).x() - screen->geometry().x();
//    int minDist = 9999;
//    int pickedCard = 0;

//    for(int i=0; i<3; i++)
//    {
//        int xCard = screenRects[i].x + screenRects[i].width/2;
//        int dist = abs(xMouse - xCard);
//        if(dist < minDist)
//        {
//            minDist = dist;
//            pickedCard = i;
//        }
//    }

//    draftCards[pickedCard].radioItem->setChecked(true);
//}


void DraftHandler::clearScore(QLabel *label, bool clearText)
{
    if(transparency == Transparent)
    {
        label->setStyleSheet("QLabel {background-color: transparent; color: white;}");
    }
    else
    {
        label->setStyleSheet("");
    }

    if(clearText)   label->setText("");
}


void DraftHandler::highlightScore(QLabel *label)
{
    label->setStyleSheet("QLabel {background-color: transparent; color: rgb(50,175,50);}");
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
        ui->textDraft1->setStyleSheet("QTextBrowser{background-color: transparent; color: white;}");
        ui->textDraft2->setStyleSheet("QTextBrowser{background-color: transparent; color: white;}");
        ui->textDraft3->setStyleSheet("QTextBrowser{background-color: transparent; color: white;}");
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
        ui->textDraft1->setStyleSheet("");
        ui->textDraft2->setStyleSheet("");
        ui->textDraft3->setStyleSheet("");
        ui->textBrowserDraft->setStyleSheet("");
        ui->groupBoxDraft->setStyleSheet("QGroupBox{border: 0px solid transparent; margin-top: 15px; background-color: transparent; color: rgb(50,175,50);}"
                                         "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top center;}");
    }

    //Clear score labels
    clearScore(ui->labelDraft1, false);
    clearScore(ui->labelDraft2, false);
    clearScore(ui->labelDraft3, false);
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

    if(learningMode)
    {
        for(int i=0; i<3; i++)
        {
            draftCards[i].scoreItem->hide();
        }
        ui->textBrowserDraft->setText("");
    }
    else
    {
        for(int i=0; i<3; i++)
        {
            draftCards[i].scoreItem->show();
        }
    }
}


void DraftHandler::redrawAllCards()
{
    if(!drafting)   return;

    for(int i=0; i<3; i++)
    {
        draftCards[i].draw();
    }
}


//Construir json de HearthArena
//1) Copiar line (var cards = ...)
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
