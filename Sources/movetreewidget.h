#ifndef MOVETREEWIDGET_H
#define MOVETREEWIDGET_H


#include <QObject>
#include <QTreeWidget>

class MoveTreeWidget : public QTreeWidget
{
public:
    MoveTreeWidget(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // MOVETREEWIDGET_H
