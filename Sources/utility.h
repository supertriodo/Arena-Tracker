#ifndef UTILITY_H
#define UTILITY_H

#include "Cards/deckcard.h"
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QScreen>

#ifndef Q_OS_ANDROID
#include "opencv2/opencv.hpp"
using namespace cv;
#endif

#define ANIMATION_TIME 750

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque };
enum Theme { ThemeWhite, ThemeBlack };
enum LoadingScreen { menu, arena, constructed, adventure, tavernBrawl, spectator };


class GameResultPost
{
public:
    QUrlQuery postData;
    QNetworkRequest request;
};


class Utility
{
//Constructor
public:
    Utility();
    ~Utility();

//Variables
private:
    static QMap<QString, QJsonObject> *enCardsJson;   //en-us json
    static QMap<QString, QJsonObject> *cardsJson;   //local json
    static QString diacriticLetters;
    static QStringList noDiacriticLetters;


//Metodos
public:
    static QString heroToLogNumber(const QString &hero);
    static QString heroToLogNumber(CardClass cardClass);
    static CardClass heroFromLogNumber(QString hero);
    static QString heroStringFromLogNumber(QString hero);
    static QString getHeroColor(int order);
    static QString getHeroName(int order);
    static QString getHeroLogNumber(int order);
    static QString appPath();
    static QString dataPath();
    static QString gameslogPath();
    static QString hscardsPath();
    static QString extraPath();
    static QString cardEnNameFromCode(QString code);
    static QString cardEnCodeFromName(QString name);
    static QString cardLocalCodeFromName(QString name);
    static void setEnCardsJson(QMap<QString, QJsonObject> *enCardsJson);
    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
    static QString removeAccents(QString s);
    static void fadeInWidget(QWidget *widget);
    static void fadeOutWidget(QWidget *widget);
    static QString getLoadingScreenString(LoadingScreen loadingScreen);

#ifndef Q_OS_ANDROID
    static std::vector<Point2f> findTemplateOnScreen(QString templateImage, QScreen *screen, std::vector<Point2f> templatePoints, bool showMatches=false);
#endif
};

#endif // UTILITY_H
