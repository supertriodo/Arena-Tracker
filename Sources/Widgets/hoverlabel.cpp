#include "hoverlabel.h"
#include <QtWidgets>

HoverLabel::HoverLabel(QWidget * parent) : 	QLabel(parent)
{
    setAttribute(Qt::WA_Hover);
}

bool HoverLabel::event(QEvent *e)
{
    if(e->type() == QEvent::HoverEnter)
    {
        emit enter(this);
        return true;
    }
    else if(e->type() == QEvent::HoverLeave)
    {
        emit leave();
        return true;
    }
    else if(e->type() == QEvent::MouseButtonPress)
    {
        emit click(this);
        return true;
    }
//    else if(e->type() == QEvent::MouseButtonDblClick)
//    {
//        emit doubleClicked();
//        return true;
//    }

    return QLabel::event(e);
}
