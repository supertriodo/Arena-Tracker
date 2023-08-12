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
    reset();
}


void GraveyardHandler::completeUI()
{
    //Lists
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
    minionsWeaponsGroup->addButton(ui->graveyardButtonAll);
    minionsWeaponsGroup->addButton(ui->graveyardButtonMinions);
    minionsWeaponsGroup->addButton(ui->graveyardButtonWeapons);
    minionsWeaponsGroup->addButton(ui->graveyardButtonSpells);

    connect(ui->graveyardButtonAll, SIGNAL(clicked(bool)),
            this, SLOT(buttonAllClicked()));
    connect(ui->graveyardButtonMinions, SIGNAL(clicked(bool)),
            this, SLOT(buttonMinionsClicked()));
    connect(ui->graveyardButtonWeapons, SIGNAL(clicked(bool)),
            this, SLOT(buttonWeaponsClicked()));
    connect(ui->graveyardButtonSpells, SIGNAL(clicked(bool)),
            this, SLOT(buttonSpellsClicked()));

    //Patreon button
    planPatreonButton = new QPushButton(ui->tabGraveyard);
    planPatreonButton->setFlat(true);
    planPatreonButton->setIcon(QIcon(":/Images/becomePatreon.png"));
    planPatreonButton->setIconSize(QSize(217, 51));
    planPatreonButton->setToolTip("Unlock Graveyard becoming a patron (3€)");

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
        ui->graveyardButtonAll->show();
        ui->graveyardButtonMinions->show();
        ui->graveyardButtonWeapons->show();
        ui->graveyardButtonSpells->show();

        this->planPatreonButton->hide();
    }
    else
    {
        ui->graveyardListWidgetEnemy->hide();
        ui->graveyardListWidgetPlayer->hide();
        ui->graveyardButtonPlayer->hide();
        ui->graveyardButtonEnemy->hide();
        ui->graveyardButtonAll->hide();
        ui->graveyardButtonMinions->hide();
        ui->graveyardButtonWeapons->hide();
        ui->graveyardButtonSpells->hide();

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


void GraveyardHandler::buttonAllClicked()
{
    showAll();
}


void GraveyardHandler::buttonMinionsClicked()
{
    onlyShow(MINION);
}


void GraveyardHandler::buttonWeaponsClicked()
{
    onlyShow(WEAPON);
}


void GraveyardHandler::buttonSpellsClicked()
{
    onlyShow(SPELL);
}


void GraveyardHandler::onlyShow(CardType cardType)
{
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        deckCard.listItem->setHidden(deckCard.getType() != cardType);
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        deckCard.listItem->setHidden(deckCard.getType() != cardType);
    }
}


void GraveyardHandler::showAll()
{
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        deckCard.listItem->setHidden(false);
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        deckCard.listItem->setHidden(false);
    }
}


void GraveyardHandler::setTheme()
{
    ui->graveyardButtonPlayer->setIcon(QIcon(ThemeHandler::buttonGraveyardPlayerFile()));
    ui->graveyardButtonEnemy->setIcon(QIcon(ThemeHandler::buttonGraveyardEnemyFile()));
    ui->graveyardButtonAll->setIcon(QIcon(ThemeHandler::buttonGraveyardAllFile()));
    ui->graveyardButtonWeapons->setIcon(QIcon(ThemeHandler::buttonGraveyardWeaponsFile()));

    redrawMinionSpellIcons();
}


void GraveyardHandler::redrawMinionSpellIcons()
{
    ui->graveyardButtonMinions->setIcon(QIcon(drawNumberedIcon(ThemeHandler::buttonGraveyardMinionsFile(),
                                                               racesPlayer.count() + racesPlayer[ALL], racesEnemy.count() + racesEnemy[ALL])));
    ui->graveyardButtonSpells->setIcon(QIcon(drawNumberedIcon(ThemeHandler::buttonGraveyardSpellsFile(), schoolsPlayer.count(), schoolsEnemy.count())));
}


void GraveyardHandler::reset()
{
    this->lastSecretIdAdded = -1;
    ui->graveyardListWidgetPlayer->clear();
    ui->graveyardListWidgetEnemy->clear();
    deckCardListPlayer.clear();
    deckCardListEnemy.clear();
    racesPlayer.clear();
    racesPlayer[ALL]=-1;
    racesEnemy.clear();
    racesEnemy[ALL]=-1;
    schoolsPlayer.clear();
    schoolsEnemy.clear();
    redrawMinionSpellIcons();

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
    QList<SynergyCard> &deckCardList = (friendly?deckCardListPlayer:deckCardListEnemy);
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
            deckCardList[i].draw(false);
            break;
        }
    }

    if(!found)
    {
        SynergyCard deckCard(code);

        //Allow only weapon/minion/spell
        CardType cardType = deckCard.getType();
        if(cardType != MINION && cardType != WEAPON && cardType != SPELL)
        {
            emit pDebug("Avoid adding non weapon/minion/spell to graveyard: " + deckCard.getName());
            return;
        }

        deckCard.id = id;
        deckCard.listItem = new QListWidgetItem();
        deckCard.setRaceSchoolTag();
        insertDeckCard(friendly, deckCard);

        //Show/hide
        if(!ui->graveyardButtonAll->isChecked() &&
           ((cardType == MINION && !ui->graveyardButtonMinions->isChecked()) ||
            (cardType == WEAPON && !ui->graveyardButtonWeapons->isChecked()) ||
            (cardType == SPELL && !ui->graveyardButtonSpells->isChecked())))
        {
            deckCard.listItem->setHidden(true);
        }

        //Draw
        deckCard.draw(false);
        emit checkCardImage(code);

        //schools/races
        updateRacesSchools(friendly, deckCard);
    }

    emit pDebug("Add to " + (friendly?QString("Player"):QString("Enemy")) + " Graveyard: " +
                Utility::getCardAttribute(code, "name").toString());
}


void GraveyardHandler::insertDeckCard(bool friendly, SynergyCard &deckCard)
{
    QList<SynergyCard> &deckCardList = (friendly?deckCardListPlayer:deckCardListEnemy);
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
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        deckCard.draw(false);
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        deckCard.draw(false);
    }
}


void GraveyardHandler::redrawSpellWeaponCards()
{
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw(false);
        }
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw(false);
        }
    }
}


void GraveyardHandler::redrawAllCards()
{
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        deckCard.draw(false);
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        deckCard.draw(false);
    }
}


void GraveyardHandler::redrawDownloadedCardImage(QString code)
{
    for(SynergyCard &deckCard: deckCardListPlayer)
    {
        if(deckCard.getCode() == code)  deckCard.draw(false);
    }

    for(SynergyCard &deckCard: deckCardListEnemy)
    {
        if(deckCard.getCode() == code)  deckCard.draw(false);
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


void GraveyardHandler::updateRacesSchools(bool friendly, SynergyCard &deckCard)
{
    QMap<CardRace, int> &races = (friendly?racesPlayer:racesEnemy);
    QMap<CardSchool, bool> &schools = (friendly?schoolsPlayer:schoolsEnemy);

    CardType type = deckCard.getType();
    if(type == MINION)
    {
        QList<CardRace> cardRaces = deckCard.getRace();
        for(const CardRace &cardRace: cardRaces)
        {
            if(cardRace == ALL) races[ALL]++;
            else if(cardRace == MURLOC || cardRace == DEMON || cardRace == MECHANICAL || cardRace == ELEMENTAL || cardRace == BEAST || cardRace == TOTEM ||
                    cardRace == PIRATE || cardRace == DRAGON || cardRace == NAGA || cardRace == UNDEAD || cardRace == QUILBOAR)
                races[cardRace] = 1;
        }
    }
    else if(type == SPELL)
    {
        CardSchool cardSchool = deckCard.getSchool();
        if(cardSchool == ARCANE || cardSchool == FEL || cardSchool == FIRE || cardSchool == FROST || cardSchool == HOLY || cardSchool == SHADOW || cardSchool == NATURE)
            schools[cardSchool] = true;
    }

    redrawMinionSpellIcons();
}


QPixmap GraveyardHandler::drawNumberedIcon(QString iconFile, int numberPlayer, int numberEnemy)
{
    QPixmap canvas(iconFile);
    float scale = 3;

    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Enemy Tag
        if(numberEnemy > 0)
        {
            QString text = QString::number(numberEnemy);
            QFont font(ThemeHandler::cardsFont());
            font.setBold(true);
            font.setKerning(true);
    #ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    #else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    #endif
            font.setPixelSize(12*scale);
            QFontMetrics fm(font);
            int textWide = fm.width(text);

            painter.setPen(QPen(BLACK));
            painter.setBrush(SOFT_RED);
            painter.drawRoundRect(canvas.width() - textWide - 3*scale, 0, 3*scale + textWide, 11*scale, 50, 50);

            painter.setPen(QPen(BLACK));
            painter.setBrush(WHITE);
            Utility::drawShadowText(painter, font, text, canvas.width() - textWide - 2*scale, 6*scale, false);
        }

        //Player Tag
        if(numberPlayer > 0)
        {
            QString text = QString::number(numberPlayer);
            QFont font(ThemeHandler::cardsFont());
            font.setBold(true);
            font.setKerning(true);
    #ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    #else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    #endif
            font.setPixelSize(12*scale);
            QFontMetrics fm(font);
            int textWide = fm.width(text);

            QColor synergyTagColor = QColor(ThemeHandler::synergyTagColor());
            painter.setPen(QPen(BLACK));
            painter.setBrush(synergyTagColor.isValid()?synergyTagColor:BLACK);
            painter.drawRoundRect(0, canvas.height() - 11*scale, 3*scale + textWide, 11*scale, 50, 50);

            painter.setPen(QPen(BLACK));
            painter.setBrush(WHITE);
            Utility::drawShadowText(painter, font, text, 1*scale, canvas.height() - 5*scale, false);
        }
    painter.end();

    return canvas;
}

