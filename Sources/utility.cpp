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


//--------------------------------------------------------
//----Conversiones de clases
//----NEW HERO CLASS
//--------------------------------------------------------
QString Utility::className2classLogNumber(const QString &hero)
{
    const QString heroL = hero.toLower();
    if(heroL.compare("druid")==0)           return QString("06");
    else if(heroL.compare("hunter")==0)     return QString("05");
    else if(heroL.compare("mage")==0)       return QString("08");
    else if(heroL.compare("paladin")==0)    return QString("04");
    else if(heroL.compare("priest")==0)     return QString("09");
    else if(heroL.compare("rogue")==0)      return QString("03");
    else if(heroL.compare("shaman")==0)     return QString("02");
    else if(heroL.compare("warlock")==0)    return QString("07");
    else if(heroL.compare("warrior")==0)    return QString("01");
    else if(heroL.compare("demonhunter")==0) return QString("10");
    else if(heroL.compare("deathknight")==0) return QString("11");
    else return QString();
}


QString Utility::classEnum2classLogNumber(CardClass cardClass)
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
    else if(cardClass == DEMONHUNTER) return QString("10");
    else if(cardClass == DEATHKNIGHT) return QString("11");
    else return QString();
}


QString Utility::classEnum2classUName(CardClass cardClass)
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
    else if(cardClass == DEMONHUNTER) return QString("DEMONHUNTER");
    else if(cardClass == DEATHKNIGHT) return QString("DEATHKNIGHT");
    else if(cardClass == NEUTRAL) return QString("NEUTRAL");
    else return QString();
}


CardClass Utility::classLogNumber2classEnum(const QString &hero)
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
    else if(hero == QString("10"))  return DEMONHUNTER;
    else if(hero == QString("11"))  return DEATHKNIGHT;
    else                            return INVALID_CLASS;
}


QString Utility::classLogNumber2classUL_ULName(const QString &hero)
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
    else if(hero == QString("10"))  return "Demon Hunter";
    else if(hero == QString("11"))  return "Death Knight";
    else                            return "";
}


QString Utility::classLogNumber2classULName(const QString &hero)
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
    else if(hero == QString("10"))  return "Demonhunter";
    else if(hero == QString("11"))  return "Deathknight";
    else                            return "";
}


QString Utility::classLogNumber2classUName(const QString &hero)
{
    return Utility::classLogNumber2classULName(hero).toUpper();
}


QString Utility::classLogNumber2classLName(const QString &hero)
{
    return Utility::classLogNumber2classULName(hero).toLower();
}


//--------------------------------------------------------
//----Return info about heroes in alphabetical order
//----NEW HERO CLASS
//--------------------------------------------------------
QString Utility::classOrder2classColor(int order)
{
    QString heroesColors[NUM_HEROS+1] = {"#2c566a", "#1e5b3d", "#7f4f29", "#347c2c", "#699cd5", "#de9a3e", "#a4a4a4", "#4c4e54", "#0070DE", "#6c4177", "#912a21", "#A9A9A9"};
    if(order < 0 || order > NUM_HEROS)    return "";
    return heroesColors[order];
}


QString Utility::classOrder2classULName(int order)
{
    QString heroes[NUM_HEROS] = {"Deathknight", "Demonhunter", "Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    if(order < 0 || order > (NUM_HEROS-1))  return "";
    return heroes[order];
}


QString Utility::classOrder2classUL_ULName(int order)
{
    QString heroes[NUM_HEROS] = {"Death Knight", "Demon Hunter", "Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    if(order < 0 || order > (NUM_HEROS-1))  return "";
    return heroes[order];
}


QString Utility::classOrder2classLogNumber(int order)
{
    QString heroesLogNumber[NUM_HEROS] = {"11", "10", "06", "05", "08", "04", "09", "03", "02", "07", "01"};
    if(order < 0 || order > (NUM_HEROS-1))  return "";
    return heroesLogNumber[order];
}


int Utility::classLogNumber2classOrder(const QString &heroLog)
{
    int heroeOrder[NUM_HEROS] = {10, 8, 7, 5, 3, 2, 9, 4, 6, 1, 0};
    int heroLogInt = heroLog.toInt() - 1;
    if(heroLogInt < 0 || heroLogInt > (NUM_HEROS-1))    return -1;
    return heroeOrder[heroLogInt];
}


int Utility::className2classOrder(const QString &className)
{
    QString logNumber = Utility::className2classLogNumber(className);
    return classLogNumber2classOrder(logNumber);
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
        default:
            return "UNKNOWN";
    }
}


LoadingScreenState Utility::getLoadingScreenFromString(const QString &loadingScreenString)
{
    if(loadingScreenString == "ARENA")          return arena;
    else if(loadingScreenString == "RANKED")    return ranked;
    else if(loadingScreenString == "SOLO")      return adventure;
    else if(loadingScreenString == "BRAWL")     return tavernBrawl;
    else if(loadingScreenString == "FRIENDLY")  return friendly;
    else                                        return unknown;
}


QString Utility::cardEnNameFromCode(const QString &code)
{
    return (*cardsJson)[code].value("name").toObject().value("enUS").toString();
}


QString Utility::cardLocalNameFromCode(const QString &code)
{
    return (*cardsJson)[code].value("name").toObject().value(localLang).toString();
}


QString Utility::cardEnTextFromCode(const QString &code)
{
    QString text = (*cardsJson)[code].value("text").toObject().value("enUS").toString();
    text.replace('\n',' ');
    text.replace(QChar(160), ' ');
    return text;
}


QStringList Utility::cardEnCodesFromName(const QString &name, bool onlyCollectible)
{
    QStringList codes;
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toObject().value("enUS").toString() == name)
        {
            if(!onlyCollectible || ((it->value("collectible").toBool()) && (!it.key().startsWith("HERO_"))))
            {
                codes << it.key();
            }
        }
    }

    return codes;
}


QString Utility::cardLocalCodeFromName(const QString &name)
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


bool Utility::cardsJsonContains(const QString &code, const QString &attribute)
{
    return (*cardsJson)[code].contains(attribute);
}


QJsonValue Utility::getCardAttribute(const QString &code, const QString &attribute)
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


QString Utility::getCodeFromCardAttribute(const QString &attribute, QJsonValue value)
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


CardType Utility::getTypeFromCode(const QString &code)
{
    QString value = Utility::getCardAttribute(code, "type").toString();
    if(value == "MINION")           return MINION;
    else if(value == "SPELL")       return SPELL;
    else if(value == "WEAPON")      return WEAPON;
    else if(value == "HERO")        return HERO;
    else if(value == "HERO_POWER")  return HERO_POWER;
    else if(value == "LOCATION")    return LOCATION;
    else                            return INVALID_TYPE;
}


CardRarity Utility::getRarityFromCode(const QString &code)
{
    QString value = Utility::getCardAttribute(code, "rarity").toString();
    if(value == "FREE")             return FREE;
    else if(value == "COMMON")      return COMMON;
    else if(value == "RARE")        return RARE;
    else if(value == "EPIC")        return EPIC;
    else if(value == "LEGENDARY")   return LEGENDARY;
    else                            return INVALID_RARITY;
}


QList<CardRace> Utility::getRaceFromCode(const QString &code)
{
    QJsonValue jsonVraces = Utility::getCardAttribute(code, "races");
    if(jsonVraces.isUndefined() || !jsonVraces.isArray())
    {
        QString stringCardRace = Utility::getCardAttribute(code, "race").toString();
        return {raceString2cardRace(stringCardRace)};
    }
    else
    {
        QList<CardRace> cardRaceList;
        for(const QJsonValue &jsonVrace: (const QJsonArray)jsonVraces.toArray())
        {
            cardRaceList << raceString2cardRace(jsonVrace.toString());
        }
        return cardRaceList;
    }
}


CardRace Utility::raceString2cardRace(const QString &value)
{
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
    else if(value == "MECH")        return MECHANICAL;
    else if(value == "ELEMENTAL")   return ELEMENTAL;
    else if(value == "OGRE")        return OGRE;
    else if(value == "BEAST")       return BEAST;
    else if(value == "TOTEM")       return TOTEM;
    else if(value == "NERUBIAN")    return NERUBIAN;
    else if(value == "PIRATE")      return PIRATE;
    else if(value == "DRAGON")      return DRAGON;
    else if(value == "BLANK")       return BLANK;
    else if(value == "ALL")         return ALL;
    else if(value == "EGG")         return EGG;
    else if(value == "QUILBOAR")    return QUILBOAR;
    else if(value == "CENTAUR")     return CENTAUR;
    else if(value == "FURBOLG")     return FURBOLG;
    else if(value == "HIGHELF")     return HIGHELF;
    else if(value == "TREANT")      return TREANT;
    else if(value == "OWLKIN")      return OWLKIN;
    else if(value == "HALFORC")     return HALFORC;
    else if(value == "LOCK")        return LOCK;
    else if(value == "NAGA")        return NAGA;
    else if(value == "OLDGOD")      return OLDGOD;
    else if(value == "PANDAREN")    return PANDAREN;
    else if(value == "GRONN")       return GRONN;
    else                            return INVALID_RACE;
}


CardSchool Utility::getSchoolFromCode(const QString &code)
{
    QString value = Utility::getCardAttribute(code, "spellSchool").toString();
    if(value == "NONE")                 return NONE;
    else if(value == "ARCANE")          return ARCANE;
    else if(value == "FEL")             return FEL;
    else if(value == "FIRE")            return FIRE;
    else if(value == "FROST")           return FROST;
    else if(value == "HOLY")            return HOLY;
    else if(value == "SHADOW")          return SHADOW;
    else if(value == "NATURE")          return NATURE;
    else if(value == "PHYSICAL_COMBAT") return PHYSICAL_COMBAT;
    else                                return INVALID_SCHOOL;
}


//--------------------------------------------------------
//----NEW HERO CLASS
//--------------------------------------------------------
QList<CardClass> Utility::getClassFromCode(const QString &code)
{
    QJsonValue jsonVclasses = Utility::getCardAttribute(code, "classes");
    if(jsonVclasses.isUndefined() || !jsonVclasses.isArray())
    {
        QString stringCardClass = Utility::getCardAttribute(code, "cardClass").toString();
        return {classString2cardClass(stringCardClass)};
    }
    else
    {
        QList<CardClass> cardClassList;
        for(const QJsonValue &jsonVclass: (const QJsonArray)jsonVclasses.toArray())
        {
            cardClassList << classString2cardClass(jsonVclass.toString());
        }
        return cardClassList;
    }
}


CardClass Utility::classString2cardClass(const QString &value)
{
    if(value == "")             return NEUTRAL;
    else if(value == "NEUTRAL") return NEUTRAL;
    else if(value == "DEATHKNIGHT") return DEATHKNIGHT;
    else if(value == "DEMONHUNTER") return DEMONHUNTER;
    else if(value == "DRUID")   return DRUID;
    else if(value == "HUNTER")  return HUNTER;
    else if(value == "MAGE")    return MAGE;
    else if(value == "PALADIN") return PALADIN;
    else if(value == "PRIEST")  return PRIEST;
    else if(value == "ROGUE")   return ROGUE;
    else if(value == "SHAMAN")  return SHAMAN;
    else if(value == "WARLOCK") return WARLOCK;
    else if(value == "WARRIOR") return WARRIOR;
    else                        return INVALID_CLASS;
}


/*
 * Core "CORE"
 * Legacy "LEGACY" // Old Basic
 * Expert "EXPERT1"// Old Classic
 * Demon Hunter Initiate "DEMON_HUNTER_INITIATE"
 * Taverns of Time "TAVERNS_OF_TIME"
 * Caverns of Time "WONDERS"
 * Duel Treasures "TREASURES"
 * Curse of Naxxramas "NAXX"
 * Goblins vs Gnomes "GVG"
 * Blackrock Mountain "BRM"
 * The Grand Tournament "TGT"
 * League of Explorer "LOE"
 * Whispers of the Old Gods "OG"
 * One Night in Karazhan "KARA"
 * Mean Streets of Gadgetzan "GANGS"
 * Journey to Un'Goro "UNGORO"
 * Knights of the Frozen Throne "ICECROWN"
 * Kobolds and Catacombs "LOOTAPALOOZA"
 * The Witchwood "GILNEAS"
 * The boomsday Project "BOOMSDAY"
 * Rastakhan's Rumble "TROLL"
 * Rise of Shadows "DALARAN"
 * Saviors of Uldum "ULDUM"
 * Descent of Dragons "DRAGONS"
 * Galakrond's Awakening "YEAR_OF_THE_DRAGON"
 * Ashes of Outland "BLACK_TEMPLE"
 * Scholomance Academy "SCHOLOMANCE"
 * Madness at the Darkmoon Faire "DARKMOON_FAIRE"
 * Forged in the Barrens "THE_BARRENS"
 * United in Stormwind "STORMWIND"
 * Fractured in Alterac Valley "ALTERAC_VALLEY"
 * Voyage to the Sunken City "THE_SUNKEN_CITY"
 * Murder at Castle Nathria "REVENDRETH"
 * Path of Arthas "PATH_OF_ARTHAS"
 * March of the Lick King "RETURN_OF_THE_LICH_KING"
 * Festival of Legends "BATTLE_OF_THE_BANDS"
 * TITANS "TITANS"
 * Showdown in the Badlands "WILD_WEST"
 * 10-year anniversary celebration "EVENT"
 * Whizbang’s Workshop "WHIZBANGS_WORKSHOP"
 */


bool Utility::isFromStandardSet(const QString &code)
{
    QString cardSet = getCardAttribute(code, "set").toString();

    if( cardSet == "CORE" ||
//        cardSet == "LEGACY" || cardSet == "VANILLA" || cardSet == "DEMON_HUNTER_INITIATE" ||
//        cardSet == "BASIC" || cardSet == "EXPERT1" || cardSet == "HOF" || //<-- LEGACY SET
//        cardSet == "NAXX" || cardSet == "GVG" ||
//        cardSet == "BRM" || cardSet == "TGT" || cardSet == "LOE" ||
//        cardSet == "OG" || cardSet == "KARA" || cardSet == "GANGS" ||
//        cardSet == "UNGORO" || cardSet == "ICECROWN" || cardSet == "LOOTAPALOOZA" ||
//        cardSet == "GILNEAS" || cardSet == "BOOMSDAY" || cardSet == "TROLL" ||
//        cardSet == "DALARAN" || cardSet == "ULDUM" || cardSet == "DRAGONS" || cardSet == "YEAR_OF_THE_DRAGON" ||
//        cardSet == "BLACK_TEMPLE" || cardSet == "SCHOLOMANCE" || cardSet == "DARKMOON_FAIRE" ||
//        cardSet == "THE_BARRENS" || cardSet == "STORMWIND" || cardSet == "ALTERAC_VALLEY" ||
//        cardSet == "THE_SUNKEN_CITY" || cardSet == "REVENDRETH" || cardSet == "PATH_OF_ARTHAS" || cardSet == "RETURN_OF_THE_LICH_KING" ||
        cardSet == "BATTLE_OF_THE_BANDS" || cardSet == "TITANS" || cardSet == "WILD_WEST" ||
        cardSet == "WHIZBANGS_WORKSHOP")
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool Utility::isASecret(const QString &code)
{
    const QJsonArray mechanics = getCardAttribute(code, "mechanics").toArray();

    for(const QJsonValue &mechanic: mechanics)
    {
        if(mechanic.toString() == "SECRET") return true;
    }
    return false;
}


bool Utility::isAHero(const QString &code)
{
    return (Utility::getTypeFromCode(code) == HERO);
}


QStringList Utility::getSetCodes(const QString &set, bool excludeHeroes, bool onlyCollectible)
{
    QStringList setCodes;
    const QList<QString> codeList = Utility::cardsJson->keys();
    for(const QString &code: codeList)
    {
        if(getCardAttribute(code, "set").toString() == set)
        {
            if  (
                (!onlyCollectible || getCardAttribute(code, "collectible").toBool()) &&
                (!excludeHeroes || !(code.startsWith("HERO_0") || code.startsWith("HERO_1")))
                )
            {
                setCodes.append(code);
            }
        }
    }
    return setCodes;
}


QStringList Utility::getSetIntCodes(const int &set, bool excludeHeroes, bool onlyCollectible)
{
    QStringList setCodes;
    const QList<QString> codeList = Utility::cardsJson->keys();
    for(const QString &code: codeList)
    {
        if(getCardAttribute(code, "set").toInt() == set)
        {
            if  (
                (!onlyCollectible || getCardAttribute(code, "collectible").toBool()) &&
                (!excludeHeroes || !(code.startsWith("HERO_0") || code.startsWith("HERO_1")))
                )
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
    const QList<QString> codeList = Utility::cardsJson->keys();
    for(const QString &code: codeList)
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
    const QList<QString> codeList = Utility::cardsJson->keys();
    for(const QString &code: codeList)
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


QString Utility::histogramsPath()
{
    return dataPath() + "/Histograms";
}


QString Utility::arenaStatsPath()
{
    return dataPath() + "/Arena Stats";
}


void Utility::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    Utility::cardsJson = cardsJson;
}


void Utility::setLocalLang(const QString &localLang)
{
    Utility::localLang = localLang;
}


QString Utility::removeAccents(const QString &s)
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


std::vector<Point2f> Utility::findTemplateOnScreen(const QString &templateImage, QScreen *screen,
                                                   const std::vector<Point2f> &templatePoints,
                                                   QPointF &screenScale, int &screenHeight)
{
    QRect rect = screen->geometry();
    QImage image = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height()).toImage();

    //Bug Fix: When using a resolution scale in you OS, draft scores will be postioned outside the screen. Now it's fixed.
    //Screen scale
    screenScale.setX(rect.width() / static_cast<qreal>(image.width()));
    screenScale.setY(rect.height() / static_cast<qreal>(image.height()));
    screenHeight = image.height();

    cv::Mat mat(image.height(),image.width(),CV_8UC4,image.bits(), static_cast<size_t>(image.bytesPerLine()));

    std::vector<Point2f> screenPoints;
    findTemplateOnMat(templateImage, mat, templatePoints, screenPoints, 10);
    return screenPoints;
}


ulong Utility::findTemplateOnMat(const QString &templateImage, cv::Mat &mat, bool showMatches)
{
    std::vector<Point2f> templatePoints, targetPoints;
    return findTemplateOnMat(templateImage, mat, templatePoints, targetPoints, 5, showMatches);
}


ulong Utility::findTemplateOnMat(const QString &templateImage, cv::Mat &mat, const std::vector<Point2f> &templatePoints,
                                std::vector<Point2f> &targetPoints, ulong minGoodMatches, bool showMatches)
{
    cv::Mat screenCapture = mat.clone();

    Mat img_object = imread((Utility::extraPath() + "/" + templateImage).toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    if(!img_object.data)
    {
        qDebug() << "Utility: Cannot find" << templateImage;
        return 0;
    }
    Mat img_scene;
    cv::cvtColor(screenCapture, img_scene, CV_BGR2GRAY);

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( img_object, keypoints_object );
    detector.detect( img_scene, keypoints_scene );
    if(keypoints_scene.empty())
    {
        qDebug() << "Utility: Bad screen for opencv flan.";
        return 0;
    }

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
    ulong goodMatches = good_matches.size();
    if((goodMatches < minGoodMatches) || templatePoints.empty())    return goodMatches;

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
    perspectiveTransform(templatePoints, targetPoints, H);

    //Show matches
    if(showMatches)
    {
        Mat img_matches;
        drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        imshow( "Good Matches & Object detection", img_matches );
    }

    return goodMatches;
}


QPropertyAnimation * Utility::fadeInWidget(QWidget * widget, bool force)
{
    QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if(eff == nullptr)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() < 1 || force)
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


QPropertyAnimation * Utility::fadeOutWidget(QWidget * widget, bool force)
{
    QGraphicsOpacityEffect *eff = static_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if(eff == nullptr)
    {
        eff = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(eff);
        eff->setOpacity(1);
    }

    if(eff->opacity() > 0 || force)
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

    for(QScreen *screen: (const QList<QScreen *>)QGuiApplication::screens())
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


void Utility::dumpOnFile(const QByteArray &data, const QString &path)
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


void Utility::drawTagText(QPainter &painter, const QFont &font, const QString &text, int x, int y, int xBorder, int yBorder, float scale, bool alignCenter)
{
    QFontMetrics fm(font);
    int textWide = fm.width(text);

    QColor synergyTagColor = QColor(ThemeHandler::synergyTagColor());
    painter.setPen(QPen(BLACK));
    painter.setBrush(synergyTagColor.isValid()?synergyTagColor:BLACK);
    painter.drawRoundRect((x-xBorder)*scale - (alignCenter?textWide/2:0), (y-yBorder)*scale, ((xBorder*2)*scale)+textWide, ((yBorder*2)-1)*scale, 15, 80);

    painter.setPen(QPen(BLACK));
    painter.setBrush(WHITE);
    Utility::drawShadowText(painter, font, text, x*scale, y*scale, alignCenter);
}


void Utility::clearLayout(QLayout* layout, bool deleteWidgets, bool recursive)
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
            if(recursive)   clearLayout(childLayout, deleteWidgets, recursive);
        }
        if(deleteWidgets)   delete item;
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


void Utility::fadeInLayout(QLayout* layout)
{
    fadeLayout(layout, true);
}


void Utility::fadeOutLayout(QLayout* layout)
{
    fadeLayout(layout, false);
}


void Utility::fadeLayout(QLayout* layout, bool in)
{
    for(int i=0; i<layout->count(); i++)
    {
        QLayoutItem *child = layout->itemAt(i);
        QWidget *widget = child->widget();
        if(widget != nullptr)
        {
            if(in)  fadeInWidget(widget);
            else    fadeOutWidget(widget);
        }
    }
}


void Utility::shrinkText(QFont &font, const QString &text, int startFontSize, int maxLong)
{
    int fontSize = startFontSize;
    font.setPixelSize(fontSize);

    QFontMetrics fm(font);
    int textWide = fm.width(text);
    while(textWide > maxLong)
    {
        fontSize--;
        font.setPixelSize(fontSize);
        fm = QFontMetrics(font);
        textWide = fm.width(text);
    }
}


bool Utility::createDir(const QString &pathDir)
{
    QFileInfo dirInfo(pathDir);
    if(!dirInfo.exists())
    {
        QDir().mkdir(pathDir);
        qDebug() << pathDir + " - created.";
        return true;
    }
    return false;
}


void Utility::unZip(const QString &zipName, const QString &targetPath)
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


//Resize HEARTHSTONE TOP DECKS signature cards
void Utility::resizeSignatureCards()
{
    QString goldenDir = QDir::homePath() + "/Documentos/ArenaTracker/HearthstoneGoldenCards";
    QDir dir(goldenDir);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filterName;
    filterName << "*.png";
    dir.setNameFilters(filterName);

    for(const QString &file: (const QStringList)dir.entryList())
    {
        QImage webImage(goldenDir + "/" + file);
//        webImage = webImage.scaledToWidth(218, Qt::SmoothTransformation);
        webImage = webImage.scaledToHeight(266, Qt::SmoothTransformation);
        webImage = webImage.copy(-8, -26, 200, 304);

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


void Utility::checkTierlistsCount(QStringList &arenaSets)
{
    QStringList haSets;
    QStringList allHeroes;
    for(int i=0; i<NUM_HEROS; i++)   allHeroes << Utility::classOrder2classLogNumber(i);

    for(const QString &heroLog: (const QStringList)allHeroes)
    {
        const QString heroString = Utility::classLogNumber2classUL_ULName(heroLog);
        const CardClass heroClass = Utility::classLogNumber2classEnum(heroLog);

        qDebug()<<endl<<"--------------------"<<heroString<<"--------------------";
        QMap<QString, QString> arenaMap;

        //Arena Codes List
        for(const QString &set: qAsConst(arenaSets))
        {
            QStringList codeList;
            if(Utility::needCodesSpecific(set)) codeList.append(Utility::getSetCodesSpecific(set));
            else                                codeList.append(Utility::getSetCodes(set, true, true));
            for(const QString &code: (const QStringList)codeList)
            {
                QList<CardClass> cardClassList = Utility::getClassFromCode(code);
                if(cardClassList.contains(NEUTRAL) || cardClassList.contains(heroClass))
                {
                    arenaMap[code] = "";
                }
            }
        }

        //HearthArena Names List
        QFile jsonFileHA(Utility::extraPath() + "/hearthArena.json");
        jsonFileHA.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonDocument jsonDocHA = QJsonDocument::fromJson(jsonFileHA.readAll());
        jsonFileHA.close();

        const QStringList haNames = jsonDocHA.object().value(heroString).toObject().keys();

        qDebug()<<heroString<<"Arena count:"<<arenaMap.count();
        qDebug()<<heroString<<"HearthArena count:"<<haNames.count();


        //Check Missing cards
        bool missing = false;
        const QStringList arenaCodes = arenaMap.keys();
        for(const QString &code: arenaCodes)
        {
            QString name = Utility::cardEnNameFromCode(code);
            if(haNames.contains(name))
            {
                QStringList arenaNames = arenaMap.values();
                if(arenaNames.contains(name))
                {
                    qDebug()<<"Duplicated card in arenaSets:"<<code<<name;
                }
                arenaMap[code] = name;

                QString set = getCardAttribute(code, "set").toString();
                if(!haSets.contains(set))   haSets << set;
            }
            else
            {
                qDebug()<<"HearthArena missing:"<<code<<name;
                missing = true;
            }
        }
        if(!missing)    qDebug()<<"HearthArena OK!";
        missing = false;
        QStringList arenaNames = arenaMap.values();
        for(const QString &name: haNames)
        {
            if(!arenaNames.contains(name))
            {
                QStringList codes = Utility::cardEnCodesFromName(name);
                if(codes.isEmpty())  codes = Utility::cardEnCodesFromName(name, false);
                if(codes.isEmpty())  qDebug()<<"HearthArena WRONG NAME!!!"<<name;
                else
                {
                    qDebug()<<"Arena missing:"<<codes<<name;
                    missing = true;
//                    QString set = getCardAttribute(codes, "set").toString();
//                    if(!haSets.contains(set))   haSets << set;
                }
            }
        }
        if(!missing)    qDebug()<<"Arena OK!";
    }

    arenaSets.sort();
    haSets.sort();
    qDebug()<<endl<<"---------------------------------------------------------------------------"
                "SETS ---------------------------------------------------------------------------";
    qDebug()<<"Arena Sets:"<<arenaSets;
    qDebug()<<"HA    Sets:"<<haSets;
    qDebug()<<"---------------------------------------------------------------------------"
                "SETS ---------------------------------------------------------------------------"<<endl;
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


bool Utility::checkHeroPortrait(const QString &code)
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
            else if(!code.startsWith("HERO_"))
            {
                qDebug()<<"----- NO golden:" << code << "-" << Utility::cardEnNameFromCode(code);
            }
        }
    }
}


QStringList Utility::getThemeList()
{
    return {"Purple",
             "Aquamarine",
             "Blue",
             "Brown",
             "Gold",
             "Green",
             "Red",
             "Tan" };
}


void Utility::timeStamp(const QString &tag)
{
    static qint64 start;
    qint64 end = QDateTime::currentMSecsSinceEpoch();
    qDebug()<<tag<<end-start;
    start = end;
}


void Utility::mergeHSRwithFireCards()
{
    QFile hsrJsonFile("/home/triodo/Documentos/ArenaTracker/CardsJson/cards.json");
    hsrJsonFile.open(QIODevice::ReadOnly);
    QByteArray hsrData = hsrJsonFile.readAll();
    hsrJsonFile.close();

    QFile backup("/home/triodo/Documentos/ArenaTracker/CardsJson/cardsOld.json");
    backup.open(QIODevice::WriteOnly | QIODevice::Text);
    backup.write(hsrData);
    backup.close();

    QFile fireJsonFile("/home/triodo/Documentos/ArenaTracker/CardsJson/cards_enUS.gz.json");
    fireJsonFile.open(QIODevice::ReadOnly);
    QByteArray fireData = fireJsonFile.readAll();
    fireJsonFile.close();

    QMap<QString, QJsonObject> fireMap;
    const QJsonArray fireArray = QJsonDocument::fromJson(fireData).array();
    for(const QJsonValue &fireCard: fireArray)
    {
        QJsonObject fireObject = fireCard.toObject();
        fireMap[fireObject.value("id").toString()] = fireObject;
    }

    QJsonArray hsrArray = QJsonDocument::fromJson(hsrData).array();
    qDebug()<<"Before HSR cards:" << hsrArray.count();
    for(QJsonArray::iterator it = hsrArray.begin(); it != hsrArray.end(); it++)
    {
        QJsonObject hsrObject = it->toObject();
        QString id = hsrObject.value("id").toString();
        QJsonValue races = fireMap[id].value("races");
        if(!races.isUndefined())
        {
            hsrObject["races"] = races;
            it.a->replace(it.i, hsrObject);
            qDebug()<<"Replace item" << it.i << "id =" << id << "with races =" << races;
        }
        else
        {
            qDebug()<<"Item" << it.i << "id =" << id << "no races";
        }
    }
    qDebug()<<"After HSR cards:" << hsrArray.count();

    QJsonDocument jsonDoc;
    jsonDoc.setArray(hsrArray);
    hsrJsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
    hsrJsonFile.write(jsonDoc.toJson());
    hsrJsonFile.close();
}


//--------------------------------------------------------
//----Constants
//--------------------------------------------------------


bool Utility::codeEqConstant(const QString &code, const QString &codeConstant)
{
    QString coreCodeConstant = "CORE_" + codeConstant;
    if(code == codeConstant || code == coreCodeConstant)    return true;
    return false;
}


QString Utility::otherCodeConstant(const QString &code)
{
    QString otherCode;
    if(code.startsWith("CORE_"))
    {
        otherCode = code.mid(5);
    }
    else
    {
        otherCode = "CORE_" + code;
    }
    return otherCode;
}


//--------------------------------------------------------
//----Sets non collectible
//--------------------------------------------------------


bool Utility::needCodesSpecific(const QString &set)
{
    if(set == "TAVERNS_OF_TIME")    return true;
    else if(set == "TREASURES")            return true;
    return false;
}


QStringList Utility::getSetCodesSpecific(const QString &set)
{
    if(set == "TAVERNS_OF_TIME")
    {
        return
        {
        "TOT_030",
        "TOT_056",
        "TOT_067",
        "TOT_069",
        "TOT_102",
        "TOT_103",
        "TOT_105",
        "TOT_107",
        "TOT_108",
        "TOT_109",
        "TOT_110",
        "TOT_111",
        "TOT_112",
        "TOT_116",
        "TOT_117",
        "TOT_118",
        "TOT_308",
        "TOT_313",
        "TOT_316",
        "TOT_320",
        "TOT_330",
        "TOT_332",
        "TOT_334",
        "TOT_340",
        "TOT_341",
        "TOT_342",
        "TOT_343",
        "TOT_345"
        };
    }
    //Tesoros de duelos
    else if(set == "TREASURES")
    {
        return
        {
        "PVPDR_SCH_Active44", "PVPDR_SCH_Active58", "PVPDR_SCH_Active60",
        "GILA_825", "PVPDR_SCH_Active17", "PVPDR_SCH_Active08",
        "PVPDR_SCH_Active03", "PVPDR_SCH_Active38", "PVPDR_SCH_Active50",
        "PVPDR_SCH_Active57", "PVPDR_SCH_Active30", "PVPDR_SCH_Active48",
        "PVPDR_SCH_Active31", "PVPDR_SCH_Active28", "PVPDR_SCH_Active34",
        "NAX12_04", "LOOT_998k", "LOEA_01",
        "PVPDR_YOP_Active02", "NAX11_04", "PVPDR_SCH_Active35",
        "PVPDR_SCH_Active02", "PVPDR_SCH_Active39", "PVPDR_SCH_Active42",
        "PVPDR_GUEST_Diablot7", "GILA_BOSS_35t", "NAX2_05H",
        "PVPDR_YOP_Active01", "PVPDR_SCH_Active27", "PVPDR_SCH_Active29",
        "PVPDR_SCH_Active45", "PVPDR_SCH_Active52", "PVPDR_SCH_Active61",
        "PVPDR_SCH_Active47", "PVPDR_SCH_Active46", "PVPDR_SCH_Active43"
        };
    }
    return {};
}

