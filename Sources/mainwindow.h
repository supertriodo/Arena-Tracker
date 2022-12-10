#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "detachwindow.h"
#include "logloader.h"
#include "gamewatcher.h"
#include "Cards/deckcard.h"
#include "hscarddownloader.h"
#include "enemydeckhandler.h"
#include "graveyardhandler.h"
#include "deckhandler.h"
#include "enemyhandhandler.h"
#include "planhandler.h"
#include "arenahandler.h"
#include "secretshandler.h"
#include "popularcardshandler.h"
#include "drawcardhandler.h"
#include "rngcardhandler.h"
#include "drafthandler.h"
#include "trackobotuploader.h"
#include "premiumhandler.h"
#include "Widgets/cardwindow.h"
#include "Widgets/cardlistwindow.h"
#include <QMainWindow>
#include <QJsonObject>

#define DIVIDE_TABS_H 444
#define DIVIDE_TABS_H2 666
#define DIVIDE_TABS_V 500
#define BIG_BUTTONS_H 48
#define SMALL_BUTTONS_H 19
#define DEFAULT_THEME "Random"
#define JSON_CARDS_URL "https://api.hearthstonejson.com/v1/latest/all/cards.json"
#define HEARTHARENA_TIERLIST_URL "https://www.heartharena.com/tierlist"
//#define HEARTHARENA_TIERLIST_URL "https://www.heartharena.com/tierlist/preview" //Problematico, mejor evitar
#define HSR_HEROES_WINRATE "https://hsreplay.net/api/v1/analytics/query/player_class_performance_summary/"
#define HSR_CARDS_PATCH "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=ARENA&TimeRange=CURRENT_PATCH"
#define HSR_CARDS_EXP "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=ARENA&TimeRange=CURRENT_EXPANSION"
#define HSR_CARDS_14DAYS "https://hsreplay.net/api/v1/analytics/query/card_list_free/?GameType=ARENA&TimeRange=LAST_14_DAYS"
#define EXTRA_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Extra"
#define IMAGES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Images"
#define THEMES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Themes"
#define HA_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/HearthArena"
#define ARENA_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Arena"
#define SYNERGIES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Synergies"
#define USER_GUIDE_URL          "https://triodo.gitbook.io/arena-tracker-documentation/en"


namespace Ui {
class Extended;
}

class DetachWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

friend class DetachWindow;

//Constructor
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;


//Variables
private:
    Ui::Extended *ui;
    bool patreonVersion;
    LogLoader *logLoader;
    GameWatcher *gameWatcher;
    HSCardDownloader *cardDownloader;
    EnemyDeckHandler *enemyDeckHandler;
    GraveyardHandler *graveyardHandler;
    DrawCardHandler *drawCardHandler;
    RngCardHandler *rngCardHandler;
    DeckHandler *deckHandler;
    EnemyHandHandler *enemyHandHandler;
    PlanHandler *planHandler;
    ArenaHandler *arenaHandler;
    SecretsHandler *secretsHandler;
    PopularCardsHandler *popularCardsHandler;
    DraftHandler * draftHandler;
    CardWindow *cardWindow;
    CardListWindow *cardListWindow;
    TrackobotUploader *trackobotUploader;
    PremiumHandler *premiumHandler;
    QMap<QString, QJsonObject> cardsJson;
    QPoint dragPosition;
    QFile* atLogFile;
    bool mouseInApp;
    Transparency transparency;
    bool oneWindow;
    DetachWindow *deckWindow, *arenaWindow, *enemyWindow, *enemyDeckWindow, *graveyardWindow, *planWindow;
    int cardHeight;
    int drawDisappear;
    QNetworkAccessManager *networkManager;
    QStringList allCardsDownloadList;
    TwitchHandler *twitchTester;
    //Gestionan si es necesario bajar todas las cartas usadas en arena debido a que el directorio de cartas se haya borrado
    //o haya una nueva version de tier list (rotacion sets)
    //Si es necesario tambien se reconstruira el string de sets activos en arena "arenaSets" que se usa para saber que secretos mostrar
    bool cardsJsonLoaded, arenaSetsLoaded, allCardsDownloadNeeded;
    QMap<QString, float> *cardsPickratesMap;
    QMap<QString, float> *cardsIncludedWinratesMap;
    QMap<QString, int> *cardsIncludedDecksMap;
    QMap<QString, float> *cardsPlayedWinratesMap;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsPickrates;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsIncludedWinrates;
    QFutureWatcher<QMap<QString, int> *> futureProcessHSRCardsIncludedDecks;
    QFutureWatcher<QMap<QString, float> *> futureProcessHSRCardsPlayedWinrates;



//Metodos
public:
    LoadingScreenState getLoadingScreen();

private:
    void createLogLoader();
    void createArenaHandler();
    void createGameWatcher();
    void createCardWindow();
    void createCardListWindow();
    void createCardDownloader();
    void createEnemyDeckHandler();
    void createGraveyardHandler();
    void createDrawCardHandler();
    void createRngCardHandler();
    void createDeckHandler();
    void createEnemyHandHandler();
    void createPlanHandler();
    void createSecretsHandler();
    void createPopularCardsHandler();
    void createDraftHandler();
    void createVersionChecker();
    void createTrackobotUploader();
    void createPremiumHandler();
    void readSettings();
    void writeSettings();
    void completeUI();
    void completeUIButtons();
    void completeUITabNames();
    void completeConfigTab();
    void addDraftMenu(QPushButton *button);
    void spreadTransparency(Transparency newTransparency);
    void updateOtherTabsTransparency();
    void spreadTheme();
    void updateMainUITheme();
    void updateAllDetachWindowTheme(const QString &mainCSS);
    void updateDetachWindowTheme(QWidget *paneWidget);
    void updateButtonsTheme();
    void updateTabWidgetsTheme(bool transparent, bool resizing);
    QString getHSLanguage();
    void createCardsJsonMap(QByteArray &jsonData);
    void resizeTopButtons(int right, int top);
    void resizeChecks();
    void moveTabTo(QWidget *widget, QTabWidget *tabWidget);
    void resetSettings();
    void createLogFile();
    void closeLogFile();
    void createDataDir();
    void calculateCardWindowMinimumWidth(DetachWindow *detachWindow, bool hasBorders);
    void initConfigTab(int tooltipScale, int cardHeight, bool autoSize, bool showClassColor, bool showSpellColor, bool showManaLimits,
                       bool showTotalAttack, bool showRngList, bool twitchChatVotes, QString theme,
                       bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR, QString draftAvg,
                       int popularCardsShown, bool showSecrets, bool showWildSecrets,
                       bool showDraftScoresOverlay, bool showDraftMechanicsOverlay, bool draftLearningMode, bool draftShowDrops);
    void moveInScreen(QPoint pos, QSize size);
    int getScreenHighest();
    void completeHighResConfigTab();
    void spreadTamCard(int value);
    int getTamCard();
    int getAutoTamCard();
    void createNetworkManager();
    void initCardsJson();
    void removeHSCards(bool forceRemove = false);
    void removeExtraAndHistograms();
    void removeHistograms();
    void checkCardsJsonVersion(QString cardsJsonVersion);
    void askLinuxShortcut();
    void showMessageAppImageShortcut();
    void createLinuxShortcut();
    void createDebugPack();
    void showWindowFrame(bool showFrame=true);
    void spreadDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR);
    DraftMethod draftMethodFromString(QString draftAvg);
    void showProgressBar(bool animated=true);
    bool askImportAccount();
    void checkFirstRunNewVersion();
    void startProgressBarMini(int maximum);
    void hideProgressBarMini();
    void advanceProgressBarMini(int remaining);
    void updateProgressAllCardsDownload(QString code);
    void completeConfigComboTheme();
    void completeConfigComboAvg();
    void initConfigTheme(QString theme);
    void downloadExtraFiles();
    void downloadThemes();
    void downloadTheme(QString theme, int version);
    void downloadHearthArenaVersion();
    void downloadHearthArenaJson(int version);
    void downloadArenaVersion();
    void checkArenaVersionJson(const QJsonObject &jsonObject);
    void downloadSynergiesVersion();
    void downloadSynergiesJson(int version);
    void updateTabIcons();
    void initHSRHeroesWinrate();
    void localHSRHeroesWinrate();
    void processHSRHeroesWinrate(const QJsonObject &jsonObject);
    void deleteTwitchTester();
    void checkTwitchConnection();
    void checkArenaCards();
    void downloadAllArenaCodes(const QStringList &codeList);
    void processHSRCardClassDouble(const QJsonArray &jsonArray, const QString &tag, QMap<QString, float> &cardsMap, bool trunk=false);
    void processHSRCardClassInt(const QJsonArray &jsonArray, const QString &tag, QMap<QString, int> &cardsMap);
    void initHSRCards();
    void localHSRCards();
    void startProcessHSRCards(const QJsonObject &jsonObject);
    void downloadHearthArenaTierlistOriginal();
    void saveHearthArenaTierlistOriginal(const QByteArray &html="");
    void initConfigAvgScore(QString draftAvg);

protected:
    //Override events
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;

//Signals
signals:


//Slots
public slots:
    //GameWatcher
    void resetDeck(bool deckRead=false);
    void resetDeckDontRead();

    //Multi Handlers
    bool checkCardImage(QString code, bool isHero=false);

    //HSCardDownloader
    void redrawDownloadedCardImage(QString code);

    //Widgets
    void resizeSlot(QSize size);

    //MainWindow
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="MainWindow");
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file);


private slots:
    void test();
    void testEnemyHand();
    void testPlan();
    void testArenaGames();
    void testDelay();
    void testSynergies();
    void testHeroPortraits();
    void downloadHeroPortrait(QString code);
    void testTierlists();
    void testDownloadRotation(bool fromHearth);
    void testSecretsHSR(LoadingScreenState loadingScreenState);
    void testPopularList(int i=2);
    void confirmNewArenaDraft(QString hero);
    void transparentAlways();
    void transparentAuto();
    void transparentNever();
    void transparentFramed();
    void updateTimeDraw(int value);
    void updatePopularCardsShown(int value);
    void updateTamCard(int value);
    void updateShowDraftScoresOverlay(bool checked);
    void updateShowDraftMechanicsOverlay(bool checked);
    void updateDraftLearningMode(bool checked);
    void updateDraftShowDrops(bool checked);
    void updateTooltipScale(int value);
    void closeApp();
    void updateShowClassColor(bool checked);
    void updateShowSpellColor(bool checked);
    void updateShowManaLimits(bool checked);
    void fadeBarAndButtons(bool fadeOut);
    void spreadMouseInApp();
    void logReset();
    void spreadCorrectTamCard();
    void completeArenaDeck();
    void changingTabResetSizePlan();
    void resetSizePlan();
    void updateShowTotalAttack(bool checked);
    void updateShowRngList(bool checked);
    void updateShowSecrets(bool checked);
    void updateShowWildSecrets(bool checked);
    void setLocalLang();
    void replyFinished(QNetworkReply *reply);
    void checkLinuxShortcut();
    void spreadTransparency();
    void startProgressBar(int maximum, QString text);
    void advanceProgressBar(int remaining, QString text="");
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void hideProgressBar();
    void missingOnWeb(QString code);
    void allCardsDownloaded();
    void init();
    void loadTheme(QString theme, bool initTheme=false);
    void showPremiumDialog();
    void createDetachWindow(int index, const QPoint &dropPoint);
    void createDetachWindow(QWidget *paneWidget, const QPoint& dropPoint = QPoint());
    void closedDetachWindow(DetachWindow *detachWindow, QWidget *paneWidget);
    void swapSizePlan(bool sizePlan);
    void calculateMinimumWidth();
    void changingTabUpdateDraftSize();
    void setPremium(bool premium);
    void twitchTesterConnectionOk(bool ok, bool setup = true);
    void updateTwitchChatVotes(bool checked);
    void configureTwitchDialogs();
    void processPopularCardsHandlerPickrates();
    void openUserGuide();
    void spreadDraftMethod();
    void spreadDraftAvg(QString draftAvg);
};

#endif // MAINWINDOW_H
