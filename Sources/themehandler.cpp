#include "themehandler.h"
#include <QtWidgets>

ThemeHandler::ThemeHandler()
{

}


QString ThemeHandler::bgColor()
{
    return "white";
}


QString ThemeHandler::fgColor()
{
    return "black";
}


QString ThemeHandler::themeColor1()
{
//#define DARK_GREEN_H QString("#0F4F0F")
    return QString("#0F4F0F");
}


QString ThemeHandler::themeColor2()
{
//#define GREEN_H QString("#32AF32")
    return QString("#32AF32");
}
