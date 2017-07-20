#include "draftitemcounter.h"
#include <QtWidgets>

DraftItemCounter::DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap) : QObject(parent)
{
    labelIcon.setPixmap(pixmap);
    labelCounter.setFixedWidth(10);
    hLayout->addWidget(&labelIcon);
    hLayout->addWidget(&labelCounter);

    reset();
}


void DraftItemCounter::reset()
{
    this->counter = 0;
    labelCounter.setText("0");
    labelIcon.setHidden(true);
    labelCounter.setHidden(true);
}


void DraftItemCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    if(!mouseInApp && transparency == Transparent)
    {
        labelIcon.setStyleSheet("QLabel {background-color: transparent; color: white;}");
        labelCounter.setStyleSheet("QLabel {background-color: transparent; color: white;}");
    }
    else
    {
        labelIcon.setStyleSheet("");
        labelCounter.setStyleSheet("");
    }
}


bool DraftItemCounter::increase()
{
    this->counter++;
    labelCounter.setText(QString::number(counter));
    if(counter == 1)
    {
        labelIcon.setHidden(false);
        labelCounter.setHidden(false);
        return true; //Returns true for the first element.
    }
    return false;
}


bool DraftItemCounter::isHidden()
{
    return counter == 0;
}
