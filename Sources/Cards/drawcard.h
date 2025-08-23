#ifndef DRAWCARD_H
#define DRAWCARD_H

#include "synergycard.h"


class DrawCard : public SynergyCard
{
public:
    DrawCard(QString code, bool showDrops=false);
    ~DrawCard();

//Metodos
public:
    void draw();
};

#endif // DRAWCARD_H
