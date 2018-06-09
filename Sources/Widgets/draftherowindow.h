#ifndef DRAFTHEROWINDOW_H
#define DRAFTHEROWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"


#define MARGIN 10

class DraftHeroWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftHeroWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex);
    ~DraftHeroWindow();

//Variables
private:
    ScoreButton *scoresPushButton[3];
    int scoreWidth;


//Metodos
private:

public:
    void setScores(double rating1, double rating2, double rating3);
    void hideScores(bool quick=false);
};

#endif // DRAFTHEROWINDOW_H
