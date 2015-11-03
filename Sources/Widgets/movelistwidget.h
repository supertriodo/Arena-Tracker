#ifndef MOVELISTWIDGET_H
#define MOVELISTWIDGET_H

#include <QObject>
#include <QListWidget>

class MoveListWidget : public QListWidget
{
    Q_OBJECT

public:
    MoveListWidget(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void xLeave();
};

#endif // MOVELISTWIDGET_H
