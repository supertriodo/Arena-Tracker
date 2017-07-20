#ifndef DRAFTITEMCOUNTER_H
#define DRAFTITEMCOUNTER_H

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
#include "../utility.h"

class DraftItemCounter : public QObject
{
    Q_OBJECT
public:
    DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap);

//Variables
private:
    QLabel labelIcon, labelCounter;
    int counter;

//Metodos
public:
    void reset();
    void setTransparency(Transparency transparency, bool mouseInApp);
    bool increase();
    bool isHidden();

signals:

public slots:
};

#endif // DRAFTITEMCOUNTER_H
