#include "themehandler.h"
#include <QtWidgets>

bool ThemeHandler::themeBlack;

ThemeHandler::ThemeHandler()
{

}


QString ThemeHandler::bgColor()
{
    if(themeBlack)  return "black";
    else            return "yellow";
}


QString ThemeHandler::fgColor()
{
    if(themeBlack)  return "white";
    else            return "black";
}


QString ThemeHandler::themeColor1()
{
//#define DARK_GREEN_H QString("#0F4F0F")
//    return QString("#0F4F0F");
    if(themeBlack)  return QString("#0F4F0F");
    else            return "orange";
}


QString ThemeHandler::themeColor2()
{
//#define GREEN_H QString("#32AF32")
//    return QString("#32AF32");
    if(themeBlack)  return QString("#32AF32");
    else            return "red";
}


void ThemeHandler::loadTheme(bool themeBlack)
{
    ThemeHandler::themeBlack = themeBlack;
}
