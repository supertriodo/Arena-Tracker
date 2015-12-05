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
    QList<DeckCard> synergiesDeckCardLists[3];
    int scoreWidth;
    int maxSynergyHeight;


//Metodos
private:
    int getCard(QString &name, QString &code);

public:
    void setScores(double rating1, double rating2, double rating3, QString synergy1, QString synergy2, QString synergy3);
    void hideScores();
    void setLearningMode(bool value);

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void cardLeave();

private slots:
    void showSynergies(int index);
    void hideSynergies(int index);
    void showSynergies();
    void findSynergyCardEntered(QListWidgetItem *item);
};

#endif // DRAFTSCOREWINDOW_H
