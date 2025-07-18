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
    ScoreButton(QWidget * parent, ScoreSource scoreSource, int classOrder);

//Variables
private:
    float score;
    bool learningMode, hoverScore;
    float bestScoreOpacity;
    int includedDecks, classOrder;
    ScoreSource scoreSource;

    static float heroScores[NUM_HEROS];
    static int playerRuns[NUM_HEROS], playerWins[NUM_HEROS], playerLost[NUM_HEROS];
    static float minHeroScore, maxHeroScore;

//Metodos
private:
    void getScoreColor(int &r, int &g, int &b, float score, ScoreSource scoreSource);
    void drawPixmap(QPixmap &canvas, QRect &targetAll, bool bigFont=false);

protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:
    void setScore(float score, float bestScore, int includedDecks=-1);
    void setClassOrder(int classOrder);
    void setLearningMode(bool value);
    void setHoverScore(bool value);
    void draw();

    static QPixmap scorePixmap(ScoreSource scoreSource, float score, bool useWideHeroesColor, int size=80, int classOrder=-1, int includedDecks=-1);
    static QIcon scoreIcon(ScoreSource scoreSource, float score, int size=80, bool useWideHeroesColor=true);
    static void setHeroScores(float heroScores[NUM_HEROS]);
    static void setPlayerRuns(int playerRuns[NUM_HEROS]);
    static void setPlayerWins(int playerWins[NUM_HEROS]);
    static void setPlayerLost(int playerLost[NUM_HEROS]);
    static float getHeroScore(int classOrder);
    static float getPlayerRun(int classOrder);
    static float getPlayerWinrate(int classOrder);
    static void addRun(int classOrder, int wins, int lost);

signals:
    void spreadHoverScore(bool value);
    void showHSRwebPicks();
};

#endif // SCOREBUTTON_H
