#include "enemyhandhandler.h"
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

    ui->enemyAttackHeroLabel->setFixedHeight(TAM_ATK_HERO);
    ui->enemyAttackRivalLabel->setFixedHeight(TAM_ATK_HERO);
    QFont font("Belwe Bd BT");
    font.setPixelSize(25);
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
    font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif
    ui->enemyAttackVSLabel->setFont(font);
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
    foreach(HandCard handCard, enemyHandList)
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
    return NULL;
}


void EnemyHandHandler::lastHandCardIsCoin()
{
    if(enemyHandList.empty())   return;//En modo practica el mulligan enemigo termina antes de robar las cartas

    HandCard& coin = enemyHandList.last();
    coin.setCode(COIN);
    coin.draw();

    emit revealEnemyCard(coin.id, COIN);
    emit checkCardImage(COIN);
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
    foreach(HandCard handCard, enemyHandList)
    {
        if(handCard.getCardClass()<9)
        {
            handCard.draw();
        }
    }
}


void EnemyHandHandler::redrawSpellWeaponCards()
{
    foreach(HandCard handCard, enemyHandList)
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
    foreach(HandCard handCard, enemyHandList)
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

    if(!mouseInApp && (transparency==Transparent || (inGame && transparency==AutoTransparent)))
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground);
        ui->tabEnemy->repaint();

        //Tambien nos hacemos cargo en transparency==Transparent para que se llame a MainWindowFade al empezar y terminar un juego
        if(inTabEnemy && (transparency == Transparent || transparency == AutoTransparent))
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground, false);
        ui->tabEnemy->repaint();

        if(inTabEnemy && transparency==AutoTransparent)
        {
            emit needMainWindowFade(false);
        }
    }
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


void EnemyHandHandler::drawHeroTotalAttack(bool friendly, int totalAttack, int totalMaxAttack)
{
    //Font
    QFont font("Belwe Bd BT");
    font.setPixelSize(TAM_ATK_HERO/2.0);
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
    int textHigh = fm.height();


    int widthCanvas = std::max(TAM_ATK_HERO, textWide);
    QPixmap canvas(widthCanvas, TAM_ATK_HERO);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Background
        painter.drawPixmap((widthCanvas - TAM_ATK_HERO)/2, 0, TAM_ATK_HERO, TAM_ATK_HERO, QPixmap(":Images/bgTotalAttack.png"));

        //Text
        painter.setFont(font);
        QPen pen(BLACK);
        pen.setWidth(1);
        painter.setPen(pen);
        if(friendly)    painter.setBrush(GREEN);
        else            painter.setBrush(SOFT_RED);

        QPainterPath path;
        path.addText(widthCanvas/2 - textWide/2 - 1, 5*TAM_ATK_HERO/8 + textHigh/4 - 1 , font, text);
        painter.drawPath(path);
    painter.end();

    if(friendly)
    {
        ui->enemyAttackHeroLabel->setPixmap(canvas);
        ui->enemyAttackHeroLabel->setFixedWidth(widthCanvas);
    }
    else
    {
        ui->enemyAttackRivalLabel->setPixmap(canvas);
        ui->enemyAttackRivalLabel->setFixedWidth(widthCanvas);
    }
}


void EnemyHandHandler::buffHandCard(int id)
{
    HandCard * handCard = getHandCard(id);

    if(handCard == NULL)
    {
        emit pDebug("ERROR: HandCard not found for buffing. Id: " + QString::number(id));
        return;
    }

    int cardBuff = getCardBuff(this->lastCreatedByCode);
    handCard->addBuff(cardBuff, cardBuff);
    emit enemyCardBuff(id, cardBuff, cardBuff);
}


//Card exceptions
bool EnemyHandHandler::isLastCreatedByCodeValid(QString code)
{
    if(code == DARKSHIRE_COUNCILMAN)    return false;
    if(code == FLAMEWAKER)              return false;
    return true;
}


int EnemyHandHandler::getCardBuff(QString code)
{
    if(code == DON_HANCHO)          return 5;
    if(code == STOLEN_GOODS)        return 3;
    if(code == SMUGGLERS_CRATE)     return 2;
    if(code == HIDDEN_CACHE)        return 2;
    if(code == SHAKY_ZYPGUNNER)     return 2;
    if(code == GRIMY_GADGETEER)     return 2;
    return 1;
}


bool EnemyHandHandler::isClonerCard(QString code)
{
    if(code == MIMIC_POD)   return true;
    if(code == THISTLE_TEA) return true;
    return false;
}

