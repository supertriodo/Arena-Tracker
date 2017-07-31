#include "utility.h"
#include "themehandler.h"
#include <QtWidgets>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

QMap<QString, QJsonObject> * Utility::cardsJson = NULL;
QString Utility::localLang = "enUS";
QString Utility::diacriticLetters;
QStringList Utility::noDiacriticLetters;

Utility::Utility()
{

}

Utility::~Utility()
{

}


QString Utility::heroToLogNumber(const QString &hero)
{
    if(hero.toLower().compare("druid")==0)        return QString("06");
    else if(hero.toLower().compare("hunter")==0)  return QString("05");
    else if(hero.toLower().compare("mage")==0)    return QString("08");
    else if(hero.toLower().compare("paladin")==0) return QString("04");
    else if(hero.toLower().compare("priest")==0)  return QString("09");
    else if(hero.toLower().compare("rogue")==0)   return QString("03");
    else if(hero.toLower().compare("shaman")==0)  return QString("02");
    else if(hero.toLower().compare("warlock")==0) return QString("07");
    else if(hero.toLower().compare("warrior")==0) return QString("01");
    else return QString();
}


QString Utility::heroToLogNumber(CardClass cardClass)
{
    if(cardClass == DRUID)        return QString("06");
    else if(cardClass == HUNTER)  return QString("05");
    else if(cardClass == MAGE)    return QString("08");
    else if(cardClass == PALADIN) return QString("04");
    else if(cardClass == PRIEST)  return QString("09");
    else if(cardClass == ROGUE)   return QString("03");
    else if(cardClass == SHAMAN)  return QString("02");
    else if(cardClass == WARLOCK) return QString("07");
    else if(cardClass == WARRIOR) return QString("01");
    else return QString();
}


CardClass Utility::heroFromLogNumber(QString hero)
{
    if(hero == QString("06"))       return DRUID;
    else if(hero == QString("05"))  return HUNTER;
    else if(hero == QString("08"))  return MAGE;
    else if(hero == QString("04"))  return PALADIN;
    else if(hero == QString("09"))  return PRIEST;
    else if(hero == QString("03"))  return ROGUE;
    else if(hero == QString("02"))  return SHAMAN;
    else if(hero == QString("07"))  return WARLOCK;
    else if(hero == QString("01"))  return WARRIOR;
    else                            return INVALID_CLASS;
}


QString Utility::heroStringFromLogNumber(QString hero)
{
    if(hero == QString("06"))       return "DRUID";
    else if(hero == QString("05"))  return "HUNTER";
    else if(hero == QString("08"))  return "MAGE";
    else if(hero == QString("04"))  return "PALADIN";
    else if(hero == QString("09"))  return "PRIEST";
    else if(hero == QString("03"))  return "ROGUE";
    else if(hero == QString("02"))  return "SHAMAN";
    else if(hero == QString("07"))  return "WARLOCK";
    else if(hero == QString("01"))  return "WARRIOR";
    else                            return "";
}


QString Utility::heroString2FromLogNumber(QString hero)
{
    if(hero == QString("06"))       return "Druid";
    else if(hero == QString("05"))  return "Hunter";
    else if(hero == QString("08"))  return "Mage";
    else if(hero == QString("04"))  return "Paladin";
    else if(hero == QString("09"))  return "Priest";
    else if(hero == QString("03"))  return "Rogue";
    else if(hero == QString("02"))  return "Shaman";
    else if(hero == QString("07"))  return "Warlock";
    else if(hero == QString("01"))  return "Warrior";
    else                            return "";
}


//Return info about heroes in alphabetical order
QString Utility::getHeroColor(int order)
{
    QString heroesColors[10] = {"#7f4f29", "#347c2c", "#699cd5", "#de9a3e", "#a4a4a4", "#4c4e54", "#0070DE", "#6c4177", "#912a21", "#A9A9A9"};
    return heroesColors[order];
}


QString Utility::getHeroName(int order)
{
    QString heroes[9] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    if(order>8) return "";
    return heroes[order];
}


QString Utility::getHeroLogNumber(int order)
{
    QString heroesLogNumber[9] = {"06", "05", "08", "04", "09", "03", "02", "07", "01"};
    return heroesLogNumber[order];
}


int Utility::heroLogNumber2HeroOrder(QString heroLog)
{
    int heroeOrder[9] = {8, 6, 5, 3, 1, 0, 7, 2, 4};
    int heroLogInt = heroLog.toInt() - 1;
    if(heroLogInt < 0 || heroLogInt > 8)    return 0;
    else                                    return heroeOrder[heroLogInt];
}


QString Utility::getLoadingScreenToString(LoadingScreenState loadingScreen)
{
    switch(loadingScreen)
    {
        case arena:
            return "ARENA";
        case ranked:
            return "RANKED";
        case adventure:
            return "SOLO";
        case tavernBrawl:
            return "BRAWL";
        case friendly:
            return "FRIENDLY";
        case casual:
            return "CASUAL";
        default:
            return "UNKNOWN";
    }
}


LoadingScreenState Utility::getLoadingScreenFromString(QString loadingScreenString)
{
    if(loadingScreenString == "ARENA")          return arena;
    else if(loadingScreenString == "RANKED")    return ranked;
    else if(loadingScreenString == "SOLO")      return adventure;
    else if(loadingScreenString == "BRAWL")     return tavernBrawl;
    else if(loadingScreenString == "FRIENDLY")  return friendly;
    else if(loadingScreenString == "CASUAL")    return casual;
    else                                        return unknown;
}


QString Utility::cardEnNameFromCode(QString code)
{
    return (*cardsJson)[code].value("name").toObject().value("enUS").toString();
}


QString Utility::cardEnTextFromCode(QString code)
{
    return (*cardsJson)[code].value("text").toObject().value("enUS").toString();
}


QString Utility::cardEnCodeFromName(QString name)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toObject().value("enUS").toString() == name)
        {
            if(!it->value("cost").isUndefined())    return it.key();
        }
    }

    return "";
}


QString Utility::cardLocalCodeFromName(QString name)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toObject().value(localLang).toString() == name)
        {
            if(!it->value("cost").isUndefined())    return it.key();
        }
    }

    return "";
}


QJsonValue Utility::getCardAttribute(QString code, QString attribute)
{
    if(attribute == "text" || attribute == "name")
    {
        return (*cardsJson)[code].value(attribute).toObject().value(localLang);
    }
    else
    {
        return (*cardsJson)[code].value(attribute);
    }
}


QString Utility::getCodeFromCardAttribute(QString attribute, QJsonValue value)
{
    if(attribute == "text" || attribute == "name")
    {
        for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
        {
            if(it->value(attribute).toObject().value(localLang) == value)
            {
                return it.key();
            }
        }
    }
    else
    {
        for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
        {
            if(it->value(attribute) == value)
            {
                return it.key();
            }
        }
    }

    return "";
}


bool Utility::isFromStandardSet(QString code)
{
    QString cardSet = getCardAttribute(code, "set").toString();

    if( cardSet == "CORE" || cardSet == "EXPERT1" ||
        cardSet == "OG" || cardSet == "KARA" || cardSet == "GANGS" ||
        cardSet == "UNGORO" )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool Utility::isASecret(QString code)
{
    QJsonArray mechanics = getCardAttribute(code, "mechanics").toArray();

    foreach(QJsonValue mechanic, mechanics)
    {
        if(mechanic.toString() == "SECRET") return true;
    }
    return false;
}


QString Utility::appPath()
{
    QString dirPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir dir(dirPath);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.absolutePath();
#else
    return dirPath;
#endif
}


QString Utility::dataPath()
{
    QFileInfo dirInfo(appPath() + "/Arena Tracker");
    if(dirInfo.exists())   return dirInfo.absoluteFilePath();
    else
    {
#ifdef Q_OS_LINUX
        return QDir::homePath() + "/.local/share" + "/Arena Tracker";
#else
        return QDir::homePath() + "/Arena Tracker";
#endif
    }
}


QString Utility::gameslogPath()
{
    return dataPath() + "/Games Drafts Logs";
}


QString Utility::hscardsPath()
{
    return dataPath() + "/Hearthstone Cards";
}


QString Utility::extraPath()
{
    return dataPath() + "/Extra";
}


QString Utility::themesPath()
{
    return dataPath() + "/Themes";
}


void Utility::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    Utility::cardsJson = cardsJson;
}


void Utility::setLocalLang(QString localLang)
{
    Utility::localLang = localLang;
}


QString Utility::removeAccents(QString s)
{
    if (diacriticLetters.isEmpty())
    {
        diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
        noDiacriticLetters << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
    }

    QString output = "";
    for (int i = 0; i < s.length(); i++)
    {
        QChar c = s[i];
        int dIndex = diacriticLetters.indexOf(c);
        if (dIndex < 0)
        {
            output.append(c);
        }
        else
        {
            QString replacement = noDiacriticLetters[dIndex];
            output.append(replacement);
        }
    }

    return output;
}


std::vector<Point2f> Utility::findTemplateOnScreen(QString templateImage, QScreen *screen, std::vector<Point2f> templatePoints, QPointF &screenScale)
{
    std::vector<Point2f> screenPoints;
    QRect rect = screen->geometry();
    QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();

    //Screen scale
    screenScale.setX(rect.width() / (float)image.width());
    screenScale.setY(rect.height() / (float)image.height());

    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), image.bytesPerLine());
    cv::Mat screenCapture = mat.clone();

    Mat img_object = imread((Utility::extraPath() + "/" + templateImage).toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    if(!img_object.data)
    {
        qDebug() << "Utility: Cannot find" << templateImage;
        return screenPoints;
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

    qDebug()<< "Utility: FLANN min dist:" <<min_dist;

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( matches[i].distance < /*min(0.05,max(2*min_dist, 0.02))*/0.04 )
       { good_matches.push_back( matches[i]); }
    }
    qDebug()<< "Utility: FLANN Keypoints buenos:" <<good_matches.size();
    if(good_matches.size() < 10)    return screenPoints;


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
    perspectiveTransform(templatePoints, screenPoints, H);

    //Show matches
    if(false)
    {
        Mat img_matches;
        drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        imshow( "Good Matches & Object detection", img_matches );
    }

    return screenPoints;
}


QPropertyAnimation * Utility::fadeInWidget(QWidget * widget)
{
    QGraphicsOpacityEffect *eff = (QGraphicsOpacityEffect *)widget->graphicsEffect();
    if(eff == 0)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() == 1) return NULL;

    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(ANIMATION_TIME);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(SHOW_EASING_CURVE);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    return a;
}


QPropertyAnimation * Utility::fadeOutWidget(QWidget * widget)
{
    QGraphicsOpacityEffect *eff = (QGraphicsOpacityEffect *)widget->graphicsEffect();
    if(eff == 0)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() == 0) return NULL;

    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(ANIMATION_TIME);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(SHOW_EASING_CURVE);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    return a;
}


QPixmap Utility::getTransformedImage(QPixmap image, QPointF pos, QPointF anchor, qreal rot, QPointF &origin)
{
    QPointF center = QPointF(image.width() / 2, image.height() / 2);
    qreal dist = QLineF(anchor, center).length();
    qreal a = qAtan2(anchor.y() - center.y(), anchor.x() - center.x());
    QPointF rotAnchor(qCos(rot + a) * dist, qSin(rot + a) * dist);
    rotAnchor += center;

    QPixmap rotImage = image.transformed(QTransform().rotateRadians(rot));

    QPointF rotCenter = QPointF(rotImage.width() / 2, rotImage.height() / 2);
    QPointF offset = rotCenter - center;

    origin = pos - (rotAnchor + offset);

    return rotImage;
}


bool Utility::isLeftOfScreen(QPoint center)
{
    int topScreen, bottomScreen, leftScreen, rightScreen;
    int midX = center.x();
    int midY = center.y();

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect screenRect = screen->geometry();
        topScreen = screenRect.y();
        bottomScreen = topScreen + screenRect.height();
        leftScreen = screenRect.x();
        rightScreen = leftScreen + screenRect.width();

        if(midX < leftScreen || midX > rightScreen ||
                midY < topScreen || midY > bottomScreen) continue;

        if(midX-leftScreen > rightScreen-midX)  return false;
        else                                    return true;
    }

    return true;
}


void Utility::dumpOnFile(QByteArray &data, QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"ERROR: Failed to create" << path;
        return;
    }

    file.write(data);
    file.close();
}


void Utility::drawShadowText(QPainter &painter, const QFont &font, const QString &text, int x, int y, bool alignCenter)
{
    QFontMetrics fm(font);
    int textWide = fm.width(text);
    int textHigh = fm.height();

    double offsetY = 0.25 - ThemeHandler::cardsFontOffsetY()/100.0;

    QPainterPath path;
    path.addText(x - (alignCenter?textWide/2:0), y + textHigh*offsetY, font, text);
    painter.drawPath(path);
}


void Utility::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while(QLayoutItem* item = layout->takeAt(0))
    {
        if(deleteWidgets)
        {
            if(QWidget* widget = item->widget())
            {
                widget->deleteLater();
            }
        }
        if(QLayout* childLayout = item->layout())
        {
            clearLayout(childLayout, deleteWidgets);
        }
        delete item;
    }
}


void Utility::showItemsLayout(QLayout* layout)
{
    for(int i=0; i<layout->count(); i++)
    {
        QLayoutItem *child = layout->itemAt(i);
        QWidget *widget = child->widget();
        if(widget != NULL)  widget->show();
//        if (QLayout* childLayout = child->layout())  showItemsLayout(childLayout);
    }
}
