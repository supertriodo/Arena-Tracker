#ifndef MOVELISTWIDGET_H
#define MOVELISTWIDGET_H

#include <QObject>
#include <QListWidget>

class MoveListWidget : public QListWidget
{
public:
    MoveListWidget(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MOVELISTWIDGET_H
