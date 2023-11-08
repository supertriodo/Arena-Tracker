#include "enemyhandhandler.h"
#include "themehandler.h"
#include <QtWidgets>

EnemyHandHandler::EnemyHandHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->inGame = false;
    this->transparency = Opaque;
    this->knownCard = "";
    this->numKnownCards = 0;
    this->lastCreatedByCode = "";
    this->mouseInApp = false;
    this->gettingLinkCards = false;

    completeUI();
}


EnemyHandHandler::~EnemyHandHandler()
{
    ui->enemyHandListWidget->clear();
    enemyHandList.clear();
    linkIdsList.clear();
}


void EnemyHandHandler::reset()
{
    ui->enemyHandListWidget->clear();
    enemyHandList.clear();
    this->lastCreatedByCode = "";
}


void EnemyHandHandler::completeUI()
{
    ui->enemyHandListWidget->setMinimumHeight(0);
    ui->enemyHandListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    ui->enemyHandListWidget->setMouseTracking(true);

    hideHeroAttack();

    connect(ui->enemyHandListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findHandCardEntered(QListWidgetItem*)));
}


void EnemyHandHandler::setLastCreatedByCode(QString code)
{
    if(isLastCreatedByCodeValid(code))      this->lastCreatedByCode = code;
    else                                    emit pDebug("CreateBy code is in the forbidden list: " + code, Warning);
}


void EnemyHandHandler::revealCreatedByCard(QString code, QString createdByCode, int quantity)
{
    emit pDebug("Revealing next " + QString::number(quantity) + " card/s. CreatedBy: " + createdByCode + " as " + code);
    int numHandCards = enemyHandList.count();

    if(numHandCards >= quantity)
    {
        for(int i=0; i<quantity; i++)
        {
            HandCard& card = enemyHandList[numHandCards-1-i];
            if(card.special && card.getCreatedByCode() == createdByCode)
            {
                card.setCode(code);
                card.draw();
                emit revealEnemyCard(card.id, code);
                emit checkCardImage(code);
                emit pDebug("Revealed card. Id: " + QString::number(card.id) + " CreatedBy: " + createdByCode + " as " + code);
            }
        }
    }
}


//Ya no se usa, pero puede ser util en el futuro.
//Sirve para convertir las n siguientes special cards robadas en el codigo especificado
//Se usaba para duplicate, pero ahora las cartas se roban antes de revelarse el secreto
void EnemyHandHandler::convertKnownCard(QString &code, int quantity)
{
    this->knownCard = code;
    this->numKnownCards = quantity;
}


void EnemyHandHandler::linkNextCards()
{
    this->gettingLinkCards = true;
    this->linkIdsList.clear();
}


void EnemyHandHandler::linkCards(HandCard& card)
{
    //Special cards, 2do en adelante
    if(card.special)
    {
        card.linkIdsList = QList<int>(linkIdsList);

        for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++)
        {
            if(linkIdsList.contains(it->id))
            {
                it->linkIdsList.append(card.id);
            }
        }
        linkIdsList.append(card.id);
    }
    else
    {
        //1er carta
        if(linkIdsList.isEmpty())
        {
            linkIdsList.append(card.id);
        }
        //Esta carta no es del link
        else
        {
            gettingLinkCards = false;
            linkIdsList.clear();
        }
    }
}


void EnemyHandHandler::showEnemyCardDraw(int id, int turn, bool special, QString code)
{
    if(isIDinHand(id))    return;

    bool isCreatedByCard = false;

    //Convert Known special cards
    if(!special)
    {
        knownCard.clear();
        numKnownCards = 0;
    }
    else if(code.isEmpty())
    {
        if(numKnownCards>0)
        {
            code = knownCard;
            numKnownCards--;
        }
        else if(!this->lastCreatedByCode.isEmpty())
        {
            isCreatedByCard = true;
        }
    }


    HandCard handCard(code);
    handCard.id = id;
    handCard.turn = turn;
    handCard.special = special;
    handCard.listItem = new QListWidgetItem();
    ui->enemyHandListWidget->addItem(handCard.listItem);

    if(code.isEmpty() && isDrawSpecificCards()) isCreatedByCard = true;

    if(isCreatedByCard)
    {
        handCard.setCreatedByCode(this->lastCreatedByCode);
        emit checkCardImage(this->lastCreatedByCode);
        emit enemyCardDraw(id, code, this->lastCreatedByCode, turn);
    }
    else
    {
        emit enemyCardDraw(id, code, "", turn);
    }

    if(gettingLinkCards)    linkCards(handCard);
    handCard.draw();
    enemyHandList.append(handCard);

    if(code != "")      emit checkCardImage(code);

    //Hide TabBar with 1 HandCards, was shown with 0 HandCards to avoid a full transparent window
    if(enemyHandList.count() == 1)  updateTransparency();
}


bool EnemyHandHandler::isIDinHand(int id)
{
    for(const HandCard &handCard: qAsConst(enemyHandList))
    {
        if(handCard.id == id)   return true;
    }
    return false;
}


HandCard *EnemyHandHandler::getHandCard(int id)
{
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++)
    {
        if(it->id == id)    return &(*it);
    }
    return nullptr;
}


bool EnemyHandHandler::isCoinInHand()
{
    for(HandCard &handCard: enemyHandList)
    {
        if(handCard.getCode() == THE_COIN)  return true;
    }
    return false;
}


void EnemyHandHandler::lastHandCardIsCoin()
{
    if(enemyHandList.empty())   return;//En modo practica el mulligan enemigo termina antes de robar las cartas

    HandCard& coin = enemyHandList.last();
    coin.setCode(THE_COIN);
    coin.draw();

    emit revealEnemyCard(coin.id, THE_COIN);
    emit checkCardImage(THE_COIN);
}


void EnemyHandHandler::hideEnemyCardPlayed(int id, QString code)
{
    if(isClonerCard(code))  linkNextCards();

    int i=0;
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++, i++)
    {
        if(it->id == id)
        {
            if(!it->linkIdsList.isEmpty())     revealLinkedCards(code, it->linkIdsList);

            delete it->listItem;
            enemyHandList.removeAt(i);

            //Show TabBar with 0 HandCards to avoid a full transparent window
            if(enemyHandList.isEmpty())  updateTransparency();

            return;
        }
    }
}


void EnemyHandHandler::revealLinkedCards(QString code, QList<int> &ids)
{
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++)
    {
        if(ids.contains(it->id))
        {
            it->setCode(code);
            it->draw();
            emit revealEnemyCard(it->id, code);
            emit checkCardImage(code);
        }
    }
}


void EnemyHandHandler::redrawDownloadedCardImage(QString &code)
{
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++)
    {
        if(it->getCode() == code || it->getCreatedByCode() == code)    it->draw();
    }
}


void EnemyHandHandler::redrawClassCards()
{
    for(HandCard &handCard: enemyHandList)
    {
        handCard.draw();
    }
}


void EnemyHandHandler::redrawSpellWeaponCards()
{
    for(HandCard &handCard: enemyHandList)
    {
        CardType cardType = handCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            handCard.draw();
        }
    }
}


void EnemyHandHandler::redrawAllCards()
{
    for(HandCard &handCard: enemyHandList)
    {
        handCard.draw();
    }
}


void EnemyHandHandler::lockEnemyInterface()
{
    this->inGame = true;
    resetHeroAttack();
    showHeroAttack();
    updateTransparency();

    reset();
}


void EnemyHandHandler::unlockEnemyInterface()
{
    this->inGame = false;
    hideHeroAttack();
    updateTransparency();
}


void EnemyHandHandler::resetHeroAttack()
{
    drawHeroTotalAttack(true, 0, 0);
    drawHeroTotalAttack(false, 0, 0);
}


void EnemyHandHandler::showHeroAttack()
{
    if(ui->enemyAttackHeroLabel->isHidden() && inGame && showAttackBar)
    {
        ui->tabEnemyLayout->removeItem(ui->horizontalLayoutEnemy);
        ui->tabEnemyLayout->addItem(ui->horizontalLayoutEnemy);
        ui->enemyAttackHeroLabel->setHidden(false);
        ui->enemyAttackRivalLabel->setHidden(false);
        ui->enemyAttackVSLabel->setHidden(false);
    }
}


void EnemyHandHandler::hideHeroAttack()
{
    ui->enemyAttackHeroLabel->setHidden(true);
    ui->enemyAttackRivalLabel->setHidden(true);
    ui->enemyAttackVSLabel->setHidden(true);
    ui->tabEnemyLayout->removeItem(ui->horizontalLayoutEnemy);
}


void EnemyHandHandler::setShowAttackBar(bool value)
{
    this->showAttackBar = value;
    if(value)   showHeroAttack();
    else        hideHeroAttack();
}


void EnemyHandHandler::updateTransparency()
{
    bool inTabEnemy = ui->tabWidget->currentWidget() == ui->tabEnemy;

    if(transparency==Transparent || (!mouseInApp && inGame && transparency==AutoTransparent))
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground);
        ui->tabEnemy->repaint();

        //Tambien nos hacemos cargo en transparency==Transparent para que se llame a MainWindowFade al empezar y terminar un juego
        if(inTabEnemy && (transparency==Transparent || transparency==AutoTransparent))
        {
            if(mouseInApp)      emit needMainWindowFade(false);
            else                emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground, false);
        ui->tabEnemy->repaint();

        //transparency==Transparent nunca llegara hasta aqui
        if(inTabEnemy && transparency==AutoTransparent)
        {
            emit needMainWindowFade(false);
        }
    }
}


void EnemyHandHandler::setTheme()
{
    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(25);
    ui->enemyAttackVSLabel->setFont(font);

    redrawTotalAttack();
}


void EnemyHandHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void EnemyHandHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}


bool EnemyHandHandler::isEmpty()
{
    return enemyHandList.isEmpty();
}


void EnemyHandHandler::findHandCardEntered(QListWidgetItem * item)
{
    HandCard handCard = enemyHandList[ui->enemyHandListWidget->row(item)];
    QString code = handCard.getCode();
    if(code.isEmpty())  code = handCard.getCreatedByCode();

    QRect rectCard = ui->enemyHandListWidget->visualItemRect(item);
    QPoint posCard = ui->enemyHandListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int enemyHandListTop = ui->enemyHandListWidget->mapToGlobal(QPoint(0,0)).y();
    int enemyHandListBottom = ui->enemyHandListWidget->mapToGlobal(QPoint(0,ui->enemyHandListWidget->height())).y();
    emit cardEntered(code, globalRectCard, enemyHandListTop, enemyHandListBottom);
}


void EnemyHandHandler::redrawTotalAttack()
{
    if(!ui->enemyAttackHeroLabel->isHidden())
    {
        drawHeroTotalAttack(true, playerTotalAttack, playerTotalMaxAttack);
    }
    if(!ui->enemyAttackRivalLabel->isHidden())
    {
        drawHeroTotalAttack(false, enemyTotalAttack, enemyTotalMaxAttack);
    }
}


void EnemyHandHandler::drawHeroTotalAttack(bool friendly, int totalAttack, int totalMaxAttack)
{
    int tamAtkHero = static_cast<int>(std::max(35,DeckCard::getCardHeight())*1.7);

    //Font
    QFont font(ThemeHandler::cardsFont());
    font.setPixelSize(static_cast<int>(tamAtkHero/1.5));
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
    font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

    QFontMetrics fm(font);
    QString text;
    if(totalAttack == totalMaxAttack)   text = QString::number(totalAttack);
    else                                text = QString::number(totalAttack) + "/" + QString::number(totalMaxAttack);
    int textWide = fm.width(text);


    int widthCanvas = std::max(tamAtkHero, textWide);
    QPixmap canvas(widthCanvas, tamAtkHero);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Background
        painter.drawPixmap((widthCanvas - tamAtkHero)/2, 0, tamAtkHero, tamAtkHero, QPixmap(ThemeHandler::bgTotalAttackFile()));

        //Text
        painter.setFont(font);
        QPen pen(BLACK);
        pen.setWidth(1);
        painter.setPen(pen);
        if(friendly)    painter.setBrush(GREEN);
        else            painter.setBrush(SOFT_RED);

        Utility::drawShadowText(painter, font, text, widthCanvas/2 - 1, 5*tamAtkHero/8 - 1, true);
    painter.end();

    if(friendly)
    {
        ui->enemyAttackHeroLabel->setPixmap(canvas);
        ui->enemyAttackHeroLabel->setFixedWidth(widthCanvas);
        ui->enemyAttackHeroLabel->setFixedHeight(tamAtkHero);
        this->playerTotalAttack = totalAttack;
        this->playerTotalMaxAttack = totalMaxAttack;
    }
    else
    {
        ui->enemyAttackRivalLabel->setPixmap(canvas);
        ui->enemyAttackRivalLabel->setFixedWidth(widthCanvas);
        ui->enemyAttackRivalLabel->setFixedHeight(tamAtkHero);
        this->enemyTotalAttack = totalAttack;
        this->enemyTotalMaxAttack = totalMaxAttack;
    }
}


void EnemyHandHandler::buffHandCard(int id)
{
    HandCard * handCard = getHandCard(id);

    if(handCard == nullptr)
    {
        emit pDebug("ERROR: HandCard not found for buffing. Id: " + QString::number(id));
        return;
    }

    int cardBuff = getCardBuff(this->lastCreatedByCode);
    handCard->addBuff(cardBuff, cardBuff);
    emit enemyCardBuff(id, cardBuff, cardBuff);
}


void EnemyHandHandler::forgeHandCard(int id)
{
    HandCard * handCard = getHandCard(id);

    if(handCard == nullptr)
    {
        emit pDebug("ERROR: HandCard not found for forge. Id: " + QString::number(id));
        return;
    }

    handCard->forge();
    emit enemyCardForge(id);
}


//Card exceptions
bool EnemyHandHandler::isLastCreatedByCodeValid(const QString &code)
{
    Q_UNUSED(code);
    return true;
}


int EnemyHandHandler::getCardBuff(const QString &code)
{
    if(Utility::codeEqConstant(code, DON_HANCHO))       return 5;
    if(Utility::codeEqConstant(code, STOLEN_GOODS))     return 3;
    if(Utility::codeEqConstant(code, SMUGGLERS_CRATE))  return 2;
    if(Utility::codeEqConstant(code, HIDDEN_CACHE))     return 2;
    if(Utility::codeEqConstant(code, SHAKY_ZYPGUNNER))  return 2;
    if(Utility::codeEqConstant(code, GRIMY_GADGETEER))  return 2;
    return 1;
}


bool EnemyHandHandler::isClonerCard(const QString &code)
{
    QStringList candidates = { MIMIC_POD, THISTLE_TEA, TOLINS_GOBLET, ELEMENTARY_REACTION, GRAND_EMPRESS_SHEKZARA,
        ENCUMBERED_PACK_MULE, DOOR_OF_SHADOWS, SERVICE_BELL, CREATION_PROTOCOL_FORGED
    };
    QString otherCode = Utility::otherCodeConstant(code);
    return candidates.contains(code) || candidates.contains(otherCode);
}


bool EnemyHandHandler::isDrawSpecificCards()
{
    QStringList candidates = { GNOMISH_EXPERIMENTER,FAR_SIGHT,VARIAN_WRYNN,TAELAN_FORDRING,MALYGOS_THE_SPELLWEAVER,
        SENSE_DEMONS,HOLY_WRATH,ICE_FISHING,FORGE_OF_SOULS,HOWLING_COMMANDER,THE_CURATOR,ARCANOLOGIST,BRIGHTEYED_SCOUT,
        TOLVIR_WARDEN,SMALLTIME_RECRUITS,WONDROUS_WAND,RAVEN_FAMILIAR,CAVERN_SHINYFINDER,ELVEN_MINSTREL,CURSED_CASTAWAY,
        BOGSHAPER,TOWN_CRIER,DOLLMASTER_DORIAN,COUNTESS_ASHMORE,SANDBINDER,WITCHWOOD_PIPER,BOOK_OF_SPECTERS,
        SUBJECT_9,STORM_CHASER,DEAD_RINGER,PRISMATIC_LENS,CRYSTOLOGY,JUICY_PSYCHMELON,PREDATORY_INSTINCTS,BWONSAMDI_THE_DEAD,
        MURLOC_TASTYFIN,SPIRIT_OF_THE_FROG,AKALI_THE_RHINO,MASTERS_CALL,RAIDING_PARTY,KINGS_ELEKK,CAPTAINS_PARROT,
        ARCANE_FLETCHER,URSATRON,CALL_TO_ADVENTURE,JEPETTO_JOYBUZZ,SALHETS_PRIDE,ANCIENT_MYSTERIES,
        DIVING_GRYPHON,ANCHARRR,NECRIUM_APOTHECARY,ELEMENTAL_ALLIES,GALADROND_THE_UNBREAKABLE,GALADROND_THE_UNBREAKABLE2,
        GALADROND_THE_UNBREAKABLE3,GALADROND_THE_NIGHTMARE,GALADROND_THE_NIGHTMARE2,GALADROND_THE_NIGHTMARE3,KRONX_DRAGONHOOF,
        TRANSMOGRIFIER,TENTACLED_MENACE,STOWAWAY,
        STARSCRYER,CORSAIR_CACHE,FUNGAL_FORTUNES,SCAVENGERS_INGENUITY,THE_DARK_PORTAL,SKULL_OF_GULDAN,VASHJ_PRIME,
        DOUBLE_JUMP,
        INSIGHT,INSIGHT_CORRUPT,GHUUN_THE_BLOOD_GOD,CLAW_MACHINE,FREE_ADMISSION,REDSCALE_DRAGONTAMER,SWINDLE,
        GRAND_EMPRESS_SHEKZARA,RINGMASTER_WHATLEY,STAGE_DIVE,STAGE_DIVE_CORRUPT,CAGEMATCH_CUSTODIAN,
        PRIMORDIAL_PROTECTOR,TINYFINS_CARAVAN,VENGEFUL_SPIRIT,TUSKPIERCER,LIVING_SEED,REFRESHING_SPRING_WATER,BARAK_KODOBANE,
        KNIGHT_OF_ANOINTMENT,NORTHWATCH_COMMANDER,WARSONG_WRANGLER,
        FROSTWEAVE_DUNGEONEER, CLERIC_OF_ANSHE, SHROUD_OF_CONCEALMENT, DEVOUT_DUNGEONEER, PRIMAL_DUNGEONEER,
        RAID_THE_DOCKS, HARBOR_SCAMP, FELGORGER, VARIAN_KING_OF_STORMWIND, INVESTMENT_OPPORTUNITY, SANCTUM_CHANDLER,
        ALLIANCE_BANNERMAN, SKETCHY_INFORMATION, TWILIGHT_DECEPTOR, SORCERERS_GAMBIT, LEATHERWORKING_KIT,
        HULLBREAKER, DEEPWATER_EVOKER,
        HERALD_OF_LOKHOLAR, DUN_BALDAR_BUNKER, VITALITY_SURGE, FELFIRE_IN_THE_HOLE, BALINDA_STONEHEARTH,
        CAPTURE_COLDTOOTH_MINE, SPIRIT_GUIDE, STONEHEARTH_VINDICATOR, AXE_BERSERKER, SMOKESCREEN, HORN_OF_WRATHION,
        GORLOC_RAVAGER, SEAFLOOR_GATEWAY, CONCHS_CALL, RADAR_DETECTOR, CUTLASS_COURIER, ABYSSAL_DEPTHS, TRENCH_SURVEYOR,
        AQUATIC_FORM, SWITCHEROO, CRUSHCLAW_ENFORCER, GONE_FISHIN, FOSSIL_FANATIC,
        DEATH_BLOSSOM_WHOMPER, NIGHTSHADE_BUD, SESSELIE_OF_THE_FAE_COURT, WIDOWBLOOM_SEEDSMAN, RELIC_OF_DIMENSIONS,
        CONQUERORS_BANNER, DOOR_OF_SHADOWS, SERVICE_BELL, ORDER_IN_THE_COURT, WEAPONS_EXPERT,
        FLESH_BEHEMOTH, ARCANE_QUIVER, RUNEFORGING, LAST_STAND, PRESCIENCE, HARBINGER_OF_WINTER, OVERSEER_FRIGIDARA,
        MARK_OF_SCORN, THE_PURATOR, TRINKET_TRACKER,
        RUSH_THE_STAGE, GUITAR_SOLOIST, VOLUME_UP, CHORUS_RIFF, PEACEFUL_PIPER, SUMMER_FLOWERCHILD, INSTRUMENT_TECH,
        METROGNOME, BANJOSAUR, COSTUMED_SINGER, MAGATHA_BANE_OF_MUSIC,
        STEAM_GUARDIAN, LOKEN_JAILER_OF_YOGGSARON, SHARPEYED_SEEKER, TRAM_OPERATOR, PIT_STOP, EMBRACE_OF_NATURE,
        PEBBLY_PAGE, ALAKIR_THE_WINDS_OF_TIME, DARKBOMB2, STOLEN_GOODS2, TIMELINE_ACCELERATOR, ALARMED_SECURITYBOT,
        TRUSTY_COMPANION, FRACKING, CACTUS_CUTTER, HI_HO_SILVERWING, CATTLE_RUSTLER, BLINDEYE_SHARPSHOOTER, BARTEND_O_BOT,
        STARGAZING, MESADUNE_THE_FRACTURED, MESSENGER_BUZZARD, TEN_GALLON_HAT, TAKE_TO_THE_SKIES
    };

    QString otherCode = Utility::otherCodeConstant(lastCreatedByCode);
    return candidates.contains(lastCreatedByCode) || candidates.contains(otherCode);
}

