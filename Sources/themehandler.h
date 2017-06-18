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
    static QString bgTopButtonsColor_, hoverTopButtonsColor_;
    static int borderMenuWidth_;
    static QString fgMenuColor_, bgMenu_, borderMenu_;
    static QString borderItemMenuColor_, bgSelectedItemMenuColor_, fgSelectedItemMenuColor_;
    static int borderDecksWidth_;
    static QString bgDecks_, borderDecks_;
    static QString bgSelectedItemListColor_, fgSelectedItemListColor_;
    static QString borderTooltipColor_, bgTooltipColor_, fgTooltipColor_;
    static QString borderProgressBarColor_, bgProgressBarColor_, fgProgressBarColor_, chunkProgressBarColor_;
    static QString borderLineEditColor_, bgLineEditColor_, fgLineEditColor_;
    static QString bgSelectionLineEditColor_, fgSelectionLineEditColor_;
    static QString defaultFont_, cardsFont_, bigFont_;
    static int cardsFontOffsetY_;


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
    static QString bgTopButtonsColor();
    static QString hoverTopButtonsColor();
    static QString fgMenuColor();
    static QString borderItemMenuColor();
    static QString bgSelectedItemMenuColor();
    static QString fgSelectedItemMenuColor();
    static QString bgMenu();
    static QString borderMenu();
    static QString bgDecks();
    static QString borderDecks();
    static QString bgSelectedItemListColor();
    static QString fgSelectedItemListColor();
    static QString borderTooltipColor();
    static QString bgTooltipColor();
    static QString fgTooltipColor();
    static QString borderProgressBarColor();
    static QString bgProgressBarColor();
    static QString fgProgressBarColor();
    static QString chunkProgressBarColor();
    static QString borderLineEditColor();
    static QString bgLineEditColor();
    static QString fgLineEditColor();
    static QString bgSelectionLineEditColor();
    static QString fgSelectionLineEditColor();
    static QString defaultFont();
    static QString cardsFont();
    static QString bigFont();
    static int cardsFontOffsetY();
};

#endif // THEMEHANDLER_H
