#include "movetabwidget.h"
#include <QtWidgets>

MoveTabWidget::MoveTabWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setTabBar(new MoveTabBar(this));
    this->hide();
}


void MoveTabWidget::setTheme(Theme theme)
{
    QString foregroundColor = (theme==ThemeWhite)?"black":"white";
    QString backgroundColor = (theme==ThemeWhite)?"white":"black";
    QString borderColor = (theme==ThemeWhite)?"#C2C7CB":"#0F4F0F";
    this->setStyleSheet(
        "QTabBar::tab:selected {background: " + backgroundColor + ";border-bottom-color: " + backgroundColor + ";}"
        "QTabBar::tab:hover {background: " + backgroundColor + ";border-bottom-color: " + backgroundColor + ";}"
        "QTabWidget::tab-bar {alignment: center;}"
        "QTabWidget::pane {border-color: transparent; background: " + backgroundColor + ";}"
        "QTabWidget::pane {border-top: 2px solid " + borderColor + ";position: absolute;top: -0.75em;}"
        "QTabBar::tab {border: 3px solid " + borderColor + ";"
            "padding: 2px;background: " + borderColor + "; color: " + foregroundColor + ";}"
    );
}



MoveTabBar::MoveTabBar(QWidget *parent) : QTabBar(parent)
{
    QFont font("Sans Serif", 10);
    this->setFont(font);
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

