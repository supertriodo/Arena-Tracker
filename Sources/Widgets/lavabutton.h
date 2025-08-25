#ifndef LAVABUTTON_H
#define LAVABUTTON_H

#include "Sources/Cards/synergyweightcard.h"
#include "Sources/Widgets/hoverlabel.h"
#include <QObject>

class LavaButton : public HoverLabel
{
    Q_OBJECT

//Constructor
public:
    LavaButton(QWidget * parent, float min, float max);

//Variables
private:
    float value, value_0_1, min, max;
    QList<SynergyWeightCard> synergyWeightCardList;

//Metodos
private:
    void draw();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:
    void setValue(int totalMana, int numDD, int numCards);
    void reset();
    void increase(SynergyWeightCard &synergyWeightCard);
    void increase(const QString &code, int draw, int toYourHand, int discover);
    QList<SynergyWeightCard> getsynergyWeightCardList();

signals:
    void iconEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);
    void iconLeave();

private slots:
    void sendIconEnter();
};

#endif // LAVABUTTON_H
