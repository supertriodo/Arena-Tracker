#include "drawcard.h"
#include "../Synergies/draftdropcounter.h"

DrawCard::DrawCard(QString code, bool showDrops) : SynergyCard(code)
{
    //Add drops tags
    if(showDrops)
    {
        QMap<QString, QString> dropKeys = DraftDropCounter::getMapKeySynergies();
        if(DraftDropCounter::isDrop2(code))         setSynergyTag(dropKeys["drop2"]);
        else if(DraftDropCounter::isDrop3(code))    setSynergyTag(dropKeys["drop3"]);
        else if(DraftDropCounter::isDrop4(code))    setSynergyTag(dropKeys["drop4"]);
    }
}

DrawCard::~DrawCard()
{

}


void DrawCard::draw()
{
    SynergyCard::draw(false);
}
