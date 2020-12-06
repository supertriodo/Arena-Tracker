#ifndef DRAFTSCOREWINDOW_H
#define DRAFTSCOREWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QHBoxLayout>
#include "movelistwidget.h"
#include "scorebutton.h"
#include "twitchbutton.h"
#include "../utility.h"
#include "../Cards/deckcard.h"


#define MARGIN 10
#define SYNERGY_MOTION_UPDATE_TIME 50


class SynergyMotion
{
public:
    bool moveDown, moving;
    int value, maximum, stepValue;
};

class DraftScoreWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, bool normalizedLF);
    ~DraftScoreWindow();

//Variables
private:
    QHBoxLayout *horLayoutMechanics[3];
    ScoreButton *scoresPushButton[3];
    ScoreButton *scoresPushButton2[3];
    ScoreButton *scoresPushButton3[3];
    TwitchButton *twitchButton[3];
    MoveListWidget *synergiesListWidget[3];
    QList<DeckCard> synergiesDeckCardLists[3];
    SynergyMotion synergyMotions[3];
    int scoreWidth;
    int maxSynergyHeight;


//Metodos
private:
    void resetTwitchScore();
    void resizeSynergyList();
    QString getMechanicTooltip(QString iconName);
    QPixmap createMechanicIconPixmap(const QString &mechanicIcon, int count, const MechanicBorderColor dropBorderColor);
    void checkScoresSpace(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR, bool showTwitch);
    bool paintDropBorder(QPainter &painter, const QString &mechanicIcon, const MechanicBorderColor dropBorderColor);

public:
    void setScores(float rating1, float rating2, float rating3, DraftMethod draftMethod, int includedDecks1, int includedDecks2, int includedDecks3);
    void hideScores(bool quick=false);
    void setLearningMode(bool value);
    void showTwitchScores(bool show=true);
    void setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR);
    void redrawSynergyCards();
    void setSynergies(int posCard, QMap<QString, int> &synergies, QMap<QString, int> &mechanicIcons, const MechanicBorderColor dropBorderColor);
    void setNormalizedLF(bool value);
    void setTwitchScores(int vote1, int vote2, int vote3, QString username);

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void cardLeave();
    void showHSRwebPicks();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftScoreWindow");

private slots:
    void hideSynergies(int index);
    void showSynergies();
    void findSynergyCardEntered(QListWidgetItem *item);
    void spreadLearningShow(bool value);
    void stepScrollSynergies(int indexList);
    void resumeSynergyMotion();
};

#endif // DRAFTSCOREWINDOW_H
