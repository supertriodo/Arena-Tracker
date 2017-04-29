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

//Secretos
#define AVENGE QString("FP1_020")
#define NOBLE_SACRIFICE QString("EX1_130")
#define REPENTANCE QString("EX1_379")
#define REDEMPTION QString("EX1_136")
#define SACRED_TRIAL QString("LOE_027")
#define EYE_FOR_AN_EYE QString("EX1_132")
#define GETAWAY_KODO QString("CFM_800")
#define COMPETITIVE_SPIRIT QString("AT_073")

#define FREEZING_TRAP QString("EX1_611")
#define EXPLOSIVE_TRAP QString("EX1_610")
#define BEAR_TRAP QString("AT_060")
#define SNIPE QString("EX1_609")
#define DART_TRAP QString("LOE_021")
#define CAT_TRICK QString("KAR_004")
#define MISDIRECTION QString("EX1_533")
#define HIDDEN_CACHE QString("CFM_026")
#define SNAKE_TRAP QString("EX1_554")

#define MIRROR_ENTITY QString("EX1_294")
#define DDUPLICATE QString("FP1_018")
#define ICE_BARRIER QString("EX1_289")
#define POTION_OF_POLIMORPH QString("CFM_620")
#define EFFIGY QString("AT_002")
#define VAPORIZE QString("EX1_594")
#define COUNTERSPELL QString("EX1_287")
#define MANA_BIND QString("UNG_024")
#define SPELLBENDER QString("tt_010")
#define ICE_BLOCK QString("EX1_295")

//Cartas
#define MAD_SCIENTIST "FP1_004"
#define THE_COIN "GAME_005"
#define SWIPE "CS2_012"
#define ENGRAVE "LOE_104"
#define FROTHING_BERSEKER "EX1_604"
#define ARMORSMITH "EX1_402"
#define KNIFE_JUGGLER "NEW1_019"
#define ADDLED_GRIZZLY "OG_313"
#define DARKSHIRE_COUNCILMAN "OG_113"
#define ACIDMAW "AT_063"
#define WRATHGUARD "AT_026"
#define IMP_GANG_BOSS "BRM_006"
#define AXE_FLINGER "BRM_016"
#define DRAGON_EGG "BRM_022"
#define ACOLYTE_OF_PAIN "EX1_007"
#define GURUBASHI_BERSERKER "EX1_399"
#define LIGHTWARDEN "EX1_001"
#define TRUESILVER_CHAMPION "CS2_097"
#define GOREHOWL "EX1_411"
#define LEOKK "NEW1_033"
#define LOREWALKER_CHO "EX1_100"
#define NERUBIAN_PROPHET "OG_138"
#define THE_SKELETON_KNIGHT "AT_128"
#define GLADIATORS_LONGBOW "DS1_188"
#define EXPLOSIVE_SHOT "EX1_537"
#define POWERSHOT "AT_056"
#define ARMORED_WARHORSE "AT_108"
#define SWORD_OF_JUSTICE "EX1_366"
#define GRIMSCALE_ORACLE "EX1_508"
#define SIEGE_ENGINE "GVG_086"
#define POLLUTED_HOARDER "OG_323"
#define FLAMEWAKER "BRM_002"
#define MAD_BOMBER "EX1_082"
#define MADDER_BOMBER "GVG_090"
#define SPREADING_MADNESS "OG_116"
#define ARCANE_MISSILES "EX1_277"
#define AVENGING_WRATH "EX1_384"
#define GOBLIN_BLASTMAGE "GVG_004"
#define SMUGGLERS_CRATE "CFM_334"
#define STOLEN_GOODS "CFM_752"
#define SHAKY_ZYPGUNNER "CFM_336"
#define GRIMY_GADGETEER "CFM_754"
#define DON_HANCHO "CFM_685"
#define INNERVATE "EX1_169"
#define COUNTERFEIT_COIN "CFM_630"
#define KABAL_CHEMIST "CFM_619"
#define MIMIC_POD "UNG_060"
#define THISTLE_TEA "OG_073"


using namespace cv;

#define ANIMATION_TIME 750

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque, Framed };
enum Theme { ThemeWhite, ThemeBlack };
enum LoadingScreenState { menu, arena, constructed, adventure, tavernBrawl };
enum LogComponent { logLoadingScreen, logArena, logPower, logZone, logInvalid };
enum DraftMethod { None, HearthArena, LightForge };



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
    static QString getHeroColor(int order);
    static QString getHeroName(int order);
    static QString getHeroLogNumber(int order);
    static QJsonValue getCardAtribute(QString code, QString attribute);
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
    static void fadeInWidget(QWidget *widget);
    static void fadeOutWidget(QWidget *widget);
    static QString getLoadingScreenString(LoadingScreenState loadingScreen);
    static std::vector<Point2f> findTemplateOnScreen(QString templateImage, QScreen *screen, std::vector<Point2f> templatePoints, bool showMatches=false);
    static QPixmap getTransformedImage(QPixmap image, QPointF pos, QPointF anchor, qreal rot, QPointF &origin);
    static bool isLeftOfScreen(QPoint center);
    static bool isFromStandardSet(QString code);
    static bool isASecret(QString code);
};

#endif // UTILITY_H
