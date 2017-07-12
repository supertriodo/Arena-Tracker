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
    ScoreButton *scoresPushButton2[3];
    MoveListWidget *synergiesListWidget[3];
    QList<DeckCard> synergiesDeckCardLists[3];
    int scoreWidth;
    int maxSynergyHeight;


//Metodos
private:
    int getCard(QString &name, QString &code);
    void resizeSynergyList();

public:
    void setScores(double rating1, double rating2, double rating3, QString synergy1, QString synergy2, QString synergy3, DraftMethod draftMethod);
    void hideScores();
    void setLearningMode(bool value);
    void setDraftMethod(DraftMethod draftMethod);
    void redrawSynergyCards();

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void cardLeave();

private slots:
    void hideSynergies(int index);
    void showSynergies();
    void findSynergyCardEntered(QListWidgetItem *item);
    void spreadLearningShow(bool value);
};

#endif // DRAFTSCOREWINDOW_H
