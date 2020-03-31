#include "utility.h"
#include "themehandler.h"
#include <QtWidgets>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

using namespace libzippp;
using namespace std;


QMap<QString, QJsonObject> * Utility::cardsJson = nullptr;
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


QString Utility::heroToHeroString(CardClass cardClass)
{
    if(cardClass == DRUID)        return QString("DRUID");
    else if(cardClass == HUNTER)  return QString("HUNTER");
    else if(cardClass == MAGE)    return QString("MAGE");
    else if(cardClass == PALADIN) return QString("PALADIN");
    else if(cardClass == PRIEST)  return QString("PRIEST");
    else if(cardClass == ROGUE)   return QString("ROGUE");
    else if(cardClass == SHAMAN)  return QString("SHAMAN");
    else if(cardClass == WARLOCK) return QString("WARLOCK");
    else if(cardClass == WARRIOR) return QString("WARRIOR");
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
    QString text = (*cardsJson)[code].value("text").toObject().value("enUS").toString();
    text.replace('\n',' ');
    text.replace(QChar(160), ' ');
    return text;
}


QString Utility::cardEnCodeFromName(QString name, bool onlyCollectible)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toObject().value("enUS").toString() == name)
        {
            if(!onlyCollectible || ((it->value("collectible").toBool()) && (!it.key().startsWith("HERO_"))))
            {
                return it.key();
            }
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


CardType Utility::getTypeFromCode(QString code)
{
    QString value = Utility::getCardAttribute(code, "type").toString();
    if(value == "MINION")           return MINION;
    else if(value == "SPELL")       return SPELL;
    else if(value == "WEAPON")      return WEAPON;
    else if(value == "ENCHANTMENT") return ENCHANTMENT;
    else if(value == "HERO")        return HERO;
    else if(value == "HERO_POWER")  return HERO_POWER;
    else                            return INVALID_TYPE;
}


CardRarity Utility::getRarityFromCode(QString code)
{
    QString value = Utility::getCardAttribute(code, "rarity").toString();
    if(value == "FREE")             return COMMON;
    else if(value == "COMMON")      return COMMON;
    else if(value == "RARE")        return RARE;
    else if(value == "EPIC")        return EPIC;
    else if(value == "LEGENDARY")   return LEGENDARY;
    else                            return INVALID_RARITY;
}


CardRace Utility::getRaceFromCode(QString code)
{
    QString value = Utility::getCardAttribute(code, "race").toString();
    if(value == "BLOODELF")         return BLOODELF;
    else if(value == "DRAENEI")     return DRAENEI;
    else if(value == "DWARF")       return DWARF;
    else if(value == "GNOME")       return GNOME;
    else if(value == "GOBLIN")      return GOBLIN;
    else if(value == "HUMAN")       return HUMAN;
    else if(value == "NIGHTELF")    return NIGHTELF;
    else if(value == "ORC")         return ORC;
    else if(value == "TAUREN")      return TAUREN;
    else if(value == "TROLL")       return TROLL;
    else if(value == "UNDEAD")      return UNDEAD;
    else if(value == "WORGEN")      return WORGEN;
    else if(value == "GOBLIN2")     return GOBLIN2;
    else if(value == "MURLOC")      return MURLOC;
    else if(value == "DEMON")       return DEMON;
    else if(value == "SCOURGE")     return SCOURGE;
    else if(value == "MECHANICAL")  return MECHANICAL;
    else if(value == "ELEMENTAL")   return ELEMENTAL;
    else if(value == "OGRE")        return OGRE;
    else if(value == "BEAST")       return BEAST;
    else if(value == "TOTEM")       return TOTEM;
    else if(value == "NERUBIAN")    return NERUBIAN;
    else if(value == "PIRATE")      return PIRATE;
    else if(value == "DRAGON")      return DRAGON;
    else                            return INVALID_RACE;
}


CardClass Utility::getClassFromCode(QString code)
{
    QString value = Utility::getCardAttribute(code, "cardClass").toString();
    if(value == "")             return NEUTRAL;
    else if(value == "NEUTRAL") return NEUTRAL;
    else if(value == "DRUID")   return DRUID;
    else if(value == "HUNTER")  return HUNTER;
    else if(value == "MAGE")    return MAGE;
    else if(value == "PALADIN") return PALADIN;
    else if(value == "PRIEST")  return PRIEST;
    else if(value == "ROGUE")   return ROGUE;
    else if(value == "SHAMAN")  return SHAMAN;
    else if(value == "WARLOCK") return WARLOCK;
    else if(value == "WARRIOR") return WARRIOR;
    else if(value == "DREAM")   return DREAM;
    else                        return INVALID_CLASS;
}


bool Utility::isFromStandardSet(QString code)
{
    QString cardSet = getCardAttribute(code, "set").toString();

    if( cardSet == "CORE" || cardSet == "EXPERT1" ||
//        cardSet == "HOF" || cardSet == "NAXX" || cardSet == "GVG" ||
//        cardSet == "BRM" || cardSet == "TGT" || cardSet == "LOE" ||
//        cardSet == "OG" || cardSet == "KARA" || cardSet == "GANGS" ||
//        cardSet == "UNGORO" || cardSet == "ICECROWN" || cardSet == "LOOTAPALOOZA" ||
        cardSet == "GILNEAS" || cardSet == "BOOMSDAY" || cardSet == "TROLL" ||
        cardSet == "DALARAN" || cardSet == "ULDUM" || cardSet == "DRAGONS" || cardSet == "YEAR_OF_THE_DRAGON" ||
        cardSet == "BLACK_TEMPLE")
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool Utility::hasGoldenImage(QString code)
{
    QString cardSet = getCardAttribute(code, "set").toString();

    if( cardSet == "CORE" || cardSet == "EXPERT1" ||
        cardSet == "HOF" || cardSet == "NAXX" || cardSet == "GVG" ||
        cardSet == "BRM" || cardSet == "TGT" || cardSet == "LOE" ||
        cardSet == "OG" || cardSet == "KARA" || cardSet == "GANGS" ||
        cardSet == "UNGORO" || cardSet == "ICECROWN" || cardSet == "LOOTAPALOOZA" ||
        cardSet == "GILNEAS" || cardSet == "BOOMSDAY")
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


QStringList Utility::getSetCodes(const QString &set, bool onlyCollectible)
{
    QStringList setCodes;
    for(const QString &code: Utility::cardsJson->keys())
    {
        if(getCardAttribute(code, "set").toString() == set)
        {
            if(!onlyCollectible || getCardAttribute(code, "collectible").toBool())
            {
                setCodes.append(code);
            }
        }
    }
    return setCodes;
}


QStringList Utility::getWildCodes()
{
    QStringList setCodes;
    for(const QString &code: Utility::cardsJson->keys())
    {
        if(getCardAttribute(code, "collectible").toBool() == true)
        {
            setCodes.append(code);
        }
    }
    return setCodes;
}


QStringList Utility::getStandardCodes()
{
    QStringList setCodes;
    for(const QString &code: Utility::cardsJson->keys())
    {
        if(Utility::isFromStandardSet(code) &&
            (getCardAttribute(code, "collectible").toBool() == true))
        {
            setCodes.append(code);
        }
    }
    return setCodes;
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
    screenScale.setX(rect.width() / static_cast<qreal>(image.width()));
    screenScale.setY(rect.height() / static_cast<qreal>(image.height()));

    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), static_cast<size_t>(image.bytesPerLine()));
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
    { double dist = static_cast<double>(matches[static_cast<ulong>(i)].distance);
      if( dist < min_dist ) min_dist = dist;
    }

    qDebug()<< "Utility: FLANN min dist:" <<min_dist;

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( static_cast<double>(matches[static_cast<ulong>(i)].distance) < /*min(0.05,max(2*min_dist, 0.02))*/0.04 )
       { good_matches.push_back( matches[static_cast<ulong>(i)]); }
    }
    qDebug()<< "Utility: FLANN Keypoints buenos:" <<good_matches.size();
    if(good_matches.size() < 10)    return screenPoints;


    //-- Localize the object (find homography)
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for( uint i = 0; i < good_matches.size(); i++ )
    {
      //-- Get the keypoints from the good matches
      obj.push_back( keypoints_object[ static_cast<ulong>(good_matches[i].queryIdx) ].pt );
      scene.push_back( keypoints_scene[ static_cast<ulong>(good_matches[i].trainIdx) ].pt );
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
    QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if(eff == nullptr)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() < 1)
    {
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(ANIMATION_TIME);
        a->setStartValue(0);
        a->setEndValue(1);
        a->setEasingCurve(SHOW_EASING_CURVE);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        return a;
    }
    else    return nullptr;
}


QPropertyAnimation * Utility::fadeOutWidget(QWidget * widget)
{
    QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if(eff == nullptr)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() > 0)
    {
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(ANIMATION_TIME);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(SHOW_EASING_CURVE);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        return a;
    }
    else    return nullptr;
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


void Utility::dumpOnFile(const QByteArray &data, QString path)
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


void Utility::drawShadowText(QPainter &painter, const QFont &font, const QString &text, int x, int y, bool alignCenter, bool isCardText)
{
    //Gran parte de los textos pintados usan el offset (que solo deberia ser usado para el texto de cartas),
    //Eso hace que en los diferentes temas muestren el texto diferente donde no deberian (Ej Vida/Atk cartas replay)
    QFontMetrics fm(font);

    int textWide = fm.width(text);
    int textHigh = fm.height();

    double offsetY = 0.25 - (isCardText?ThemeHandler::cardsFontOffsetY():0)/100.0;
    if(font.family() == LG_FONT)
    {
#ifdef Q_OS_WIN
        offsetY += 0.05;
#else
        offsetY += 0.15;
#endif
    }

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
        if(widget != nullptr)  widget->show();
//        if (QLayout* childLayout = child->layout())  showItemsLayout(childLayout);
    }
}


float Utility::normalizeLF(float score, bool doit)
{
    if(doit)    return score - 45;
    else        return score;
}


bool Utility::createDir(QString pathDir)
{
    QFileInfo dirInfo(pathDir);
    if(!dirInfo.exists())
    {
        QDir().mkdir(pathDir);
        qDebug() << pathDir + " created.";
        return true;
    }
    return false;
}


void Utility::unZip(QString zipName, QString targetPath)
{
    ZipArchive zf(zipName.toStdString());
    zf.open(ZipArchive::READ_ONLY);

    vector<ZipEntry> entries = zf.getEntries();
    vector<ZipEntry>::iterator it;
    for(it=entries.begin() ; it!=entries.end(); ++it)
    {
        ZipEntry entry = *it;
        QString name = entry.getName().data();
        int size = static_cast<int>(entry.getSize());
        if(name.endsWith('/'))
        {
#ifdef Q_OS_MAC
            if(!name.endsWith("__MACOSX/"))
#endif
            createDir(targetPath + "/" + name);
        }
        else
        {
            char* binaryData = static_cast<char *>(entry.readAsBinary());
            QByteArray byteArray(binaryData, size);
            dumpOnFile(byteArray, targetPath + "/" + name);
            qDebug() << "Unzipped " + name;
            delete[] binaryData;
        }
    }

    zf.close();
}


void Utility::resizeGoldenCards()
{
    QString goldenDir = QDir::homePath() + "/Documentos/ArenaTracker/HearthstoneGoldenCards";
    QDir dir(goldenDir);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filterName;
    filterName << "*.png";
    dir.setNameFilters(filterName);

    for(const QString &file: dir.entryList())
    {
        QImage webImage(goldenDir + "/" + file);
        webImage = webImage.scaledToWidth(205, Qt::SmoothTransformation);
        webImage = webImage.copy(4, -10, 200, 303);

        if(!webImage.save(goldenDir + "/Resized/" + file, "png"))
        {
            qDebug()<<"Failed to save card image to disk: " + file;
        }
        else
        {
            qDebug()<<"Card resized: " + file;
        }
    }
}


void Utility::fixLightforgeTierlist()
{
    QString originalLF = QDir::homePath() + "/Documentos/ArenaTracker/LightForge/Json extract/originalLF.json";
    QString cardMapLF = QDir::homePath() + "/Documentos/ArenaTracker/LightForge/Json extract/lightForgeCardMaps.json";
    QString fixedLF1 = QDir::homePath() + "/Documentos/ArenaTracker/LightForge/lightForge.json";
    QString fixedLF2 = Utility::extraPath() + "/lightForge.json";

    QFile tierList(originalLF);
    tierList.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = QString::fromUtf8(tierList.readAll());
    tierList.close();

    QFile jsonFile(cardMapLF);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    const QJsonArray jsonArray = jsonDoc.object().value("CardMaps").toArray();
    for(QJsonValue jsonCardMap: jsonArray)
    {
        QJsonObject jsonCardMapObject = jsonCardMap.toObject();
        QString lfCode = jsonCardMapObject.value("CardId").toString();
        QString code = jsonCardMapObject.value("ClientId").toString();
        data.replace(lfCode, code);
    }

    //Copy to local and source
    QFile tierListFixed(fixedLF1);
    tierListFixed.open(QIODevice::WriteOnly | QIODevice::Text);
    tierListFixed.write(data.toUtf8());
    tierListFixed.close();
    QFile::remove(fixedLF2);
    QFile::copy(fixedLF1, fixedLF2);
    qDebug()<<"DEBUG TL: lightForge.json created (source and local)";
}


void Utility::checkTierlistsCount()
{
    QString allHeroes[] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    for(const QString &heroString: allHeroes)
    {
        qDebug()<<endl<<"-----"<<heroString<<"-----";
        QStringList lfCodes, haCodes;

        //LightForge Codes List
        QFile jsonFileLF(Utility::extraPath() + "/lightForge.json");
        jsonFileLF.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonDocument jsonDocLF = QJsonDocument::fromJson(jsonFileLF.readAll());
        jsonFileLF.close();
        const QJsonArray jsonCardsArrayLF = jsonDocLF.object().value("Cards").toArray();
        for(QJsonValue jsonCard: jsonCardsArrayLF)
        {
            QJsonObject jsonCardObject = jsonCard.toObject();
            QString code = jsonCardObject.value("CardId").toString();

            const QJsonArray jsonScoresArray = jsonCardObject.value("Scores").toArray();
            for(QJsonValue jsonScore: jsonScoresArray)
            {
                QJsonObject jsonScoreObject = jsonScore.toObject();
                QString hero = jsonScoreObject.value("Hero").toString();

                if(hero == nullptr || hero == heroString)
                {
                    if(!lfCodes.contains(code))  lfCodes.append(code);
                }
            }
        }

        //HearthArena Codes List
        QFile jsonFileHA(Utility::extraPath() + "/hearthArena.json");
        jsonFileHA.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonDocument jsonDocHA = QJsonDocument::fromJson(jsonFileHA.readAll());
        jsonFileHA.close();

        QJsonObject jsonNamesObjectHA = jsonDocHA.object().value(heroString).toObject();
        for(const QString &name: jsonNamesObjectHA.keys())
        {
            QString code = Utility::cardEnCodeFromName(name);
            if(code.isEmpty())  code = Utility::cardEnCodeFromName(name, false);
            if(code.isEmpty())  qDebug()<<"HearthArena wrong name:"<<name;
            else                haCodes.append(code);
        }

        qDebug()<<heroString<<"LightForge count:"<<lfCodes.count();
        qDebug()<<getArenaSets(lfCodes);

        qDebug()<<heroString<<"HearthArena count:"<<haCodes.count();
        qDebug()<<getArenaSets(haCodes);

        //Check Missing cards
        bool missing = false;
        for(const QString &code: lfCodes)
        {
            if(!haCodes.contains(code))
            {
                qDebug()<<"HearthArena missing:"<<code<<Utility::cardEnNameFromCode(code);
                missing = true;
            }
        }
        if(!missing)    qDebug()<<"HearthArena OK!";
        missing = false;
        for(const QString &code: haCodes)
        {
            if(!lfCodes.contains(code))
            {
                qDebug()<<"LightForge missing:"<<code<<Utility::cardEnNameFromCode(code);
                missing = true;
            }
        }
        if(!missing)    qDebug()<<"LightForge OK!";
    }
}


QStringList Utility::getArenaSets(const QStringList &codeList)
{
    QStringList arenaSets;
    for(const QString &code: codeList)
    {
        QString cardSet = Utility::getCardAttribute(code, "set").toString();
        if(!arenaSets.contains(cardSet))    arenaSets.append(cardSet);
    }
    return arenaSets;
}


bool Utility::checkHeroPortrait(QString code)
{
    if(code.isEmpty())  return false;

    QFileInfo cardFile(QDir::homePath() + "/Documentos/ArenaTracker/HearthstoneCards/" + code + ".png");

    if(!cardFile.exists())  return false;
    return true;
}


void Utility::checkMissingGoldenCards()
{
    QDir dir(QDir::homePath() + "/Documentos/ArenaTracker/HearthstoneCards");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filterName;
    filterName << "*.png";
    dir.setNameFilters(filterName);

    QStringList files = dir.entryList();

    for(int i = 0; i < (files.count()-1); i++)
    {
        if(!files[i].endsWith("_premium.png"))
        {
            QString code = files[i].left(files[i].length()-4);

            if(files[i+1].endsWith("_premium.png"))
            {
                if(code == files[i+1].left(files[i+1].length()-12))
                {
                    if(QFileInfo(dir.absoluteFilePath(files[i])).size() == QFileInfo(dir.absoluteFilePath(files[i+1])).size())
                    {
                        qDebug()<<"Same golden:" << code << "-" << Utility::cardEnNameFromCode(code);
                    }
                }
                else
                {
                    qDebug()<<"DEBUG MISSING GOLDEN: ERROR: Files missing"<<files[i]<<files[i+1];
                }
            }
            else if(hasGoldenImage(code) && !code.startsWith("HERO_"))
            {
                qDebug()<<"----- NO golden:" << code << "-" << Utility::cardEnNameFromCode(code);
            }
        }
    }
}


//Whizband Decks
QString Utility::whizbangDeckString(QString code)
{
    if(code == QString("2648")) return "### Ysera's Dream Defenders\nAAECAZICBvX8AuyJA+iUA/atA4iwA4exAwxAX8QGjQiJ8QK/8gLoiQOvogOwrQP9rQOMrgOBsQMA";
    else if(code == QString("2650")) return "### Trees are Friends\nAAECAZICBNWDA8OUA9OcA/etAw39Au0D9wPmBdfvAt/7Ar/9ArSJA86UA6+iA/ytA/6tA/+tAwA=";
    else if(code == QString("2652")) return "### Galakrond's Fury\nAAECAaoIBpMJ7/cC9ooD5qUD47QD08ADDI/7Apz/ArSXA8aZA8+lA9SlA7WtA7atA7etA7mtA6qvA9CvAwA=";
    else if(code == QString("2654")) return "### Corrupt the Waters\nAAECAaoICqbwAu/3Aur6ArmZA72ZA8WZA9qdA4SnA+GoA/CtAwr1BN4F/gWyBu/xAq2RA7SRA8aZA7ulA8+lAwA=";
    else if(code == QString("2656")) return "### Drop the Ancharrr!\nAAECAQcEyAOvBN6tA+iwAw0csAL/A6gF1AXuBucHnfACn6ED9agD3K0D3a0D6bADAA==";
    else if(code == QString("2657")) return "### Hack the System\nAAECAQcES6CAA/eoA+iwAw3/A6IE/wf7DJ3wApvzAvT1Ap77ArP8AoiHA5+hA/WoA/aoAwA=";
    else if(code == QString("2658")) return "### Galakrond's Guile\nAAECAaIHCJwCsgKvBPYEtIYDkpcD/q4Dy8ADC7QBlwaIB90Ihgm0kQOPlwO2rgO5rgOqrwPOrwMA";
    else if(code == QString("2659")) return "### Bazaar Burglary\nAAECAaIHBrICmwXr8AK0hgOnqAOqqAMMtAGIB90IhgnH+AKPlwOQlwP7mgP+mgO7pQOtqAO3rgMA";
    else if(code == QString("2661")) return "### Lightforged Retribution\nAAECAZ8FAvoG/fsCDtwDrwf+B9b+Atn+AsWhA8OkA5amA5asA4ytA4euA5CuA5uuA5yuAwA=";
    else if(code == QString("2662")) return "### Perfect Justice\nAAECAZ8FHooB3APSBN4F8gX0Bc8G+gavB/YH/gePCfsMm/AC/fsChPwCoIADvYYD44YD7IYD7IkDg6EDoaED/KMDw6QDhKcDlqwDiK4DkK4DkbEDAAA=";
    else if(code == QString("2665")) return "### Dragonbane's Devastation\nAAECAR8KxwOvBMkErgaY8AKA8wKnggOghQOfpQOHsAMKngG1A4cE/gz1iQO+mAOipQP7rwP8rwOFsAMA";
    else if(code == QString("2666")) return "### Unseal the Vault\nAAECAR8GuwXtCcn4AuOLA+aWA6SlAwy1A94E2wmBCvbsAqCFA6SIA9ePA56dA8edA+SkA5ipAwA=";
    else if(code == QString("2667")) return "### The Clutches of Valdris\nAAECAf0GBsQInPgCoIAD7IkD8KwD8awDDIoB4QeNCPMM8fcC2pYDoaED+qQD5awD66wD7KwD7qwDAA==";
    else if(code == QString("2668")) return "### Ultimate Impfestation\nAAECAf0GAA8w0wHOB9kHsQjCCJDuAp7xAvT3AquRA7+YA4CaA4edA4idA/qkAwA=";
    else if(code == QString("2670")) return "### Aspects of the Arcane\nAAECAf0EBJaaA4qeA4GxA+G2Aw3hB40Izu8CifEC6IkD7IkD55UDg5YDoJsD9awD+qwD8K8Dh7EDAA==";
    else if(code == QString("2671")) return "### Reno's Riches\nAAECAf0EHk2KAbsCiwPJA6sEywSWBd4F8gWKB+wH+wye8AK38QLG+AKggAPsiQPnlQO9mQOfmwOKngOhoQP8owOSpAO/pAOEpwP1rAPsrwORsQMAAA==";
    else if(code == QString("2672")) return "### Galakrond's Wit\nAAECAa0GCPYH5fcCg5QDqaUDmakDqq8D47QDyMADC+UE1QjSCvIMl4cD0qUDhKgD16wD2qwD/q4Dza8DAA==";
    else if(code == QString("2673")) return "### Day of the Dead\nAAECAa0GCNcKvfMC+/4CoIAD1pkDk5sDg6ADn6kDC/gC5QTRCtMK8vECl4cDgpQDmJsDmZsD0qUD2awDAA==";

    else                             return "";
}

CardClass Utility::whizbangHero(QString code)
{
    if(code == QString("2648") || code == QString("2650")) return DRUID;
    else if(code == QString("2665") || code == QString("2666")) return HUNTER;
    else if(code == QString("2670") || code == QString("2671")) return MAGE;
    else if(code == QString("2661") || code == QString("2662")) return PALADIN;
    else if(code == QString("2672") || code == QString("2673")) return PRIEST;
    else if(code == QString("2658") || code == QString("2659")) return ROGUE;
    else if(code == QString("2652") || code == QString("2654")) return SHAMAN;
    else if(code == QString("2667") || code == QString("2668")) return WARLOCK;
    else if(code == QString("2656") || code == QString("2657")) return WARRIOR;

    else                                                         return INVALID_CLASS;
}





