#ifndef DRAFTMECHANICSWINDOW_H
#define DRAFTMECHANICSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"
#include "lavabutton.h"
#include "../Synergies/draftitemcounter.h"
#include "../Synergies/draftdropcounter.h"

#define M_NUM_TYPES 3


class DraftMechanicsWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, bool patreonVersion);
    ~DraftMechanicsWindow();


//Variables
private:
    bool patreonVersion;
    LavaButton *lavaButton;
    ScoreButton *scoreButtonLF, *scoreButtonHA, *scoreButtonHSR;
    HoverLabel *helpMark;
    DraftItemCounter **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;
    DraftDropCounter **dropCounters;
    int scoreWidth;
    bool showingHelp, showDrops;
    DraftMethod draftMethodAvgScore;

//Metodos
private:
    void deleteDraftItemCounters();
    void updateManaCounter(int manaIncrease, int numCards);

public:
    void setDraftMethodAvgScore(DraftMethod draftMethodAvgScore);
    void setShowDrops(bool value);
    void setScores(int deckScoreHA, int deckScoreLF, float deckScoreHSR);
    void updateCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                        QStringList &drop2List, QStringList &drop3List, QStringList &drop4List,
                        QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                        QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList, int manaIncrease, int numCards);
    void updateDeckWeight(int numCards, int draw, int toYourHand, int discover);
    void setTheme();
    int draftedCardsCount();

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
