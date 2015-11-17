#ifndef SCOREBUTTON_H
#define SCOREBUTTON_H

#include <QObject>
#include <QPushButton>

class ScoreButton : public QPushButton
{
    Q_OBJECT

//Constructor
public:
    ScoreButton(QWidget * parent);

//Variables
private:
    double score;
    bool learningMode, learningShow;

//Metodos
protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

public:
    void setScore(double score);
    void setLearningMode(bool value);
};

#endif // SCOREBUTTON_H
