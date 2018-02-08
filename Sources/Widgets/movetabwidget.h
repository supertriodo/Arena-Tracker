#ifndef MOVETABWIDGET_H
#define MOVETABWIDGET_H


#include <QTabWidget>
#include <QTabBar>
#include "../utility.h"

class MoveTabWidget : public QTabWidget
{
    Q_OBJECT

private:
    int tabSize;

public:
    MoveTabWidget(QWidget *parent = 0);
    void setTheme(QString tabBarAlignment, int maxWidth, bool resizing, bool transparent);

signals:
    void detachTab(int index, const QPoint& dropPoint);
};


class MoveTabBar : public QTabBar
{
    Q_OBJECT

private:
    QPoint dragStartPos/*, dragLastPos*/;

public:
    MoveTabBar(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

signals:
    void detachTab(int index, const QPoint& dropPoint);
};

#endif // MOVETABWIDGET_H
