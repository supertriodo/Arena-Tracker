#ifndef DRAFTHEROWINDOW_H
#define DRAFTHEROWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"
#include "twitchbutton.h"


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
    TwitchButton *twitchButton[3];
    int scoreWidth;


//Metodos
private:
    void resetTwitchScore();

public:
    void setScores(float rating1, float rating2, float rating3);
    void hideScores(bool quick=false);
    void showTwitchScores(bool show=true);
    void setTwitchScores(int vote1, int vote2, int vote3);
};

#endif // DRAFTHEROWINDOW_H
