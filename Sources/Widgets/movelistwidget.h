#ifndef MOVELISTWIDGET_H
#define MOVELISTWIDGET_H

#include <QObject>
#include <QListWidget>

class MoveListWidget : public QListWidget
{
public:
    MoveListWidget(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // MOVELISTWIDGET_H
