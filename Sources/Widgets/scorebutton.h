#ifndef SCOREBUTTON_H
#define SCOREBUTTON_H

#include <QObject>
#include <QLabel>
#include "../utility.h"

#define MIN_HSR_DECKS 2000

class ScoreButton : public QLabel
{
    Q_OBJECT

//Constructor
public:
    ScoreButton(QWidget * parent, ScoreSource scoreSource);

//Variables
private:
    float score;
    bool learningMode, learningShow;
    float bestScoreOpacity;
    int includedDecks, classOrder;
    ScoreSource scoreSource;

//Metodos
private:
    void getScoreColor(int &r, int &g, int &b, float score);
    void drawPixmap(QPixmap &canvas, QRect &targetAll, bool bigFont=false);

protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:
    void setScore(float score, float bestScore, int includedDecks=-1, int classOrder=-1);
    void setLearningMode(bool value);
    void setLearningShow(bool value);
    void draw();
    static QIcon scoreIcon(ScoreSource scoreSource, float score, int size=80);

signals:
    void spreadLearningShow(bool value);
    void showHSRwebPicks();
};

#endif // SCOREBUTTON_H
