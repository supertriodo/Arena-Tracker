#include "themehandler.h"
#include "constants.h"
#include "utility.h"
#include <QtWidgets>

QString ThemeHandler::bgApp_;
QString ThemeHandler::borderApp_;
QString ThemeHandler::borderTransparent_;
int ThemeHandler::borderWidth_;
QString ThemeHandler::fgColor_;
QString ThemeHandler::themeColor1_;
QString ThemeHandler::themeColor2_;
QString ThemeHandler::bgWidgets_;
QString ThemeHandler::bgTabsColor_, ThemeHandler::hoverTabsColor_, ThemeHandler::selectedTabsColor_;
QString ThemeHandler::bgTopButtonsColor_, ThemeHandler::hoverTopButtonsColor_;
QString ThemeHandler::fgMenuColor_, ThemeHandler::bgMenuColor_;
QString ThemeHandler::bgSelectedItemMenuColor_, ThemeHandler::fgSelectedItemMenuColor_;
int ThemeHandler::borderDecksWidth_;
QString ThemeHandler::bgDecks_, ThemeHandler::borderDecks_;
QString ThemeHandler::bgSelectedItemListColor_, ThemeHandler::fgSelectedItemListColor_;
QString ThemeHandler::borderTooltipColor_, ThemeHandler::bgTooltipColor_, ThemeHandler::fgTooltipColor_;
QString ThemeHandler::borderProgressBarColor_, ThemeHandler::bgProgressBarColor_, ThemeHandler::fgProgressBarColor_, ThemeHandler::chunkProgressBarColor_;
QString ThemeHandler::borderLineEditColor_, ThemeHandler::bgLineEditColor_, ThemeHandler::fgLineEditColor_;
QString ThemeHandler::bgSelectionLineEditColor_, ThemeHandler::fgSelectionLineEditColor_;
QString ThemeHandler::defaultFont_, ThemeHandler::cardsFont_, ThemeHandler::bigFont_;
int ThemeHandler::cardsFontOffsetY_;
QString ThemeHandler::gamesOnZ2HColor_;
QString ThemeHandler::tabArenaFile_, ThemeHandler::tabConfigFile_, ThemeHandler::tabDeckFile_, ThemeHandler::tabEnemyDeckFile_;
QString ThemeHandler::tabGamesFile_, ThemeHandler::tabHandFile_, ThemeHandler::tabLogFile_, ThemeHandler::tabPlanFile_;
QString ThemeHandler::buttonRemoveDeckFile_, ThemeHandler::buttonLoadDeckFile_, ThemeHandler::buttonNewDeckFile_, ThemeHandler::buttonSaveDeckFile_;
QString ThemeHandler::buttonMinFile_, ThemeHandler::buttonPlusFile_, ThemeHandler::buttonRemoveFile_;
QString ThemeHandler::buttonCloseFile_, ThemeHandler::buttonMinimizeFile_, ThemeHandler::buttonResizeFile_;
QString ThemeHandler::buttonForceDraftFile_, ThemeHandler::buttonDraftRefreshFile_;
QString ThemeHandler::buttonGamesReplayFile_, ThemeHandler::buttonGamesWebFile_;
QString ThemeHandler::buttonPlanFirstFile_, ThemeHandler::buttonPlanLastFile_, ThemeHandler::buttonPlanRefreshFile_;
QString ThemeHandler::buttonPlanPrevFile_, ThemeHandler::buttonPlanNextFile_, ThemeHandler::buttonPlanResizeFile_;
QString ThemeHandler::bgCard1Files_[10], ThemeHandler::bgCard2Files_[10], ThemeHandler::heroFiles_[9];
QString ThemeHandler::bgTotalAttackFile_;
QString ThemeHandler::branchClosedFile_, ThemeHandler::branchOpenFile_;
QString ThemeHandler::coinFile_, ThemeHandler::firstFile_;
QString ThemeHandler::loseFile_, ThemeHandler::winFile_;
QString ThemeHandler::haBestFile_, ThemeHandler::haCloseFile_, ThemeHandler::haOpenFile_;
QString ThemeHandler::lfBestFile_, ThemeHandler::lfCloseFile_, ThemeHandler::lfOpenFile_;
QString ThemeHandler::handCardFile_, ThemeHandler::handCardBYFile_, ThemeHandler::handCardBYUnknownFile_;
QString ThemeHandler::starFile_, ThemeHandler::manaLimitFile_, ThemeHandler::unknownFile_;
QString ThemeHandler::minionsCounterFile_, ThemeHandler::spellsCounterFile_, ThemeHandler::weaponsCounterFile_, ThemeHandler::manaCounterFile_;
QString ThemeHandler::aoeMechanicFile_, ThemeHandler::tauntMechanicFile_, ThemeHandler::survivalMechanicFile_, ThemeHandler::drawMechanicFile_;
QString ThemeHandler::pingMechanicFile_, ThemeHandler::damageMechanicFile_, ThemeHandler::destroyMechanicFile_, ThemeHandler::reachMechanicFile_;
QString ThemeHandler::bgDraftMechanicsFile_, ThemeHandler::bgDraftMechanicsHelpFile_, ThemeHandler::fgDraftMechanicsColor_;
bool ThemeHandler::manaLimitBehind_;
QString ThemeHandler::themeLoaded_;


QString ThemeHandler::bgApp()
{
    return bgApp_;
}


QString ThemeHandler::borderApp(bool transparent)
{
    if(transparent)     return borderTransparent_;
    else                return borderApp_;
}


int ThemeHandler::borderWidth()
{
    return borderWidth_;
}


QString ThemeHandler::fgColor()
{
    return fgColor_;
}


QString ThemeHandler::themeColor1()
{
    return themeColor1_;
}


QString ThemeHandler::themeColor2()
{
    return themeColor2_;
}


QString ThemeHandler::bgWidgets()
{
    return bgWidgets_;
}


QString ThemeHandler::bgTabsColor()
{
    return bgTabsColor_;
}

QString ThemeHandler::hoverTabsColor()
{
    return hoverTabsColor_;
}


QString ThemeHandler::selectedTabsColor()
{
    return selectedTabsColor_;
}


QString ThemeHandler::bgTopButtonsColor()
{
    return bgTopButtonsColor_;
}

QString ThemeHandler::hoverTopButtonsColor()
{
    return hoverTopButtonsColor_;
}


QString ThemeHandler::fgMenuColor()
{
    return fgMenuColor_;
}


QString ThemeHandler::bgSelectedItemMenuColor()
{
    return bgSelectedItemMenuColor_;
}


QString ThemeHandler::fgSelectedItemMenuColor()
{
    return fgSelectedItemMenuColor_;
}


QString ThemeHandler::bgMenuColor()
{
    return bgMenuColor_;
}


QString ThemeHandler::bgDecks()
{
    return bgDecks_;
}


QString ThemeHandler::borderDecks()
{
    return borderDecks_;
}


QString ThemeHandler::bgSelectedItemListColor()
{
    return bgSelectedItemListColor_;
}


QString ThemeHandler::fgSelectedItemListColor()
{
    return fgSelectedItemListColor_;
}


QString ThemeHandler::borderTooltipColor()
{
    return borderTooltipColor_;
}


QString ThemeHandler::bgTooltipColor()
{
    return bgTooltipColor_;
}


QString ThemeHandler::fgTooltipColor()
{
    return fgTooltipColor_;
}


QString ThemeHandler::borderProgressBarColor()
{
    return borderProgressBarColor_;
}


QString ThemeHandler::bgProgressBarColor()
{
    return bgProgressBarColor_;
}


QString ThemeHandler::fgProgressBarColor()
{
    return fgProgressBarColor_;
}


QString ThemeHandler::chunkProgressBarColor()
{
    return chunkProgressBarColor_;
}


QString ThemeHandler::borderLineEditColor()
{
    return borderLineEditColor_;
}


QString ThemeHandler::bgLineEditColor()
{
    return bgLineEditColor_;
}


QString ThemeHandler::fgLineEditColor()
{
    return fgLineEditColor_;
}


QString ThemeHandler::bgSelectionLineEditColor()
{
    return bgSelectionLineEditColor_;
}


QString ThemeHandler::fgSelectionLineEditColor()
{
    return fgSelectionLineEditColor_;
}


QString ThemeHandler::defaultFont()
{
    return defaultFont_;
}


QString ThemeHandler::cardsFont()
{
    return cardsFont_;
}


QString ThemeHandler::bigFont()
{
    return bigFont_;
}


int ThemeHandler::cardsFontOffsetY()
{
    return cardsFontOffsetY_;
}


QString ThemeHandler::gamesOnZ2HColor()
{
    return gamesOnZ2HColor_;
}


QString ThemeHandler::tabArenaFile()
{
    return tabArenaFile_;
}


QString ThemeHandler::tabConfigFile()
{
    return tabConfigFile_;
}


QString ThemeHandler::tabDeckFile()
{
    return tabDeckFile_;
}


QString ThemeHandler::tabEnemyDeckFile()
{
    return tabEnemyDeckFile_;
}


QString ThemeHandler::tabGamesFile()
{
    return tabGamesFile_;
}


QString ThemeHandler::tabHandFile()
{
    return tabHandFile_;
}


QString ThemeHandler::tabLogFile()
{
    return tabLogFile_;
}


QString ThemeHandler::tabPlanFile()
{
    return tabPlanFile_;
}


QString ThemeHandler::buttonRemoveDeckFile()
{
    return buttonRemoveDeckFile_;
}


QString ThemeHandler::buttonLoadDeckFile()
{
    return buttonLoadDeckFile_;
}


QString ThemeHandler::buttonNewDeckFile()
{
    return buttonNewDeckFile_;
}


QString ThemeHandler::buttonSaveDeckFile()
{
    return buttonSaveDeckFile_;
}


QString ThemeHandler::buttonMinFile()
{
    return buttonMinFile_;
}


QString ThemeHandler::buttonPlusFile()
{
    return buttonPlusFile_;
}


QString ThemeHandler::buttonRemoveFile()
{
    return buttonRemoveFile_;
}


QString ThemeHandler::buttonCloseFile()
{
    return buttonCloseFile_;
}


QString ThemeHandler::buttonMinimizeFile()
{
    return buttonMinimizeFile_;
}


QString ThemeHandler::buttonResizeFile()
{
    return buttonResizeFile_;
}


QString ThemeHandler::buttonForceDraftFile()
{
    return buttonForceDraftFile_;
}


QString ThemeHandler::buttonDraftRefreshFile()
{
    return buttonDraftRefreshFile_;
}


QString ThemeHandler::buttonGamesReplayFile()
{
    return buttonGamesReplayFile_;
}


QString ThemeHandler::buttonGamesWebFile()
{
    return buttonGamesWebFile_;
}


QString ThemeHandler::buttonPlanFirstFile()
{
    return buttonPlanFirstFile_;
}


QString ThemeHandler::buttonPlanLastFile()
{
    return buttonPlanLastFile_;
}


QString ThemeHandler::buttonPlanRefreshFile()
{
    return buttonPlanRefreshFile_;
}


QString ThemeHandler::buttonPlanPrevFile()
{
    return buttonPlanPrevFile_;
}


QString ThemeHandler::buttonPlanNextFile()
{
    return buttonPlanNextFile_;
}


QString ThemeHandler::buttonPlanResizeFile()
{
    return buttonPlanResizeFile_;
}


QString ThemeHandler::bgCard1File(int order)
{
    if(order>8) return bgCard1Files_[9];
    return bgCard1Files_[order];
}


QString ThemeHandler::bgCard2File(int order)
{
    if(order>8 || order<0)  return bgCard2Files_[9];
    return bgCard2Files_[order];
}


QString ThemeHandler::heroFile(int order)
{
    if(order>8 || order<0)  return "";
    return heroFiles_[order];
}


QString ThemeHandler::heroFile(QString heroLog)
{
    return heroFile(Utility::heroLogNumber2HeroOrder(heroLog));
}


QString ThemeHandler::bgTotalAttackFile()
{
    return bgTotalAttackFile_;
}


QString ThemeHandler::branchClosedFile()
{
    return branchClosedFile_;
}


QString ThemeHandler::branchOpenFile()
{
    return branchOpenFile_;
}


QString ThemeHandler::coinFile()
{
    return coinFile_;
}


QString ThemeHandler::firstFile()
{
    return firstFile_;
}


QString ThemeHandler::loseFile()
{
    return loseFile_;
}


QString ThemeHandler::winFile()
{
    return winFile_;
}


QString ThemeHandler::haBestFile()
{
    return haBestFile_;
}


QString ThemeHandler::haCloseFile()
{
    return haCloseFile_;
}


QString ThemeHandler::haOpenFile()
{
    return haOpenFile_;
}


QString ThemeHandler::lfBestFile()
{
    return lfBestFile_;
}


QString ThemeHandler::lfCloseFile()
{
    return lfCloseFile_;
}


QString ThemeHandler::lfOpenFile()
{
    return lfOpenFile_;
}


QString ThemeHandler::handCardFile()
{
    return handCardFile_;
}


QString ThemeHandler::handCardBYFile()
{
    return handCardBYFile_;
}


QString ThemeHandler::handCardBYUnknownFile()
{
    return handCardBYUnknownFile_;
}


QString ThemeHandler::starFile()
{
    return starFile_;
}


QString ThemeHandler::minionsCounterFile()
{
    return minionsCounterFile_;
}


QString ThemeHandler::spellsCounterFile()
{
    return spellsCounterFile_;
}


QString ThemeHandler::weaponsCounterFile()
{
    return weaponsCounterFile_;
}


QString ThemeHandler::manaCounterFile()
{
    return manaCounterFile_;
}


QString ThemeHandler::aoeMechanicFile()
{
    return aoeMechanicFile_;
}


QString ThemeHandler::tauntMechanicFile()
{
    return tauntMechanicFile_;
}


QString ThemeHandler::survivalMechanicFile()
{
    return survivalMechanicFile_;
}


QString ThemeHandler::drawMechanicFile()
{
    return drawMechanicFile_;
}


QString ThemeHandler::pingMechanicFile()
{
    return pingMechanicFile_;
}


QString ThemeHandler::damageMechanicFile()
{
    return damageMechanicFile_;
}


QString ThemeHandler::destroyMechanicFile()
{
    return destroyMechanicFile_;
}


QString ThemeHandler::reachMechanicFile()
{
    return reachMechanicFile_;
}


QString ThemeHandler::bgDraftMechanicsFile()
{
    return bgDraftMechanicsFile_;
}


QString ThemeHandler::bgDraftMechanicsHelpFile()
{
    return bgDraftMechanicsHelpFile_;
}


QString ThemeHandler::fgDraftMechanicsColor()
{
    return fgDraftMechanicsColor_;
}


QString ThemeHandler::manaLimitFile()
{
    return manaLimitFile_;
}


QString ThemeHandler::unknownFile()
{
    return unknownFile_;
}


bool ThemeHandler::manaLimitBehind()
{
    return manaLimitBehind_;
}


QString ThemeHandler::loadThemeFile(const QString &themePath, QJsonObject &jsonObject, const QString &key)
{
    QString file = jsonObject.value(key).toString("");
    if(file.isEmpty())  return "";
    else                return themePath + file;
}


QString ThemeHandler::loadThemeCF(const QString &themePath, QJsonObject &jsonObject, const QString &key)
{
    QString file = jsonObject.value(key).toString("");
    if(file.contains("."))  return themePath + file;
    else                    return file;
}


QString ThemeHandler::loadThemeFont(const QString &themePath, QJsonObject &jsonObject, const QString &key)
{
    QString file = jsonObject.value(key).toString("");
    if(file.isEmpty())  return "";
    else
    {
        QFontDatabase::addApplicationFont(themePath + file);
        return file.split("/").last().split(".").first();
    }
}


void ThemeHandler::loadThemeValues(const QString &themePath, QByteArray &jsonData)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(jsonData).object();

    //"-----FONTS-----": 0,
    defaultFont_ = loadThemeFont(themePath, jsonObject, "defaultFont");
    bigFont_ = loadThemeFont(themePath, jsonObject, "bigFont");
    cardsFont_ = loadThemeFont(themePath, jsonObject, "cardsFont");
    cardsFontOffsetY_ = jsonObject.value("cardsFontOffsetY").toInt(0);

    //"-----MAIN COLORS-----": 0,
    bgApp_ = loadThemeCF(themePath, jsonObject, "bgAppCF");
    fgColor_ = jsonObject.value("fgColor").toString("");
    borderApp_ = loadThemeCF(themePath, jsonObject, "borderAppCF");
    borderWidth_ = jsonObject.value("borderWidth").toInt(0);
    themeColor1_ = jsonObject.value("theme1Color").toString("");
    themeColor2_ = jsonObject.value("theme2Color").toString("");
    bgWidgets_ = jsonObject.value("bgWidgetsColor").toString("");

    //"-----MIN CLOSE RESIZE-----": 0,
    bgTopButtonsColor_ = jsonObject.value("bgTopButtonsColor").toString("");
    hoverTopButtonsColor_ = jsonObject.value("hoverTopButtonsColor").toString("");
    buttonMinimizeFile_ = loadThemeFile(themePath, jsonObject, "buttonMinimizeFile");
    buttonCloseFile_ = loadThemeFile(themePath, jsonObject, "buttonCloseFile");
    buttonResizeFile_ = loadThemeFile(themePath, jsonObject, "buttonResizeFile");

    //"-----TABS-----": 0,
    bgTabsColor_ = jsonObject.value("bgTabsColor").toString("");
    hoverTabsColor_ = jsonObject.value("hoverTabsColor").toString("");
    selectedTabsColor_ = jsonObject.value("selectedTabsColor").toString("");
    tabArenaFile_ = loadThemeFile(themePath, jsonObject, "tabArenaFile");
    tabGamesFile_ = loadThemeFile(themePath, jsonObject, "tabGamesFile");
    tabHandFile_ = loadThemeFile(themePath, jsonObject, "tabHandFile");
    tabDeckFile_ = loadThemeFile(themePath, jsonObject, "tabDeckFile");
    tabEnemyDeckFile_ = loadThemeFile(themePath, jsonObject, "tabEnemyDeckFile");
    tabPlanFile_ = loadThemeFile(themePath, jsonObject, "tabPlanFile");
    tabConfigFile_ = loadThemeFile(themePath, jsonObject, "tabConfigFile");
    tabLogFile_ = loadThemeFile(themePath, jsonObject, "tabLogFile");

    //"-----DECK WINDOW-----": 0,
    buttonNewDeckFile_ = loadThemeFile(themePath, jsonObject, "buttonNewDeckFile");
    buttonLoadDeckFile_ = loadThemeFile(themePath, jsonObject, "buttonLoadDeckFile");
    buttonSaveDeckFile_ = loadThemeFile(themePath, jsonObject, "buttonSaveDeckFile");
    buttonRemoveDeckFile_ = loadThemeFile(themePath, jsonObject, "buttonRemoveDeckFile");
    buttonRemoveFile_ = loadThemeFile(themePath, jsonObject, "buttonRemoveFile");
    buttonMinFile_ = loadThemeFile(themePath, jsonObject, "buttonMinFile");
    buttonPlusFile_ = loadThemeFile(themePath, jsonObject, "buttonPlusFile");

    unknownFile_ = loadThemeFile(themePath, jsonObject, "unknownFile");
    starFile_ = loadThemeFile(themePath, jsonObject, "starFile");
    manaLimitFile_ = loadThemeFile(themePath, jsonObject, "manaLimitFile");
    manaLimitBehind_ = jsonObject.value("manaLimitBehind").toBool(true);

    bgCard1Files_[9] = loadThemeFile(themePath, jsonObject, "bgCard1File");
    bgCard1Files_[0] = loadThemeFile(themePath, jsonObject, "bgCard1DruidFile");
    bgCard1Files_[1] = loadThemeFile(themePath, jsonObject, "bgCard1HunterFile");
    bgCard1Files_[2] = loadThemeFile(themePath, jsonObject, "bgCard1MageFile");
    bgCard1Files_[3] = loadThemeFile(themePath, jsonObject, "bgCard1PaladinFile");
    bgCard1Files_[4] = loadThemeFile(themePath, jsonObject, "bgCard1PriestFile");
    bgCard1Files_[5] = loadThemeFile(themePath, jsonObject, "bgCard1RogueFile");
    bgCard1Files_[6] = loadThemeFile(themePath, jsonObject, "bgCard1ShamanFile");
    bgCard1Files_[7] = loadThemeFile(themePath, jsonObject, "bgCard1WarlockFile");
    bgCard1Files_[8] = loadThemeFile(themePath, jsonObject, "bgCard1WarriorFile");

    bgCard2Files_[9] = loadThemeFile(themePath, jsonObject, "bgCard2File");
    bgCard2Files_[0] = loadThemeFile(themePath, jsonObject, "bgCard2DruidFile");
    bgCard2Files_[1] = loadThemeFile(themePath, jsonObject, "bgCard2HunterFile");
    bgCard2Files_[2] = loadThemeFile(themePath, jsonObject, "bgCard2MageFile");
    bgCard2Files_[3] = loadThemeFile(themePath, jsonObject, "bgCard2PaladinFile");
    bgCard2Files_[4] = loadThemeFile(themePath, jsonObject, "bgCard2PriestFile");
    bgCard2Files_[5] = loadThemeFile(themePath, jsonObject, "bgCard2RogueFile");
    bgCard2Files_[6] = loadThemeFile(themePath, jsonObject, "bgCard2ShamanFile");
    bgCard2Files_[7] = loadThemeFile(themePath, jsonObject, "bgCard2WarlockFile");
    bgCard2Files_[8] = loadThemeFile(themePath, jsonObject, "bgCard2WarriorFile");

    //"-----GAMES TAB-----": 0,
    gamesOnZ2HColor_ = jsonObject.value("gamesOnZ2HColor").toString("");
    buttonGamesReplayFile_ = loadThemeFile(themePath, jsonObject, "buttonGamesReplayFile");
    buttonGamesWebFile_ = loadThemeFile(themePath, jsonObject, "buttonGamesWebFile");

    bgSelectedItemListColor_ = jsonObject.value("bgSelectedItemListColor").toString("");
    fgSelectedItemListColor_ = jsonObject.value("fgSelectedItemListColor").toString("");

    branchClosedFile_ = loadThemeFile(themePath, jsonObject, "branchClosedFile");
    branchOpenFile_ = loadThemeFile(themePath, jsonObject, "branchOpenFile");

    coinFile_ = loadThemeFile(themePath, jsonObject, "coinFile");
    firstFile_ = loadThemeFile(themePath, jsonObject, "firstFile");
    loseFile_ = loadThemeFile(themePath, jsonObject, "loseFile");
    winFile_ = loadThemeFile(themePath, jsonObject, "winFile");

    heroFiles_[0] = loadThemeFile(themePath, jsonObject, "heroDruidFile");
    heroFiles_[1] = loadThemeFile(themePath, jsonObject, "heroHunterFile");
    heroFiles_[2] = loadThemeFile(themePath, jsonObject, "heroMageFile");
    heroFiles_[3] = loadThemeFile(themePath, jsonObject, "heroPaladinFile");
    heroFiles_[4] = loadThemeFile(themePath, jsonObject, "heroPriestFile");
    heroFiles_[5] = loadThemeFile(themePath, jsonObject, "heroRogueFile");
    heroFiles_[6] = loadThemeFile(themePath, jsonObject, "heroShamanFile");
    heroFiles_[7] = loadThemeFile(themePath, jsonObject, "heroWarlockFile");
    heroFiles_[8] = loadThemeFile(themePath, jsonObject, "heroWarriorFile");

    //"-----ENEMY HAND TAB-----": 0,
    handCardFile_ = loadThemeFile(themePath, jsonObject, "handCardFile");
    handCardBYFile_ = loadThemeFile(themePath, jsonObject, "handCardBYFile");
    handCardBYUnknownFile_ = loadThemeFile(themePath, jsonObject, "handCardBYUnknownFile");
    bgTotalAttackFile_ = loadThemeFile(themePath, jsonObject, "bgTotalAttackFile");

    //"-----REPLAY TAB-----": 0,
    buttonPlanResizeFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanResizeFile");
    buttonPlanFirstFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanFirstFile");
    buttonPlanPrevFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanPrevFile");
    buttonPlanNextFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanNextFile");
    buttonPlanLastFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanLastFile");
    buttonPlanRefreshFile_ = loadThemeFile(themePath, jsonObject, "buttonPlanRefreshFile");

    //"-----DRAFT-----": 0,
    buttonDraftRefreshFile_ = loadThemeFile(themePath, jsonObject, "buttonDraftRefreshFile");
    haBestFile_ = loadThemeFile(themePath, jsonObject, "haBestFile");
    haCloseFile_ = loadThemeFile(themePath, jsonObject, "haCloseFile");
    haOpenFile_ = loadThemeFile(themePath, jsonObject, "haOpenFile");
    lfBestFile_ = loadThemeFile(themePath, jsonObject, "lfBestFile");
    lfCloseFile_ = loadThemeFile(themePath, jsonObject, "lfCloseFile");
    lfOpenFile_ = loadThemeFile(themePath, jsonObject, "lfOpenFile");

    bgDraftMechanicsFile_ = loadThemeFile(themePath, jsonObject, "bgDraftMechanicsFile");
    bgDraftMechanicsHelpFile_ = loadThemeFile(themePath, jsonObject, "bgDraftMechanicsHelpFile");
    fgDraftMechanicsColor_ = jsonObject.value("fgDraftMechanicsColor").toString("");
    minionsCounterFile_ = loadThemeFile(themePath, jsonObject, "minionsCounterFile");
    spellsCounterFile_ = loadThemeFile(themePath, jsonObject, "spellsCounterFile");
    weaponsCounterFile_ = loadThemeFile(themePath, jsonObject, "weaponsCounterFile");
    manaCounterFile_ = loadThemeFile(themePath, jsonObject, "manaCounterFile");
    aoeMechanicFile_ = loadThemeFile(themePath, jsonObject, "aoeMechanicFile");
    tauntMechanicFile_ = loadThemeFile(themePath, jsonObject, "tauntMechanicFile");
    survivalMechanicFile_ = loadThemeFile(themePath, jsonObject, "survivalMechanicFile");
    drawMechanicFile_ = loadThemeFile(themePath, jsonObject, "drawMechanicFile");
    pingMechanicFile_ = loadThemeFile(themePath, jsonObject, "pingMechanicFile");
    damageMechanicFile_ = loadThemeFile(themePath, jsonObject, "damageMechanicFile");
    destroyMechanicFile_ = loadThemeFile(themePath, jsonObject, "destroyMechanicFile");
    reachMechanicFile_ = loadThemeFile(themePath, jsonObject, "reachMechanicFile");

    //"-----MENU LISTS-----": 0,
    buttonForceDraftFile_ = loadThemeFile(themePath, jsonObject, "buttonForceDraftFile");
    bgMenuColor_ = jsonObject.value("bgMenuColor").toString("");
    fgMenuColor_ = jsonObject.value("fgMenuColor").toString("");
    bgSelectedItemMenuColor_ = jsonObject.value("bgSelectedItemMenuColor").toString("");
    fgSelectedItemMenuColor_ = jsonObject.value("fgSelectedItemMenuColor").toString("");

    //"-----TOOLTIPS-----": 0,
    bgTooltipColor_ = jsonObject.value("bgTooltipColor").toString("");
    fgTooltipColor_ = jsonObject.value("fgTooltipColor").toString("");
    borderTooltipColor_ = jsonObject.value("borderTooltipColor").toString("");

    //"-----PROGRESS BAR-----": 0,
    bgProgressBarColor_ = jsonObject.value("bgProgressBarColor").toString("");
    fgProgressBarColor_ = jsonObject.value("fgProgressBarColor").toString("");
    borderProgressBarColor_ = jsonObject.value("borderProgressBarColor").toString("");
    chunkProgressBarColor_ = jsonObject.value("chunkProgressBarColor").toString("");

    //"-----LOAD DECKS-----": 0,
    bgDecks_ = loadThemeCF(themePath, jsonObject, "bgDecksCF");
    borderDecks_ = loadThemeCF(themePath, jsonObject, "borderDecksCF");
    borderDecksWidth_ = jsonObject.value("borderDecksWidth").toInt(0);

    //"-----DECK NAME LINE EDIT-----": 0,
    bgLineEditColor_ = jsonObject.value("bgLineEditColor").toString("");
    fgLineEditColor_ = jsonObject.value("fgLineEditColor").toString("");
    borderLineEditColor_ = jsonObject.value("borderLineEditColor").toString("");
    bgSelectionLineEditColor_ = jsonObject.value("bgSelectionLineEditColor").toString("");
    fgSelectionLineEditColor_ = jsonObject.value("fgSelectionLineEditColor").toString("");
}


bool ThemeHandler::loadTheme(QString theme)
{
    QFileInfo themeDir(Utility::themesPath() + "/" + theme);
    if(!themeDir.exists() || !themeDir.isDir()) return false;

    QFile jsonFile(themeDir.filePath() + "/theme.json");
    if(!jsonFile.exists() || !jsonFile.open(QIODevice::ReadOnly))   return false;

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    loadThemeValues(themeDir.filePath() + "/", jsonData);
    defaultEmptyValues();
    themeLoaded_ = theme;
    return true;
}


QString ThemeHandler::themeLoaded()
{
    return themeLoaded_;
}


void ThemeHandler::defaultEmptyValues()
{
    //Background
    if(bgApp_.isEmpty())
    {
        bgApp_ = "background-color: black;";
    }
    else if(bgApp_.contains("."))
    {
        bgApp_ = "background-image: url(" + bgApp_ + ");background-position: center;";
    }
    else
    {
        bgApp_ = "background-color: " + bgApp_ + ";";
    }


    //Border
    QString borderWidthS = QString::number(borderWidth_);
    borderTransparent_ = "border-color: transparent; border-width: " + borderWidthS + "px; border-style: solid;";
    if(borderApp_.isEmpty())
    {
        borderWidth_ = 0;
        borderApp_ = borderTransparent_ = "border-color: transparent;";
    }
    else if(borderApp_.contains("."))
    {
        borderApp_ = "border-image: url(" + borderApp_ + ") " + borderWidthS + "; border-width: " + borderWidthS + "px;";
    }
    else
    {
        borderApp_ = "border-color: " + borderApp_ + "; border-width: " + borderWidthS + "px; border-style: solid;";
    }


    //Background widgets
    if(bgWidgets_.isEmpty())
    {
        bgWidgets_ = "background-color: transparent;";
    }
    else
    {
        bgWidgets_ = "background-color: " + bgWidgets_ + ";";
    }


    //Background Decks
    if(bgDecks_.isEmpty())
    {
        bgDecks_ = bgApp_;
    }
    else if(bgDecks_.contains("."))
    {
        bgDecks_ = "background-image: url(" + bgDecks_ + ");";
    }
    else
    {
        bgDecks_ = "background-color: " + bgDecks_ + ";";
    }


    //Border Decks
    QString borderDecksWidthS = QString::number(borderDecksWidth_);
    if(borderDecks_.isEmpty())
    {
        borderDecksWidth_ = 0;
        borderDecks_ = "border-color: transparent;";
    }
    else if(borderDecks_.contains("."))
    {
        borderDecks_ = "border-image: url(" + borderDecks_ + ") " + borderDecksWidthS + "; border-width: " + borderDecksWidthS + "px;";
    }
    else
    {
        borderDecks_ = "border-color: " + borderDecks_ + "; border-width: " + borderDecksWidthS + "px; border-style: solid;";
    }

    //Default values
    if(fgColor_.isEmpty())                  fgColor_ = "white";
    if(themeColor1_.isEmpty())              themeColor1_ = "#0F4F0F";
    if(themeColor2_.isEmpty())              themeColor2_ = "#32AF32";
    if(hoverTabsColor_.isEmpty())           hoverTabsColor_ = "transparent";
    if(selectedTabsColor_.isEmpty())        selectedTabsColor_ = "transparent";
    if(bgTabsColor_.isEmpty())              bgTabsColor_ = themeColor1_;
    if(bgTopButtonsColor_.isEmpty())        bgTopButtonsColor_ = "transparent";
    if(hoverTopButtonsColor_.isEmpty())     hoverTopButtonsColor_ = themeColor1_;
    if(bgMenuColor_.isEmpty())              bgMenuColor_ = themeColor1_;
    if(fgMenuColor_.isEmpty())              fgMenuColor_ = fgColor_;
    if(bgSelectedItemMenuColor_.isEmpty())  bgSelectedItemMenuColor_ = themeColor2_;
    if(fgSelectedItemMenuColor_.isEmpty())  fgSelectedItemMenuColor_ = fgColor_;
    if(bgSelectedItemListColor_.isEmpty())  bgSelectedItemListColor_ = themeColor1_;
    if(fgSelectedItemListColor_.isEmpty())  fgSelectedItemListColor_ = fgColor_;
    if(borderTooltipColor_.isEmpty())       borderTooltipColor_ = themeColor2_;
    if(bgTooltipColor_.isEmpty())           bgTooltipColor_ = themeColor1_;
    if(fgTooltipColor_.isEmpty())           fgTooltipColor_ = fgColor_;
    if(borderProgressBarColor_.isEmpty())   borderProgressBarColor_ = themeColor2_;
    if(bgProgressBarColor_.isEmpty())       bgProgressBarColor_ = themeColor2_;
    if(fgProgressBarColor_.isEmpty())       fgProgressBarColor_ = fgColor_;
    if(chunkProgressBarColor_.isEmpty())    chunkProgressBarColor_ = themeColor1_;
    if(borderLineEditColor_.isEmpty())      borderLineEditColor_ = themeColor2_;
    if(bgLineEditColor_.isEmpty())          bgLineEditColor_ = themeColor1_;
    if(fgLineEditColor_.isEmpty())          fgLineEditColor_ = fgColor_;
    if(bgSelectionLineEditColor_.isEmpty()) bgSelectionLineEditColor_ = fgColor_;
    if(fgSelectionLineEditColor_.isEmpty()) fgSelectionLineEditColor_ = themeColor1_;
    if(defaultFont_.isEmpty())              defaultFont_ = LG_FONT;
    if(bigFont_.isEmpty())                  bigFont_ = defaultFont_;
    if(cardsFont_.isEmpty())                cardsFont_ = HS_FONT;
    if(gamesOnZ2HColor_.isEmpty())          gamesOnZ2HColor_ = themeColor2_;
    if(tabArenaFile_.isEmpty())             tabArenaFile_ = ":/Images/arena.png";
    if(tabConfigFile_.isEmpty())            tabConfigFile_ = ":/Images/config.png";
    if(tabDeckFile_.isEmpty())              tabDeckFile_ = ":/Images/deck.png";
    if(tabEnemyDeckFile_.isEmpty())         tabEnemyDeckFile_ = ":/Images/enemyDeck.png";
    if(tabGamesFile_.isEmpty())             tabGamesFile_ = ":/Images/games.png";
    if(tabHandFile_.isEmpty())              tabHandFile_ = ":/Images/hand.png";
    if(tabLogFile_.isEmpty())               tabLogFile_ = ":/Images/log.png";
    if(tabPlanFile_.isEmpty())              tabPlanFile_ = ":/Images/plan.png";
    if(buttonRemoveDeckFile_.isEmpty())     buttonRemoveDeckFile_ = ":/Images/clearDeck.png";
    if(buttonLoadDeckFile_.isEmpty())       buttonLoadDeckFile_ = ":/Images/loadDeck.png";
    if(buttonNewDeckFile_.isEmpty())        buttonNewDeckFile_ = ":/Images/newDeck.png";
    if(buttonSaveDeckFile_.isEmpty())       buttonSaveDeckFile_ = ":/Images/saveDeck.png";
    if(buttonMinFile_.isEmpty())            buttonMinFile_ = ":/Images/minus64.png";
    if(buttonPlusFile_.isEmpty())           buttonPlusFile_ = ":/Images/plus64.png";
    if(buttonRemoveFile_.isEmpty())         buttonRemoveFile_ = ":/Images/remove64.png";
    if(buttonCloseFile_.isEmpty())          buttonCloseFile_ = ":/Images/close.png";
    if(buttonMinimizeFile_.isEmpty())       buttonMinimizeFile_ = ":/Images/minimize.png";
    if(buttonResizeFile_.isEmpty())         buttonResizeFile_ = ":/Images/resize.png";
    if(buttonForceDraftFile_.isEmpty())     buttonForceDraftFile_ = ":/Images/draft.png";
    if(buttonDraftRefreshFile_.isEmpty())   buttonDraftRefreshFile_ = ":/Images/refresh.png";
    if(buttonGamesReplayFile_.isEmpty())    buttonGamesReplayFile_ = ":/Images/replay.png";
    if(buttonGamesWebFile_.isEmpty())       buttonGamesWebFile_ = ":/Images/web.png";
    if(buttonPlanFirstFile_.isEmpty())      buttonPlanFirstFile_ = ":/Images/planFirst.png";
    if(buttonPlanLastFile_.isEmpty())       buttonPlanLastFile_ = ":/Images/planLast.png";
    if(buttonPlanRefreshFile_.isEmpty())    buttonPlanRefreshFile_ = ":/Images/refresh.png";
    if(buttonPlanPrevFile_.isEmpty())       buttonPlanPrevFile_ = ":/Images/planPrev.png";
    if(buttonPlanNextFile_.isEmpty())       buttonPlanNextFile_ = ":/Images/planNext.png";
    if(buttonPlanResizeFile_.isEmpty())     buttonPlanResizeFile_ = ":/Images/planResize.png";

    QString heroes[9] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    for(int i=0; i<9; i++)
    {
        if(bgCard1Files_[i].isEmpty())
        {
            if(bgCard1Files_[9].isEmpty())
            {
                bgCard1Files_[i] = ":/Images/bgCard1" + heroes[i] + ".png";
            }
            else
            {
                bgCard1Files_[i] = bgCard1Files_[9];
            }
        }

        if(bgCard2Files_[i].isEmpty())
        {
            if(bgCard2Files_[9].isEmpty())
            {
                bgCard2Files_[i] = ":/Images/bgCard2" + heroes[i] + ".png";
            }
            else
            {
                bgCard2Files_[i] = bgCard2Files_[9];
            }
        }

        if(heroFiles_[i].isEmpty())     heroFiles_[i] = ":/Images/hero" + Utility::getHeroLogNumber(i) + ".png";
    }
    if(bgCard1Files_[9].isEmpty())      bgCard1Files_[9] = ":/Images/bgCard1.png";
    if(bgCard2Files_[9].isEmpty())      bgCard2Files_[9] = ":/Images/bgCard2.png";
    if(bgTotalAttackFile_.isEmpty())    bgTotalAttackFile_ = ":/Images/bgTotalAttack.png";
    if(branchClosedFile_.isEmpty())     branchClosedFile_ = ":/Images/branchClosed.png";
    if(branchOpenFile_.isEmpty())       branchOpenFile_ = ":/Images/branchOpen.png";
    if(coinFile_.isEmpty())             coinFile_ = ":/Images/coin.png";
    if(firstFile_.isEmpty())            firstFile_ = ":/Images/first.png";
    if(loseFile_.isEmpty())             loseFile_ = ":/Images/lose.png";
    if(winFile_.isEmpty())              winFile_ = ":/Images/win.png";
    if(haBestFile_.isEmpty())           haBestFile_ = ":/Images/haBest.png";
    if(haCloseFile_.isEmpty())          haCloseFile_ = ":/Images/haClose.png";
    if(haOpenFile_.isEmpty())           haOpenFile_ = ":/Images/haOpen.png";
    if(lfBestFile_.isEmpty())           lfBestFile_ = ":/Images/lfBest.png";
    if(lfCloseFile_.isEmpty())          lfCloseFile_ = ":/Images/lfClose.png";
    if(lfOpenFile_.isEmpty())           lfOpenFile_ = ":/Images/lfOpen.png";
    if(handCardFile_.isEmpty())         handCardFile_ = ":/Images/handCard1.png";
    if(handCardBYFile_.isEmpty())       handCardBYFile_ = ":/Images/handCard3.png";
    if(handCardBYUnknownFile_.isEmpty())handCardBYUnknownFile_ = ":/Images/handCard2.png";
    if(starFile_.isEmpty())             starFile_ = ":/Images/legendaryStar.png";
    if(manaLimitFile_.isEmpty())        manaLimitFile_ = ":/Images/manaLimit.png";
    if(unknownFile_.isEmpty())          unknownFile_ = ":/Images/unknown.png";
    if(minionsCounterFile_.isEmpty())   minionsCounterFile_ = ":/Images/minionsCounter.png";
    if(spellsCounterFile_.isEmpty())    spellsCounterFile_ = ":/Images/spellsCounter.png";
    if(weaponsCounterFile_.isEmpty())   weaponsCounterFile_ = ":/Images/weaponsCounter.png";
    if(manaCounterFile_.isEmpty())      manaCounterFile_ = ":/Images/manaCounter.png";
    if(aoeMechanicFile_.isEmpty())      aoeMechanicFile_ = ":/Images/aoeMechanic.png";
    if(tauntMechanicFile_.isEmpty())    tauntMechanicFile_ = ":/Images/tauntMechanic.png";
    if(survivalMechanicFile_.isEmpty()) survivalMechanicFile_ = ":/Images/restoreMechanic.png";
    if(drawMechanicFile_.isEmpty())     drawMechanicFile_ = ":/Images/drawMechanic.png";
    if(pingMechanicFile_.isEmpty())     pingMechanicFile_ = ":/Images/pingMechanic.png";
    if(damageMechanicFile_.isEmpty())   damageMechanicFile_ = ":/Images/damageMechanic.png";
    if(destroyMechanicFile_.isEmpty())  destroyMechanicFile_ = ":/Images/destroyMechanic.png";
    if(reachMechanicFile_.isEmpty())    reachMechanicFile_ = ":/Images/reachMechanic.png";
    if(bgDraftMechanicsFile_.isEmpty()) bgDraftMechanicsFile_ = ":/Images/bgDraftMechanics.png";
    if(bgDraftMechanicsHelpFile_.isEmpty()) bgDraftMechanicsHelpFile_ = ":/Images/bgDraftMechanicsHelp.png";
    if(fgDraftMechanicsColor_.isEmpty())fgDraftMechanicsColor_ = fgColor_;
}
