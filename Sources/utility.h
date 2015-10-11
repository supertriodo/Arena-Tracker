#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QMap>
#include <QJsonObject>

#define ANIMATION_TIME 750

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Always, Auto, Never };

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
