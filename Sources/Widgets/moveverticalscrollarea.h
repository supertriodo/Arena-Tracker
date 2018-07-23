#ifndef MOVEVERTICALSCROLLAREA_H
#define MOVEVERTICALSCROLLAREA_H

#include <QScrollArea>

class MoveVerticalScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit MoveVerticalScrollArea(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // MOVEVERTICALSCROLLAREA_H
