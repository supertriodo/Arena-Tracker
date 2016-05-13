#ifndef PLANGRAPHICSVIEW_H
#define PLANGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>

class PlanGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    PlanGraphicsView(QWidget *parent = 0);

//Variables

//Metodos
public:
    void updateView(int minionsZone);

//Override events
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif // PLANGRAPHICSVIEW_H
