#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "ui_mainwindow.h"
#include "draftcard.h"
#include "heartharenamentor.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <QObject>

using namespace cv;

class DraftHandler : public QObject
{
    Q_OBJECT
public:
    DraftHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::MainWindow *ui);
    ~DraftHandler();

//Variables
private:
    Ui::MainWindow *ui;
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


//Metodos
private:
    void completeUI();
    cv::MatND getHist(QString &code);
    cv::MatND getHist(cv::Mat &srcBase);
    void initCodesAndHistMaps(QString &hero);
    void resetTab();
    void clearLists();
    bool getScreenCardsHist(cv::MatND screenCardsHist[3]);
    void getBestMatchingCodes(cv::MatND screenCardsHist[3], QString codes[3]);
//    void showImage(QString code);
    bool areNewCards(QString codes[3]);
    bool areSameRarity(QString codes[]);
    void showNewCards(QString codes[3]);
    void createHearthArenaMentor();    
    void insertIntoDeck();    
    void resetCodesCandidates();    
    void updateBoxTitle(QString cardRating);    
    bool findScreenRects();    
    void selectMouseCard();

public:
    void reHistDownloadedCardImage(QString code);

signals:
    void checkCardImage(QString code);
    void setStatusBarMessage(QString message, int timeout);
    void newDeckCard(QString code, int total);
    void sendLog(QString line);

public slots:
    void beginDraft(QString hero);
    void endDraft();
    void pauseDraft();
    void resumeDraft();
    void showNewCards(QString tip, double rating1, double rating2, double rating3,
                      QString synergy1, QString synergy2, QString synergy3);

private slots:
    void captureDraft();
};

#endif // DRAFTHANDLER_H
