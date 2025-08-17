#include "keysynergies.h"

QMap<QString, KeySynergies> KeySynergies::keySynergiesMap;
QMap<QString, QList<QString>> *KeySynergies::synergyCodes = nullptr;


KeySynergies::KeySynergies()
{
    this->synergyTag = "";
}
KeySynergies::KeySynergies(QString synergyTag)
{
    this->synergyTag = synergyTag;
}


KeySynergies::~KeySynergies()
{
}


void KeySynergies::reset()
{
    this->codeMap.clear();
    this->codeSynMap.clear();
}


void KeySynergies::increase(const QString &code)
{
    if(codeMap.contains(code))  codeMap[code]++;
    else                        codeMap[code] = 1;
}


void KeySynergies::increaseSyn(const QString &code)
{
    if(codeSynMap.contains(code))   codeSynMap[code]++;
    else                            codeSynMap[code] = 1;
}


void KeySynergies::insertCards(QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QMap<QString,int> synergies;
    if(synergyTagMap.contains(synergyTag))  synergies = synergyTagMap[synergyTag];

    const QList<QString> codeList = codeMap.keys();
    for(const QString &code: codeList)
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeMap[code];
        }
    }

    if(!synergies.isEmpty())    synergyTagMap[synergyTag] = synergies;
}


void KeySynergies::insertSynCards(QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QMap<QString,int> synergies;
    if(synergyTagMap.contains(synergyTag))  synergies = synergyTagMap[synergyTag];

    const QList<QString> codeList = codeSynMap.keys();
    for(const QString &code: codeList)
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeSynMap[code];
        }
    }

    if(!synergies.isEmpty())    synergyTagMap[synergyTag] = synergies;
}


void KeySynergies::createKeySynergies()
{
    QMap<QString, QString> map = getMapKeySynergies();
    const auto keys = map.keys();
    for(const QString &key: keys)
    {
        const QString &synergyTag = map[key];
        keySynergiesMap.insert(key, KeySynergies(synergyTag));
        keySynergiesMap.insert(key+"All", KeySynergies(synergyTag));
    }
}


void KeySynergies::setSynergyCodes(QMap<QString, QList<QString>> *synergyCodes)
{
    KeySynergies::synergyCodes = synergyCodes;
}


void KeySynergies::resetAll()
{
    const auto keys = keySynergiesMap.keys();
    for(const QString &key: keys)
    {
        keySynergiesMap[key].reset();
    }
}


bool KeySynergies::containsAll(const QString &text, const QString &words)
{
    for(const QString &word: (const QStringList)words.split(" "))
    {
        if(word == "/")
        {
            QString textNobold = text;
            textNobold.remove("</");
            if(!textNobold.contains(word))    return false;
        }
        else if(!text.contains(word))    return false;
    }
    return true;
}


QStringList KeySynergies::debugKeySynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                      const QString &text, CardType cardType, int attack, int cost)
{
    QStringList mec;

    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            mec << key;
        }
        else if(isKeyGen(key+"Gen", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            mec << key+"Gen";
        }
        //Syn
        if(isKeySyn(key+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            mec << key+"Syn";
        }
        else if(isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            mec << keyAll+"Syn";
        }
    }
    return mec;
}


void KeySynergies::updateKeySynergies(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                        const QString &text, CardType cardType, int attack, int cost)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[key].increase(code);
            keySynergiesMap[keyAll].increase(code);
        }
        else if(isKeyGen(key+"Gen", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[keyAll].increase(code);
        }
        //Syn
        if(isKeySyn(key+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[key].increaseSyn(code);
        }
        else if(isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[keyAll].increaseSyn(code);
        }
    }
}


void KeySynergies::getKeySynergies(const QString &code, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                   const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                   const QString &text, CardType cardType, int attack, int cost)
{
    const auto keys = getListKeySynergies();
    for(const QString &key: keys)
    {
        const QString &keyAll = key+"All";
        //Gen
        if(isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[key].insertSynCards(synergyTagMap);
            keySynergiesMap[keyAll].insertSynCards(synergyTagMap);
        }
        else if(isKeyGen(key+"Gen", code, mechanics, referencedTags, text, cardType, attack, cost))
        {
            keySynergiesMap[keyAll].insertSynCards(synergyTagMap);
        }
        //Syn
        if(!isWeakKeySyn(key))
        {
            if(isKeySyn(key+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
            {
                keySynergiesMap[key].insertCards(synergyTagMap);
            }
            else if(isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost))
            {
                keySynergiesMap[keyAll].insertCards(synergyTagMap);
            }
        }
    }
}


//Sinergias que no queremos que se muestren en cada carta pq son debiles y muchas.
//Abarrotan la lista con sinergias innecesarias.
bool KeySynergies::isWeakKeySyn(const QString &key)
{
    QStringList weakKeys = {"return"};
    return weakKeys.contains(key);
}


//Usada por LayeredSynergies para devolver sinergias parciales que luego haran union
void KeySynergies::getPartKeySynergies(const QString &partSynergy, QMap<QString, QMap<QString, int> > &synergyTagMap)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll].insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);

        if(keySynergiesMap.contains(key))   keySynergiesMap[key].insertCards(synergyTagMap);
    }
    else if(partSynergy.endsWith("Gen"))
    {
        QString key = partSynergy;
        key.chop(3);
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll].insertSynCards(synergyTagMap);
    }
    else
    {
        QString key = partSynergy;
        const QString &keyAll = key+"All";

        if(keySynergiesMap.contains(key))   keySynergiesMap[key].insertSynCards(synergyTagMap);
        if(keySynergiesMap.contains(keyAll))keySynergiesMap[keyAll].insertSynCards(synergyTagMap);
    }
}


//Usada por LayeredSynergies para verificar que el code hace sinergia con cada una de las partSynergy
bool KeySynergies::isPartKey(const QString &partSynergy, const QString &code, QString &partSynergyTag,
                             const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, CardType cardType, int attack, int cost)
{
    if(partSynergy.endsWith("AllSyn"))
    {
        QString key = partSynergy;
        key.chop(6);
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;

        return isKey(key, code, mechanics, referencedTags, text, cardType, attack, cost) ||
               isKeyGen(key+"Gen", code, mechanics, referencedTags, text, cardType, attack, cost);
    }
    else if(partSynergy.endsWith("Syn"))
    {
        QString key = partSynergy;
        key.chop(3);
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
        const QString &keyAll = key+"All";

        return isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost);
    }
    else
    {
        QString key = partSynergy;
        partSynergyTag = getSynergyTag(key);
        if(partSynergyTag.isEmpty())    return false;
        const QString &keyAll = key+"All";

        return isKeySyn(key+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost) ||
               isKeyAllSyn(keyAll+"Syn", code, mechanics, referencedTags, text, cardType, attack, cost);
    }
}


QString KeySynergies::getSynergyTag(const QString &key)
{
    if(!keySynergiesMap.contains(key))  return "";
    return keySynergiesMap[key].synergyTag;
}


QStringList KeySynergies::getListKeySynergies()
{
    return getMapKeySynergies().keys();
}


QMap<QString, QString> KeySynergies::getMapKeySynergies()
{
    QMap<QString, QString> keys;

    keys["overload"] = "Overload";
    keys["secret"] = "Secret";
    keys["freezeEnemy"] = "Freeze";
    keys["discard"] = "Discard";
    keys["battlecry"] = "Battlecry";
    keys["silenceOwn"] = "Silence";
    keys["tauntGiver"] = "Taunt";
    keys["token"] = "Token";
    keys["tokenCard"] = "Token Card";
    keys["combo"] = "Combo";
    keys["windfuryMinion"] = "Windfury";
    keys["attackBuff"] = "Attack Buff";
    keys["attackNerf"] = "Attack Nerf";
    keys["healthBuff"] = "Health Buff";
    keys["return"] = "Return";
    keys["stealth"] = "Stealth";
    keys["spellDamage"] = "Spell Damage";
    keys["divineShield"] = "Divine Shield";
    keys["enrage"] = "Enrage";
    keys["restoreFriendlyMinion"] = "Heal";
    keys["restoreTargetMinion"] = "Heal";
    keys["evolve"] = "Evolve";
    keys["spawnEnemy"] = "Spawn Enemy";
    keys["lifesteal"] = "Lifesteal";
    keys["egg"] = "Egg";
    keys["damageFriendlyHero"] = "Hurt";
    keys["rush"] = "Rush";
    keys["echo"] = "Echo";
    keys["magnetic"] = "Magnetic";
    keys["handBuff"] = "Hand Buff";
    keys["enemyDraw"] = "Draw Enemy";
    keys["heroAttack"] = "Hero Attack";
    keys["spellBuff"] = "Enchant";
    keys["otherClass"] = "Other Class";
    keys["silverHand"] = "Silver Hand";
    keys["treant"] = "Treant";
    keys["lackey"] = "Lackey";
    keys["outcast"] = "Outcast";
    keys["chooseOne"] = "Choose One";
    keys["endTurn"] = "Each Turn";
    keys["rushGiver"] = "Rush";
    keys["dredge"] = "Dredge";
    keys["corpse"] = "Corpses";
    keys["excavate"] = "Excavate";
    keys["libram"] = "Libram";
    keys["starship"] = "Starship";
    keys["shuffle"] = "Shuffle";

    //New Synergy Step 1
    return keys;
}


//New Synergy Step 2
bool KeySynergies::isKey(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost)
{
    if(synergyCodes->contains(code))
    {
        if(key == "tokenCard")
        {
            return (*synergyCodes)[code].contains("tokenCard") || (*synergyCodes)[code].contains("lackey");
        }
        else if(key == "damageFriendlyHero")
        {
            if(cardType == WEAPON && attack > 0)
            {
                return true;
            }
            else
            {
                return ((*synergyCodes)[code].contains("damageFriendlyHero") || (*synergyCodes)[code].contains("weaponGen"));
            }
        }
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "freezeEnemy")
        {
            if(mechanics.contains(QJsonValue("FREEZE")))
            {
                return true;
            }
            else if(referencedTags.contains(QJsonValue("FREEZE")))
            {
                return !text.contains("frozen");
            }
            return false;
        }
        else if(key == "discard")
        {
            if(text.contains("discard") && text.contains("random card"))
            {
                return true;
            }
            return false;
        }
        else if(key == "silenceOwn")
        {
            if(mechanics.contains(QJsonValue("SILENCE")))
            {
                return true;
            }
            else if(referencedTags.contains(QJsonValue("SILENCE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "token")
        {
            if((text.contains("1/1") || text.contains("1/2") || text.contains("1/3") ||
                 text.contains("2/1") || text.contains("2/2") || text.contains("2/3") ||
                 text.contains("3/1") || text.contains("3/2") || text.contains("3/3")
                 )
                && text.contains("summon") && !text.contains("opponent"))
            {
                return true;
            }
            return false;
        }
        else if(key == "tokenCard")
        {
            if(cost == 0)                                                       return true;
            else if(cost == 1 && mechanics.contains(QJsonValue("TWINSPELL")))   return true;
            else if(cost == 2 && mechanics.contains(QJsonValue("TWINSPELL")))   return true;
            else if(cost < 4 && mechanics.contains(QJsonValue("ECHO")))         return true;
            else if(text.contains("lackey"))                                    return true;
            return false;
        }
        else if(key == "windfuryMinion")
        {
            if(cardType != MINION)  return false;
            else if(mechanics.contains(QJsonValue("WINDFURY")))
            {
                return true;
            }
            return false;
        }
        else if(key == "attackBuff")
        {
            if(text.contains("+")
                && (text.contains("give") || text.contains("have"))
                && (text.contains("minion") || text.contains("character"))
                && (text.contains("attack") || text.contains("/+"))
                && !text.contains("c'thun"))
            {
                return true;
            }
            return false;
        }
        else if(key == "attackNerf")
        {
            if(containsAll(text, "change minion attack 1") || containsAll(text, "set minion attack 1")
                || containsAll(text, "give minion attack -") || containsAll(text, "reduce minion attack"))
            {
                return true;
            }
            return false;
        }
        else if(key == "healthBuff")
        {
            if(text.contains("+")
                && (text.contains("give") || text.contains("have"))
                && (text.contains("minion") || text.contains("character"))
                && (text.contains("health") || text.contains("/+"))
                && !text.contains("c'thun"))
            {
                return true;
            }
            return false;
        }
        else if(key == "return")
        {
            if(text.contains("return a friendly minion"))
            {
                return true;
            }
            return false;
        }
        else if(key == "stealth")
        {
            if(mechanics.contains(QJsonValue("STEALTH")))
            {
                return true;
            }
            return false;
        }
        else if(key == "spellDamage")
        {
            int spellDamage = Utility::getCardAttribute(code, "spellDamage").toInt();
            return spellDamage > 0;
        }
        else if(key == "evolve")
        {
            if(text.contains("transform") && text.contains("cost") && text.contains("more"))
            {
                return true;
            }
            return false;
        }
        else if(key == "spawnEnemy")
        {
            if(text.contains("summon") && text.contains("opponent"))
            {
                return true;
            }
            return false;
        }
        else if(key == "restoreTargetMinion")
        {
            if(text.contains("restore") && !text.contains("friendly") && !text.contains("hero"))
            {
                return true;
            }
            return false;
        }
        else if(key == "restoreFriendlyMinion")
        {
            if(text.contains("restore") && text.contains("friendly") && !text.contains("hero"))
            {
                return true;
            }
            return false;
        }
        else if(key == "enrage")
        {
            if(mechanics.contains(QJsonValue("ENRAGED")) || mechanics.contains(QJsonValue("FRENZY")))
            {
                return true;
            }
            return false;
        }
        else if(key == "egg")
        {
            if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
            {
                return true;
            }
            return false;
        }
        else if(key == "damageFriendlyHero")
        {
            if(cardType == WEAPON && attack > 0)
            {
                return true;
            }
            return false;
        }
        else if(key == "handBuff")
        {
            if(text.contains("give") && text.contains("in your hand"))
            {
                return true;
            }
            return false;
        }
        else if(key == "enemyDraw")
        {
            if((text.contains("both players") || text.contains("each player") || text.contains("your opponent") || text.contains("your enemy")) &&
                text.contains("draw") && !text.contains("when drawn"))
            {
                return true;
            }
            return false;
        }
        else if(key == "heroAttack")
        {
            if((text.contains("hero") || text.contains("character")) && text.contains("attack"))
            {
                return true;
            }
            return false;
        }
        else if(key == "spellBuff")
        {
            if((text.contains("set") || text.contains("give")) &&
                ((text.contains("minion") && !text.contains("minions")) || (text.contains("character") && !text.contains("characters")))
                && cardType == SPELL && !mechanics.contains("SECRET"))
            {
                return true;
            }
            return false;
        }
        else if(key == "otherClass")
        {
            if((text.contains("opponent") || text.contains("another")) && text.contains("class"))
            {
                return true;
            }
            return false;
        }
        else if(key == "silverHand")
        {
            if(text.contains("silver hand"))
            {
                return true;
            }
            return false;
        }
        else if(key == "treant")
        {
            if(text.contains("treant"))
            {
                return true;
            }
            return false;
        }
        else if(key == "lackey")
        {
            if(text.contains("lackey"))
            {
                return true;
            }
            return false;
        }
        else if(key == "endTurn")
        {
            if((text.contains("at the end of ") || text.contains("at the start of ")))
            {
                return true;
            }
            return false;
        }
        else if(key == "rushGiver")
        {
            if(containsAll(text, "give rush"))
            {
                return true;
            }
            return false;
        }
        else if(key == "dredge")
        {
            if(mechanics.contains(QJsonValue("DREDGE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "corpse")
        {
            if(mechanics.contains(QJsonValue("REBORN")))   return true;
            else if((text.contains("1/1") || text.contains("1/2") || text.contains("1/3") ||
                     text.contains("2/1") || text.contains("2/2") || text.contains("2/3") ||
                     text.contains("3/1") || text.contains("3/2") || text.contains("3/3")
                     )
                    && text.contains("summon") && !text.contains("opponent"))
            {
                return true;
            }
            return false;
        }
        else if(key == "excavate")
        {
            if(text.contains("excavate"))
            {
                return true;
            }
            return false;
        }
        else if(key == "shuffle")
        {
            if(text.contains("shuffle"))
            {
                return true;
            }
            return false;
        }
        else if(key == "divineShield")
        {
            if(mechanics.contains(QJsonValue("DIVINE_SHIELD")))
            {
                return true;
            }
            return false;
        }
        else if(key == "overload")
        {
            int overload = Utility::getCardAttribute(code, "overload").toInt();
            return overload > 0;
        }
        else if(key == "secret")
        {
            if(mechanics.contains(QJsonValue("SECRET")))
            {
                return true;
            }
            return false;
        }
        else if(key == "echo")
        {
            if(mechanics.contains(QJsonValue("ECHO")))
            {
                return true;
            }
            return false;
        }
        else if(key == "rush")
        {
            if(mechanics.contains(QJsonValue("RUSH")))
            {
                return true;
            }
            return false;
        }
        else if(key == "magnetic")
        {
            if(mechanics.contains(QJsonValue("MAGNETIC")))
            {
                return true;
            }
            return false;
        }
        else if(key == "lifesteal")
        {
            if(mechanics.contains(QJsonValue("LIFESTEAL")))
            {
                return true;
            }
            return false;
        }
        else if(key == "battlecry")
        {
            if(mechanics.contains(QJsonValue("BATTLECRY")))
            {
                return true;
            }
            return false;
        }
        else if(key == "outcast")
        {
            if(mechanics.contains(QJsonValue("OUTCAST")))
            {
                return true;
            }
            return false;
        }
        else if(key == "chooseOne")
        {
            if(mechanics.contains(QJsonValue("CHOOSE_ONE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "starship")
        {
            if(mechanics.contains(QJsonValue("STARSHIP_PIECE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "combo")
        {
            if(mechanics.contains(QJsonValue("COMBO")))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool KeySynergies::isKeyGen(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost)
{
    Q_UNUSED(mechanics);
    Q_UNUSED(text);
    Q_UNUSED(cardType);
    Q_UNUSED(attack);
    Q_UNUSED(cost);

    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "divineShieldGen")
        {
            if(referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
            {
                return true;
            }
            return false;
        }
        else if(key == "overloadGen")
        {
            if(referencedTags.contains(QJsonValue("OVERLOAD")))
            {
                return true;
            }
            return false;
        }
        else if(key == "echoGen")
        {
            if(referencedTags.contains(QJsonValue("ECHO")))
            {
                return true;
            }
            return false;
        }
        else if(key == "rushGen")
        {
            if(referencedTags.contains(QJsonValue("RUSH")))
            {
                return true;
            }
            return false;
        }
        else if(key == "lifestealGen")
        {
            if(referencedTags.contains(QJsonValue("LIFESTEAL")))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool KeySynergies::isKeySyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost)
{
    Q_UNUSED(cost);

    if(synergyCodes->contains(code))
    {
        if(key == "tauntGiverSyn")
        {
            return (*synergyCodes)[code].contains("tauntGiverSyn") || (*synergyCodes)[code].contains("deathrattleOpponent") ||
                   (*synergyCodes)[code].contains("egg");
        }
        else if(key == "attackBuffSyn")
        {
            return (*synergyCodes)[code].contains("attackBuffSyn") || (*synergyCodes)[code].contains("egg");
        }
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "freezeEnemySyn")
        {
            if(referencedTags.contains(QJsonValue("FREEZE")))
            {
                return text.contains("frozen");
            }
            return false;
        }
        else if(key == "discardSyn")
        {
            if(text.contains("discard") && !text.contains("random card"))
            {
                return true;
            }
            return false;
        }
        else if(key == "silenceOwnSyn")
        {
            if(mechanics.contains(QJsonValue("CANT_ATTACK")))
            {
                return true;
            }
            return false;
        }
        else if(key == "tauntGiverSyn")
        {
            if(mechanics.contains(QJsonValue("CANT_ATTACK")))
            {
                return true;
            }
            else if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
            {
                return true;
            }
            return false;
        }
        else if(key == "tokenSyn")
        {
            if(mechanics.contains(QJsonValue("INFUSE")))
            {
                return true;
            }
            else if(text.contains("+")
                     && (text.contains("minions") || text.contains("characters"))
                     && !text.contains("hand") && !text.contains("health"))
            {
                return true;
            }
            else if(text.contains("control") && text.contains("least") && text.contains("minions"))
            {
                return true;
            }
            return false;
        }
        else if(key == "tokenCardSyn")
        {
            if(text.contains("play") && text.contains("card") && !text.contains("player"))
            {
                return true;
            }
            return false;
        }
        else if(key == "attackBuffSyn")
        {
            if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
            {
                return true;
            }
            return false;
        }
        else if(key == "attackNerfSyn")
        {
            if(containsAll(text, "destroy less attack") || containsAll(text, "control less attack"))
            {
                return true;
            }
            return false;
        }
        else if(key == "returnSyn")
        {
            if(cardType != MINION)  return false;
            else if(mechanics.contains(QJsonValue("BATTLECRY")) || mechanics.contains(QJsonValue("COMBO")) ||
                     mechanics.contains(QJsonValue("CHOOSE_ONE")) || mechanics.contains(QJsonValue("COLOSSAL")))
            {
                if(
                    ((text.contains("summon") || text.contains("give") || text.contains("restore")) && !text.contains("opponent")) ||
                    (text.contains("deal") && !(text.contains("your") && text.contains("hero"))) ||
                    text.contains("swap") ||
                    (text.contains("play") && !text.contains("played")) ||
                    text.contains("discover") ||
                    text.contains("draw") ||
                    (text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return")) ||
                    text.contains("destroy") ||
                    text.contains("transform") ||
                    (text.contains("take") && text.contains("control")) ||
                    text.contains("freeze") ||
                    text.contains("remove") ||
                    text.contains("silence") ||
                    text.contains("cast") ||
                    text.contains("equip") ||
                    text.contains("recruit")
                    )
                {
                    return true;
                }
            }
            else if(mechanics.contains(QJsonValue("ECHO")))
            {
                return true;
            }
            return false;
        }
        else if(key == "spellDamageSyn")
        {
            if(cardType != SPELL)  return false;
            else if(mechanics.contains(QJsonValue("SECRET")))   return false;
            else if(
                (text.contains("all") || text.contains("adjacent")) && (text.contains("damage"))
                && !text.contains("random")
                )
            {
                return true;
            }
            return false;
        }
        else if(key == "restoreTargetMinionSyn")
        {
            if(text.contains("overheal"))
            {
                return true;
            }
            return false;
        }
        else if(key == "restoreFriendlyMinionSyn")
        {
            if(text.contains("overheal"))
            {
                return true;
            }
            return false;
        }
        else if(key == "enrageSyn")
        {
            if(text.contains("deal") && text.contains("1 damage") &&
                !text.contains("enemy") && !text.contains("random") && !text.contains("hero"))
            {
                return true;
            }
            return false;
        }
        else if(key == "eggSyn")
        {
            if(text.contains("swap") && text.contains("attack") && text.contains("health") && text.contains("minion"))
            {
                return true;
            }
            return false;
        }
        else if(key == "handBuffSyn")
        {
            if((text.contains("enchantments")) || (text.contains("of this minion")))
            {
                return true;
            }
            return false;
        }
        else if(key == "enemyDrawSyn")
        {
            if(text.contains("shuffle") && (text.contains("opponent") || text.contains("enemy")) &&
                text.contains("deck") && text.contains("when drawn"))
            {
                return true;
            }
            return false;
        }
        else if(key == "spellBuffSyn")
        {
            if((text.contains("spell") && text.contains("cast") && text.contains("minion")) || text.contains("enchantments"))
            {
                return true;
            }
            return false;
        }
        else if(key == "otherClassSyn")
        {
            if(text.contains("card") && text.contains("from another class"))
            {
                return true;
            }
            return false;
        }
        else if(key == "endTurnSyn")
        {
            if((containsAll(text, "copy") || containsAll(text, "copies")) &&
                (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
                !containsAll(text, "in your deck"))
            {
                return true;
            }
            return false;
        }
        else if(key == "rushGiverSyn")
        {
            if(mechanics.contains(QJsonValue("ENRAGED")) || mechanics.contains(QJsonValue("FRENZY")))
            {
                return true;
            }
            else if(text.contains("poisonous") || text.contains("also damages"))
            {
                return true;
            }
            return false;
        }
        else if(key == "dredgeSyn")
        {
            if(containsAll(text, "bottom deck"))
            {
                return true;
            }
            return false;
        }
        else if(key == "corpseSyn")
        {
            if(text.contains("corpse"))
            {
                return true;
            }
            return false;
        }
        else if(key == "excavateSyn")
        {
            if(text.contains("excavate"))
            {
                return true;
            }
            return false;
        }
        else if(key == "shuffleSyn")
        {
            if(containsAll(text, "deck didn't start"))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool KeySynergies::isKeyAllSyn(const QString &key, const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                         const QString &text, CardType cardType, int attack, int cost)
{
    Q_UNUSED(mechanics);
    Q_UNUSED(text);
    Q_UNUSED(cardType);
    Q_UNUSED(attack);
    Q_UNUSED(cost);

    if(synergyCodes->contains(code))
    {
        return (*synergyCodes)[code].contains(key);
    }
    else
    {
        if(key == "secretAllSyn")
        {
            if(referencedTags.contains(QJsonValue("SECRET")))
            {
                return true;
            }
            return false;
        }
        else if(key == "battlecryAllSyn")
        {
            if(referencedTags.contains(QJsonValue("BATTLECRY")))
            {
                return true;
            }
            return false;
        }
        else if(key == "outcastAllSyn")
        {
            if(referencedTags.contains(QJsonValue("OUTCAST")))
            {
                return true;
            }
            return false;
        }
        else if(key == "chooseOneAllSyn")
        {
            if(referencedTags.contains(QJsonValue("CHOOSE_ONE")))
            {
                return true;
            }
            return false;
        }
        else if(key == "starshipAllSyn")
        {
            if(referencedTags.contains(QJsonValue("STARSHIP")))
            {
                return true;
            }
            return false;
        }
        else if(key == "comboAllSyn")
        {
            if(referencedTags.contains(QJsonValue("COMBO")))
            {
                return true;
            }
            return false;
        }
    }

    return false;
}

