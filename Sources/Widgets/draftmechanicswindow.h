#ifndef DRAFTMECHANICSWINDOW_H
#define DRAFTMECHANICSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"
#include "lavabutton.h"
#include "../Synergies/draftitemcounter.h"


class DraftMechanicsWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, bool patreonVersion, bool draftMethodHA, bool draftMethodLF, bool normalizedLF);
    ~DraftMechanicsWindow();


//Variables
private:
    bool patreonVersion;
    LavaButton *lavaButton;
    ScoreButton *scoreButtonLF, *scoreButtonHA;
    HoverLabel *helpMark;
    DraftItemCounter **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;
    int scoreWidth;
    bool showingHelp;
    bool draftMethodHA, draftMethodLF;

//Metodos
private:
    void deleteDraftItemCounters();

public:
    void setDraftMethod(bool draftMethodHA, bool draftMethodLF);
    void setScores(int deckScoreHA, int deckScoreLF);
    void updateManaCounter(int numIncrease, int numCards);
    void updateCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList, QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList, QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList);
    void updateDeckWeight(int numCards, int draw, int toYourHand, int discover);
    void setTheme();
    void setNormalizedLF(bool value);

public slots:
    void sendItemEnter(QList<DeckCard> &deckCardList, QRect &labelRect);

private slots:
    void showHelp();
    void hideHelp();

signals:
    void itemEnter(QList<DeckCard> &deckCardList, QPoint &originList, int maxLeft, int maxRight);
    void itemLeave();
    void showPremiumDialog();
};

#endif // DRAFTMECHANICSWINDOW_H
