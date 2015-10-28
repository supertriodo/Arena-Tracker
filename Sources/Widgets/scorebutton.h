#ifndef SCOREBUTTON_H
#define SCOREBUTTON_H

#include <QObject>
#include <QPushButton>

class ScoreButton : public QPushButton
{
    Q_OBJECT

//Constructor
public:
    ScoreButton(QWidget * parent, int index);

//Variables
private:
    int index;
    bool drawArrow;
    bool drawHLines;

//Metodos
protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setDrawArrow(bool value);
    void setDrawHLines(bool value);

signals:
    void enter(int index);
    void leave(int index);
};

#endif // SCOREBUTTON_H
