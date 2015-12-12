#include "movetabwidget.h"
#include <QtWidgets>

MoveTabWidget::MoveTabWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setTabBar(new MoveTabBar(this));
    this->hide();
}


void MoveTabWidget::setTheme(Theme theme, QString tabBarAlignment)
{
    QString foregroundColor = (theme==ThemeWhite)?"black":"white";
    QString backgroundColor = (theme==ThemeWhite)?"#F0F0F0":"black";
    QString borderColor = (theme==ThemeWhite)?"#0F4F0F":"#0F4F0F";

    this->setStyleSheet(
        "QTabBar::tab:selected {background: " + backgroundColor + ";border-bottom-color: " + backgroundColor + ";}"
        "QTabBar::tab:hover {background: " + backgroundColor + ";border-bottom-color: " + backgroundColor + ";}"
        "QTabWidget::tab-bar {alignment: " + tabBarAlignment + ";}"
        "QTabWidget::pane {border-color: transparent; background: " + backgroundColor + ";}"
        "QTabWidget::pane {position: absolute;top: -38px;}"
        "QTabBar::tab {border: 2px solid " + borderColor + ";"
            "padding: 5px;background: " + borderColor + "; color: " + foregroundColor + "; height: 24px; width: 32px}"
    );
}



MoveTabBar::MoveTabBar(QWidget *parent) : QTabBar(parent)
{
    QFont font("Sans Serif", 10);
    this->setFont(font);
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

