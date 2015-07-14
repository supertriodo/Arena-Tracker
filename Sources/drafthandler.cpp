#include "drafthandler.h"
#include "deckhandler.h"
#include <QtWidgets>

DraftHandler::DraftHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::MainWindow *ui) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->cardsDownloading = 0;
    this->captureLoop = false;
    this->deckRating = 0;

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
    ui->radioButtonDraft1->setIconSize(CARD_SIZE);
    ui->radioButtonDraft2->setIconSize(CARD_SIZE);
    ui->radioButtonDraft3->setIconSize(CARD_SIZE);

    QFont font("Belwe Bd BT");
    font.setPointSize(18);
    ui->radioButtonDraft1->setFont(font);
    ui->radioButtonDraft2->setFont(font);
    ui->radioButtonDraft3->setFont(font);

    draftCards[0].radioItem = ui->radioButtonDraft1;
    draftCards[1].radioItem = ui->radioButtonDraft2;
    draftCards[2].radioItem = ui->radioButtonDraft3;

    ui->tabWidget->removeTab(1);
}


void DraftHandler::createHearthArenaMentor()
{
    hearthArenaMentor = new HearthArenaMentor(this);
    connect(hearthArenaMentor, SIGNAL(newTip(QString,double,double,double,QString,QString,QString)),
            this, SLOT(showNewCards(QString,double,double,double,QString,QString,QString)));
    connect(hearthArenaMentor, SIGNAL(sendLog(QString)),
            this->parent(), SLOT(writeLog(QString)));
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

        QFileInfo cardFile("./HSCards/" + code + ".png");
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
    qDebug() << "DraftHandler: Num histogramas BD:" << hearthArenaCodes.count();

    if(cardsDownloading==0) resumeDraft();
    else
    {
        ui->progressBar->setMaximum(cardsDownloading);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(true);
        emit setStatusBarMessage(tr("Downloading cards..."), 0);
    }
}


void DraftHandler::reHistDownloadedCardImage(QString code)
{
    if(cardsDownloading == 0)   return; //No hay drafting en proceso

    cardsHist[code] = getHist(code);
    cardsDownloading--;

    if(cardsDownloading==0)
    {
        ui->progressBar->setVisible(false);
        emit setStatusBarMessage(tr("Cards downloaded."), 3000);
        resumeDraft();
    }
    else
    {
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
}


void DraftHandler::resetTab()
{
    for(int i=0; i<3; i++)
    {
        draftCards[i].radioItem->setText("");
        draftCards[i].code="";
        draftCards[i].draw();
    }

    //Mostrar sinergies
    ui->textDraft1->setText("");
    ui->textDraft2->setText("");
    ui->textDraft3->setText("");
    ui->textBrowserDraft->setText("");

    ui->groupBoxDraft->setTitle("");
    ui->tabWidget->insertTab(1, ui->tabDraft, "Draft");
    ui->tabWidget->setCurrentIndex(1);
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
}


void DraftHandler::beginDraft(QString hero)
{
    if(captureLoop) return;

    int heroInt = hero.toInt();
    if(heroInt<1 || heroInt>9)
    {
        qDebug() << "DraftHandler: Begin draft de heroe incorrecto:" << hero;
        emit sendLog(tr("Log: ERROR: Started draft of unknown hero ") + hero);
        return;
    }
    else
    {
        qDebug() << "DraftHandler: Begin draft de heroe:" << hero;
        emit sendLog(tr("Log: New draft started."));
    }

    resetTab();
    clearLists();

    initCodesAndHistMaps(hero);
    this->arenaHero = hero;
}


void DraftHandler::resumeDraft()
{
    //Solo ocurre si ya hemos iniciado un draft (gamestate == drafting)
    if(captureLoop)    return;
    captureLoop = true;

    qDebug() << "DraftHandler: Resume draft.";
    emit sendLog(tr("Log: Draft resumed."));

    QTimer::singleShot(2000, this, SLOT(captureDraft()));
}


void DraftHandler::pauseDraft()
{
    //Para bucle captura
    captureLoop = false;

    qDebug() << "DraftHandler: Pause draft.";
    emit sendLog(tr("Log: Draft paused."));
}


void DraftHandler::endDraft()
{
    //Guardamos ultima carta
    for(int i=0; i<3; i++)
    {
        if(draftCards[i].radioItem->isChecked())
        {
            draftedCards.push_back(hearthArenaCodes[draftCards[i].code]);
            updateBoxTitle(draftCards[i].radioItem->text());
            qDebug() << "DraftHandler: Card picked:" << draftCards[i].code << "-" << draftedCards.count();
            emit sendLog(tr("Draft:") + " (" + QString::number(draftedCards.count()) + ")" + (*cardsJson)[draftCards[i].code].value("name").toString());
            emit sendLog(tr("Draft: ") + ui->groupBoxDraft->title());
        }
    }

    //Creamos el mazo
    insertIntoDeck();

    //Oculta tab
    ui->tabWidget->removeTab(1);
    ui->tabWidget->setCurrentIndex(tabDeck);

    clearLists();

    captureLoop = false;

    qDebug() << "DraftHandler: End draft.";
    emit sendLog(tr("Log: Draft ended."));
}


void DraftHandler::insertIntoDeck()
{
    int numCards = draftedCards.count();
    bool isDeckComplete = (numCards == 30);

    while(!draftedCards.isEmpty())
    {
        int total = 1;
        for(int j=draftedCards.count()-1; j>0; j--)
        {
            if(draftedCards.first() == draftedCards[j])
            {
                total++;
                draftedCards.removeAt(j);
            }
        }

        if((numCards <= 30) || total > 1)
        {
            QString code = hearthArenaCodes.key(draftedCards.first());
            emit newDeckCard(code, total);
        }
        draftedCards.pop_front();
    }

    if(isDeckComplete)
    {
        qDebug() << "DraftHandler: Deck completo de draft.";
        emit sendLog(tr("Draft: Deck complete."));
        emit deckComplete();
    }
    else
    {
        qDebug() << "DraftHandler: Deck incompleto de" << numCards << "cartas.";
        emit sendLog(tr("Draft: Deck incomplete: ") + QString::number(numCards) + tr(" cards. It'll be completed while you play."));
    }
}


void DraftHandler::captureDraft()
{
    if(!captureLoop)    return;

    //Capture
    cv::MatND screenCardsHist[3];
    if(getScreenCardsHist(screenCardsHist))
    {
        QString codes[3];
        getBestMatchingCodes(screenCardsHist, codes);

        if(areNewCards(codes))  showNewCards(codes);

        QTimer::singleShot(1000, this, SLOT(captureDraft()));
    }
    else
    {
        QTimer::singleShot(5000, this, SLOT(captureDraft()));
    }
}


bool DraftHandler::areNewCards(QString codes[3])
{
    qDebug() << codes[0] << codes[1] << codes[2];
    if((codes[0]==draftCards[0].code && codes[1]==draftCards[1].code) ||
        (codes[0]==draftCards[0].code && codes[2]==draftCards[2].code) ||
        (codes[1]==draftCards[1].code && codes[2]==draftCards[2].code))
    {
        qDebug() << "DraftHandler: Sin cambios (2+=).";
        resetCodesCandidates();
        return false;
    }
    else if(codes[0]=="" || codes[1]=="" || codes[2]=="")
    {
        qDebug() << "DraftHandler: No son cartas.";
        resetCodesCandidates();
        return false;
    }
    else if(!areSameRarity(codes))
    {
        return false;
    }
    else if(codes[0]!=codesCandidates[0] ||
            codes[1]!=codesCandidates[1] ||
            codes[2]!=codesCandidates[2])
    {
        qDebug() << "DraftHandler: Nuevos candidatos.";
        for(int i=0; i<3; i++)
        {
            codesCandidates[i]=codes[i];
        }
        return false;
    }
    else
    {
        qDebug() << "DraftHandler: Nuevas cartas.";
        resetCodesCandidates();
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
            qDebug() << "DraftHandler: Cartas de distinta rareza:" << raritySample << rarity;
            return false;
        }
    }

    return true;
}


void DraftHandler::showNewCards(QString codes[3])
{
    int intCodes[3];
    for(int i=0; i<3; i++)
    {
        if(draftCards[i].radioItem->isChecked() && !draftCards[i].code.isEmpty())
        {
            draftedCards.push_back(hearthArenaCodes[draftCards[i].code]);
            updateBoxTitle(draftCards[i].radioItem->text());
            qDebug() << "DraftHandler: Card picked:" << draftCards[i].code << "-" << draftedCards.count();
            emit sendLog(tr("Draft:") + " (" + QString::number(draftedCards.count()) + ")" + (*cardsJson)[draftCards[i].code].value("name").toString());
        }

        draftCards[i].radioItem->setText("");
        draftCards[i].code=codes[i];
        draftCards[i].draw();
        intCodes[i] = hearthArenaCodes[codes[i]];
    }

    //Limpiar texto
    ui->textDraft1->setText("");
    ui->textDraft2->setText("");
    ui->textDraft3->setText("");
    ui->textBrowserDraft->setText("");


    hearthArenaMentor->askCardsRating(arenaHero, draftedCards, intCodes);
}


void DraftHandler::updateBoxTitle(QString cardRating)
{
    deckRating += cardRating.toDouble();
    int numCards = draftedCards.count();
    int actualRating = (int)(deckRating/numCards);
    ui->groupBoxDraft->setTitle(QString("DECK RATING: " + QString::number(actualRating) +
                                        " (" + QString::number(numCards) + "/30)"));
}


void DraftHandler::showNewCards(QString tip, double rating1, double rating2, double rating3,
                                QString synergy1, QString synergy2, QString synergy3)
{
    double ratings[3] = {rating1,rating2,rating3};
    double maxRating = 0;

    for(int i=0; i<3; i++)
    {
        draftCards[i].radioItem->setText(QString::number(ratings[i]));

        if(ratings[i] > maxRating)
        {
            maxRating = ratings[i];
            draftCards[i].radioItem->setChecked(true);
        }
    }

    //Mostrar sinergies
    QString synergies[3] = {synergy1,synergy2, synergy3};
    QTextBrowser *texts[3] = {ui->textDraft1,ui->textDraft2,ui->textDraft3};

    for(int i=0; i<3; i++)
    {
        QString text = synergies[i];
        texts[i]->setText(text);
    }


    ui->textBrowserDraft->setText(tip);
}


bool DraftHandler::getScreenCardsHist(cv::MatND screenCardsHist[3])
{
    if(!findScreenRects())  return false;


    QList<QScreen *> screens = QGuiApplication::screens();
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

//    bigCards[0] = screenCapture(cv::Rect(512,259,97,97));
//    bigCards[1] = screenCapture(cv::Rect(740,259,97,97));
//    bigCards[2] = screenCapture(cv::Rect(963,259,97,97));

    cv::Mat screenCards[3];
    cv::resize(bigCards[0], screenCards[0], cv::Size(80, 80));
    cv::resize(bigCards[1], screenCards[1], cv::Size(80, 80));
    cv::resize(bigCards[2], screenCards[2], cv::Size(80, 80));

#ifdef QT_DEBUG
    cv::imshow("Card1", screenCards[0]);
    cv::imshow("Card2", screenCards[1]);
    cv::imshow("Card3", screenCards[2]);
#endif

    for(int i=0; i<3; i++)  screenCardsHist[i] = getHist(screenCards[i]);

    return true;
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


//void DraftHandler::showImage(QString code)
//{
//    if(code.isEmpty())  return;
//    cv::Mat fullCard = cv::imread(QString("./HSCards/" + code + ".png").toStdString(), CV_LOAD_IMAGE_COLOR);
//    cv::Mat srcBase = fullCard(cv::Rect(60,71,80,80));
//    cv::imshow(code.toStdString(), srcBase);
//}


cv::MatND DraftHandler::getHist(QString &code)
{
    cv::Mat fullCard = cv::imread(QString("./HSCards/" + code + ".png").toStdString(), CV_LOAD_IMAGE_COLOR);
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


bool DraftHandler::findScreenRects()
{
    if(screenRects[0].width != 0)   return true;

    QList<QScreen *> screens = QGuiApplication::screens();
    for(screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;

        QRect rect = screen->geometry();
        QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();
        cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), image.bytesPerLine());

        cv::Mat screenCapture = mat.clone();

        Mat img_object = imread("./HSCards/arenaTemplate.png", CV_LOAD_IMAGE_GRAYSCALE );
        if(!img_object.data)
        {
            qDebug() << "DraftHandler: " << "ERROR: Fallo al leer arenaTemplate.png";
            emit sendLog(tr("File: ERROR:Cannot find arenaTemplate.png. Make sure HSCards dir is in the same place as the exe."));
            return false;
        }
        Mat img_scene;
        cv::cvtColor(screenCapture, img_scene, CV_BGR2GRAY);

        //-- Step 1: Detect the keypoints using SURF Detector
        int minHessian = 400;

        SurfFeatureDetector detector( minHessian );

        std::vector<KeyPoint> keypoints_object, keypoints_scene;

        detector.detect( img_object, keypoints_object );
        detector.detect( img_scene, keypoints_scene );

        //-- Step 2: Calculate descriptors (feature vectors)
        SurfDescriptorExtractor extractor;

        Mat descriptors_object, descriptors_scene;

        extractor.compute( img_object, keypoints_object, descriptors_object );
        extractor.compute( img_scene, keypoints_scene, descriptors_scene );

        //-- Step 3: Matching descriptor vectors using FLANN matcher
        FlannBasedMatcher matcher;
        std::vector< DMatch > matches;
        matcher.match( descriptors_object, descriptors_scene, matches );

        double min_dist = 100;

        //-- Quick calculation of max and min distances between keypoints
        for( int i = 0; i < descriptors_object.rows; i++ )
        { double dist = matches[i].distance;
          if( dist < min_dist ) min_dist = dist;
        }

        qDebug()<< "DraftHandler: FLANN min dist:" <<min_dist;

        //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
        std::vector< DMatch > good_matches;

        for( int i = 0; i < descriptors_object.rows; i++ )
        { if( matches[i].distance < /*min(0.05,max(2*min_dist, 0.02))*/0.04 )
           { good_matches.push_back( matches[i]); }
        }
        qDebug()<< "DraftHandler: FLANN Keypoints buenos:" <<good_matches.size();
        if(good_matches.size() < 10)    continue;


        //-- Localize the object (find homography)
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;

        for( uint i = 0; i < good_matches.size(); i++ )
        {
          //-- Get the keypoints from the good matches
          obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
          scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
        }

        Mat H = findHomography( obj, scene, CV_RANSAC );

        //-- Get the corners from the image_1 ( the object to be "detected" )
        std::vector<Point2f> obj_corners(6);
        obj_corners[0] = cvPoint(205,276); obj_corners[1] = cvPoint(205+118,276+118);
        obj_corners[2] = cvPoint(484,276); obj_corners[3] = cvPoint(484+118,276+118);
        obj_corners[4] = cvPoint(762,276); obj_corners[5] = cvPoint(762+118,276+118);
        std::vector<Point2f> scene_corners(4);

        perspectiveTransform( obj_corners, scene_corners, H);


#ifdef QT_DEBUG
        //Show matches
        Mat img_matches;
        drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
        line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[4] + Point2f( img_object.cols, 0), scene_corners[5] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );

        //-- Show detected matches
        imshow( "Good Matches & Object detection", img_matches );
#endif


        //Calculamos screenRect
        for(int i=0; i<3; i++)
        {
            screenRects[i]=cv::Rect(scene_corners[i*2], scene_corners[i*2+1]);
            qDebug() << "DraftHandler: screenRect:" << screenRects[i].x << screenRects[i].y << screenRects[i].width << screenRects[i].height;
        }

        return true;
    }
    return false;
}




//Construir json de HearthArena
//1) Copiar line (var cards = ...)
//2) Eliminar al principio ("\")
//3) Eliminar al final (\"";)
//4) Eliminar (\\\\\\\\\\\\\\") buscar en gedit (\\\\\\\") Problemas con " en descripciones.
//5) Eliminar todas las (\)
