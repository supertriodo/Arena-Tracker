#ifndef DRAFTSCOREWINDOW_H
#define DRAFTSCOREWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "movelistwidget.h"
#include "scorebutton.h"
#include "../utility.h"
#include "../Cards/deckcard.h"


#define MARGIN 10

class DraftScoreWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex);
    ~DraftScoreWindow();

//Variables
private:
    ScoreButton *scoresPushButton[3];
    MoveListWidget *synergiesListWidget[3];
    int scoreWidth;
    int maxSynergyHeight;


//Metodos
private:
    int getCard(QString &name, QString &code);

public:
    void setScores(double rating1, double rating2, double rating3, QString synergy1, QString synergy2, QString synergy3);
    void hideScores();

private slots:
    void showSynergies(int index);
    void hideSynergies(int index);
    void showSynergies();
};

#endif // DRAFTSCOREWINDOW_H
