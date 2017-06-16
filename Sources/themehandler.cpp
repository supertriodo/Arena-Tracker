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
QString ThemeHandler::bgTabsColor_, ThemeHandler::hoverTabsColor_, ThemeHandler::selectedTabsColor_;
QString ThemeHandler::bgTopButtonsColor_, ThemeHandler::hoverTopButtonsColor_;
int ThemeHandler::borderMenuWidth_;
QString ThemeHandler::fgMenuColor_, ThemeHandler::bgMenu_, ThemeHandler::borderMenu_;
QString ThemeHandler::borderItemMenuColor_, ThemeHandler::bgSelectedItemMenuColor_, ThemeHandler::fgSelectedItemMenuColor_;

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


QString ThemeHandler::bgTabsColor()
{
    return bgTabsColor_;
}

QString ThemeHandler::hoverTabsColor()
{
    return hoverTabsColor_;
}


QString ThemeHandler::selectedTabsColor()
{
    return selectedTabsColor_;
}


QString ThemeHandler::bgTopButtonsColor()
{
    return bgTopButtonsColor_;
}

QString ThemeHandler::hoverTopButtonsColor()
{
    return hoverTopButtonsColor_;
}


QString ThemeHandler::fgMenuColor()
{
    return fgMenuColor_;
}


QString ThemeHandler::borderItemMenuColor()
{
    return borderItemMenuColor_;
}


QString ThemeHandler::bgSelectedItemMenuColor()
{
    return bgSelectedItemMenuColor_;
}


QString ThemeHandler::fgSelectedItemMenuColor()
{
    return fgSelectedItemMenuColor_;
}


QString ThemeHandler::bgMenu()
{
    return bgMenu_;
}


QString ThemeHandler::borderMenu()
{
    return borderMenu_;
}


void ThemeHandler::reset()
{
    bgApp_ = borderApp_ = borderTransparent_;
    borderWidth_ = 0;
    fgColor_ = themeColor1_ = themeColor2_ = bgWidgets_ = "";
    bgTabsColor_ = hoverTabsColor_ = selectedTabsColor_ = "";
    bgTopButtonsColor_ = hoverTopButtonsColor_ = "";
    borderMenuWidth_ = 0;
    fgMenuColor_ = bgMenu_ = borderMenu_ = "";
    borderItemMenuColor_ = bgSelectedItemMenuColor_ = fgSelectedItemMenuColor_ = "";
}


void ThemeHandler::loadTheme(bool themeBlack)
{
    reset();

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
        hoverTabsColor_ = "rgba(0,0,0,100)";
        selectedTabsColor_ = "red";
        bgTabsColor_ = "blue";
        bgTopButtonsColor_ = "rgba(0,0,0,100)";
        hoverTopButtonsColor_ = "yellow";
        borderItemMenuColor_ = "blue";
        bgSelectedItemMenuColor_ = "yellow";
        fgSelectedItemMenuColor_ = "black";
        bgMenu_ = "pink";
        borderMenu_ = "grey";
        borderMenuWidth_ = 10;
    }


    //Background
    if(bgApp_.isEmpty())
    {
        bgApp_ = "background-color: black;";
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


    //Background Menu
    if(bgMenu_.isEmpty())
    {
        bgMenu_ = bgApp_;
    }
    else if(bgMenu_.contains("."))
    {
        bgMenu_ = "background-image: url(./" + bgMenu_ + ");";
    }
    else
    {
        bgMenu_ = "background-color: " + bgMenu_ + ";";
    }


    //Border Menu
    QString borderMenuWidthS = QString::number(borderMenuWidth_);
    if(borderMenu_.isEmpty())
    {
        borderMenuWidth_ = 0;
        borderMenu_ = "border-color: transparent;";
    }
    else if(borderMenu_.contains("."))
    {
        borderMenu_ = "border-image: url(./" + borderMenu_ + ") " + borderMenuWidthS + "; border-width: " + borderMenuWidthS + "px;";
    }
    else
    {
        borderMenu_ = "border-color: " + borderMenu_ + "; border-width: " + borderMenuWidthS + "px; border-style: solid;";
    }

    //Default values
    if(fgColor_.isEmpty())              fgColor_ = "white";
    if(themeColor1_.isEmpty())          themeColor1_ = "transparent";
    if(themeColor2_.isEmpty())          themeColor2_ = "transparent";
    if(hoverTabsColor_.isEmpty())       hoverTabsColor_ = "transparent";
    if(selectedTabsColor_.isEmpty())    selectedTabsColor_ = "transparent";
    if(bgTabsColor_.isEmpty())          bgTabsColor_ = themeColor1_;
    if(bgTopButtonsColor_.isEmpty())    bgTopButtonsColor_ = "transparent";
    if(hoverTopButtonsColor_.isEmpty()) hoverTopButtonsColor_ = themeColor1_;
    if(fgMenuColor_.isEmpty())          fgMenuColor_ = fgColor_;
    if(borderItemMenuColor_.isEmpty())  borderItemMenuColor_ = "transparent";
    if(bgSelectedItemMenuColor_.isEmpty())  bgSelectedItemMenuColor_ = themeColor1_;
    if(fgSelectedItemMenuColor_.isEmpty())  fgSelectedItemMenuColor_ = fgColor_;
}
