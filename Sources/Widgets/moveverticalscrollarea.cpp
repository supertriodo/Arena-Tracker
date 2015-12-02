#include <QtWidgets>

#include "moveverticalscrollarea.h"

MoveVerticalScrollArea::MoveVerticalScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setStyleSheet("QScrollArea { background: transparent; }"
                  "QScrollArea > QWidget > QWidget { background: transparent; }");
    //             QScrollArea > ViewPort > WidgetContents
    setFrameShape(QFrame::NoFrame);
}


void MoveVerticalScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);

    if(!verticalScrollBar()->isVisible())
    {
        widget()->setMaximumWidth(this->width());
        widget()->setMinimumWidth(this->width());
    }
    else
    {
        widget()->setMaximumWidth(this->width() - verticalScrollBar()->width());
        widget()->setMinimumWidth(this->width() - verticalScrollBar()->width());
    }
}


void MoveVerticalScrollArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    QScrollArea::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveVerticalScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    QScrollArea::mouseMoveEvent(event);
    event->ignore();
}
void MoveVerticalScrollArea::mousePressEvent(QMouseEvent *event)
{
    QScrollArea::mousePressEvent(event);
    event->ignore();
}
void MoveVerticalScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
    QScrollArea::mouseReleaseEvent(event);
    event->ignore();
}
