#include "mechaniccounter.h"
#include "keysynergies.h"
#include "../themehandler.h"

DraftItemCounter ** MechanicCounter::mechanicCounters;
QMap<QString, QList<QString>> * MechanicCounter::synergyCodes;

MechanicCounter::MechanicCounter()
{

}


DraftItemCounter ** MechanicCounter::createMechanicCounters(QObject *parent, QGridLayout *mechanicsLayout)
{
    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_REACH] = new DraftItemCounter(parent, "Reach", "Reach", mechanicsLayout, 2, 0,
                                                     QPixmap(ThemeHandler::reachMechanicFile()), 32, true, false);
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(parent, "Taunt", "Taunt", mechanicsLayout, 2, 1,
                                                         QPixmap(ThemeHandler::tauntMechanicFile()), 32, true, false);
    mechanicCounters[V_SURVIVABILITY] = new DraftItemCounter(parent, "Survival", "Survival", mechanicsLayout, 2, 2,
                                                             QPixmap(ThemeHandler::survivalMechanicFile()), 32, true, false);
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(parent, "Draw", "Draw", mechanicsLayout, 2, 3,
                                                             QPixmap(ThemeHandler::drawMechanicFile()), 32, true, false);

    mechanicCounters[V_PING] = new DraftItemCounter(parent, "Ping", "Ping", mechanicsLayout, 3, 0,
                                                    QPixmap(ThemeHandler::pingMechanicFile()), 32, true, false);
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(parent, "Removal", "Removal", mechanicsLayout, 3, 1,
                                                      QPixmap(ThemeHandler::damageMechanicFile()), 32, true, false);
    mechanicCounters[V_DESTROY] = new DraftItemCounter(parent, "Hard Removal", "Hard Removal", mechanicsLayout, 3, 2,
                                                       QPixmap(ThemeHandler::destroyMechanicFile()), 32, true, false);
    mechanicCounters[V_AOE] = new DraftItemCounter(parent, "AOE", "AOE", mechanicsLayout, 3, 3,
                                                   QPixmap(ThemeHandler::aoeMechanicFile()), 32, true, false);

    mechanicCounters[V_DISCOVER] = new DraftItemCounter(parent, "Discover");
    mechanicCounters[V_DRAW] = new DraftItemCounter(parent, "Draw");
    mechanicCounters[V_TOYOURHAND] = new DraftItemCounter(parent, "Gen Cards");
    mechanicCounters[V_TAUNT] = new DraftItemCounter(parent, "Taunt");
    mechanicCounters[V_RESTORE_FRIENDLY_HEROE] = new DraftItemCounter(parent, "Heal");
    mechanicCounters[V_ARMOR] = new DraftItemCounter(parent, "Armor");

    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(parent, "Deathrattle");
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(parent, "Deathrattle");
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(parent, "Jade Golem");
    mechanicCounters[V_HERO_POWER] = new DraftItemCounter(parent, "Hero Power");

    return mechanicCounters;
}


void MechanicCounter::deleteMechanicCounters()
{
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        delete mechanicCounters[i];
    }
    delete []mechanicCounters;
}


void MechanicCounter::setTheme()
{
    mechanicCounters[V_AOE]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()), 32, false);
    mechanicCounters[V_TAUNT_ALL]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()), 32, false);
    mechanicCounters[V_SURVIVABILITY]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()), 32, false);
    mechanicCounters[V_DISCOVER_DRAW]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()), 32, false);

    mechanicCounters[V_PING]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()), 32, false);
    mechanicCounters[V_DAMAGE]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()), 32, false);
    mechanicCounters[V_DESTROY]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()), 32, false);
    mechanicCounters[V_REACH]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()), 32, false);
}


void MechanicCounter::resetAll()
{
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->reset();
    }
}


void MechanicCounter::setHidden(bool hide)
{
    if(hide)
    {
        mechanicCounters[V_AOE]->hide();
        mechanicCounters[V_TAUNT_ALL]->hide();
        mechanicCounters[V_SURVIVABILITY]->hide();
        mechanicCounters[V_DISCOVER_DRAW]->hide();

        mechanicCounters[V_PING]->hide();
        mechanicCounters[V_DAMAGE]->hide();
        mechanicCounters[V_DESTROY]->hide();
        mechanicCounters[V_REACH]->hide();
    }
    else
    {
        mechanicCounters[V_AOE]->show();
        mechanicCounters[V_TAUNT_ALL]->show();
        mechanicCounters[V_SURVIVABILITY]->show();
        mechanicCounters[V_DISCOVER_DRAW]->show();

        mechanicCounters[V_PING]->show();
        mechanicCounters[V_DAMAGE]->show();
        mechanicCounters[V_DESTROY]->show();
        mechanicCounters[V_REACH]->show();
    }
}


void MechanicCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


void MechanicCounter::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    MechanicCounter::synergyCodes = synergyCodes;
}


QStringList MechanicCounter::debugMechanicSynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                                    const QString &text, CardType cardType, int attack, int cost)
{
    QStringList mec;

    //Gen
    if(isDiscoverGen(code, mechanics, referencedTags))                      mec<<"discover";
    if(isDrawGen(code, text))                                               mec<<"draw";
    if(isToYourHandGen(code, cost, mechanics, text))                        mec<<"toYourHand";
    if(isAoeGen(code, text))                                                mec<<"aoe";
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mec<<"ping";
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"damageMinions";
    if(isDestroyGen(code, mechanics, text))                                 mec<<"destroy";
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"reach";
    if(isRestoreFriendlyHeroGen(code, mechanics, referencedTags, text))     mec<<"restoreFriendlyHeroGen o lifesteal o lifestealGen";
    if(isArmorGen(code, text))                                              mec<<"armor";
    if(isTaunt(code, mechanics))                                            mec<<"taunt";
    else if(isTauntGen(code, referencedTags))                               mec<<"tauntGen";

    if(isDeathrattleMinion(code, mechanics, cardType))                      mec<<"deathrattle o deathrattleOpponent";
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mec<<"deathrattle o deathrattleGen";
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mec<<"jadeGolemGenGen";
    if(isHeroPowerGen(code, text))                                          mec<<"heroPowerGenGenX";


    //Syn
    if(isDeathrattleGoodAllSyn(code, text))                                 mec<<"deathrattleGoodAllSyn";

    return mec;
}


void MechanicCounter::getMechanicCounters(QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                          QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                          QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                          QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                          int &draw, int &toYourHand, int &discover)
{
    aoeMap = mechanicCounters[V_AOE]->getCodeTagMap();
    tauntMap = mechanicCounters[V_TAUNT_ALL]->getCodeTagMap();
    survivabilityMap = mechanicCounters[V_SURVIVABILITY]->getCodeTagMap();
    drawMap = mechanicCounters[V_DISCOVER_DRAW]->getCodeTagMap();
    pingMap = mechanicCounters[V_PING]->getCodeTagMap();
    damageMap = mechanicCounters[V_DAMAGE]->getCodeTagMap();
    destroyMap = mechanicCounters[V_DESTROY]->getCodeTagMap();
    reachMap = mechanicCounters[V_REACH]->getCodeTagMap();

    discover = draw = toYourHand = 0;
    {
        QMap<QString, int> codeMap = mechanicCounters[V_DISCOVER]->getCodeMap();
        const QList<QString> codeList = codeMap.keys();
        for(const QString &code: codeList)
        {
            QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
            QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
            discover += codeMap[code] * numDiscoverGen(code, mechanics, referencedTags);
        }
    }
    {
        QMap<QString, int> codeMap = mechanicCounters[V_DRAW]->getCodeMap();
        const QList<QString> codeList = codeMap.keys();
        for(const QString &code: codeList)
        {
            QString text = Utility::cardEnTextFromCode(code).toLower();
            draw += codeMap[code] * numDrawGen(code, text);
        }
    }
    {
        QMap<QString, int> codeMap = mechanicCounters[V_TOYOURHAND]->getCodeMap();
        const QList<QString> codeList = codeMap.keys();
        for(const QString &code: codeList)
        {
            QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
            QString text = Utility::cardEnTextFromCode(code).toLower();
            int cost = Utility::getCardAttribute(code, "cost").toInt();
            toYourHand += codeMap[code] * numToYourHandGen(code, cost, mechanics, text);
        }
    }
}


void MechanicCounter::getMechanicSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons,
                                           const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                           const QString &text, CardType cardType, int attack, int cost)
{
    bool addRestoreIcon = false;

    //GEN
    if(isDiscoverDrawGen(code, cost, mechanics, referencedTags, text))
    {
        mechanicIcons[M_DISCOVER_DRAW] = mechanicCounters[V_DISCOVER_DRAW]->count() + 1;
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->insertSynCards(synergyTagMap);
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergyTagMap);
        mechanicIcons[M_TAUNT_ALL] = mechanicCounters[V_TAUNT_ALL]->count() + 1;
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergyTagMap);
        mechanicIcons[M_TAUNT_ALL] = mechanicCounters[V_TAUNT_ALL]->count() + 1;
    }
    if(isAoeGen(code, text))
    {
        mechanicCounters[V_AOE]->insertSynCards(synergyTagMap);
        mechanicIcons[M_AOE] = mechanicCounters[V_AOE]->count() + 1;
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->insertSynCards(synergyTagMap);
        mechanicIcons[M_PING] = mechanicCounters[V_PING]->count() + 1;
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons[M_DAMAGE] = mechanicCounters[V_DAMAGE]->count() + 1;
    }
    if(isDestroyGen(code, mechanics, text))
    {
        mechanicIcons[M_DESTROY] = mechanicCounters[V_DESTROY]->count() + 1;
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons[M_REACH] = mechanicCounters[V_REACH]->count() + 1;
    }
    if(isArmorGen(code, text))
    {
        mechanicCounters[V_ARMOR]->insertSynCards(synergyTagMap);
        addRestoreIcon = true;
    }
    if(isRestoreFriendlyHeroGen(code, mechanics, referencedTags, text))
    {
        mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertSynCards(synergyTagMap);
        addRestoreIcon = true;
    }
    if(addRestoreIcon)
    {
        mechanicIcons[M_SURVIVABILITY] = mechanicCounters[V_SURVIVABILITY]->count() + 1;
    }
    if(isDiscoverGen(code, mechanics, referencedTags))          mechanicCounters[V_DISCOVER]->insertSynCards(synergyTagMap);
    if(isDrawGen(code, text))                                   mechanicCounters[V_DRAW]->insertSynCards(synergyTagMap);
    if(isToYourHandGen(code, cost, mechanics, text))            mechanicCounters[V_TOYOURHAND]->insertSynCards(synergyTagMap);

    if(isDeathrattleMinion(code, mechanics, cardType))          mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergyTagMap);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType)) mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertSynCards(synergyTagMap);

    //Sinergias gen-gen
    if(isJadeGolemGen(code, mechanics, referencedTags))         mechanicCounters[V_JADE_GOLEM]->insertCards(synergyTagMap);
    //Sinergias gen-gen -- Evitamos sinergias con la misma carta
    if(isHeroPowerGen(code, text))                              mechanicCounters[V_HERO_POWER]->insertCards(synergyTagMap, code);


    //SYN
    if(isDiscoverSyn(code))                                     mechanicCounters[V_DISCOVER]->insertCards(synergyTagMap);
    if(isDrawSyn(code))                                         mechanicCounters[V_DRAW]->insertCards(synergyTagMap);
    if(isToYourHandSyn(code))                                   mechanicCounters[V_TOYOURHAND]->insertCards(synergyTagMap);
    if(isRestoreFriendlyHeroSyn(code))                          mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertCards(synergyTagMap);
    if(isArmorSyn(code))                                        mechanicCounters[V_ARMOR]->insertCards(synergyTagMap);
    if(isReachSyn(code))                                        mechanicCounters[V_REACH]->insertCards(synergyTagMap);
    if(isTauntSyn(code))                                        mechanicCounters[V_TAUNT]->insertCards(synergyTagMap);
    else if(isTauntAllSyn(code))                                mechanicCounters[V_TAUNT_ALL]->insertCards(synergyTagMap);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergyTagMap);
    if(isDamageMinionsSyn(code))                                mechanicCounters[V_DAMAGE]->insertCards(synergyTagMap);
    if(isDestroySyn(code))                                      mechanicCounters[V_DESTROY]->insertCards(synergyTagMap);
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergyTagMap);

    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergyTagMap);
    else if(isDeathrattleGoodAllSyn(code, text))                mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertCards(synergyTagMap);
}


void MechanicCounter::updateMechanicCounters(const QString &code,
                                             QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                             QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                             QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                             QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                             int &draw, int &toYourHand, int &discover,
                                             const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                             const QString &text, CardType cardType, int attack, int cost)
{
    bool isSurvivability = false;

    //GEN
    if(isDiscoverDrawGen(code, cost, mechanics, referencedTags, text))
    {
        mechanicCounters[V_DISCOVER_DRAW]->increase(code);
        drawMap.insertMulti(code, "");
    }
    if(isAoeGen(code, text))
    {
        mechanicCounters[V_AOE]->increase(code);
        aoeMap.insertMulti(code, "");
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->increase(code);
        pingMap.insertMulti(code, "");
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_DAMAGE]->increase(code);
        damageMap.insertMulti(code, "");
    }
    if(isDestroyGen(code, mechanics, text))
    {
        mechanicCounters[V_DESTROY]->increase(code);
        destroyMap.insertMulti(code, "");
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_REACH]->increase(code);
        reachMap.insertMulti(code, "");
    }
    if(isRestoreFriendlyHeroGen(code, mechanics, referencedTags, text))
    {
        mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increase(code);
        isSurvivability = true;
    }
    if(isArmorGen(code, text))
    {
        mechanicCounters[V_ARMOR]->increase(code);
        isSurvivability = true;
    }
    if(isSurvivability)
    {
        mechanicCounters[V_SURVIVABILITY]->increase(code);
        survivabilityMap.insertMulti(code, "");
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->increase(code);
        mechanicCounters[V_TAUNT_ALL]->increase(code);
        tauntMap.insertMulti(code, "");
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicCounters[V_TAUNT_ALL]->increase(code);
        tauntMap.insertMulti(code, "");
    }
    discover = numDiscoverGen(code, mechanics, referencedTags);
    draw = numDrawGen(code, text);
    toYourHand = numToYourHandGen(code, cost, mechanics, text);
    if(discover > 0)                                                        mechanicCounters[V_DISCOVER]->increase(code);
    if(draw > 0)                                                            mechanicCounters[V_DRAW]->increase(code);
    if(toYourHand > 0)                                                      mechanicCounters[V_TOYOURHAND]->increase(code);

    //Sinergia deathrattle
    if(isDeathrattleMinion(code, mechanics, cardType))                      mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increase(code);

    //Sinergias gen-gen
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isHeroPowerGen(code, text))                                          mechanicCounters[V_HERO_POWER]->increase(code);


    //SYN
    if(isDiscoverSyn(code))                                                 mechanicCounters[V_DISCOVER]->increaseSyn(code);
    if(isDrawSyn(code))                                                     mechanicCounters[V_DRAW]->increaseSyn(code);
    if(isToYourHandSyn(code))                                               mechanicCounters[V_TOYOURHAND]->increaseSyn(code);
    if(isRestoreFriendlyHeroSyn(code))                                      mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increaseSyn(code);
    if(isArmorSyn(code))                                                    mechanicCounters[V_ARMOR]->increaseSyn(code);
    if(isReachSyn(code))                                                    mechanicCounters[V_REACH]->increaseSyn(code);
    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    else if(isTauntAllSyn(code))                                            mechanicCounters[V_TAUNT_ALL]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isDamageMinionsSyn(code))                                            mechanicCounters[V_DAMAGE]->increaseSyn(code);
    if(isDestroySyn(code))                                                  mechanicCounters[V_DESTROY]->increaseSyn(code);
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);

    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    else if(isDeathrattleGoodAllSyn(code, text))                            mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increaseSyn(code);
}


//Gen
bool MechanicCounter::isDiscoverDrawGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text)
{
    //TEST
    //&& (text.contains("draw") || text.contains("discover") || (text.contains("to") && text.contains("your") && text.contains("hand")))
    return(isDiscoverGen(code, mechanics, referencedTags) ||
            isDrawGen(code, text) ||
            isToYourHandGen(code, cost, mechanics, text));
}
bool MechanicCounter::isDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    return numDiscoverGen(code, mechanics, referencedTags)>0;
}
int MechanicCounter::numDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    //TEST
    //&& text.contains("discover")
    if(synergyCodes->contains(code))
    {
        for(QString mechanic: (const QStringList)(*synergyCodes)[code])
        {
            if(mechanic.startsWith("discover"))
            {
                mechanic.remove(0,8);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        return 0;
    }
    else if(mechanics.contains(QJsonValue("DISCOVER")) || referencedTags.contains(QJsonValue("DISCOVER")))
    {
        return 1;
    }
    return 0;
}
bool MechanicCounter::isDrawGen(const QString &code, const QString &text)
{
    return numDrawGen(code, text)>0;
}
int MechanicCounter::numDrawGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("draw")
    if(synergyCodes->contains(code))
    {
        for(QString mechanic: (const QStringList)(*synergyCodes)[code])
        {
            if(mechanic.startsWith("draw"))
            {
                mechanic.remove(0,4);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        return 0;
    }
    else if((text.contains("draw") && !text.contains("drawn")))
    {
        return 1;
    }
    return 0;
}
bool MechanicCounter::isToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text)
{
    return numToYourHandGen(code, cost, mechanics, text)>0;
}
int MechanicCounter::numToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //&& (text.contains("to") && text.contains("your") && text.contains("hand"))
    if(synergyCodes->contains(code))
    {
        for(QString mechanic: (const QStringList)(*synergyCodes)[code])
        {
            if(mechanic.startsWith("toYourHand"))
            {
                mechanic.remove(0,10);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        if((*synergyCodes)[code].contains("echo")) return 1;
        return 0;
    }
    else if(mechanics.contains(QJsonValue("ECHO"))) return 1;
    else if(cost > 1 && mechanics.contains(QJsonValue("TWINSPELL")))    return 1;
    else if(text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return"))
    {
        return 1;
    }
    return 0;
}
bool MechanicCounter::isTaunt(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("taunt");
    }
    else if(mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isTauntGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("tauntGen") || (*synergyCodes)[code].contains("tauntGiver");
    }
    else if(referencedTags.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isAoeGen(const QString &code, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("aoe");
    }
    else
    {
        return  (text.contains("all") || text.contains("adjacent")) &&
               (text.contains("damage") ||
                (text.contains("destroy") && text.contains("minions"))
                );
    }
}
//El ping debe poder seleccionar a un enemigo
bool MechanicCounter::isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                               const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("ping");
    }
    //Anything that deals damage
    else if(text.contains("deal") && text.contains("1 damage") &&
             !text.contains("hero"))
    {
        if(mechanics.contains("DEATHRATTLE") || text.contains("random"))    return false;
        else return true;
    }
    else if(attack != 1)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
        else if(mechanics.contains(QJsonValue("RUSH"))) return true;
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool MechanicCounter::isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("reach");
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
             !text.contains("1 damage") && !text.contains("minion") && !text.contains("random") &&
             !(text.contains("to") && text.contains("your") && text.contains("hero")))
    {
        return true;
    }
    //Hero attack
    else if(text.contains("+") && text.contains("give") && text.contains("attack") &&
             (text.contains("hero") || text.contains("character")))
    {
        return true;
    }
    else if(attack < 2)  return false;
    //Charge and stealth minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")) ||
            mechanics.contains(QJsonValue("STEALTH")) || referencedTags.contains(QJsonValue("STEALTH")))
        {
            return !text.contains("gain <b>charge</b>") && !text.contains("can't attack heroes");
        }
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool MechanicCounter::isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                        const QString &text, const CardType &cardType, int attack)
{
    //TEST
    //    (text.contains("damage") && text.contains("deal") &&
    //                !text.contains("1 damage") && !text.contains("all") && !text.contains("hero")) &&
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("damageMinions") || (*synergyCodes)[code].contains("rushGiver");
    }
    else if(KeySynergies::containsAll(text, "give rush"))
    {
        return true;
    }
    //Anything that deals damage (no pings)
    else if(text.contains("damage") && text.contains("deal") &&
             !text.contains("1 damage") && !text.contains("all") && !text.contains("hero"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    //Hero attack
    else if(text.contains("+") && text.contains("give") && text.contains("attack") &&
             (text.contains("hero") || text.contains("character")))
    {
        return true;
    }
    else if(attack < 2)  return false;
    //Charge minions
    else if(cardType == MINION)
    {
        if(mechanics.contains(QJsonValue("CHARGE")) || referencedTags.contains(QJsonValue("CHARGE")))
        {
            return !text.contains("gain <b>charge</b>");
        }
        else if(mechanics.contains(QJsonValue("RUSH"))) return true;
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool MechanicCounter::isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("destroy");
    }
    else if(text.contains("destroy") && text.contains("minion") &&
             !text.contains("all"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    return false;
}
bool MechanicCounter::isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("jadeGolemGenGen");
    }
    else if(mechanics.contains(QJsonValue("JADE_GOLEM")) || referencedTags.contains(QJsonValue("JADE_GOLEM")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isHeroPowerGen(const QString &code, const QString &text)
{
    //TEST
    //text.contains("hero power") || (text.contains("heal") && text.contains("deal damage") && cardClass == PRIEST)
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("heroPowerGenGenX");
    }
    else if(text.contains("hero power") || (text.contains("heal") && text.contains("deal damage")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("deathrattle") || (*synergyCodes)[code].contains("deathrattleOpponent");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                          const CardType &cardType)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    if(synergyCodes->contains(code))
    {
        return ((*synergyCodes)[code].contains("deathrattle") || (*synergyCodes)[code].contains("deathrattleGen")) &&
               !(*synergyCodes)[code].contains("silenceOwnSyn") &&
               !(*synergyCodes)[code].contains("deathrattleOpponent");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("restoreFriendlyHero") || (*synergyCodes)[code].contains("lifesteal")
        || (*synergyCodes)[code].contains("lifestealGen");
    }
    else if(mechanics.contains(QJsonValue("LIFESTEAL")) || referencedTags.contains(QJsonValue("LIFESTEAL")))
    {
        return true;
    }
    else if(text.contains("restore"))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isArmorGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("armor")
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("armor");
    }
    else if(text.contains("armor"))
    {
        return true;
    }
    return false;
}


//Syn
bool MechanicCounter::isDiscoverSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("discoverSyn");
    }
    return false;
}
bool MechanicCounter::isDrawSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("drawSyn");
    }
    return false;
}
bool MechanicCounter::isToYourHandSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("toYourHandSyn");
    }
    return false;
}
bool MechanicCounter::isPingSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("pingSyn");
    }
    return false;
}
bool MechanicCounter::isDamageMinionsSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("damageMinionsSyn");
    }
    return false;
}
bool MechanicCounter::isDestroySyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("destroySyn");
    }
    return false;
}
bool MechanicCounter::isReachSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("reachSyn");
    }
    return false;
}
bool MechanicCounter::isAoeSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("aoeSyn");
    }
    return false;
}
bool MechanicCounter::isTauntSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("tauntSyn");
    }
    return false;
}
bool MechanicCounter::isTauntAllSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("tauntAllSyn");
    }
    return false;
}
bool MechanicCounter::isDeathrattleSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("deathrattleSyn");
    }
    return false;
}
bool MechanicCounter::isDeathrattleGoodAllSyn(const QString &code, const QString &text)
{
    //TEST
    //    (containsAll(text, "copy") || containsAll(text, "copies")) &&
    //    (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
    //    !containsAll(text, "in your deck")
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("deathrattleGoodAllSyn");
    }
    else if((KeySynergies::containsAll(text, "copy") || KeySynergies::containsAll(text, "copies")) &&
             (KeySynergies::containsAll(text, "/") || KeySynergies::containsAll(text, "attack") || KeySynergies::containsAll(text, "health")) &&
             !KeySynergies::containsAll(text, "in your deck"))
    {
        return true;
    }
    return false;
}
bool MechanicCounter::isRestoreFriendlyHeroSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("restoreFriendlyHeroSyn");
    }
    return false;
}
bool MechanicCounter::isArmorSyn(const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains("armorSyn");
    }
    return false;
}
