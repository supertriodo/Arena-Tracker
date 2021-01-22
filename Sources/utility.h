#ifndef UTILITY_H
#define UTILITY_H

#include "Cards/deckcard.h"
#include "opencv2/opencv.hpp"
#include "Utils/libzippp.h"
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QScreen>
#include <QPropertyAnimation>


#define REMOVE_CARDS_ON_VERSION_UPDATE false
#define REMOVE_EXTRA_ON_VERSION_UPDATE false
#define DEBUG_REPLAY_AUTO_ADVANCE false


using namespace cv;

#define FLOATEQ(X, Y)  (fabs(X - Y) < 0.000001f)
#define ANIMATION_TIME 750
#define SHOW_EASING_CURVE QEasingCurve::OutCubic
#define HIDE_EASING_CURVE QEasingCurve::InCubic
#define NUM_HEROS 10

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque, Framed };
enum LoadingScreenState { menu, arena, ranked, adventure, tavernBrawl, friendly, casual, gameMode, unknown };
enum LogComponent { logLoadingScreen, logArena, logPower, logZone, logAsset, logInvalid };
enum DraftMethod { HearthArena, LightForge, HSReplay, None };
enum ScoreSource { Score_HearthArena, Score_LightForge, Score_Heroes, Score_HSReplay };
enum MechanicBorderColor { MechanicBorderRed, MechanicBorderGrey, MechanicBorderGreen };

enum VisibleRace {V_MURLOC, V_DEMON, V_MECHANICAL, V_ELEMENTAL, V_BEAST, V_TOTEM, V_PIRATE, V_DRAGON,
                  V_MURLOC_ALL, V_DEMON_ALL, V_MECHANICAL_ALL, V_ELEMENTAL_ALL,
                  V_BEAST_ALL, V_TOTEM_ALL, V_PIRATE_ALL, V_DRAGON_ALL, V_NUM_RACES};
enum VisibleType {V_MINION, V_SPELL, V_WEAPON, V_SPELL_ALL, V_WEAPON_ALL, V_NUM_TYPES};
enum VisibleDrop {V_DROP2, V_DROP3, V_DROP4, V_NUM_DROPS};
enum VisibleMechanics {V_AOE, V_TAUNT, V_TAUNT_ALL,
                       V_DISCOVER_DRAW, V_DISCOVER, V_DRAW, V_TOYOURHAND,
                       V_PING, V_DAMAGE, V_DESTROY, V_REACH,
                       V_OVERLOAD, V_JADE_GOLEM, V_SECRET, V_SECRET_ALL, V_FREEZE_ENEMY, V_DISCARD, V_EVOLVE,
                       V_BATTLECRY, V_BATTLECRY_ALL, V_SILENCE, V_STEALTH, V_DEATHRATTLE, V_DEATHRATTLE_GOOD_ALL,
                       V_TAUNT_GIVER, V_TOKEN, V_WINDFURY_MINION, V_ATTACK_BUFF, V_ATTACK_NERF, V_HEALTH_BUFF,
                       V_RETURN, V_DIVINE_SHIELD, V_DIVINE_SHIELD_ALL, V_ENRAGED, V_SPELL_DAMAGE, V_SURVIVABILITY,
                       V_RESTORE_FRIENDLY_HEROE, V_RESTORE_FRIENDLY_MINION, V_RESTORE_TARGET_MINION,
                       V_ARMOR, V_TOKEN_CARD, V_COMBO, V_LIFESTEAL_MINION, V_LIFESTEAL_ALL, V_SPAWN_ENEMY,
                       V_EGG, V_RUSH, V_RUSH_ALL, V_ECHO, V_ECHO_ALL, V_DAMAGE_FRIENDLY_HERO, V_SPELL_BUFF,
                       V_MAGNETIC, V_MAGNETIC_ALL, V_HAND_BUFF, V_ENEMY_DRAW, V_HERO_ATTACK, V_HERO_POWER,
                       V_OTHER_CLASS, V_SILVER_HAND, V_TREANT, V_LACKEY, V_OUTCAST,
                       V_NUM_MECHANICS};
//New Synergy Step 1


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
private:
    static CardClass classString2cardClass(QString value);
    static void fadeLayout(QLayout *layout, bool in);

public:
    static QString className2classLogNumber(const QString &hero);
    static QString classEnum2classLogNumber(CardClass cardClass);
    static QString classEnum2classUName(CardClass cardClass);
    static CardClass classLogNumber2classEnum(QString hero);
    static QString classLogNumber2classUName(QString hero);
    static QString classLogNumber2classUL_ULName(QString hero);
    static QString classLogNumber2classULName(QString hero);
    static QString classLogNumber2classLName(QString hero);
    static QString classOrder2classColor(int order);
    static QString classOrder2classULName(int order);
    static QString classOrder2classUL_ULName(int order);
    static QString classOrder2classLogNumber(int order);
    static bool cardsJsonContains(QString code, QString attribute);
    static QJsonValue getCardAttribute(QString code, QString attribute);
    static QString appPath();
    static QString dataPath();
    static QString gameslogPath();
    static QString hscardsPath();
    static QString extraPath();
    static QString cardEnNameFromCode(QString code);
    static QString cardEnCodeFromName(QString name, bool onlyCollectible=true);
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
    static CardType getTypeFromCode(QString code);
    static CardRarity getRarityFromCode(QString code);
    static QList<CardClass> getClassFromCode(QString code);
    static CardRace getRaceFromCode(QString code);
    static bool isFromStandardSet(QString code);
    static bool isASecret(QString code);
    static void dumpOnFile(const QByteArray &data, QString path);
    static QString getCodeFromCardAttribute(QString attribute, QJsonValue value);
    static void drawShadowText(QPainter &painter, const QFont &font, const QString &text, int x, int y, bool alignCenter, bool isCardText=true);
    static QString themesPath();
    static int classLogNumber2classOrder(QString heroLog);
    static QString cardEnTextFromCode(QString code);
    static void clearLayout(QLayout *layout, bool deleteWidgets, bool recursive);
    static void showItemsLayout(QLayout *layout);
    static QStringList getSetCodes(const QString &set, bool excludeHeroes, bool onlyCollectible);
    static QStringList getWildCodes();
    static QStringList getStandardCodes();
    static float normalizeLF(float score, bool doit);
    static bool createDir(QString pathDir);
    static void unZip(QString zipName, QString targetPath);
    static void fixLightforgeTierlist();
    static void checkTierlistsCount();
    static bool hasGoldenImage(QString code);
    static void resizeGoldenCards();
    static bool checkHeroPortrait(QString code);
    static void checkMissingGoldenCards();
    static QStringList getArenaSets(const QStringList &codeList);
    static void fadeOutLayout(QLayout *layout);
    static void fadeInLayout(QLayout *layout);
};

#endif // UTILITY_H
