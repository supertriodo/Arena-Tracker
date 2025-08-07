#ifndef DRAFTSCOREWINDOW_H
#define DRAFTSCOREWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QHBoxLayout>
#include "movelistwidget.h"
#include "scorebutton.h"
#include "twitchbutton.h"
#include "hoverlabel.h"
#include "../utility.h"
#include "../Cards/synergycard.h"


#define MARGIN 10
#define SYNERGY_MOTION_UPDATE_TIME 50


class SynergyMotion
{
public:
    bool moveDown, moving, running;
    int value, maximum, stepValue;
};

class DraftScoreWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    DraftScoreWindow(QWidget *parent, QRect rect, QSize sizeCard, int screenIndex, int classOrder);
    ~DraftScoreWindow();

//Variables
private:
    QHBoxLayout *horLayoutScores[3];
    QHBoxLayout *horLayoutScores2[3];
    QGridLayout *gridLayoutMechanics[3];
    ScoreButton *scoresPushButton[3];
    ScoreButton *scoresPushButton2[3];
    ScoreButton *scoresPushButton3[3];
    TwitchButton *twitchButton[3];
    MoveListWidget *synergiesListWidget[3];
    QList<SynergyCard> synergyCardLists[3];
    SynergyMotion synergyMotions[3];
    int scoreWidth;
    int maxSynergyHeight, maxSynergyHeight1Row, maxSynergyHeight2Row;
    bool scores2Rows;
    bool showHA, showLF, showHSR, showTwitch;
    bool wantedMechanics[M_NUM_MECHANICS];
    SynergyCard *warningCard[3];
    HoverLabel *warningCardLabel[3];
    HoverLabel *warningOkLabel[3];
    bool onWarnMode[3];


//Metodos
private:
    void resizeSynergyList();
    QString getMechanicTooltip(MechanicIcons mechanicIcon);
    QPixmap createMechanicIconPixmap(MechanicIcons mechanicIcon, int count, const MechanicBorderColor dropBorderColor);
    void checkScoresSpace();
    bool paintDropBorder(QPainter &painter, MechanicIcons mechanicIcon, const MechanicBorderColor dropBorderColor);
    void reorderMechanics();
    void createMechanicIcon(int posCard, int posMech, MechanicIcons mechanicIcon, int count,
                            const MechanicBorderColor dropBorderColor);
    void groupSynergyTags(QMap<QString, QMap<QString, int> > &synergyTagMap);
    QString getMechanicFile(MechanicIcons mechanicIcon);
    bool isWantedMechanic(uint mechanicIcon);
    void hideWarnings();
    void hideWarning(int i);
    void showScores(int i);

public:
    void setScores(float rating1, float rating2, float rating3, DraftMethod draftMethod,
                   int includedDecks1, int includedDecks2, int includedDecks3);
    void hideScores(bool quick=false);
    void setLearningMode(bool value);
    void showTwitchScores(bool show=true);
    void setDraftMethod(bool draftMethodHA, bool draftMethodLF, bool draftMethodHSR, bool updateSynergies);
    void redrawSynergyCards();
    void setSynergies(int posCard, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons,
                      const MechanicBorderColor dropBorderColor);
    void setTwitchScores(int vote1, int vote2, int vote3, QString username);
    void setWantedMechanic(uint mechanicIcon, bool value);
    void setWantedMechanics(bool wantedMechanics[]);
    void setWarningCard(const int posCard, const QString &code);
    void setTheme();
    QList<SynergyCard> *getSynergyCardLists();

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void cardLeave();
    void showHSRwebPicks();
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftScoreWindow");

private slots:
    void hideSynergies();
    void showSynergies();
    void findSynergyCardEntered(QListWidgetItem *item);
    void spreadHoverScore(bool value);
    void stepScrollSynergies(int indexList);
    void resumeSynergyMotion();
    void clearMechanics();
    void findWarningCardLabelEntered(HoverLabel *hoverLabel);
    void warningOkClick(HoverLabel *hoverLabel);
};

#endif // DRAFTSCOREWINDOW_H
