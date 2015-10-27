#include "movetreewidget.h"
#include <QtWidgets>

MoveTreeWidget::MoveTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    this->setFrameShape(QFrame::NoFrame);


//ARENA
//    <property name="sizePolicy">
//     <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
//      <horstretch>0</horstretch>
//      <verstretch>0</verstretch>
//     </sizepolicy>
//    </property>
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    <property name="font">
//     <font>
//      <family>Belwe Bd BT</family>
//      <pointsize>14</pointsize>
//      <weight>50</weight>
//      <bold>false</bold>
//      <kerning>false</kerning>
//     </font>
//    </property>
    QFont font("Belwe Bd BT", 14, 50);
    this->setFont(font);

//    <property name="horizontalScrollBarPolicy">
//     <enum>Qt::ScrollBarAsNeeded</enum>
//    </property>
//    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    <property name="sizeAdjustPolicy">
//     <enum>QAbstractScrollArea::AdjustIgnored</enum>
//    </property>
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    <property name="selectionMode">
//     <enum>QAbstractItemView::NoSelection</enum>
//    </property>
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
//    <property name="animated">
//     <bool>true</bool>
//    </property>
    this->setAnimated(true);
//    <property name="headerHidden">
//     <bool>true</bool>
//    </property>
    this->setHeaderHidden(true);
    this->header()->close();
//    <attribute name="headerDefaultSectionSize">
//     <number>0</number>
//    </attribute>
//    <attribute name="headerMinimumSectionSize">
//     <number>0</number>
//    </attribute>



//SECRETS
//    <property name="enabled">
//     <bool>true</bool>
//    </property>
//    <property name="sizePolicy">
//     <sizepolicy hsizetype="Expanding" vsizetype="Preferred">
//      <horstretch>0</horstretch>
//      <verstretch>0</verstretch>
//     </sizepolicy>
//    </property>

//    <property name="minimumSize">
//     <size>
//      <width>0</width>
//      <height>0</height>
//     </size>
//    </property>

//    <property name="horizontalScrollBarPolicy">
//     <enum>Qt::ScrollBarAlwaysOff</enum>
//    </property>

//    <property name="selectionMode">
//     <enum>QAbstractItemView::NoSelection</enum>
//    </property>

//    <property name="indentation">
//     <number>5</number>
//    </property>

//    <property name="itemsExpandable">
//     <bool>false</bool>
//    </property>

//    <property name="headerHidden">
//     <bool>true</bool>
//    </property>

//    <property name="expandsOnDoubleClick">
//     <bool>false</bool>
//    </property>
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
