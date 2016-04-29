#ifndef WEBUPLOADER_H
#define WEBUPLOADER_H

#include "gamewatcher.h"
#include "Cards/deckcard.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QUrlQuery>



#define WEB_URL QString("http://www.arenamastery.com/")


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
    bool connected;


//Metodos
private:
    QString heroToWebNumber(const QString &hero);
    GameResult createGameResult(const QRegularExpressionMatch &match, const QString &arenaCurrentHero);
    bool getArenaCurrentAndGames(QNetworkReply *reply, QList<GameResult> &list, bool getCards=false);
    void uploadNext();
    void postRequest(QNetworkRequest request, QUrlQuery postData);
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
    bool isConnected();

signals:
    void loadedGameResult(GameResult gameResult, LoadingScreenState loadingScreen);
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
