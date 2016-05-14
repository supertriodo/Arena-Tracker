#ifndef PLANHANDLER_H
#define PLANHANDLER_H

#include "Widgets/ui_extended.h"
#include "Widgets/miniongraphicsitem.h"
#include "utility.h"
#include <QObject>

class PlanHandler : public QObject
{
    Q_OBJECT
public:
    PlanHandler(QObject *parent, Ui::Extended *ui);
    ~PlanHandler();

//Variables
private:
    Ui::Extended *ui;
    QList<MinionGraphicsItem *> playerMinions, enemyMinions;
    MinionGraphicsItem * lastMinionAdded;
    bool inGame;
    bool mouseInApp;
    Transparency transparency;

//Metodos:
private:
    void updateTransparency();
    void completeUI();
    void updateZonePos(bool friendly);
    QList<MinionGraphicsItem *> *getMinionList(bool friendly);
    int findMinionPos(QList<MinionGraphicsItem *> *minionsList, int id);
    void addMinion(bool friendly, QString code, int id, int pos);
    void updateMinionPos(bool friendly, int id, int pos);
    void removeMinion(bool friendly, int id);

public:
    void setTransparency(Transparency value);
    void setMouseInApp(bool value);

signals:
//    void checkCardImage(QString code);
//    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void needMainWindowFade(bool fade);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="PlanHandler");

public slots:
    void playerMinionZonePlayAdd(QString code, int id, int pos);
    void enemyMinionZonePlayAdd(QString code, int id, int pos);
    void playerMinionZonePlayRemove(int id);
    void enemyMinionZonePlayRemove(int id);
    void playerMinionPosChange(int id, int pos);
    void enemyMinionPosChange(int id, int pos);
};

#endif // PLANHANDLER_H
