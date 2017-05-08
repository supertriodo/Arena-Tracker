#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/draftcard.h"
#include "heartharenamentor.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include <QObject>
#include <QFutureWatcher>


#define CAPTUREDRAFT_START_TIME         1500
#define CAPTUREDRAFT_LOOP_TIME          200
#define CAPTUREDRAFT_LOOP_FLANN_TIME    200

#define CARD_ACCEPTED_THRESHOLD             0.35
#define CARD_ACCEPTED_THRESHOLD_INCREASE    0.01


class LFtier
{
public:
    int score = 0;
    int maxCard = -1;
};

class ScreenDetection
{
public:
    cv::Rect screenRects[3];
    int screenIndex = -1;
};

class DraftHandler : public QObject
{
    Q_OBJECT
public:
    DraftHandler(QObject *parent, Ui::Extended *ui);
    ~DraftHandler();

//Variables
private:
    Ui::Extended *ui;
    HearthArenaMentor *hearthArenaMentor;
    QMap<QString, int> hearthArenaCodes;
    QMap<QString, LFtier> lightForgeTiers;
    QMap<QString, cv::MatND> cardsHist;
    int cardsDownloading;
    DraftCard draftCards[3];
    QMap<QString, DraftCard> draftCardMaps[3];
    QMap<double, QString> bestMatchesMaps[3];
    bool cardDetected[3];
    QString arenaHero;
    QList<int> draftedCards;
    double deckRating;
    cv::Rect screenRects[3];
    int screenIndex;
    int numCaptured;
    bool drafting, capturing, leavingArena;
    bool mouseInApp;
    Transparency transparency;
    Theme theme;
    DraftScoreWindow *draftScoreWindow;
    bool showDraftOverlay;
    bool learningMode;
    QString justPickedCard; //Evita doble pick card en Arena.log
    DraftMethod draftMethod;
    QFutureWatcher<ScreenDetection> futureFindScreenRects;
    QFutureWatcher<QMap<QString, LFtier> > futureInitLightForgeTiers;
    QLabel *labelCard[3];
    QLabel *labelLFscore[3];
    QLabel *labelHAscore[3];
    double shownTierScores[3];


//Metodos
private:
    void completeUI();
    cv::MatND getHist(QString &code);
    cv::MatND getHist(cv::Mat &srcBase);
    void initCodesAndHistMaps(QString &hero);
    void resetTab();
    void clearLists(bool keepDraftedCards);
    void getScreenCardsHist(cv::MatND screenCardsHist[3]);
    void showNewCards(DraftCard bestCards[]);
    void createHearthArenaMentor();    
    void updateBoxTitle(double cardRating=0);
    bool screenFound();
    ScreenDetection findScreenRects();
    void removeTabHero();
    void clearScore(QLabel *label, DraftMethod draftMethod, bool clearText=true);
    void highlightScore(QLabel *label, DraftMethod draftMethod);
    void deleteDraftScoreWindow();
    void showOverlay();
    void newCaptureDraftLoop(bool delayed=false);
    void updateScoresVisibility();
    void updateTipVisibility();
    void initHearthArenaCodes(QString &hero);
    QMap<QString, LFtier> initLightForgeTiers(const QString &heroString);
    void createDraftScoreWindow();
    void startInitLightForgeTiers(const QString &heroString);
    void initDraftedCards(QList<DeckCard> deckCardList);
    void mapBestMatchingCodes(cv::MatND screenCardsHist[]);
    double getMinMatch(const QMap<QString, DraftCard> &draftCardMaps);
    bool areCardsDetected();
    void buildBestMatchesMaps();
    void getBestCards(DraftCard bestCards[3]);

public:
    void reHistDownloadedCardImage(QString &code);
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setShowDraftOverlay(bool value);
    void setLearningMode(bool value);
    void redrawAllCards();
    void updateTamCard(int value);
    void setDraftMethod(DraftMethod value);
    void setTheme(Theme theme);
    void craftGoldenCopy(int cardIndex);
    bool isDrafting();

signals:
    void checkCardImage(QString code);
    void newDeckCard(QString code);
    void draftStarted();
    void draftEnded();
    void overlayCardEntered(QString code, QRect rectCard, int maxTop, int maxBottom, bool alignReverse=true);
    void overlayCardLeave();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftHandler");

public slots:
    void beginDraft(QString hero, QList<DeckCard> deckCardList=QList<DeckCard>());
    void endDraft();
    void showNewRatings(QString tip, double rating1, double rating2, double rating3,
                        double tierScore1, double tierScore2, double tierScore3,
                        QString synergy1, QString synergy2, QString synergy3,
                        int maxCard1, int maxCard2, int maxCard3, DraftMethod draftMethod);
    void pickCard(QString code);
    void enterArena();
    void leaveArena();

private slots:
    void captureDraft();
    void finishFindScreenRects();
    void startFindScreenRects();
    void finishInitLightForgeTiers();
};

#endif // DRAFTHANDLER_H
