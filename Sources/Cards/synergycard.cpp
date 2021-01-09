#include "synergycard.h"
#include <QtWidgets>

int SynergyCard::synergyWidth = 218;


SynergyCard::SynergyCard(QString code) : DeckCard(code)
{

}


SynergyCard::~SynergyCard()
{

}


void SynergyCard::draw()
{
    QPixmap canvas = DeckCard::draw(total, false, BLACK, false);
    canvas = canvas.scaled(QSize(synergyWidth,35),
                           synergyWidth<218?Qt::KeepAspectRatio:Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
    this->listItem->setIcon(QIcon(canvas));
}

void SynergyCard::setSynergyWidth(int value)
{
    SynergyCard::synergyWidth = value;
}
