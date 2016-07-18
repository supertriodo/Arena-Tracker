#include "enemydeckhandler.h"
#include <QtWidgets>

EnemyDeckHandler::EnemyDeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson,
                                   Ui::Extended *ui, EnemyHandHandler *enemyHandHandler) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->enemyHandHandler = enemyHandHandler;
    this->transparency = Opaque;
    this->enemyClass = INVALID_CLASS;
    this->lastSecretIdAdded = -1;
    this->mouseInApp = false;
    this->inGame = false;

    completeUI();
    reset();
}

EnemyDeckHandler::~EnemyDeckHandler()
{
    ui->enemyDeckListWidget->clear();
    deckCardList.clear();
}


void EnemyDeckHandler::completeUI()
{
    ui->enemyDeckListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->enemyDeckListWidget->setMouseTracking(true);

    connect(ui->enemyDeckListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDeckCardEntered(QListWidgetItem*)));

}


void EnemyDeckHandler::reset()
{
    this->enemyClass = INVALID_CLASS;
    this->lastSecretIdAdded = -1;
    ui->enemyDeckListWidget->clear();
    deckCardList.clear();

    DeckCard deckCard("");
    deckCard.total = 30;
    deckCard.remaining = deckCard.total;
    deckCard.listItem = new QListWidgetItem();
    deckCard.draw();
    insertDeckCard(deckCard);

    emit pDebug("Deck list cleared.");
}


void EnemyDeckHandler::setEnemyClass(QString hero)
{
    this->enemyClass = Utility::heroFromLogNumber(hero);
}


void EnemyDeckHandler::enemyKnownCardDraw(QString code)
{
    this->lastSecretIdAdded = -1;
    newDeckCard(code, 1, true);
}


void EnemyDeckHandler::enemyCardPlayed(int id, QString code)
{
    this->lastSecretIdAdded = -1;

    if(enemyHandHandler->isFromEnemyDeck(id))
    {
        newDeckCard(code, 1, true);
    }

    enemyHandHandler->hideEnemyCardPlayed(id, code);
}


void EnemyDeckHandler::enemySecretRevealed(int id, QString code)
{
    //Los secretos se revelan 2 veces en el log, para evitar anadirlo 2 veces usamos lastScretIdAdded
    if(id == lastSecretIdAdded) return;

    this->lastSecretIdAdded = id;
    newDeckCard(code, 1, true);
}


void EnemyDeckHandler::newDeckCard(QString code, int total, bool add)
{
    if(code.isEmpty())  return;

    //Mazo completo
    if(deckCardList[0].total < (uint)total)
    {
        emit pDebug("Deck is full: Not adding: (" + QString::number(total) + ") " +
                    (*cardsJson)[code].value("name").toString(), Warning);
        return;
    }

    //Ya existe en el mazo
    bool found = false;
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCardList[i].getCode() == code)
        {
            if(!add)
            {
                emit pDebug((*cardsJson)[code].value("name").toString() + " already in deck.");
                return;
            }

            found = true;
            deckCardList[i].total+=total;
            deckCardList[i].remaining+=total;
            deckCardList[i].draw();
            break;
        }
    }

    if(!found)
    {
        DeckCard deckCard(code);
        CardClass cardClass = deckCard.getCardClass();

        if(enemyClass != INVALID_CLASS && cardClass != enemyClass && cardClass != NEUTRAL)
        {
            emit pDebug("Wrong class card: Not adding: (" + QString::number(total) + ") " +
                        (*cardsJson)[code].value("name").toString(), Warning);
            return;
        }

        deckCard.total = total;
        deckCard.remaining = total;
        deckCard.listItem = new QListWidgetItem();
        insertDeckCard(deckCard);
        deckCard.draw();
        emit checkCardImage(code);
    }

    deckCardList[0].total-=total;
    deckCardList[0].remaining = deckCardList[0].total;
    deckCardList[0].draw();
    if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);

    emit pDebug("Add to deck: (" + QString::number(total) + ")" +
                (*cardsJson)[code].value("name").toString());
}


void EnemyDeckHandler::insertDeckCard(DeckCard &deckCard)
{
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCard.getCost() < deckCardList[i].getCost())
        {
            deckCardList.insert(i, deckCard);
            ui->enemyDeckListWidget->insertItem(i, deckCard.listItem);
            return;
        }
        else if(deckCard.getCost() == deckCardList[i].getCost())
        {
            if(deckCard.getType() != deckCardList[i].getType())
            {
                if(deckCard.getType() == WEAPON || deckCardList[i].getType() == MINION)
                {
                    deckCardList.insert(i, deckCard);
                    ui->enemyDeckListWidget->insertItem(i, deckCard.listItem);
                    return;
                }
            }
            else if(deckCard.getName().toLower() < deckCardList[i].getName().toLower())
            {
                deckCardList.insert(i, deckCard);
                ui->enemyDeckListWidget->insertItem(i, deckCard.listItem);
                return;
            }
        }
    }
    deckCardList.append(deckCard);
    ui->enemyDeckListWidget->addItem(deckCard.listItem);
}


void EnemyDeckHandler::updateTransparency()
{
    if(!mouseInApp && (transparency==Transparent || (inGame && transparency==AutoTransparent)))
    {
        ui->tabEnemyDeck->setAttribute(Qt::WA_NoBackground);
        ui->tabEnemyDeck->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabEnemyDeck)
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabEnemyDeck->setAttribute(Qt::WA_NoBackground, false);
        ui->tabEnemyDeck->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabEnemyDeck)
        {
            emit needMainWindowFade(false);
        }
    }
}


void EnemyDeckHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void EnemyDeckHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}


void EnemyDeckHandler::lockEnemyDeckInterface()
{
    this->inGame = true;
    updateTransparency();

    reset();
}


void EnemyDeckHandler::unlockEnemyDeckInterface()
{
    this->inGame = false;
    updateTransparency();
}


void EnemyDeckHandler::redrawClassCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        if(deckCard.getCardClass()<9)
        {
            deckCard.draw();
        }
    }
}


void EnemyDeckHandler::redrawSpellWeaponCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw();
        }
    }
}


void EnemyDeckHandler::redrawAllCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        deckCard.draw();
    }
}


void EnemyDeckHandler::redrawDownloadedCardImage(QString code)
{
    foreach(DeckCard deckCard, deckCardList)
    {
        if(deckCard.getCode() == code)  deckCard.draw();
    }
}


void EnemyDeckHandler::findDeckCardEntered(QListWidgetItem * item)
{
    QString code = deckCardList[ui->enemyDeckListWidget->row(item)].getCode();

    QRect rectCard = ui->enemyDeckListWidget->visualItemRect(item);
    QPoint posCard = ui->enemyDeckListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int deckListTop = ui->enemyDeckListWidget->mapToGlobal(QPoint(0,0)).y();
    int deckListBottom = ui->enemyDeckListWidget->mapToGlobal(QPoint(0,ui->enemyDeckListWidget->height())).y();
    emit cardEntered(code, globalRectCard, deckListTop, deckListBottom);
}

QList<DeckCard> EnemyDeckHandler::getDeckCardList()
{
    return deckCardList;
}
