#include "synergyhandler.h"
#include "themehandler.h"
#include "Synergies/keysynergies.h"
#include "Synergies/layeredsynergies.h"
#include "Synergies/cardtypecounter.h"
#include "Synergies/racecounter.h"
#include "Synergies/schoolcounter.h"
#include "Synergies/mechaniccounter.h"
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


void SynergyHandler::connectCounters(DraftDropCounter **dropCounters, DraftItemCounter **cardTypeCounters, DraftItemCounter **mechanicCounters)
{
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
}


void SynergyHandler::createDraftItemCounters()
{
    QGridLayout *mechanicsLayout = new QGridLayout();

    DraftItemCounter **cardTypeCounters = CardTypeCounter::createCardTypeCounters(this, mechanicsLayout);
    manaCounter = new DraftItemCounter(this, "Mana AVG", "Mana AVG", mechanicsLayout, 0, 3,
                                       QPixmap(ThemeHandler::manaCounterFile()), 32, false, false);

    dropCounters = new DraftDropCounter *[V_NUM_DROPS];//TODO drops
    dropCounters[V_DROP2] = new DraftDropCounter(this, "2 Drop", "2 Cost", mechanicsLayout, 1, 0, TARGET_DROP_2,
                                                 QPixmap(ThemeHandler::drop2CounterFile()), 32, true, false);
    dropCounters[V_DROP3] = new DraftDropCounter(this, "3 Drop", "3 Cost", mechanicsLayout, 1, 1, TARGET_DROP_3,
                                                 QPixmap(ThemeHandler::drop3CounterFile()), 32, true, false);
    dropCounters[V_DROP4] = new DraftDropCounter(this, "4 Drop", "4 Cost", mechanicsLayout, 1, 2, TARGET_DROP_4,
                                                 QPixmap(ThemeHandler::drop4CounterFile()), 32, true, false);

    DraftItemCounter **mechanicCounters = MechanicCounter::createMechanicCounters(this, mechanicsLayout);
    RaceCounter::createRaceCounters(this);
    SchoolCounter::createSchoolCounters(this);
    KeySynergies::createKeySynergies();

    connectCounters(dropCounters, cardTypeCounters, mechanicCounters);

    CardTypeCounter::setSynergyCodes(&synergyCodes);
    RaceCounter::setSynergyCodes(&synergyCodes);
    SchoolCounter::setSynergyCodes(&synergyCodes);
    MechanicCounter::setSynergyCodes(&synergyCodes);
    KeySynergies::setSynergyCodes(&synergyCodes);
    LayeredSynergies::setSynergyCodes(&synergyCodes);

    QHBoxLayout *horLayoutMechanics = new QHBoxLayout();
    horLayoutMechanics->addLayout(mechanicsLayout);
    horLayoutMechanics->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutMechanics);
}


void SynergyHandler::deleteDraftItemCounters()
{
    delete manaCounter;
    CardTypeCounter::deleteCardTypeCounters();
    RaceCounter::deleteRaceCounters();
    SchoolCounter::deleteSchoolCounters();
    MechanicCounter::deleteMechanicCounters();

    for(int i=0; i<V_NUM_DROPS; i++)
    {
        delete dropCounters[i];
    }
    delete []dropCounters;
}


void SynergyHandler::setTheme()
{
    CardTypeCounter::setTheme();
    MechanicCounter::setTheme();
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()), 32, false);

    dropCounters[V_DROP2]->setTheme(QPixmap(ThemeHandler::drop2CounterFile()), 32, false);
    dropCounters[V_DROP3]->setTheme(QPixmap(ThemeHandler::drop3CounterFile()), 32, false);
    dropCounters[V_DROP4]->setTheme(QPixmap(ThemeHandler::drop4CounterFile()), 32, false);
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


bool SynergyHandler::initSynergyCodes(const QStringList &arenaCodes, bool all)
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
    CardTypeCounter::resetAll();
    RaceCounter::resetAll();
    SchoolCounter::resetAll();
    MechanicCounter::resetAll();
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->reset();
    }
    KeySynergies::resetAll();
    LayeredSynergies::reset();

    //Reset stats maps
    costMinions.clear();
    attackMinions.clear();
    healthMinions.clear();
    costSpells.clear();
    costWeapons.clear();
    attackWeapons.clear();
    healthWeapons.clear();
}


void SynergyHandler::setHidden(bool hide)
{
    CardTypeCounter::setHidden(hide);
    MechanicCounter::setHidden(hide);
    if(hide)
    {
        manaCounter->hide();

        dropCounters[V_DROP2]->hide();
        dropCounters[V_DROP3]->hide();
        dropCounters[V_DROP4]->hide();
    }
    else
    {
        manaCounter->show();

        dropCounters[V_DROP2]->show();
        dropCounters[V_DROP3]->show();
        dropCounters[V_DROP4]->show();
    }
}


void SynergyHandler::setTransparency(Transparency transparency, bool mouseInApp)
{
    manaCounter->setTransparency(transparency, mouseInApp);
    CardTypeCounter::setTransparency(transparency, mouseInApp);
    MechanicCounter::setTransparency(transparency, mouseInApp);
    for(int i=0; i<V_NUM_DROPS; i++)
    {
        dropCounters[i]->setTransparency(transparency, mouseInApp);
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
    CardTypeCounter::getCardTypeCounters(spellMap, minionMap, weaponMap);
    MechanicCounter::getMechanicCounters(aoeMap, tauntMap, survivabilityMap, drawMap,
                                         pingMap, damageMap, destroyMap, reachMap,
                                         draw, toYourHand, discover);

    drop2Map = dropCounters[V_DROP2]->getCodeTagMap();
    drop3Map = dropCounters[V_DROP3]->getCodeTagMap();
    drop4Map = dropCounters[V_DROP4]->getCodeTagMap();

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
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();

    updateDropCounters(deckCard, drop2Map, drop3Map, drop4Map);
    updateManaCounter(deckCard);
    CardTypeCounter::updateCardTypeCounters(deckCard, spellMap, minionMap, weaponMap);
    RaceCounter::updateRaceCounters(deckCard);
    SchoolCounter::updateSchoolCounters(deckCard);
    MechanicCounter::updateMechanicCounters(deckCard, aoeMap, tauntMap, survivabilityMap, drawMap,
                           pingMap, damageMap, destroyMap, reachMap,
                           draw, toYourHand, discover);
    KeySynergies::updateKeySynergies(code, mechanics, referencedTags, text, cardType, attack, cost);
    updateStatsCards(deckCard);
    LayeredSynergies::updateLayeredSynergies(code);//TODO arguments
}


void SynergyHandler::updateManaCounter(DeckCard &deckCard)
{
    manaCounter->increase(getCorrectedCardMana(deckCard), CardTypeCounter::draftedCardsCount());//TODO deckCard
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
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();
    int cost = deckCard.getCost();
    QList<CardRace> cardRace = deckCard.getRace();
    CardSchool cardSchool = deckCard.getSchool();


    getDropMechanicIcons(code, mechanicIcons, dropBorderColor, attack, health, cost);
    CardTypeCounter::getCardTypeSynergies(code, synergyTagMap, text, cardType);
    RaceCounter::getRaceSynergies(code, synergyTagMap, mechanics, text, cardRace);
    SchoolCounter::getSchoolSynergies(code, synergyTagMap, text, cardSchool);
    MechanicCounter::getMechanicSynergies(code, synergyTagMap, mechanicIcons, mechanics, referencedTags, text, cardType, attack, cost);
    KeySynergies::getKeySynergies(code, synergyTagMap, mechanics, referencedTags, text, cardType, attack, cost);
    getStatsCardsSynergies(code, synergyTagMap, cardType, attack, health, cost);
    LayeredSynergies::getLayeredSynergies(code, synergyTagMap, mechanics, referencedTags, text, cardType, attack, cost);
    CardTypeCounter::getDirectLinkSynergies(code, directLinks, synergyTagMap["Extra"]);
}


void SynergyHandler::getDropMechanicIcons(const QString &code, QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor,
                                          int attack, int health, int cost)
{
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


void SynergyHandler::getStatsCardsSynergies(const QString &code, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                            CardType cardType, int attack, int health, int cost)
{
    if(cardType == MINION)
    {
        costMinions.insertCards(true, cost, synergyTagMap["Cost"]);
        attackMinions.insertCards(true, attack, synergyTagMap["Attack"]);
        healthMinions.insertCards(true, health, synergyTagMap["Health"]);
    }
    else if(cardType == SPELL)
    {
        costSpells.insertCards(true, cost, synergyTagMap["Cost"]);
    }
    else if(cardType == WEAPON)
    {
        costWeapons.insertCards(true, cost, synergyTagMap["Cost"]);
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
    if(mechanic.startsWith('&'))
    {
        const QStringList layeredSynergy = mechanic.split('&', Qt::SkipEmptyParts);
        bool allValid = true;
        for(const auto &partSynergy: layeredSynergy)
        {
            if(!isValidSynergyCode(partSynergy, match))
            {
                allValid = false;
                break;
            }
        }
        return allValid;
    }
    QStringList validMecs = {
        "spellGen", "minionGen", "weaponGen", "locationGen",
        "spellSyn", "minionSyn", "weaponSyn", "locationSyn",
        "spellAllSyn", "minionAllSyn", "weaponAllSyn", "locationAllSyn",

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
    };//TODO validMecs separar

    const auto keys = KeySynergies::getListKeySynergies();
    for(const QString &keyS: keys)
    {
        validMecs << keyS << keyS+"Gen" << keyS+"Syn" << keyS+"AllSyn";
    }

    return validMecs.contains(mechanic);
}


void SynergyHandler::testSynergies(const QString &miniSet)
{
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, true);
    int num = 0;

    for(QString &code: arenaCodes)
    // for(QString &code: (QStringList)Utility::getSetCodes("SPACE", true, false))
    // for(QString &code: (QStringList)Utility::getSetCodesSpecific("TREASURES"))
    // for(QString &code: (QStringList)Utility::getStandardCodes())
    // for(QString &code: (QStringList)Utility::getWildCodes())
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
               // cost == 1
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
                // if(num>50 && num<=100)
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
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, true);

    QStringList codeList;
    if(Utility::needCodesSpecific(set)) codeList.append(Utility::getSetCodesSpecific(set));
    else                                codeList.append(Utility::getSetCodes(set, true, onlyCollectible));

    qDebug()<<"\n-----SynergiesNames.json-----\n";
    for(const QString &code: codeList)
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            qDebug()<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
        }
    }

    qDebug()<<"\n-----Synergies.json-----\n";
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
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, true);

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


    if(isDrop2(code, cost, attack, health)) mec<<"drop2";
    if(isDrop3(code, cost, attack, health)) mec<<"drop3";
    if(isDrop4(code, cost, attack, health)) mec<<"drop4";

    mec << CardTypeCounter::debugCardTypeSynergies(code, text);
    mec << RaceCounter::debugRaceSynergies(code, mechanics, text);
    mec << SchoolCounter::debugSchoolSynergies(code, text);
    mec << MechanicCounter::debugMechanicSynergies(code, mechanics, referencedTags, text, cardType, attack, cost);
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
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, true);

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
        if(cost == 1)
        // if(cost == 2)
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
                // if(num>50 && num<=100)
                // if(num>100 && num<=150)
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
        HUNTERS_INSIGHT, SANDBOX_SCOUNDREL, ENSMALLEN, AVIANA_ELUNES_CHOSEN, REANIMATED_PTERRORDAX, ENTOMOLOGIST_TORU, BLASTEROID
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
