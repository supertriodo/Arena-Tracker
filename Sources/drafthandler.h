#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "Widgets/ui_extended.h"
#include "Cards/draftcard.h"
#include "heartharenamentor.h"
#include "utility.h"
#include "Widgets/draftscorewindow.h"
#include <QObject>


#define CAPTUREDRAFT_START_TIME         0
#define CAPTUREDRAFT_LOOP_TIME          200
#define CAPTUREDRAFT_LOOP_FLANN_TIME    2000

class DraftHandler : public QObject
{
    Q_OBJECT
public:
    DraftHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui);
    ~DraftHandler();

//Variables
private:
    Ui::Extended *ui;
    QMap<QString, QJsonObject> *cardsJson;
    HearthArenaMentor *hearthArenaMentor;
    QMap<QString, int> hearthArenaCodes;
    QMap<QString, cv::MatND> cardsHist;
    int cardsDownloading;
    DraftCard draftCards[3];
    bool captureLoop;
    QString arenaHero;
    QList<int> draftedCards;
    QString codesCandidates[3];
    double deckRating;
    cv::Rect screenRects[3];
    int screenIndex;
    int nextCount;
    bool drafting;
    Transparency transparency;
    DraftScoreWindow *draftScoreWindow;
    bool showDraftOverlay;
    bool learningMode;


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
    bool findScreenRects();
    void removeTabHero();
    void clearScore(QLabel *label, bool clearText=true);
    void highlightScore(QLabel *label);
    void deleteDraftScoreWindow();
    void showOverlay();

public:
    void reHistDownloadedCardImage(QString &code);
    void setTransparency(Transparency value);
    void setShowDraftOverlay(bool value);
    void setLearningMode(bool value);
    void redrawAllCards();

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
    void pauseDraft();
    void resumeDraft();
    void showNewRatings(QString tip, double rating1, double rating2, double rating3,
                        double tierScore1, double tierScore2, double tierScore3,
                        QString synergy1, QString synergy2, QString synergy3);
    void pickCard(QString code);

private slots:
    void captureDraft();
};

#endif // DRAFTHANDLER_H
