#ifndef LAVABUTTON_H
#define LAVABUTTON_H

#include <QObject>
#include <QLabel>
#include "../utility.h"

class LavaButton : public QLabel
{
    Q_OBJECT

//Constructor
public:
    LavaButton(QWidget * parent, float min, float max);

//Variables
private:
    float value, value_0_1, min, max;
    int drawCards, toYourHandCards, discoverCards;

//Metodos
private:
    void draw();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setValue(int totalMana, int numCards, int drawCards, int toYourHandCards, int discoverCards);
    void reset();
};

#endif // LAVABUTTON_H
