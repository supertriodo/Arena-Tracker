#ifndef RESIZEBUTTON_H
#define RESIZEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>


class ResizeButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ResizeButton(QWidget *parent = 0);
    ~ResizeButton();


//Variables
private:
    QPoint dragPosition;
    QWidget *mainWindow;

//Override events
protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void newSize(QSize size);
};

#endif // RESIZEBUTTON_H
