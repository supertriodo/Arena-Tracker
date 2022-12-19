#include "synergyhandler.h"
#include "themehandler.h"
#include <QtWidgets>

SynergyHandler::SynergyHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;

    createDraftItemCounters();
}


SynergyHandler::~SynergyHandler()
{
    deleteDraftItemCounters();
}


void SynergyHandler::createDraftItemCounters()
{
    QGridLayout *mechanicsLayout = new QGridLayout();


    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, "Minion", "Minion Gen", mechanicsLayout, 0, 0,
                                                    QPixmap(ThemeHandler::minionsCounterFile()), 32, true, false);
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, "Spell", "Spell Gen", mechanicsLayout, 0, 1,
                                                     QPixmap(ThemeHandler::spellsCounterFile()), 32, true, false);
    cardTypeCounters[V_SPELL_ALL] = new DraftItemCounter(this, "Spell");
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, "Weapon", "Weapon Gen", mechanicsLayout, 0, 2,
                                                      QPixmap(ThemeHandler::weaponsCounterFile()), 32, true, false);
    cardTypeCounters[V_WEAPON_ALL] = new DraftItemCounter(this, "Weapon");

    connect(cardTypeCounters[V_MINION], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(cardTypeCounters[V_SPELL], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(cardTypeCounters[V_WEAPON], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));

    connect(cardTypeCounters[V_MINION], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(cardTypeCounters[V_SPELL], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(cardTypeCounters[V_WEAPON], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));

    manaCounter = new DraftItemCounter(this, "Mana AVG", "Mana AVG", mechanicsLayout, 0, 3,
                                       QPixmap(ThemeHandler::manaCounterFile()), 32, false, false);

    dropCounters = new DraftDropCounter *[V_NUM_DROPS];
    dropCounters[V_DROP2] = new DraftDropCounter(this, "2 Drop", "2 Cost", mechanicsLayout, 1, 0, TARGET_DROP_2,
                                                 QPixmap(ThemeHandler::drop2CounterFile()), 32, true, false);
    dropCounters[V_DROP3] = new DraftDropCounter(this, "3 Drop", "3 Cost", mechanicsLayout, 1, 1, TARGET_DROP_3,
                                                 QPixmap(ThemeHandler::drop3CounterFile()), 32, true, false);
    dropCounters[V_DROP4] = new DraftDropCounter(this, "4 Drop", "4 Cost", mechanicsLayout, 1, 2, TARGET_DROP_4,
                                                 QPixmap(ThemeHandler::drop4CounterFile()), 32, true, false);

    connect(dropCounters[V_DROP2], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(dropCounters[V_DROP3], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(dropCounters[V_DROP4], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));

    connect(dropCounters[V_DROP2], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(dropCounters[V_DROP3], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(dropCounters[V_DROP4], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));

    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(this, "Elemental");
    raceCounters[V_BEAST] = new DraftItemCounter(this, "Beast");
    raceCounters[V_MURLOC] = new DraftItemCounter(this, "Murloc");
    raceCounters[V_DRAGON] = new DraftItemCounter(this, "Dragon");
    raceCounters[V_PIRATE] = new DraftItemCounter(this, "Pirate");
    raceCounters[V_MECHANICAL] = new DraftItemCounter(this, "Mech");
    raceCounters[V_DEMON] = new DraftItemCounter(this, "Demon");
    raceCounters[V_TOTEM] = new DraftItemCounter(this, "Totem");
    raceCounters[V_NAGA] = new DraftItemCounter(this, "Naga");
    raceCounters[V_UNDEAD] = new DraftItemCounter(this, "Undead");

    raceCounters[V_ELEMENTAL_ALL] = new DraftItemCounter(this, "Elemental");
    raceCounters[V_BEAST_ALL] = new DraftItemCounter(this, "Beast");
    raceCounters[V_MURLOC_ALL] = new DraftItemCounter(this, "Murloc");
    raceCounters[V_DRAGON_ALL] = new DraftItemCounter(this, "Dragon");
    raceCounters[V_PIRATE_ALL] = new DraftItemCounter(this, "Pirate");
    raceCounters[V_MECHANICAL_ALL] = new DraftItemCounter(this, "Mech");
    raceCounters[V_DEMON_ALL] = new DraftItemCounter(this, "Demon");
    raceCounters[V_TOTEM_ALL] = new DraftItemCounter(this, "Totem");
    raceCounters[V_NAGA_ALL] = new DraftItemCounter(this, "Naga");
    raceCounters[V_UNDEAD_ALL] = new DraftItemCounter(this, "Undead");

    schoolCounters = new DraftItemCounter *[V_NUM_SCHOOLS];
    schoolCounters[V_ARCANE] = new DraftItemCounter(this, "Arcane");
    schoolCounters[V_FEL] = new DraftItemCounter(this, "Fel");
    schoolCounters[V_FIRE] = new DraftItemCounter(this, "Fire");
    schoolCounters[V_FROST] = new DraftItemCounter(this, "Frost");
    schoolCounters[V_HOLY] = new DraftItemCounter(this, "Holy");
    schoolCounters[V_SHADOW] = new DraftItemCounter(this, "Shadow");
    schoolCounters[V_NATURE] = new DraftItemCounter(this, "Nature");

    schoolCounters[V_ARCANE_ALL] = new DraftItemCounter(this, "Arcane");
    schoolCounters[V_FEL_ALL] = new DraftItemCounter(this, "Fel");
    schoolCounters[V_FIRE_ALL] = new DraftItemCounter(this, "Fire");
    schoolCounters[V_FROST_ALL] = new DraftItemCounter(this, "Frost");
    schoolCounters[V_HOLY_ALL] = new DraftItemCounter(this, "Holy");
    schoolCounters[V_SHADOW_ALL] = new DraftItemCounter(this, "Shadow");
    schoolCounters[V_NATURE_ALL] = new DraftItemCounter(this, "Nature");

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_REACH] = new DraftItemCounter(this, "Reach", "Reach", mechanicsLayout, 2, 0,
                                                     QPixmap(ThemeHandler::reachMechanicFile()), 32, true, false);
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(this, "Taunt", "Taunt", mechanicsLayout, 2, 1,
                                                         QPixmap(ThemeHandler::tauntMechanicFile()), 32, true, false);
    mechanicCounters[V_SURVIVABILITY] = new DraftItemCounter(this, "Survival", "Survival", mechanicsLayout, 2, 2,
                                                             QPixmap(ThemeHandler::survivalMechanicFile()), 32, true, false);
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, "Draw", "Draw", mechanicsLayout, 2, 3,
                                                             QPixmap(ThemeHandler::drawMechanicFile()), 32, true, false);

    mechanicCounters[V_PING] = new DraftItemCounter(this, "Ping", "Ping", mechanicsLayout, 3, 0,
                                                    QPixmap(ThemeHandler::pingMechanicFile()), 32, true, false);
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(this, "Removal", "Removal", mechanicsLayout, 3, 1,
                                                      QPixmap(ThemeHandler::damageMechanicFile()), 32, true, false);
    mechanicCounters[V_DESTROY] = new DraftItemCounter(this, "Hard Removal", "Hard Removal", mechanicsLayout, 3, 2,
                                                       QPixmap(ThemeHandler::destroyMechanicFile()), 32, true, false);
    mechanicCounters[V_AOE] = new DraftItemCounter(this, "AOE", "AOE", mechanicsLayout, 3, 3,
                                                   QPixmap(ThemeHandler::aoeMechanicFile()), 32, true, false);

    connect(mechanicCounters[V_AOE], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_PING], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(mechanicCounters[V_REACH], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));

    connect(mechanicCounters[V_AOE], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_PING], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
    connect(mechanicCounters[V_REACH], SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));

    mechanicCounters[V_DISCOVER] = new DraftItemCounter(this, "Discover");
    mechanicCounters[V_DRAW] = new DraftItemCounter(this, "Draw");
    mechanicCounters[V_TOYOURHAND] = new DraftItemCounter(this, "Gen Cards");
    mechanicCounters[V_TAUNT] = new DraftItemCounter(this, "Taunt");
    mechanicCounters[V_OVERLOAD] = new DraftItemCounter(this, "Overload");
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this, "Jade Golem");
    mechanicCounters[V_HERO_POWER] = new DraftItemCounter(this, "Hero Power");
    mechanicCounters[V_SECRET] = new DraftItemCounter(this, "Secret");
    mechanicCounters[V_SECRET_ALL] = new DraftItemCounter(this, "Secret");
    mechanicCounters[V_FREEZE_ENEMY] = new DraftItemCounter(this, "Freeze");
    mechanicCounters[V_DISCARD] = new DraftItemCounter(this, "Discard");
    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this, "Deathrattle");
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(this, "Deathrattle");
    mechanicCounters[V_BATTLECRY] = new DraftItemCounter(this, "Battlecry");
    mechanicCounters[V_BATTLECRY_ALL] = new DraftItemCounter(this, "Battlecry");
    mechanicCounters[V_SILENCE] = new DraftItemCounter(this, "Silence");
    mechanicCounters[V_TAUNT_GIVER] = new DraftItemCounter(this, "Taunt");
    mechanicCounters[V_TOKEN] = new DraftItemCounter(this, "Token");
    mechanicCounters[V_TOKEN_CARD] = new DraftItemCounter(this, "Token Card");
    mechanicCounters[V_COMBO] = new DraftItemCounter(this, "Combo");
    mechanicCounters[V_COMBO_ALL] = new DraftItemCounter(this, "Combo");
    mechanicCounters[V_WINDFURY_MINION] = new DraftItemCounter(this, "Windfury");
    mechanicCounters[V_ATTACK_BUFF] = new DraftItemCounter(this, "Attack Buff");
    mechanicCounters[V_ATTACK_NERF] = new DraftItemCounter(this, "Attack Nerf");
    mechanicCounters[V_HEALTH_BUFF] = new DraftItemCounter(this, "Health Buff");
    mechanicCounters[V_RETURN] = new DraftItemCounter(this, "Return");
    mechanicCounters[V_STEALTH] = new DraftItemCounter(this, "Stealth");
    mechanicCounters[V_SPELL_DAMAGE] = new DraftItemCounter(this, "Spell Damage");
    mechanicCounters[V_DIVINE_SHIELD] = new DraftItemCounter(this, "Divine Shield");
    mechanicCounters[V_DIVINE_SHIELD_ALL] = new DraftItemCounter(this, "Divine Shield");
    mechanicCounters[V_ENRAGED] = new DraftItemCounter(this, "Enrage");
    mechanicCounters[V_RESTORE_FRIENDLY_MINION] = new DraftItemCounter(this, "Heal");
    mechanicCounters[V_RESTORE_TARGET_MINION] = new DraftItemCounter(this, "Heal");
    mechanicCounters[V_RESTORE_FRIENDLY_HEROE] = new DraftItemCounter(this, "Heal");
    mechanicCounters[V_ARMOR] = new DraftItemCounter(this, "Armor");
    mechanicCounters[V_EVOLVE] = new DraftItemCounter(this, "Evolve");
    mechanicCounters[V_SPAWN_ENEMY] = new DraftItemCounter(this, "Spawn Enemy");
    mechanicCounters[V_LIFESTEAL_MINION] = new DraftItemCounter(this, "Lifesteal");
    mechanicCounters[V_LIFESTEAL_ALL] = new DraftItemCounter(this, "Lifesteal");
    mechanicCounters[V_EGG] = new DraftItemCounter(this, "Egg");
    mechanicCounters[V_DAMAGE_FRIENDLY_HERO] = new DraftItemCounter(this, "Hurt");
    mechanicCounters[V_RUSH] = new DraftItemCounter(this, "Rush");
    mechanicCounters[V_RUSH_ALL] = new DraftItemCounter(this, "Rush");
    mechanicCounters[V_ECHO] = new DraftItemCounter(this, "Echo");
    mechanicCounters[V_ECHO_ALL] = new DraftItemCounter(this, "Echo");
    mechanicCounters[V_MAGNETIC] = new DraftItemCounter(this, "Magnetic");
    mechanicCounters[V_MAGNETIC_ALL] = new DraftItemCounter(this, "Magnetic");
    mechanicCounters[V_HAND_BUFF] = new DraftItemCounter(this, "Hand Buff");
    mechanicCounters[V_ENEMY_DRAW] = new DraftItemCounter(this, "Draw Enemy");
    mechanicCounters[V_HERO_ATTACK] = new DraftItemCounter(this, "Hero Attack");
    mechanicCounters[V_SPELL_BUFF] = new DraftItemCounter(this, "Enchant");
    mechanicCounters[V_OTHER_CLASS] = new DraftItemCounter(this, "Other Class");
    mechanicCounters[V_SILVER_HAND] = new DraftItemCounter(this, "Silver Hand");
    mechanicCounters[V_TREANT] = new DraftItemCounter(this, "Treant");
    mechanicCounters[V_LACKEY] = new DraftItemCounter(this, "Lackey");
    mechanicCounters[V_OUTCAST] = new DraftItemCounter(this, "Outcast");
    mechanicCounters[V_OUTCAST_ALL] = new DraftItemCounter(this, "Outcast");
    mechanicCounters[V_END_TURN] = new DraftItemCounter(this, "Each Turn");
    mechanicCounters[V_RUSH_GIVER] = new DraftItemCounter(this, "Rush");
    mechanicCounters[V_DREDGE] = new DraftItemCounter(this, "Dredge");
    mechanicCounters[V_CORPSE] = new DraftItemCounter(this, "Corpses");
    //New Synergy Step 2


    QHBoxLayout *horLayoutMechanics = new QHBoxLayout();
    horLayoutMechanics->addLayout(mechanicsLayout);
    horLayoutMechanics->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutMechanics);
}


void SynergyHandler::deleteDraftItemCounters()
{
    delete manaCounter;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        delete cardTypeCounters[i];
    }
    delete []cardTypeCounters;

    for(int i=0; i<V_NUM_DROPS; i++)
    {
        delete dropCounters[i];
    }
    delete []dropCounters;

    for(int i=0; i<V_NUM_RACES; i++)
    {
        delete raceCounters[i];
    }
    delete []raceCounters;

    for(int i=0; i<V_NUM_SCHOOLS; i++)
    {
        delete schoolCounters[i];
    }
    delete []schoolCounters;

    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        delete mechanicCounters[i];
    }
    delete []mechanicCounters;
}


void SynergyHandler::setTheme()
{
    cardTypeCounters[V_MINION]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()), 32, false);
    cardTypeCounters[V_SPELL]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()), 32, false);
    cardTypeCounters[V_WEAPON]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()), 32, false);
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()), 32, false);

    dropCounters[V_DROP2]->setTheme(QPixmap(ThemeHandler::drop2CounterFile()), 32, false);
    dropCounters[V_DROP3]->setTheme(QPixmap(ThemeHandler::drop3CounterFile()), 32, false);
    dropCounters[V_DROP4]->setTheme(QPixmap(ThemeHandler::drop4CounterFile()), 32, false);

    mechanicCounters[V_AOE]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()), 32, false);
    mechanicCounters[V_TAUNT_ALL]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()), 32, false);
    mechanicCounters[V_SURVIVABILITY]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()), 32, false);
    mechanicCounters[V_DISCOVER_DRAW]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()), 32, false);

    mechanicCounters[V_PING]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()), 32, false);
    mechanicCounters[V_DAMAGE]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()), 32, false);
    mechanicCounters[V_DESTROY]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()), 32, false);
    mechanicCounters[V_REACH]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()), 32, false);
}



void SynergyHandler::sendItemEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect)
{
    int iconTop = labelRect.top();
    int iconBottom = labelRect.bottom();
    QPoint mainTopLeft = ui->tabDraft->mapToGlobal(QPoint(0,0));
    QPoint mainBottomRight = ui->tabDraft->mapToGlobal(QPoint(ui->tabDraft->width(),ui->tabDraft->height()));

    int maxTop = -1;//mainTopLeft.y();
    int maxBottom = mainBottomRight.y();
    QPoint rectTopLeft(mainTopLeft.x(), iconTop);
    QPoint rectBottomRight(mainBottomRight.x(), iconBottom);
    QRect rect(rectTopLeft, rectBottomRight);

    emit itemEnter(synergyCardList, rect, maxTop, maxBottom);
}


bool SynergyHandler::initSynergyCodes(bool all)
{
    if(!all && !synergyCodes.isEmpty())
    {
        emit pDebug("Synergy Cards: Skip reduced synergy lists creation.");
        return false;
    }

    synergyCodes.clear();
    directLinks.clear();

    QFile jsonFile(Utility::extraPath() + "/synergies.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonObject = jsonDoc.object();
    QStringList coreCodes = Utility::getSetCodes("CORE", true, true);
    QStringList arenaCodes = getAllArenaCodes();

    for(const QString &code: (const QStringList)jsonObject.keys())
    {
        if(jsonObject.value(code).isArray())
        {
            QJsonArray synergies = jsonObject.value(code).toArray();
            if(all || arenaCodes.contains(code))       initSynergyCode(code, synergies);

            QString coreCode = "CORE_" + code;
            if(coreCodes.contains(coreCode))    initSynergyCode(coreCode, synergies);
        }
    }
    emit pDebug("Synergy Cards " + QString(all?"ALL":"MIN") + ": " + QString::number(synergyCodes.count()));


    //Direct links
    QJsonObject dlObject = jsonObject.value("DIRECT_LINKS").toObject();
    for(const QString &code: (const QStringList)dlObject.keys())
    {
        if(dlObject.value(code).isArray())
        {
            QJsonArray synergies = dlObject.value(code).toArray();
            if(all || arenaCodes.contains(code))
            {
                for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
                {
                    QString code2 = it->toString();
                    initDirectLink(code, code2, coreCodes, arenaCodes, all);
                }
            }

            QString coreCode = "CORE_" + code;
            if(coreCodes.contains(coreCode))
            {
                for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
                {
                    QString code2 = it->toString();
                    initDirectLink(coreCode, code2, coreCodes, arenaCodes, all);
                }
            }
        }
    }
    emit pDebug("Direct Link Cards " + QString(all?"ALL":"MIN") + ": " + QString::number(directLinks.count()));

    return true;
}


void SynergyHandler::initSynergyCode(const QString &code, const QJsonArray &synergies)
{
    if(synergyCodes.contains(code)) return;

    synergyCodes[code];
    for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
    {
        synergyCodes[code].append(it->toString());
    }
}


void SynergyHandler::initDirectLink(const QString &code, const QString &code2,
                                    const QStringList &coreCodes, const QStringList &arenaCodes, bool all)
{
    if(all || arenaCodes.contains(code2))
    {
        directLinks[code].append(code2);
        directLinks[code2].append(code);
    }

    QString coreCode2 = "CORE_" + code2;
    if(coreCodes.contains(coreCode2))
    {
        directLinks[code].append(coreCode2);
        directLinks[coreCode2].append(code);
    }
}


void SynergyHandler::clearSynergyLists()
{
    synergyCodes.clear();
    directLinks.clear();
}


void SynergyHandler::clearCounters()
{
    //Reset counters
    manaCounter->reset();
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->reset();
    }
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->reset();
    }
    for(int i=0; i<V_NUM_RACES; i++)
    {
        raceCounters[i]->reset();
    }
    for(int i=0; i<V_NUM_SCHOOLS; i++)
    {
        schoolCounters[i]->reset();
    }
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->reset();
    }

    //Reset stats maps
    costMinions.clear();
    attackMinions.clear();
    healthMinions.clear();
    costSpells.clear();
    costWeapons.clear();
    attackWeapons.clear();
    healthWeapons.clear();
}


QStringList SynergyHandler::getAllArenaCodes()
{
    QStringList codeList;

    for(const QString &set: qAsConst(arenaSets))
    {
        if(Utility::needCodesSpecific(set)) codeList.append(Utility::getSetCodesSpecific(set));
        else                                codeList.append(Utility::getSetCodes(set, true, true));
    }
    return codeList;
}


void SynergyHandler::setArenaSets(QStringList arenaSets)
{
    this->arenaSets = arenaSets;
}


void SynergyHandler::setHidden(bool hide)
{
    if(hide)
    {
        cardTypeCounters[V_MINION]->hide();
        cardTypeCounters[V_SPELL]->hide();
        cardTypeCounters[V_WEAPON]->hide();
        manaCounter->hide();

        dropCounters[V_DROP2]->hide();
        dropCounters[V_DROP3]->hide();
        dropCounters[V_DROP4]->hide();

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
        cardTypeCounters[V_MINION]->show();
        cardTypeCounters[V_SPELL]->show();
        cardTypeCounters[V_WEAPON]->show();
        manaCounter->show();

        dropCounters[V_DROP2]->show();
        dropCounters[V_DROP3]->show();
        dropCounters[V_DROP4]->show();

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


int SynergyHandler::draftedCardsCount()
{
    int num = 0;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        num += cardTypeCounters[i]->count();
    }
    return num;
}


void SynergyHandler::setTransparency(Transparency transparency, bool mouseInApp)
{
    manaCounter->setTransparency(transparency, mouseInApp);
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        cardTypeCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


int SynergyHandler::getCounters(
        QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
        QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
        QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
        QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
        QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
        QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
        int &draw, int &toYourHand, int &discover)
{
    spellMap = cardTypeCounters[V_SPELL]->getCodeTagMap();
    minionMap = cardTypeCounters[V_MINION]->getCodeTagMap();
    weaponMap = cardTypeCounters[V_WEAPON]->getCodeTagMap();

    drop2Map = dropCounters[V_DROP2]->getCodeTagMap();
    drop3Map = dropCounters[V_DROP3]->getCodeTagMap();
    drop4Map = dropCounters[V_DROP4]->getCodeTagMap();

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

    return manaCounter->count();
}


int SynergyHandler::getManaCounterCount()
{
    return manaCounter->count();
}


void SynergyHandler::updateCounters(
        DeckCard &deckCard,
        QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
        QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
        QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
        QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
        QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
        QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
        int &draw, int &toYourHand, int &discover)
{
    updateRaceCounters(deckCard);
    updateSchoolCounters(deckCard);
    updateCardTypeCounters(deckCard, spellMap, minionMap, weaponMap);
    updateDropCounters(deckCard, drop2Map, drop3Map, drop4Map);
    updateManaCounter(deckCard);
    updateMechanicCounters(deckCard, aoeMap, tauntMap, survivabilityMap, drawMap,
                           pingMap, damageMap, destroyMap, reachMap,
                           draw, toYourHand, discover);
    updateStatsCards(deckCard);
}


void SynergyHandler::updateManaCounter(DeckCard &deckCard)
{
    manaCounter->increase(getCorrectedCardMana(deckCard), draftedCardsCount());
}


void SynergyHandler::updateRaceCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    QList<CardRace> cardRace = deckCard.getRace();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();

    if(cardRace.contains(MURLOC) || cardRace.contains(ALL))
    {
        raceCounters[V_MURLOC]->increase(code);
        raceCounters[V_MURLOC_ALL]->increase(code);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->increase(code);
    if(cardRace.contains(DEMON) || cardRace.contains(ALL))
    {
        raceCounters[V_DEMON]->increase(code);
        raceCounters[V_DEMON_ALL]->increase(code);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->increase(code);
    if(cardRace.contains(MECHANICAL) || cardRace.contains(ALL))
    {
        raceCounters[V_MECHANICAL]->increase(code);
        raceCounters[V_MECHANICAL_ALL]->increase(code);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->increase(code);
    if(cardRace.contains(ELEMENTAL) || cardRace.contains(ALL))
    {
        raceCounters[V_ELEMENTAL]->increase(code);
        raceCounters[V_ELEMENTAL_ALL]->increase(code);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->increase(code);
    if(cardRace.contains(BEAST) || cardRace.contains(ALL))
    {
        raceCounters[V_BEAST]->increase(code);
        raceCounters[V_BEAST_ALL]->increase(code);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->increase(code);
    if(cardRace.contains(TOTEM) || cardRace.contains(ALL))
    {
        raceCounters[V_TOTEM]->increase(code);
        raceCounters[V_TOTEM_ALL]->increase(code);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->increase(code);
    if(cardRace.contains(PIRATE) || cardRace.contains(ALL))
    {
        raceCounters[V_PIRATE]->increase(code);
        raceCounters[V_PIRATE_ALL]->increase(code);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->increase(code);
    if(cardRace.contains(DRAGON) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAGON]->increase(code);
        raceCounters[V_DRAGON_ALL]->increase(code);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->increase(code);
    if(cardRace.contains(NAGA) || cardRace.contains(ALL))
    {
        raceCounters[V_NAGA]->increase(code);
        raceCounters[V_NAGA_ALL]->increase(code);
    }
    else if(isNagaGen(code))        raceCounters[V_NAGA_ALL]->increase(code);
    if(cardRace.contains(UNDEAD) || cardRace.contains(ALL))
    {
        raceCounters[V_UNDEAD]->increase(code);
        raceCounters[V_UNDEAD_ALL]->increase(code);
    }
    else if(isUndeadGen(code))      raceCounters[V_UNDEAD_ALL]->increase(code);

    if(isMurlocSyn(code))                           raceCounters[V_MURLOC]->increaseSyn(code);
    else if(isMurlocAllSyn(code, text))             raceCounters[V_MURLOC_ALL]->increaseSyn(code);
    if(isDemonSyn(code))                            raceCounters[V_DEMON]->increaseSyn(code);
    else if(isDemonAllSyn(code, text))              raceCounters[V_DEMON_ALL]->increaseSyn(code);
    if(isMechSyn(code))                             raceCounters[V_MECHANICAL]->increaseSyn(code);
    else if(isMechAllSyn(code, mechanics, text))    raceCounters[V_MECHANICAL_ALL]->increaseSyn(code);
    if(isElementalSyn(code))                        raceCounters[V_ELEMENTAL]->increaseSyn(code);
    else if(isElementalAllSyn(code, text))          raceCounters[V_ELEMENTAL_ALL]->increaseSyn(code);
    if(isBeastSyn(code))                            raceCounters[V_BEAST]->increaseSyn(code);
    else if(isBeastAllSyn(code, text))              raceCounters[V_BEAST_ALL]->increaseSyn(code);
    if(isTotemSyn(code))                            raceCounters[V_TOTEM]->increaseSyn(code);
    else if(isTotemAllSyn(code, text))              raceCounters[V_TOTEM_ALL]->increaseSyn(code);
    if(isPirateSyn(code))                           raceCounters[V_PIRATE]->increaseSyn(code);
    else if(isPirateAllSyn(code, text))             raceCounters[V_PIRATE_ALL]->increaseSyn(code);
    if(isDragonSyn(code))                           raceCounters[V_DRAGON]->increaseSyn(code);
    else if(isDragonAllSyn(code, text))             raceCounters[V_DRAGON_ALL]->increaseSyn(code);
    if(isNagaSyn(code))                             raceCounters[V_NAGA]->increaseSyn(code);
    else if(isNagaAllSyn(code, text))               raceCounters[V_NAGA_ALL]->increaseSyn(code);
    if(isUndeadSyn(code))                           raceCounters[V_UNDEAD]->increaseSyn(code);
    else if(isUndeadAllSyn(code, text))             raceCounters[V_UNDEAD_ALL]->increaseSyn(code);
}


void SynergyHandler::updateSchoolCounters(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardSchool cardSchool = deckCard.getSchool();

    if(cardSchool == ARCANE)
    {
        schoolCounters[V_ARCANE]->increase(code);
        schoolCounters[V_ARCANE_ALL]->increase(code);
    }
    else if(isArcaneGen(code))      schoolCounters[V_ARCANE_ALL]->increase(code);
    if(cardSchool == FEL)
    {
        schoolCounters[V_FEL]->increase(code);
        schoolCounters[V_FEL_ALL]->increase(code);
    }
    else if(isFelGen(code))         schoolCounters[V_FEL_ALL]->increase(code);
    if(cardSchool == FIRE)
    {
        schoolCounters[V_FIRE]->increase(code);
        schoolCounters[V_FIRE_ALL]->increase(code);
    }
    else if(isFireGen(code))        schoolCounters[V_FIRE_ALL]->increase(code);
    if(cardSchool == FROST)
    {
        schoolCounters[V_FROST]->increase(code);
        schoolCounters[V_FROST_ALL]->increase(code);
    }
    else if(isFrostGen(code))       schoolCounters[V_FROST_ALL]->increase(code);
    if(cardSchool == HOLY)
    {
        schoolCounters[V_HOLY]->increase(code);
        schoolCounters[V_HOLY_ALL]->increase(code);
    }
    else if(isHolyGen(code))        schoolCounters[V_HOLY_ALL]->increase(code);
    if(cardSchool == SHADOW)
    {
        schoolCounters[V_SHADOW]->increase(code);
        schoolCounters[V_SHADOW_ALL]->increase(code);
    }
    else if(isShadowGen(code))      schoolCounters[V_SHADOW_ALL]->increase(code);
    if(cardSchool == NATURE)
    {
        schoolCounters[V_NATURE]->increase(code);
        schoolCounters[V_NATURE_ALL]->increase(code);
    }
    else if(isNatureGen(code))      schoolCounters[V_NATURE_ALL]->increase(code);

    if(isArcaneSyn(code))                           schoolCounters[V_ARCANE]->increaseSyn(code);
    else if(isArcaneAllSyn(code, text))             schoolCounters[V_ARCANE_ALL]->increaseSyn(code);
    if(isFelSyn(code))                              schoolCounters[V_FEL]->increaseSyn(code);
    else if(isFelAllSyn(code, text))                schoolCounters[V_FEL_ALL]->increaseSyn(code);
    if(isFireSyn(code))                             schoolCounters[V_FIRE]->increaseSyn(code);
    else if(isFireAllSyn(code, text))               schoolCounters[V_FIRE_ALL]->increaseSyn(code);
    if(isFrostSyn(code))                            schoolCounters[V_FROST]->increaseSyn(code);
    else if(isFrostAllSyn(code, text))              schoolCounters[V_FROST_ALL]->increaseSyn(code);
    if(isHolySyn(code))                             schoolCounters[V_HOLY]->increaseSyn(code);
    else if(isHolyAllSyn(code, text))               schoolCounters[V_HOLY_ALL]->increaseSyn(code);
    if(isShadowSyn(code))                           schoolCounters[V_SHADOW]->increaseSyn(code);
    else if(isShadowAllSyn(code, text))             schoolCounters[V_SHADOW_ALL]->increaseSyn(code);
    if(isNatureSyn(code))                           schoolCounters[V_NATURE]->increaseSyn(code);
    else if(isNatureAllSyn(code, text))             schoolCounters[V_NATURE_ALL]->increaseSyn(code);
}


void SynergyHandler::updateCardTypeCounters(DeckCard &deckCard, QMap<QString, QString> &spellMap, QMap<QString,
                                            QString> &minionMap, QMap<QString, QString> &weaponMap)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)
    {
        cardTypeCounters[V_SPELL]->increase(code);
        cardTypeCounters[V_SPELL_ALL]->increase(code);
        spellMap.insertMulti(code, "");
    }
    else if(isSpellGen(code))
    {
        cardTypeCounters[V_SPELL_ALL]->increase(code);
        cardTypeCounters[V_SPELL]->increaseExtra(code);
        spellMap.insertMulti(code, ".");
    }
    if(cardType == MINION || cardType == HERO || cardType == LOCATION)
    {
        cardTypeCounters[V_MINION]->increase(code);
        minionMap.insertMulti(code, "");
    }
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->increase(code);
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
        weaponMap.insertMulti(code, "");
    }
    else if(isWeaponGen(code, text))
    {
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
        cardTypeCounters[V_WEAPON]->increaseExtra(code);
        weaponMap.insertMulti(code, ".");
    }


    if(isSpellSyn(code))                cardTypeCounters[V_SPELL]->increaseSyn(code);
    else if(isSpellAllSyn(code, text))  cardTypeCounters[V_SPELL_ALL]->increaseSyn(code);
    if(isWeaponSyn(code))               cardTypeCounters[V_WEAPON]->increaseSyn(code);
    else if(isWeaponAllSyn(code, text)) cardTypeCounters[V_WEAPON_ALL]->increaseSyn(code);
}


void SynergyHandler::updateDropCounters(DeckCard &deckCard, QMap<QString, QString> &drop2Map, QMap<QString,
                                        QString> &drop3Map, QMap<QString, QString> &drop4Map)
{
    QString code = deckCard.getCode();
    int cost = deckCard.getCost();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();

    if(isDrop2(code, cost, attack, health))
    {
        dropCounters[V_DROP2]->increase(code);
        drop2Map.insertMulti(code, "");
        if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code, "2 Drop");
            drop3Map.insertMulti(code, "2 Drop");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code, "2 Drop");
            drop4Map.insertMulti(code, "2 Drop");
        }
    }
    else if(isDrop3(code, cost, attack, health))
    {
        dropCounters[V_DROP3]->increase(code);
        drop3Map.insertMulti(code, "");
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code, "3 Drop");
            drop2Map.insertMulti(code, "3 Drop");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code, "3 Drop");
            drop4Map.insertMulti(code, "3 Drop");
        }
    }
    else if(isDrop4(code, cost, attack, health))
    {
        dropCounters[V_DROP4]->increase(code);
        drop4Map.insertMulti(code, "");
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code, "4 Drop");
            drop2Map.insertMulti(code, "4 Drop");
        }
        else if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code, "4 Drop");
            drop3Map.insertMulti(code, "4 Drop");
        }
    }
    else
    {
        if(cost == 2)
        {
            dropCounters[V_DROP2]->increaseExtra(code);
            drop2Map.insertMulti(code, ".");
        }
        else if(cost == 3)
        {
            dropCounters[V_DROP3]->increaseExtra(code);
            drop3Map.insertMulti(code, ".");
        }
        else if(cost == 4)
        {
            dropCounters[V_DROP4]->increaseExtra(code);
            drop4Map.insertMulti(code, ".");
        }
    }

    //Hay una carta mas en el mazo
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->increaseNumCards();
    }
}


void SynergyHandler::updateMechanicCounters(DeckCard &deckCard,
                                            QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
                                            QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
                                            QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
                                            QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
                                            int &draw, int &toYourHand, int &discover)
{
    bool isSurvivability = false;
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();

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
    discover = numDiscoverGen(code, mechanics, referencedTags);
    draw = numDrawGen(code, text);
    toYourHand = numToYourHandGen(code, cost, mechanics, text);
    if(discover > 0)                                                        mechanicCounters[V_DISCOVER]->increase(code);
    if(draw > 0)                                                            mechanicCounters[V_DRAW]->increase(code);
    if(toYourHand > 0)                                                      mechanicCounters[V_TOYOURHAND]->increase(code);
    if(isOverload(code))                                                    mechanicCounters[V_OVERLOAD]->increase(code);
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isHeroPowerGen(code, text))                                          mechanicCounters[V_HERO_POWER]->increase(code);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mechanicCounters[V_FREEZE_ENEMY]->increase(code);
    if(isDiscardGen(code, text))                                            mechanicCounters[V_DISCARD]->increase(code);
    if(isDeathrattleMinion(code, mechanics, cardType))                      mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increase(code);
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mechanicCounters[V_SILENCE]->increase(code);
    if(isTauntGiverGen(code))                                               mechanicCounters[V_TAUNT_GIVER]->increase(code);
    if(isTokenGen(code, mechanics, text))                                   mechanicCounters[V_TOKEN]->increase(code);
    if(isTokenCardGen(code, cost, mechanics, referencedTags, text))         mechanicCounters[V_TOKEN_CARD]->increase(code);
    if(isWindfuryMinion(code, mechanics, cardType))                         mechanicCounters[V_WINDFURY_MINION]->increase(code);
    if(isAttackBuffGen(code, text))                                         mechanicCounters[V_ATTACK_BUFF]->increase(code);
    if(isAttackNerfGen(code, text))                                         mechanicCounters[V_ATTACK_NERF]->increase(code);
    if(isHealthBuffGen(code, text))                                         mechanicCounters[V_HEALTH_BUFF]->increase(code);
    if(isReturnGen(code, text))                                             mechanicCounters[V_RETURN]->increase(code);
    if(isStealthGen(code, mechanics))                                       mechanicCounters[V_STEALTH]->increase(code);
    if(isSpellDamageGen(code))                                              mechanicCounters[V_SPELL_DAMAGE]->increase(code);
    if(isEvolveGen(code, text))                                             mechanicCounters[V_EVOLVE]->increase(code);
    if(isSpawnEnemyGen(code, text))                                         mechanicCounters[V_SPAWN_ENEMY]->increase(code);
    if(isRestoreTargetMinionGen(code, text))                                mechanicCounters[V_RESTORE_TARGET_MINION]->increase(code);
    if(isRestoreFriendlyMinionGen(code, text))                              mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increase(code);
    if(isEnrageGen(code, mechanics))                                        mechanicCounters[V_ENRAGED]->increase(code);
    if(isEggGen(code, mechanics, attack, cardType))                         mechanicCounters[V_EGG]->increase(code);
    if(isDamageFriendlyHeroGen(code))                                       mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->increase(code);
    if(isHandBuffGen(code, text))                                           mechanicCounters[V_HAND_BUFF]->increase(code);
    if(isEnemyDrawGen(code, text))                                          mechanicCounters[V_ENEMY_DRAW]->increase(code);
    if(isHeroAttackGen(code, text))                                         mechanicCounters[V_HERO_ATTACK]->increase(code);
    if(isSpellBuffGen(code, text, mechanics, cardType))                     mechanicCounters[V_SPELL_BUFF]->increase(code);
    if(isOtherClassGen(code, text))                                         mechanicCounters[V_OTHER_CLASS]->increase(code);
    if(isSilverHandGen(code, text))                                         mechanicCounters[V_SILVER_HAND]->increase(code);
    if(isTreantGen(code, text))                                             mechanicCounters[V_TREANT]->increase(code);
    if(isLackeyGen(code, text))                                             mechanicCounters[V_LACKEY]->increase(code);
    if(isEndTurnGen(code, text))                                            mechanicCounters[V_END_TURN]->increase(code);
    if(isRushGiverGen(code, text))                                          mechanicCounters[V_RUSH_GIVER]->increase(code);
    if(isDredge(code, mechanics))                                           mechanicCounters[V_DREDGE]->increase(code);
    if(isCorpseGen(code, mechanics, text))                                  mechanicCounters[V_CORPSE]->increase(code);
    //New Synergy Step 3
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
    if(isDivineShield(code, mechanics))
    {
        mechanicCounters[V_DIVINE_SHIELD]->increase(code);
        mechanicCounters[V_DIVINE_SHIELD_ALL]->increase(code);
    }
    else if(isDivineShieldGen(code, referencedTags))                        mechanicCounters[V_DIVINE_SHIELD_ALL]->increase(code);
    if(isSecret(code, mechanics))
    {
        mechanicCounters[V_SECRET]->increase(code);
        mechanicCounters[V_SECRET_ALL]->increase(code);
    }
    else if(isSecretGen(code))                                              mechanicCounters[V_SECRET_ALL]->increase(code);
    if(isEcho(code, mechanics))
    {
        mechanicCounters[V_ECHO]->increase(code);
        mechanicCounters[V_ECHO_ALL]->increase(code);
    }
    else if(isEchoGen(code, referencedTags))                                mechanicCounters[V_ECHO_ALL]->increase(code);
    if(isRush(code, mechanics))
    {
        mechanicCounters[V_RUSH]->increase(code);
        mechanicCounters[V_RUSH_ALL]->increase(code);
    }
    else if(isRushGen(code, referencedTags))                                mechanicCounters[V_RUSH_ALL]->increase(code);
    if(isMagnetic(code, mechanics))
    {
        mechanicCounters[V_MAGNETIC]->increase(code);
        mechanicCounters[V_MAGNETIC_ALL]->increase(code);
    }
    else if(isMagneticGen(code))                                            mechanicCounters[V_MAGNETIC_ALL]->increase(code);
    if(isLifestealMinon(code, mechanics, cardType))
    {
        mechanicCounters[V_LIFESTEAL_MINION]->increase(code);
        mechanicCounters[V_LIFESTEAL_ALL]->increase(code);
    }
    else if(isLifestealGen(code, referencedTags))                           mechanicCounters[V_LIFESTEAL_ALL]->increase(code);
    if(isBattlecry(code, mechanics))
    {
        mechanicCounters[V_BATTLECRY]->increase(code);
        mechanicCounters[V_BATTLECRY_ALL]->increase(code);
    }
    else if(isBattlecryGen(code))                                           mechanicCounters[V_BATTLECRY_ALL]->increase(code);
    if(isOutcast(code, mechanics))
    {
        mechanicCounters[V_OUTCAST]->increase(code);
        mechanicCounters[V_OUTCAST_ALL]->increase(code);
    }
    else if(isOutcastGen(code))                                             mechanicCounters[V_OUTCAST_ALL]->increase(code);
    if(isCombo(code, mechanics))
    {
        mechanicCounters[V_COMBO]->increase(code);
        mechanicCounters[V_COMBO_ALL]->increase(code);
    }
    else if(isComboGen(code))                                               mechanicCounters[V_COMBO_ALL]->increase(code);


    //SYN
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isDiscoverSyn(code))                                                 mechanicCounters[V_DISCOVER]->increaseSyn(code);
    if(isDrawSyn(code))                                                     mechanicCounters[V_DRAW]->increaseSyn(code);
    if(isToYourHandSyn(code))                                               mechanicCounters[V_TOYOURHAND]->increaseSyn(code);
    if(isOverloadSyn(code, text))                                           mechanicCounters[V_OVERLOAD]->increaseSyn(code);
    if(isFreezeEnemySyn(code, referencedTags, text))                        mechanicCounters[V_FREEZE_ENEMY]->increaseSyn(code);
    if(isDiscardSyn(code, text))                                            mechanicCounters[V_DISCARD]->increaseSyn(code);
    if(isSilenceOwnSyn(code, mechanics))                                    mechanicCounters[V_SILENCE]->increaseSyn(code);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_TAUNT_GIVER]->increaseSyn(code);
    if(isTokenSyn(code, mechanics, text))                                   mechanicCounters[V_TOKEN]->increaseSyn(code);
    if(isTokenCardSyn(code, text))                                          mechanicCounters[V_TOKEN_CARD]->increaseSyn(code);
    if(isWindfuryMinionSyn(code))                                           mechanicCounters[V_WINDFURY_MINION]->increaseSyn(code);
    if(isAttackBuffSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_ATTACK_BUFF]->increaseSyn(code);
    if(isAttackNerfSyn(code, text))                                         mechanicCounters[V_ATTACK_NERF]->increaseSyn(code);
    if(isHealthBuffSyn(code))                                               mechanicCounters[V_HEALTH_BUFF]->increaseSyn(code);
    if(isReturnSyn(code, mechanics, cardType, text))                        mechanicCounters[V_RETURN]->increaseSyn(code);
    if(isStealthSyn(code))                                                  mechanicCounters[V_STEALTH]->increaseSyn(code);
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   mechanicCounters[V_SPELL_DAMAGE]->increaseSyn(code);
    if(isEvolveSyn(code))                                                   mechanicCounters[V_EVOLVE]->increaseSyn(code);
    if(isSpawnEnemySyn(code))                                               mechanicCounters[V_SPAWN_ENEMY]->increaseSyn(code);
    if(isRestoreTargetMinionSyn(code))                                      mechanicCounters[V_RESTORE_TARGET_MINION]->increaseSyn(code);
    if(isRestoreFriendlyHeroSyn(code))                                      mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increaseSyn(code);
    if(isRestoreFriendlyMinionSyn(code))                                    mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increaseSyn(code);
    if(isArmorSyn(code))                                                    mechanicCounters[V_ARMOR]->increaseSyn(code);
    if(isEnrageSyn(code, text))                                             mechanicCounters[V_ENRAGED]->increaseSyn(code);
    if(isEggSyn(code, text))                                                mechanicCounters[V_EGG]->increaseSyn(code);
    if(isDamageFriendlyHeroSyn(code))                                       mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->increaseSyn(code);
    if(isHandBuffSyn(code, text))                                           mechanicCounters[V_HAND_BUFF]->increaseSyn(code);
    if(isEnemyDrawSyn(code, text))                                          mechanicCounters[V_ENEMY_DRAW]->increaseSyn(code);
    if(isHeroAttackSyn(code))                                               mechanicCounters[V_HERO_ATTACK]->increaseSyn(code);
    if(isSpellBuffSyn(code, text))                                          mechanicCounters[V_SPELL_BUFF]->increaseSyn(code);
    if(isOtherClassSyn(code, text))                                         mechanicCounters[V_OTHER_CLASS]->increaseSyn(code);
    if(isSilverHandSyn(code))                                               mechanicCounters[V_SILVER_HAND]->increaseSyn(code);
    if(isTreantSyn(code))                                                   mechanicCounters[V_TREANT]->increaseSyn(code);
    if(isLackeySyn(code))                                                   mechanicCounters[V_LACKEY]->increaseSyn(code);
    if(isEndTurnSyn(code, text))                                            mechanicCounters[V_END_TURN]->increaseSyn(code);
    if(isRushGiverSyn(code, mechanics, text))                               mechanicCounters[V_RUSH_GIVER]->increaseSyn(code);
    if(isDredgeSyn(code, text))                                             mechanicCounters[V_DREDGE]->increaseSyn(code);
    if(isCorpseSyn(code, text))                                             mechanicCounters[V_CORPSE]->increaseSyn(code);
    //New Synergy Step 4
    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    else if(isTauntAllSyn(code))                                            mechanicCounters[V_TAUNT_ALL]->increaseSyn(code);
    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    else if(isDeathrattleGoodAllSyn(code, text))                            mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increaseSyn(code);
    if(isDivineShieldSyn(code))                                             mechanicCounters[V_DIVINE_SHIELD]->increaseSyn(code);
    else if(isDivineShieldAllSyn(code))                                     mechanicCounters[V_DIVINE_SHIELD_ALL]->increaseSyn(code);
    if(isSecretSyn(code))                                                   mechanicCounters[V_SECRET]->increaseSyn(code);
    else if(isSecretAllSyn(code, referencedTags))                           mechanicCounters[V_SECRET_ALL]->increaseSyn(code);
    if(isEchoSyn(code))                                                     mechanicCounters[V_ECHO]->increaseSyn(code);
    else if(isEchoAllSyn(code))                                             mechanicCounters[V_ECHO_ALL]->increaseSyn(code);
    if(isRushSyn(code))                                                     mechanicCounters[V_RUSH]->increaseSyn(code);
    else if(isRushAllSyn(code))                                             mechanicCounters[V_RUSH_ALL]->increaseSyn(code);
    if(isMagneticSyn(code))                                                 mechanicCounters[V_MAGNETIC]->increaseSyn(code);
    else if(isMagneticAllSyn(code))                                         mechanicCounters[V_MAGNETIC_ALL]->increaseSyn(code);
    if(isLifestealMinionSyn(code))                                          mechanicCounters[V_LIFESTEAL_MINION]->increaseSyn(code);
    else if(isLifestealAllSyn(code))                                        mechanicCounters[V_LIFESTEAL_ALL]->increaseSyn(code);
    if(isBattlecrySyn(code))                                                mechanicCounters[V_BATTLECRY]->increaseSyn(code);
    else if(isBattlecryAllSyn(code, referencedTags))                        mechanicCounters[V_BATTLECRY_ALL]->increaseSyn(code);
    if(isOutcastSyn(code))                                                  mechanicCounters[V_OUTCAST]->increaseSyn(code);
    else if(isOutcastAllSyn(code, referencedTags))                          mechanicCounters[V_OUTCAST_ALL]->increaseSyn(code);
    if(isComboSyn(code))                                                    mechanicCounters[V_COMBO]->increaseSyn(code);
    else if(isComboAllSyn(code, referencedTags))                            mechanicCounters[V_COMBO_ALL]->increaseSyn(code);
}


void SynergyHandler::updateStatsCards(DeckCard &deckCard)
{
    QString code = deckCard.getCode();

    if(deckCard.getType() == MINION)
    {
        //Stats
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();

        costMinions.appendStatValue(false, deckCard.getCost(), code);
        attackMinions.appendStatValue(false, attack, code);
        healthMinions.appendStatValue(false, health, code);
    }
    else if(deckCard.getType() == SPELL)
    {
        costSpells.appendStatValue(false, deckCard.getCost(), code);
    }
    else if(deckCard.getType() == WEAPON)
    {
        //Stats
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();

        costWeapons.appendStatValue(false, deckCard.getCost(), code);
        attackWeapons.appendStatValue(false, attack, code);
        healthWeapons.appendStatValue(false, health, code);
    }

    //Synergies
    const QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.cardType)
        {
            case S_MINION:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costMinions.appendStatValue(false, statSyn.statValue, code);
                        else                costMinions.updateStatsMapSyn(statSyn, code);
                    break;
                    case S_ATTACK:
                        if(statSyn.isGen)   attackMinions.appendStatValue(false, statSyn.statValue, code);
                        else                attackMinions.updateStatsMapSyn(statSyn, code);
                    break;
                    case S_HEALTH:
                        if(statSyn.isGen)   healthMinions.appendStatValue(false, statSyn.statValue, code);
                        else                healthMinions.updateStatsMapSyn(statSyn, code);
                    break;
                }
            break;
            case S_SPELL:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costSpells.appendStatValue(false, statSyn.statValue, code);
                        else                costSpells.updateStatsMapSyn(statSyn, code);
                    break;
                    case S_ATTACK:
                    case S_HEALTH:
                    break;
                }
            break;
            case S_WEAPON:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costWeapons.appendStatValue(false, statSyn.statValue, code);
                        else                costWeapons.updateStatsMapSyn(statSyn, code);
                    break;
                    case S_ATTACK:
                        if(statSyn.isGen)   attackWeapons.appendStatValue(false, statSyn.statValue, code);
                        else                attackWeapons.updateStatsMapSyn(statSyn, code);
                    break;
                    case S_HEALTH:
                        if(statSyn.isGen)   healthWeapons.appendStatValue(false, statSyn.statValue, code);
                        else                healthWeapons.updateStatsMapSyn(statSyn, code);
                    break;
                }
            break;
        }
    }

//    qDebug()<<"*****COST MAP*****";
//    costMinions.qDebugContents();
//    qDebug()<<"*****ATTACK MAP*****";
//    attackMinions.qDebugContents();
//    qDebug()<<"*****HEALTH MAP*****";
//    healthMinions.qDebugContents();
}


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                  QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor)
{
    getCardTypeSynergies(deckCard, synergyTagMap);
    getDropMechanicIcons(deckCard, mechanicIcons, dropBorderColor);
    getRaceSynergies(deckCard, synergyTagMap);
    getSchoolSynergies(deckCard, synergyTagMap);
    getMechanicSynergies(deckCard, synergyTagMap, mechanicIcons);
    getDirectLinkSynergies(deckCard, synergyTagMap["Extra"]);
    getStatsCardsSynergies(deckCard, synergyTagMap);
}


void SynergyHandler::getCardTypeSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    //Evita mostrar spellSyn cards en cada hechizo que veamos, es sinergia debil
    if(cardType == SPELL)
    {
        cardTypeCounters[V_SPELL]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isSpellGen(code))                   cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isWeaponGen(code, text))            cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);


    if(isSpellSyn(code))                        cardTypeCounters[V_SPELL]->insertCards(synergyTagMap);
    else if(isSpellAllSyn(code, text))          cardTypeCounters[V_SPELL_ALL]->insertCards(synergyTagMap);
    if(isWeaponSyn(code))                       cardTypeCounters[V_WEAPON]->insertCards(synergyTagMap);
    else if(isWeaponAllSyn(code, text))         cardTypeCounters[V_WEAPON_ALL]->insertCards(synergyTagMap);
}


void SynergyHandler::getDropMechanicIcons(DeckCard &deckCard, QMap<MechanicIcons, int> &mechanicIcons,
                                          MechanicBorderColor &dropBorderColor)
{
    QString code = deckCard.getCode();
    int cost = deckCard.getCost();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();

    if(isDrop2(code, cost, attack, health))
    {
        mechanicIcons[M_DROP2] = dropCounters[V_DROP2]->count() + 1;
        dropBorderColor = dropCounters[V_DROP2]->getMechanicBorderColor();
    }
    else if(isDrop3(code, cost, attack, health))
    {
        mechanicIcons[M_DROP3] = dropCounters[V_DROP3]->count() + 1;
        dropBorderColor = dropCounters[V_DROP3]->getMechanicBorderColor();
    }
    else if(isDrop4(code, cost, attack, health))
    {
        mechanicIcons[M_DROP4] = dropCounters[V_DROP4]->count() + 1;
        dropBorderColor = dropCounters[V_DROP4]->getMechanicBorderColor();
    }
    else
    {
        dropBorderColor = MechanicBorderGrey;
    }
}


void SynergyHandler::getRaceSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    QList<CardRace> cardRace = deckCard.getRace();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();

    if(cardRace.contains(MURLOC) || cardRace.contains(ALL))
    {
        raceCounters[V_MURLOC]->insertSynCards(synergyTagMap);
        raceCounters[V_MURLOC_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DEMON) || cardRace.contains(ALL))
    {
        raceCounters[V_DEMON]->insertSynCards(synergyTagMap);
        raceCounters[V_DEMON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(MECHANICAL) || cardRace.contains(ALL))
    {
        raceCounters[V_MECHANICAL]->insertSynCards(synergyTagMap);
        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(ELEMENTAL) || cardRace.contains(ALL))
    {
        raceCounters[V_ELEMENTAL]->insertSynCards(synergyTagMap);
        raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(BEAST) || cardRace.contains(ALL))
    {
        raceCounters[V_BEAST]->insertSynCards(synergyTagMap);
        raceCounters[V_BEAST_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(TOTEM) || cardRace.contains(ALL))
    {
        raceCounters[V_TOTEM]->insertSynCards(synergyTagMap);
        raceCounters[V_TOTEM_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(PIRATE) || cardRace.contains(ALL))
    {
        raceCounters[V_PIRATE]->insertSynCards(synergyTagMap);
        raceCounters[V_PIRATE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DRAGON) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAGON]->insertSynCards(synergyTagMap);
        raceCounters[V_DRAGON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(NAGA) || cardRace.contains(ALL))
    {
        raceCounters[V_NAGA]->insertSynCards(synergyTagMap);
        raceCounters[V_NAGA_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isNagaGen(code))        raceCounters[V_NAGA_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(UNDEAD) || cardRace.contains(ALL))
    {
        raceCounters[V_UNDEAD]->insertSynCards(synergyTagMap);
        raceCounters[V_UNDEAD_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isUndeadGen(code))      raceCounters[V_UNDEAD_ALL]->insertSynCards(synergyTagMap);

    if(isMurlocSyn(code))                           raceCounters[V_MURLOC]->insertCards(synergyTagMap);
    else if(isMurlocAllSyn(code, text))             raceCounters[V_MURLOC_ALL]->insertCards(synergyTagMap);
    if(isDemonSyn(code))                            raceCounters[V_DEMON]->insertCards(synergyTagMap);
    else if(isDemonAllSyn(code, text))              raceCounters[V_DEMON_ALL]->insertCards(synergyTagMap);
    if(isMechSyn(code))                             raceCounters[V_MECHANICAL]->insertCards(synergyTagMap);
    else if(isMechAllSyn(code, mechanics, text))    raceCounters[V_MECHANICAL_ALL]->insertCards(synergyTagMap);
    if(isElementalSyn(code))                        raceCounters[V_ELEMENTAL]->insertCards(synergyTagMap);
    else if(isElementalAllSyn(code, text))          raceCounters[V_ELEMENTAL_ALL]->insertCards(synergyTagMap);
    if(isBeastSyn(code))                            raceCounters[V_BEAST]->insertCards(synergyTagMap);
    else if(isBeastAllSyn(code, text))              raceCounters[V_BEAST_ALL]->insertCards(synergyTagMap);
    if(isTotemSyn(code))                            raceCounters[V_TOTEM]->insertCards(synergyTagMap);
    else if(isTotemAllSyn(code, text))              raceCounters[V_TOTEM_ALL]->insertCards(synergyTagMap);
    if(isPirateSyn(code))                           raceCounters[V_PIRATE]->insertCards(synergyTagMap);
    else if(isPirateAllSyn(code, text))             raceCounters[V_PIRATE_ALL]->insertCards(synergyTagMap);
    if(isDragonSyn(code))                           raceCounters[V_DRAGON]->insertCards(synergyTagMap);
    else if(isDragonAllSyn(code, text))             raceCounters[V_DRAGON_ALL]->insertCards(synergyTagMap);
    if(isNagaSyn(code))                             raceCounters[V_NAGA]->insertCards(synergyTagMap);
    else if(isNagaAllSyn(code, text))               raceCounters[V_NAGA_ALL]->insertCards(synergyTagMap);
    if(isUndeadSyn(code))                           raceCounters[V_UNDEAD]->insertCards(synergyTagMap);
    else if(isUndeadAllSyn(code, text))             raceCounters[V_UNDEAD_ALL]->insertCards(synergyTagMap);
}


void SynergyHandler::getSchoolSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardSchool cardSchool = deckCard.getSchool();

    if(cardSchool == ARCANE)
    {
        schoolCounters[V_ARCANE]->insertSynCards(synergyTagMap);
        schoolCounters[V_ARCANE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isArcaneGen(code))      schoolCounters[V_ARCANE_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FEL)
    {
        schoolCounters[V_FEL]->insertSynCards(synergyTagMap);
        schoolCounters[V_FEL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFelGen(code))       schoolCounters[V_FEL_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FIRE)
    {
        schoolCounters[V_FIRE]->insertSynCards(synergyTagMap);
        schoolCounters[V_FIRE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFireGen(code))        schoolCounters[V_FIRE_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == FROST)
    {
        schoolCounters[V_FROST]->insertSynCards(synergyTagMap);
        schoolCounters[V_FROST_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isFrostGen(code))   schoolCounters[V_FROST_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == HOLY)
    {
        schoolCounters[V_HOLY]->insertSynCards(synergyTagMap);
        schoolCounters[V_HOLY_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isHolyGen(code))       schoolCounters[V_HOLY_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == SHADOW)
    {
        schoolCounters[V_SHADOW]->insertSynCards(synergyTagMap);
        schoolCounters[V_SHADOW_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isShadowGen(code))       schoolCounters[V_SHADOW_ALL]->insertSynCards(synergyTagMap);
    if(cardSchool == NATURE)
    {
        schoolCounters[V_NATURE]->insertSynCards(synergyTagMap);
        schoolCounters[V_NATURE_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isNatureGen(code))      schoolCounters[V_NATURE_ALL]->insertSynCards(synergyTagMap);

    if(isArcaneSyn(code))                           schoolCounters[V_ARCANE]->insertCards(synergyTagMap);
    else if(isArcaneAllSyn(code, text))             schoolCounters[V_ARCANE_ALL]->insertCards(synergyTagMap);
    if(isFelSyn(code))                              schoolCounters[V_FEL]->insertCards(synergyTagMap);
    else if(isFelAllSyn(code, text))                schoolCounters[V_FEL_ALL]->insertCards(synergyTagMap);
    if(isFireSyn(code))                             schoolCounters[V_FIRE]->insertCards(synergyTagMap);
    else if(isFireAllSyn(code, text))               schoolCounters[V_FIRE_ALL]->insertCards(synergyTagMap);
    if(isFrostSyn(code))                            schoolCounters[V_FROST]->insertCards(synergyTagMap);
    else if(isFrostAllSyn(code, text))              schoolCounters[V_FROST_ALL]->insertCards(synergyTagMap);
    if(isHolySyn(code))                             schoolCounters[V_HOLY]->insertCards(synergyTagMap);
    else if(isHolyAllSyn(code, text))               schoolCounters[V_HOLY_ALL]->insertCards(synergyTagMap);
    if(isShadowSyn(code))                           schoolCounters[V_SHADOW]->insertCards(synergyTagMap);
    else if(isShadowAllSyn(code, text))             schoolCounters[V_SHADOW_ALL]->insertCards(synergyTagMap);
    if(isNatureSyn(code))                           schoolCounters[V_NATURE]->insertCards(synergyTagMap);
    else if(isNatureAllSyn(code, text))             schoolCounters[V_NATURE_ALL]->insertCards(synergyTagMap);
}


void SynergyHandler::getMechanicSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                          QMap<MechanicIcons, int> &mechanicIcons)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();
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
    if(isRestoreTargetMinionGen(code, text))                    mechanicCounters[V_RESTORE_TARGET_MINION]->insertSynCards(synergyTagMap);
    if(isRestoreFriendlyMinionGen(code, text))                  mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertSynCards(synergyTagMap);
    //Sinergias gen-gen
    if(isJadeGolemGen(code, mechanics, referencedTags))         mechanicCounters[V_JADE_GOLEM]->insertCards(synergyTagMap);
    //Sinergias gen-gen -- Evitamos sinergias con la misma carta
    if(isHeroPowerGen(code, text))                              mechanicCounters[V_HERO_POWER]->insertCards(synergyTagMap, code);
    if(isDiscoverGen(code, mechanics, referencedTags))          mechanicCounters[V_DISCOVER]->insertSynCards(synergyTagMap);
    if(isDrawGen(code, text))                                   mechanicCounters[V_DRAW]->insertSynCards(synergyTagMap);
    if(isToYourHandGen(code, cost, mechanics, text))            mechanicCounters[V_TOYOURHAND]->insertSynCards(synergyTagMap);
    if(isOverload(code))                                        mechanicCounters[V_OVERLOAD]->insertSynCards(synergyTagMap);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text)) mechanicCounters[V_FREEZE_ENEMY]->insertSynCards(synergyTagMap);
    if(isDiscardGen(code, text))                                mechanicCounters[V_DISCARD]->insertSynCards(synergyTagMap);
    if(isDeathrattleMinion(code, mechanics, cardType))          mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergyTagMap);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType)) mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertSynCards(synergyTagMap);
    if(isSilenceOwnGen(code, mechanics, referencedTags))        mechanicCounters[V_SILENCE]->insertSynCards(synergyTagMap);
    if(isTauntGiverGen(code))                                   mechanicCounters[V_TAUNT_GIVER]->insertSynCards(synergyTagMap);
    if(isTokenGen(code, mechanics, text))                       mechanicCounters[V_TOKEN]->insertSynCards(synergyTagMap);
    if(isTokenCardGen(code, cost, mechanics, referencedTags, text)) mechanicCounters[V_TOKEN_CARD]->insertSynCards(synergyTagMap);
    if(isWindfuryMinion(code, mechanics, cardType))             mechanicCounters[V_WINDFURY_MINION]->insertSynCards(synergyTagMap);
    if(isAttackBuffGen(code, text))                             mechanicCounters[V_ATTACK_BUFF]->insertSynCards(synergyTagMap);
    if(isAttackNerfGen(code, text))                             mechanicCounters[V_ATTACK_NERF]->insertSynCards(synergyTagMap);
    if(isHealthBuffGen(code, text))                             mechanicCounters[V_HEALTH_BUFF]->insertSynCards(synergyTagMap);
    if(isReturnGen(code, text))                                 mechanicCounters[V_RETURN]->insertSynCards(synergyTagMap);
    if(isStealthGen(code, mechanics))                           mechanicCounters[V_STEALTH]->insertSynCards(synergyTagMap);
    if(isSpellDamageGen(code))                                  mechanicCounters[V_SPELL_DAMAGE]->insertSynCards(synergyTagMap);
    if(isEvolveGen(code, text))                                 mechanicCounters[V_EVOLVE]->insertSynCards(synergyTagMap);
    if(isSpawnEnemyGen(code, text))                             mechanicCounters[V_SPAWN_ENEMY]->insertSynCards(synergyTagMap);
    if(isEnrageGen(code, mechanics))                            mechanicCounters[V_ENRAGED]->insertSynCards(synergyTagMap);
    if(isEggGen(code, mechanics, attack, cardType))             mechanicCounters[V_EGG]->insertSynCards(synergyTagMap);
    if(isDamageFriendlyHeroGen(code))                           mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->insertSynCards(synergyTagMap);
    if(isHandBuffGen(code, text))                               mechanicCounters[V_HAND_BUFF]->insertSynCards(synergyTagMap);
    if(isEnemyDrawGen(code, text))                              mechanicCounters[V_ENEMY_DRAW]->insertSynCards(synergyTagMap);
    if(isHeroAttackGen(code, text))                             mechanicCounters[V_HERO_ATTACK]->insertSynCards(synergyTagMap);
    if(isSpellBuffGen(code, text, mechanics, cardType))         mechanicCounters[V_SPELL_BUFF]->insertSynCards(synergyTagMap);
    if(isOtherClassGen(code, text))                             mechanicCounters[V_OTHER_CLASS]->insertSynCards(synergyTagMap);
    if(isSilverHandGen(code, text))                             mechanicCounters[V_SILVER_HAND]->insertSynCards(synergyTagMap);
    if(isTreantGen(code, text))                                 mechanicCounters[V_TREANT]->insertSynCards(synergyTagMap);
    if(isLackeyGen(code, text))                                 mechanicCounters[V_LACKEY]->insertSynCards(synergyTagMap);
    if(isEndTurnGen(code, text))                                mechanicCounters[V_END_TURN]->insertSynCards(synergyTagMap);
    if(isRushGiverGen(code, text))                              mechanicCounters[V_RUSH_GIVER]->insertSynCards(synergyTagMap);
    if(isDredge(code, mechanics))                               mechanicCounters[V_DREDGE]->insertSynCards(synergyTagMap);
    if(isCorpseGen(code, mechanics, text))                      mechanicCounters[V_CORPSE]->insertSynCards(synergyTagMap);
    //New Synergy Step 5
    if(isDivineShield(code, mechanics))
    {
        mechanicCounters[V_DIVINE_SHIELD]->insertSynCards(synergyTagMap);
        mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDivineShieldGen(code, referencedTags))            mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergyTagMap);

    if(isSecret(code, mechanics))
    {
        mechanicCounters[V_SECRET]->insertSynCards(synergyTagMap);
        mechanicCounters[V_SECRET_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isSecretGen(code))                                  mechanicCounters[V_SECRET_ALL]->insertSynCards(synergyTagMap);
    if(isEcho(code, mechanics))
    {
        mechanicCounters[V_ECHO]->insertSynCards(synergyTagMap);
        mechanicCounters[V_ECHO_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isEchoGen(code, referencedTags))                    mechanicCounters[V_ECHO_ALL]->insertSynCards(synergyTagMap);
    if(isRush(code, mechanics))
    {
        mechanicCounters[V_RUSH]->insertSynCards(synergyTagMap);
        mechanicCounters[V_RUSH_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isRushGen(code, referencedTags))                    mechanicCounters[V_RUSH_ALL]->insertSynCards(synergyTagMap);
    if(isMagnetic(code, mechanics))
    {
        mechanicCounters[V_MAGNETIC]->insertSynCards(synergyTagMap);
        mechanicCounters[V_MAGNETIC_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isMagneticGen(code))                                mechanicCounters[V_MAGNETIC_ALL]->insertSynCards(synergyTagMap);
    if(isLifestealMinon(code, mechanics, cardType))
    {
        mechanicCounters[V_LIFESTEAL_MINION]->insertSynCards(synergyTagMap);
        mechanicCounters[V_LIFESTEAL_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isLifestealGen(code, referencedTags))               mechanicCounters[V_LIFESTEAL_ALL]->insertSynCards(synergyTagMap);
    if(isBattlecry(code, mechanics))
    {
        mechanicCounters[V_BATTLECRY]->insertSynCards(synergyTagMap);
        mechanicCounters[V_BATTLECRY_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isBattlecryGen(code))                               mechanicCounters[V_BATTLECRY_ALL]->insertSynCards(synergyTagMap);
    if(isOutcast(code, mechanics))
    {
        mechanicCounters[V_OUTCAST]->insertSynCards(synergyTagMap);
        mechanicCounters[V_OUTCAST_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isOutcastGen(code))                                 mechanicCounters[V_OUTCAST_ALL]->insertSynCards(synergyTagMap);
    if(isCombo(code, mechanics))
    {
        mechanicCounters[V_COMBO]->insertSynCards(synergyTagMap);
        mechanicCounters[V_COMBO_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isComboGen(code))                                   mechanicCounters[V_COMBO_ALL]->insertSynCards(synergyTagMap);


    //SYN
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergyTagMap);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergyTagMap);
    if(isDiscoverSyn(code))                                     mechanicCounters[V_DISCOVER]->insertCards(synergyTagMap);
    if(isDrawSyn(code))                                         mechanicCounters[V_DRAW]->insertCards(synergyTagMap);
    if(isToYourHandSyn(code))                                   mechanicCounters[V_TOYOURHAND]->insertCards(synergyTagMap);
    if(isOverloadSyn(code, text))                               mechanicCounters[V_OVERLOAD]->insertCards(synergyTagMap);
    if(isFreezeEnemySyn(code, referencedTags, text))            mechanicCounters[V_FREEZE_ENEMY]->insertCards(synergyTagMap);
    if(isDiscardSyn(code, text))                                mechanicCounters[V_DISCARD]->insertCards(synergyTagMap);
    if(isSilenceOwnSyn(code, mechanics))                        mechanicCounters[V_SILENCE]->insertCards(synergyTagMap);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))      mechanicCounters[V_TAUNT_GIVER]->insertCards(synergyTagMap);
    if(isTokenSyn(code, mechanics, text))                       mechanicCounters[V_TOKEN]->insertCards(synergyTagMap);
    if(isTokenCardSyn(code, text))                              mechanicCounters[V_TOKEN_CARD]->insertCards(synergyTagMap);
    if(isWindfuryMinionSyn(code))                               mechanicCounters[V_WINDFURY_MINION]->insertCards(synergyTagMap);
    if(isAttackBuffSyn(code, mechanics, attack, cardType))      mechanicCounters[V_ATTACK_BUFF]->insertCards(synergyTagMap);
    if(isAttackNerfSyn(code, text))                             mechanicCounters[V_ATTACK_NERF]->insertCards(synergyTagMap);
    if(isHealthBuffSyn(code))                                   mechanicCounters[V_HEALTH_BUFF]->insertCards(synergyTagMap);
    //returnSyn es sinergia debil
//    if(isReturnSyn(code, mechanics, cardType, text))            mechanicCounters[V_RETURN]->insertCards(synergies);
    if(isStealthSyn(code))                                      mechanicCounters[V_STEALTH]->insertCards(synergyTagMap);
    if(isSpellDamageSyn(code, mechanics, cardType, text))       mechanicCounters[V_SPELL_DAMAGE]->insertCards(synergyTagMap);
    if(isEvolveSyn(code))                                       mechanicCounters[V_EVOLVE]->insertCards(synergyTagMap);
    if(isSpawnEnemySyn(code))                                   mechanicCounters[V_SPAWN_ENEMY]->insertCards(synergyTagMap);
    if(isRestoreTargetMinionSyn(code))                          mechanicCounters[V_RESTORE_TARGET_MINION]->insertCards(synergyTagMap);
    if(isRestoreFriendlyHeroSyn(code))                          mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertCards(synergyTagMap);
    if(isRestoreFriendlyMinionSyn(code))                        mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertCards(synergyTagMap);
    if(isArmorSyn(code))                                        mechanicCounters[V_ARMOR]->insertCards(synergyTagMap);
    if(isEnrageSyn(code, text))                                 mechanicCounters[V_ENRAGED]->insertCards(synergyTagMap);
    if(isEggSyn(code, text))                                    mechanicCounters[V_EGG]->insertCards(synergyTagMap);
    if(isDamageFriendlyHeroSyn(code))                           mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->insertCards(synergyTagMap);
    if(isHandBuffSyn(code, text))                               mechanicCounters[V_HAND_BUFF]->insertCards(synergyTagMap);
    if(isEnemyDrawSyn(code, text))                              mechanicCounters[V_ENEMY_DRAW]->insertCards(synergyTagMap);
    if(isHeroAttackSyn(code))                                   mechanicCounters[V_HERO_ATTACK]->insertCards(synergyTagMap);
    if(isSpellBuffSyn(code, text))                              mechanicCounters[V_SPELL_BUFF]->insertCards(synergyTagMap);
    if(isOtherClassSyn(code, text))                             mechanicCounters[V_OTHER_CLASS]->insertCards(synergyTagMap);
    if(isSilverHandSyn(code))                                   mechanicCounters[V_SILVER_HAND]->insertCards(synergyTagMap);
    if(isTreantSyn(code))                                       mechanicCounters[V_TREANT]->insertCards(synergyTagMap);
    if(isLackeySyn(code))                                       mechanicCounters[V_LACKEY]->insertCards(synergyTagMap);
    if(isEndTurnSyn(code, text))                                mechanicCounters[V_END_TURN]->insertCards(synergyTagMap);
    if(isRushGiverSyn(code, mechanics, text))                   mechanicCounters[V_RUSH_GIVER]->insertCards(synergyTagMap);
    if(isDredgeSyn(code, text))                                 mechanicCounters[V_DREDGE]->insertCards(synergyTagMap);
    if(isCorpseSyn(code, text))                                 mechanicCounters[V_CORPSE]->insertCards(synergyTagMap);
    //New Synergy Step 6
    if(isTauntSyn(code))                                        mechanicCounters[V_TAUNT]->insertCards(synergyTagMap);
    else if(isTauntAllSyn(code))                                mechanicCounters[V_TAUNT_ALL]->insertCards(synergyTagMap);
    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergyTagMap);
    else if(isDeathrattleGoodAllSyn(code, text))                mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertCards(synergyTagMap);
    if(isDivineShieldSyn(code))                                 mechanicCounters[V_DIVINE_SHIELD]->insertCards(synergyTagMap);
    else if(isDivineShieldAllSyn(code))                         mechanicCounters[V_DIVINE_SHIELD_ALL]->insertCards(synergyTagMap);
    if(isSecretSyn(code))                                       mechanicCounters[V_SECRET]->insertCards(synergyTagMap);
    else if(isSecretAllSyn(code, referencedTags))               mechanicCounters[V_SECRET_ALL]->insertCards(synergyTagMap);
    if(isEchoSyn(code))                                         mechanicCounters[V_ECHO]->insertCards(synergyTagMap);
    else if(isEchoAllSyn(code))                                 mechanicCounters[V_ECHO_ALL]->insertCards(synergyTagMap);
    if(isRushSyn(code))                                         mechanicCounters[V_RUSH]->insertCards(synergyTagMap);
    else if(isRushAllSyn(code))                                 mechanicCounters[V_RUSH_ALL]->insertCards(synergyTagMap);
    if(isMagneticSyn(code))                                     mechanicCounters[V_MAGNETIC]->insertCards(synergyTagMap);
    else if(isMagneticAllSyn(code))                             mechanicCounters[V_MAGNETIC_ALL]->insertCards(synergyTagMap);
    if(isLifestealMinionSyn(code))                              mechanicCounters[V_LIFESTEAL_MINION]->insertCards(synergyTagMap);
    else if(isLifestealAllSyn(code))                            mechanicCounters[V_LIFESTEAL_ALL]->insertCards(synergyTagMap);
    if(isBattlecrySyn(code))                                    mechanicCounters[V_BATTLECRY]->insertCards(synergyTagMap);
    else if(isBattlecryAllSyn(code, referencedTags))            mechanicCounters[V_BATTLECRY_ALL]->insertCards(synergyTagMap);
    if(isOutcastSyn(code))                                      mechanicCounters[V_OUTCAST]->insertCards(synergyTagMap);
    else if(isOutcastAllSyn(code, referencedTags))              mechanicCounters[V_OUTCAST_ALL]->insertCards(synergyTagMap);
    if(isComboSyn(code))                                        mechanicCounters[V_COMBO]->insertCards(synergyTagMap);
    else if(isComboAllSyn(code, referencedTags))                mechanicCounters[V_COMBO_ALL]->insertCards(synergyTagMap);
}


void SynergyHandler::getDirectLinkSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();

    if(directLinks.contains(code))
    {
        const QList<QString> linkCodes = directLinks[code];

        for(const QString &linkCode: linkCodes)
        {
            if(cardTypeCounters[V_MINION]->insertCode(linkCode, synergies)){}
            else if(cardTypeCounters[V_WEAPON]->insertCode(linkCode, synergies)){}
            else cardTypeCounters[V_SPELL]->insertCode(linkCode, synergies);
        }
    }
}


void SynergyHandler::getStatsCardsSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QString code = deckCard.getCode();

    if(deckCard.getType() == MINION)
    {
        //Stats
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();

        costMinions.insertCards(true, deckCard.getCost(), synergyTagMap["Cost"]);
        attackMinions.insertCards(true, attack, synergyTagMap["Attack"]);
        healthMinions.insertCards(true, health, synergyTagMap["Health"]);
    }
    else if(deckCard.getType() == SPELL)
    {
        costSpells.insertCards(true, deckCard.getCost(), synergyTagMap["Cost"]);
    }
    else if(deckCard.getType() == WEAPON)
    {
        //Stats
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();

        costWeapons.insertCards(true, deckCard.getCost(), synergyTagMap["Cost"]);
        attackWeapons.insertCards(true, attack, synergyTagMap["Attack"]);
        healthWeapons.insertCards(true, health, synergyTagMap["Health"]);
    }

    //Synergies
    const QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.cardType)
        {
            case S_MINION:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costMinions.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                        else                costMinions.insertStatCards(statSyn, synergyTagMap["Cost"]);
                    break;
                    case S_ATTACK:
                        if(statSyn.isGen)   attackMinions.insertCards(true, statSyn.statValue, synergyTagMap["Attack"]);
                        else                attackMinions.insertStatCards(statSyn, synergyTagMap["Attack"]);
                    break;
                    case S_HEALTH:
                        if(statSyn.isGen)   healthMinions.insertCards(true, statSyn.statValue, synergyTagMap["Health"]);
                        else                healthMinions.insertStatCards(statSyn, synergyTagMap["Health"]);
                    break;
                }
            break;
            case S_SPELL:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costSpells.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                        else                costSpells.insertStatCards(statSyn, synergyTagMap["Cost"]);
                    break;
                    case S_ATTACK:
                    case S_HEALTH:
                    break;
                }
            break;
            case S_WEAPON:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Cost"]);
                        else                costWeapons.insertStatCards(statSyn, synergyTagMap["Cost"]);
                    break;
                    case S_ATTACK:
                        if(statSyn.isGen)   attackWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Attack"]);
                        else                attackWeapons.insertStatCards(statSyn, synergyTagMap["Attack"]);
                    break;
                    case S_HEALTH:
                        if(statSyn.isGen)   healthWeapons.insertCards(true, statSyn.statValue, synergyTagMap["Health"]);
                        else                healthWeapons.insertStatCards(statSyn, synergyTagMap["Health"]);
                    break;
                }
            break;
        }
    }
}


bool SynergyHandler::isValidSynergyCode(const QString &mechanic)
{
    if(mechanic.startsWith('='))    return true;
    QStringList validMecs = {
        "spellGen", "weaponGen",
        "spellSyn", "weaponSyn",
        "spellAllSyn", "weaponAllSyn",

        "murlocGen", "demonGen", "mechGen", "elementalGen", "beastGen", "totemGen", "pirateGen", "dragonGen", "nagaGen", "undeadGen",
        "murlocSyn", "demonSyn", "mechSyn", "elementalSyn", "beastSyn", "totemSyn", "pirateSyn", "dragonSyn", "nagaSyn", "undeadSyn",
        "murlocAllSyn", "demonAllSyn", "mechAllSyn", "elementalAllSyn", "beastAllSyn",
        "totemAllSyn", "pirateAllSyn", "dragonAllSyn", "nagaAllSyn", "undeadAllSyn",

        "arcaneGen", "felGen", "fireGen", "frostGen", "holyGen", "shadowGen", "natureGen",
        "arcaneSyn", "felSyn", "fireSyn", "frostSyn", "holySyn", "shadowSyn", "natureSyn",
        "arcaneAllSyn", "felAllSyn", "fireAllSyn", "frostAllSyn", "holyAllSyn", "shadowAllSyn", "natureAllSyn",

        "drop2", "drop3", "drop4",

        "discover", "drawGen", "toYourHandGen", "enemyDrawGen",
        "discoverSyn", "drawSyn", "toYourHandSyn", "enemyDrawSyn",

        "taunt", "tauntGen", "divineShield", "divineShieldGen", "windfury", "overload",
        "tauntSyn", "tauntAllSyn", "divineShieldSyn", "divineShieldAllSyn", "windfuryMinionSyn", "overloadSyn",

        "jadeGolemGen", "secret", "secretGen", "freezeEnemyGen", "discardGen", "stealthGen",
        "heroPowerGen", "secretSyn", "secretAllSyn", "freezeEnemySyn", "discardSyn", "stealthSyn",

        "damageMinionsGen", "reachGen", "pingGen", "aoeGen", "destroyGen",
        "damageMinionsSyn", "reachSyn", "pingSyn", "aoeSyn", "destroySyn",

        "deathrattle", "deathrattleGen", "deathrattleOpponent", "silenceOwnGen", "battlecry", "battlecryGen", "returnGen",
        "deathrattleSyn", "deathrattleGoodAllSyn", "silenceOwnSyn", "battlecrySyn", "battlecryAllSyn", "returnSyn",

        "enrageGen", "tauntGiverGen", "evolveGen", "spawnEnemyGen", "spellDamageGen", "handBuffGen", "spellBuffGen",
        "enrageSyn", "tauntGiverSyn", "evolveSyn", "spawnEnemySyn", "spellDamageSyn", "handBuffSyn", "spellBuffSyn",

        "tokenGen", "tokenCardGen", "combo", "comboGen", "attackBuffGen", "attackNerfGen", "healthBuffGen", "heroAttackGen",
        "tokenSyn", "tokenCardSyn", "comboSyn", "comboAllSyn", "attackBuffSyn", "attackNerfSyn", "healthBuffSyn", "heroAttackSyn",

        "restoreTargetMinionGen", "restoreFriendlyHeroGen", "restoreFriendlyMinionGen",
        "restoreTargetMinionSyn", "restoreFriendlyHeroSyn", "restoreFriendlyMinionSyn",

        "armorGen", "lifesteal", "lifestealGen",
        "armorSyn", "lifestealMinionSyn", "lifestealAllSyn",

        "eggGen", "damageFriendlyHeroGen", "echo", "echoGen", "rush", "rushGen", "magnetic", "magneticGen",
        "eggSyn", "damageFriendlyHeroSyn", "echoSyn", "echoAllSyn", "rushSyn", "rushAllSyn", "magneticSyn", "magneticAllSyn",

        "otherClassGen", "silverHandGen", "treantGen", "lackeyGen", "outcast", "outcastGen", "endTurnGen", "rushGiverGen",
        "otherClassSyn", "silverHandSyn", "treantSyn", "lackeySyn", "outcastSyn", "outcastAllSyn", "endTurnSyn", "rushGiverSyn",

        "dredge", "corpseGen",
        "dredgeSyn", "corpseSyn"
        //New Synergy Step 7
    };
    if(mechanic.startsWith("discover") || mechanic.startsWith("drawGen") || mechanic.startsWith("toYourHandGen"))   return true;
    return validMecs.contains(mechanic);
}


bool SynergyHandler::containsAll(const QString &text, const QString &words)
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


void SynergyHandler::testSynergies(const QString &miniSet)
{
    initSynergyCodes(true);
    int num = 0;

//    for(const QString &code: (const QStringList)Utility::getSetCodes("RETURN_OF_THE_LICH_KING", true, true))
//    for(const QString &code: (const QStringList)Utility::getSetCodesSpecific("TAVERNS_OF_TIME"))
//    for(const QString &code: (const QStringList)Utility::getStandardCodes())
    for(const QString &code: (const QStringList)Utility::getWildCodes())
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            CardType cardType = Utility::getTypeFromCode(code);
            QString text = Utility::cardEnTextFromCode(code).toLower();
            int attack = Utility::getCardAttribute(code, "attack").toInt();
            int health = Utility::getCardAttribute(code, "health").toInt();
            int cost = Utility::getCardAttribute(code, "cost").toInt();
            QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
            QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
            if(
                    containsAll(text, "give deathrattle")
//                    text.contains("can't attack heroes")
//                    mechanics.contains(QJsonValue("DEATHRATTLE"))
//                    referencedTags.contains(QJsonValue("COMBO"))
//                    cardType == MINION
//                    mechanics.contains(QJsonValue("CORRUPT"))
//                    && attack<4 && health<4
//                    && (attack + health)<7
//                    && !isTokenGen(code, mechanics, text)
//                    isCorpseGen(code, mechanics, text) && text.contains("risen")
//                    isCorpseSyn(code, text)
//                    && !isDrop2(code, cost, attack, health)
//                    && !isDrop3(code, cost, attack, health)
//                    && !isDrop4(code, cost, attack, health)


///Update bombing cards --> PlanHandler::isCardBomb (Hearthpwn Search: damage random)
//containsAll(text, "damage random")
///Update cartas que dan mana inmediato (monedas) --> CardGraphicsItem::getManaSpent (Hearthpwn Search: gain mana this turn only)
//containsAll(text, "gain mana this turn only") || containsAll(text, "refresh mana")
///Update cartas que en la practica tienen un coste diferente --> SynergyHandler::getCorrectedCardMana (Hearthpwn Search: cost / spend all your mana)
//containsAll(text, "spend all your mana") || text.contains("cost")
///Update cartas que roban un tipo especifico de carta (Curator) --> EnemyHandHandler::isDrawSpecificCards (Hearthpwn Search: draw from your deck)
//isDrawGen(code, text) || containsAll(text, "draw")
///Update cartas que roban una carta y la clonan (Mimic Pod) --> EnemyHandHandler::isClonerCard (Hearthpwn Search: draw cop)
//containsAll(text, "draw cop")
///Update AOE que marcan un objetivo principal y le hacen algo diferente que al resto (Swipe) --> MinionGraphicsItem::isAoeWithTarget (Hearthpwn Search: draw from your deck)
//isAoeGen(code, text)
                )
            {
//                qDebug()<<++num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;
                debugSynergiesCode(code, ++num);
//                qDebug()<<code+" "+Utility::cardEnNameFromCode(code);
//                qDebug()<<mechanics<<endl<<referencedTags;

                if(num>0 && num<=50)
                {
                    QDesktopServices::openUrl(QUrl(
                        "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
                        ));
                    QThread::msleep(100);
                }
            }
            Q_UNUSED(cardType);
            Q_UNUSED(text);
            Q_UNUSED(attack);
            Q_UNUSED(health);
            Q_UNUSED(cost);
            Q_UNUSED(mechanics);
            Q_UNUSED(referencedTags);
        }
    }

    clearSynergyLists();
}


void SynergyHandler::debugSynergiesSet(const QString &set, int openFrom, int openTo, const QString &miniSet, bool onlyCollectible)
{
    initSynergyCodes(true);

    QStringList codeList;
    if(Utility::needCodesSpecific(set)) codeList.append(Utility::getSetCodesSpecific(set));
    else                                codeList.append(Utility::getSetCodes(set, true, onlyCollectible));

    qDebug()<<endl<<"-----SynergiesNames.json-----"<<endl;
    for(const QString &code: codeList)
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            qDebug()<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
        }
    }

    qDebug()<<endl<<"-----Synergies.json-----"<<endl;
    int num = 0;
    for(const QString &code: codeList)
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            debugSynergiesCode(code, ++num);

            if(num>openFrom && num<=openTo)
            {
                QDesktopServices::openUrl(QUrl(
                    "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
//                    "https://cards.hearthpwn.com/enUS/" + code + ".png"
                    ));
                QThread::msleep(100);
            }
        }
    }

    clearSynergyLists();
}


bool SynergyHandler::shouldBeInSynergies(const QString &code)
{
    const QString &set = Utility::getCardAttribute(code, "set").toString();
    return (!code.startsWith("HERO_") && set != "WILD_EVENT" && set != "VANILLA") && !set.startsWith("PLACEHOLDER");
}


void SynergyHandler::debugMissingSynergiesAllSets()
{
    initSynergyCodes(true);
    int num = 0;
    const QStringList wildCodes = Utility::getWildCodes();
    for(const QString &code: (const QStringList)wildCodes)
    {
        //Missing synergy
        if(!synergyCodes.contains(code))
        {
            if(shouldBeInSynergies(code))
            {
                debugSynergiesCode(code, ++num);

//                if(num>150 && num<=170)
//                {
//                    QDesktopServices::openUrl(QUrl(
//                        "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
//                        ));
//                    QThread::msleep(100);
//                }
            }
        }
        else
        {
            QStringList invalidMecs;
            for(const QString &mechanic: (const QStringList)synergyCodes[code])
            {
                if(!isValidSynergyCode(mechanic))   invalidMecs.append(mechanic);
            }
            //Wrong spelled mechanic
            if(!invalidMecs.isEmpty())  qDebug()<<"DEBUG SYNERGIES: Code:"<<code<<"No mecs:"<<invalidMecs;
            StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);//Check fallos en synergy stats -> =GenMinionHealth1
        }
    }
    if(num == 0)
    {
        qDebug()<<"DEBUG SYNERGIES: OK - No missing synergies.";
    }
    else
    {
        qDebug()<<"DEBUG SYNERGIES: Those synergies missing.";
    }


    num = 0;
    const QStringList synergyCodesKeys = synergyCodes.keys();
    for(const QString &code: (const QStringList)synergyCodesKeys)
    {
        //Extra synergy, not used, need to be removed
        if(!shouldBeInSynergies(code))  debugSynergiesCode(code, ++num);
    }
    if(num == 0)
    {
        qDebug()<<"DEBUG SYNERGIES: OK - No extra synergies.";
    }
    else
    {
        qDebug()<<"DEBUG SYNERGIES: Those synergies to be removed.";
    }

    clearSynergyLists();
}


void SynergyHandler::debugSynergiesCode(QString code, int num)
{
    QString origCode = code;
    if(code.startsWith("CORE_"))
    {
        QString subCode = code.mid(5);
        if(!synergyCodes.contains(code) && synergyCodes.contains(subCode))  code = subCode;
    }

    QStringList mec;
    CardType cardType = Utility::getTypeFromCode(code);
    QString text = Utility::cardEnTextFromCode(code).toLower();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();
    int cost = Utility::getCardAttribute(code, "cost").toInt();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    if(isMurlocAllSyn(code, text))          mec<<"murlocAllSyn";
    if(isDemonAllSyn(code, text))           mec<<"demonAllSyn";
    if(isMechAllSyn(code, mechanics, text)) mec<<"mechAllSyn";
    if(isElementalAllSyn(code, text))       mec<<"elementalAllSyn";
    if(isBeastAllSyn(code, text))           mec<<"beastAllSyn";
    if(isTotemAllSyn(code, text))           mec<<"totemAllSyn";
    if(isPirateAllSyn(code, text))          mec<<"pirateAllSyn";
    if(isDragonAllSyn(code, text))          mec<<"dragonAllSyn";
    if(isNagaAllSyn(code, text))            mec<<"nagaAllSyn";
    if(isUndeadAllSyn(code, text))          mec<<"undeadAllSyn";

    if(isArcaneAllSyn(code, text))          mec<<"arcaneAllSyn";
    if(isFelAllSyn(code, text))             mec<<"felAllSyn";
    if(isFireAllSyn(code, text))            mec<<"fireAllSyn";
    if(isFrostAllSyn(code, text))           mec<<"frostAllSyn";
    if(isHolyAllSyn(code, text))            mec<<"holyAllSyn";
    if(isShadowAllSyn(code, text))          mec<<"shadowAllSyn";
    if(isNatureAllSyn(code, text))          mec<<"natureAllSyn";

    if(isWeaponGen(code, text))             mec<<"weaponGen";
    if(isSpellAllSyn(code, text))           mec<<"spellAllSyn";
    if(isWeaponAllSyn(code, text))          mec<<"weaponAllSyn";

    if(isDrop2(code, cost, attack, health)) mec<<"drop2";
    if(isDrop3(code, cost, attack, health)) mec<<"drop3";
    if(isDrop4(code, cost, attack, health)) mec<<"drop4";

    if(isDiscoverGen(code, mechanics, referencedTags))                      mec<<"discover";
    if(isDrawGen(code, text))                                               mec<<"drawGen";
    if(isToYourHandGen(code, cost, mechanics, text))                        mec<<"toYourHandGen";
    if(isAoeGen(code, text))                                                mec<<"aoeGen";
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mec<<"pingGen";
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"damageMinionsGen";
    if(isDestroyGen(code, mechanics, text))                                 mec<<"destroyGen";
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"reachGen";
    if(isOverload(code))                                                    mec<<"overload";
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mec<<"jadeGolemGen";
    if(isHeroPowerGen(code, text))                                          mec<<"heroPowerGen";
    if(isSecret(code, mechanics))                                           mec<<"secret";
    if(isEcho(code, mechanics))                                             mec<<"echo";
    if(isEchoGen(code, referencedTags))                                     mec<<"echoGen";
    if(isRush(code, mechanics))                                             mec<<"rush";
    if(isRushGen(code, referencedTags))                                     mec<<"rushGen";
    if(isMagnetic(code, mechanics))                                         mec<<"magnetic";
    if(isEggGen(code, mechanics, attack, cardType))                         mec<<"eggGen";
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mec<<"freezeEnemyGen";
    if(isDiscardGen(code, text))                                            mec<<"discardGen";
    if(isDeathrattleMinion(code, mechanics, cardType))                      mec<<"deathrattle o deathrattleOpponent";
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mec<<"deathrattle o deathrattleGen";
    if(isBattlecry(code, mechanics))                                        mec<<"battlecry";
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mec<<"silenceOwnGen";
    if(isTokenGen(code, mechanics, text))                                   mec<<"tokenGen";
    if(isTokenCardGen(code, cost, mechanics, referencedTags, text))         mec<<"tokenCardGen";
    if(isWindfuryMinion(code, mechanics, cardType))                         mec<<"windfury";
    if(isAttackBuffGen(code, text))                                         mec<<"attackBuffGen";
    if(isAttackNerfGen(code, text))                                         mec<<"attackNerfGen";
    if(isHealthBuffGen(code, text))                                         mec<<"healthBuffGen";
    if(isReturnGen(code, text))                                             mec<<"returnGen";
    if(isStealthGen(code, mechanics))                                       mec<<"stealthGen";
    if(isSpellDamageGen(code))                                              mec<<"spellDamageGen";
    if(isEvolveGen(code, text))                                             mec<<"evolveGen";
    if(isSpawnEnemyGen(code, text))                                         mec<<"spawnEnemyGen";
    if(isRestoreTargetMinionGen(code, text))                                mec<<"restoreTargetMinionGen";
    if(isRestoreFriendlyHeroGen(code, mechanics, referencedTags, text))     mec<<"restoreFriendlyHeroGen o lifesteal o lifestealGen";
    if(isRestoreFriendlyMinionGen(code, text))                              mec<<"restoreFriendlyMinionGen";
    if(isArmorGen(code, text))                                              mec<<"armorGen";
    if(isLifestealMinon(code, mechanics, cardType))                         mec<<"lifesteal";
    if(isLifestealGen(code, referencedTags))                                mec<<"lifestealGen";
    if(isTaunt(code, mechanics))                                            mec<<"taunt";
    else if(isTauntGen(code, referencedTags))                               mec<<"tauntGen";
    if(isDivineShield(code, mechanics))                                     mec<<"divineShield";
    else if(isDivineShieldGen(code, referencedTags))                        mec<<"divineShieldGen";
    if(isEnrageGen(code, mechanics))                                        mec<<"enrageGen";
    if(isCombo(code, mechanics))                                            mec<<"combo";
    if(isHandBuffGen(code, text))                                           mec<<"handBuffGen";
    if(isEnemyDrawGen(code, text))                                          mec<<"enemyDrawGen";
    if(isHeroAttackGen(code, text))                                         mec<<"heroAttackGen";
    if(isSpellBuffGen(code, text, mechanics, cardType))                     mec<<"spellBuffGen";
    if(isOtherClassGen(code, text))                                         mec<<"otherClassGen";
    if(isSilverHandGen(code, text))                                         mec<<"silverHandGen";
    if(isTreantGen(code, text))                                             mec<<"treantGen";
    if(isLackeyGen(code, text))                                             mec<<"lackeyGen";
    if(isOutcast(code, mechanics))                                          mec<<"outcast";
    if(isEndTurnGen(code, text))                                            mec<<"endTurnGen";
    if(isRushGiverGen(code, text))                                          mec<<"rushGiverGen";
    if(isDredge(code, mechanics))                                           mec<<"dredge";
    if(isCorpseGen(code, mechanics, text))                                  mec<<"corpseGen";
    //New Synergy Step 8 (Solo si busca patron)

    //Solo analizamos los que tienen patrones definidos
    if(isOverloadSyn(code, text))                                           mec<<"overloadSyn";
    if(isSecretAllSyn(code, referencedTags))                                mec<<"secretAllSyn";
    if(isFreezeEnemySyn(code, referencedTags, text))                        mec<<"freezeEnemySyn";
    if(isDiscardSyn(code, text))                                            mec<<"discardSyn";
    if(isDeathrattleGoodAllSyn(code, text))                                 mec<<"deathrattleGoodAllSyn";
    if(isBattlecryAllSyn(code, referencedTags))                             mec<<"battlecryAllSyn";
    if(isSilenceOwnSyn(code, mechanics))                                    mec<<"silenceOwnSyn";
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mec<<"tauntGiverSyn";
    if(isAttackBuffSyn(code, mechanics, attack, cardType))                  mec<<"attackBuffSyn";
    if(isAttackNerfSyn(code, text))                                         mec<<"attackNerfSyn";
    if(isTokenSyn(code, mechanics, text))                                   mec<<"tokenSyn";
    if(isReturnSyn(code, mechanics, cardType, text))                        mec<<"returnSyn";
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   mec<<"spellDamageSyn";
    if(isEnrageSyn(code, text))                                             mec<<"enrageSyn";
    if(isComboAllSyn(code, referencedTags))                                 mec<<"comboAllSyn";
    if(isTokenCardSyn(code, text))                                          mec<<"tokenCardSyn";
    if(isEggSyn(code, text))                                                mec<<"eggSyn";
    if(isHandBuffSyn(code, text))                                           mec<<"handBuffSyn";
    if(isEnemyDrawSyn(code, text))                                          mec<<"enemyDrawSyn";
    if(isSpellBuffSyn(code, text))                                          mec<<"spellBuffSyn";
    if(isOtherClassSyn(code, text))                                         mec<<"otherClassSyn";
    if(isOutcastAllSyn(code, referencedTags))                               mec<<"outcastAllSyn";
    if(isEndTurnSyn(code, text))                                            mec<<"endTurnSyn";
    if(isRushGiverSyn(code, mechanics, text))                               mec<<"rushGiverSyn";
    if(isDredgeSyn(code, text))                                             mec<<"dredgeSyn";
    if(isCorpseSyn(code, text))                                             mec<<"corpseSyn";
    //New Synergy Step 9 (Solo si busca patron)

    qDebug()<<num<<origCode+(origCode==code?"":" COPY of "+code)<<Utility::getCardAttribute(origCode, "set").toString()<<text;
    qDebug()<<origCode<<": ["<<Utility::cardEnNameFromCode(origCode)<<"],";
    if(synergyCodes.contains(code)) qDebug()<<"--MANUAL-- :"<<origCode<<": ["<<synergyCodes[code]<<"],";
    else                            qDebug()<<origCode<<": ["<<mec<<"],";

    Q_UNUSED(health);
}


//Increase counters
bool SynergyHandler::isSpellGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("spellGen");
    return false;
}
bool SynergyHandler::isWeaponGen(const QString &code, const QString &text)
{
    //NO TEST
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponGen");
    }
    else if(text.contains("equip "))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isMurlocGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("murlocGen");
    return false;
}
bool SynergyHandler::isDemonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("demonGen");
    return false;
}
bool SynergyHandler::isMechGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("mechGen");
    return false;
}
bool SynergyHandler::isElementalGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("elementalGen");
    return false;
}
bool SynergyHandler::isBeastGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("beastGen");
    return false;
}
bool SynergyHandler::isTotemGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("totemGen");
    return false;
}
bool SynergyHandler::isPirateGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("pirateGen");
    return false;
}
bool SynergyHandler::isDragonGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("dragonGen");
    return false;
}
bool SynergyHandler::isNagaGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("nagaGen");
    return false;
}
bool SynergyHandler::isUndeadGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("undeadGen");
    return false;
}
bool SynergyHandler::isArcaneGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("arcaneGen");
    return false;
}
bool SynergyHandler::isFelGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("felGen");
    return false;
}
bool SynergyHandler::isFireGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("fireGen");
    return false;
}
bool SynergyHandler::isFrostGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("frostGen");
    return false;
}
bool SynergyHandler::isHolyGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("holyGen");
    return false;
}
bool SynergyHandler::isShadowGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("shadowGen");
    return false;
}
bool SynergyHandler::isNatureGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("natureGen");
    return false;
}
bool SynergyHandler::isDiscoverDrawGen(const QString &code, int cost, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text)
{
    //TEST
    //&& (text.contains("draw") || text.contains("discover") || (text.contains("to") && text.contains("your") && text.contains("hand")))
    return(isDiscoverGen(code, mechanics, referencedTags) ||
            isDrawGen(code, text) ||
            isToYourHandGen(code, cost, mechanics, text));
}
bool SynergyHandler::isDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    return numDiscoverGen(code, mechanics, referencedTags)>0;
}
int SynergyHandler::numDiscoverGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    //TEST
    //&& text.contains("discover")
    if(synergyCodes.contains(code))
    {
        for(QString mechanic: (const QStringList)synergyCodes[code])
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
bool SynergyHandler::isDrawGen(const QString &code, const QString &text)
{
    return numDrawGen(code, text)>0;
}
int SynergyHandler::numDrawGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("draw")
    if(synergyCodes.contains(code))
    {
        for(QString mechanic: (const QStringList)synergyCodes[code])
        {
            if(mechanic.startsWith("drawGen"))
            {
                mechanic.remove(0,7);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        return 0;
    }
    else if((text.contains("draw") && !text.contains("drawn")) || containsAll(text, "discover deck"))
    {
        return 1;
    }
    return 0;
}
bool SynergyHandler::isToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text)
{
    return numToYourHandGen(code, cost, mechanics, text)>0;
}
int SynergyHandler::numToYourHandGen(const QString &code, int cost, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //&& (text.contains("to") && text.contains("your") && text.contains("hand"))
    if(synergyCodes.contains(code))
    {
        for(QString mechanic: (const QStringList)synergyCodes[code])
        {
            if(mechanic.startsWith("toYourHandGen"))
            {
                mechanic.remove(0,13);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        if(synergyCodes[code].contains("echo")) return 1;
        return 0;
    }
    else if(isEcho(code, mechanics))                                    return 1;
    else if(cost > 1 && mechanics.contains(QJsonValue("TWINSPELL")))    return 1;
    else if(text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return"))
    {
        return 1;
    }
    return 0;
}
bool SynergyHandler::isTaunt(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("taunt");
    }
    else if(mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGen") || synergyCodes[code].contains("tauntGiverGen");
    }
    else if(referencedTags.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isAoeGen(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeGen");
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
bool SynergyHandler::isPingGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                             const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingGen");
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
        else if(isRush(code, mechanics))  return true;
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool SynergyHandler::isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                              const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("reachGen");
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
bool SynergyHandler::isDamageMinionsGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                      const QString &text, const CardType &cardType, int attack)
{
    //TEST
//    (text.contains("damage") && text.contains("deal") &&
//                !text.contains("1 damage") && !text.contains("all") && !text.contains("hero")) &&
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageMinionsGen") || synergyCodes[code].contains("rushGiverGen");
    }
    else if(isRushGiverGen(code, text))
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
        else if(isRush(code, mechanics))  return true;
    }
    //Weapons
    else if(cardType == WEAPON) return true;
    return false;
}
bool SynergyHandler::isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroyGen");
    }
    else if(text.contains("destroy") && text.contains("minion") &&
            !text.contains("all"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    return false;
}
bool SynergyHandler::isEnrageGen(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //text.contains("takes") && text.contains("damage")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageGen");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")) || mechanics.contains(QJsonValue("FRENZY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOverload(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("overload");
    }
    else
    {
        int overload = Utility::getCardAttribute(code, "overload").toInt();
        return overload > 0;
    }
}
bool SynergyHandler::isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("jadeGolemGen");
    }
    else if(mechanics.contains(QJsonValue("JADE_GOLEM")) || referencedTags.contains(QJsonValue("JADE_GOLEM")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHeroPowerGen(const QString &code, const QString &text)
{
    //TEST
    //text.contains("hero power") || (text.contains("heal") && text.contains("deal damage") && cardClass == PRIEST)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("heroPowerGen");
    }
    else if(text.contains("hero power") || (text.contains("heal") && text.contains("deal damage")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSecret(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secret");
    }
    else if(mechanics.contains(QJsonValue("SECRET")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSecretGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secretGen");
    }
    return false;
}
bool SynergyHandler::isEcho(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echo");
    }
    else if(mechanics.contains(QJsonValue("ECHO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEchoGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echoGen");
    }
    else if(referencedTags.contains(QJsonValue("ECHO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRush(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rush");
    }
    else if(mechanics.contains(QJsonValue("RUSH")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRushGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushGen");
    }
    else if(referencedTags.contains(QJsonValue("RUSH")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isMagnetic(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("magnetic");
    }
    else if(mechanics.contains(QJsonValue("MODULAR")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isMagneticGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("magneticGen");
    }
    return false;
}
bool SynergyHandler::isEggGen(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("eggGen");
    }
    else if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDamageFriendlyHeroGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageFriendlyHeroGen");
    }
    return false;
}
bool SynergyHandler::isFreezeEnemyGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                 const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeEnemyGen");
    }
    else if(mechanics.contains(QJsonValue("FREEZE")))
    {
        return true;
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return !text.contains("frozen");
    }
    return false;
}
bool SynergyHandler::isDiscardGen(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discardGen");
    }
    else if(text.contains("discard") && text.contains("random card"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattle") || synergyCodes[code].contains("deathrattleOpponent");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleGoodAll(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                          const CardType &cardType)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    if(synergyCodes.contains(code))
    {
        return (synergyCodes[code].contains("deathrattle") || synergyCodes[code].contains("deathrattleGen")) &&
                !synergyCodes[code].contains("silenceOwnSyn") &&
                !synergyCodes[code].contains("deathrattleOpponent");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("DEATHRATTLE")) || referencedTags.contains(QJsonValue("DEATHRATTLE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isBattlecry(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecry");
    }
    else if(mechanics.contains(QJsonValue("BATTLECRY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isBattlecryGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecryGen");
    }
    return false;
}
bool SynergyHandler::isSilenceOwnGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silenceOwnGen");
    }
    else if(mechanics.contains(QJsonValue("SILENCE")))
    {
        return true;
    }
    else if(referencedTags.contains(QJsonValue("SILENCE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGiverGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverGen");
    }
    return false;
}
bool SynergyHandler::isTokenGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //(text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("2/2")) && !text.contains("opponent")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenGen");
    }
    else if(mechanics.contains(QJsonValue("REBORN")))   return true;
    else if((
                text.contains("1/1") || text.contains("1/2") || text.contains("1/3") ||
                text.contains("2/1") || text.contains("2/2") || text.contains("2/3") ||
                text.contains("3/1") || text.contains("3/2") || text.contains("3/3")
             )
            && text.contains("summon") && !text.contains("opponent"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenCardGen(const QString &code, int cost, const QJsonArray &mechanics,
                                    const QJsonArray &referencedTags, const QString &text)
{
    //Incluimos cartas que en conjunto permitan jugar 2+ cartas de coste 0/1/2
    //Resumen: 2+2, 1+X, 0
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenCardGen") || synergyCodes[code].contains("lackeyGen");
    }
    else if(cost == 0)                                                  return true;
    else if (cost == 1 &&
                (mechanics.contains(QJsonValue("TWINSPELL")) ||
                isDiscoverDrawGen(code, cost, mechanics, referencedTags, text))
            )
                                                                        return true;
    else if(cost == 2 && mechanics.contains(QJsonValue("TWINSPELL")))   return true;
    else if(cost < 4 && isEcho(code, mechanics))                        return true;
    else if(text.contains("lackey"))                                    return true;
    return false;
}
bool SynergyHandler::isCombo(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("combo");
    }
    else if(mechanics.contains(QJsonValue("COMBO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isComboGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboGen");
    }
    return false;
}
bool SynergyHandler::isWindfuryMinion(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("windfury");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("WINDFURY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isAttackBuffGen(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+")
//    && (text.contains("minion") || text.contains("character"))
//    && (text.contains("attack") || text.contains("/+")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackBuffGen");
    }
    else if(text.contains("+")
            && (text.contains("give") || text.contains("have"))
            && (text.contains("minion") || text.contains("character"))
            && (text.contains("attack") || text.contains("/+"))
            &&/* !text.contains("hand") && *//*!text.contains("random") && */!text.contains("c'thun"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isAttackNerfGen(const QString &code, const QString &text)
{
    //TEST
//    containsAll(text, "change minion attack 1")
//    || containsAll(text, "set minion attack 1")
//    || containsAll(text, "give minion attack -")
//    || containsAll(text, "reduce minion attack")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackNerfGen");
    }
    else if(containsAll(text, "change minion attack 1") || containsAll(text, "set minion attack 1")
            || containsAll(text, "give minion attack -") || containsAll(text, "reduce minion attack"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHealthBuffGen(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+")
//    && (text.contains("minion") || text.contains("character"))
//    && (text.contains("health") || text.contains("/+")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("healthBuffGen");
    }
    else if(text.contains("+")
            && (text.contains("give") || text.contains("have"))
            && (text.contains("minion") || text.contains("character"))
            && (text.contains("health") || text.contains("/+"))
            && /*!text.contains("hand") && *//*!text.contains("random") && */!text.contains("c'thun"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isReturnGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("return a friendly minion")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("returnGen");
    }
    else if(text.contains("return a friendly minion"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isStealthGen(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("STEALTH")) ||  referencedTags.contains(QJsonValue("STEALTH")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("stealthGen");
    }
    else if(mechanics.contains(QJsonValue("STEALTH")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDivineShield(const QString &code, const QJsonArray &mechanics)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DIVINE_SHIELD")) ||  referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShield");
    }
    else if(mechanics.contains(QJsonValue("DIVINE_SHIELD")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDivineShieldGen(const QString &code, const QJsonArray &referencedTags)
{
    //TEST
    //&& (mechanics.contains(QJsonValue("DIVINE_SHIELD")) ||  referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldGen");
    }
    else if(referencedTags.contains(QJsonValue("DIVINE_SHIELD")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRestoreTargetMinionGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("restore") // No hace falta "heal", restore lo cubre todo excepto Prophet Velen y Wickerflame Burnbristle
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreTargetMinionGen");
    }
    else if(text.contains("restore") && !text.contains("friendly") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHeroGen") || synergyCodes[code].contains("lifesteal")
                || synergyCodes[code].contains("lifestealGen");
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
bool SynergyHandler::isRestoreFriendlyMinionGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyMinionGen");
    }
    else if(text.contains("restore") && text.contains("friendly") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isArmorGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("armor")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armorGen");
    }
    else if(text.contains("armor"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isLifestealMinon(const QString &code, const QJsonArray &mechanics, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifesteal");
    }
    else if(cardType != MINION)  return false;
    else if(mechanics.contains(QJsonValue("LIFESTEAL")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isLifestealGen(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifestealGen");
    }
    else if(referencedTags.contains(QJsonValue("LIFESTEAL")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSpellDamageGen(const QString &code)
{
    //TEST
    //&& text.contains("spell") && text.contains("damage")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellDamageGen");
    }
    else
    {
        int spellDamage = Utility::getCardAttribute(code, "spellDamage").toInt();
        return spellDamage > 0;
    }
}
bool SynergyHandler::isEvolveGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("transform") && text.contains("cost") && text.contains("more")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("evolveGen");
    }
    else if(text.contains("transform") && text.contains("cost") && text.contains("more"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSpawnEnemyGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("summon") && text.contains("for") && text.contains("your") && text.contains("opponent")
    //Lo he abreviado pq algunos casos se escaban.
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spawnEnemyGen");
    }
    else if(text.contains("summon") && text.contains("opponent"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHandBuffGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("give") && text.contains("in your hand"))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("handBuffGen");
    }
    else if(text.contains("give") && text.contains("in your hand"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEnemyDrawGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("both players") || text.contains("each player") || text.contains("your opponent") || text.contains("your enemy")) && text.contains("draw") && !text.contains("when drawn")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enemyDrawGen");
    }
    else if((text.contains("both players") || text.contains("each player") || text.contains("your opponent") || text.contains("your enemy")) &&
            text.contains("draw") && !text.contains("when drawn"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHeroAttackGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("hero") || text.contains("character")) && text.contains("attack")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("heroAttackGen");
    }
    else if((text.contains("hero") || text.contains("character")) && text.contains("attack"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSpellBuffGen(const QString &code, const QString &text, const QJsonArray &mechanics, const CardType &cardType)
{
    //TEST
    //((text.contains("set") || text.contains("give")) &&
    //((text.contains("minion") && !text.contains("minions")) || (text.contains("character") && !text.contains("characters")))
    //&& cardType == SPELL && !mechanics.contains("SECRET"))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellBuffGen");
    }
    else if((text.contains("set") || text.contains("give")) &&
            ((text.contains("minion") && !text.contains("minions")) || (text.contains("character") && !text.contains("characters")))
            && cardType == SPELL && !mechanics.contains("SECRET"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOtherClassGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("opponent") || text.contains("another")) && text.contains("class")
    //&& (cardClass == NEUTRAL || cardClass == ROGUE)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("otherClassGen");
    }
    else if((text.contains("opponent") || text.contains("another")) && text.contains("class"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilverHandGen(const QString &code, const QString &text)
{
    //TEST
    //text.contains("silver hand")
    //&& (cardClass == NEUTRAL || cardClass == PALADIN)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silverHandGen");
    }
    else if(text.contains("silver hand"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTreantGen(const QString &code, const QString &text)
{
    //TEST
    //text.contains("treant")
    //&& (cardClass == NEUTRAL || cardClass == DRUID)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("treantGen");
    }
    else if(text.contains("treant"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isLackeyGen(const QString &code, const QString &text)
{
    //TEST
    //text.contains("lackey")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lackeyGen");
    }
    else if(text.contains("lackey"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOutcast(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("outcast");
    }
    else if(mechanics.contains(QJsonValue("OUTCAST")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOutcastGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("outcastGen");
    }
    return false;
}
bool SynergyHandler::isEndTurnGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("at the end of ") || text.contains("at the start of "))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("endTurnGen");
    }
    else if((text.contains("at the end of ") || text.contains("at the start of ")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRushGiverGen(const QString &code, const QString &text)
{
    //TEST
    //containsAll(text, "give rush")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushGiverGen");
    }
    else if(containsAll(text, "give rush"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDredge(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dredge");
    }
    else if(mechanics.contains(QJsonValue("DREDGE")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isCorpseGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("corpseGen");
    }
    else if(isTokenGen(code, mechanics, text) && !text.contains("risen"))
    {
        return true;
    }
    return false;
}
//New Synergy Step 10
bool SynergyHandler::isDrop2(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("drop2");
    }
    else if(cost == 2 && !(
                (attack==1 && health<4) ||
                (attack==2 && health<2)
                ))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDrop3(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("drop3");
    }
    else if(cost == 3 && health != 1 && !(
                (attack==1 && health<7) ||
                (attack==2 && health<4)
                ))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDrop4(const QString &code, int cost, int attack, int health)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("drop4");
    }
    else if(cost == 4 && health != 1 && !(
                (attack==1) ||
                (attack==2 && health<5) ||
                (attack==3 && health<5) ||
                (attack==4 && health<3)
                ))
    {
        return true;
    }
    return false;
}


//Synergy items
bool SynergyHandler::isSpellSyn(const QString &code)
{
    //NO TEST
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellSyn");
    }
    return false;
}
bool SynergyHandler::isSpellAllSyn(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("spell")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellAllSyn");
    }
    else
    {
        return  text.contains("spellburst") ||
                (text.contains("spell") && (text.contains("you cast") || text.contains("cost")));
    }
}
bool SynergyHandler::isWeaponSyn(const QString &code)
{
    //NO TEST
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponSyn");
    }
    return false;
}
bool SynergyHandler::isWeaponAllSyn(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("weapon")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("weaponAllSyn");
    }
    else
    {
        return text.contains("weapon") && !text.contains("opponent's weapon");
    }
}
bool SynergyHandler::isMurlocSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("murlocSyn");
    }
    return false;
}
bool SynergyHandler::isMurlocAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("murlocAllSyn");
    }
    else
    {
        return text.contains("murloc");
    }
}
bool SynergyHandler::isDemonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonSyn");
    }
    return false;
}
bool SynergyHandler::isDemonAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("demonAllSyn");
    }
    else
    {
        return text.contains("demon");
    }
}
bool SynergyHandler::isMechSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechSyn");
    }
    return false;
}
bool SynergyHandler::isMechAllSyn(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("mechAllSyn");
    }
    else if(isMagnetic(code, mechanics) || isMagneticGen(code))    return true;
    else
    {
        return text.contains("mech");
    }
}
bool SynergyHandler::isElementalSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalSyn");
    }
    return false;
}
bool SynergyHandler::isElementalAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("elementalAllSyn");
    }
    else
    {
        return text.contains("elemental");
    }
}
bool SynergyHandler::isBeastSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastSyn");
    }
    return false;
}
bool SynergyHandler::isBeastAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("beastAllSyn");
    }
    else
    {
        return text.contains("beast");
    }
}
bool SynergyHandler::isTotemSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemSyn");
    }
    return false;
}
bool SynergyHandler::isTotemAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("totemAllSyn");
    }
    else
    {
        return text.contains("totem");
    }
}
bool SynergyHandler::isPirateSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateSyn");
    }
    return false;
}
bool SynergyHandler::isPirateAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pirateAllSyn");
    }
    else
    {
        return text.contains("pirate");
    }
}
bool SynergyHandler::isDragonSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonSyn");
    }
    return false;
}
bool SynergyHandler::isDragonAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonAllSyn");
    }
    else
    {
        return text.contains("dragon");
    }
}
bool SynergyHandler::isNagaSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("nagaSyn");
    }
    return false;
}
bool SynergyHandler::isNagaAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("nagaAllSyn");
    }
    else
    {
        return text.contains("naga");
    }
}
bool SynergyHandler::isUndeadSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("undeadSyn");
    }
    return false;
}
bool SynergyHandler::isUndeadAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("undeadAllSyn");
    }
    else
    {
        return text.contains("undead");
    }
}
bool SynergyHandler::isArcaneSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("arcaneSyn");
    }
    return false;
}
bool SynergyHandler::isArcaneAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("arcaneAllSyn");
    }
    else
    {
        return containsAll(text, "arcane spell");
    }
}
bool SynergyHandler::isFelSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("felSyn");
    }
    return false;
}
bool SynergyHandler::isFelAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("felAllSyn");
    }
    else
    {
        return containsAll(text, "fel spell");
    }
}
bool SynergyHandler::isFireSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("fireSyn");
    }
    return false;
}
bool SynergyHandler::isFireAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("fireAllSyn");
    }
    else
    {
        return containsAll(text, "fire spell");
    }
}
bool SynergyHandler::isFrostSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("frostSyn");
    }
    return false;
}
bool SynergyHandler::isFrostAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("frostAllSyn");
    }
    else
    {
        return containsAll(text, "frost spell");
    }
}
bool SynergyHandler::isHolySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("holySyn");
    }
    return false;
}
bool SynergyHandler::isHolyAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("holyAllSyn");
    }
    else
    {
        return containsAll(text, "holy spell");
    }
}
bool SynergyHandler::isShadowSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("shadowSyn");
    }
    return false;
}
bool SynergyHandler::isShadowAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("shadowAllSyn");
    }
    else
    {
        return containsAll(text, "shadow spell");
    }
}
bool SynergyHandler::isNatureSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("natureSyn");
    }
    return false;
}
bool SynergyHandler::isNatureAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("natureAllSyn");
    }
    else
    {
        return containsAll(text, "nature spell");
    }
}
bool SynergyHandler::isDiscoverSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discoverSyn");
    }
    return false;
}
bool SynergyHandler::isDrawSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("drawSyn");
    }
    return false;
}
bool SynergyHandler::isToYourHandSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("toYourHandSyn");
    }
    return false;
}
bool SynergyHandler::isEnrageSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enrageSyn");
    }
    else if(text.contains("deal") && text.contains("1 damage") &&
            !text.contains("enemy") && !text.contains("random") && !text.contains("hero"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOverloadSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("overloadSyn");
    }
    else if(text.contains("overload"))
    {
        int overload = Utility::getCardAttribute(code, "overload").toInt();
        return overload == 0;
    }
    return false;
}
bool SynergyHandler::isPingSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingSyn");
    }
    return false;
}
bool SynergyHandler::isAoeSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("aoeSyn");
    }
    return false;
}
bool SynergyHandler::isTauntSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntSyn");
    }
    return false;
}
bool SynergyHandler::isTauntAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntAllSyn");
    }
    return false;
}
bool SynergyHandler::isSecretSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secretSyn");
    }
    return false;
}
bool SynergyHandler::isSecretAllSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("secretAllSyn");
    }
    else if(referencedTags.contains(QJsonValue("SECRET")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEchoSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echoSyn");
    }
    return false;
}
bool SynergyHandler::isEchoAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echoAllSyn");
    }
    return false;
}
bool SynergyHandler::isRushSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushSyn");
    }
    return false;
}
bool SynergyHandler::isRushAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushAllSyn");
    }
    return false;
}
bool SynergyHandler::isMagneticSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("magneticSyn");
    }
    return false;
}
bool SynergyHandler::isMagneticAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("magneticAllSyn");
    }
    return false;
}
bool SynergyHandler::isEggSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("eggSyn");
    }
    if(text.contains("swap") && text.contains("attack") && text.contains("health") && text.contains("minion"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDamageFriendlyHeroSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageFriendlyHeroSyn");
    }
    return false;
}
bool SynergyHandler::isFreezeEnemySyn(const QString &code, const QJsonArray &referencedTags, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("freezeEnemySyn");
    }
    else if(referencedTags.contains(QJsonValue("FREEZE")))
    {
        return text.contains("frozen");
    }
    return false;
}
bool SynergyHandler::isDiscardSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("discardSyn");
    }
    else if(text.contains("discard") && !text.contains("random card"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDeathrattleSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleSyn");
    }
    return false;
}
bool SynergyHandler::isDeathrattleGoodAllSyn(const QString &code, const QString &text)
{
    //TEST
//    (containsAll(text, "copy") || containsAll(text, "copies")) &&
//    (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
//    !containsAll(text, "in your deck")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleGoodAllSyn");
    }
    else if((containsAll(text, "copy") || containsAll(text, "copies")) &&
            (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
            !containsAll(text, "in your deck"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isBattlecrySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecrySyn");
    }
    return false;
}
bool SynergyHandler::isBattlecryAllSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecryAllSyn");
    }
    else if(referencedTags.contains(QJsonValue("BATTLECRY")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilenceOwnSyn(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silenceOwnSyn");
    }
    else if(mechanics.contains(QJsonValue("CANT_ATTACK")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTauntGiverSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tauntGiverSyn") || synergyCodes[code].contains("deathrattleOpponent") ||
               synergyCodes[code].contains("eggGen");
    }
    else if(mechanics.contains(QJsonValue("CANT_ATTACK")))
    {
        return true;
    }
    else if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenSyn(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    //TEST
//    && (text.contains("+") && (text.contains("minions") || text.contains("characters"))
//    && !text.contains("hand") && !text.contains("health")
//    || (text.contains("control") && text.contains("least") && text.contains("minions")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenSyn");
    }
    else if(mechanics.contains(QJsonValue("INFUSE")))
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
bool SynergyHandler::isTokenCardSyn(const QString &code, const QString &text)
{
    //TEST
    //text.contains("play") && text.contains("card") && !text.contains("player")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenCardSyn");// || synergyCodes[code].contains("comboGen");
    }
    else if(text.contains("play") && text.contains("card") && !text.contains("player"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isComboSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboSyn");
    }
    return false;
}
bool SynergyHandler::isComboAllSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboAllSyn");
    }
    else if(referencedTags.contains(QJsonValue("COMBO")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isWindfuryMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("windfuryMinionSyn");
    }
    return false;
}
bool SynergyHandler::isAttackBuffSyn(const QString &code, const QJsonArray &mechanics, int attack, const CardType &cardType)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackBuffSyn") || synergyCodes[code].contains("eggGen");
    }
    else if(cardType == MINION && attack == 0 && mechanics.contains(QJsonValue("DEATHRATTLE")) && !mechanics.contains(QJsonValue("TAUNT")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isAttackNerfSyn(const QString &code, const QString &text)
{
    //TEST
    //    containsAll(text, "destroy less attack")
    //    || containsAll(text, "control less attack")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("attackNerfSyn");
    }
    else if(containsAll(text, "destroy less attack") || containsAll(text, "control less attack"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHealthBuffSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("healthBuffSyn");
    }
    return false;
}
bool SynergyHandler::isReturnSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text)
{
    //TEST
    //&& mechanics.contains(QJsonValue("BATTLECRY"))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("returnSyn");
    }
    else if(cardType != MINION)  return false;
    else if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echo");
    }
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
    else if(isEcho(code, mechanics)) return true;
    return false;
}
bool SynergyHandler::isStealthSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("stealthSyn");
    }
    return false;
}
bool SynergyHandler::isDivineShieldSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldSyn");
    }
    return false;
}
bool SynergyHandler::isDivineShieldAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("divineShieldAllSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreTargetMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreTargetMinionSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyHeroSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHeroSyn");
    }
    return false;
}
bool SynergyHandler::isRestoreFriendlyMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyMinionSyn");
    }
    return false;
}
bool SynergyHandler::isArmorSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armorSyn");
    }
    return false;
}
bool SynergyHandler::isLifestealMinionSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifestealMinionSyn");
    }
    return false;
}
bool SynergyHandler::isLifestealAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lifestealAllSyn");
    }
    return false;
}
bool SynergyHandler::isSpellDamageSyn(const QString &code, const QJsonArray &mechanics, const CardType &cardType, const QString &text)
{
    //TEST
//    && (text.contains("all") || text.contains("adjacent")) && (text.contains("damage"))
//    && cardType == SPELL
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellDamageSyn");
    }
    else if(cardType != SPELL)  return false;
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
bool SynergyHandler::isEvolveSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("evolveSyn");
    }
    return false;
}
bool SynergyHandler::isSpawnEnemySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spawnEnemySyn");
    }
    return false;
}
bool SynergyHandler::isHandBuffSyn(const QString &code, const QString &text)
{
    //TEST
    //((text.contains("enchantments")) || (text.contains("of this minion")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("handBuffSyn");
    }
    else if((text.contains("enchantments")) || (text.contains("of this minion")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEnemyDrawSyn(const QString &code, const QString &text)
{
    //TEST
    //text.contains("shuffle") && (text.contains("opponent") || text.contains("enemy")) && text.contains("deck") && text.contains("when drawn")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("enemyDrawSyn");
    }
    else if(text.contains("shuffle") && (text.contains("opponent") || text.contains("enemy")) &&
            text.contains("deck") && text.contains("when drawn"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isHeroAttackSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("heroAttackSyn");
    }
    return false;
}
bool SynergyHandler::isSpellBuffSyn(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("spell") && text.contains("cast") && text.contains("minion")) || text.contains("enchantments")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spellBuffSyn");
    }
    else if((text.contains("spell") && text.contains("cast") && text.contains("minion")) || text.contains("enchantments"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isOtherClassSyn(const QString &code, const QString &text)
{
    //TEST
    //text.contains("card") && text.contains("from another class")
    //&& (cardClass == NEUTRAL || cardClass == ROGUE)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("otherClassSyn");
    }
    else if(text.contains("card") && text.contains("from another class"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilverHandSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silverHandSyn");
    }
    return false;
}
bool SynergyHandler::isTreantSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("treantSyn");
    }
    return false;
}
bool SynergyHandler::isLackeySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("lackeySyn");
    }
    return false;
}
bool SynergyHandler::isOutcastSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("outcastSyn");
    }
    return false;
}
bool SynergyHandler::isOutcastAllSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("outcastAllSyn");
    }
    else if(referencedTags.contains(QJsonValue("OUTCAST")))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEndTurnSyn(const QString &code, const QString &text)
{
    //TEST
//    (containsAll(text, "copy") || containsAll(text, "copies")) &&
//    (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
//    !containsAll(text, "in your deck")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("endTurnSyn");
    }
    else if((containsAll(text, "copy") || containsAll(text, "copies")) &&
            (containsAll(text, "/") || containsAll(text, "attack") || containsAll(text, "health")) &&
            !containsAll(text, "in your deck"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRushGiverSyn(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //text.contains("poisonous") || text.contains("also damages")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushGiverSyn");
    }
    else if(mechanics.contains(QJsonValue("ENRAGED")) || mechanics.contains(QJsonValue("FRENZY")))
    {
        return true;
    }
    else if(text.contains("poisonous") || text.contains("also damages"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isDredgeSyn(const QString &code, const QString &text)
{
    //TEST
//    containsAll(text, "bottom deck")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dredgeSyn");
    }
    else if(containsAll(text, "bottom deck"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isCorpseSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("corpseSyn");
    }
    else if(text.contains("corpse"))
    {
        return true;
    }
    return false;
}
//New Synergy Step 11


int SynergyHandler::getCorrectedCardMana(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    QString otherCode = Utility::otherCodeConstant(code);

    //Evitar draw/discover cost 0/1/2 -> no draw y mantenemos coste original
    //Descuento minions on board -> suponemos 2 aliados y 2 enemigos (4 total)
    //Descuento spells cast this game -> 3
    {
    QStringList candidates = {
        GRASP_THE_FUTURE, TIMEWAY_WANDERER, BLOODBLOOM, PRIMORDIAL_GLYPH, FAR_SIGHT, CHEAT_DEATH,
        LUNAS_POCKET_GALAXY, ACADEMIC_ESPIONAGE, HAUNTING_VISIONS, WAXMANCY, IMPRISONED_SATYR, SKULL_OF_GULDAN, DEMONIC_STUDIES,
        DRACONIC_STUDIES, ATHLETIC_STUDIES, PRIMORDIAL_STUDIES, CARRION_STUDIES, NATURE_STUDIES, ILLIDARI_STUDIES, INSIGHT,
        FLOODSAIL_DECKHAND, EFFICIENT_OCTOBOT, LIVING_SEED, SCABBS_CUTTERBUTTER, KINDLING_ELEMENTAL, CELESTIAL_INK_SET,
        RUNED_MITHRIL_ROD, SIGIL_OF_ALACRITY, TO_THE_FRONT, CERATHINE_FLEETRUNNER, RECONNAISSANCE, SHIVERING_SORCERESS,
        BRACING_COLD, WAYWARD_SAGE, SWIFTSCALE_TRICKSTER, PLANTED_EVIDENCE, SERRATED_BONE_SPIKE, MURLOCULA, BONELORD_FROSTWHISPER,
        ROTTEN_RODENT, PRIESTESS_VALISHJ
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 0;
    }

    {
    QStringList candidates = {
        FROM_THE_DEPTHS, EYE_BEAM, AUCTIONHOUSE_GAVEL, SI7_SKULKER, PRIDE_SEEKER, STORMPIKE_MARSHAL, MURKWATER_SCRIBE,
        ANUBREKHAN
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 1;
    }

    {
    QStringList candidates = {
        FRENZIED_FELWING, PALM_READING, FELGORGER, FROSTWOLF_WARMASTER, STORMPIKE_BATTLE_RAM, SEAFLOOR_GATEWAY, GREENTHUMB_GARDENER,
        LIGHT_OF_THE_PHOENIX
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 2;
    }

    {
    QStringList candidates = {
        NERUBIAN_PROPHET, CORRIDOR_CREEPER, SECOND_RATE_BRUISER, DREAMPETAL_FLORIST, FEL_GUARDIANS, CUTTING_CLASS, GRANITE_FORGEBORN,
        CLUMSY_COURIER, EXCAVATION_SPECIALIST
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 3;
    }

    {
    QStringList candidates = {
        MOLTEN_BLADE, SHIFTER_ZERUS, SHIFTING_SCROLL, CHAMELEOS, UMBRAL_OWL, TENT_TRASHER, FROSTSABER_MATRIARCH, WILDPAW_GNOLL,
        SCRIBBLING_STENOGRAPHER, SHADOW_OF_DEMISE, RELIC_OF_DIMENSIONS, STITCHED_GIANT
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 4;
    }

    {
    QStringList candidates = {
        FORBIDDEN_SHAPING, FORBIDDEN_FLAME, FORBIDDEN_HEALING, FORBIDDEN_RITUAL, FORBIDDEN_ANCIENT, FORBIDDEN_WORDS, MOGU_FLESHSHAPER,
        RABBLE_BOUNCER, DEVOUT_PUPIL, EMBIGGEN, POWER_WORD_FORTITUDE, SHIELD_SHATTER, LOKHOLAR_THE_ICE_LORD, LIGHTRAY, CRYPT_KEEPER,
        VENGEFUL_WALLOPER
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 5;
    }

    {
    QStringList candidates = {
        TIMEBOUND_GIANT, DEMONBOLT, SEA_GIANT, BLOODBOIL_BRUTE, FLESH_GIANT, IREBOUND_BRUTE, GOLDSHIRE_GNOLL, THE_GARDENS_GRACE,
        GIGANTOTEM
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 6;
    }

    {
    QStringList candidates = {
        URZUL_GIANT
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 7;
    }

    {
    QStringList candidates = {
        CLOCKWORK_GIANT, MULCHMUNCHER
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 8;
    }

    {
    QStringList candidates = {
        GRAVE_HORROR
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 9;
    }

    {
    QStringList candidates = {
        LIVING_MANA, NAGA_GIANT
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 10;
    }

    int overload = Utility::getCardAttribute(code, "overload").toInt();
    return std::min(10, deckCard.getCost()) + overload;
}


/*
Races
murlocGen (murloc no es un tag, se saca de la carta)
murlocSyn o murlocAllSyn



Mechanics links:
spellGen (no usamos spell, es un cardType)
spellSyn o spellAllSyn
weaponGen (no usamos weapon, es un cardType)
weaponSyn o weaponAllSyn
destroyGen o damageMinionsGen
taunt o tauntGen
tauntSyn o tauntAllSyn
deathrattle (solo para minions) o deathrattleGen o deathrattleOpponent(solo se ejecuta en turno oponente)
deathrattleSyn o deathrattleGoodAllSyn
deathrattleOpponent y taunGiverSyn (no hace falta poner taunGiverSyn si es deathrattleOpponent)
divineShield o divineShieldGen
divineShieldSyn o divineShieldAllSyn
secret o secretGen
secretSyn o secretAllSyn
echo o echoGen
echoSyn o echoAllSyn
rush o rushGen
rushSyn o rushAllSyn
magnetic o magneticGen
magneticSyn o magneticAllSyn
windfury - windfuryMinionSyn
lifesteal o lifestealGen
lifestealMinionSyn o lifestealAllSyn
lifesteal y restoreFriendlyHeroGen (no hace falta poner restore si es lifesteal)
restoreTargetMinionGen <--> restoreFriendlyHeroGen
restoreTargetMinionGen o restoreFriendlyMinionGen
restoreTargetMinionSyn, restoreFriendlyMinionSyn, restoreFriendlyHeroSyn
damageFriendlyHeroGen/damageFriendlyHeroSyn
eggGen/eggSyn
=(>|<)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)



Mechanics list:
spellGen, weaponGen,
drop2, drop3, drop4,
murlocGen, demonGen, mechGen, elementalGen, beastGen, totemGen, pirateGen, dragonGen, nagaGen
arcaneGen, felGen, fireGen, frostGen, holyGen, shadowGen, natureGen
discover, drawGen, toYourHandGen, enemyDrawGen
taunt, tauntGen, divineShield, divineShieldGen, windfury, overload
jadeGolemGen, heroPowerGen, secret, secretGen, freezeEnemyGen, discardGen, stealthGen

reachGen, aoeGen, pingGen, damageMinionsGen, destroyGen
deathrattle, deathrattleGen, deathrattleOpponent, silenceOwnGen, battlecry, battlecryGen, returnGen
enrageGen, rushGiverGen, tauntGiverGen, evolveGen, spawnEnemyGen, spellDamageGen, handBuffGen, spellBuffGen
tokenGen, tokenCardGen, combo, comboGen, attackBuffGen, attackNerfGen, healthBuffGen, heroAttackGen
restoreTargetMinionGen, restoreFriendlyHeroGen, restoreFriendlyMinionGen, armorGen, lifesteal, lifestealGen
eggGen, damageFriendlyHeroGen, echo, echoGen, rush, rushGen, magnetic, magneticGen, otherClassGen,
silverHandGen, treantGen, lackeyGen, outcast, outcastGen, endTurnGen, dredge, corpse
=(>|<)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)

//New Synergy Step 12

Double check:
DAMAGE/DESTROY: reachGen(no atk1), aoeGen(spellDamageSyn/eggSyn),
                pingGen(enrageSyn), damageMinionsGen, destroyGen(8+ damage/no rush)
BATTLECRY/COMBO/ECHO/DEATHRATTLE: returnsyn(battlecry/choose one/combo/echo),
                silenceOwnSyn/evolveSyn(deathrattle/malo)
ENRAGE/FRENZY/TAKE DAMAGE: enrageGen(take damage)/rushGiverSyn
RUSHGIVERSYN: enrageGen/frenzy, poison, damage adjacents
SUMMON: tokenGen(summon)
TOYOURHAND: tokenCardGen(small cards to hand) <--> tokenGen(2+) o spellGen
PLAY CARDS: tokenCardSyn
BUFF ALL: tokenSyn(beneficio masa), handBuffGen
CANT ATTACK: silenceOwnSyn, tauntGiverSyn
COPY ITSELF: handBuffSyn
5/5 COPY: deathrattleGoodAllSyn, endTurnSyn
DESTROY TARDIO: freezeEnemySyn
DESTROY PROPIO/SWAP/copia 1-1: eggSyn

RESTORE: restoreTargetMinionGen o restoreFriendlyMinionGen <--> restoreFriendlyHeroGen
DAMAGE HERO: damageFriendlyHeroGen/damageFriendlyHeroSyn
CHARGE/RUSH: pingGen(atk1) o damageMinionsGen(no atk1) <--> reachGen(no atk1) o rush
STEALTH: stealthGen <--> reachGen(no atk1)

SPAWN ENEMIES: spawnEnemyGen
DRAW ENEMY: enemyDrawGen/enemyDrawSyn
HERO ATTACK: heroAttackGen/heroAttackSyn <--> weaponAllSyn
HERO POWER: heroPowerGen
SPELL BUFF: spellBuffGen/spellBuffSyn
ATK/HEALTH: attackNerfGen, attackBuffGen, healthBuffGen
OTHER CLASS: otherClassGen/otherClassSyn
DREDGE: dredge/dredgeSyn
CORPSE: corpseGen/corpseSyn
SILVER HAND: silverHandGen/silverHandSyn
TREANT: treantGen/treantSyn
LACKEY: lackeyGen/lackeySyn

    - Drop2 (Derrota 2/2 --> 3+/1+, 2/2+, 1/4+)
    - Drop3 (Derrota 3/3 --> 3+/2+, 2/4+, 1/7+), no health 1
    - Drop4 (Derrota 4/4 --> 5+/2+, 4/3+, 3/5+, 2/5+, no 1/x), no health 1

    +pingGen (NO RANDOM/NO DEATHRATTLE)
    +damageMinionsGen/destroyGen (SI RANDOM/NO DEATHRATTLE)
    +aoeGen (SI RANDOM/SI DEATHRATTLE)


REGLAS
+No hacemos sinergias si requieren 3 cartas, por ejemplo la carta que crea dos 1/1 si tiene un dragon en la mano no es tokenGen,
    pq necesitariamos 3 cartas, la que genera 1/1s, el dragon y el que tiene tokenSyn, con hechizos tampoco.
+Cartas con tags/synergias condicionales, solo las ponemos si son muy faciles de satisfacer, (Nesting roc si, servant of kalimos no).
    Synergias con todo tu mazo son faciles, como robar 2 murlocs.
    Synergias JOUST son faciles.
    Synergias SPELLBURST/(con hechizos) no pero tags si, suponemos 1 hechizo. Synergias con schools of magic no.
    Synergias CORRUPT si. Synergias OUTCAST si.
    Synergias con arma en rogue/warrior si.
    Synergias HONORABLE KILL no en minions sin rush, si en hechizos/armas/minion rush.
    Synergias INFUSE si.
    Synergias CORPSES si.
    Synergias con undead en deathknight si.
+Una carta no es spellGen si para generarlos requiere otros hechizos.
+returnGen es sinergia debil por eso solo las mostramos en un sentido, para evitar mostrarlas continuamente en todos lados.
+elementalGen/dragonGen/nagaGen solo para generacion de cartas en mano, no en board.
+eggGen implica (attackBuffSyn y tauntGiverSyn), echo implica toYourHandGen, rush implica pingGen/damageMinionsGen, lackeyGen implica tokenCardGen
+tokenCardGen Incluye cartas que en conjunto permitan jugar 2+ cartas de coste 2 las 2 o
    1 carta de coste 0/1 y otra de cualquier coste o 1 carta de coste 0 (no hace falta indicarlo si coste 0).
    Resumen: 3+3+3(echo), 2+2, 1+X, 0
+drawGen/toYourHandGen: Costes 3-6 son toYourHandGen(1).
    Dragones cuentan x2 en mana.
    Murlocs cuestan %2 en mana.
    Pueden incluir un numero al final para indicar que roba mas de 1 carta. El maximo es 5 para evitar indicar
    que un mazo es muy pesado solo por una carta. Para toYourHandGen si nos dan varias cartas a lo largo de varios turnos (como Pyros)
    sumamos el mana de todo lo que nos dan, lo dividimos entre 4 y ese sera el numero the toYourHandGen.
    Cartas que se juegan indefinidamente 1 vez/turno suponemos que las jugamos 5 turnos. Ej Headcrack (coste 3) es toYourHandGen3 (3x4/4).
+discover from your deck (no copy) es drawGen ya que la carta viene de tu mazo. (BAR_545)
+discover cards de minions que no van a la mano sino que se invocan no son marcadas como discover, para que no aumente el deck weight.
+drawSyn: Somos restrictivos. Solo lo ponemos si cada vez que se roba hay un efecto claro, no la posibilidad de robar algo bueno.
    Shuffle into your deck no son drawSyn. Tiene que funcionar con todo tipo de cartas; minions, weapon o spells.
+tokenGen son 2 small minions (max 3/3), reborn y deathrattle son tokenGen (max 3/3)
    tambien cuentan las cartas generadas a mano (tokenCardGen).
+No son tokenSyn las cartas "Destroy friendly minion", synergia muy debil.
+freezeEnemyGen deben poder usarse sobre enemigos
+pingGen (NO RANDOM/NO DEATHRATTLE): tienen como proposito eliminar divineShield y rematar, deben ser proactivos, no random ni deathrattle.
+damageMinionsGen y destroyGen (SI RANDOM/NO DEATHRATTLE): deben ser proactivos, permitimos que sean random pero no deathrattle ni secretos (random o no)
+aoeGen (SI RANDOM/SI DEATHRATTLE): puede ser deathrattle random (>= 2dmg), quitaremos manualmente excepciones como el tentaculo de n'zoth o unstable ghoul.
+aoeGen: los aoe tienen que afectar al menos 3 objetivos
+aoeGen: no son destroyGen ni damageMinionsGen (ni siquiera token rush),
    a no ser que haga mucho dano a uno y poco a los demas, o que tenga 2 modos.
+aoeGen: Los bombing/missiles no son pingGen, si tienen los suficientes misiles/bombs (3) aoeGen pero nunca reachGen ya que no son fiables.
+spellDamageSyn es para aoe o damage a 2 objetivos
+No incluir sinergias que no sean explicitas, por ejemplo aoe freeze no deberian tener sinergias con otros aoe.
+lifesteal y windfury los ponemos en minion/hechizos/armas pero las synergias solo son con minions
+Si una carta mejora cuando mayor sea nuestra mano no tengo en cuenta como sinergias otras cartas que hagan nuestra mano mayor,
    es una sinergia muy debil.
+No usamos los "=Gen(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)" ya que al no poder distinguir si se generan en el tablero, mano o mazo
    no se pueden asociar bien con los syn.
+Sinergias con 1/1s incluye lackeySyn y silverHandSyn. (jadeGolemSyn no existe)
+Summon a 5/5 copy of a minion in your deck, es una sinergia que no mostramos ya que el deck es muy grande y es dificil de acertar.
+Rush minion nunca los consideramos destroyGen por mucho ataque que tengan.
+RushGiverGen/RushGen: Los rushGiverGen solo son rushGen si automaticamente le dan rush al invocarlas.
+RushGiverSyn/EnrageGen/Frenzy: Solo son rushGiverSyn, los enrage minions de 5+ mana con un enrage significativo. Taunt 2/6 enrage +3 atk no lo es.
+ReturnSyn lo ponemos tambien battlecry neutros, como ambos jugadores roban 1 carta.
+Sinergias con cartas de alto coste solo las ponemos para coste 6+ ("=>SynMinionCost6", "=>SynSpellCost6", "=>SynWeaponCost6")
+CorpseGen para todo tokenGen que no genere risen minions (no dejan corpse)
+CorpseSyn solo si gasta 2+ corpses.
+evolveSyn: suele ponerse en minions que pierdan su valor en el battlecry o que tengan un mal deathrattle.
    Lo ponemos en minions que cuesten 2.5+ mana de lo que deberian por stats (3/3 es 3 mana, 3/4 es 3.5 mana)
    o 1.5+ si tienen reduccion de coste (nerubian prophet, thing from below) o son baratos (<5)
    o minions que suelen hacer rush sin morir y pierden atributos.
+drop234: Inicialmente se asignan solo por su coste, si no son basta con no poner la key, no existen keys nodrop234 ya que no son necesarias.
+Un drop debe ser eficiente jugado en su turno suponiendo que el enemigo tenga en juego un minion del turno anterior y
        el jugador no tiene ningun minion.
    Un drop debe poner algo en la mesa, aunque sea un secreto. Si solo elimina cosas, roba cartas o buffa no es un drop.
    Si dan cristales de mana son drops.
    Si tiene buenos stats para jugarlo como drop, es un drop, aunque sea mejor jugarlo mas tarde (infuse, corrupt).
    Los minions con 0 de ataque no son drops, huevos tampoco, ya que su uso optimo es en otro turno.
    Los buffos no son drops, supon que tu board esta vacio.
    Los minions can't attack no son drops, a no ser que sea muy facil hacerlos atacar (hero power)
    Los drop ideales son minions con altos stats. Una bola de fuego no es un drop4, el 3/3 que hace 3 de dano al azar si es un drop4
        ya que lo que mata es un drop3 enemigo.
    No es un drop2 si preferimos crear un 1/1 con heropower ha sacarlo.
    No poner un drop en un coste diferente de su mana a no ser que haya un razon de peso. El unico "Deadly Poison" y overload 1
        1+1 = drop2 / 2+1 = drop3 / 3+1 = drop4 / Todo lo demas es drop de su coste (4+1 = drop4)
    Stats minimos sin ningun extra en tempo, considerar que las condiciones con cartas especificas no se cumplen,
        como secretos, razas, hechizos, quizas holding.
        - Drop2 (Derrota 2/2 --> 3+/1+, 2/2+, 1/4+)
        - Drop3 (Derrota 3/3 --> 3+/2+, 2/4+, 1/7+), no health 1
        - Drop4 (Derrota 4/4 --> 5+/2+, 4/3+, 3/5+, 2/5+, no 1/x), no health 1
*/
