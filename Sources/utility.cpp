#include "utility.h"
#include <QtWidgets>

QMap<QString, QJsonObject> * Utility::enCardsJson;
QMap<QString, QJsonObject> * Utility::cardsJson;

Utility::Utility()
{

}

Utility::~Utility()
{

}


QString Utility::heroToLogNumber(const QString &hero)
{
    if(hero.compare("Druid")==0)        return QString("06");
    else if(hero.compare("Hunter")==0)  return QString("05");
    else if(hero.compare("Mage")==0)    return QString("08");
    else if(hero.compare("Paladin")==0) return QString("04");
    else if(hero.compare("Priest")==0)  return QString("09");
    else if(hero.compare("Rogue")==0)   return QString("03");
    else if(hero.compare("Shaman")==0)  return QString("02");
    else if(hero.compare("Warlock")==0) return QString("07");
    else if(hero.compare("Warrior")==0) return QString("01");
    else return QString();
}


//Return info about heroes in alphabetical order
QString Utility::getHeroColor(int order)
{
    QString heroesColors[10] = {"#FF7D0A", "#ABD473", "#69CCF0", "#F58CBA", "#FFFFFF", "#FFF569", "#0070DE", "#9482C9", "#C79C6E", "#A9A9A9"};
    return heroesColors[order];
}


QString Utility::getHeroName(int order)
{
    QString heroes[9] = {"Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior"};
    return heroes[order];
}


QString Utility::getHeroLogNumber(int order)
{
    QString heroesLogNumber[9] = {"06", "05", "08", "04", "09", "03", "02", "07", "01"};
    return heroesLogNumber[order];
}


QString Utility::cardEnNameFromCode(QString code)
{
    return (*enCardsJson)[code].value("name").toString();
}


QString Utility::cardEnCodeFromName(QString name)
{
    for (QMap<QString, QJsonObject>::const_iterator it = enCardsJson->cbegin(); it != enCardsJson->cend(); it++)
    {
        if(it->value("name").toString() == name)
        {
            if(!it->value("cost").isUndefined())    return it.key();
        }
    }

    return "";
}


QString Utility::cardLocalCodeFromName(QString name)
{
    for (QMap<QString, QJsonObject>::const_iterator it = cardsJson->cbegin(); it != cardsJson->cend(); it++)
    {
        if(it->value("name").toString() == name)
        {
            if(!it->value("cost").isUndefined())    return it.key();
        }
    }

    return "";
}


QString Utility::appPath()
{
    QString dirPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir dir(dirPath);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.absolutePath();
#else
    return dirPath;
#endif
}


void Utility::setEnCardsJson(QMap<QString, QJsonObject> *enCardsJson)
{
    Utility::enCardsJson = enCardsJson;
}

void Utility::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    Utility::cardsJson = cardsJson;
}
