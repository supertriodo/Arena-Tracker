#include "themehandler.h"
#include <QtWidgets>

bool ThemeHandler::themeBlack;

ThemeHandler::ThemeHandler()
{

}


QString ThemeHandler::bgApp()
{
    if(themeBlack)  return "background-color: black;";
    else            return "background-image: url(./fondo2.jpg);";
}


QString ThemeHandler::fgColor()
{
    if(themeBlack)  return "white";
    else            return "white";
}


QString ThemeHandler::themeColor1()
{
//#define DARK_GREEN_H QString("#0F4F0F")
//    return QString("#0F4F0F");
    if(themeBlack)  return QString("#0F4F0F");
    else            return "purple";
}


QString ThemeHandler::themeColor2()
{
//#define GREEN_H QString("#32AF32")
//    return QString("#32AF32");
    if(themeBlack)  return QString("#32AF32");
    else            return "red";
}


QString ThemeHandler::bgWidgets()
{
    if(themeBlack)  return "background-color: transparent;";
    else            return "background-image: url(./trans50.png);";
}


void ThemeHandler::loadTheme(bool themeBlack)
{
    ThemeHandler::themeBlack = themeBlack;
}
