#include "synergyhandler.h"
#include "qjsondocument.h"
#include "qthread.h"
#include "themehandler.h"
#include "QDesktopServices"
#include <QDebug>
#include "Synergies/keysynergies.h"
#include "Synergies/layeredsynergies.h"
#include "Synergies/cardtypecounter.h"
#include "Synergies/racecounter.h"
#include "Synergies/schoolcounter.h"
#include "Synergies/mechaniccounter.h"
#include "Synergies/statsynergies.h"

SynergyHandler::SynergyHandler(QObject *parent, Ui::Extended *ui, LavaButton *lavaButton) : QObject(parent)
{
    this->ui = ui;

    createDraftItemCounters(lavaButton);
}


SynergyHandler::~SynergyHandler()
{
}


void SynergyHandler::connectCounters(QMap<QString, DraftDropCounter*> * dropCounters,
                                     QMap<QString, DraftItemCounter*> * cardTypeCounters,
                                     QMap<QString, DraftItemCounter*> * mechanicCounters,
                                     LavaButton *lavaButton)
{
    const auto cardTypesKeys = CardTypeCounter::getListKeyLabels();
    for(const auto &key: cardTypesKeys)
    {
        connect((*cardTypeCounters)[key], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
                this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
        connect((*cardTypeCounters)[key], SIGNAL(iconLeave()),
                this, SIGNAL(itemLeave()));
    }

    const auto dropKeys = DraftDropCounter::getListKeyLabels();
    for(const auto &key: dropKeys)
    {
        connect((*dropCounters)[key], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
                this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
        connect((*dropCounters)[key], SIGNAL(iconLeave()),
                this, SIGNAL(itemLeave()));
    }

    const auto mechanicKeys = MechanicCounter::getListKeyLabels();
    for(const auto &key: mechanicKeys)
    {
        connect((*mechanicCounters)[key], SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
                this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
        connect((*mechanicCounters)[key], SIGNAL(iconLeave()),
                this, SIGNAL(itemLeave()));
    }

    connect(lavaButton, SIGNAL(iconEnter(QList<SynergyCard>&,QRect&)),
            this, SLOT(sendItemEnter(QList<SynergyCard>&,QRect&)));
    connect(lavaButton, SIGNAL(iconLeave()),
            this, SIGNAL(itemLeave()));
}


void SynergyHandler::createDraftItemCounters(LavaButton *lavaButton)
{
    QGridLayout *mechanicsLayout = new QGridLayout();

    QMap<QString, DraftItemCounter*> * cardTypeCounters = CardTypeCounter::createCardTypeCounters(this, mechanicsLayout);

    manaCounter = new DraftItemCounter(this, "Mana AVG", "Mana AVG", mechanicsLayout, 0, 3,
                                       QPixmap(ThemeHandler::manaCounterFile()), 32, false, false);

    QMap<QString, DraftDropCounter*> * dropCounters = DraftDropCounter::createDropCounters(this, mechanicsLayout);
    QMap<QString, DraftItemCounter*> * mechanicCounters = MechanicCounter::createMechanicCounters(this, mechanicsLayout, lavaButton);
    RaceCounter::createRaceCounters(this);
    SchoolCounter::createSchoolCounters(this);
    KeySynergies::createKeySynergies();

    connectCounters(dropCounters, cardTypeCounters, mechanicCounters, lavaButton);

    CardTypeCounter::setSynergyCodes(&synergyCodes);
    DraftDropCounter::setSynergyCodes(&synergyCodes);
    RaceCounter::setSynergyCodes(&synergyCodes);
    SchoolCounter::setSynergyCodes(&synergyCodes);
    MechanicCounter::setSynergyCodes(&synergyCodes);
    KeySynergies::setSynergyCodes(&synergyCodes);
    StatSynergies::setSynergyCodes(&synergyCodes);
    LayeredSynergies::setSynergyCodes(&synergyCodes);

    QHBoxLayout *horLayoutMechanics = new QHBoxLayout();
    horLayoutMechanics->addLayout(mechanicsLayout);
    horLayoutMechanics->addStretch();
    ui->draftVerticalLayout->addLayout(horLayoutMechanics);
}


void SynergyHandler::setTheme()
{
    CardTypeCounter::setTheme();
    MechanicCounter::setTheme();
    manaCounter->setTheme(QPixmap(ThemeHandler::manaCounterFile()), 32, false);
    DraftDropCounter::setTheme();
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
    coreCodes << Utility::getSetCodes("PLACEHOLDER_202204", true, true);

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
    DraftDropCounter::resetAll();
    KeySynergies::resetAll();
    StatSynergies::resetAll();
    LayeredSynergies::reset();
}


void SynergyHandler::setHidden(bool hide)
{
    CardTypeCounter::setHidden(hide);
    MechanicCounter::setHidden(hide);
    DraftDropCounter::setHidden(hide);
    if(hide)
    {
        manaCounter->hide();
    }
    else
    {
        manaCounter->show();
    }
}


void SynergyHandler::setTransparency(Transparency transparency, bool mouseInApp)
{
    manaCounter->setTransparency(transparency, mouseInApp);
    CardTypeCounter::setTransparency(transparency, mouseInApp);
    MechanicCounter::setTransparency(transparency, mouseInApp);
    DraftDropCounter::setTransparency(transparency, mouseInApp);
}


int SynergyHandler::getCounters(
        QMap<QString, QString> &spellMap, QMap<QString, QString> &minionMap, QMap<QString, QString> &weaponMap,
        QMap<QString, QString> &drop2Map, QMap<QString, QString> &drop3Map, QMap<QString, QString> &drop4Map,
        QMap<QString, QString> &aoeMap, QMap<QString, QString> &tauntMap,
        QMap<QString, QString> &survivabilityMap, QMap<QString, QString> &drawMap,
        QMap<QString, QString> &pingMap, QMap<QString, QString> &damageMap,
        QMap<QString, QString> &destroyMap, QMap<QString, QString> &reachMap,
        QList<SynergyWeightCard> &synergyWeightCardList)
{
    CardTypeCounter::getCardTypeCounters(spellMap, minionMap, weaponMap);
    MechanicCounter::getMechanicCounters(aoeMap, tauntMap, survivabilityMap, drawMap,
                                         pingMap, damageMap, destroyMap, reachMap,
                                         synergyWeightCardList);
    DraftDropCounter::getDropCounters(drop2Map, drop3Map, drop4Map);

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
        QList<SynergyWeightCard> &synergyWeightCardList)
{
    QString code = deckCard.getCode();
    QString name = Utility::cardEnNameFromCode(code);
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();
    int cost = deckCard.getCost();
    QList<CardRace> cardRace = deckCard.getRace();
    CardSchool cardSchool = deckCard.getSchool();

    DraftDropCounter::updateDropCounters(code, drop2Map, drop3Map, drop4Map, attack, health, cost);
    CardTypeCounter::updateCardTypeCounters(code, spellMap, minionMap, weaponMap, text, cardType);
    RaceCounter::updateRaceCounters(code, mechanics, text, cardRace);
    SchoolCounter::updateSchoolCounters(code, text, cardSchool);
    MechanicCounter::updateMechanicCounters(code, aoeMap, tauntMap, survivabilityMap, drawMap,
                                            pingMap, damageMap, destroyMap, reachMap,
                                            synergyWeightCardList,
                                            mechanics, referencedTags, text, cardType, attack, cost);
    KeySynergies::updateKeySynergies(code, mechanics, referencedTags, name, text, cardType, attack, cost);
    StatSynergies::updateStatsSynergies(code, cardType, attack, health, cost);
    LayeredSynergies::updateLayeredSynergies(code);
    updateManaCounter(code, cost);
}


void SynergyHandler::updateManaCounter(const QString &code, int cost)
{
    manaCounter->increase(Utility::getCorrectedCardMana(code, cost), CardTypeCounter::draftedCardsCount());
}


void SynergyHandler::getSynergies(DeckCard &deckCard, QMap<QString, QMap<QString, int>> &synergyTagMap,
                                  QMap<MechanicIcons, int> &mechanicIcons, MechanicBorderColor &dropBorderColor)
{
    QString code = deckCard.getCode();
    QString name = Utility::cardEnNameFromCode(code);
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
    QString text = Utility::cardEnTextFromCode(code).toLower();
    CardType cardType = deckCard.getType();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();
    int cost = deckCard.getCost();
    QList<CardRace> cardRace = deckCard.getRace();
    CardSchool cardSchool = deckCard.getSchool();


    DraftDropCounter::getDropMechanicIcons(code, mechanicIcons, dropBorderColor, attack, health, cost);
    CardTypeCounter::getCardTypeSynergies(code, synergyTagMap, text, cardType);
    RaceCounter::getRaceSynergies(code, synergyTagMap, mechanics, text, cardRace);
    SchoolCounter::getSchoolSynergies(code, synergyTagMap, text, cardSchool);
    MechanicCounter::getMechanicSynergies(code, synergyTagMap, mechanicIcons, mechanics, referencedTags, text, cardType, attack, cost);
    KeySynergies::getKeySynergies(code, synergyTagMap, mechanics, referencedTags, name, text, cardType, attack, cost);
    StatSynergies::getStatsSynergies(code, synergyTagMap, cardType, attack, health, cost);
    LayeredSynergies::getLayeredSynergies(code, synergyTagMap, mechanics, referencedTags, name, text, cardType, cardRace, cardSchool, attack, health, cost);
    CardTypeCounter::getDirectLinkSynergies(code, directLinks, synergyTagMap["Extra"]);
}


bool SynergyHandler::isValidSynergyCode(const QString &mechanic, QRegularExpressionMatch *match)
{
    static const auto regex = QRegularExpression("^=[<>]?(Syn|Gen)(Minion|Spell|Weapon)(Cost|Attack|Health)[0-9]{1,2}$");
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
        const QStringList layeredSynergy = mechanic.split('&', QString::SkipEmptyParts);
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

    if(CardTypeCounter::getListValidSynergies().contains(mechanic))     return true;
    if(DraftDropCounter::getListValidSynergies().contains(mechanic))    return true;
    if(RaceCounter::getListValidSynergies().contains(mechanic))         return true;
    if(SchoolCounter::getListValidSynergies().contains(mechanic))       return true;
    if(MechanicCounter::getListValidSynergies().contains(mechanic))     return true;
    if(KeySynergies::getListValidSynergies().contains(mechanic))        return true;

    return false;
}


void SynergyHandler::testSynergies(const QString &miniSet)
{
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, true);
    int num = 0;

    // for(QString &code: arenaCodes)
    for(QString &code: (QStringList)Utility::getSetCodes("THE_LOST_CITY", true, false))
    // for(QString &code: (QStringList)Utility::getSetCodesSpecific("TREASURES"))
    // for(QString &code: (QStringList)Utility::getStandardCodes())
    // for(QString &code: (QStringList)Utility::getWildCodes())
    {
        if(code.startsWith("VAN_"))     continue;
        if(code.startsWith("CORE_"))    code = code.mid(5);

        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            CardType cardType = Utility::getTypeFromCode(code);
            CardRarity rarity = Utility::getRarityFromCode(code);
            QString text = Utility::cardEnTextFromCode(code).toLower();
            QString name = Utility::cardEnNameFromCode(code).toLower();
            int attack = Utility::getCardAttribute(code, "attack").toInt();
            int health = Utility::getCardAttribute(code, "health").toInt();
            int cost = Utility::getCardAttribute(code, "cost").toInt();
            QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
            QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();
            bool showImages = false;
            // bool showImages = true;
            if(
                text.contains("imp") || text.contains("Imp")
                // KeySynergies::containsAll(text, "deathrattle attack")
                // synergyCodes[code].isEmpty() && rarity == LEGENDARY
                // KeySynergies::isKey("lifesteal", code, mechanics, referencedTags, text, cardType, attack, cost)
                // KeySynergies::isKeyGen("lifestealGen", code, mechanics, referencedTags, text, cardType, attack, cost)
                // KeySynergies::isKeySyn("lifestealSyn", code, mechanics, referencedTags, text, cardType, attack, cost)
                // KeySynergies::isKeyAllSyn("lifestealAllSyn", code, mechanics, referencedTags, text, cardType, attack, cost)
//                mechanics.contains(QJsonValue("MAGNETIC"))
//                referencedTags.contains(QJsonValue("CHOOSE_ONE"))
               // && cardType != MINION
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
// MechanicCounter::isDrawGen(code, text) || KeySynergies::containsAll(text, "draw")
///Update cartas que roban una carta y la clonan (Mimic Pod) --> EnemyHandHandler::isClonerCard (Hearthpwn Search: draw cop)
// KeySynergies::containsAll(text, "draw cop")
///Update AOE que marcan un objetivo principal y le hacen algo diferente que al resto (Swipe) --> MinionGraphicsItem::isAoeWithTarget
// MechanicCounter::isAoeGen(code, text)
                )
            {
                debugSynergiesCode(code, ++num);
                // qDebug()<<++num<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],"<<"-->"<<text;
                // qDebug()<<code+" "+Utility::cardEnNameFromCode(code);
                // qDebug()<<code;
                // qDebug()<<mechanics<<endl<<referencedTags;

                if(showImages && num>0 && num<=50)
                // if(showImages && num>50 && num<=100)
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

    qDebug()<<"\n-----Synergies.json-----\n";
    int num = 0;
    QMap<QString, QStringList> synergiesMap;
    for(const QString &code: codeList)
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            QStringList mec = debugSynergiesCode(code, ++num);
            synergiesMap.insert(code, mec);

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

    qDebug()<<"\n-----SynergiesNames.json-----\n";
    for(const QString &code: codeList)
    {
        if(miniSet.isEmpty() || code.startsWith(miniSet))
        {
            qDebug()<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
        }
    }

    saveSynergiesSetJson(synergiesMap);
    clearSynergyLists();
}


void SynergyHandler::saveSynergiesSetJson(QMap<QString, QStringList> &synergiesMap)
{
    QFile file("/home/triodo/modelo/synergiesSet.json");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Cannot create:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    bool first = true;
    for(const auto &code: synergiesMap.keys())
    {
        QStringList mecs = synergiesMap[code];
        if(first)
        {
            out << "{\n";
            first = false;
        }
        else    out << ",\n";
        QString dq = mecs.isEmpty()?"":"\"";
        out << QStringLiteral("\"%1\" : [ %3%2%3 ]").arg(code, mecs.join("\", \""), dq);
    }
    out << "\n}";
}


bool SynergyHandler::shouldBeInSynergies(const QString &code)
{
    const QString &set = Utility::getCardAttribute(code, "set").toString();
    return (!code.startsWith("HERO_") && !set.isEmpty() &&
            set != "WILD_EVENT" && set != "VANILLA");
}


void SynergyHandler::debugMissingSynergies(bool onlyArena, bool showCards)
{
    QStringList arenaCodes = Utility::getAllArenaCodes();
    initSynergyCodes(arenaCodes, !onlyArena);

    int num = 0;
    QMap<QString, QStringList> synergiesMap;
    const QStringList wildCodes = Utility::getWildCodes();
    const QStringList &codes = onlyArena?arenaCodes:wildCodes;
    const QStringList coreCodes = Utility::getSetCodes("CORE", true, true);
    for(const QString &code: codes)
    {
        //Missing synergy
        bool codeMissing = !synergyCodes.contains(code);
        if(codeMissing)
        {
            if(shouldBeInSynergies(code))
            {
                if(num == 0)    qDebug()<<"\n-----Synergies.json-----\n";
                QStringList mec = debugSynergiesCode(code, ++num);
                synergiesMap.insert(code, mec);

               if(showCards && num>0 && num<=50)
               {
                   QDesktopServices::openUrl(QUrl(
                       "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
                       ));
                   QThread::msleep(100);
               }
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
            if(!invalidMecs.isEmpty())  qDebug()<<"<<<<<DEBUG SYNERGIES INVALID: Code:"<<code<<"No mecs:"<<invalidMecs;
            delete match;
        }
    }
    if(num == 0)
    {
        qDebug()<<"DEBUG SYNERGIES: OK - No missing synergies.";
    }
    else
    {
        qDebug()<<"\n-----SynergiesNames.json-----\n";
        for(const QString &code: synergiesMap.keys())
        {
            qDebug()<<code<<": ["<<Utility::cardEnNameFromCode(code)<<"],";
        }

        saveSynergiesSetJson(synergiesMap);
        qDebug()<<"\n<<<<<DEBUG SYNERGIES: Those synergies missing.";
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
        qDebug()<<"<<<<<DEBUG SYNERGIES: Those synergies to be removed.";
    }

    clearSynergyLists();
}


QStringList SynergyHandler::debugSynergiesCode(QString code, int num)
{
    QString origCode = code;
    if(code.startsWith("CORE_"))
    {
        QString subCode = code.mid(5);
        if(!synergyCodes.contains(code) && synergyCodes.contains(subCode))  code = subCode;
    }

    QStringList mec;
    QString name = Utility::cardEnNameFromCode(origCode);
    CardType cardType = Utility::getTypeFromCode(code);
    QString text = Utility::cardEnTextFromCode(code).toLower();
    int attack = Utility::getCardAttribute(code, "attack").toInt();
    int health = Utility::getCardAttribute(code, "health").toInt();
    int cost = Utility::getCardAttribute(code, "cost").toInt();
    QJsonArray mechanics = Utility::getCardAttribute(code, "mechanics").toArray();
    QJsonArray referencedTags = Utility::getCardAttribute(code, "referencedTags").toArray();

    mec << DraftDropCounter::debugDropSynergies(code, attack, health, cost);
    mec << CardTypeCounter::debugCardTypeSynergies(code, text);
    mec << RaceCounter::debugRaceSynergies(code, mechanics, text);
    mec << SchoolCounter::debugSchoolSynergies(code, text);
    mec << MechanicCounter::debugMechanicSynergies(code, mechanics, referencedTags, text, cardType, attack, cost);
    mec << KeySynergies::debugKeySynergies(code, mechanics, referencedTags, name, text, cardType, attack, cost);


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
    return mec;
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
        int cost = Utility::getCardAttribute(code, "cost").toInt();

        //>0.2 drop
        //0.2 - 0.15 duda
        //<0.15 no drop
        // if(cost == 1)
        if(cost == 2)
        // if(cost == 3)
        // if(cost == 4)
        {
            int played = statsMap[code].value("played").toInt();
            bool drop2 = DraftDropCounter::isDrop2(code);
            bool drop3 = DraftDropCounter::isDrop3(code);
            bool drop4 = DraftDropCounter::isDrop4(code);
            bool drop = drop2 || drop3 || drop4;

            if(dropRatio < 0.2 && drop)//Drop bajos, por debajo de 0.15 solo dejar drops perfectos (buenos stats) a no ser que su efecto sea tan potente que se desperdiciaria en drop.
            // if(dropRatio > 0.15 && !drop)//Drop altos
            {
                if((num%50 == 0))   qDebug()<<"\n";

                num++;
                if(num>0 && num<=50)
                // if(num>50 && num<=100)
                // if(num>100 && num<=150)
                {
                    // QDesktopServices::openUrl(QUrl(
                    //     "https://art.hearthstonejson.com/v1/render/latest/enUS/512x/" + code + ".png"
                    //     ));
                    // QThread::msleep(100);
                }
            }

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
        }
    }

    clearSynergyLists();
}


int SynergyHandler::getCorrectedCardMana(DeckCard &deckCard)
{
    return Utility::getCorrectedCardMana(deckCard.getCode(), deckCard.getCost());
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
DAMAGE/DESTROY: reach(no atk1), aoe 3+(spellDamageSyn/eggAllSyn),
                ping(enrageSyn), damageMinions, destroy(8+ damage/no rush)
BATTLECRY/COMBO/ECHO/DEATHRATTLE: returnsyn(battlecry/choose one/combo/echo),
                silenceOwnSyn/evolveSyn(deathrattle/malo)
ENRAGE/FRENZY/TAKE DAMAGE: enrage(take damage)/rushGiverSyn
RUSHGIVERSYN: enrage/frenzy, poison, damage adjacents
SUMMON: token 2+(3/3)max
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
Imp: imp/impSyn

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
+windfuryMinion y deathrattle son solo para minions.
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

