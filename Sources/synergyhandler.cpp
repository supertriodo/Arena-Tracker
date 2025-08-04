#include "synergyhandler.h"
#include "themehandler.h"
#include "Synergies/keysynergies.h"
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
    cardTypeCounters[V_LOCATION] = new DraftItemCounter(this, "Location");
    cardTypeCounters[V_LOCATION_ALL] = new DraftItemCounter(this, "Location");

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
    raceCounters[V_QUILBOAR] = new DraftItemCounter(this, "Quilboar");
    raceCounters[V_DRAENEI] = new DraftItemCounter(this, "Draenei");
    //New race step

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
    raceCounters[V_QUILBOAR_ALL] = new DraftItemCounter(this, "Quilboar");
    raceCounters[V_DRAENEI_ALL] = new DraftItemCounter(this, "Draenei");
    //New race step

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
    mechanicCounters[V_RESTORE_FRIENDLY_HEROE] = new DraftItemCounter(this, "Heal");
    mechanicCounters[V_ARMOR] = new DraftItemCounter(this, "Armor");

    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this, "Deathrattle");
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(this, "Deathrattle");
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this, "Jade Golem");
    mechanicCounters[V_HERO_POWER] = new DraftItemCounter(this, "Hero Power");

    KeySynergies::createKeySynergies();
    KeySynergies::setSynergyCodes(&synergyCodes);

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
    KeySynergies::resetAll();

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
    if(cardRace.contains(QUILBOAR) || cardRace.contains(ALL))
    {
        raceCounters[V_QUILBOAR]->increase(code);
        raceCounters[V_QUILBOAR_ALL]->increase(code);
    }
    else if(isQuilboarGen(code))    raceCounters[V_QUILBOAR_ALL]->increase(code);
    if(cardRace.contains(DRAENEI) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAENEI]->increase(code);
        raceCounters[V_DRAENEI_ALL]->increase(code);
    }
    else if(isDraeneiGen(code))    raceCounters[V_DRAENEI_ALL]->increase(code);
    //New race step

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
    if(isQuilboarSyn(code))                         raceCounters[V_QUILBOAR]->increaseSyn(code);
    else if(isQuilboarAllSyn(code, text))           raceCounters[V_QUILBOAR_ALL]->increaseSyn(code);
    if(isDraeneiSyn(code))                         raceCounters[V_DRAENEI]->increaseSyn(code);
    else if(isDraeneiAllSyn(code, text))           raceCounters[V_DRAENEI_ALL]->increaseSyn(code);
    //New race step
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
    if(cardType == LOCATION)
    {
        cardTypeCounters[V_LOCATION]->increase(code);
        cardTypeCounters[V_LOCATION_ALL]->increase(code);
    }
    else if(isLocationGen(code))
    {
        cardTypeCounters[V_LOCATION_ALL]->increase(code);
    }


    if(isSpellSyn(code))                cardTypeCounters[V_SPELL]->increaseSyn(code);
    else if(isSpellAllSyn(code, text))  cardTypeCounters[V_SPELL_ALL]->increaseSyn(code);
    if(isWeaponSyn(code))               cardTypeCounters[V_WEAPON]->increaseSyn(code);
    else if(isWeaponAllSyn(code, text)) cardTypeCounters[V_WEAPON_ALL]->increaseSyn(code);
    if(isLocationSyn(code))             cardTypeCounters[V_LOCATION]->increaseSyn(code);
    else if(isLocationAllSyn(code, text)) cardTypeCounters[V_LOCATION_ALL]->increaseSyn(code);
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

    KeySynergies::updateKeySynergies(code, mechanics, referencedTags, text, cardType, attack, cost);
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

    //Evita mostrar spellSyn/spellAllSyn cards en cada hechizo que veamos, es sinergia debil
//    if(cardType == SPELL)
//    {
//        cardTypeCounters[V_SPELL]->insertSynCards(synergyTagMap);
//        cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
//    }
//    else if(isSpellGen(code))                   cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergyTagMap);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isWeaponGen(code, text))            cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergyTagMap);
    if(cardType == LOCATION)
    {
        cardTypeCounters[V_LOCATION]->insertSynCards(synergyTagMap);
        cardTypeCounters[V_LOCATION_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isLocationGen(code))                cardTypeCounters[V_LOCATION_ALL]->insertSynCards(synergyTagMap);


    if(isSpellSyn(code))                        cardTypeCounters[V_SPELL]->insertCards(synergyTagMap);
    else if(isSpellAllSyn(code, text))          cardTypeCounters[V_SPELL_ALL]->insertCards(synergyTagMap);
    if(isWeaponSyn(code))                       cardTypeCounters[V_WEAPON]->insertCards(synergyTagMap);
    else if(isWeaponAllSyn(code, text))         cardTypeCounters[V_WEAPON_ALL]->insertCards(synergyTagMap);
    if(isLocationSyn(code))                     cardTypeCounters[V_LOCATION]->insertCards(synergyTagMap);
    else if(isLocationAllSyn(code, text))       cardTypeCounters[V_LOCATION_ALL]->insertCards(synergyTagMap);
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
    if(cardRace.contains(QUILBOAR) || cardRace.contains(ALL))
    {
        raceCounters[V_QUILBOAR]->insertSynCards(synergyTagMap);
        raceCounters[V_QUILBOAR_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isQuilboarGen(code))    raceCounters[V_QUILBOAR_ALL]->insertSynCards(synergyTagMap);
    if(cardRace.contains(DRAENEI) || cardRace.contains(ALL))
    {
        raceCounters[V_DRAENEI]->insertSynCards(synergyTagMap);
        raceCounters[V_DRAENEI_ALL]->insertSynCards(synergyTagMap);
    }
    else if(isDraeneiGen(code))    raceCounters[V_DRAENEI_ALL]->insertSynCards(synergyTagMap);
    //New race step

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
    if(isQuilboarSyn(code))                         raceCounters[V_QUILBOAR]->insertCards(synergyTagMap);
    else if(isQuilboarAllSyn(code, text))           raceCounters[V_QUILBOAR_ALL]->insertCards(synergyTagMap);
    if(isDraeneiSyn(code))                          raceCounters[V_DRAENEI]->insertCards(synergyTagMap);
    else if(isDraeneiAllSyn(code, text))            raceCounters[V_DRAENEI_ALL]->insertCards(synergyTagMap);
    //New race step
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

    KeySynergies::getKeySynergies(code, synergyTagMap, mechanics, referencedTags, text, cardType, attack, cost);
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


bool SynergyHandler::isValidSynergyCode(const QString &mechanic, QRegularExpressionMatch *match)
{
    static const auto regex = QRegularExpression("^=[<>]?(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)[0-9]*$");
    if(mechanic.contains(regex, match))
    {
        return true;
    }
    static const auto regex2 = QRegularExpression("^(discover|draw|toYourHand)[2-5]?$");
    if(mechanic.contains(regex2, match))
    {
        return true;
    }
    QStringList validMecs = {
        "spellGen", "weaponGen", "locationGen",
        "spellSyn", "weaponSyn", "locationSyn",
        "spellAllSyn", "weaponAllSyn", "locationAllSyn",

        "murlocGen", "demonGen", "mechGen", "elementalGen", "beastGen", "totemGen", "pirateGen",
        "dragonGen", "nagaGen", "undeadGen", "quilboarGen", "draeneiGen",
        "murlocSyn", "demonSyn", "mechSyn", "elementalSyn", "beastSyn", "totemSyn", "pirateSyn",
        "dragonSyn", "nagaSyn", "undeadSyn", "quilboarSyn", "draeneiSyn",
        "murlocAllSyn", "demonAllSyn", "mechAllSyn", "elementalAllSyn", "beastAllSyn", "totemAllSyn", "pirateAllSyn",
        "dragonAllSyn", "nagaAllSyn", "undeadAllSyn", "quilboarAllSyn", "draeneiAllSyn",
        //New race step
        "arcaneGen", "felGen", "fireGen", "frostGen", "holyGen", "shadowGen", "natureGen",
        "arcaneSyn", "felSyn", "fireSyn", "frostSyn", "holySyn", "shadowSyn", "natureSyn",
        "arcaneAllSyn", "felAllSyn", "fireAllSyn", "frostAllSyn", "holyAllSyn", "shadowAllSyn", "natureAllSyn",

        "drop2", "drop3", "drop4",

        "discover", "draw", "toYourHand",
        "discoverSyn", "drawSyn", "toYourHandSyn",
        "restoreFriendlyHero", "armor",
        "restoreFriendlyHeroSyn", "armorSyn",
        "reach", "taunt", "tauntGen",
        "reachSyn", "tauntSyn", "tauntAllSyn",
        "damageMinions", "ping", "aoe", "destroy",
        "damageMinionsSyn", "pingSyn", "aoeSyn", "destroySyn",

        "deathrattle", "deathrattleGen", "deathrattleOpponent",
        "deathrattleSyn", "deathrattleGoodAllSyn",

        "jadeGolemGenGen", "heroPowerGenGenX"
    };

    const auto keys = KeySynergies::getListKeySynergies();
    for(const QString &keyS: keys)
    {
        validMecs << keyS << keyS+"Gen" << keyS+"Syn" << keyS+"AllSyn";
    }

    return validMecs.contains(mechanic);
}


void SynergyHandler::testSynergies(const QString &miniSet)
{
    initSynergyCodes(true);
    int num = 0;

    // for(QString &code: (QStringList)Utility::getSetCodes("SPACE", true, false))
   // for(QString &code: (QStringList)Utility::getSetCodesSpecific("TREASURES"))
   // for(QString &code: (QStringList)Utility::getStandardCodes())
   for(QString &code: (QStringList)Utility::getWildCodes())
    {
        if(code.startsWith("VAN_"))     continue;
        if(code.startsWith("CORE_"))    code = code.mid(5);

        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            CardType cardType = Utility::getTypeFromCode(code);
            QString text = Utility::cardEnTextFromCode(code).toLower();
            QString name = Utility::cardEnNameFromCode(code).toLower();
            int attack = Utility::getCardAttribute(code, "attack").toInt();
            int health = Utility::getCardAttribute(code, "health").toInt();
            int cost = Utility::getCardAttribute(code, "cost").toInt();
            QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
            QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
            if(
                KeySynergies::containsAll(text, "starship")
                // KeySynergies::isKey("overload", code, mechanics, referencedTags, text, cardType, attack, cost)
                // text.contains("kindred")
//                mechanics.contains(QJsonValue("MAGNETIC"))
//                referencedTags.contains(QJsonValue("CHOOSE_ONE"))
//                cardType == MINION
//                mechanics.contains(QJsonValue("CORRUPT"))
//                && attack<4 && health<4
//                cost > 5
//                && !isTokenGen(code, mechanics, text)
//                && !isDrop2(code, cost, attack, health)
//                && !isDrop3(code, cost, attack, health)
               // && !isDrop4(code, cost, attack, health)


///Update bombing cards --> PlanHandler::isCardBomb (Hearthpwn Search: damage random)
// KeySynergies::containsAll(text, "damage random") || KeySynergies::containsAll(text, "damage split")
///Update cartas que dan mana inmediato (monedas) --> CardGraphicsItem::getManaSpent (Hearthpwn Search: gain mana this turn only)
// KeySynergies::containsAll(text, "gain mana this turn only") || KeySynergies::containsAll(text, "refresh mana")
///Update cartas que en la practica tienen un coste diferente --> SynergyHandler::getCorrectedCardMana (Hearthpwn Search: cost / spend all your mana)
// KeySynergies::containsAll(text, "spend all your mana") || text.contains("cost")//cambio 3+ mana / -+ 1 o 2 no molestarse
///Update cartas que roban un tipo especifico de carta (Curator) --> EnemyHandHandler::isDrawSpecificCards (Hearthpwn Search: draw from your deck)
// isDrawGen(code, text) || KeySynergies::containsAll(text, "draw")
///Update cartas que roban una carta y la clonan (Mimic Pod) --> EnemyHandHandler::isClonerCard (Hearthpwn Search: draw cop)
// KeySynergies::containsAll(text, "draw cop")
///Update AOE que marcan un objetivo principal y le hacen algo diferente que al resto (Swipe) --> MinionGraphicsItem::isAoeWithTarget
// isAoeGen(code, text)
                )
            {
                debugSynergiesCode(code, ++num);
                // qDebug()<<++num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;
                // qDebug()<<code+" "+Utility::cardEnNameFromCode(code);
                // qDebug()<<code;
                // qDebug()<<mechanics<<endl<<referencedTags;

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
            Q_UNUSED(name);
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
//                    "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
                    "https://cards.hearthpwn.com/enUS/" + code + ".png"
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
    return (!code.startsWith("HERO_") && !set.isEmpty() &&
            set != "WILD_EVENT" && set != "VANILLA");
}


void SynergyHandler::debugMissingSynergiesAllSets()
{
    initSynergyCodes(true);
    int num = 0;
    const QStringList wildCodes = Utility::getWildCodes();
    for(const QString &code: (const QStringList)wildCodes)
    {
        //Missing synergy
        bool codeMissing = !synergyCodes.contains(code);
        if(codeMissing)
        {
            bool subCodeMissing = true;
            if(code.startsWith("CORE_"))
            {
                QString subCode = code.mid(5);
                if(synergyCodes.contains(subCode))  subCodeMissing = false;
            }

            if(subCodeMissing && shouldBeInSynergies(code))
            {
                debugSynergiesCode(code, ++num);

               // if(num>0 && num<=50)
               // {
               //     QDesktopServices::openUrl(QUrl(
               //         "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
               //         ));
               //     QThread::msleep(100);
               // }
            }
        }
        else
        {
            QRegularExpressionMatch *match = new QRegularExpressionMatch();
            QStringList invalidMecs;
            for(const QString &mechanic: (const QStringList)synergyCodes[code])
            {
                if(!isValidSynergyCode(mechanic, match))    invalidMecs.append(mechanic);
            }
            //Wrong spelled mechanic
            if(!invalidMecs.isEmpty())  qDebug()<<"DEBUG SYNERGIES: Code:"<<code<<"No mecs:"<<invalidMecs;
            StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);//Check fallos en synergy stats -> =GenMinionHealth1
            delete match;
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
    if(isQuilboarAllSyn(code, text))        mec<<"quilboarAllSyn";
    if(isDraeneiAllSyn(code, text))         mec<<"draeneiAllSyn";
    //New race step

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
    if(isLocationAllSyn(code, text))        mec<<"locationAllSyn";

    if(isDrop2(code, cost, attack, health)) mec<<"drop2";
    if(isDrop3(code, cost, attack, health)) mec<<"drop3";
    if(isDrop4(code, cost, attack, health)) mec<<"drop4";


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

    mec << KeySynergies::debugKeySynergies(code, mechanics, referencedTags, text, cardType, attack, cost);


    QString name = Utility::cardEnNameFromCode(origCode);
    qDebug()<<num<<origCode+(origCode==code?"":" COPY of "+code)<<Utility::getCardAttribute(origCode, "set").toString()<<text;
    qDebug()<<origCode<<": ["<<name<<"],";
    if(synergyCodes.contains(code)) qDebug()<<"--MANUAL-- :"<<origCode<<": ["<<synergyCodes[code]<<"],";
    else
    {
        qDebug()<<origCode<<": ["<<mec<<"],";

        //Para revisiones de cartas
//        for(const QString &icode: synergyCodes.keys())
//        {
//            QString iname = Utility::cardEnNameFromCode(icode);
//            if(name == iname)   qDebug()<<"--REV NAME-- :"<<icode<<"-"<<origCode<<": ["<<synergyCodes[icode]<<"],";
//        }
    }

    Q_UNUSED(health);
}


void SynergyHandler::debugDrops()
{
    initSynergyCodes(true);

    QString filename = "global.gz.json";
    QFile file(Utility::extraPath() + "/" + filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit pDebug("ERROR: Failed to open " + filename);
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QMap<QString, QJsonObject> statsMap;

    QList<QPair<QString, float>> dropRatioList;
    const QJsonArray &data = QJsonDocument::fromJson(jsonData).object().value("stats").toArray();
    for(const QJsonValue &card: data)
    {
        QJsonObject cardObject = card.toObject();
        QString code = cardObject.value("cardId").toString();
        if(!Utility::getCardAttribute(code, "collectible").toBool())    continue;
        QJsonObject cardStatsObject = cardObject.value("stats").toObject();
        int played = cardStatsObject.value("played").toInt();
        if(played == 0) continue;
        int onCurve = cardStatsObject.value("playedOnCurve").toInt();
        float dropRatio = onCurve/(float)played;
        dropRatioList << qMakePair(code, dropRatio);
        statsMap.insert(code, cardStatsObject);
    }

    std::sort(dropRatioList.begin(), dropRatioList.end(), [](const QPair<QString, float> &a, const QPair<QString, float> &b) {
        return a.second < b.second;
    });

    int num = 0;
    for(const auto &pair: qAsConst(dropRatioList))
    {
        const auto &code = pair.first;
        const auto &dropRatio = pair.second;
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();
        int cost = Utility::getCardAttribute(code, "cost").toInt();

        //>0.2 drop
        //0.2 - 0.15 duda
        //<0.15 no drop
        if(cost == 2)
        // if(cost == 3)
        // if(cost == 4)
        {
            int played = statsMap[code].value("played").toInt();
            bool drop2 = isDrop2(code, cost, attack, health);
            bool drop3 = isDrop3(code, cost, attack, health);
            bool drop4 = isDrop4(code, cost, attack, health);
            bool drop = drop2 || drop3 || drop4;
            if(played>10000)
            {
                qDebug()<<(drop?"\t":"")<<pair<<(drop2?"YES":"-----")<<
                    (drop3?"YES":"-----")<<
                    (drop4?"YES":"-----");
            }
            else
            {
                qDebug()<<(drop?"\t":"")<<pair<<(drop2?"YES":"-----")<<
                    (drop3?"YES":"-----")<<
                    (drop4?"YES":"-----")<<
                    "PLAYED:"<<played;
            }

            if(dropRatio > 0.15 && !drop)//Drop altos
            // if(dropRatio < 0.2 && drop)//Drop bajos, por debajo de 0.15 solo dejar drops perfectos (buenos stats) a no ser que su efecto sea tan potente que se desperdiciaria en drop.
            {
                num++;
                if(num>0 && num<=50)
                {
                    QDesktopServices::openUrl(QUrl(
                        "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
                        ));
                    QThread::msleep(100);
                }
            }
        }
    }

    clearSynergyLists();
}


//Increase counters
bool SynergyHandler::isSpellGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("spellGen");
    return false;
}
bool SynergyHandler::isWeaponGen(const QString &code, const QString &text)
{
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
bool SynergyHandler::isLocationGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("locationGen");
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
bool SynergyHandler::isQuilboarGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("quilboarGen");
    return false;
}
bool SynergyHandler::isDraeneiGen(const QString &code)
{
    if(synergyCodes.contains(code)) return synergyCodes[code].contains("draeneiGen");
    return false;
}
//New race step
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
            if(mechanic.startsWith("toYourHand"))
            {
                mechanic.remove(0,10);
                if(!mechanic.isEmpty())     return mechanic.toInt();
                return 1;
            }
        }
        if(synergyCodes[code].contains("echo")) return 1;
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
        return synergyCodes[code].contains("tauntGen") || synergyCodes[code].contains("tauntGiver");
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
        return synergyCodes[code].contains("aoe");
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
        return synergyCodes[code].contains("ping");
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
bool SynergyHandler::isReachGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                              const QString &text, const CardType &cardType, int attack)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("reach");
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
        return synergyCodes[code].contains("damageMinions") || synergyCodes[code].contains("rushGiver");
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
bool SynergyHandler::isDestroyGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroy");
    }
    else if(text.contains("destroy") && text.contains("minion") &&
            !text.contains("all"))
    {
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
        else return true;
    }
    return false;
}
bool SynergyHandler::isJadeGolemGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("jadeGolemGenGen");
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
        return synergyCodes[code].contains("heroPowerGenGenX");
    }
    else if(text.contains("hero power") || (text.contains("heal") && text.contains("deal damage")))
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
bool SynergyHandler::isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHero") || synergyCodes[code].contains("lifesteal")
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
bool SynergyHandler::isArmorGen(const QString &code, const QString &text)
{
    //TEST
    //&& text.contains("armor")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armor");
    }
    else if(text.contains("armor"))
    {
        return true;
    }
    return false;
}
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
bool SynergyHandler::isLocationSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("locationSyn");
    }
    return false;
}
bool SynergyHandler::isLocationAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("locationAllSyn");
    }
    else
    {
        return text.contains("location");
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
    else if(mechanics.contains(QJsonValue("MAGNETIC"))) return true;
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
bool SynergyHandler::isQuilboarSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("quilboarSyn");
    }
    return false;
}
bool SynergyHandler::isQuilboarAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("quilboarAllSyn");
    }
    else
    {
        return text.contains("quilboar");
    }
}
bool SynergyHandler::isDraeneiSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("draeneiSyn");
    }
    return false;
}
bool SynergyHandler::isDraeneiAllSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("draeneiAllSyn");
    }
    else
    {
        return text.contains("draenei");
    }
}
//New race step
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
        return KeySynergies::containsAll(text, "arcane spell");
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
        return KeySynergies::containsAll(text, "fel spell");
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
        return KeySynergies::containsAll(text, "fire spell");
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
        return KeySynergies::containsAll(text, "frost spell");
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
        return KeySynergies::containsAll(text, "holy spell");
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
        return KeySynergies::containsAll(text, "shadow spell");
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
        return KeySynergies::containsAll(text, "nature spell");
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
bool SynergyHandler::isPingSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("pingSyn");
    }
    return false;
}
bool SynergyHandler::isDamageMinionsSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("damageMinionsSyn");
    }
    return false;
}
bool SynergyHandler::isDestroySyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("destroySyn");
    }
    return false;
}
bool SynergyHandler::isReachSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("reachSyn");
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
    else if((KeySynergies::containsAll(text, "copy") || KeySynergies::containsAll(text, "copies")) &&
            (KeySynergies::containsAll(text, "/") || KeySynergies::containsAll(text, "attack") || KeySynergies::containsAll(text, "health")) &&
            !KeySynergies::containsAll(text, "in your deck"))
    {
        return true;
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
bool SynergyHandler::isArmorSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("armorSyn");
    }
    return false;
}


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
        ROTTEN_RODENT, PRIESTESS_VALISHJ, FREQUENCY_OSCILLATOR, LOVE_EVERLASTING, BIG_DREAMS, BLOOD_TREANT, AQUA_ARCHIVIST,
        HUNTERS_INSIGHT, SANDBOX_SCOUNDREL, ENSMALLEN, AVIANA_ELUNES_CHOSEN, REANIMATED_PTERRORDAX, ENTOMOLOGIST_TORU
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 0;
    }

    {
    QStringList candidates = {
        FROM_THE_DEPTHS, EYE_BEAM, AUCTIONHOUSE_GAVEL, SI7_SKULKER, PRIDE_SEEKER, STORMPIKE_MARSHAL, MURKWATER_SCRIBE,
        LIFE_FROM_DEATH, RUSH_THE_STAGE, HOLY_COWBOY, LOAD_THE_CHAMBER, SPARKLING_PHIAL, BOULDERING_BUDDY, FELFIRE_BONFIRE
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 1;
    }

    {
    QStringList candidates = {
        FRENZIED_FELWING, PALM_READING, FELGORGER, FROSTWOLF_WARMASTER, STORMPIKE_BATTLE_RAM, SEAFLOOR_GATEWAY, GREENTHUMB_GARDENER,
        LIGHT_OF_THE_PHOENIX, SPREAD_THE_WORD, JAZZ_BASS, ALTERED_CHORD, CATTLE_RUSTLER
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 2;
    }

    {
    QStringList candidates = {
        NERUBIAN_PROPHET, CORRIDOR_CREEPER, SECOND_RATE_BRUISER, DREAMPETAL_FLORIST, FEL_GUARDIANS, CUTTING_CLASS, GRANITE_FORGEBORN,
        CLUMSY_COURIER, EXCAVATION_SPECIALIST, ANUBREKHAN, INZAH, WISDOM_OF_NORGANNON, TRAM_OPERATOR, SPIRIT_PEDDLER, URSOL
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 3;
    }

    {
    QStringList candidates = {
        MOLTEN_BLADE, SHIFTER_ZERUS, SHIFTING_SCROLL, CHAMELEOS, UMBRAL_OWL, TENT_TRASHER, FROSTSABER_MATRIARCH, WILDPAW_GNOLL,
        SCRIBBLING_STENOGRAPHER, SHADOW_OF_DEMISE, RELIC_OF_DIMENSIONS, STITCHED_GIANT, DJ_MANASTORM, STARSTRUNG_BOW, PRISMATIC_BEAM,
        THRISTY_DRIFTER, MANTLE_SHAPER, TABLE_FLIP, EREDAR_BRUTE, AGAMAGGAN
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 4;
    }

    {
    QStringList candidates = {
        FORBIDDEN_SHAPING, FORBIDDEN_FLAME, FORBIDDEN_HEALING, FORBIDDEN_RITUAL, FORBIDDEN_ANCIENT, FORBIDDEN_WORDS, MOGU_FLESHSHAPER,
        RABBLE_BOUNCER, DEVOUT_PUPIL, EMBIGGEN, POWER_WORD_FORTITUDE, SHIELD_SHATTER, LOKHOLAR_THE_ICE_LORD, LIGHTRAY, CRYPT_KEEPER,
        VENGEFUL_WALLOPER, ABYSSAL_BASSIST, IMPRISONED_HORROR, CULTIVATION, FORBIDDEN_FRUIT, RED_GIANT
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 5;
    }

    {
    QStringList candidates = {
        TIMEBOUND_GIANT, DEMONBOLT, SEA_GIANT, BLOODBOIL_BRUTE, FLESH_GIANT, IREBOUND_BRUTE, THE_GARDENS_GRACE,
        GIGANTOTEM, GOLDSHIRE_GNOLL, LIVING_HORIZON, SEASIDE_GIANT
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 6;
    }

    {
    QStringList candidates = {
        URZUL_GIANT, LOST_EXARCH, FYE_THE_SETTING_SUN
    };
    if(candidates.contains(code) || candidates.contains(otherCode)) return 7;
    }

    {
    QStringList candidates = {
        CLOCKWORK_GIANT, MULCHMUNCHER, RESKA_THE_PIT_BOSS
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
        LIVING_MANA, NAGA_GIANT, FANOTTEM_LORD_OF_THE_OPERA, YOGG_SARON_UNLEASHED, PLAYHOUSE_GIANT, THE_CEASELESS_EXPANSE
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
locationGen (no usamos location, es un cardType)
locationSyn o locationAllSyn
destroy o damageMinions
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
windfuryMinion - windfuryMinionSyn
lifesteal o lifestealGen
lifestealSyn o lifestealAllSyn
lifesteal y restoreFriendlyHero (no hace falta poner restore si es lifesteal)
restoreTargetMinion <--> restoreFriendlyHero
restoreTargetMinion o restoreFriendlyMinion
restoreTargetMinionSyn, restoreFriendlyMinionSyn, restoreFriendlyHeroSyn
damageFriendlyHero/damageFriendlyHeroSyn
egg/eggSyn
=(>|<)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)



Mechanics list:
spellGen, weaponGen,
drop2, drop3, drop4,
murlocGen, demonGen, mechGen, elementalGen, beastGen, totemGen, pirateGen, dragonGen, nagaGen, undeadGen, quilboarGen, draeneiGen
//New race step
arcaneGen, felGen, fireGen, frostGen, holyGen, shadowGen, natureGen
discover, draw, toYourHand, enemyDraw
taunt, tauntGen, divineShield, divineShieldGen, windfuryMinion, overload
jadeGolemGenGen, heroPowerGenGenX, secret, secretGen, freezeEnemy, discard, stealth

reach, aoe, ping, damageMinions, destroy
deathrattle, deathrattleGen, deathrattleOpponent, silenceOwn, battlecry, battlecry, return
enrage, rushGiver, tauntGiver, evolve, spawnEnemy, spellDamage, handBuff, spellBuff
token, tokenCard, combo, comboGen, attackBuff, attackNerf, healthBuff, heroAttack
restoreTargetMinion, restoreFriendlyHero, restoreFriendlyMinion, armor, lifesteal, lifestealGen
egg, damageFriendlyHero, echo, echoGen, rush, rushGen, magnetic, magneticGen, otherClass,
silverHand, treant, lackey, outcast, outcastGen, endTurn, dredge, corpse, chooseOne, chooseOne
excavate, libram, libramGen, starship, starshipGen, shuffle
=(>|<)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)(0-20)

//New Synergy Step 3

Double check:
DAMAGE/DESTROY: reach(no atk1), aoe(spellDamageSyn/eggSyn),
                ping(enrageSyn), damageMinions, destroy(8+ damage/no rush)
BATTLECRY/COMBO/ECHO/DEATHRATTLE: returnsyn(battlecry/choose one/combo/echo),
                silenceOwnSyn/evolveSyn(deathrattle/malo)
ENRAGE/FRENZY/TAKE DAMAGE: enrage(take damage)/rushGiverSyn
RUSHGIVERSYN: enrage/frenzy, poison, damage adjacents
SUMMON: token(summon)
TOYOURHAND: tokenCard(small cards to hand) <--> token(2+) o spellGen
PLAY CARDS: tokenCardSyn
BUFF ALL: tokenSyn(buff 3+ minions), handBuff
CANT ATTACK: silenceOwnSyn, tauntGiverSyn
COPY ITSELF: handBuffSyn
5/5 COPY: deathrattleGoodAllSyn, endTurnSyn
DESTROY PROPIO/SWAP/copia 1-1: eggSyn

RESTORE: restoreTargetMinion o restoreFriendlyMinion <--> restoreFriendlyHero
DAMAGE HERO: damageFriendlyHero/damageFriendlyHeroSyn
CHARGE/RUSH: ping(atk1) o damageMinions(no atk1) <--> reach(no atk1) o rush
STEALTH: stealth <--> reach(no atk1)

SPAWN ENEMIES: spawnEnemy
DRAW ENEMY: enemyDraw/enemyDrawSyn
HERO ATTACK: heroAttack/heroAttackSyn <--> weaponAllSyn
HERO POWER: heroPower
SPELL BUFF: spellBuff/spellBuffSyn
ATK/HEALTH: attackNerf, attackBuff, healthBuff
OTHER CLASS: otherClass/otherClassSyn
DREDGE: dredge/dredgeSyn
SHUFFLE: shuffle/shuffleSyn
CORPSE: corpse/corpseSyn
SILVER HAND: silverHand/silverHandSyn
TREANT: treant/treantSyn
LACKEY: lackey/lackeySyn

    - Drop2 (Derrota 2/2 --> 3+/1+, 2/1+?, 1/3+?) ? debe hacer algo mas (facilmente) aunque no sea en board
    - Drop3 (Derrota 3/3 --> 3+/2+?, 2/3+?, 1/5+?), no health 1,
    - Drop4 (Derrota 4/4 --> 5+/2+, 4/3+?, 3/4+?, 2/5+, no 1/x), no health 1

    +ping (NO RANDOM/NO DEATHRATTLE)
    +damageMinions/destroy (SI RANDOM/NO DEATHRATTLE)
    +aoe (SI RANDOM/SI DEATHRATTLE)

GEN MANO: elementalGen/dragonGen/nagaGen/draeneiGen
ALL SCHOOLS:"arcaneAllSyn", "felAllSyn", "fireAllSyn", "frostAllSyn", "holyAllSyn", "shadowAllSyn", "natureAllSyn"
ALL RACES:  "murlocSyn", "demonSyn", "mechSyn", "elementalSyn", "beastSyn", "totemSyn", "pirateSyn", "dragonSyn", "nagaSyn", "undeadSyn", "quilboarSyn", "draeneiSyn"
            "murlocSyn", "demonSyn", "mechSyn", "elementalAllSyn", "beastSyn", "totemSyn", "pirateSyn", "dragonAllSyn", "nagaAllSyn", "undeadSyn", "quilboarSyn", "draeneiAllSyn"
            "murlocAllSyn", "demonAllSyn", "mechAllSyn", "elementalAllSyn", "beastAllSyn", "totemAllSyn", "pirateAllSyn", "dragonAllSyn", "nagaAllSyn", "undeadAllSyn", "quilboarAllSyn", "draeneiAllSyn"
//New race step


REGLAS
+No hacemos sinergias si requieren 3 cartas, por ejemplo la carta que crea dos 1/1 si tiene un dragon en la mano no es token,
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
    Synergias FINALE si.
    Synergias OVERHEAL en priest si.
    Synergias QUICKDRAW no.
+Una carta no es spellGen si para generarlos requiere otros hechizos.
+return/spellSyn/spellAllSyn es sinergia debil por eso solo las mostramos en un sentido, para evitar mostrarlas continuamente en todos lados.
+elementalGen/dragonGen/nagaGen/draeneiGen solo para generacion de cartas en mano, no en board.
+egg implica (attackBuffSyn y tauntGiverSyn), echo implica toYourHand, rush implica ping/damageMinions, lackey implica tokenCard,
    weapon/weaponGen implica damageFriendlyHero
+tokenCard Incluye cartas que en conjunto permitan jugar 2+ cartas de coste 2 las 2 o
    1 carta de coste 0/1 y otra de cualquier coste o 1 carta de coste 0 (no hace falta indicarlo si coste 0).
    Resumen: 3+3+3(echo), 2+2, 1+X, 0
+draw/toYourHand: Costes 3-5 son toYourHand(1).
    Dragones cuentan x1.5 o x2 en mana.
    Murlocs cuestan %2 en mana.
    Pueden incluir un numero al final para indicar que roba mas de 1 carta. El maximo es 5 para evitar indicar
    que un mazo es muy pesado solo por una carta. Para toYourHand si nos dan varias cartas a lo largo de varios turnos (como Pyros)
    sumamos el mana de todo lo que nos dan, lo dividimos entre 4 y ese sera el numero de toYourHand.
    Cartas que se juegan indefinidamente 1 vez/turno suponemos que las jugamos 5 turnos. Ej Headcrack (coste 3) es toYourHand3 (3x4/4).
+discover from your deck (no copy) es discover aunque falle. (BAR_545)
+discover cards de minions que no van a la mano sino que se invocan no son marcadas como discover, para que no aumente el deck weight.
+drawSyn: Somos restrictivos. Solo lo ponemos si cada vez que se roba hay un efecto claro, no la posibilidad de robar algo bueno.
    Shuffle into your deck no son drawSyn. Tiene que funcionar con todo tipo de cartas; minions, weapon o spells.
+token son 2 small minions (max 3/3), tambien cuentan las cartas generadas a mano (tokenCard), reborn y deathrattle no son token.
+tokenSyn: No son tokenSyn las cartas "Destroy friendly minion", solo cartas que necesiten 3+ minions.
+Corpse para todo token que no genere risen minions (no dejan corpse) y para reborn y deathrattle.
+CorpseSyn solo si gasta 2+ corpses.
+freezeEnemy deben poder usarse sobre enemigos, combo con destroy tardio (freezeEnemySyn).
+ping (NO RANDOM/NO DEATHRATTLE): tienen como proposito eliminar divineShield y rematar, deben ser proactivos, no random ni deathrattle.
+damageMinions y destroy (SI RANDOM/NO DEATHRATTLE): deben ser proactivos, permitimos que sean random pero no deathrattle ni secretos (random o no)
+aoe (SI RANDOM/SI DEATHRATTLE): puede ser deathrattle random (>= 2dmg), quitaremos manualmente excepciones como el tentaculo de n'zoth o unstable ghoul (1 dmg).
+aoe: los aoe tienen que afectar al menos 3 objetivos
+aoe: no son destroy ni damageMinions (ni siquiera token rush),
    a no ser que haga mucho dano a uno y poco a los demas, o que tenga 2 modos.
+aoe: Los bombing/missiles no son ping, si tienen los suficientes misiles/bombs (3) aoe pero nunca reach ya que no son fiables.
+spellDamageSyn es para aoe o damage a 2 objetivos
+Si una carta mejora cuando mayor sea nuestra mano no tengo en cuenta como sinergias otras cartas que hagan nuestra mano mayor,
    es una sinergia muy debil.
+No usamos los "=Gen(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)" ya que al no poder distinguir si se generan en el tablero, mano o mazo
    no se pueden asociar bien con los syn.
+Sinergias con 1/1s incluye lackeySyn y silverHandSyn. (jadeGolemSyn no existe)
+Summon a 5/5 copy of a minion in your deck, es una sinergia que no mostramos ya que el deck es muy grande y es dificil de acertar.
+Rush minion nunca los consideramos destroy por mucho ataque que tengan.
+RushGiver/RushGen: Los rushGiver solo son rushGen si automaticamente le dan rush al invocarlas.
+RushGiverSyn/Enrage/Frenzy: Solo son rushGiverSyn, los enrage minions de 5+ mana con un enrage significativo. Taunt 2/6 enrage +3 atk no lo es.
+ReturnSyn lo ponemos tambien battlecry neutros, como ambos jugadores roban 1 carta.
    No lo ponemos en battlecry condicionales a no ser que sean muy faciles de satisfacer.
+Sinergias con cartas de alto coste solo las ponemos para coste 6+ ("=>SynMinionCost6", "=>SynSpellCost6", "=>SynWeaponCost6")
+Egg/EggSyn lo usamos en huevos y criaturas que queremos sacrificar con aoes o destroy a friendly minion spells.
+evolveSyn: suele ponerse en minions que pierdan su valor en el battlecry o que tengan un mal deathrattle.
    Lo ponemos en minions que cuesten 2.5+ mana de lo que deberian por stats (3/3 es 3 mana, 3/4 es 3.5 mana)
    o 1.5+ si tienen reduccion de coste (nerubian prophet, thing from below) o son baratos (<5)
    o minions que suelen hacer rush sin morir y pierden atributos.
+drop234: Inicialmente se asignan solo por su coste.
    Un drop debe ser eficiente jugado en su turno suponiendo que el enemigo tenga en juego un minion del turno anterior y
        el jugador no tiene ningun minion.
    Un drop debe poner algo en la mesa, aunque sea un secreto. Si solo elimina cosas, roba cartas o buffa no es un drop.
    Si dan cristales de mana son drops.
    Si tiene buenos stats para jugarlo como drop, es un drop, aunque sea mejor jugarlo mas tarde (infuse, corrupt).
    Los minions con 0 de ataque no son drops, huevos tampoco, ya que su uso optimo es en otro turno.
    Los buffos no son drops, supon que tu board esta vacio.
    Los minions can't attack no son drops, a no ser que sea muy facil hacerlos atacar (hero power)
    Los drop ideales son minions con altos stats. Una bola de fuego no es un drop4, el 3/3 que hace 3 de dano al azar si es un drop4
        ya que lo que mata es un drop3 enemigo.
    No es un drop2 si preferimos crear un 1/1 con heropower a sacarlo.
    No poner un drop en un coste diferente de su mana a no ser que haya un razon de peso. El unico "Deadly Poison" y overload 1
        1+1 = drop2 / 2+1 = drop3 / 3+1 = drop4 / Todo lo demas es drop de su coste
        excepto (4+1 = no drop4) ya que necesitaremos otro drop4 en turno 5
    Stats minimos sin ningun extra en tempo, considerar que las condiciones con cartas especificas no se cumplen,
        como secretos, razas, hechizos, quizas holding.
        - Drop2 (Derrota 2/2 --> 3+/1+, 2/2+, 1/4+)
        - Drop3 (Derrota 3/3 --> 3+/2+, 2/4+, 1/7+), no health 1
        - Drop4 (Derrota 4/4 --> 5+/2+, 4/3+, 3/5+, 2/5+, no 1/x), no health 1
*/


//TODO
//Incluir tipo de carta (minion, weapon, spell, location) en LLM
//Revisar lifesteal para todos (minion, weapon, spell) y lifestealSyn/AllSyn para todos, sin son solo minion hacer compuesto &&
