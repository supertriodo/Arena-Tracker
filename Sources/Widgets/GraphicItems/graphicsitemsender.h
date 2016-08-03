#ifndef GRAPHICSITEMSENDER_H
#define GRAPHICSITEMSENDER_H

#include <QObject>
#include "../ui_extended.h"

class GraphicsItemSender : public QObject
{
    Q_OBJECT

public:
    GraphicsItemSender(QObject *parent, Ui::Extended *ui);

//Variables
private:
    Ui::Extended *ui;
    QString lastCode;

//Metodos
public:
    void sendPlanCardEntered(QString code, QPoint rectCardTopLeft, QPoint rectCardBottomRight);
    void sendPlanCardLeave();
    void sendResetDeadProbs();
    void sendCheckBomb(QString code);

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
    void cardLeave();
    void resetDeadProbs();
    void checkBomb(QString code);
};

#endif // GRAPHICSITEMSENDER_H
