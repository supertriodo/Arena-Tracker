#include "resizebutton.h"
#include <QtWidgets>

ResizeButton::ResizeButton(QWidget *parent) : QPushButton(parent)
{
    mainWindow = parent;

    this->setIconSize(QSize(24,24));
    this->setFixedSize(QSize(26,26));
}


ResizeButton::~ResizeButton()
{

}


void ResizeButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = mainWindow->frameGeometry().bottomRight() - event->globalPos();
        event->accept();
    }
}


void ResizeButton::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint topLeft = mainWindow->frameGeometry().topLeft();
        QPoint newBottomRight = event->globalPos() + dragPosition;
        QPoint midApp = mainWindow->frameGeometry().center();

        foreach (QScreen *screen, QGuiApplication::screens())
        {
            if(screen->geometry().contains(midApp))
            {
                int maxRight = screen->geometry().right();
                int maxBottom = screen->geometry().bottom();
                if(newBottomRight.y()>maxBottom)    newBottomRight.setY(maxBottom);
                if(newBottomRight.x()>maxRight)     newBottomRight.setX(maxRight);
                break;
            }
        }

        QSize size(newBottomRight.x() - topLeft.x(), newBottomRight.y() - topLeft.y());
        emit newSize(size);
        event->accept();
    }
}


void ResizeButton::leaveEvent(QEvent * e)
{
    QPushButton::leaveEvent(e);
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}


void ResizeButton::enterEvent(QEvent * e)
{
    QPushButton::enterEvent(e);
    QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
}

