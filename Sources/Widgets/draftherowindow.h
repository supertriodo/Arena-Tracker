#ifndef DRAFTHEROWINDOW_H
#define DRAFTHEROWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QHBoxLayout>
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
    QHBoxLayout *horLayoutScores[3];
    QHBoxLayout *horLayoutScores2[3];
    ScoreButton *scoresPushButton[3];
    ScoreButton *scoresPlayerPushButton[3];
    TwitchButton *twitchButton[3];
    int scoreWidth;
    bool scores2Rows, showTwitch;


//Metodos
private:
    void resetTwitchScore();
    void checkScoresSpace();

public:
    void setScores(int classOrder[3]);
    void hideScores(bool quick=false);
    void showTwitchScores(bool show=true);
    void setTwitchScores(int vote1, int vote2, int vote3, QString username);

signals:
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftHeroWindow");
};

#endif // DRAFTHEROWINDOW_H
