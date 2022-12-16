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
    DraftMechanicsWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, bool patreonVersion, int classOrder);
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
    QGridLayout *mechanicsLayout;

//Metodos
private:
    void deleteDraftItemCounters();
    void updateManaCounter(int manaIncrease, int numCards);
    void updateItemCounter(QMap<QString, QString> &codeTagMap, DraftItemCounter *counter);
    void updateDropCounter(QMap<QString, QString> &codeTagMap, DraftDropCounter *counter);

public:
    void setDraftMethodAvgScore(DraftMethod draftMethodAvgScore);
    void setShowDrops(bool value);
    void setScores(int deckScoreHA, int deckScoreLF, float deckScoreHSR);
    void updateCounters(QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
                        QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
                        QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                        QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                        QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                        QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                        int manaIncrease, int numCards);
    void updateDeckWeight(int numCards, int draw, int toYourHand, int discover);
    void setTheme();
    int draftedCardsCount();

public slots:
    void sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);

private slots:
    void showHelp();
    void hideHelp();

signals:
    void itemEnter(QList<SynergyCard> &synergyCardList, QPoint &originList, int maxLeft, int maxRight);
    void itemLeave();
    void showPremiumDialog();
};

#endif // DRAFTMECHANICSWINDOW_H
