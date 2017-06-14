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


QString ThemeHandler::borderApp(bool transparent)
{
    QString borderWidth;
    QString borderImage;

    if(themeBlack)
    {

    }
    else
    {
        borderWidth = "12";
        borderImage = "url(./border2.png)";
    }

    if(borderWidth == 0 || borderImage.isEmpty())   return "";
    else
    {
        if(transparent)     return "border-image: url(./transparent.png) " + borderWidth + "; border-width: " + borderWidth + ";";
        else                return "border-image: " + borderImage + " " + borderWidth + "; border-width: " + borderWidth + ";";
    }
}


int ThemeHandler::borderWidth()
{
    if(themeBlack)  return 0;
    else            return 12;
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
    else            return "background-color: rgba(0,0,0,100);";
}


void ThemeHandler::loadTheme(bool themeBlack)
{
    ThemeHandler::themeBlack = themeBlack;
}
