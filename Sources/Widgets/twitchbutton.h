#ifndef TWITCHBUTTON_H
#define TWITCHBUTTON_H

#include <QObject>
#include <QLabel>
#include "../utility.h"

class TwitchButton : public QLabel
{
    Q_OBJECT

//Constructor
public:
    TwitchButton(QWidget * parent, double min, double max);

//Variables
private:
    float value, value_0_1, min, max;
    int votes;
    bool isBestScore;

//Metodos
private:
    void draw();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setValue(float value, int votes, bool isBestScore);
    void reset();
};

#endif // TWITCHBUTTON_H
