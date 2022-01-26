#ifndef GRAPHICSITEMSENDER_H
#define GRAPHICSITEMSENDER_H

#include <QObject>
#include "../extended_ui.h"

class CardGraphicsItem;
class HeroPowerGraphicsItem;
class MinionGraphicsItem;
class HeroGraphicsItem;

class GraphicsItemSender : public QObject
{
    Q_OBJECT

public:
    GraphicsItemSender(QObject *parent, Ui::Extended *ui);

//Variables
private:
    Ui::Extended *ui;
    QString lastCode; //Evita enviar continuamente signals mientras el raton se mueve dentro del secreto.
    int lastId;

    //Metodos
private:
    QRect getRectCard(QPoint rectCardTopLeft, QPoint rectCardBottomRight, int &maxTop, int &maxBottom);

public:
    void sendPlanCardPress(CardGraphicsItem *card);
    void sendPlanCardEntered(QString code, QPoint rectCardTopLeft, QPoint rectCardBottomRight);
    void sendPlanSecretEntered(int id, QPoint rectCardTopLeft, QPoint rectCardBottomRight);
    void sendPlanCardLeave();
    void sendResetDeadProbs();
    void sendCheckBomb(QString code);
    void sendHeroTotalAttackChange(bool friendly, int totalAttack, int totalMaxAttack);

signals:
    void heroPowerPress(HeroPowerGraphicsItem *card, Qt::MouseButton mouseButton);
    void cardPress(CardGraphicsItem *card, Qt::MouseButton mouseButton);
    void minionPress(MinionGraphicsItem *minion, Qt::MouseButton mouseButton);
    void minionWheel(MinionGraphicsItem *minion, bool up);
    void heroPress(HeroGraphicsItem *hero, Qt::MouseButton mouseButton);
    void heroWheel(HeroGraphicsItem *hero, bool up);
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void secretEntered(int id, QRect &rectCard, int maxTop, int maxBottom);
    void cardLeave();
    void resetDeadProbs();
    void checkBomb(QString code);
    void heroTotalAttackChange(bool friendly, int totalAttack, int totalMaxAttack);
};

#endif // GRAPHICSITEMSENDER_H
