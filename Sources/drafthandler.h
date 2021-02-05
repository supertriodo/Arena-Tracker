#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "Widgets/ui_extended.h"
#include "deckhandler.h"
#include "Cards/draftcard.h"
#include "utility.h"
#include "Widgets/draftherowindow.h"
#include "Widgets/draftscorewindow.h"
#include "Widgets/draftmechanicswindow.h"
#include "synergyhandler.h"
#include "twitchhandler.h"
#include "arenahandler.h"
#include <QObject>
#include <QFutureWatcher>


#define CAPTUREDRAFT_START_TIME         1500
#define CAPTUREDRAFT_LOOP_TIME          100
#define CAPTUREDRAFT_LOOP_TIME_FADING   200
#define FINDINGFRAME_LOOP_TIME          200

#define CARD_ACCEPTED_THRESHOLD             0.35
#define CARD_ACCEPTED_THRESHOLD_INCREASE    0.02
#define CAPTURE_MIN_CANDIDATES                 7
#define CAPTURE_EXTENDED_CANDIDATES            15


class ScreenDetection
{
public:
    cv::Rect screenRects[3];
    int screenIndex = -1;
    int screenHeight = 1;
    QPointF screenScale = QPointF(0,0);
};

class DraftHandler : public QObject
{
    Q_OBJECT
public:
    DraftHandler(QObject *parent, Ui::Extended *ui, DeckHandler *deckHandler, ArenaHandler *arenaHandler);
    ~DraftHandler();

//Variables
private:
    bool patreonVersion;
    Ui::Extended *ui;
    DeckHandler *deckHandler;
    ArenaHandler *arenaHandler;
    SynergyHandler *synergyHandler;
    LavaButton *lavaButton;
    ScoreButton *scoreButtonLF, *scoreButtonHA, *scoreButtonHSR;
    QMap<QString, int> hearthArenaTiers;
    QMap<QString, int> lightForgeTiers;
    QMap<QString, cv::MatND> cardsHist;
    QStringList cardsDownloading;
    DraftCard draftCards[3];
    //Guarda los mejores candidatos de esta iteracion
    QMap<QString, DraftCard> draftCardMaps[3];  //[Code(_premium)] --> DraftCard
    //Se crea al final de la iteracion para ordenar los candidatos por match score
    QMap<double, QString> bestMatchesMaps[3];   //[Match] --> Code(_premium)
    bool cardDetected[3];
    CardClass arenaHero;
    int deckRatingHA, deckRatingLF;
    float deckRatingHSR;
    cv::Rect screenRects[3];
    int screenIndex;
    int numCaptured;
    bool drafting, heroDrafting, capturing, findingFrame, leavingArena;
    bool mouseInApp;
    Transparency transparency;
    DraftHeroWindow *draftHeroWindow;
    DraftScoreWindow *draftScoreWindow;
    DraftMechanicsWindow * draftMechanicsWindow;
    bool showDraftScoresOverlay, showDraftMechanicsOverlay;
    bool learningMode, showDrops;
    QString justPickedCard; //Evita doble pick card en Arena.log
    bool draftMethodHA, draftMethodLF, draftMethodHSR;
    DraftMethod draftMethodAvgScore;
    QFutureWatcher<ScreenDetection> futureFindScreenRects;
    QLabel *labelLFscore[3];
    QLabel *labelHAscore[3];
    QLabel *labelHSRscore[3];
    QComboBox *comboBoxCard[3];
    bool extendedCapture;
    QStringList heroCodesList;
    QMap<QString, float> heroWinratesMap;
    QMap<QString, float> *cardsIncludedWinratesMap;
    QMap<QString, int> *cardsIncludedDecksMap;
    QMap<QString, float> *cardsPlayedWinratesMap;
    TwitchHandler *twitchHandler;
    bool multiclassArena;
    QStringList arenaSets;


//Metodos
private:
    void completeUI();
    cv::MatND getHist(const QString &code);
    cv::MatND getHist(cv::Mat &srcBase);
    void initCodesAndHistMaps(QString hero="");
    void resetTab(bool alreadyDrafting);
    void clearLists(bool keepCounters);
    void endDraft();
    bool getScreenCardsHist(cv::MatND screenCardsHist[3]);
    void showNewCards(DraftCard bestCards[]);
    void updateDeckScore(float cardRatingHA=0, float cardRatingLF=0, float cardRatingHSR=0);
    bool screenFound();
    ScreenDetection findScreenRects();
    void clearScore(QLabel *label, DraftMethod draftMethod, bool clearText=true);
    void highlightScore(QLabel *label, DraftMethod draftMethod);
    void deleteDraftHeroWindow();
    void deleteDraftScoreWindow();
    void showOverlay();
    void newCaptureDraftLoop(bool delayed=false);
    void updateScoresVisibility();
    void initHearthArenaTiers(const QString &heroString, const bool multiClassDraft);
    void initLightForgeTiers(const CardClass &heroClass, const bool multiClassDraft, const bool createCardHist);
    void createDraftWindows(const QPointF &screenScale);
    void mapBestMatchingCodes(cv::MatND screenCardsHist[]);
    double getMinMatch(const QMap<QString, DraftCard> &draftCardMaps);
    bool areCardsDetected();
    void buildBestMatchesMaps();
    CardRarity getBestRarity();
    void getBestCards(DraftCard bestCards[3]);
    void addCardHist(QString code, bool premium, bool isHero=false);
    QString degoldCode(QString fileName);
    void createScoreItems();
    void createSynergyHandler();
    bool isGoldCode(QString fileName);
    void connectAllComboBox();
    void clearAndDisconnectAllComboBox();
    void clearAndDisconnectComboBox(int index);
    void initDraftMechanicsWindowCounters();
    void initSynergyCounters(QList<DeckCard> &deckCardList);
    void updateLabelDeckScore(int deckScoreLF, int deckScoreHA, float deckScoreHSR, int numCards);
    void showMessageDeckScore(int deckScoreLF, int deckScoreHA, float deckScoreHSR);
    void updateAvgScoresVisibility();
    void endHeroDraft();
    void showNewHeroes();
    void createTwitchHandler();
    void deleteTwitchHandler();
    QString getDeckAvgString(int deckScoreLF, int deckScoreHA, float deckScoreHSR);
    void buildDraftMechanicsWindow();

public:
    void buildHeroCodesList();
    void reHistDownloadedCardImage(const QString &fileNameCode, bool missingOnWeb=false);
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setShowDraftScoresOverlay(bool value);
    void setShowDraftMechanicsOverlay(bool value);
    void setLearningMode(bool value);
    void setShowDrops(bool value);
    void redrawAllCards();
    void updateTamCard();
    void setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR);
    void setTheme();
    void craftGoldenCopy(int cardIndex);
    bool isDrafting();
    void deMinimizeScoreWindow();
    QStringList getAllArenaCodes();
    QStringList getAllHeroCodes();
    void setHeroWinratesMap(QMap<QString, float> &heroWinratesMap);
    void setCardsIncludedWinratesMap(QMap<QString, float> cardsIncludedWinratesMap[]);
    void setCardsIncludedDecksMap(QMap<QString, int> cardsIncludedDecksMap[]);
    void setCardsPlayedWinratesMap(QMap<QString, float> cardsPlayedWinratesMap[]);
    void updateTwitchChatVotes();
    void setDraftMethodAvgScore(DraftMethod draftMethodAvgScore);
    void setMulticlassArena(bool multiclassArena);
    SynergyHandler *getSynergyHandler();
    void setArenaSets(QStringList arenaSets);

signals:
    void checkCardImage(QString code, bool isHero=false);
    void newDeckCard(QString code);
    void draftStarted();
    void draftEnded();
    void downloadStarted();
    void downloadEnded();
    void overlayCardEntered(QString code, QRect rectCard, int maxTop, int maxBottom, bool alignReverse=true);
    void overlayCardLeave();
    void advanceProgressBar(int remaining, QString text);
    void startProgressBar(int maximum, QString text);
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void itemEnter(QList<DeckCard> &deckCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemEnterOverlay(QList<DeckCard> &deckCardList, QPoint &originList, int maxLeft=-1, int maxRight=-1);
    void itemLeave();
    void showPremiumDialog();
    void calculateMinimumWidth();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftHandler");

public slots:
    void beginDraft(QString hero, QList<DeckCard> deckCardList = QList<DeckCard>());
    void continueDraft();
    void beginHeroDraft();
    void heroDraftDeck(QString hero);
    void endDraftShowMechanicsWindow();
    void endDraftHideMechanicsWindow();
    void deleteDraftMechanicsWindow();
    void showNewRatings(float rating1, float rating2, float rating3,
                        float tierScore1, float tierScore2, float tierScore3, DraftMethod draftMethod,
                        int includedDecks1=-1, int includedDecks2=-1, int includedDecks3=-1);
    void pickCard(QString code);
    void enterArena();
    void leaveArena();
    void minimizeScoreWindow();
    void setPremium(bool premium);
    void updateMinimumHeight();

private slots:
    void captureDraft();
    void finishFindScreenRects();
    void startFindScreenRects();
    void comboBoxChanged();
    void refreshCapturedCards();
    void twitchHandlerConnectionOk(bool ok);
    void twitchHandlerVoteUpdate(int vote1, int vote2, int vote3, QString username);
    void showHSRwebPicks();
    void newFindScreenLoop();
};

#endif // DRAFTHANDLER_H
