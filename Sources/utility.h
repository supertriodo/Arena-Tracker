#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QUrlQuery>
#include <QNetworkRequest>

#define ANIMATION_TIME 750

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Transparent, AutoTransparent, Opaque };
enum Theme { ThemeWhite, ThemeBlack };


class GameResultPost
{
public:
    QUrlQuery postData;
    QNetworkRequest request;
};


class Utility
{
//Constructor
public:
    Utility();
    ~Utility();

//Variables
private:
    static QMap<QString, QJsonObject> *cardsJson;


//Metodos
public:
    static QString heroToLogNumber(const QString &hero);
    static QString appPath();
    static QString cardEnNameFromCode(QString code);
    static QString cardEnCodeFromName(QString name);
    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
};

#endif // UTILITY_H
