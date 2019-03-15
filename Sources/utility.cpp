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


QString Utility::cardEnCodeFromName(QString name, bool onlyCollectible)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toObject().value("enUS").toString() == name)
        {
            if(!onlyCollectible || it->value("collectible").toBool())    return it.key();
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
        cardSet == "UNGORO" || cardSet == "ICECROWN" || cardSet == "LOOTAPALOOZA" ||
        cardSet == "GILNEAS" || cardSet == "BOOMSDAY" || cardSet == "TROLL")
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
    QString fixedLF = QDir::homePath() + "/Documentos/ArenaTracker/LightForge/Json extract/lightForge.json";

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

    QFile tierListFixed(fixedLF);
    tierListFixed.open(QIODevice::WriteOnly | QIODevice::Text);
    tierListFixed.write(data.toUtf8());
    tierListFixed.close();
    qDebug()<<"lightForge.json created";
}


void Utility::checkTierlistsCount()
{
    QString allHeroes[] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    for(const QString &heroString: allHeroes)
    {
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
            haCodes.append(code);
        }

        qDebug()<<endl<<"-----"<<heroString<<"-----";
        qDebug()<<heroString<<"LightForge count:"<<lfCodes.count();
        qDebug()<<heroString<<"HearthArena count:"<<haCodes.count();

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


//Whizband Decks
QString Utility::whizbangDeckString(QString code)
{
    if(code == QString("1606")) return "### Out of the Woods\nAAECAZICBMnCAofOAsLOApnTAg1AX8QG5AiU0gKY0gKo0gKL4QKE5gKL5gL15wLf+wLo/AIA";
    else if(code == QString("1607")) return "### I Hunt Alone\nAAECAR8IhwTFCN3SAunSAobTAvLqApuFA6KKAwuoArUDyQSXCNsJ/gzf0gLj0gLh4wLq4wKH+wIA";
    else if(code == QString("1609")) return "### Spells are fun, SO FUN!\nAAECAf0EBNACvwib0wKj6wINTYoByQPsB/sMysMClscCx8cC29MC1eEC1+ECluQC1+sCAA==";
    else if(code == QString("1754")) return "### Greymane's Alliance\nAAECAZ8FBvoGucEC4fACzfQC6/cC/fsCDNwD9AXPBq8HsQizwQKIxwLZxwKbywK35wL27ALZ/gIA";
    else if(code == QString("1760")) return "### The Gilneas Armory\nAAECAQcIqgbTwwKZxwLN7wKb8AKe+AKO+wKggAMLS5EDogT/B5vCAsrDAqLHAsrnAqrsArrsAvLxAgA=";
    else if(code == QString("1817")) return "### The Boomsday Project\nAAECAQcEze8Cm/ACkvgCoIADDZEGzM0CuuwCnfACl/MCn/UCpfUC5PcCjvgCg/sCqPsCs/wCzIEDAA==";
    else if(code == QString("1818")) return "### Demonology Lab\nAAECAf0GApfTAo+CAw4w9wTCCPYIm8sC980C8dAC8tAC9PcC0/gCqvkCt/0Cw/0C+v4CAA==";
    else if(code == QString("1819")) return "### The Omega Project\nAAECAf0GApfTApz4Ag6KAbYHxAjnywLy0AL40AKI0gL85QLq5gLo5wK38QLF8wL8+gKPgAMA";
    else if(code == QString("1820")) return "### Stolen Research\nAAECAaIHBLICgNMC6/ACqPcCDbQBywObBYYJgcIC68ICm8gC5dEC2+MC6vMCt/UCovcCx/gCAA==";
    else if(code == QString("1821")) return "### The Necrium Trials\nAAECAaIHBvYEi+ECz+ECnOICn/gCi4oDDLQBjAKGCYHCAqvCAuvCAtvjArT2At76Auz8AtGBA86MAwA=";
    else if(code == QString("1825")) return "### Kangor's Endless Army\nAAECAZ8FBvQF+ga5wQLx/gKggAPehgMM3AOPCbPBAuPLAp/1AqX1Atb+Atn+AuH+ApGAA9GAA8yBAwA=";
    else if(code == QString("1826")) return "### Witchwood Awoken\nAAECAaoIDN4F7QX/BYoHwAfPxwLCzgLD6gKn7gLv9wLq+gLzigMJgQT1BP4Fx8ECm8sC8+cCm/8CioADl4ADAA==";
    else if(code == QString("1827")) return "### The Storm Bringer\nAAECAaoIBMAH88ICofgCmfsCDb0B+QOGBvAHkwnrwgKw8AL28AKz9wLq+gKP+wKc/wKKgAMA";
    else if(code == QString("1828")) return "### Flark's Fireworks\nAAECAR8C4fUCoIADDo0Bigbh4wKf9QLg9QLi9QLv9QKZ9wK5+AKR+wKY+wKE/QL2/QLMgQMA";
    else if(code == QString("1829")) return "### Trees Are Friends\nAAECAZICAiTF/QIO/QLtA/cD5gWxCIbBAqTCAuvCAtfvAsHzAt/7Ar/9AtWDA7SJAwA=";
    else if(code == QString("1830")) return "### Quest For Immortality\nAAECAa0GBMnCApbEAsv4Ao2CAw37AeUE0wryDKvCAubMAvDPAujQAovhAoL3AqH+AvX+AoiCAwA=";
    else if(code == QString("1831")) return "### Awesome Augmentation\nAAECAa0GAqUJvsgCDvgC5QT2B9EK0gryDPsM0cEC2MEC5fcC5ogDi4kD0okD64oDAA==";
    else if(code == QString("1832")) return "### Shooting Stars\nAAECAf0EAqLTAu72Ag67ApUDvwOrBLQElgW/wQL77AKS7wK89wKj/QKV/wK5/wLvgAMA";
    else if(code == QString("1930")) return "### Feral Frenzy\nAAECAZICBpkC/M0CmdMCm+gC9fwCwYYDDEC0BeYFmgjrwgKHzgKY0gKm7wKYhgPAhgPGhgPPiQMA";
    else if(code == QString("1931")) return "### Animal Instincts\nAAECAR8G7QmG0wKA8wLqiQOiigPjiwMMqAK1A+sH2wmBCo7DAtfNAt3SAovlAqCFA7CLA+SLAwA=";
    else if(code == QString("1932")) return "### Power of the Dragonhawk\nAAECAf0ECNACxQT7DNPFApvTAu72Ap74AqiHAwvsB5vCAuvCAsrDAtfhApbkArfxAr36AqSHA6aHA82JAwA=";
    else if(code == QString("1933")) return "### Shirvallah's Chosen\nAAECAZ8FBuPjApvwAv37Atn+Ar2GA+OGAwzcA/QFrwf2B8rDAojHAuPLAvnsAt6GA+aGA+yGA++GAwA=";
    else if(code == QString("1934")) return "### Surrender to Madness\nAAECAa0GBpvCAsnHAsrLApziAqeHA8CPAwzlBPIM+wzKwwKbywLo0ALL5gKJ8QLeggPqiAOwiQPsiQMA";
    else if(code == QString("1935")) return "### Cursed Crew\nAAECAaIHBM0D18oCzfQC1owDDbQBiAfnB4YJ3NEC2+MC3+8CovcCqv8CroUDz4kDzowD24wDAA==";
    else if(code == QString("1936")) return "### Big Bad Voodoo\nAAECAaoIBpMJ688CsPAC4vgCmfsCy4UDDPAHkcEC68ICysMCm8sC+9MC3+kCm/8CnP8CvYUD24kD5YkDAA==";
    else if(code == QString("1937")) return "### Reckless Ritual\nAAECAf0GBJTHArjQAo+CA/CGAw0w0AT3BM4HwgjrwgKRxwKSzQL3zQLx0ALy0ALWhgOvjQMA";
    else if(code == QString("1938")) return "### Wings of War\nAAECAQcGkAf/B6IJ+wz4hgOShwMMogTJxwLMzQKJ8QKb8wL09QKBhwOLhwPoiQPsiQOqiwPolAMA";
    else if(code == QString("1978")) return "### Hir'eek's Offering\nAAECAf0GApfTAvKGAw4w9wTCCPYI68ICm8sC980C8dAC8tAC9PcC0/gCw/0C6YYD3YkDAA==";
    else if(code == QString("2118")) return "### Trees Are Friends\nAAECAZICAiTF/QIO/QLtA/cD5gWxCIbBAqTCAuvCAtfvAsHzAt/7AuH7Ar/9AtWDAwA=";
    else if(code == QString("2119")) return "### The Boomsday Project\nAAECAQcEze8Cm/ACkvgCoIADDczNArrsAp3wApfzAp/1AqX1AuT3Ao74AoP7Aqj7ArP8AsyBA/iGAwA=";
    else if(code == QString("2121")) return "### The Necrium Trials\nAAECAaIHCIwC7QX7BeXRAs/hAvDmAtjpAp/4Agu0AYHCAqvCAuvCAtvjAurmArT2At76Auz8Avb9AtGBAwA=";
    else if(code == QString("2122")) return "### Kangor's Endless Army\nAAECAZ8FBvQFzwb6BrnBAvH+AqCAAwzcA48Js8EC48sCn/UCpfUC1v4C2f4C4f4CkYAD0YADzIEDAA==";
    else if(code == QString("2123")) return "### I Hunt Alone\nAAECAR8EhwTp0gKG0wLy6gINjQGoArUDyQSXCNsJ/gzd0gLf0gLj0gLh4wLq4wKH+wIA";
    else if(code == QString("2124")) return "### Spells are fun, SO FUN!\nAAECAf0EBNACm9MCo+sCr4cDDU2KAckD7Af7DMrDApbHAtvTAtXhAtfhApbkAtfrAs2JAwA=";
    else if(code == QString("2125")) return "### Awesome Augmentation\nAAECAa0GBKIJvsgC2OMCy/gCDfgC5QSNCNEK8gzRwQLYwQLL5gKC9wLl9wL1/gLxgAPeggMA";

    else                             return "";
}

CardClass Utility::whizbangHero(QString code)
{
    if(code == QString("1930") || code == QString("1829") || code == QString("1606") || code == QString("2118")) return DRUID;
    else if(code == QString("1607") || code == QString("1931") || code == QString("1828") || code == QString("2123")) return HUNTER;
    else if(code == QString("2124") || code == QString("1932") || code == QString("1832") || code == QString("1609")) return MAGE;
    else if(code == QString("1933") || code == QString("1825") || code == QString("1754") || code == QString("2122")) return PALADIN;
    else if(code == QString("1934") || code == QString("1831") || code == QString("1830") || code == QString("2125")) return PRIEST;
    else if(code == QString("1935") || code == QString("1821") || code == QString("1820") || code == QString("2121")) return ROGUE;
    else if(code == QString("1826") || code == QString("1936") || code == QString("1827")) return SHAMAN;
    else if(code == QString("1937") || code == QString("1978") || code == QString("1819") || code == QString("1818")) return WARLOCK;
    else if(code == QString("1938") || code == QString("2119") || code == QString("1760") || code == QString("1817")) return WARRIOR;

    else                                                         return INVALID_CLASS;
}





