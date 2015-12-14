#ifndef WEBUPLOADER_H
#define WEBUPLOADER_H

#include "gamewatcher.h"
#include "Cards/deckcard.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QUrlQuery>
    #define QURL QUrlQuery
#else
    #define QURL QUrl
#endif



#define WEB_URL QString("http://www.arenamastery.com/")

class GameResultPost
{
public:
    QURL postData;
    QNetworkRequest request;
};


class WebUploader : public QObject
{
    Q_OBJECT
public:
    WebUploader(QObject *parent);
    ~WebUploader();

    enum WebState { signup, checkArenaCurrentLoad, loadArenaCurrent,
                    createArena, rewardsSent, checkArenaCurrentReload,
                    gameResultSent, reloadArenaCurrent,
                    getArenaDeck1, getArenaDeck2, complete };

//Variables
private:
    QNetworkAccessManager *networkManager;
    WebState webState;
    int arenaCurrentID, lastArenaID;
    QString arenaCurrentHero;
    QList<GameResultPost> *gameResultPostList;
    GameResultPost *rewardsPost;
    QRegularExpressionMatch *match;
    bool deckInWeb;
    QString arenaCards;
    QString playerEmail, password;
    bool isConnected;


//Metodos
private:
//    void readSettings(QString &playerEmail, QString &password);
//    void askLoginData(QString &playerEmail, QString &password);
    QString heroToWebNumber(const QString &hero);
    GameResult createGameResult(const QRegularExpressionMatch &match, const QString &arenaCurrentHero);
    bool getArenaCurrentAndGames(QNetworkReply *reply, QList<GameResult> &list, bool getCards=false);
    void uploadNext();
    void postRequest(QNetworkRequest request, QURL postData);
    void createArenaCards(QList<DeckCard> &deckCardList);
    void getArenaCards(QString &html);
    void uploadArenaCards();    
    void showWebState();
    void checkArenaReload();

public:
    bool uploadArenaRewards(ArenaRewards &arenaRewards);
    bool uploadNewGameResult(GameResult &gameresult, QList<DeckCard> *deckCardList=NULL);
    bool uploadNewArena(const QString &hero);
    void uploadDeck(QList<DeckCard> *deckCardList);
    void refresh();
    void tryConnect(QString playerEmail, QString password);

signals:
    void loadedGameResult(GameResult gameResult);
    void loadedArena(QString hero);
    void loadArenaCurrentFinished();
    void reloadedGameResult(GameResult gameResult);
    void reloadedArena(QString hero);
    void synchronized();
    void noArenaFound();
    void newDeckCard(QString card, int total);
    void newWebDeckCardList();
    void connectionTried(bool isConnected);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="WebUploader");

private slots:
    void connectWeb();

public slots:
    void replyFinished(QNetworkReply *reply);
    void askArenaCards();
};

#endif // WEBUPLOADER_H
