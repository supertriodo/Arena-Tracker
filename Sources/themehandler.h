#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <QString>

class ThemeHandler
{
public:
    ThemeHandler();


private:
    static bool themeBlack;

public:
    static QString bgApp();
    static QString borderApp(bool transparent);
    static int borderWidth();
    static QString fgColor();
    static QString themeColor1();
    static QString themeColor2();
    static void loadTheme(bool themeBlack);
    static QString bgWidgets();
};

#endif // THEMEHANDLER_H
