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

//Override events
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void progressiveZoom();
};

#endif // PLANGRAPHICSVIEW_H
