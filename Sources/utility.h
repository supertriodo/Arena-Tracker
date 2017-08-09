#ifndef UTILITY_H
#define UTILITY_H

#include "Cards/deckcard.h"
#include "opencv2/opencv.hpp"
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QScreen>
#include <QPropertyAnimation>


using namespace cv;

#define ANIMATION_TIME 750
#define SHOW_EASING_CURVE QEasingCurve::OutCubic
#define HIDE_EASING_CURVE QEasingCurve::InCubic

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque, Framed };
enum LoadingScreenState { menu, arena, ranked, adventure, tavernBrawl, friendly, casual, unknown };
enum LogComponent { logLoadingScreen, logArena, logPower, logZone, logAsset, logInvalid };
enum DraftMethod { HearthArena, LightForge, All };



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
    static QMap<QString, QJsonObject> *cardsJson;
    static QString localLang;
    static QString diacriticLetters;
    static QStringList noDiacriticLetters;


//Metodos
public:
    static QString heroToLogNumber(const QString &hero);
    static QString heroToLogNumber(CardClass cardClass);
    static CardClass heroFromLogNumber(QString hero);
    static QString heroStringFromLogNumber(QString hero);
    static QString heroString2FromLogNumber(QString hero);
    static QString getHeroColor(int order);
    static QString getHeroName(int order);
    static QString getHeroLogNumber(int order);
    static QJsonValue getCardAttribute(QString code, QString attribute);
    static QString appPath();
    static QString dataPath();
    static QString gameslogPath();
    static QString hscardsPath();
    static QString extraPath();
    static QString cardEnNameFromCode(QString code);
    static QString cardEnCodeFromName(QString name);
    static QString cardLocalCodeFromName(QString name);
    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
    static void setLocalLang(QString localLang);
    static QString removeAccents(QString s);
    static QPropertyAnimation *fadeInWidget(QWidget *widget);
    static QPropertyAnimation *fadeOutWidget(QWidget *widget);
    static QString getLoadingScreenToString(LoadingScreenState loadingScreen);
    static LoadingScreenState getLoadingScreenFromString(QString loadingScreenString);
    static std::vector<Point2f> findTemplateOnScreen(QString templateImage, QScreen *screen, std::vector<Point2f> templatePoints, QPointF &screenScale);
    static QPixmap getTransformedImage(QPixmap image, QPointF pos, QPointF anchor, qreal rot, QPointF &origin);
    static bool isLeftOfScreen(QPoint center);
    static bool isFromStandardSet(QString code);
    static bool isASecret(QString code);
    static void dumpOnFile(QByteArray &data, QString path);
    static QString getCodeFromCardAttribute(QString attribute, QJsonValue value);
    static void drawShadowText(QPainter &painter, const QFont &font, const QString &text, int x, int y, bool alignCenter);
    static QString themesPath();
    static int heroLogNumber2HeroOrder(QString heroLog);
    static QString cardEnTextFromCode(QString code);
    static void clearLayout(QLayout *layout, bool deleteWidgets);
    static void showItemsLayout(QLayout *layout);
    static QStringList getSetCodes(QString set);
};

#endif // UTILITY_H
