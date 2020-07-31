#include "graveyardhandler.h"
#include "themehandler.h"

#include <QtWidgets>

GraveyardHandler::GraveyardHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->lastSecretIdAdded = -1;
    this->mouseInApp = false;
    this->inGame = false;

    completeUI();
    reset();
}

GraveyardHandler::~GraveyardHandler()
{
    ui->graveyardListWidgetPlayer->clear();
    ui->graveyardListWidgetEnemy->clear();
    deckCardListPlayer.clear();
    deckCardListEnemy.clear();
}


void GraveyardHandler::completeUI()
{
    ui->graveyardListWidgetPlayer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graveyardListWidgetPlayer->setMouseTracking(true);

    ui->graveyardListWidgetEnemy->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graveyardListWidgetEnemy->setMouseTracking(true);

    connect(ui->graveyardListWidgetPlayer, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findPlayerDeckCardEntered(QListWidgetItem*)));
    connect(ui->graveyardListWidgetEnemy, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findEnemyDeckCardEntered(QListWidgetItem*)));


    //Top buttons
    QButtonGroup *playerEnemyGroup = new QButtonGroup(ui->tabGraveyard);
    playerEnemyGroup->addButton(ui->graveyardButtonPlayer);
    playerEnemyGroup->addButton(ui->graveyardButtonEnemy);

    connect(ui->graveyardButtonPlayer, SIGNAL(clicked(bool)),
            this, SLOT(buttonPlayerClicked()));
    connect(ui->graveyardButtonEnemy, SIGNAL(clicked(bool)),
            this, SLOT(buttonEnemyClicked()));

    QButtonGroup *minionsWeaponsGroup = new QButtonGroup(ui->tabGraveyard);
    minionsWeaponsGroup->addButton(ui->graveyardButtonMinions);
    minionsWeaponsGroup->addButton(ui->graveyardButtonWeapons);

    connect(ui->graveyardButtonMinions, SIGNAL(clicked(bool)),
            this, SLOT(buttonMinionsClicked()));
    connect(ui->graveyardButtonWeapons, SIGNAL(clicked(bool)),
            this, SLOT(buttonWeaponsClicked()));

    //Patreon button
    planPatreonButton = new QPushButton(ui->tabGraveyard);
    planPatreonButton->setFlat(true);
    planPatreonButton->setIcon(QIcon(":/Images/becomePatreon.png"));
    planPatreonButton->setIconSize(QSize(217, 51));
    planPatreonButton->setToolTip("Unlock Graveyard becoming a patron (3$)");

    ui->tabGraveyardVerticalLayout->insertWidget(0, planPatreonButton);

    connect(planPatreonButton, SIGNAL(clicked()),
            this, SIGNAL(showPremiumDialog()));

    setPremium(false);
}


void GraveyardHandler::setPremium(bool premium)
{
    if(premium)
    {
        ui->graveyardListWidgetEnemy->hide();
        ui->graveyardListWidgetPlayer->show();
        ui->graveyardButtonPlayer->show();
        ui->graveyardButtonEnemy->show();
        ui->graveyardButtonMinions->show();
        ui->graveyardButtonWeapons->show();

        this->planPatreonButton->hide();
    }
    else
    {
        ui->graveyardListWidgetEnemy->hide();
        ui->graveyardListWidgetPlayer->hide();
        ui->graveyardButtonPlayer->hide();
        ui->graveyardButtonEnemy->hide();
        ui->graveyardButtonMinions->hide();
        ui->graveyardButtonWeapons->hide();

        this->planPatreonButton->show();
    }
}


void GraveyardHandler::buttonPlayerClicked()
{
    ui->graveyardListWidgetEnemy->hide();
    ui->graveyardListWidgetPlayer->show();
}


void GraveyardHandler::buttonEnemyClicked()
{
    ui->graveyardListWidgetPlayer->hide();
    ui->graveyardListWidgetEnemy->show();
}


void GraveyardHandler::buttonMinionsClicked()
{
    onlyShow(MINION);
}


void GraveyardHandler::buttonWeaponsClicked()
{
    onlyShow(WEAPON);
}


void GraveyardHandler::onlyShow(CardType cardType)
{
    for(DeckCard &deckCard: deckCardListPlayer)
    {
        deckCard.listItem->setHidden(deckCard.getType() != cardType);
    }

    for(DeckCard &deckCard: deckCardListEnemy)
    {
        deckCard.listItem->setHidden(deckCard.getType() != cardType);
    }
}


void GraveyardHandler::setTheme()
{
    ui->graveyardButtonPlayer->setIcon(QIcon(ThemeHandler::buttonGraveyardPlayerFile()));
    ui->graveyardButtonEnemy->setIcon(QIcon(ThemeHandler::buttonGraveyardEnemyFile()));
    ui->graveyardButtonMinions->setIcon(QIcon(ThemeHandler::buttonGraveyardMinionsFile()));
    ui->graveyardButtonWeapons->setIcon(QIcon(ThemeHandler::buttonGraveyardWeaponsFile()));
}


void GraveyardHandler::reset()
{
    this->lastSecretIdAdded = -1;
    ui->graveyardListWidgetPlayer->clear();
    ui->graveyardListWidgetEnemy->clear();
    deckCardListPlayer.clear();
    deckCardListEnemy.clear();

    emit pDebug("Graveyard deck lists cleared.");
}


void GraveyardHandler::playerCardGraveyard(int id, QString code)
{
    this->lastSecretIdAdded = -1;
    newDeckCard(true, code, id);
}


void GraveyardHandler::enemyCardGraveyard(int id, QString code)
{
    this->lastSecretIdAdded = -1;
    newDeckCard(false, code, id);
}


void GraveyardHandler::enemySecretRevealed(int id, QString code)
{
    //Los secretos se revelan 2 veces en el log, para evitar anadirlo 2 veces usamos lastScretIdAdded
    if(id == lastSecretIdAdded) return;

    this->lastSecretIdAdded = id;
    newDeckCard(false, code, id);
}


void GraveyardHandler::newDeckCard(bool friendly, QString code, int id)
{
    if(code.isEmpty())  return;

    //Ya existe en el mazo
    bool found = false;
    QList<DeckCard> &deckCardList = (friendly?deckCardListPlayer:deckCardListEnemy);
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCardList[i].getCode() == code)
        {
            if(deckCardList[i].id == id)
            {
                emit pDebug("Card id=" + QString::number(id) + " already in deck.");
                return;
            }

            found = true;
            deckCardList[i].total++;
            deckCardList[i].remaining++;
            deckCardList[i].draw();
            break;
        }
    }

    if(!found)
    {
        DeckCard deckCard(code);

        //Allow only weapon/minion
        CardType cardType = deckCard.getType();
        if(cardType != MINION && cardType != WEAPON)
        {
            emit pDebug("Avoid adding non weapon/minion to graveyard: " + deckCard.getName());
            return;
        }

        deckCard.id = id;
        deckCard.listItem = new QListWidgetItem();
        insertDeckCard(friendly, deckCard);

        //Show/hide
        if((cardType == MINION && !ui->graveyardButtonMinions->isChecked()) ||
                (cardType == WEAPON && !ui->graveyardButtonWeapons->isChecked()))
        {
            deckCard.listItem->setHidden(true);
        }

        //Draw
        deckCard.draw();
        emit checkCardImage(code);
    }

    emit pDebug("Add to " + (friendly?QString("Player"):QString("Enemy")) + " Graveyard: " +
                Utility::getCardAttribute(code, "name").toString());
}


void GraveyardHandler::insertDeckCard(bool friendly, DeckCard &deckCard)
{
    QList<DeckCard> &deckCardList = (friendly?deckCardListPlayer:deckCardListEnemy);
    MoveListWidget *listWidget = (friendly?ui->graveyardListWidgetPlayer:ui->graveyardListWidgetEnemy);

    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCard.getCost() < deckCardList[i].getCost())
        {
            deckCardList.insert(i, deckCard);
            listWidget->insertItem(i, deckCard.listItem);
            return;
        }
        else if(deckCard.getCost() == deckCardList[i].getCost())
        {
            if(deckCard.getName().toLower() < deckCardList[i].getName().toLower())
            {
                deckCardList.insert(i, deckCard);
                listWidget->insertItem(i, deckCard.listItem);
                return;
            }
        }
    }
    deckCardList.append(deckCard);
    listWidget->addItem(deckCard.listItem);
}


void GraveyardHandler::updateTransparency()
{
    if(transparency==Transparent || (!mouseInApp && inGame && transparency==AutoTransparent))
    {
        ui->tabGraveyard->setAttribute(Qt::WA_NoBackground);
        ui->tabGraveyard->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabGraveyard)
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabGraveyard->setAttribute(Qt::WA_NoBackground, false);
        ui->tabGraveyard->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabGraveyard)
        {
            emit needMainWindowFade(false);
        }
    }
}


void GraveyardHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void GraveyardHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}


void GraveyardHandler::lockGraveyardInterface()
{
    this->inGame = true;
    updateTransparency();

    reset();
}


void GraveyardHandler::unlockGraveyardInterface()
{
    this->inGame = false;
    updateTransparency();
}


void GraveyardHandler::redrawClassCards()
{
    foreach(DeckCard deckCard, deckCardListPlayer)
    {
        deckCard.draw();
    }

    foreach(DeckCard deckCard, deckCardListEnemy)
    {
        deckCard.draw();
    }
}


void GraveyardHandler::redrawSpellWeaponCards()
{
    foreach(DeckCard deckCard, deckCardListPlayer)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw();
        }
    }

    foreach(DeckCard deckCard, deckCardListEnemy)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw();
        }
    }
}


void GraveyardHandler::redrawAllCards()
{
    foreach(DeckCard deckCard, deckCardListPlayer)
    {
        deckCard.draw();
    }

    foreach(DeckCard deckCard, deckCardListEnemy)
    {
        deckCard.draw();
    }
}


void GraveyardHandler::redrawDownloadedCardImage(QString code)
{
    foreach(DeckCard deckCard, deckCardListPlayer)
    {
        if(deckCard.getCode() == code)  deckCard.draw();
    }

    foreach(DeckCard deckCard, deckCardListEnemy)
    {
        if(deckCard.getCode() == code)  deckCard.draw();
    }
}


void GraveyardHandler::findPlayerDeckCardEntered(QListWidgetItem * item)
{
    findDeckCardEntered(true, item);
}


void GraveyardHandler::findEnemyDeckCardEntered(QListWidgetItem * item)
{
    findDeckCardEntered(false, item);
}


void GraveyardHandler::findDeckCardEntered(bool friendly, QListWidgetItem * item)
{
    MoveListWidget *listWidget = (friendly?ui->graveyardListWidgetPlayer:ui->graveyardListWidgetEnemy);
    QString code = (friendly?deckCardListPlayer:deckCardListEnemy)[listWidget->row(item)].getCode();

    QRect rectCard = listWidget->visualItemRect(item);
    QPoint posCard = listWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int deckListTop = listWidget->mapToGlobal(QPoint(0,0)).y();
    int deckListBottom = listWidget->mapToGlobal(QPoint(0,listWidget->height())).y();
    emit cardEntered(code, globalRectCard, deckListTop, deckListBottom);
}
