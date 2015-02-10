#ifndef WEBUPLOADER_H
#define WEBUPLOADER_H

#include "gamewatcher.h"
#include "deckcard.h"
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
    WebUploader(QObject *parent, QMap<QString, QJsonObject> *cardsJson);
    ~WebUploader();

    enum WebState { signup, checkArenaCurrentLoad, loadArenaCurrent,
                    createArena, checkArenaCurrentReload1, checkArenaCurrentReload2,
                    reloadArenaCurrent1, reloadArenaCurrent2, complete };

//Variables
private:
    QNetworkAccessManager *networkManager;
    WebState webState;
    int arenaCurrentID;
    QString arenaCurrentHero;
    QList<GameResultPost> *gameResultPostList;
    GameResultPost *rewardsPost;
    QRegularExpressionMatch *match;
    bool deckInWeb;
    QString arenaCards;
    QMap<QString, QJsonObject> *cardsJson;

//Metodos
private:
    void connectWeb();
    void readSettings(QString &playerEmail, QString &password);
    void askLoginData(QString &playerEmail, QString &password);
    QString heroToLogNumber(const QString &hero);
    QString heroToWebNumber(const QString &hero);
    GameResult createGameResult(const QRegularExpressionMatch &match, const QString &arenaCurrentHero);
    bool getArenaCurrentAndGames(QNetworkReply *reply, QList<GameResult> &list, bool getCards=false);
    void uploadNext();
    void postRequest(QNetworkRequest request, QURL postData);
    void createArenaCards(QList<DeckCard> &deckCardList);
    QString codeFromName(QString name);
    void GetArenaCards(QString &html);

public:
    bool uploadArenaRewards(ArenaRewards &arenaRewards);
    bool uploadNewGameResult(GameResult &gameresult, QList<DeckCard> *deckCardList=NULL);
    bool uploadNewArena(const QString &hero);
    void checkArenaCurrentReload();
    void uploadArenaCards();

signals:
    void loadedGameResult(GameResult gameResult);
    void loadedArena(QString hero);
    void reloadedGameResult(GameResult gameResult);
    void reloadedArena(QString hero);
    void synchronized();
    void noArenaFound();
    void sendLog(QString line);
    void newDeckCard(QString card, int total);
    void resetDeckCardList();

public slots:
    void replyFinished(QNetworkReply *reply);

};

#endif // WEBUPLOADER_H
