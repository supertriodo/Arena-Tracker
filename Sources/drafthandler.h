#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/draftcard.h"
#include "heartharenamentor.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include <QObject>
#include <QFutureWatcher>


#define CAPTUREDRAFT_START_TIME         500
#define CAPTUREDRAFT_LOOP_TIME          200
#define CAPTUREDRAFT_LOOP_FLANN_TIME    2000


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
    QString arenaHero;
    QList<int> draftedCards;
    QString codesCandidates[3];
    double deckRating;
    cv::Rect screenRects[3];
    int screenIndex;
    int nextCount;
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


//Metodos
private:
    void completeUI();
    cv::MatND getHist(QString &code);
    cv::MatND getHist(cv::Mat &srcBase);
    void initCodesAndHistMaps(QString &hero);
    void resetTab();
    void clearLists();
    void getScreenCardsHist(cv::MatND screenCardsHist[3]);
    void getBestMatchingCodes(cv::MatND screenCardsHist[3], QString codes[3]);
    bool areNewCards(QString codes[3]);
    bool areSameRarity(QString codes[]);
    void showNewCards(QString codes[3]);
    void createHearthArenaMentor();    
    void resetCodesCandidates();    
    void updateBoxTitle(double cardRating);
    bool screenRectsFound();
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
    void initLightForgeTiers(const QString &heroString);
    void createDraftScoreWindow();

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
    void beginDraft(QString hero);
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
};

#endif // DRAFTHANDLER_H
