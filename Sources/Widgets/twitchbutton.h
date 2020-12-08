#ifndef TWITCHBUTTON_H
#define TWITCHBUTTON_H

#include <QObject>
#include <QLabel>
#include "../utility.h"

#define FT_DRAW_STEP 50
#define FT_MAX_LIFE 6000
#define FT_SIZE static_cast<float>(6000)
#define FT_OPACITY static_cast<float>(2000)
#define FT_OFFSET static_cast<float>(3000)

class FloatingText
{
public:
    QString username;
    qint64 birth;
    int pixelSize;
    bool up;
};

class TwitchButton : public QLabel
{
    Q_OBJECT

//Constructor
public:
    TwitchButton(QWidget * parent, float min, float max);

//Variables
private:
    float value, value_0_1, min, max;
    int votes;
    bool isBestScore;
    QList<FloatingText> ftList;

//Metodos
private:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setValue(float value, int votes, bool isBestScore, QString username="");
    void reset();

private slots:
    void draw();
};

#endif // TWITCHBUTTON_H
