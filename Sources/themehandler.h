#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <QString>

class ThemeHandler
{
public:
    ThemeHandler();


private:
    static QString bgApp_;
    static QString borderApp_;
    static QString borderTransparent_;
    static int borderWidth_;
    static QString fgColor_;
    static QString themeColor1_;
    static QString themeColor2_;
    static QString bgWidgets_;

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
