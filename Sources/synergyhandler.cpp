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
    horLayoutCardTypes = new QHBoxLayout();
    horLayoutMechanics1 = new QHBoxLayout();
    horLayoutMechanics2 = new QHBoxLayout();


    cardTypeCounters = new DraftItemCounter *[V_NUM_TYPES];
    cardTypeCounters[V_MINION] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(ThemeHandler::minionsCounterFile()), false);
    cardTypeCounters[V_SPELL] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(ThemeHandler::spellsCounterFile()), false);
    cardTypeCounters[V_SPELL_ALL] = new DraftItemCounter(this);
    cardTypeCounters[V_WEAPON] = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(ThemeHandler::weaponsCounterFile()), false);
    cardTypeCounters[V_WEAPON_ALL] = new DraftItemCounter(this);


    manaCounter = new DraftItemCounter(this, horLayoutCardTypes, QPixmap(ThemeHandler::manaCounterFile()), false);

    raceCounters = new DraftItemCounter *[V_NUM_RACES];
    raceCounters[V_ELEMENTAL] = new DraftItemCounter(this);
    raceCounters[V_BEAST] = new DraftItemCounter(this);
    raceCounters[V_MURLOC] = new DraftItemCounter(this);
    raceCounters[V_DRAGON] = new DraftItemCounter(this);
    raceCounters[V_PIRATE] = new DraftItemCounter(this);
    raceCounters[V_MECHANICAL] = new DraftItemCounter(this);
    raceCounters[V_DEMON] = new DraftItemCounter(this);
    raceCounters[V_TOTEM] = new DraftItemCounter(this);

    raceCounters[V_ELEMENTAL_ALL] = new DraftItemCounter(this);
    raceCounters[V_BEAST_ALL] = new DraftItemCounter(this);
    raceCounters[V_MURLOC_ALL] = new DraftItemCounter(this);
    raceCounters[V_DRAGON_ALL] = new DraftItemCounter(this);
    raceCounters[V_PIRATE_ALL] = new DraftItemCounter(this);
    raceCounters[V_MECHANICAL_ALL] = new DraftItemCounter(this);
    raceCounters[V_DEMON_ALL] = new DraftItemCounter(this);
    raceCounters[V_TOTEM_ALL] = new DraftItemCounter(this);

    mechanicCounters = new DraftItemCounter *[V_NUM_MECHANICS];
    mechanicCounters[V_AOE] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(ThemeHandler::aoeMechanicFile()));
    mechanicCounters[V_TAUNT_ALL] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(ThemeHandler::tauntMechanicFile()));
    mechanicCounters[V_SURVIVABILITY] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(ThemeHandler::survivalMechanicFile()));
    mechanicCounters[V_DISCOVER_DRAW] = new DraftItemCounter(this, horLayoutMechanics1, QPixmap(ThemeHandler::drawMechanicFile()));

    mechanicCounters[V_PING] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(ThemeHandler::pingMechanicFile()));
    mechanicCounters[V_DAMAGE] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(ThemeHandler::damageMechanicFile()));
    mechanicCounters[V_DESTROY] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(ThemeHandler::destroyMechanicFile()));
    mechanicCounters[V_REACH] = new DraftItemCounter(this, horLayoutMechanics2, QPixmap(ThemeHandler::reachMechanicFile()));

    connect(mechanicCounters[V_AOE], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_TAUNT_ALL], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_SURVIVABILITY], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_DISCOVER_DRAW], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_PING], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_DAMAGE], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_DESTROY], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));
    connect(mechanicCounters[V_REACH], SIGNAL(iconEnter(QList<DeckCard>&,QRect &)),
            this, SLOT(sendItemEnter(QList<DeckCard>&,QRect &)));

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

    mechanicCounters[V_DISCOVER] = new DraftItemCounter(this);
    mechanicCounters[V_DRAW] = new DraftItemCounter(this);
    mechanicCounters[V_TOYOURHAND] = new DraftItemCounter(this);
    mechanicCounters[V_TAUNT] = new DraftItemCounter(this);
    mechanicCounters[V_OVERLOAD] = new DraftItemCounter(this);
    mechanicCounters[V_JADE_GOLEM] = new DraftItemCounter(this);
    mechanicCounters[V_HERO_POWER] = new DraftItemCounter(this);
    mechanicCounters[V_SECRET] = new DraftItemCounter(this);
    mechanicCounters[V_SECRET_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_FREEZE_ENEMY] = new DraftItemCounter(this);
    mechanicCounters[V_DISCARD] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE] = new DraftItemCounter(this);
    mechanicCounters[V_DEATHRATTLE_GOOD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_BATTLECRY] = new DraftItemCounter(this);
    mechanicCounters[V_SILENCE] = new DraftItemCounter(this);
    mechanicCounters[V_TAUNT_GIVER] = new DraftItemCounter(this);
    mechanicCounters[V_TOKEN] = new DraftItemCounter(this);
    mechanicCounters[V_TOKEN_CARD] = new DraftItemCounter(this);
    mechanicCounters[V_COMBO] = new DraftItemCounter(this);
    mechanicCounters[V_WINDFURY_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_ATTACK_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_HEALTH_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_RETURN] = new DraftItemCounter(this);
    mechanicCounters[V_STEALTH] = new DraftItemCounter(this);
    mechanicCounters[V_SPELL_DAMAGE] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD] = new DraftItemCounter(this);
    mechanicCounters[V_DIVINE_SHIELD_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_ENRAGED] = new DraftItemCounter(this);
    mechanicCounters[V_RESTORE_FRIENDLY_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_RESTORE_TARGET_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_RESTORE_FRIENDLY_HEROE] = new DraftItemCounter(this);
    mechanicCounters[V_ARMOR] = new DraftItemCounter(this);
    mechanicCounters[V_EVOLVE] = new DraftItemCounter(this);
    mechanicCounters[V_SPAWN_ENEMY] = new DraftItemCounter(this);
    mechanicCounters[V_LIFESTEAL_MINION] = new DraftItemCounter(this);
    mechanicCounters[V_EGG] = new DraftItemCounter(this);
    mechanicCounters[V_DAMAGE_FRIENDLY_HERO] = new DraftItemCounter(this);
    mechanicCounters[V_RUSH] = new DraftItemCounter(this);
    mechanicCounters[V_RUSH_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_ECHO] = new DraftItemCounter(this);
    mechanicCounters[V_ECHO_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_MAGNETIC] = new DraftItemCounter(this);
    mechanicCounters[V_MAGNETIC_ALL] = new DraftItemCounter(this);
    mechanicCounters[V_HAND_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_ENEMY_DRAW] = new DraftItemCounter(this);
    mechanicCounters[V_HERO_ATTACK] = new DraftItemCounter(this);
    mechanicCounters[V_SPELL_BUFF] = new DraftItemCounter(this);
    mechanicCounters[V_OTHER_CLASS] = new DraftItemCounter(this);
    mechanicCounters[V_SILVER_HAND] = new DraftItemCounter(this);
    mechanicCounters[V_TREANT] = new DraftItemCounter(this);
    //New Synergy Step 2


    horLayoutCardTypes->addStretch();
    horLayoutMechanics1->addStretch();
    horLayoutMechanics2->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutCardTypes);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics1);
    ui->draftVerticalLayout->addLayout(horLayoutMechanics2);
}


void SynergyHandler::deleteDraftItemCounters()
{
    delete manaCounter;
    for(int i=0; i<V_NUM_TYPES; i++)
    {
        delete cardTypeCounters[i];
    }
    delete []cardTypeCounters;

    for(int i=0; i<V_NUM_RACES; i++)
    {
        delete raceCounters[i];
    }
    delete []raceCounters;

    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        delete mechanicCounters[i];
    }
    delete []mechanicCounters;
}


void SynergyHandler::setTheme()
{
    cardTypeCounters[V_MINION]->setTheme(QPixmap(ThemeHandler::minionsCounterFile()));
    cardTypeCounters[V_SPELL]->setTheme(QPixmap(ThemeHandler::spellsCounterFile()));
    cardTypeCounters[V_WEAPON]->setTheme(QPixmap(ThemeHandler::weaponsCounterFile()));
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()));

    mechanicCounters[V_AOE]->setTheme(QPixmap(ThemeHandler::aoeMechanicFile()));
    mechanicCounters[V_TAUNT_ALL]->setTheme(QPixmap(ThemeHandler::tauntMechanicFile()));
    mechanicCounters[V_SURVIVABILITY]->setTheme(QPixmap(ThemeHandler::survivalMechanicFile()));
    mechanicCounters[V_DISCOVER_DRAW]->setTheme(QPixmap(ThemeHandler::drawMechanicFile()));

    mechanicCounters[V_PING]->setTheme(QPixmap(ThemeHandler::pingMechanicFile()));
    mechanicCounters[V_DAMAGE]->setTheme(QPixmap(ThemeHandler::damageMechanicFile()));
    mechanicCounters[V_DESTROY]->setTheme(QPixmap(ThemeHandler::destroyMechanicFile()));
    mechanicCounters[V_REACH]->setTheme(QPixmap(ThemeHandler::reachMechanicFile()));
}



void SynergyHandler::sendItemEnter(QList<DeckCard> &deckCardList, QRect &labelRect)
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

    emit itemEnter(deckCardList, rect, maxTop, maxBottom);
}


void SynergyHandler::initSynergyCodes()
{
    synergyCodes.clear();
    directLinks.clear();

    QFile jsonFile(Utility::extraPath() + "/synergies.json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();
    QJsonObject jsonObject = jsonDoc.object();

    for(const QString &code: jsonObject.keys())
    {
        if(jsonObject.value(code).isArray())
        {
            synergyCodes[code];
            QJsonArray synergies = jsonObject.value(code).toArray();
            for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
            {
                synergyCodes[code].append(it->toString());
            }
        }
    }
    emit pDebug("Synergy Cards: " + QString::number(synergyCodes.count()));


    //Direct links
    QJsonObject dlObject = jsonObject.value("DIRECT_LINKS").toObject();
    for(const QString &code: dlObject.keys())
    {
        if(dlObject.value(code).isArray())
        {
            QJsonArray synergies = dlObject.value(code).toArray();
            for(QJsonArray::const_iterator it=synergies.constBegin(); it!=synergies.constEnd(); it++)
            {
                directLinks[code].append(it->toString());
                directLinks[it->toString()].append(code);
            }
        }
    }
    emit pDebug("Direct Link Cards: " + QString::number(directLinks.count()));
}


void SynergyHandler::clearLists(bool keepCounters)
{
    synergyCodes.clear();
    directLinks.clear();

    if(!keepCounters)
    {
        //Reset counters
        manaCounter->reset();
        for(int i=0; i<V_NUM_TYPES; i++)
        {
            cardTypeCounters[i]->reset();
        }
        for(int i=0; i<V_NUM_RACES; i++)
        {
            raceCounters[i]->reset();
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
    }
}


void SynergyHandler::setHidden(bool hide)
{
    if(hide)
    {
        cardTypeCounters[V_MINION]->hide();
        cardTypeCounters[V_SPELL]->hide();
        cardTypeCounters[V_WEAPON]->hide();
        manaCounter->hide();

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
    for(int i=0; i<V_NUM_RACES; i++)
    {
        raceCounters[i]->setTransparency(transparency, mouseInApp);
    }
    for(int i=0; i<V_NUM_MECHANICS; i++)
    {
        mechanicCounters[i]->setTransparency(transparency, mouseInApp);
    }
}


int SynergyHandler::getCounters(QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                                QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList,
                                int &draw, int &toYourHand, int &discover)
{
    for(DeckCard &deckCard: cardTypeCounters[V_SPELL]->getDeckCardList())
    {
        QString code = deckCard.getCode();
        if(DeckCard(code).getType() == SPELL)
        {
            for(int i=0; i<deckCard.total; i++)    spellList.append(code);
        }
    }
    for(DeckCard &deckCard: cardTypeCounters[V_MINION]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    minionList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: cardTypeCounters[V_WEAPON]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    weaponList.append(deckCard.getCode());
    }


    for(DeckCard &deckCard: mechanicCounters[V_AOE]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    aoeList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_TAUNT_ALL]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    tauntList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_SURVIVABILITY]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    survivabilityList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_DISCOVER_DRAW]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    drawList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_PING]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    pingList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_DAMAGE]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    damageList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_DESTROY]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    destroyList.append(deckCard.getCode());
    }
    for(DeckCard &deckCard: mechanicCounters[V_REACH]->getDeckCardList())
    {
        for(int i=0; i<deckCard.total; i++)    reachList.append(deckCard.getCode());
    }

    discover = draw = toYourHand = 0;
    for(DeckCard &deckCard: mechanicCounters[V_DISCOVER]->getDeckCardList())
    {
        QString code = deckCard.getCode();
        QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
        QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
        discover += deckCard.total * numDiscoverGen(code, mechanics, referencedTags);
    }
    for(DeckCard &deckCard: mechanicCounters[V_DRAW]->getDeckCardList())
    {
        QString code = deckCard.getCode();
        QString text = Utility::cardEnTextFromCode(code).toLower();
        draw += deckCard.total * numDrawGen(code, text);
    }
    for(DeckCard &deckCard: mechanicCounters[V_TOYOURHAND]->getDeckCardList())
    {
        QString code = deckCard.getCode();
        QString text = Utility::cardEnTextFromCode(code).toLower();
        toYourHand += deckCard.total * numToYourHandGen(code, text);
    }

    return manaCounter->count();
}


int SynergyHandler::getManaCounterCount()
{
    return manaCounter->count();
}


void SynergyHandler::updateCounters(DeckCard &deckCard, QStringList &spellList, QStringList &minionList, QStringList &weaponList,
                                    QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                    QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList,
                                    int &draw, int &toYourHand, int &discover)
{
    updateRaceCounters(deckCard);
    updateCardTypeCounters(deckCard, spellList, minionList, weaponList);
    updateManaCounter(deckCard);
    updateMechanicCounters(deckCard, aoeList, tauntList, survivabilityList, drawList, pingList, damageList, destroyList, reachList,
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
    CardRace cardRace = deckCard.getRace();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();

    if(cardRace == MURLOC)
    {
        raceCounters[V_MURLOC]->increase(code);
        raceCounters[V_MURLOC_ALL]->increase(code);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->increase(code);
    if(cardRace == DEMON)
    {
        raceCounters[V_DEMON]->increase(code);
        raceCounters[V_DEMON_ALL]->increase(code);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->increase(code);
    if(cardRace == MECHANICAL)
    {
        raceCounters[V_MECHANICAL]->increase(code);
        raceCounters[V_MECHANICAL_ALL]->increase(code);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->increase(code);
    if(cardRace == ELEMENTAL)
    {
        raceCounters[V_ELEMENTAL]->increase(code);
        raceCounters[V_ELEMENTAL_ALL]->increase(code);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->increase(code);
    if(cardRace == BEAST)
    {
        raceCounters[V_BEAST]->increase(code);
        raceCounters[V_BEAST_ALL]->increase(code);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->increase(code);
    if(cardRace == TOTEM)
    {
        raceCounters[V_TOTEM]->increase(code);
        raceCounters[V_TOTEM_ALL]->increase(code);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->increase(code);
    if(cardRace == PIRATE)
    {
        raceCounters[V_PIRATE]->increase(code);
        raceCounters[V_PIRATE_ALL]->increase(code);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->increase(code);
    if(cardRace == DRAGON)
    {
        raceCounters[V_DRAGON]->increase(code);
        raceCounters[V_DRAGON_ALL]->increase(code);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->increase(code);

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
    if(isDragonSyn(code, text))                     raceCounters[V_DRAGON]->increaseSyn(code);
    else if(isDragonAllSyn(code))                   raceCounters[V_DRAGON_ALL]->increaseSyn(code);
}


void SynergyHandler::updateCardTypeCounters(DeckCard &deckCard, QStringList &spellList, QStringList &minionList, QStringList &weaponList)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    if(cardType == SPELL)
    {
        cardTypeCounters[V_SPELL]->increase(code);
        cardTypeCounters[V_SPELL_ALL]->increase(code);
        spellList.append(code);
    }
    else if(isSpellGen(code))           cardTypeCounters[V_SPELL_ALL]->increase(code);
    if(cardType == MINION || cardType == HERO)
    {
        cardTypeCounters[V_MINION]->increase(code);
        minionList.append(code);
    }
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->increase(code);
        cardTypeCounters[V_WEAPON_ALL]->increase(code);
        weaponList.append(code);
    }
    else if(isWeaponGen(code, text))    cardTypeCounters[V_WEAPON_ALL]->increase(code);


    if(isSpellSyn(code))                cardTypeCounters[V_SPELL]->increaseSyn(code);
    else if(isSpellAllSyn(code, text))  cardTypeCounters[V_SPELL_ALL]->increaseSyn(code);
    if(isWeaponSyn(code))               cardTypeCounters[V_WEAPON]->increaseSyn(code);
    else if(isWeaponAllSyn(code, text)) cardTypeCounters[V_WEAPON_ALL]->increaseSyn(code);
}


void SynergyHandler::updateMechanicCounters(DeckCard &deckCard,
                                            QStringList &aoeList, QStringList &tauntList, QStringList &survivabilityList, QStringList &drawList,
                                            QStringList &pingList, QStringList &damageList, QStringList &destroyList, QStringList &reachList,
                                            int &draw, int &toYourHand, int &discover)
{
    bool isSurvivability = false;
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    CardClass cardClass = deckCard.getCardClass();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
//    int cost = deckCard.getCost();

    //GEN
    if(isDiscoverDrawGen(code, mechanics, referencedTags, text))
    {
        mechanicCounters[V_DISCOVER_DRAW]->increase(code);
        drawList.append(code);
    }
    if(isAoeGen(code, text))
    {
        mechanicCounters[V_AOE]->increase(code);
        aoeList.append(code);
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->increase(code);
        pingList.append(code);
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_DAMAGE]->increase(code);
        damageList.append(code);
    }
    if(isDestroyGen(code, mechanics, text))
    {
        mechanicCounters[V_DESTROY]->increase(code);
        destroyList.append(code);
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_REACH]->increase(code);
        reachList.append(code);
    }
    if(isRestoreFriendlyHeroGen(code, mechanics, text))
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
        survivabilityList.append(code);
    }
    discover = numDiscoverGen(code, mechanics, referencedTags);
    draw = numDrawGen(code, text);
    toYourHand = numToYourHandGen(code, text);
    if(discover > 0)                                                        mechanicCounters[V_DISCOVER]->increase(code);
    if(draw > 0)                                                            mechanicCounters[V_DRAW]->increase(code);
    if(toYourHand > 0)                                                      mechanicCounters[V_TOYOURHAND]->increase(code);
    if(isOverload(code))                                                    mechanicCounters[V_OVERLOAD]->increase(code);
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mechanicCounters[V_JADE_GOLEM]->increase(code);
    if(isHeroPowerGen(code, text, cardClass))                               mechanicCounters[V_HERO_POWER]->increase(code);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mechanicCounters[V_FREEZE_ENEMY]->increase(code);
    if(isDiscardGen(code, text))                                            mechanicCounters[V_DISCARD]->increase(code);
    if(isDeathrattleMinion(code, mechanics, cardType))                      mechanicCounters[V_DEATHRATTLE]->increase(code);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increase(code);
    if(isBattlecry(code, mechanics))                                        mechanicCounters[V_BATTLECRY]->increase(code);
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mechanicCounters[V_SILENCE]->increase(code);
    if(isTauntGiverGen(code))                                               mechanicCounters[V_TAUNT_GIVER]->increase(code);
    if(isTokenGen(code, text))                                              mechanicCounters[V_TOKEN]->increase(code);
    //TokenCard es synergia debil
    //Evitamos que aparezcan token cards synergies en cada combo card
//    if(isTokenCardGen(code, cost))                                          mechanicCounters[V_TOKEN_CARD]->increase(code);
    if(isComboGen(code, mechanics))                                         mechanicCounters[V_COMBO]->increase(code);
    if(isWindfuryMinion(code, mechanics, cardType))                         mechanicCounters[V_WINDFURY_MINION]->increase(code);
    if(isAttackBuffGen(code, text))                                         mechanicCounters[V_ATTACK_BUFF]->increase(code);
    if(isHealthBuffGen(code, text))                                         mechanicCounters[V_HEALTH_BUFF]->increase(code);
    if(isReturnGen(code, text))                                             mechanicCounters[V_RETURN]->increase(code);
    if(isStealthGen(code, mechanics))                                       mechanicCounters[V_STEALTH]->increase(code);
    if(isSpellDamageGen(code))                                              mechanicCounters[V_SPELL_DAMAGE]->increase(code);
    if(isEvolveGen(code, text))                                             mechanicCounters[V_EVOLVE]->increase(code);
    if(isSpawnEnemyGen(code, text))                                         mechanicCounters[V_SPAWN_ENEMY]->increase(code);
    if(isRestoreTargetMinionGen(code, text))                                mechanicCounters[V_RESTORE_TARGET_MINION]->increase(code);
    if(isRestoreFriendlyMinionGen(code, text))                              mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increase(code);
    if(isLifestealMinon(code, mechanics, cardType))                         mechanicCounters[V_LIFESTEAL_MINION]->increase(code);
    if(isEnrageGen(code, mechanics))                                        mechanicCounters[V_ENRAGED]->increase(code);
    if(isEggGen(code, mechanics, attack, cardType))                         mechanicCounters[V_EGG]->increase(code);
    if(isDamageFriendlyHeroGen(code))                                       mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->increase(code);
    if(isHandBuffGen(code, text))                                           mechanicCounters[V_HAND_BUFF]->increase(code);
    if(isEnemyDrawGen(code, text))                                          mechanicCounters[V_ENEMY_DRAW]->increase(code);
    if(isHeroAttackGen(code, text))                                         mechanicCounters[V_HERO_ATTACK]->increase(code);
    if(isSpellBuffGen(code, text, mechanics, cardType))                     mechanicCounters[V_SPELL_BUFF]->increase(code);
    if(isOtherClassGen(code, text, cardClass))                              mechanicCounters[V_OTHER_CLASS]->increase(code);
    if(isSilverHandGen(code, text, cardClass))                              mechanicCounters[V_SILVER_HAND]->increase(code);
    if(isTreantGen(code, text, cardClass))                                  mechanicCounters[V_TREANT]->increase(code);
    //New Synergy Step 3
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->increase(code);
        mechanicCounters[V_TAUNT_ALL]->increase(code);
        tauntList.append(code);
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicCounters[V_TAUNT_ALL]->increase(code);
        tauntList.append(code);
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
    if(isEcho(code, text))
    {
        mechanicCounters[V_ECHO]->increase(code);
        mechanicCounters[V_ECHO_ALL]->increase(code);
    }
    else if(isEchoGen(code))                                                mechanicCounters[V_ECHO_ALL]->increase(code);
    if(isRush(code, text))
    {
        mechanicCounters[V_RUSH]->increase(code);
        mechanicCounters[V_RUSH_ALL]->increase(code);
    }
    else if(isRushGen(code))                                                mechanicCounters[V_RUSH_ALL]->increase(code);
    if(isMagnetic(code, mechanics))
    {
        mechanicCounters[V_MAGNETIC]->increase(code);
        mechanicCounters[V_MAGNETIC_ALL]->increase(code);
    }
    else if(isMagneticGen(code))                                            mechanicCounters[V_MAGNETIC_ALL]->increase(code);



    //SYN
    if(isAoeSyn(code))                                                      mechanicCounters[V_AOE]->increaseSyn(code);
    if(isPingSyn(code))                                                     mechanicCounters[V_PING]->increaseSyn(code);
    if(isDiscoverSyn(code))                                                 mechanicCounters[V_DISCOVER]->increaseSyn(code);
    if(isDrawSyn(code))                                                     mechanicCounters[V_DRAW]->increaseSyn(code);
    if(isToYourHandSyn(code))                                               mechanicCounters[V_TOYOURHAND]->increaseSyn(code);
    if(isOverloadSyn(code, text))                                           mechanicCounters[V_OVERLOAD]->increaseSyn(code);
    if(isFreezeEnemySyn(code, referencedTags, text))                        mechanicCounters[V_FREEZE_ENEMY]->increaseSyn(code);
    if(isDiscardSyn(code, text))                                            mechanicCounters[V_DISCARD]->increaseSyn(code);
    if(isBattlecrySyn(code, referencedTags))                                mechanicCounters[V_BATTLECRY]->increaseSyn(code);
    if(isSilenceOwnSyn(code, mechanics))                                    mechanicCounters[V_SILENCE]->increaseSyn(code);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_TAUNT_GIVER]->increaseSyn(code);
    if(isTokenSyn(code, text))                                              mechanicCounters[V_TOKEN]->increaseSyn(code);
    if(isTokenCardSyn(code, text))                                          mechanicCounters[V_TOKEN_CARD]->increaseSyn(code);
    if(isComboSyn(code, referencedTags))                                    mechanicCounters[V_COMBO]->increaseSyn(code);
    if(isWindfuryMinionSyn(code))                                           mechanicCounters[V_WINDFURY_MINION]->increaseSyn(code);
    if(isAttackBuffSyn(code, mechanics, attack, cardType))                  mechanicCounters[V_ATTACK_BUFF]->increaseSyn(code);
    if(isHealthBuffSyn(code))                                               mechanicCounters[V_HEALTH_BUFF]->increaseSyn(code);
    if(isReturnSyn(code, mechanics, cardType, text))                        mechanicCounters[V_RETURN]->increaseSyn(code);
    if(isStealthSyn(code))                                                  mechanicCounters[V_STEALTH]->increaseSyn(code);
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   mechanicCounters[V_SPELL_DAMAGE]->increaseSyn(code);
    if(isEvolveSyn(code))                                                   mechanicCounters[V_EVOLVE]->increaseSyn(code);
    if(isSpawnEnemySyn(code))                                               mechanicCounters[V_SPAWN_ENEMY]->increaseSyn(code);
    if(isRestoreTargetMinionSyn(code))                                      mechanicCounters[V_RESTORE_TARGET_MINION]->increaseSyn(code);
    if(isRestoreFriendlyHeroSyn(code))                                      mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->increaseSyn(code);
    if(isRestoreFriendlyMinionSyn(code))                                    mechanicCounters[V_RESTORE_FRIENDLY_MINION]->increaseSyn(code);
    if(isLifestealMinionSyn(code))                                          mechanicCounters[V_LIFESTEAL_MINION]->increaseSyn(code);
    if(isArmorSyn(code))                                                    mechanicCounters[V_ARMOR]->increaseSyn(code);
    if(isEnrageSyn(code, text))                                             mechanicCounters[V_ENRAGED]->increaseSyn(code);
    if(isEggSyn(code, text))                                                mechanicCounters[V_EGG]->increaseSyn(code);
    if(isDamageFriendlyHeroSyn(code))                                       mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->increaseSyn(code);
    if(isHandBuffSyn(code, text))                                           mechanicCounters[V_HAND_BUFF]->increaseSyn(code);
    if(isEnemyDrawSyn(code, text))                                          mechanicCounters[V_ENEMY_DRAW]->increaseSyn(code);
    if(isHeroAttackSyn(code))                                               mechanicCounters[V_HERO_ATTACK]->increaseSyn(code);
    if(isSpellBuffSyn(code, text))                                          mechanicCounters[V_SPELL_BUFF]->increaseSyn(code);
    if(isOtherClassSyn(code, text, cardClass))                              mechanicCounters[V_OTHER_CLASS]->increaseSyn(code);
    if(isSilverHandSyn(code))                                               mechanicCounters[V_SILVER_HAND]->increaseSyn(code);
    if(isTreantSyn(code))                                                   mechanicCounters[V_TREANT]->increaseSyn(code);
    //New Synergy Step 4
    if(isTauntSyn(code))                                                    mechanicCounters[V_TAUNT]->increaseSyn(code);
    else if(isTauntAllSyn(code))                                            mechanicCounters[V_TAUNT_ALL]->increaseSyn(code);
    if(isDeathrattleSyn(code))                                              mechanicCounters[V_DEATHRATTLE]->increaseSyn(code);
    else if(isDeathrattleGoodAllSyn(code))                                  mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->increaseSyn(code);
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
//    else if(deckCard.getType() == WEAPON)
//    {
//        //Stats
//        int attack = Utility::getCardAttribute(code, "attack").toInt();
//        int health = Utility::getCardAttribute(code, "health").toInt();

//        costWeapons.appendStatValue(false, deckCard.getCost(), code);
//        attackWeapons.appendStatValue(false, attack, code);
//        healthWeapons.appendStatValue(false, health, code);
//    }

    //Synergies
    QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
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
//                switch(statSyn.statKind)
//                {
//                    case S_COST:
//                        if(statSyn.isGen)   costWeapons.appendStatValue(false, statSyn.statValue, code);
//                        else                costWeapons.updateStatsMapSyn(statSyn, code);
//                    break;
//                    case S_ATTACK:
//                        if(statSyn.isGen)   attackWeapons.appendStatValue(false, statSyn.statValue, code);
//                        else                attackWeapons.updateStatsMapSyn(statSyn, code);
//                    break;
//                    case S_HEALTH:
//                        if(statSyn.isGen)   healthWeapons.appendStatValue(false, statSyn.statValue, code);
//                        else                healthWeapons.updateStatsMapSyn(statSyn, code);
//                    break;
//                }
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


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QMap<QString, int> &mechanicIcons)
{
    getCardTypeSynergies(deckCard, synergies);
    getRaceSynergies(deckCard, synergies);
    getMechanicSynergies(deckCard, synergies, mechanicIcons);
    getDirectLinkSynergies(deckCard, synergies);
    getStatsCardsSynergies(deckCard, synergies);
}


void SynergyHandler::getCardTypeSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();

    //Evita mostrar spellSyn cards en cada hechizo que veamos
//    if(cardType == SPELL)
//    {
//        cardTypeCounters[V_SPELL]->insertSynCards(synergies);
//        cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergies);
//    }
//    else if(isSpellGen(code))                   cardTypeCounters[V_SPELL_ALL]->insertSynCards(synergies);
    if(cardType == WEAPON)
    {
        cardTypeCounters[V_WEAPON]->insertSynCards(synergies);
        cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);
    }
    else if(isWeaponGen(code, text))            cardTypeCounters[V_WEAPON_ALL]->insertSynCards(synergies);


    if(isSpellSyn(code))                        cardTypeCounters[V_SPELL]->insertCards(synergies);
    else if(isSpellAllSyn(code, text))          cardTypeCounters[V_SPELL_ALL]->insertCards(synergies);
    if(isWeaponSyn(code))                       cardTypeCounters[V_WEAPON]->insertCards(synergies);
    else if(isWeaponAllSyn(code, text))         cardTypeCounters[V_WEAPON_ALL]->insertCards(synergies);
}


void SynergyHandler::getRaceSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardRace cardRace = deckCard.getRace();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();

    if(cardRace == MURLOC)
    {
        raceCounters[V_MURLOC]->insertSynCards(synergies);
        raceCounters[V_MURLOC_ALL]->insertSynCards(synergies);
    }
    else if(isMurlocGen(code))      raceCounters[V_MURLOC_ALL]->insertSynCards(synergies);
    if(cardRace == DEMON)
    {
        raceCounters[V_DEMON]->insertSynCards(synergies);
        raceCounters[V_DEMON_ALL]->insertSynCards(synergies);
    }
    else if(isDemonGen(code))       raceCounters[V_DEMON_ALL]->insertSynCards(synergies);
    if(cardRace == MECHANICAL)
    {
        raceCounters[V_MECHANICAL]->insertSynCards(synergies);
        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergies);
    }
    else if(isMechGen(code))        raceCounters[V_MECHANICAL_ALL]->insertSynCards(synergies);
    if(cardRace == ELEMENTAL)
    {
        raceCounters[V_ELEMENTAL]->insertSynCards(synergies);
        raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergies);
    }
    else if(isElementalGen(code))   raceCounters[V_ELEMENTAL_ALL]->insertSynCards(synergies);
    if(cardRace == BEAST)
    {
        raceCounters[V_BEAST]->insertSynCards(synergies);
        raceCounters[V_BEAST_ALL]->insertSynCards(synergies);
    }
    else if(isBeastGen(code))       raceCounters[V_BEAST_ALL]->insertSynCards(synergies);
    if(cardRace == TOTEM)
    {
        raceCounters[V_TOTEM]->insertSynCards(synergies);
        raceCounters[V_TOTEM_ALL]->insertSynCards(synergies);
    }
    else if(isTotemGen(code))       raceCounters[V_TOTEM_ALL]->insertSynCards(synergies);
    if(cardRace == PIRATE)
    {
        raceCounters[V_PIRATE]->insertSynCards(synergies);
        raceCounters[V_PIRATE_ALL]->insertSynCards(synergies);
    }
    else if(isPirateGen(code))      raceCounters[V_PIRATE_ALL]->insertSynCards(synergies);
    if(cardRace == DRAGON)
    {
        raceCounters[V_DRAGON]->insertSynCards(synergies);
        raceCounters[V_DRAGON_ALL]->insertSynCards(synergies);
    }
    else if(isDragonGen(code))      raceCounters[V_DRAGON_ALL]->insertSynCards(synergies);

    if(isMurlocSyn(code))                           raceCounters[V_MURLOC]->insertCards(synergies);
    else if(isMurlocAllSyn(code, text))             raceCounters[V_MURLOC_ALL]->insertCards(synergies);
    if(isDemonSyn(code))                            raceCounters[V_DEMON]->insertCards(synergies);
    else if(isDemonAllSyn(code, text))              raceCounters[V_DEMON_ALL]->insertCards(synergies);
    if(isMechSyn(code))                             raceCounters[V_MECHANICAL]->insertCards(synergies);
    else if(isMechAllSyn(code, mechanics, text))    raceCounters[V_MECHANICAL_ALL]->insertCards(synergies);
    if(isElementalSyn(code))                        raceCounters[V_ELEMENTAL]->insertCards(synergies);
    else if(isElementalAllSyn(code, text))          raceCounters[V_ELEMENTAL_ALL]->insertCards(synergies);
    if(isBeastSyn(code))                            raceCounters[V_BEAST]->insertCards(synergies);
    else if(isBeastAllSyn(code, text))              raceCounters[V_BEAST_ALL]->insertCards(synergies);
    if(isTotemSyn(code))                            raceCounters[V_TOTEM]->insertCards(synergies);
    else if(isTotemAllSyn(code, text))              raceCounters[V_TOTEM_ALL]->insertCards(synergies);
    if(isPirateSyn(code))                           raceCounters[V_PIRATE]->insertCards(synergies);
    else if(isPirateAllSyn(code, text))             raceCounters[V_PIRATE_ALL]->insertCards(synergies);
    if(isDragonSyn(code, text))                     raceCounters[V_DRAGON]->insertCards(synergies);
    else if(isDragonAllSyn(code))                   raceCounters[V_DRAGON_ALL]->insertCards(synergies);
}


void SynergyHandler::getMechanicSynergies(DeckCard &deckCard, QMap<QString,int> &synergies, QMap<QString, int> &mechanicIcons)
{
    QString code = deckCard.getCode();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    CardClass cardClass = deckCard.getCardClass();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();
    bool addRestoreIcon = false;

    //GEN
    if(isDiscoverDrawGen(code, mechanics, referencedTags, text))
    {
        mechanicIcons[ThemeHandler::drawMechanicFile()] = mechanicCounters[V_DISCOVER_DRAW]->count() + 1;
    }
    if(isTaunt(code, mechanics))
    {
        mechanicCounters[V_TAUNT]->insertSynCards(synergies);
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergies);
        mechanicIcons[ThemeHandler::tauntMechanicFile()] = mechanicCounters[V_TAUNT_ALL]->count() + 1;
    }
    else if(isTauntGen(code, referencedTags))
    {
        mechanicCounters[V_TAUNT_ALL]->insertSynCards(synergies);
        mechanicIcons[ThemeHandler::tauntMechanicFile()] = mechanicCounters[V_TAUNT_ALL]->count() + 1;
    }
    if(isAoeGen(code, text))
    {
        mechanicCounters[V_AOE]->insertSynCards(synergies);
        mechanicIcons[ThemeHandler::aoeMechanicFile()] = mechanicCounters[V_AOE]->count() + 1;
    }
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicCounters[V_PING]->insertSynCards(synergies);
        mechanicIcons[ThemeHandler::pingMechanicFile()] = mechanicCounters[V_PING]->count() + 1;
    }
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons[ThemeHandler::damageMechanicFile()] = mechanicCounters[V_DAMAGE]->count() + 1;
    }
    if(isDestroyGen(code, mechanics, text))
    {
        mechanicIcons[ThemeHandler::destroyMechanicFile()] = mechanicCounters[V_DESTROY]->count() + 1;
    }
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack))
    {
        mechanicIcons[ThemeHandler::reachMechanicFile()] = mechanicCounters[V_REACH]->count() + 1;
    }
    if(isArmorGen(code, text))
    {
        mechanicCounters[V_ARMOR]->insertSynCards(synergies);
        addRestoreIcon = true;
    }
    if(isRestoreFriendlyHeroGen(code, mechanics, text))
    {
        mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertSynCards(synergies);
        addRestoreIcon = true;
    }
    if(addRestoreIcon)
    {
        mechanicIcons[ThemeHandler::survivalMechanicFile()] = mechanicCounters[V_SURVIVABILITY]->count() + 1;
    }
    if(isRestoreTargetMinionGen(code, text))                    mechanicCounters[V_RESTORE_TARGET_MINION]->insertSynCards(synergies);
    if(isRestoreFriendlyMinionGen(code, text))                  mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertSynCards(synergies);
    if(isLifestealMinon(code, mechanics, cardType))             mechanicCounters[V_LIFESTEAL_MINION]->insertSynCards(synergies);
    if(isJadeGolemGen(code, mechanics, referencedTags))         mechanicCounters[V_JADE_GOLEM]->insertCards(synergies);//Sinergias gen-gen
    if(isHeroPowerGen(code, text, cardClass))                   mechanicCounters[V_HERO_POWER]->insertCards(synergies);//Sinergias gen-gen
    if(isDiscoverGen(code, mechanics, referencedTags))          mechanicCounters[V_DISCOVER]->insertSynCards(synergies);
    if(isDrawGen(code, text))                                   mechanicCounters[V_DRAW]->insertSynCards(synergies);
    if(isToYourHandGen(code, text))                             mechanicCounters[V_TOYOURHAND]->insertSynCards(synergies);
    if(isOverload(code))                                        mechanicCounters[V_OVERLOAD]->insertSynCards(synergies);
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text)) mechanicCounters[V_FREEZE_ENEMY]->insertSynCards(synergies);
    if(isDiscardGen(code, text))                                mechanicCounters[V_DISCARD]->insertSynCards(synergies);
    if(isDeathrattleMinion(code, mechanics, cardType))          mechanicCounters[V_DEATHRATTLE]->insertSynCards(synergies);
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType)) mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertSynCards(synergies);
    if(isBattlecry(code, mechanics))                            mechanicCounters[V_BATTLECRY]->insertSynCards(synergies);
    if(isSilenceOwnGen(code, mechanics, referencedTags))        mechanicCounters[V_SILENCE]->insertSynCards(synergies);
    if(isTauntGiverGen(code))                                   mechanicCounters[V_TAUNT_GIVER]->insertSynCards(synergies);
    if(isTokenGen(code, text))                                  mechanicCounters[V_TOKEN]->insertSynCards(synergies);
    if(isTokenCardGen(code, cost))                              mechanicCounters[V_TOKEN_CARD]->insertSynCards(synergies);
    if(isComboGen(code, mechanics))                             mechanicCounters[V_COMBO]->insertSynCards(synergies);
    if(isWindfuryMinion(code, mechanics, cardType))             mechanicCounters[V_WINDFURY_MINION]->insertSynCards(synergies);
    if(isAttackBuffGen(code, text))                             mechanicCounters[V_ATTACK_BUFF]->insertSynCards(synergies);
    if(isHealthBuffGen(code, text))                             mechanicCounters[V_HEALTH_BUFF]->insertSynCards(synergies);
    if(isReturnGen(code, text))                                 mechanicCounters[V_RETURN]->insertSynCards(synergies);
    if(isStealthGen(code, mechanics))                           mechanicCounters[V_STEALTH]->insertSynCards(synergies);
    if(isSpellDamageGen(code))                                  mechanicCounters[V_SPELL_DAMAGE]->insertSynCards(synergies);
    if(isEvolveGen(code, text))                                 mechanicCounters[V_EVOLVE]->insertSynCards(synergies);
    if(isSpawnEnemyGen(code, text))                             mechanicCounters[V_SPAWN_ENEMY]->insertSynCards(synergies);
    if(isEnrageGen(code, mechanics))                            mechanicCounters[V_ENRAGED]->insertSynCards(synergies);
    if(isEggGen(code, mechanics, attack, cardType))             mechanicCounters[V_EGG]->insertSynCards(synergies);
    if(isDamageFriendlyHeroGen(code))                           mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->insertSynCards(synergies);
    if(isHandBuffGen(code, text))                               mechanicCounters[V_HAND_BUFF]->insertSynCards(synergies);
    if(isEnemyDrawGen(code, text))                              mechanicCounters[V_ENEMY_DRAW]->insertSynCards(synergies);
    if(isHeroAttackGen(code, text))                             mechanicCounters[V_HERO_ATTACK]->insertSynCards(synergies);
    if(isSpellBuffGen(code, text, mechanics, cardType))         mechanicCounters[V_SPELL_BUFF]->insertSynCards(synergies);
    if(isOtherClassGen(code, text, cardClass))                  mechanicCounters[V_OTHER_CLASS]->insertSynCards(synergies);
    if(isSilverHandGen(code, text, cardClass))                  mechanicCounters[V_SILVER_HAND]->insertSynCards(synergies);
    if(isTreantGen(code, text, cardClass))                      mechanicCounters[V_TREANT]->insertSynCards(synergies);
    //New Synergy Step 5
    if(isDivineShield(code, mechanics))
    {
        mechanicCounters[V_DIVINE_SHIELD]->insertSynCards(synergies);
        mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergies);
    }
    else if(isDivineShieldGen(code, referencedTags))            mechanicCounters[V_DIVINE_SHIELD_ALL]->insertSynCards(synergies);

    if(isSecret(code, mechanics))
    {
        mechanicCounters[V_SECRET]->insertSynCards(synergies);
        mechanicCounters[V_SECRET_ALL]->insertSynCards(synergies);
    }
    else if(isSecretGen(code))                                  mechanicCounters[V_SECRET_ALL]->insertSynCards(synergies);
    if(isEcho(code, text))
    {
        mechanicCounters[V_ECHO]->insertSynCards(synergies);
        mechanicCounters[V_ECHO_ALL]->insertSynCards(synergies);
    }
    else if(isEchoGen(code))                                    mechanicCounters[V_ECHO_ALL]->insertSynCards(synergies);
    if(isRush(code, text))
    {
        mechanicCounters[V_RUSH]->insertSynCards(synergies);
        mechanicCounters[V_RUSH_ALL]->insertSynCards(synergies);
    }
    else if(isRushGen(code))                                    mechanicCounters[V_RUSH_ALL]->insertSynCards(synergies);
    if(isMagnetic(code, mechanics))
    {
        mechanicCounters[V_MAGNETIC]->insertSynCards(synergies);
        mechanicCounters[V_MAGNETIC_ALL]->insertSynCards(synergies);
    }
    else if(isMagneticGen(code))                                mechanicCounters[V_MAGNETIC_ALL]->insertSynCards(synergies);


    //SYN
    if(isAoeSyn(code))                                          mechanicCounters[V_AOE]->insertCards(synergies);
    if(isPingSyn(code))                                         mechanicCounters[V_PING]->insertCards(synergies);
    if(isDiscoverSyn(code))                                     mechanicCounters[V_DISCOVER]->insertCards(synergies);
    if(isDrawSyn(code))                                         mechanicCounters[V_DRAW]->insertCards(synergies);
    if(isToYourHandSyn(code))                                   mechanicCounters[V_TOYOURHAND]->insertCards(synergies);
    if(isOverloadSyn(code, text))                               mechanicCounters[V_OVERLOAD]->insertCards(synergies);
    if(isFreezeEnemySyn(code, referencedTags, text))            mechanicCounters[V_FREEZE_ENEMY]->insertCards(synergies);
    if(isDiscardSyn(code, text))                                mechanicCounters[V_DISCARD]->insertCards(synergies);
    if(isBattlecrySyn(code, referencedTags))                    mechanicCounters[V_BATTLECRY]->insertCards(synergies);
    if(isSilenceOwnSyn(code, mechanics))                        mechanicCounters[V_SILENCE]->insertCards(synergies);
    if(isTauntGiverSyn(code, mechanics, attack, cardType))      mechanicCounters[V_TAUNT_GIVER]->insertCards(synergies);
    if(isTokenSyn(code, text))                                  mechanicCounters[V_TOKEN]->insertCards(synergies);
    if(isTokenCardSyn(code, text))                              mechanicCounters[V_TOKEN_CARD]->insertCards(synergies);
    if(isComboSyn(code, referencedTags))                        mechanicCounters[V_COMBO]->insertCards(synergies);
    if(isWindfuryMinionSyn(code))                               mechanicCounters[V_WINDFURY_MINION]->insertCards(synergies);
    if(isAttackBuffSyn(code, mechanics, attack, cardType))      mechanicCounters[V_ATTACK_BUFF]->insertCards(synergies);
    if(isHealthBuffSyn(code))                                   mechanicCounters[V_HEALTH_BUFF]->insertCards(synergies);
    //returnSyn es synergia debil
//    if(isReturnSyn(code, mechanics, cardType, text))            mechanicCounters[V_RETURN]->insertCards(synergies);
    if(isStealthSyn(code))                                      mechanicCounters[V_STEALTH]->insertCards(synergies);
    if(isSpellDamageSyn(code, mechanics, cardType, text))       mechanicCounters[V_SPELL_DAMAGE]->insertCards(synergies);
    if(isEvolveSyn(code))                                       mechanicCounters[V_EVOLVE]->insertCards(synergies);
    if(isSpawnEnemySyn(code))                                   mechanicCounters[V_SPAWN_ENEMY]->insertCards(synergies);
    if(isRestoreTargetMinionSyn(code))                          mechanicCounters[V_RESTORE_TARGET_MINION]->insertCards(synergies);
    if(isRestoreFriendlyHeroSyn(code))                          mechanicCounters[V_RESTORE_FRIENDLY_HEROE]->insertCards(synergies);
    if(isRestoreFriendlyMinionSyn(code))                        mechanicCounters[V_RESTORE_FRIENDLY_MINION]->insertCards(synergies);
    if(isArmorSyn(code))                                        mechanicCounters[V_ARMOR]->insertCards(synergies);
    if(isLifestealMinionSyn(code))                              mechanicCounters[V_LIFESTEAL_MINION]->insertCards(synergies);
    if(isEnrageSyn(code, text))                                 mechanicCounters[V_ENRAGED]->insertCards(synergies);
    if(isEggSyn(code, text))                                    mechanicCounters[V_EGG]->insertCards(synergies);
    if(isDamageFriendlyHeroSyn(code))                           mechanicCounters[V_DAMAGE_FRIENDLY_HERO]->insertCards(synergies);
    if(isHandBuffSyn(code, text))                               mechanicCounters[V_HAND_BUFF]->insertCards(synergies);
    if(isEnemyDrawSyn(code, text))                              mechanicCounters[V_ENEMY_DRAW]->insertCards(synergies);
    if(isHeroAttackSyn(code))                                   mechanicCounters[V_HERO_ATTACK]->insertCards(synergies);
    if(isSpellBuffSyn(code, text))                              mechanicCounters[V_SPELL_BUFF]->insertCards(synergies);
    if(isOtherClassSyn(code, text, cardClass))                  mechanicCounters[V_OTHER_CLASS]->insertCards(synergies);
    if(isSilverHandSyn(code))                                   mechanicCounters[V_SILVER_HAND]->insertCards(synergies);
    if(isTreantSyn(code))                                       mechanicCounters[V_TREANT]->insertCards(synergies);
    //New Synergy Step 6
    if(isTauntSyn(code))                                        mechanicCounters[V_TAUNT]->insertCards(synergies);
    else if(isTauntAllSyn(code))                                mechanicCounters[V_TAUNT_ALL]->insertCards(synergies);
    if(isDeathrattleSyn(code))                                  mechanicCounters[V_DEATHRATTLE]->insertCards(synergies);
    else if(isDeathrattleGoodAllSyn(code))                      mechanicCounters[V_DEATHRATTLE_GOOD_ALL]->insertCards(synergies);
    if(isDivineShieldSyn(code))                                 mechanicCounters[V_DIVINE_SHIELD]->insertCards(synergies);
    else if(isDivineShieldAllSyn(code))                         mechanicCounters[V_DIVINE_SHIELD_ALL]->insertCards(synergies);
    if(isSecretSyn(code))                                       mechanicCounters[V_SECRET]->insertCards(synergies);
    else if(isSecretAllSyn(code, referencedTags))               mechanicCounters[V_SECRET_ALL]->insertCards(synergies);
    if(isEchoSyn(code))                                         mechanicCounters[V_ECHO]->insertCards(synergies);
    else if(isEchoAllSyn(code))                                 mechanicCounters[V_ECHO_ALL]->insertCards(synergies);
    if(isRushSyn(code))                                         mechanicCounters[V_RUSH]->insertCards(synergies);
    else if(isRushAllSyn(code))                                 mechanicCounters[V_RUSH_ALL]->insertCards(synergies);
    if(isMagneticSyn(code))                                     mechanicCounters[V_MAGNETIC]->insertCards(synergies);
    else if(isMagneticAllSyn(code))                             mechanicCounters[V_MAGNETIC_ALL]->insertCards(synergies);
}


void SynergyHandler::getDirectLinkSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();

    if(directLinks.contains(code))
    {
        QList<QString> linkCodes = directLinks[code];

        for(QString linkCode: linkCodes)
        {
            if(cardTypeCounters[V_MINION]->insertCode(linkCode, synergies)){}
            else if(cardTypeCounters[V_WEAPON]->insertCode(linkCode, synergies)){}
            else cardTypeCounters[V_SPELL]->insertCode(linkCode, synergies);
        }
    }
}


void SynergyHandler::getStatsCardsSynergies(DeckCard &deckCard, QMap<QString,int> &synergies)
{
    QString code = deckCard.getCode();

    if(deckCard.getType() == MINION)
    {
        //Stats
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int health = Utility::getCardAttribute(code, "health").toInt();

        costMinions.insertCards(true, deckCard.getCost(), synergies);
        attackMinions.insertCards(true, attack, synergies);
        healthMinions.insertCards(true, health, synergies);
    }
    else if(deckCard.getType() == SPELL)
    {
        costSpells.insertCards(true, deckCard.getCost(), synergies);
    }
//    else if(deckCard.getType() == WEAPON)
//    {
//        //Stats
//        int attack = Utility::getCardAttribute(code, "attack").toInt();
//        int health = Utility::getCardAttribute(code, "health").toInt();

//        costWeapons.insertCards(true, deckCard.getCost(), synergies);
//        attackWeapons.insertCards(true, attack, synergies);
//        healthWeapons.insertCards(true, health, synergies);
//    }

    //Synergies
    QList<StatSyn> statSyns = StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);
    for(const StatSyn &statSyn: statSyns)
    {
        switch(statSyn.cardType)
        {
            case S_MINION:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costMinions.insertCards(true, statSyn.statValue, synergies);
                        else                costMinions.insertStatCards(statSyn, synergies);
                    break;
                    case S_ATTACK:
                        if(statSyn.isGen)   attackMinions.insertCards(true, statSyn.statValue, synergies);
                        else                attackMinions.insertStatCards(statSyn, synergies);
                    break;
                    case S_HEALTH:
                        if(statSyn.isGen)   healthMinions.insertCards(true, statSyn.statValue, synergies);
                        else                healthMinions.insertStatCards(statSyn, synergies);
                    break;
                }
            break;
            case S_SPELL:
                switch(statSyn.statKind)
                {
                    case S_COST:
                        if(statSyn.isGen)   costSpells.insertCards(true, statSyn.statValue, synergies);
                        else                costSpells.insertStatCards(statSyn, synergies);
                    break;
                    case S_ATTACK:
                    case S_HEALTH:
                    break;
                }
            break;
            case S_WEAPON:
//                switch(statSyn.statKind)
//                {
//                    case S_COST:
//                        if(statSyn.isGen)   costWeapons.insertCards(true, statSyn.statValue, synergies);
//                        else                costWeapons.insertStatCards(statSyn, synergies);
//                    break;
//                    case S_ATTACK:
//                        if(statSyn.isGen)   attackWeapons.insertCards(true, statSyn.statValue, synergies);
//                        else                attackWeapons.insertStatCards(statSyn, synergies);
//                    break;
//                    case S_HEALTH:
//                        if(statSyn.isGen)   healthWeapons.insertCards(true, statSyn.statValue, synergies);
//                        else                healthWeapons.insertStatCards(statSyn, synergies);
//                    break;
//                }
            break;
        }
    }
}


bool SynergyHandler::isValidSynergyCode(const QString &mechanic)
{
    if(mechanic.startsWith('='))    return true;
    QStringList validMecs = {
        "spellGen", "weaponGen", "murlocGen", "demonGen", "mechGen", "elementalGen", "beastGen", "totemGen", "pirateGen", "dragonGen",
        "spellSyn", "weaponSyn", "murlocSyn", "demonSyn", "mechSyn", "elementalSyn", "beastSyn", "totemSyn", "pirateSyn", "dragonSyn",
        "spellAllSyn", "weaponAllSyn", "murlocAllSyn", "demonAllSyn", "mechAllSyn", "elementalAllSyn", "beastAllSyn", "totemAllSyn", "pirateAllSyn", "dragonAllSyn",

        "discover", "drawGen", "toYourHandGen", "enemyDrawGen",
        "discoverSyn", "drawSyn", "toYourHandSyn", "enemyDrawSyn",

        "taunt", "tauntGen", "divineShield", "divineShieldGen", "windfury", "overload",
        "tauntSyn", "tauntAllSyn", "divineShieldSyn", "divineShieldAllSyn", "windfuryMinionSyn", "overloadSyn",

        "jadeGolemGen", "secret", "secretGen", "freezeEnemyGen", "discardGen", "stealthGen",
        "heroPowerGen", "secretSyn", "secretAllSyn", "freezeEnemySyn", "discardSyn", "stealthSyn",

        "damageMinionsGen", "reachGen", "pingGen", "aoeGen", "destroyGen",
        "damageMinionsSyn", "reachSyn", "pingSyn", "aoeSyn", "destroySyn",

        "deathrattle", "deathrattleGen", "deathrattleOpponent", "silenceOwnGen", "battlecry", "returnGen",
        "deathrattleSyn", "deathrattleGoodAllSyn", "silenceOwnSyn", "battlecrySyn", "returnSyn",

        "enrageGen", "tauntGiverGen", "evolveGen", "spawnEnemyGen", "spellDamageGen", "handBuffGen", "spellBuffGen",
        "enrageSyn", "tauntGiverSyn", "evolveSyn", "spawnEnemySyn", "spellDamageSyn", "handBuffSyn", "spellBuffSyn",

        "tokenGen", "tokenCardGen", "comboGen", "attackBuffGen", "healthBuffGen", "heroAttackGen",
        "tokenSyn", "tokenCardSyn", "comboSyn", "attackBuffSyn", "healthBuffSyn", "heroAttackSyn",

        "restoreTargetMinionGen", "restoreFriendlyHeroGen", "restoreFriendlyMinionGen", "armorGen", "lifesteal",
        "restoreTargetMinionSyn", "restoreFriendlyHeroSyn", "restoreFriendlyMinionSyn", "armorSyn", "lifestealMinionSyn",

        "eggGen", "damageFriendlyHeroGen", "echo", "echoGen", "rush", "rushGen", "magnetic", "magneticGen",
        "eggSyn", "damageFriendlyHeroSyn", "echoSyn", "echoAllSyn", "rushSyn", "rushAllSyn", "magneticSyn", "magneticAllSyn",

        "otherClassGen", "silverHandGen", "treantGen",
        "otherClassSyn", "silverHandSyn", "treantSyn"
        //New Synergy Step 7
    };
    if(mechanic.startsWith("discover") || mechanic.startsWith("drawGen") || mechanic.startsWith("toYourHandGen"))   return true;
    return validMecs.contains(mechanic);
}


void SynergyHandler::checkSynergyCodes()
{
    qDebug()<<endl<<"-----Check Synergies.json-----"<<endl;
    initSynergyCodes();
    for(const QString &code: Utility::getWildCodes())
    {
        if(synergyCodes.contains(code))
        {
            QStringList invalidMecs;
            for(const QString &mechanic: synergyCodes[code])
            {
                if(!isValidSynergyCode(mechanic))   invalidMecs.append(mechanic);
            }
            if(!invalidMecs.isEmpty())  qDebug()<<"Code:"<<code<<"No mecs:"<<invalidMecs;
            StatSynergies::getStatsSynergiesFromJson(code, synergyCodes);//Check fallos en synergy stats -> =GenMinionHealth1
        }
    }
    qDebug()<<endl<<"-----Check complete-----"<<endl;
}


void SynergyHandler::testSynergies()
{
    initSynergyCodes();
    int num = 0;

//    for(const QString &code: Utility::getSetCodes("GILNEAS"))
//    for(const QString &code: Utility::getStandardCodes())
    for(const QString &code: Utility::getWildCodes())
    {
        DeckCard deckCard(code);
        CardType cardType = deckCard.getType();
        CardClass cardClass = deckCard.getCardClass();
        QString text = Utility::cardEnTextFromCode(code).toLower();
        int attack = Utility::getCardAttribute(code, "attack").toInt();
        int cost = deckCard.getCost();
        QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
        QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
        if(
                (text.contains("heal") && text.contains("deal damage") && cardClass == PRIEST)
//              text.contains("treant")
//                && (cardClass == NEUTRAL || cardClass == DRUID)
//            isTreantGen(code, text, cardClass)
            )
        {
//            qDebug()<<++num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;
            qDebug()<<code + " " + Utility::cardEnNameFromCode(code)<<endl<<++num<<text;
//            debugSynergiesCode(code, ++num);
//            qDebug()<<mechanics<<endl<<referencedTags;
        }
        Q_UNUSED(cardType);
        Q_UNUSED(cardClass);
        Q_UNUSED(text);
        Q_UNUSED(attack);
        Q_UNUSED(cost);
        Q_UNUSED(mechanics);
        Q_UNUSED(referencedTags);
    }
}


void SynergyHandler::debugSynergiesSet(const QString &set, bool onlyCollectible)
{
    initSynergyCodes();

    qDebug()<<endl<<"-----SynergiesNames.json-----"<<endl;
    for(const QString &code: Utility::getSetCodes(set, onlyCollectible))
    {
        qDebug()<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
    }

    qDebug()<<endl<<"-----Synergies.json-----"<<endl;
    int num = 0;
    for(const QString &code: Utility::getSetCodes(set, onlyCollectible))
    {
        debugSynergiesCode(code, ++num);
    }
}


void SynergyHandler::debugSynergiesCode(const QString &code, int num)
{
    QStringList mec;

    DeckCard deckCard(code);
    CardType cardType = deckCard.getType();
    CardClass cardClass = deckCard.getCardClass();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int cost = deckCard.getCost();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    if(isMurlocAllSyn(code, text))          mec<<"murlocAllSyn";
    if(isDemonAllSyn(code, text))           mec<<"demonAllSyn";
    if(isMechAllSyn(code, mechanics, text)) mec<<"mechAllSyn";
    if(isElementalAllSyn(code, text))       mec<<"elementalAllSyn";
    if(isBeastAllSyn(code, text))           mec<<"beastAllSyn";
    if(isTotemAllSyn(code, text))           mec<<"totemAllSyn";
    if(isPirateAllSyn(code, text))          mec<<"pirateAllSyn";
    if(isDragonSyn(code, text))             mec<<"dragonSyn";

    if(isWeaponGen(code, text))             mec<<"weaponGen";
    if(isSpellAllSyn(code, text))           mec<<"spellAllSyn";
    if(isWeaponAllSyn(code, text))          mec<<"weaponAllSyn";

    if(isDiscoverGen(code, mechanics, referencedTags))                      mec<<"discover";
    if(isDrawGen(code, text))                                               mec<<"drawGen";
    if(isToYourHandGen(code, text))                                         mec<<"toYourHandGen";
    if(isAoeGen(code, text))                                                mec<<"aoeGen";
    if(isPingGen(code, mechanics, referencedTags, text, cardType, attack))  mec<<"pingGen";
    if(isDamageMinionsGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"damageMinionsGen";
    if(isDestroyGen(code, mechanics, text))                                 mec<<"destroyGen";
    if(isReachGen(code, mechanics, referencedTags, text, cardType, attack)) mec<<"reachGen";
    if(isOverload(code))                                                    mec<<"overload";
    if(isJadeGolemGen(code, mechanics, referencedTags))                     mec<<"jadeGolemGen";
    if(isHeroPowerGen(code, text, cardClass))                               mec<<"heroPowerGen";
    if(isSecret(code, mechanics))                                           mec<<"secret";
    if(isEcho(code, text))                                                  mec<<"echo";
    if(isRush(code, text))                                                  mec<<"rush";
    if(isMagnetic(code, mechanics))                                         mec<<"magnetic";
    if(isEggGen(code, mechanics, attack, cardType))                         mec<<"eggGen";
    if(isFreezeEnemyGen(code, mechanics, referencedTags, text))             mec<<"freezeEnemyGen";
    if(isDiscardGen(code, text))                                            mec<<"discardGen";
    if(isDeathrattleMinion(code, mechanics, cardType))                      mec<<"deathrattle o deathrattleOpponent";
    if(isDeathrattleGoodAll(code, mechanics, referencedTags, cardType))     mec<<"deathrattle o deathrattleGen";
    if(isBattlecry(code, mechanics))                                        mec<<"battlecry";
    if(isSilenceOwnGen(code, mechanics, referencedTags))                    mec<<"silenceOwnGen";
    if(isTokenGen(code, text))                                              mec<<"tokenGen";
    if(isTokenCardGen(code, cost))                                          mec<<"tokenCardGen";
    if(isWindfuryMinion(code, mechanics, cardType))                         mec<<"windfury";
    if(isAttackBuffGen(code, text))                                         mec<<"attackBuffGen";
    if(isHealthBuffGen(code, text))                                         mec<<"healthBuffGen";
    if(isReturnGen(code, text))                                             mec<<"returnGen";
    if(isStealthGen(code, mechanics))                                       mec<<"stealthGen";
    if(isSpellDamageGen(code))                                              mec<<"spellDamageGen";
    if(isEvolveGen(code, text))                                             mec<<"evolveGen";
    if(isSpawnEnemyGen(code, text))                                         mec<<"spawnEnemyGen";
    if(isRestoreTargetMinionGen(code, text))                                mec<<"restoreTargetMinionGen";
    if(isRestoreFriendlyHeroGen(code, mechanics, text))                     mec<<"restoreFriendlyHeroGen o lifesteal";
    if(isRestoreFriendlyMinionGen(code, text))                              mec<<"restoreFriendlyMinionGen";
    if(isArmorGen(code, text))                                              mec<<"armorGen";
    if(isLifestealMinon(code, mechanics, cardType))                         mec<<"lifesteal";
    if(isTaunt(code, mechanics))                                            mec<<"taunt";
    else if(isTauntGen(code, referencedTags))                               mec<<"tauntGen";
    if(isDivineShield(code, mechanics))                                     mec<<"divineShield";
    else if(isDivineShieldGen(code, referencedTags))                        mec<<"divineShieldGen";
    if(isEnrageGen(code, mechanics))                                        mec<<"enrageGen";
    if(isComboGen(code, mechanics))                                         mec<<"comboGen";
    if(isHandBuffGen(code, text))                                           mec<<"handBuffGen";
    if(isEnemyDrawGen(code, text))                                          mec<<"enemyDrawGen";
    if(isHeroAttackGen(code, text))                                         mec<<"heroAttackGen";
    if(isSpellBuffGen(code, text, mechanics, cardType))                     mec<<"spellBuffGen";
    if(isOtherClassGen(code, text, cardClass))                              mec<<"otherClassGen";
    if(isSilverHandGen(code, text, cardClass))                              mec<<"silverHandGen";
    if(isTreantGen(code, text, cardClass))                                  mec<<"treantGen";
    //New Synergy Step 8

    //Solo analizamos los que tienen patrones definidos
    if(isOverloadSyn(code, text))                                           mec<<"overloadSyn";
    if(isSecretAllSyn(code, referencedTags))                                mec<<"secretAllSyn";
    if(isFreezeEnemySyn(code, referencedTags, text))                        mec<<"freezeEnemySyn";
    if(isDiscardSyn(code, text))                                            mec<<"discardSyn";
    if(isBattlecrySyn(code, referencedTags))                                mec<<"battlecrySyn";
    if(isSilenceOwnSyn(code, mechanics))                                    mec<<"silenceOwnSyn";
    if(isTauntGiverSyn(code, mechanics, attack, cardType))                  mec<<"tauntGiverSyn";
    if(isAttackBuffSyn(code, mechanics, attack, cardType))                  mec<<"attackBuffSyn";
    if(isTokenSyn(code, text))                                              mec<<"tokenSyn";
    if(isReturnSyn(code, mechanics, cardType, text))                        mec<<"returnSyn";
    if(isSpellDamageSyn(code, mechanics, cardType, text))                   mec<<"spellDamageSyn";
    if(isEnrageSyn(code, text))                                             mec<<"enrageSyn";
    if(isTokenCardSyn(code, text))                                          mec<<"tokenCardSyn";
    if(isEggSyn(code, text))                                                mec<<"eggSyn";
    if(isHandBuffSyn(code, text))                                           mec<<"handBuffSyn";
    if(isEnemyDrawSyn(code, text))                                          mec<<"enemyDrawSyn";
    if(isSpellBuffSyn(code, text))                                          mec<<"spellBuffSyn";
    if(isOtherClassSyn(code, text, cardClass))                              mec<<"otherClassSyn";
    //New Synergy Step 9 (Solo si busca patron)

    qDebug()<<num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
    if(synergyCodes.contains(code)) qDebug()<<"--MANUAL-- :"<<code<<": ["<<synergyCodes[code]<<"],";
    else                            qDebug()<<code<<": ["<<mec<<"],";
    qDebug()<<"Texto:"<<text<<endl;
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
bool SynergyHandler::isDiscoverDrawGen(const QString &code, const QJsonArray &mechanics, const QJsonArray &referencedTags,
                                       const QString &text)
{
    //TEST
    //&& (text.contains("draw") || text.contains("discover") || (text.contains("to") && text.contains("your") && text.contains("hand")))
    return(isDiscoverGen(code, mechanics, referencedTags) ||
            isDrawGen(code, text) ||
            isToYourHandGen(code, text));
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
        for(QString mechanic: synergyCodes[code])
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
        for(QString mechanic: synergyCodes[code])
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
    else if(text.contains("draw") && !text.contains("drawn"))
    {
        return 1;
    }
    return 0;
}
bool SynergyHandler::isToYourHandGen(const QString &code, const QString &text)
{
    return numToYourHandGen(code, text)>0;
}
int SynergyHandler::numToYourHandGen(const QString &code, const QString &text)
{
    //TEST
    //&& (text.contains("to") && text.contains("your") && text.contains("hand"))
    if(synergyCodes.contains(code))
    {
        for(QString mechanic: synergyCodes[code])
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
    else if(text.contains("to") && text.contains("your") && text.contains("hand") && !text.contains("return"))
    {
        return 1;
    }
    else if(isEcho(code, text)) return 1;
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
        if(mechanics.contains("DEATHRATTLE") && text.contains("random"))    return false;
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
        else if(isRush(code, text))  return true;
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
        return synergyCodes[code].contains("damageMinionsGen");
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
        else if(isRush(code, text))  return true;
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
    else if(mechanics.contains(QJsonValue("ENRAGED")))
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
bool SynergyHandler::isHeroPowerGen(const QString &code, const QString &text, const CardClass &cardClass)
{
    //TEST
    //text.contains("hero power") || (text.contains("heal") && text.contains("deal damage") && cardClass == PRIEST)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("heroPowerGen");
    }
    else if(text.contains("hero power") || (text.contains("heal") && text.contains("deal damage") && cardClass == PRIEST))
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
bool SynergyHandler::isEcho(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echo");
    }
    else if(text.contains("<b>echo</b>"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isEchoGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("echoGen");
    }
    return false;
}
bool SynergyHandler::isRush(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rush");
    }
    else if(text.contains("<b>rush</b>"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isRushGen(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("rushGen");
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
bool SynergyHandler::isTokenGen(const QString &code, const QString &text)
{
    //TEST
    //(text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("2/2")) && !text.contains("opponent")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenGen");
    }
    else if((text.contains("1/1") || text.contains("2/1") || text.contains("1/2") || text.contains("0/2") || text.contains("0/1"))
            && text.contains("summon") && !text.contains("opponent"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTokenCardGen(const QString &code, int cost)
{
    //Incluimos cartas que en conjunto permitan jugar 2+ cartas de coste 0/1/2
    //TEST
    //text.contains("to") && text.contains("your") && text.contains("hand")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenCardGen");
    }
    if(cost == 0)   return true;
    return false;
}
bool SynergyHandler::isComboGen(const QString &code, const QJsonArray &mechanics)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboGen");
    }
    else if(mechanics.contains(QJsonValue("COMBO")))
    {
        return true;
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
    //&& (mechanics.contains(QJsonValue("STEALTH")) ||  referencedTags.contains(QJsonValue("STEALTH")))
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
bool SynergyHandler::isRestoreFriendlyHeroGen(const QString &code, const QJsonArray &mechanics, const QString &text)
{
    //TEST
    //&& text.contains("restore")
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("restoreFriendlyHeroGen") || synergyCodes[code].contains("lifesteal");
    }
    else if(mechanics.contains(QJsonValue("LIFESTEAL")))
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
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("spawnEnemyGen");
    }
    else if(text.contains("summon") && text.contains("for") && text.contains("your") && text.contains("opponent"))
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
bool SynergyHandler::isOtherClassGen(const QString &code, const QString &text, const CardClass &cardClass)
{
    //TEST
    //(text.contains("opponent") || text.contains("another")) && text.contains("class")
    //&& (cardClass == NEUTRAL || cardClass == ROGUE)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("otherClassGen");
    }
    else if((text.contains("opponent") || text.contains("another")) && text.contains("class")
            && (cardClass == NEUTRAL || cardClass == ROGUE))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isSilverHandGen(const QString &code, const QString &text, const CardClass &cardClass)
{
    //TEST
    //text.contains("silver hand")
    //&& (cardClass == NEUTRAL || cardClass == PALADIN)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("silverHandGen");
    }
    else if(text.contains("silver hand")
            && (cardClass == NEUTRAL || cardClass == PALADIN))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isTreantGen(const QString &code, const QString &text, const CardClass &cardClass)
{
    //TEST
    //text.contains("treant")
    //&& (cardClass == NEUTRAL || cardClass == DRUID)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("treantGen");
    }
    else if(text.contains("treant")
            && (cardClass == NEUTRAL || cardClass == DRUID))
    {
        return true;
    }
    return false;
}
//New Synergy Step 10


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
        return  text.contains("spell") && (text.contains("you cast") || text.contains("cost"));
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
bool SynergyHandler::isDragonSyn(const QString &code, const QString &text)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonSyn");
    }
    else
    {
        return text.contains("dragon");//Con los dragones la synergia por defecto es con dragones en mano
    }
}
bool SynergyHandler::isDragonAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("dragonAllSyn");
    }
    return false;
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
bool SynergyHandler::isDeathrattleGoodAllSyn(const QString &code)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("deathrattleGoodAllSyn");
    }
    return false;
}
bool SynergyHandler::isBattlecrySyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("battlecrySyn");
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
bool SynergyHandler::isTokenSyn(const QString &code, const QString &text)
{
    //TEST
//    && (text.contains("+") && (text.contains("minions") || text.contains("characters"))
//    && !text.contains("hand") && !text.contains("health")
//    || (text.contains("control") && text.contains("least") && text.contains("minions")))
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("tokenSyn");
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
        return synergyCodes[code].contains("tokenCardSyn") || synergyCodes[code].contains("comboGen");
    }
    else if(text.contains("play") && text.contains("card") && !text.contains("player"))
    {
        return true;
    }
    return false;
}
bool SynergyHandler::isComboSyn(const QString &code, const QJsonArray &referencedTags)
{
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("comboSyn");
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
    else if(mechanics.contains(QJsonValue("BATTLECRY")) || mechanics.contains(QJsonValue("COMBO")))
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
    else if(isEcho(code, text)) return true;
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
bool SynergyHandler::isOtherClassSyn(const QString &code, const QString &text, const CardClass &cardClass)
{
    //TEST
    //text.contains("card") && text.contains("from another class")
    //&& (cardClass == NEUTRAL || cardClass == ROGUE)
    if(synergyCodes.contains(code))
    {
        return synergyCodes[code].contains("otherClassSyn");
    }
    else if(text.contains("card") && text.contains("from another class")
            && (cardClass == NEUTRAL || cardClass == ROGUE))
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
//New Synergy Step 11


int SynergyHandler::getCorrectedCardMana(DeckCard &deckCard)
{
    QString code = deckCard.getCode();
    //Tavern of time cards
//    if(code == GRASP_THE_FUTURE)    return 0;
//    if(code == TIMEBOUND_GIANT)     return 6;
//    if(code == TIMEWAY_WANDERER)    return 0;

    if(code == BLOODBLOOM)          return 0;
    if(code == PRIMORDIAL_GLYPH)    return 0;
    if(code == FAR_SIGHT)           return 0;
    if(code == CHEAT_DEATH)         return 0;
    if(code == MANA_BIND)           return 0;
    if(code == LUNAS_POCKET_GALAXY) return 0;
    if(code == ACADEMIC_ESPIONAGE)  return 0;
    if(code == HAUNTING_VISIONS)    return 0;
    if(code == NERUBIAN_PROPHET)    return 3;
    if(code == CORRIDOR_CREEPER)    return 3;
    if(code == SECOND_RATE_BRUISER) return 3;
    if(code == DREAMPETAL_FLORIST)  return 3;
    if(code == PILOTED_REAPER)      return 3;
    if(code == MOLTEN_BLADE)        return 4;
    if(code == SHIFTER_ZERUS)       return 4;
    if(code == SHIFTING_SCROLL)     return 4;
    if(code == CHAMELEOS)           return 4;
    if(code == FORBIDDEN_SHAPING)   return 4;
    if(code == FORBIDDEN_FLAME)     return 4;
    if(code == FORBIDDEN_HEALING)   return 4;
    if(code == FORBIDDEN_RITUAL)    return 4;
    if(code == FORBIDDEN_ANCIENT)   return 4;
    if(code == DEMONBOLT)           return 5;
    if(code == RABBLE_BOUNCER)      return 5;
    if(code == SEA_GIANT)           return 6;
    if(code == MULCHMUNCHER)        return 8;
    if(code == GRAVE_HORROR)        return 8;

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
lifesteal - lifestealMinionSyn
lifesteal y restoreFriendlyHeroGen (no hace falta poner restore si es lifesteal)
restoreTargetMinionGen <--> restoreFriendlyHeroGen
restoreTargetMinionGen o restoreFriendlyMinionGen
restoreTargetMinionSyn, restoreFriendlyMinionSyn, restoreFriendlyHeroSyn
damageFriendlyHeroGen/damageFriendlyHeroSyn
eggGen/eggSyn
=(>|<)(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)(0-15)



Mechanics list:
spellGen, weaponGen
murlocGen, demonGen, mechGen, elementalGen, beastGen, totemGen, pirateGen, dragonGen
discover, drawGen, toYourHandGen, enemyDrawGen
taunt, tauntGen, divineShield, divineShieldGen, windfury, overload
jadeGolemGen, heroPowerGen, secret, secretGen, freezeEnemyGen, discardGen, stealthGen

damageMinionsGen, reachGen, pingGen, aoeGen, destroyGen
deathrattle, deathrattleGen, deathrattleOpponent, silenceOwnGen, battlecry, returnGen
enrageGen, tauntGiverGen, evolveGen, spawnEnemyGen, spellDamageGen, handBuffGen, spellBuffGen
tokenGen, tokenCardGen, comboGen, attackBuffGen, healthBuffGen, heroAttackGen
restoreTargetMinionGen, restoreFriendlyHeroGen, restoreFriendlyMinionGen, armorGen, lifesteal
eggGen, damageFriendlyHeroGen, echo, echoGen, rush, rushGen, magnetic, magneticGen
otherClassGen, silverHandGen, treantGen

//New Synergy Step 12

Double check:
DAMAGE/DESTROY: reachGen, pingGen(enrageSyn), aoeGen(spellDamageSyn/eggSyn), damageMinionsGen, destroyGen
BATTLECRY/COMBO/ECHO/DEATHRATTLE: returnsyn(battlecry/combo/echo), silenceOwnSyn/evolveSyn(deathrattle/malo)
ENRAGE/TAKE DAMAGE: enrageGen(take damage),
SUMMON: tokenGen(summon) <--> =GenMinionHealth1
TOYOURHAND: tokenCardGen(small cards to hand) <--> tokenGen(2+) o spellGen <--> =GenMinionHealth1
PLAY CARDS: tokenCardSyn
BUFF ALL: tokenSyn(beneficio masa), handBuffGen, tauntGiverSyn(cant attack)
COPY ITSELF: handBuffSyn
DESTROY TARDIO: freezeEnemySyn
DESTROY PROPIO: tokenSyn, eggSyn
SWAP/copia 1-1: eggSyn
COSTE/STATS: evolveSyn
SPAWN ENEMIES: spawnEnemyGen

RESTORE: restoreTargetMinionGen o restoreFriendlyMinionGen
RESTORE: restoreTargetMinionGen <--> restoreFriendlyHeroGen
DAMAGE HERO: damageFriendlyHeroGen/damageFriendlyHeroSyn
CHARGE/RUSH: pingGen(atk1) <--> damageMinionsGen(no atk1) <--> reachGen(no atk1/no rush)
STEALTH: stealthGen <--> reachGen(no atk1)
MAGNETIC: magnetic <--> mechAllSyn

DRAW ENEMY/SHUFFLE ENEMY: enemyDrawGen/enemyDrawSyn
HERO ATTACK: heroAttackGen/heroAttackSyn
HERO POWER: heroPowerGen
SPELL BUFF: spellBuffGen/spellBuffSyn
OTHER CLASS: otherClassGen/otherClassSyn
SILVER HAND: silverHandGen/silverHandSyn
TREANT: treantGen/treantSyn



REGLAS
+No hacemos sinergias si requieren 3 cartas, por ejemplo la carta que crea dos 1/1 si tiene un dragon en la mano no es tokenGen, pq necesitariamos 3 cartas,
    la que genera 1/1s, el dragon y el que tiene tokenSyn
+Cartas con tags/synergias condicionales, solo las ponemos si son muy faciles de satisfacer, (Nesting roc si, servant of kalimos no).
    Synergias con todo tu mazo son faciles, como robar 2 murlocs. Synergias JOUST son faciles.
+spell, tokenCard, combo y return son synergias debiles por eso solo las mostramos en un sentido, para evitar mostrarlas continuamente en todos lados.
+tokenCardGen ya implica comboSyn (no hace falta poner comboSyn), eggGen implica (attackBuffSyn y tauntGiverSyn), echo implica toYourHandGen,
    rush implica pingGen/damageMinionsGen
+tokenCardGen Incluye cartas que en conjunto permitan jugar 2+ cartas de coste 0/1/2 las 2 o
    1 carta de coste 0/1 y otra de cualquier coste o 1 carta de coste 0 (no hace falta indicarlo si coste 0).
+toYourHandGen/tokenCardGen: si una carta nos da 1+ carta(s) de coste 0 o 1 es tokenCardGen, si es de mas coste sera toYourHandGen
    (a no ser que el conjunto de cartas que da se acerquen a 4 de mana todas).
+drawGen/toYourHandGen: Pueden incluir un numero al final para indicar que roba mas de 1 carta. El maximo es 5 para evitar indicar
    que un mazo es muy pesado solo por una carta. Para toYourHandGen si nos dan varias cartas a lo largo de varios turnos (como Pyros)
    sumamos el mana de todo lo que nos dan, lo dividimos entre 4 y esa sera el numero the toYourHandGen.
+tokenGen son 2 small minions (max 2/3), somos mas restrictivos si summon en deathrattle (harvest golum no es).
+=attack o =health son para cartas de la mano o del tablero dependiendo de cada tipo
    (atk5 es mano, cost1 es mano, health1 es tablero, health6 es tablero). Puedes ser Minions/Spells
+freezeEnemyGen deben poder usarse sobre enemigos
+pingGen, damageMinionsGen y destroyGen deben ser proactivos, permitimos que sean random pero no deathrattle random
+aoeGen puede ser deathrattle random, quitaremos manualmente excepciones como el tentaculo de n'zoth
+aoeGen: los aoe tienen que afectar al menos 3 objetivos
+aoeGen: no son destroyGen ni damageMinionsGen
+pingGen: tienen como proposito eliminar divineShield y rematar, deben ser baratos en coste.
+pingGen: Todos los bombing/missiles cards son pingGen, si tienen los suficientes misiles aoeGen (cinderstorm).
+spellDamageSyn es para aoe o damage a 2 objetivos
+No incluir sinergias que no sean explicitas, por ejemplo aoe freeze no deberian tener sinergias con otros aoe.
+lifesteal y windfury los ponemos en minion/hechizos/armas pero las synergias solo son con minions
+evolveSyn: suele ponerse en minions que pierdan su valor en el battlecry o que tengan un mal deathrattle.
    Lo ponemos en minions que cuesten 3+ mana de lo que deberian por stats (Sewer Crawler lo aceptamos 1/1 coste 3 por coste bajo (<5))
    o 2+ si tienen reduccion de coste (nerubian prophet, thing from below)
+Si una carta mejora cuando mayor sea nuestra mano no tengo en cuenta como sinergias otras cartas que hagan nuestra mano mayor, es una sinergia muy debil.



IDEAS RECHAZADAS
--void terror synergias -- cant attack minion o twilight summoner (no me convencen)
--devilsaur egg --> evolving spores, and other give evolve effects (no me convencen)
--Twilight Summoner --> tokenGen (no me convence)
--Darkspeaker (swap stats with a friendly minion) --> venom, divineShield, enrage, lifesteal, windfury(no me convencen)
--mirage caller, shadow essence, Prince Taldaram, Barnes, ShadowCaster, Herald Volazj --> deathrattleGood (no deathrattleOpponent, el sistema no lo permite), venom minions, divineshield, stealth, taunt (dudoso)
--buff atk grandes --> windfurySyn (dudoso)
--Vigilar synergias hechizo y eliminar debiles como arcane anomaly (option, no usar list spellSyn, asi las synergias no aparecen en cada hechizo) (TESTING)
--shadowcaster rogue --> synergias, huevo, objetivos evolution (ya es returnGen y deathrattleGoodAllSyn)
--hand buff mech --> con cartas que requieren >5atk (imposible, tri-cards no incluye druida)
--Devilsaur egg --> no tiene beastGen, no queremos ver sinergias que no sean de romper el huevo
--Divine spirit --> healthBuffSyn (no me convence)

--Steam surger --> "spellGen","toYourHandGen" (requiere combo)
--Tol'vir Stoneshaper --> tauntGen, divineShieldGen (requiere combo)
--Book Wyrm --> "damageMinionsGen" (requiere combo)
--Primalfin Lookout --> "discover" (requiere combo)
--Servant of Kalimos --> "discover" (requiere combo)
--Netherpite Historian --> "discover" (requere combo)
--Drakonid Operative --> "discover" (requiere combo)
--Stone Sentinel --> "tauntGen" (requiere combo)
--Kalimos, Primal Lord --> "aoeGen","reachGen","restoreFriendlyHeroGen","tokenGen" (requiere combo)
--Houndmaster --> "tauntGen" (requiere combo)
--Mark of Y'Shaarj --> "drawGen" (requiere combo)
--Starving Buzzard --> "drawGen" (requiere combo)
--Stampede --> "toYourHandGen" (requiere combo)

--Fight promoter --> "drawGen" (requiere combo)
--Malchezaar's Imp --> "drawGen" (requiere combo)
--Violet Teacher --> "tokenGen" (requiere combo), es un combo facil (spells), aun asi es dificil crear mas de 1 token
--Southsea Deckhand --> "damageMinionsGen","reachGen" (requiere combo), solo es valido en rogue
--Furnacefire Colossus --> "discardGen" (requiere combo), los warlock no tienen armas
--Coldwraith --> "drawGen" (requiere combo)
--Medivh's Valet --> "damageMinionsGen", "reachGen" (requiere combo)
--Blackguard --> "damageMinionsGen" (no es removal directo)
--Wind-up Burglebot --> "drawGen (dificil de conseguir)
--Tomb Lurker --> "deathrattle" (No es deathrattle, pero genera cartas deathrattle a tu mano, es el mismo efecto)
--Journey Below --> "deathrattle" (No es deathrattle, pero genera cartas deathrattle a tu mano, es el mismo efecto)
--Doomerang --> "damageMinionsGen" (requiere combo)
--Ice Breaker --> "destroyGen" (requiere combo)
--Far Sight/Mana Bind --> "drawGen" (la carta en si tiene como unico proposito reducir el coste de la robada, por lo que la dos cartas deberian ser consideradas como una sola)
--Burgly Bully --> "spellGen","tokenCardGen" (requiere combo)
--Corpsetaker --> "tauntGen" (si, es muy probable)
--Harvest Golem --> Quitar tokenGen
--Gloutonous Ooze --> Quitar armorGen (poco probable)
--Plague Scientist --> Quitar tokenSyn (es una sinergia debil, envenom podria venir bien en muchos minions como un 1/7)

--Recruit 4 or less --> "=<SynMinionCost4" (crea demasiadas sinergias)


Dudando
++Arcane anomaly --> Quitar spellSyn
*/
