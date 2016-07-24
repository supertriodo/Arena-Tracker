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

//Metodos
public:
    void sendPlanCardEntered(QString code, QPoint rectCardTopLeft, QPoint rectCardBottomRight);

signals:
    void cardEntered(QString code, QRect rectCard, int maxTop, int maxBottom);
};

#endif // GRAPHICSITEMSENDER_H
