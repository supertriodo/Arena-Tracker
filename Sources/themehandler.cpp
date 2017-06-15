#include "themehandler.h"
#include <QtWidgets>

QString ThemeHandler::bgApp_;
QString ThemeHandler::borderApp_;
QString ThemeHandler::borderTransparent_;
int ThemeHandler::borderWidth_;
QString ThemeHandler::fgColor_;
QString ThemeHandler::themeColor1_;
QString ThemeHandler::themeColor2_;
QString ThemeHandler::bgWidgets_;

ThemeHandler::ThemeHandler()
{

}


QString ThemeHandler::bgApp()
{
    return bgApp_;
}


QString ThemeHandler::borderApp(bool transparent)
{
    if(transparent)     return borderTransparent_;
    else                return borderApp_;
}


int ThemeHandler::borderWidth()
{
    return borderWidth_;
}


QString ThemeHandler::fgColor()
{
    return fgColor_;
}


QString ThemeHandler::themeColor1()
{
    return themeColor1_;
}


QString ThemeHandler::themeColor2()
{
    return themeColor2_;
}


QString ThemeHandler::bgWidgets()
{
    return bgWidgets_;
}


void ThemeHandler::loadTheme(bool themeBlack)
{
    if(themeBlack)
    {
        bgApp_ = "black";
        borderWidth_ = 0;
        borderApp_ = "";
        bgWidgets_ = "";
        fgColor_ = "white";
        themeColor1_ = "#0F4F0F";
        themeColor2_ = "#32AF32";
    }
    else
    {
        bgApp_ = "fondo2.jpg";
        borderWidth_ = 12;
        borderApp_ = "border2.png";
        bgWidgets_ = "rgba(0,0,0,100)";
        fgColor_ = "white";
        themeColor1_ = "purple";
        themeColor2_ = "red";
    }


    //Background
    if(bgApp_.isEmpty())
    {
        bgApp_ = "background-color: transparent;";
    }
    else if(bgApp_.contains("."))
    {
        bgApp_ = "background-image: url(./" + bgApp_ + ");";
    }
    else
    {
        bgApp_ = "background-color: " + bgApp_ + ";";
    }


    //Border
    QString borderWidthS = QString::number(borderWidth_);
    borderTransparent_ = "border-color: transparent; border-width: " + borderWidthS + "px; border-style: solid;";
    if(borderApp_.isEmpty())
    {
        borderWidth_ = 0;
        borderApp_ = borderTransparent_ = "border-color: transparent;";
    }
    else if(borderApp_.contains("."))
    {
        borderApp_ = "border-image: url(./" + borderApp_ + ") " + borderWidthS + "; border-width: " + borderWidthS + "px;";
    }
    else
    {
        borderApp_ = "border-color: " + borderApp_ + "; border-width: " + borderWidthS + "px; border-style: solid;";
    }


    //Background widgets
    if(bgWidgets_.isEmpty())
    {
        bgWidgets_ = "background-color: transparent;";
    }
    else
    {
        bgWidgets_ = "background-color: " + bgWidgets_ + ";";
    }
}
