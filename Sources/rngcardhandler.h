#ifndef RNGCARDHANDLER_H
#define RNGCARDHANDLER_H


#include "Widgets/extended_ui.h"
#include "Widgets/bombwindow.h"
#include "Cards/rngcard.h"
#include "planhandler.h"
#include <QObject>

class RngCardHandler : public QObject
{
    Q_OBJECT
public:
    RngCardHandler(QObject *parent, Ui::Extended *ui, PlanHandler *planHandler);
    ~RngCardHandler();

//Variables
private:
    Ui::Extended *ui;
    QList<RngCard> rngCardList;
    PlanHandler *planHandler;
    BombWindow *bombWindow;
    bool showRngList;
    bool rngAnimating;
    bool patreonVersion;

//Metodos
private:
    void completeUI();
    void createBombWindow();
    void showBombWindow();
    void newRngCard(QString code, int id);
    void updateShowRngList();

public:
    void redrawDownloadedCardImage(QString code);
    void redrawClassCards();
    void redrawSpellWeaponCards();
    void redrawAllCards();
    void setShowRngList(bool value);
    void setTheme();

signals:
    void checkCardImage(QString code);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="RngCardHandler");

public slots:
    void setPremium(bool premium);
    void removeRngCard(int id, QString code="");
    void playerCardToHand(int id, QString code, int turn);
    void clearRngList();

private slots:
    void clearRngAnimating();
    void rngCardEntered(QListWidgetItem *item);
    void adjustRngSize();
};

#endif // POPULARCARDSHANDLER_H
