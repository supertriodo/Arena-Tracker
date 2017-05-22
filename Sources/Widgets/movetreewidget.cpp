#include "movetreewidget.h"
#include <QtWidgets>

MoveTreeWidget::MoveTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    QFont font("Belwe Bd BT");
    font.setPixelSize(20);
    font.setBold(true);
    font.setKerning(true);
    this->setFont(font);
    this->setFrameShape(QFrame::NoFrame);
    this->setStyleSheet("QTreeView{background-color: transparent; outline: 0;}"

    "QTreeView::branch:has-children:!has-siblings:closed,"
    "QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(:/Images/branchClosed.png);}"

    "QTreeView::branch:open:has-children:!has-siblings,"
    "QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(:/Images/branchOpen.png);}");

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAnimated(true);
    this->setHeaderHidden(true);
    this->header()->close();
}


void MoveTreeWidget::setTheme(Theme theme)
{
    this->setStyleSheet("QTreeView{background-color: " + (theme==ThemeBlack?"black":WHITY_H) + "; outline: 0;}"

    "QTreeView::branch:has-children:!has-siblings:closed,"
    "QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(:/Images/branchClosed.png);}"

    "QTreeView::branch:open:has-children:!has-siblings,"
    "QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(:/Images/branchOpen.png);}");
}


void MoveTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTreeWidget::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTreeWidget::mouseMoveEvent(event);
    event->ignore();
}
void MoveTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
    event->ignore();
}
void MoveTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);
    event->ignore();
}


void MoveTreeWidget::leaveEvent(QEvent * e)
{
    QTreeWidget::leaveEvent(e);
    emit leave();
}
