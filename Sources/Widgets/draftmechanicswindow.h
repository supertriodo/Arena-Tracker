#ifndef DRAFTMECHANICSWINDOW_H
#define DRAFTMECHANICSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "scorebutton.h"
#include "lavabutton.h"
#include "../Synergies/draftitemcounter.h"
#include "../Synergies/draftdropcounter.h"

enum VType {V_MINION, V_SPELL, V_WEAPON, V_NUM_TYPES};
enum VMechanics {V_AOE, V_TAUNT_ALL, V_SURVIVABILITY, V_DISCOVER_DRAW, V_PING, V_DAMAGE, V_DESTROY, V_REACH, V_NUM_MECHANICS};
enum VDrop {V_DROP2, V_DROP3, V_DROP4, V_NUM_DROPS};


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
    ScoreButton *scoreButtonFire, *scoreButtonHA, *scoreButtonHSR;
    HoverLabel *helpMark;
    DraftItemCounter **cardTypeCounters, **mechanicCounters;
    DraftItemCounter *manaCounter;
    DraftDropCounter **dropCounters;
    int scoreWidth;
    bool showingHelp, showDrops;
    DraftMethod draftMethodAvgScore;
    QGridLayout *mechanicsLayout;
    //Track el estado hidden/shown antes de mostrar ayuda reenter para dejarla igual al llamar a hideHelp()
    bool hiddenBeforeReenter;

//Metodos
private:
    void deleteDraftItemCounters();
    void updateManaCounter(int manaIncrease, int numCards);
    void updateItemCounter(QMultiMap<QString, QString> &codeTagMap, DraftItemCounter *counter);
    void updateDropCounter(QMultiMap<QString, QString> &codeTagMap, DraftDropCounter *counter);

public:
    void setDraftMethodAvgScore(DraftMethod draftMethodAvgScore);
    void setShowDrops(bool value);
    void setScores(int deckScoreHA, float deckScoreFire, float deckScoreHSR);
    void updateCounters(QMultiMap<QString, QString> &spellMap, QMultiMap<QString, QString> &minionMap, QMultiMap<QString, QString> &weaponMap,
                        QMultiMap<QString, QString> &drop2Map, QMultiMap<QString, QString> &drop3Map, QMultiMap<QString, QString> &drop4Map,
                        QMultiMap<QString, QString> &aoeMap, QMultiMap<QString, QString> &tauntMap,
                        QMultiMap<QString, QString> &survivabilityMap, QMultiMap<QString, QString> &drawMap,
                        QMultiMap<QString, QString> &pingMap, QMultiMap<QString, QString> &damageMap,
                        QMultiMap<QString, QString> &destroyMap, QMultiMap<QString, QString> &reachMap,
                        int manaIncrease, int numCards);
    void updateDeckWeight(QList<SynergyWeightCard> &synergyWeightCardList, int numCards);
    void setTheme();
    int draftedCardsCount();
    void showHelpReenter();

public slots:
    void sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);

private slots:
    void showHelp(bool reenter=false);
    void hideHelp();

signals:
    void itemEnter(QList<SynergyCard> &synergyCardList, QPoint &originList, int maxLeft, int maxRight);
    void itemLeave();
    void showPremiumDialog();
};

#endif // DRAFTMECHANICSWINDOW_H
