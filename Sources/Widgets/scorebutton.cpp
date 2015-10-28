#include "scorebutton.h"
#include <QtWidgets>

ScoreButton::ScoreButton(QWidget *parent, int index) : QPushButton(parent)
{
    this->index = index;
    this->drawArrow = false;
    this->drawHLines = false;
}


void ScoreButton::leaveEvent(QEvent * e)
{
    setFlat(true);
    QPushButton::leaveEvent(e);
    emit leave(index);
}


void ScoreButton::enterEvent(QEvent * e)
{
    setFlat(false);
    QPushButton::enterEvent(e);
    emit enter(index);
}


void ScoreButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);

    QPainter painter(this);



    // Flecha
    if(drawArrow)
    {
        painter.setPen(QPen(Qt::black, width()/20));
        int ancho = width()/8;
        int inicioVertical = height()*3/4 + width()/50;
        int finalVertical = height()*7/8;
        QLine lines[3] = {
            {QLine(QPoint(width()/2,inicioVertical), QPoint(width()/2,finalVertical))}, //Vertical
            {QLine(QPoint(width()/2 - ancho,(inicioVertical+finalVertical)/2), QPoint(width()/2,finalVertical))}, //Izq
            {QLine(QPoint(width()/2 + ancho,(inicioVertical+finalVertical)/2), QPoint(width()/2,finalVertical))}  //Dcha
        };
        painter.drawLines(lines, 3);
    }

    //Lineas mejor
    if(drawHLines)
    {
        painter.setPen(QPen(Qt::black, width()/25));
        int inicioMejor = height()*5/16;
        int finalMejor = height()*12/16 - width()/50;
        QLine lines2[2] = {
            {QLine(QPoint(width()/20,inicioMejor), QPoint(width()-width()/20,inicioMejor))},
            {QLine(QPoint(width()/20,finalMejor), QPoint(width()-width()/20,finalMejor))}
        };
        painter.drawLines(lines2, 2);
    }
}


void ScoreButton::setDrawArrow(bool value)
{
    this->drawArrow = value;
}


void ScoreButton::setDrawHLines(bool value)
{
    this->drawHLines = value;
}



