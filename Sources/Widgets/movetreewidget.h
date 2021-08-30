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

//Variables
private:
    QList<int> editableColumns;

//Metodos
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;

public:
    void setEditableColumns(QList<int> cols);

signals:
    void xLeave();
    void leave();
};

#endif // MOVETREEWIDGET_H
