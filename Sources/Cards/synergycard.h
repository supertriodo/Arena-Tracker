#ifndef SYNERGYCARD_H
#define SYNERGYCARD_H

#include <qlabel.h>
#include "deckcard.h"


class SynergyCard : public DeckCard
{
public:
    SynergyCard(QString code);
    ~SynergyCard();

//Variables
private:
    QString synergyTag;

    static int synergyWidth;

//Metodos
private:
    void drawSynergyTag(QPixmap &canvas);

public:
    QString getSynergyTag();
    void setSynergyTag(QString synergyTag);
    void draw();
    void draw(QLabel *label);

    static void setSynergyWidth(int value);
};

#endif // SYNERGYCARD_H
