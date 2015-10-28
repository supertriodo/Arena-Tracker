#include "movetabwidget.h"
#include <QtWidgets>

MoveTabWidget::MoveTabWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setStyleSheet(
        "QTabBar::tab:selected {background: white;border-bottom-color: white;}"
        "QTabWidget::tab-bar {alignment: center;}"
        "QTabWidget::pane {border-color: transparent; background: white;}"
        "QTabWidget::pane {border-top: 2px solid #C2C7CB;position: absolute;top: -0.5em;}"
        "QTabBar::tab {border: 3px solid #C2C7CB;"
            "padding: 2px;background:#C2C7CB;}"
    );
    this->setTabBar(new MoveTabBar(this));
    this->setTabBarAutoHide(true);
    this->hide();
}


MoveTabBar::MoveTabBar(QWidget *parent) : QTabBar(parent)
{
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

