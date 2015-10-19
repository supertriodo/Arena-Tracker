#include "movelistwidget.h"
#include <QtWidgets>

MoveListWidget::MoveListWidget(QWidget *parent) : QListWidget(parent)
{
//    <widget class="QListWidget" name="deckListWidget">
//     <property name="sizePolicy">
//      <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
//       <horstretch>0</horstretch>
//       <verstretch>0</verstretch>
//      </sizepolicy>
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     </property>
//     <property name="font">
//      <font>
//       <family>Belwe Bd BT</family>
//       <pointsize>16</pointsize>
//      </font>
//     </property>

//     <property name="horizontalScrollBarPolicy">
//      <enum>Qt::ScrollBarAlwaysOff</enum>
//     </property>
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     <property name="sizeAdjustPolicy">
//      <enum>QAbstractScrollArea::AdjustIgnored</enum>
//     </property>
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
//     <property name="selectionBehavior">
//      <enum>QAbstractItemView::SelectRows</enum>
//     </property>
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
//    <property name="selectionMode">
//     <enum>QAbstractItemView::NoSelection</enum>
//    </property>
    this->setSelectionMode(QAbstractItemView::NoSelection);
//    </widget>
}


void MoveListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListWidget::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);
    event->ignore();
}
void MoveListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    event->ignore();
}
void MoveListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidget::mouseReleaseEvent(event);
    event->ignore();
}
