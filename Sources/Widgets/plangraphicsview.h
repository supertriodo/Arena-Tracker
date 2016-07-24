#ifndef PLANGRAPHICSVIEW_H
#define PLANGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>

#define ZOOM_SPEED 0.005

class PlanGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    PlanGraphicsView(QWidget *parent = 0);

//Variables
private:
    float zoom;
    float targetZoom;

//Metodos
private:
    void fitInViewSmooth();

public:
    void updateView(int minionsZone);
    void reset();
    void removeAll();
    int getSceneViewWidth();
    int getCardsViewHeight();

//Override events
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void sizeChanged();
    void leave();

private slots:
    void progressiveZoom();
};

#endif // PLANGRAPHICSVIEW_H
