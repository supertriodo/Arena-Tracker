#include "movetabwidget.h"
#include "../themehandler.h"
#include <QtWidgets>

MoveTabWidget::MoveTabWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setTabBar(new MoveTabBar(this));
    this->hide();
    this->tabSize = 32;
}


void MoveTabWidget::setTheme(QString tabBarAlignment, int maxWidth, bool resizing, bool transparent)
{
    QString bgTabsColor = ThemeHandler::bgTabsColor();
    QString hoverTabsColor = ThemeHandler::hoverTabsColor();
    QString selectedTabsColor = ThemeHandler::selectedTabsColor();
    QString borderWidth = "";
    if(ThemeHandler::borderWidth() > 0) borderWidth = QString::number(ThemeHandler::borderWidth());
    int tabSize = std::max(24, std::min(32, maxWidth/std::max(1, this->count()) - 14));

    if(resizing && (tabSize == this->tabSize)) return;

    QString css =
        "QTabBar::tab:selected {background: " + selectedTabsColor + "; border-bottom-color: " + selectedTabsColor + ";}"
        "QTabBar::tab:hover {background: " + hoverTabsColor + "; border-bottom-color: " + hoverTabsColor + ";}"
        "QTabWidget::pane { " + ThemeHandler::bgApp() + ThemeHandler::borderApp(transparent) +
            "position: absolute; top: -38px;}"
        "QTabBar::tab {border: 2px solid " + bgTabsColor + ";"
            "padding: 5px;background: " + bgTabsColor +
            "; height: 24px; width: " + QString::number(tabSize) + "px}";

    if(borderWidth.isEmpty())   css +=  "QTabWidget::tab-bar {alignment: " + tabBarAlignment + ";}";
    else                        css +=  "QTabWidget::tab-bar {left: " + borderWidth + "px; "
                                            "top: " + borderWidth + "px;}";

    this->setStyleSheet(css);

    if(tabSize != this->tabSize)    tabBar()->setIconSize(QSize(tabSize, tabSize));
    this->tabSize = tabSize;
}



MoveTabBar::MoveTabBar(QWidget *parent) : QTabBar(parent)
{
    this->setIconSize(QSize(32,32));
}

void MoveTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTabBar::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveTabBar::mouseMoveEvent(QMouseEvent *event)
{
    QTabBar::mouseMoveEvent(event);
    event->ignore();
}
void MoveTabBar::mousePressEvent(QMouseEvent *event)
{
    QTabBar::mousePressEvent(event);
    event->ignore();

}
void MoveTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    QTabBar::mouseReleaseEvent(event);
    event->ignore();
}

