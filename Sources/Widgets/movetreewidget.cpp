#include "movetreewidget.h"
#include <QtWidgets>

MoveTreeWidget::MoveTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    QFont font("Belwe Bd BT", 14);
    font.setBold(true);
    font.setKerning(true);
    this->setFont(font);
    this->setFrameShape(QFrame::NoFrame);
    this->setStyleSheet("background-color: transparent; outline: 0;");
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
