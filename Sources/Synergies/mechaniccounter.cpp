#include "mechaniccounter.h"
#include "keysynergies.h"
#include "../themehandler.h"

QMap<QString, DraftItemCounter*> MechanicCounter::keySynergiesMap;
QMap<QString, QList<QString>> * MechanicCounter::synergyCodes = nullptr;
LavaButton * MechanicCounter::lavaButton;

MechanicCounter::MechanicCounter()
{

}


QMap<QString, DraftItemCounter*> * MechanicCounter::createMechanicCounters(QObject *parent, QGridLayout *mechanicsLayout, LavaButton *lavaButton)
{
    MechanicCounter::lavaButton = lavaButton;

    QMap<QString, QString> map = getMapKeySynergies();
    const auto keys = map.keys();
    for(const QString &key: keys)
    {
        const QString &synergyTag = map[key];
        DraftItemCounter * item;

        if(key == "reach")
        {
            item = new DraftItemCounter(parent, "Reach", "Reach", mechanicsLayout, 2, 0, QPixmap(ThemeHandler::reachMechanicFile()), 32, true, false);
        }
        else if(key == "ping")
        {
            item = new DraftItemCounter(parent, "Ping", "Ping", mechanicsLayout, 3, 0,QPixmap(ThemeHandler::pingMechanicFile()), 32, true, false);
        }
        else if(key == "damageMinions")
        {
            item = new DraftItemCounter(parent, "Removal", "Removal", mechanicsLayout, 3, 1,QPixmap(ThemeHandler::damageMechanicFile()), 32, true, false);
        }
        else if(key == "destroy")
        {
            item = new DraftItemCounter(parent, "Hard Removal", "Hard Removal", mechanicsLayout, 3, 2,QPixmap(ThemeHandler::destroyMechanicFile()), 32, true, false);
        }
        else if(key == "aoe")
        {
            item = new DraftItemCounter(parent, "AOE", "AOE", mechanicsLayout, 3, 3,QPixmap(ThemeHandler::aoeMechanicFile()), 32, true, false);
        }
        else
        {
            item = new DraftItemCounter(parent, synergyTag);
        }

        keySynergiesMap.insert(key, item);
        //Qt los borrara cuando parent se destruya
    }

    //Only counters
    keySynergiesMap.insert("survival", new DraftItemCounter(parent, "Survival", "Survival", mechanicsLayout, 2, 2, QPixmap(ThemeHandler::survivalMechanicFile()), 32, true, false));
    keySynergiesMap.insert("discoverDraw", new DraftItemCounter(parent, "Draw", "Draw", mechanicsLayout, 2, 3, QPixmap(ThemeHandler::drawMechanicFile()), 32, true, false));

    //Key All
    keySynergiesMap.insert("tauntAll", new DraftItemCounter(parent, "Taunt", "Taunt", mechanicsLayout, 2, 1, QPixmap(ThemeHandler::tauntMechanicFile()), 32, true, false));
    keySynergiesMap.insert("deathrattleGoodAll", new DraftItemCounter(parent, "Deathrattle"));

    return &MechanicCounter::keySynergiesMap;
}


void MechanicCounter::setTheme()
{
    keySynergiesMap["aoe"]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()), 32, false);
    keySynergiesMap["tauntAll"]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()), 32, false);
    keySynergiesMap["survival"]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()), 32, false);
    keySynergiesMap["discoverDraw"]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()), 32, false);

    keySynergiesMap["ping"]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()), 32, false);
    keySynergiesMap["damageMinions"]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()), 32, false);
    keySynergiesMap["destroy"]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()), 32, false);
    keySynergiesMap["reach"]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()), 32, false);
}


void MechanicCounter::resetAll()
{
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        keySynergiesMap[key]->reset();
    }
    keySynergiesMap["tauntAll"]->reset();
    keySynergiesMap["deathrattleGoodAll"]->reset();
    keySynergiesMap["survival"]->reset();
    keySynergiesMap["discoverDraw"]->reset();
}


void MechanicCounter::setHidden(bool hide)
{
    const auto mechanicKeys = MechanicCounter::getListKeyLabels();
    for(const auto &key: mechanicKeys)
    {
        if(hide)    keySynergiesMap[key]->hide();
        else        keySynergiesMap[key]->show();
    }
}


void MechanicCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    const auto mechanicKeys = MechanicCounter::getListKeyLabels();
    for(const auto &key: mechanicKeys)
    {
        keySynergiesMap[key]->setTransparency(transparency, mouseInApp);
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

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            if(key == "restoreFriendlyHero")    mec<<"restoreFriendlyHero o lifesteal o lifestealGen";
            if(key == "deathrattle")            mec<<"deathrattle o deathrattleOpponent";
            else                                mec << key;
        }
        else if(isKeyGen(key+"Gen", code, referencedTags))
        {
            mec << key+"Gen";
        }
    }
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mec<<"deathrattle o deathrattleGen";

    //Syn
    if(isDeathrattleGoodAllSyn(code, text))                                 mec<<"deathrattleGoodAllSyn";

    return mec;
}


void MechanicCounter::getMechanicCounters(QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                          QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                          QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                          QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                          QList<SynergyWeightCard> &synergyWeightCardList)
{
    aoeMap = keySynergiesMap["aoe"]->getCodeTagMap();
    tauntMap = keySynergiesMap["tauntAll"]->getCodeTagMap();
    survivabilityMap = keySynergiesMap["survival"]->getCodeTagMap();
    drawMap = keySynergiesMap["discoverDraw"]->getCodeTagMap();
    pingMap = keySynergiesMap["ping"]->getCodeTagMap();
    damageMap = keySynergiesMap["damageMinions"]->getCodeTagMap();
    destroyMap = keySynergiesMap["destroy"]->getCodeTagMap();
    reachMap = keySynergiesMap["reach"]->getCodeTagMap();
    synergyWeightCardList = lavaButton->getsynergyWeightCardListDupped();
}


void MechanicCounter::updateMechanicCounters(const QString &code,
                                             QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                             QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                             QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                             QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                             QList<SynergyWeightCard> &synergyWeightCardList,
                                             const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                             const QString &text, CardType cardType, int attack, int cost)
{
    bool isSurvivability = false;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[key]->increase(code);

            if(key == "aoe")                    aoeMap.insertMulti(code, "");
            else if(key == "ping")              pingMap.insertMulti(code, "");
            else if(key == "damageMinions")     damageMap.insertMulti(code, "");
            else if(key == "destroy")           destroyMap.insertMulti(code, "");
            else if(key == "reach")             reachMap.insertMulti(code, "");
            else if(key == "restoreFriendlyHero" || key == "armor")  isSurvivability = true;
            else if(key == "taunt")
            {
                keySynergiesMap["tauntAll"]->increase(code);
                tauntMap.insertMulti(code, "");
            }
        }
        else if(key == "taunt" && isKeyGen("tauntGen", code, referencedTags))
        {
            keySynergiesMap["tauntAll"]->increase(code);
            tauntMap.insertMulti(code, "");
        }
        //Syn
        if(isKeySyn(key+"Syn", code))
        {
            keySynergiesMap[key]->increaseSyn(code);
        }
        else if(key == "taunt" && isKeyAllSyn("tauntAllSyn", code))
        {
            keySynergiesMap["tauntAll"]->increaseSyn(code);
        }
    }


    if(isDiscoverDrawGen(code, cost, mechanics, referencedTags, text))
    {
        keySynergiesMap["discoverDraw"]->increase(code);
        drawMap.insertMulti(code, "");
    }
    if(isSurvivability)
    {
        keySynergiesMap["survival"]->increase(code);
        survivabilityMap.insertMulti(code, "");
    }
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     keySynergiesMap["deathrattleGoodAll"]->increase(code);
    if(isDeathrattleGoodAllSyn(code, text))                                 keySynergiesMap["deathrattleGoodAll"]->increaseSyn(code);

    int draw = numDrawGen(code, text);
    int toYourHand = numToYourHandGen(code, cost, mechanics, text);
    int discover = numDiscoverGen(code, mechanics, referencedTags);
    lavaButton->increase(code, draw, toYourHand, discover);
    synergyWeightCardList.append(SynergyWeightCard(code, draw, toYourHand, discover));
}


void MechanicCounter::getMechanicSynergies(const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap, QMap<MechanicIcons, int> &mechanicIcons,
                                           const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                           const QString &text, CardType cardType, int attack, int cost)
{
    bool addRestoreIcon = false;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            if(key.endsWith("GenGen"))          keySynergiesMap[key]->insertCards(synergyTagMap);
            else if(key.endsWith("GenGenX"))    keySynergiesMap[key]->insertCards(synergyTagMap, code);
            else
            {
                keySynergiesMap[key]->insertSynCards(synergyTagMap);

                if(key == "aoe")                    mechanicIcons[M_AOE] = keySynergiesMap["aoe"]->count() + 1;
                else if(key == "ping")              mechanicIcons[M_PING] = keySynergiesMap["ping"]->count() + 1;
                else if(key == "damageMinions")     mechanicIcons[M_DAMAGE] = keySynergiesMap["damageMinions"]->count() + 1;
                else if(key == "destroy")           mechanicIcons[M_DESTROY] = keySynergiesMap["destroy"]->count() + 1;
                else if(key == "reach")             mechanicIcons[M_REACH] = keySynergiesMap["reach"]->count() + 1;
                else if(key == "restoreFriendlyHero" || key == "armor")  addRestoreIcon = true;
                else if(key == "taunt")
                {
                    keySynergiesMap["tauntAll"]->insertSynCards(synergyTagMap);
                    mechanicIcons[M_TAUNT_ALL] = keySynergiesMap["tauntAll"]->count() + 1;
                }
            }
        }
        else if(key == "taunt" && isKeyGen("tauntGen", code, referencedTags))
        {
            keySynergiesMap["tauntAll"]->insertSynCards(synergyTagMap);
            mechanicIcons[M_TAUNT_ALL] = keySynergiesMap["tauntAll"]->count() + 1;
        }
        //Syn
        if(isKeySyn(key+"Syn", code))
        {
            keySynergiesMap[key]->insertCards(synergyTagMap);
        }
        else if(key == "taunt" && isKeyAllSyn("tauntAllSyn", code))
        {
            keySynergiesMap["tauntAll"]->insertCards(synergyTagMap);
        }
    }

    if(isDiscoverDrawGen(code, cost, mechanics, referencedTags, text))
    {
        mechanicIcons[M_DISCOVER_DRAW] = keySynergiesMap["discoverDraw"]->count() + 1;
    }
    if(addRestoreIcon)
    {
        mechanicIcons[M_SURVIVABILITY] = keySynergiesMap["survival"]->count() + 1;
    }
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType)) keySynergiesMap["deathrattleGoodAll"]->insertSynCards(synergyTagMap);
    if(isDeathrattleGoodAllSyn(code, text))                             keySynergiesMap["deathrattleGoodAll"]->insertCards(synergyTagMap);
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void MechanicCounter::getPartKeySynergies(const QString &partSynergy, const QString &code, QMap<QString, QMap<QString, int> > &synergyTagMap)
{
    //No abarca la complejidad de isDeathrattleGoodAll() en caso de ser "deathrattle" o "deathrattleOpponent" o "deathrattleGen" pero es aceptable.
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("GenGenX"))
    {
        QString key = partSynergy;

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertCards(synergyTagMap, code);
    }
    else if(partSynergy.endsWith("GenGen"))
    {
        QString key = partSynergy;

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        QString keyAll;

        if(partSynergy == "deathrattleGen")
        {
            keyAll = key+"GoodAll";
        }
        else
        {
            keyAll = key+"All";
        }

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
    }
    else
    {
        QString key = partSynergy;
        QString keyAll;

        if(partSynergy == "deathrattle")
        {
            keyAll = key+"GoodAll";
        }
        else
        {
            keyAll = key+"All";
        }

        if(keySynergiesMap.contains(key))   keySynergiesMap[key]->insertSynCards(synergyTagMap);
        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll]->insertSynCards(synergyTagMap);
    }
}


//Usada por LayeredSynergies para verificar que el code hace sinergia con cada una de las partSynergy
bool MechanicCounter::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                             const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, CardType cardType, int attack, int cost)
{
    //No verificamos que los GenGenX no hacen synergias con el mismo code pero es aceptable
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);

        if(partSynergy == "deathrattleGoodAllSyn")
        {
            partSynergyTag = getSynergyTag("deathrattleGoodAll");
            return isDeathrattleGoodAll(code, mechanics,referencedTags,cardType);
        }
        else
        {
            partSynergyTag = getSynergyTag(key);
            if(partSynergyTag.isEmpty())    return false;

            return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost) ||
                   isKeyGen(key+"Gen", code, referencedTags);
        }
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost);
    }
    else if(partSynergy.endsWith("GenGenX") || partSynergy.endsWith("GenGen"))
    {
        QString key = partSynergy;
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        if(partSynergy == "deathrattleGen")
        {
            return isDeathrattleGoodAllSyn(code, text);
        }
        else
        {
            const QString &keyAll = key+"All";
            return isKeyAllSyn(keyAll+"Syn", code);
        }
    }
    else
    {
        QString key = partSynergy;
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        if(partSynergy == "deathrattle")
        {
            return isKeySyn(key+"Syn", code) ||
                   isDeathrattleGoodAllSyn(code, text);
        }
        else
        {
            const QString &keyAll = key+"All";
            return isKeySyn(key+"Syn", code) ||
                   isKeyAllSyn(keyAll+"Syn", code);
        }
    }
}


QString MechanicCounter::getSynergyTag(const QString &key)
{
    if(!keySynergiesMap.contains(key))  return "";
    return keySynergiesMap[key]->getSynergyTag();
}


QStringList MechanicCounter::getListKeyLabels()
{
    const QStringList keys = {"aoe", "tauntAll", "survival", "discoverDraw", "ping", "damageMinions", "destroy", "reach"};
    return keys;
}


QStringList MechanicCounter::getListValidSynergies()
{
    QStringList validMecs;
    const auto mKeys = MechanicCounter::getListKeySynergies();
    for(const QString &keyS: mKeys)
    {
        if(keyS.endsWith("GenGen") || keyS.endsWith("GenGenX"))
        {
            validMecs << keyS;
        }
        else
        {
            validMecs << keyS << keyS+"Syn";
        }
    }
    validMecs << "tauntGen" << "tauntAllSyn" << "deathrattleGen" << "deathrattleOpponent" << "deathrattleGoodAllSyn";
    return validMecs;
}


QStringList MechanicCounter::getListKeySynergies()
{
    return getMapKeySynergies().keys();
}


QMap<QString, QString> MechanicCounter::getMapKeySynergies()
{
    QMap<QString, QString> keys;
    keys["discover"] = "Discover";
    keys["draw"] = "Draw";
    keys["toYourHand"] = "Gen Cards";
    keys["restoreFriendlyHero"] = "Heal";
    keys["armor"] = "Armor";
    keys["reach"] = "Reach";
    keys["taunt"] = "Taunt";
    keys["damageMinions"] = "Removal";
    keys["ping"] = "Ping";
    keys["aoe"] = "AOE";
    keys["destroy"] = "Hard Removal";
    keys["deathrattle"] = "Deathrattle";
    keys["jadeGolemGenGen"] = "Jade Golem";
    keys["heroPowerGenGenX"] = "Hero Power";
    //"tauntAll", "deathrattleGoodAll"
    //"survival", "discoverDraw"

    return keys;
}


bool MechanicCounter::hasKeyAll(const QString &key)
{
    const QStringList keyAll = {"taunt", "deathrattle"};
    return keyAll.contains(key);
}


bool MechanicCounter::isKey(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost)
{
    if(key == "discover")   return isDiscoverGen(code, mechanics, referencedTags);
    if(key == "draw")       return isDrawGen(code, text);
    if(key == "toYourHand") return isToYourHandGen(code, cost, mechanics, text);

    if(synergyCodes->contains(code))
    {
        if(key == "damageMinions")  return (*synergyCodes)[code].contains("damageMinions") || (*synergyCodes)[code].contains("rushGiver");
        if(key == "deathrattle")    return (*synergyCodes)[code].contains("deathrattle") || (*synergyCodes)[code].contains("deathrattleOpponent");
        if(key == "restoreFriendlyHero")
        {
            return (*synergyCodes)[code].contains("restoreFriendlyHero") || (*synergyCodes)[code].contains("lifesteal") || (*synergyCodes)[code].contains("lifestealGen");
        }
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "taunt")
        {
            if(mechanics.contains(QJsonValue("TAUNT")))
            {
                return true;
            }
            return false;
        }
        else if(key == "aoe")
        {
            return  (text.contains("all") || text.contains("adjacent")) &&
                   (text.contains("damage") ||
                    (text.contains("destroy") && text.contains("minions"))
                    );
        }
        else if(key == "ping")
        {
            //Anything that deals damage
            if(text.contains("deal") && text.contains("1 damage") &&
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
        else if(key == "reach")
        {
            //Anything that deals damage (no pings)
            if(text.contains("damage") && text.contains("deal") &&
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
        else if(key == "damageMinions")
        {
            if(KeySynergies::containsAll(text, "give rush"))
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
        else if(key == "destroy")
        {
            if(text.contains("destroy") && text.contains("minion") &&
                     !text.contains("all"))
            {
                if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
                else return true;
            }
            return false;
        }
        else if(key == "jadeGolemGenGen")
        {
            if(mechanics.contains(QJsonValue("JADE_GOLEM")) || referencedTags.contains(QJsonValue("JADE_GOLEM")))
            {
                return true;
            }
            return false;
        }
        else if(key == "heroPowerGenGenX")
        {
            if(text.contains("hero power") || (text.contains("heal") && text.contains("deal damage")))
            {
                return true;
            }
            return false;
        }
        else if(key == "deathrattle")
        {
            if(cardType != MINION)  return false;
            else if(mechanics.contains(QJsonValue("DEATHRATTLE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "restoreFriendlyHero")
        {
            if(mechanics.contains(QJsonValue("LIFESTEAL")) || referencedTags.contains(QJsonValue("LIFESTEAL")))
            {
                return true;
            }
            else if(text.contains("restore"))
            {
                return true;
            }
            return false;
        }
        else if(key == "armor")
        {
            if(text.contains("armor"))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool MechanicCounter::isKeyGen(const QString &key, const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes->contains(code))
    {
        if(key == "tauntGen")   return (*synergyCodes)[code].contains("tauntGen") || (*synergyCodes)[code].contains("tauntGiver");

        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "tauntGen")
        {
            if(referencedTags.contains(QJsonValue("TAUNT")))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool MechanicCounter::isKeySyn(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


bool MechanicCounter::isKeyAllSyn(const QString &key, const QString &code)
{
    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }

    return false;
}


//Specific isKey
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
