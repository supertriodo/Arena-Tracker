#include "movetreewidget.h"
#include "../themehandler.h"
#include <QtWidgets>

MoveTreeWidget::MoveTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    setTheme(false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->setHeaderHidden(true);
    this->header()->close();
}


void MoveTreeWidget::setTheme(bool standAlone)
{
    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(20);
    this->setFont(font);

    this->setStyleSheet("QTreeView{" + (standAlone?ThemeHandler::bgDecks()+ThemeHandler::borderDecks():ThemeHandler::bgWidgets()) + "; outline: 0;}"

    "QTreeView::branch:has-children:!has-siblings:closed,"
    "QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(:/Images/branchClosed.png);}"

    "QTreeView::branch:open:has-children:!has-siblings,"
    "QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(:/Images/branchOpen.png);}"

    "QTreeView::item:selected {background: " + ThemeHandler::bgSelectedItemListColor() + "; color: " + ThemeHandler::fgSelectedItemListColor() + ";}"
    "QTreeView::branch:selected {background-color: " + ThemeHandler::bgSelectedItemListColor() + "; }"
    );

    this->setAnimated(!standAlone);
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
