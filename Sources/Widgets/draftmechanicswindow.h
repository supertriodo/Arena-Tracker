#ifndef DRAFTMECHANICSWINDOW_H
#define DRAFTMECHANICSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"
#include "../Synergies/draftitemcounter.h"


class DraftMechanicsWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex);
    ~DraftMechanicsWindow();


//Variables
private:
    ScoreButton *scoresPushButton;
    ScoreButton *scoresPushButton2;
    DraftItemCounter **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;

//Metodos
private:
    void deleteDraftItemCounters();

public:
    void setDraftMethod(DraftMethod draftMethod);
    void setScores(int deckScoreHA, int deckScoreLF);
    void updateManaCounter(int numIncrease, int draftedCardsCount);
    void clearLists();
    void updateCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList, QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList, QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList);
};

#endif // DRAFTMECHANICSWINDOW_H
