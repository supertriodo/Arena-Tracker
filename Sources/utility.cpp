#include "utility.h"
#include <QtWidgets>

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


QString Utility::cardEnNameFromCode(QString code)
{
    return (*cardsJson)[code].value("name").toString();
}


QString Utility::cardEnCodeFromName(QString name)
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


void Utility::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    Utility::cardsJson = cardsJson;
}
