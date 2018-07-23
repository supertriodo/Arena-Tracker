#ifndef MOVETREEWIDGET_H
#define MOVETREEWIDGET_H


#include "../utility.h"
#include <QObject>
#include <QTreeWidget>

class MoveTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    MoveTreeWidget(QWidget *parent = nullptr);
    void setTheme(bool standAlone);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void leave();
};

#endif // MOVETREEWIDGET_H
