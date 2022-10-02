#include "cardgraphicsitem.h"
#include "herographicsitem.h"
#include "../../utility.h"
#include "../../themehandler.h"
#include <QtWidgets>


CardGraphicsItem::CardGraphicsItem( int id, QString code, QString createdByCode, int turn, bool friendly,
                                    GraphicsItemSender *graphicsItemSender, QStringList mechanics)
{
    this->code = code;
    this->createdByCode = createdByCode;
    this->id = id;
    this->cost = this->origCost = Utility::getCardAttribute(code, "cost").toInt();
    this->attack = this->origAttack = Utility::getCardAttribute(code, "attack").toInt();
    this->health = this->origHealth = Utility::getCardAttribute(code, "health").toInt();
    this->buffAttack = this->buffHealth = 0;
    this->played = this->discard = this->draw = false;
    this->heightShow = HEIGHT;
    this->turn = turn;
    this->friendly = friendly;
    this->showTransparent = false;
    this->mechanics = mechanics;
    this->graphicsItemSender = graphicsItemSender;
    friendly?this->setZValue(-10):this->setZValue(-30);
    setAcceptHoverEvents(true);
}


CardGraphicsItem::CardGraphicsItem(CardGraphicsItem *copy, bool showMechanics)
{
    this->code = copy->code;
    this->createdByCode = copy->createdByCode;
    this->id = copy->id;
    this->cost = copy->cost;
    this->origCost = copy->origCost;
    this->attack = copy->attack;
    this->origAttack = copy->origAttack;
    this->health = copy->health;
    this->origHealth = copy->origHealth;
    this->buffAttack = copy->buffAttack;
    this->buffHealth = copy->buffHealth;
    this->played = copy->played;
    this->discard = copy->discard;
    this->draw = copy->draw;
    this->heightShow = copy->heightShow;
    this->turn = copy->turn;
    this->friendly = copy->friendly;
    this->showTransparent = false;
    if(showMechanics)   this->mechanics = copy->mechanics;
    this->graphicsItemSender = copy->graphicsItemSender;
    this->setPos(copy->pos());
    this->setZValue(copy->zValue());
    setAcceptHoverEvents(true);
}


void CardGraphicsItem::changeCode(QString newCode)
{
    this->code = newCode;
    this->cost = this->origCost = Utility::getCardAttribute(code, "cost").toInt();
    this->attack = this->origAttack = Utility::getCardAttribute(code, "attack").toInt();
    this->health = this->origHealth = Utility::getCardAttribute(code, "health").toInt();
    this->buffAttack = this->buffHealth = 0;
    update();
}


int CardGraphicsItem::getId()
{
    return this->id;
}


QString CardGraphicsItem::getCode()
{
    return code;
}


QString CardGraphicsItem::getCreatedByCode()
{
    return createdByCode;
}


bool CardGraphicsItem::isDiscard()
{
    return this->discard;
}


bool CardGraphicsItem::isPlayed()
{
    return this->played;
}


bool CardGraphicsItem::isDraw()
{
    return this->draw;
}


void CardGraphicsItem::setPlayed(bool played)
{
    this->played = played;
    update();
}


void CardGraphicsItem::togglePlayed()
{
    setPlayed(!this->played);
}


void CardGraphicsItem::setDiscard()
{
    this->discard = true;
    update();
}


void CardGraphicsItem::setDraw(bool drawn)
{
    this->draw = drawn;
    update();
}


void CardGraphicsItem::reduceCost(int cost)
{
    if(!played && this->cost > cost)
    {
        this->cost = cost;
        update();
    }
}


void CardGraphicsItem::addBuff(int addAttack, int addHealth)
{
    buffAttack += addAttack;
    buffHealth += addHealth;
    update();
}


void CardGraphicsItem::showManaPlayable(int mana)
{
    if(cost > mana)     this->showTransparent = true;
    else                this->showTransparent = false;
    update();
}


void CardGraphicsItem::setShowTransparent(bool value)
{
    if(showTransparent != value)
    {
        showTransparent = value;
        update();
    }
}


bool CardGraphicsItem::isTransparent()
{
    return showTransparent;
}


void CardGraphicsItem::checkDownloadedCode(QString code)
{
    if(this->code == code || this->createdByCode == code)  update();
}


QRectF CardGraphicsItem::boundingRect() const
{
    //Aumentamos 5px por la derecha para incluir el glow
    return QRectF( -WIDTH/2, -heightShow/2-CARD_LIFT, WIDTH + 5, heightShow+CARD_LIFT);
}


void CardGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    QRectF boundRect = boundingRect();
    graphicsItemSender->sendPlanCardEntered((!code.isEmpty()?code:createdByCode),
                        mapToScene(boundRect.topLeft()).toPoint(),
                        mapToScene(boundRect.bottomRight()).toPoint());
    if(friendly)    graphicsItemSender->sendCheckBomb(code);
}


void CardGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    graphicsItemSender->sendPlanCardLeave();
    if(friendly)    graphicsItemSender->sendResetDeadProbs();
}


void CardGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    graphicsItemSender->cardPress(this, event->button());
}


void CardGraphicsItem::setZonePos(bool friendly, int pos, int cardsZone, int viewWidth, int cardHeightShow)
{
    if(cardHeightShow > HEIGHT)     cardHeightShow = HEIGHT;
    const int hMinion = MinionGraphicsItem::HEIGHT-5;
    const int hHero = HeroGraphicsItem::HEIGHT;
    this->heightShow = cardHeightShow;
    viewWidth -= WIDTH+8 - viewWidth/cardsZone;
    const int wCard = std::min(175, viewWidth/cardsZone);
    int x = static_cast<int>(wCard*(pos - (cardsZone-1)/2.0));
    int y = friendly?hMinion + hHero + heightShow/2:-hMinion - hHero - heightShow/2;
    this->setPos(x, y);
}


void CardGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"CARD TAG CHANGE -->"<<id<<tag<<value;

    if(tag == "COST")
    {
        this->cost = value.toInt();
    }
    else if(tag == "ATK")
    {
        this->attack = value.toInt();
    }
    else if(tag == "HEALTH")
    {
        this->health = value.toInt();
    }
    update();
}


void CardGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    if(showTransparent) painter->setOpacity(0.2);

    bool cardLifted = (played||discard)&&!draw;
    if(played)          painter->drawPixmap(-WIDTH/2, -heightShow/2-CARD_LIFT, QPixmap(":/Images/bgCardGlow.png"), 0, 0, 190, heightShow+CARD_LIFT);
    else if(discard)    painter->drawPixmap(-WIDTH/2, -heightShow/2-CARD_LIFT, QPixmap(":/Images/bgCardDiscard.png"), 0, 0, 190, heightShow+CARD_LIFT);
    if(draw)            painter->drawPixmap(-WIDTH/2, -heightShow/2, QPixmap(":/Images/bgCardDraw.png"), 0, 0, 190, heightShow);

    if(!code.isEmpty())
    {
        painter->drawPixmap(-WIDTH/2, -heightShow/2+(cardLifted?-CARD_LIFT:0),
                            QPixmap(Utility::hscardsPath() + "/" + code + ".png"), 5, 34, WIDTH,
                            heightShow+(cardLifted?CARD_LIFT:0));

        if(mechanics.count() > 0)
        {
            painter->drawPixmap(-WIDTH/2+11, -heightShow/2+(cardLifted?-CARD_LIFT:0)+55, 30, 30,
                                QPixmap(":/Images/" + mechanics[0]));
            if(mechanics.count() > 1)
            {
                painter->drawPixmap(-WIDTH/2+11, -heightShow/2+(cardLifted?-CARD_LIFT:0)+90, 30, 30,
                                    QPixmap(":/Images/" + mechanics[1]));
            }
        }

        if(cost != origCost)
        {
            painter->drawPixmap(-WIDTH/2 + 4, -heightShow/2+(cardLifted?-CARD_LIFT:0) + 5, 45, 45,
                            QPixmap(":/Images/bgCrystal.png"),
                            0, 0, 66, 66);

            //Mana cost
            QFont font(ThemeHandler::cardsFont());
            font.setPixelSize(45);
            font.setBold(true);
            font.setKerning(true);
        #ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
        #else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
        #endif
            painter->setFont(font);
            QPen pen(BLACK);
            pen.setWidth(2);
            painter->setPen(pen);
            if(cost > origCost)         painter->setBrush(RED);
            else                        painter->setBrush(GREEN);
            QString text = QString::number(cost);
            Utility::drawShadowText(*painter, font, text, -WIDTH/2 + 25, -heightShow/2+(cardLifted?-CARD_LIFT:0) + 28, true);
        }

        if(attack != origAttack)
        {
            painter->drawPixmap(-WIDTH/2 - 2, -heightShow/2 + 197 + (cardLifted?-CARD_LIFT:0), 52, 60,
                            QPixmap(":/Images/bgHeroAttack.png"),
                            0, 0, 85, 98);

            QFont font(ThemeHandler::cardsFont());
            font.setPixelSize(40);
            font.setBold(true);
            font.setKerning(true);
        #ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
        #else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
        #endif
            painter->setFont(font);
            QPen pen(BLACK);
            pen.setWidth(2);
            painter->setPen(pen);
            if(attack < origAttack)     painter->setBrush(RED);
            else                        painter->setBrush(GREEN);
            QString text = QString::number(attack);
            Utility::drawShadowText(*painter, font, text, -WIDTH/2 + 26, -heightShow/2 + 233 + (cardLifted?-CARD_LIFT:0), true);
        }

        if(health != origHealth)
        {
            painter->drawPixmap(-WIDTH/2 - 5 + 151, -heightShow/2 - 34 + 234+(cardLifted?-CARD_LIFT:0), 39, 55,
                            QPixmap(":/Images/bgHeroLife.png"),
                            0, 0, 70, 98);

            QFont font(ThemeHandler::cardsFont());
            font.setPixelSize(40);
            font.setBold(true);
            font.setKerning(true);
        #ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
        #else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
        #endif
            painter->setFont(font);
            QPen pen(BLACK);
            pen.setWidth(2);
            painter->setPen(pen);
            if(health < origHealth)     painter->setBrush(RED);
            else                        painter->setBrush(GREEN);
            QString text = QString::number(health);
            Utility::drawShadowText(*painter, font, text, -WIDTH/2 + 163, -heightShow/2 + 233 + (cardLifted?-CARD_LIFT:0), true);
        }
    }

    else if(!createdByCode.isEmpty())
    {
        painter->drawPixmap(-48, -heightShow/2+24+(cardLifted?-CARD_LIFT:0),
                            QPixmap(Utility::hscardsPath() + "/" + createdByCode + ".png"), 49, 60, 101, 66);
        painter->drawPixmap(-81, -heightShow/2+15+(cardLifted?-CARD_LIFT:0),
                            QPixmap(":/Images/bgCardCreatedBy.png"), 0, 0, 168,
                            heightShow-15+(cardLifted?CARD_LIFT:0));
    }

    else
    {
        painter->drawPixmap(-81, -heightShow/2+15+(cardLifted?-CARD_LIFT:0),
                            QPixmap(":/Images/bgCardUnknown.png"), 0, 0, 168,
                            heightShow-15+(cardLifted?CARD_LIFT:0));

        //Turn
        QFont font(ThemeHandler::cardsFont());
        font.setPixelSize(40);
        font.setBold(true);
        font.setKerning(true);
    #ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    #else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    #endif
        painter->setFont(font);
        QPen pen(BLACK);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(WHITE);
        QString text = "T"+QString::number((this->turn+1)/2);
        Utility::drawShadowText(*painter, font, text, -35, -heightShow/2 + 71 + (cardLifted?-CARD_LIFT:0), true);

        //Buff
        if(buffAttack > 0 || buffHealth > 0)
        {
            font.setPixelSize(40);
            text = "+" + QString::number(buffAttack) + "/+" + QString::number(buffHealth);
            painter->setFont(font);
            painter->setBrush(BLACK);
            pen = QPen(GREEN);
            pen.setWidth(2);
            painter->setPen(pen);
            Utility::drawShadowText(*painter, font, text, -5, -heightShow/2 + 133 + (cardLifted?-CARD_LIFT:0), true);
        }
    }
}


int CardGraphicsItem::getManaSpent(bool includeRefresh)
{
    int saveMana = 0;

    if(Utility::cardEnNameFromCode(code) == "The Coin") saveMana = 1;

    else if(code == COUNTERFEIT_COIN)   saveMana = 1;
    else if(code == INNERVATE)          saveMana = 1;

    else if(includeRefresh)
    {
        if(code == MANA_BISCUIT)        saveMana = 2;
        else if(code == LIGHTNING_BLOOM)saveMana = 2;
    }

    return cost - saveMana;
}
