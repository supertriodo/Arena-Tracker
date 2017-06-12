#ifndef BOMBWINDOW_H
#define BOMBWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "plangraphicsview.h"
#include "../utility.h"

class BombWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    BombWindow(QWidget *parent, QGraphicsScene *graphicsScene);
    ~BombWindow();

//Variables
private:
    PlanGraphicsView * graphicsView;

//Metodos
public:
    void setTheme();
    void showAt(QRect rectTab);

signals:

public slots:
};

#endif // BOMBWINDOW_H
