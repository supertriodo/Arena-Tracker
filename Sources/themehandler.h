#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <QString>

class ThemeHandler
{
public:
    ThemeHandler();


//Variables
private:
    static QString bgApp_;
    static QString borderApp_;
    static QString borderTransparent_;
    static int borderWidth_;
    static QString fgColor_;
    static QString themeColor1_;
    static QString themeColor2_;
    static QString bgWidgets_;
    static QString bgTabsColor_, hoverTabsColor_, selectedTabsColor_;


//Metodos
private:
    static void reset();

public:
    static QString bgApp();
    static QString borderApp(bool transparent);
    static int borderWidth();
    static QString fgColor();
    static QString themeColor1();
    static QString themeColor2();
    static void loadTheme(bool themeBlack);
    static QString bgWidgets();
    static QString bgTabsColor();
    static QString hoverTabsColor();
    static QString selectedTabsColor();
};

#endif // THEMEHANDLER_H
