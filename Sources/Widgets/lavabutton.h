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
    LavaButton(QWidget * parent, double min, double max);

//Variables
private:
    double value, value_0_1, min, max;

//Metodos
private:
    void draw();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setValue(double value);
};

#endif // LAVABUTTON_H
