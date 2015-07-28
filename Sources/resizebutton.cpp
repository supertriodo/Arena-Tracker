#include "resizebutton.h"
#include <QtWidgets>

ResizeButton::ResizeButton(QWidget *parent) : QPushButton(parent)
{
    mainWindow = parent;

    this->setIconSize(QSize(24,24));
    this->setIcon(QIcon(":/Images/resize.png"));
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
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect rect = screen->geometry();

        QPoint topLeft = mainWindow->frameGeometry().topLeft();
        QPoint newBottomRight = event->globalPos() + dragPosition;
        if(newBottomRight.y()>rect.height()) newBottomRight.setY(rect.height());
        if(newBottomRight.x()>rect.width()) newBottomRight.setX(rect.width());

        QSize size(newBottomRight.x() - topLeft.x(), newBottomRight.y() - topLeft.y());
        emit newSize(size);
        event->accept();
    }
}
