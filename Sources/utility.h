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
#define REMOVE_EXTRA_AND_HISTOGRAMS_ON_VERSION_UPDATE true

#define DEBUG_REPLAY_AUTO_ADVANCE false
#define DEBUG_OVERLAYS_LEFT false


using namespace cv;

#define FLOATEQ(X, Y)  (fabs(X - Y) < 0.000001f)
#define ANIMATION_TIME 750
#define SHOW_EASING_CURVE QEasingCurve::OutCubic
#define HIDE_EASING_CURVE QEasingCurve::InCubic

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque, Framed };
enum LoadingScreenState { menu, arena, ranked, adventure, tavernBrawl, friendly, gameMode, unknown };
enum LogComponent { logLoadingScreen, logArena, logPower, logZone, logAsset, logInvalid };
enum DraftMethod { HearthArena, LightForge, HSReplay, None };
enum ScoreSource { Score_HearthArena, Score_LightForge, Score_Heroes, Score_HSReplay, Score_Heroes_Player, Score_None };
enum MechanicBorderColor { MechanicBorderRed, MechanicBorderGrey, MechanicBorderGreen };

//New race step
enum VisibleRace {V_MURLOC, V_DEMON, V_MECHANICAL, V_ELEMENTAL, V_BEAST, V_TOTEM, V_PIRATE, V_DRAGON, V_NAGA, V_UNDEAD, V_QUILBOAR, V_DRAENEI,
                  V_MURLOC_ALL, V_DEMON_ALL, V_MECHANICAL_ALL, V_ELEMENTAL_ALL, V_BEAST_ALL, V_TOTEM_ALL,
                  V_PIRATE_ALL, V_DRAGON_ALL, V_NAGA_ALL, V_UNDEAD_ALL, V_QUILBOAR_ALL, V_DRAENEI_ALL, V_NUM_RACES};
enum VisibleSchool {V_ARCANE, V_FEL, V_FIRE, V_FROST, V_HOLY, V_SHADOW, V_NATURE,
                    V_ARCANE_ALL, V_FEL_ALL, V_FIRE_ALL, V_FROST_ALL, V_HOLY_ALL,
                    V_SHADOW_ALL, V_NATURE_ALL, V_NUM_SCHOOLS};
enum VisibleType {V_MINION, V_SPELL, V_WEAPON, V_SPELL_ALL, V_WEAPON_ALL, V_LOCATION, V_LOCATION_ALL, V_NUM_TYPES};
enum VisibleDrop {V_DROP2, V_DROP3, V_DROP4, V_NUM_DROPS};
enum VisibleMechanics {V_AOE, V_TAUNT, V_TAUNT_ALL,
                       V_DISCOVER_DRAW, V_DISCOVER, V_DRAW, V_TOYOURHAND,
                       V_PING, V_DAMAGE, V_DESTROY, V_REACH,
                       V_OVERLOAD, V_OVERLOAD_ALL, V_JADE_GOLEM, V_SECRET, V_SECRET_ALL, V_FREEZE_ENEMY, V_DISCARD, V_EVOLVE,
                       V_BATTLECRY, V_BATTLECRY_ALL, V_SILENCE, V_STEALTH, V_DEATHRATTLE, V_DEATHRATTLE_GOOD_ALL,
                       V_TAUNT_GIVER, V_TOKEN, V_WINDFURY_MINION, V_ATTACK_BUFF, V_ATTACK_NERF, V_HEALTH_BUFF,
                       V_RETURN, V_DIVINE_SHIELD, V_DIVINE_SHIELD_ALL, V_ENRAGED, V_SPELL_DAMAGE, V_SURVIVABILITY,
                       V_RESTORE_FRIENDLY_HEROE, V_RESTORE_FRIENDLY_MINION, V_RESTORE_TARGET_MINION, V_ARMOR,
                       V_TOKEN_CARD, V_COMBO, V_COMBO_ALL, V_LIFESTEAL_MINION, V_LIFESTEAL_ALL, V_SPAWN_ENEMY,
                       V_EGG, V_RUSH, V_RUSH_ALL, V_ECHO, V_ECHO_ALL, V_DAMAGE_FRIENDLY_HERO, V_SPELL_BUFF,
                       V_MAGNETIC, V_MAGNETIC_ALL, V_HAND_BUFF, V_ENEMY_DRAW, V_HERO_ATTACK, V_HERO_POWER,
                       V_OTHER_CLASS, V_SILVER_HAND, V_TREANT, V_LACKEY, V_OUTCAST, V_OUTCAST_ALL, V_END_TURN,
                       V_RUSH_GIVER, V_DREDGE, V_CORPSE, V_CHOOSEONE, V_CHOOSEONE_ALL, V_EXCAVATE, V_LIBRAM, V_LIBRAM_ALL,
                       V_STARSHIP, V_STARSHIP_ALL, V_SHUFFLE,
                       V_NUM_MECHANICS};
enum MechanicIcons {M_DROP2, M_DROP3, M_DROP4,
                    M_REACH, M_TAUNT_ALL, M_SURVIVABILITY, M_DISCOVER_DRAW,
                    M_PING, M_DAMAGE, M_DESTROY, M_AOE, M_NUM_MECHANICS};
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
    static CardClass classString2cardClass(const QString &value);
    static CardRace raceString2cardRace(const QString &value);
    static void fadeLayout(QLayout *layout, bool in);

public:
    static QString className2classLogNumber(const QString &hero);
    static QString classEnum2classLogNumber(CardClass cardClass);
    static QString classEnum2classUName(CardClass cardClass);
    static CardClass classLogNumber2classEnum(const QString &hero);
    static QString classLogNumber2classUName(const QString &hero);
    static QString classLogNumber2classUL_ULName(const QString &hero);
    static QString classLogNumber2classULName(const QString &hero);
    static QString classLogNumber2classLName(const QString &hero);
    static QString classOrder2classColor(int order);
    static QString classOrder2classULName(int order);
    static QString classOrder2classUL_ULName(int order);
    static QString classOrder2classLogNumber(int order);
    static bool cardsJsonContains(const QString &code, const QString &attribute);
    static QJsonValue getCardAttribute(const QString &code, const QString &attribute);
    static QString appPath();
    static QString dataPath();
    static QString hscardsPath();
    static QString extraPath();
    static QString cardEnNameFromCode(const QString &code);
    static QString cardLocalNameFromCode(const QString &code);
    static QStringList cardEnCodesFromName(const QString &name, bool onlyCollectible=true);
    static QString cardLocalCodeFromName(const QString &name);
    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
    static void setLocalLang(const QString &localLang);
    static QString removeAccents(const QString &s);
    static QPropertyAnimation *fadeInWidget(QWidget *widget, bool force=false);
    static QPropertyAnimation *fadeOutWidget(QWidget *widget, bool force=false);
    static QString getLoadingScreenToString(LoadingScreenState loadingScreen);
    static LoadingScreenState getLoadingScreenFromString(const QString &loadingScreenString);
    static std::vector<Point2f> findTemplateOnScreen(const QString &templateImage, QScreen *screen, const std::vector<Point2f> &templatePoints, QPointF &screenScale, int &screenHeight);
    static ulong findTemplateOnMat(const QString &templateImage, cv::Mat &mat, bool showMatches=false);
    static ulong findTemplateOnMat(const QString &templateImage, Mat &mat, const std::vector<Point2f> &templatePoints,
                                   std::vector<Point2f> &targetPoints, ulong minGoodMatches, bool showMatches=false);
    static QPixmap getTransformedImage(QPixmap image, QPointF pos, QPointF anchor, qreal rot, QPointF &origin);
    static bool isLeftOfScreen(QPoint center);
    static CardType getTypeFromCode(const QString &code);
    static CardRarity getRarityFromCode(const QString &code);
    static QList<CardClass> getClassFromCode(const QString &code);
    static QList<CardRace> getRaceFromCode(const QString &code);
    static CardSchool getSchoolFromCode(const QString &code);
    static bool isFromStandardSet(const QString &code);
    static bool isASecret(const QString &code);
    static bool isAHero(const QString &code);
    static void dumpOnFile(const QByteArray &data, const QString &path);
    static QString getCodeFromCardAttribute(const QString &attribute, QJsonValue value);
    static void drawShadowText(QPainter &painter, const QFont &font, const QString &text, int x, int y, bool alignCenter, bool isCardText=true);
    static void drawTagText(QPainter &painter, const QFont &font, const QString &text, int x, int y, int xBorder, int yBorder, float scale=1, bool alignCenter=false);
    static QString themesPath();
    static QString histogramsPath();
    static QString arenaStatsPath();
    static int classLogNumber2classOrder(const QString &heroLog);
    static QString cardEnTextFromCode(const QString &code);
    static void clearLayout(QLayout *layout, bool deleteWidgets, bool recursive);
    static void showItemsLayout(QLayout *layout);
    static QStringList getSetCodes(const QString &set, bool excludeHeroes, bool onlyCollectible);
    static QStringList getSetIntCodes(const int &set, bool excludeHeroes, bool onlyCollectible);
    static QStringList getWildCodes();
    static QStringList getStandardCodes();
    static bool createDir(const QString &pathDir);
    static void unZip(const QString &zipName, const QString &targetPath);
    static void checkTierlistsCount(QStringList &arenaSets);
    static void resizeSignatureCards();
    static bool checkHeroPortrait(const QString &code);
    static void checkMissingGoldenCards();
    static QStringList getArenaSets(const QStringList &codeList);
    static void fadeOutLayout(QLayout *layout);
    static void fadeInLayout(QLayout *layout);
    static void timeStamp(const QString &tag);
    static int className2classOrder(const QString &className);
    static QStringList getThemeList();
    static bool needCodesSpecific(const QString &set);
    static QStringList getSetCodesSpecific(const QString &set);
    static bool codeEqConstant(const QString &code, const QString &codeConstant);
    static QString otherCodeConstant(const QString &code);
    static void mergeHSRwithFireCards();
    static void shrinkText(QFont &font, const QString &text, int startFontSize, int maxLong);
};

#endif // UTILITY_H
