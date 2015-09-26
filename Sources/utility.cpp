#include "utility.h"
#include <QtWidgets>

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


QString Utility::appPath()
{
    QString dirPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir dir(dirPath);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.absolutePath()
#else
    return dirPath;
#endif
}
