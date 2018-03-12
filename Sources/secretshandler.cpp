#include "secretshandler.h"
#include <QtWidgets>

SecretsHandler::SecretsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler) : QObject(parent)
{
    this->ui = ui;
    this->enemyHandHandler = enemyHandHandler;
    this->secretsAnimating = false;
    this->lastMinionDead = "";
    this->lastMinionPlayed = "";
    this->lastSpellPlayed = "";

    completeUI();
}

SecretsHandler::~SecretsHandler()
{

}


void SecretsHandler::completeUI()
{
    ui->secretsTreeWidget->setHidden(true);

    ui->secretsTreeWidget->setColumnCount(1);
    ui->secretsTreeWidget->setIconSize(10*CARD_SIZE);
    ui->secretsTreeWidget->setStyleSheet("QTreeView{background-color: transparent;}"
                                         "QTreeView::item{padding: 0px;}");
    ui->secretsTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->secretsTreeWidget->setIndentation(5);
    ui->secretsTreeWidget->setItemsExpandable(false);
    ui->secretsTreeWidget->setMouseTracking(true);
    ui->secretsTreeWidget->setFixedHeight(0);

    connect(ui->secretsTreeWidget, SIGNAL(itemEntered(QTreeWidgetItem*,int)),
            this, SLOT(findSecretCardEntered(QTreeWidgetItem*)));
}


void SecretsHandler::resetLastMinionDead(QString code, QString subType)
{
    (void) code;
    //Duplica el primer esbirro que muera despues de una accion del usuario (!TRIGGER)
    if(subType != "TRIGGER")
    {
        this->lastMinionDead.clear();

        //El ManaBind es rebelado justo antes del POWER del hechizo lanzado
        if(subType != "PLAY")   this->lastSpellPlayed.clear();
    }
    //No podemos resetear lasMinionPlayed porque entre que se invoca el minion y se desvela el secreto ocurren
    //los tres subType POWER/PLAY/TRIGGER
}


void SecretsHandler::adjustSize()
{
    if(secretsAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustSize()));
        return;
    }

    int rowHeight = ui->secretsTreeWidget->sizeHintForRow(0);
    int rows = 0;

    for(int i=0; i<activeSecretList.count(); i++)
    {
        rows += activeSecretList[i].children.count() + 1;
    }

    int height = rows*rowHeight + 2*ui->secretsTreeWidget->frameWidth();
    int maxHeight = (ui->secretsTreeWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->secretsTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->secretsTreeWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->secretsTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->secretsTreeWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->secretsAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearSecretsAnimating()));
}


void SecretsHandler::clearSecretsAnimating()
{
    this->secretsAnimating = false;
    if(activeSecretList.empty())    ui->secretsTreeWidget->setHidden(true);
}


void SecretsHandler::secretStolen(int id, QString code)
{
    knownSecretPlayed(id, INVALID_CLASS, code);
}


void SecretsHandler::secretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState)
{
    HandCard *handCard = enemyHandHandler->getHandCard(id);

    if(handCard != NULL)
    {
        QString code = handCard->getCode();
        QString createdByCode = handCard->getCreatedByCode();

        //Secreto conocido
        if(!code.isEmpty() && Utility::isASecret(code))
        {
            knownSecretPlayed(id, hero, code);
        }
        //Pocion de polimorfia
        else if(createdByCode == KABAL_CHEMIST)
        {
            knownSecretPlayed(id, hero, POTION_OF_POLIMORPH);
        }
        //Discover card, puede ser cualquier secreto standard, incluido los baneados de arena
        else if(!createdByCode.isEmpty())
        {
            unknownSecretPlayed(id, hero, loadingScreenState, true);
        }
        //Deck Card
        else
        {
            unknownSecretPlayed(id, hero, loadingScreenState);
        }
    }
    else
    {
        unknownSecretPlayed(id, hero, loadingScreenState);
    }
}


void SecretsHandler::knownSecretPlayed(int id, CardClass hero, QString code)
{
    ActiveSecret activeSecret;
    activeSecret.id = id;
    activeSecret.root.hero = hero;

    activeSecret.root.treeItem = new QTreeWidgetItem(ui->secretsTreeWidget);
    activeSecret.root.treeItem->setExpanded(true);
    activeSecret.root.setCode(code);
    activeSecret.root.draw();
    emit checkCardImage(code);

    activeSecretList.append(activeSecret);

    ui->secretsTreeWidget->setHidden(false);
    emit isolatedSecret(activeSecret.id, activeSecret.root.getCode());

    //No puede haber dos secretos iguales
    discardSecretOptionNow(code);

    adjustSize();
}


void SecretsHandler::unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState, bool discover)
{
    bool showWildSecrets = (loadingScreenState == arena && WILD_ARENA);

    ActiveSecret activeSecret;
    activeSecret.id = id;
    activeSecret.root.hero = hero;

    activeSecret.root.treeItem = new QTreeWidgetItem(ui->secretsTreeWidget);
    activeSecret.root.treeItem->setExpanded(true);
    activeSecret.root.draw();

    switch(hero)
    {
        case PALADIN:
            if(loadingScreenState == arena) activeSecret.children.append(SecretCard(HAND_OF_SALVATION));
            if(showWildSecrets) activeSecret.children.append(SecretCard(AVENGE));
            activeSecret.children.append(SecretCard(NOBLE_SACRIFICE));
            activeSecret.children.append(SecretCard(REPENTANCE));
            activeSecret.children.append(SecretCard(REDEMPTION));
            if(showWildSecrets) activeSecret.children.append(SecretCard(SACRED_TRIAL));
            activeSecret.children.append(SecretCard(EYE_FOR_AN_EYE));
            activeSecret.children.append(SecretCard(GETAWAY_KODO));
            if(showWildSecrets) activeSecret.children.append(SecretCard(COMPETITIVE_SPIRIT));
        break;

        case HUNTER:
            activeSecret.children.append(SecretCard(FREEZING_TRAP));
            activeSecret.children.append(SecretCard(EXPLOSIVE_TRAP));
            if(showWildSecrets) activeSecret.children.append(SecretCard(BEAR_TRAP));
            if(loadingScreenState != arena || discover) activeSecret.children.append(SecretCard(SNIPE));//BANNED ARENA
            if(showWildSecrets) activeSecret.children.append(SecretCard(DART_TRAP));
            activeSecret.children.append(SecretCard(WANDERING_MONSTER));
            activeSecret.children.append(SecretCard(VENOMSTRIKE_TRAP));
            activeSecret.children.append(SecretCard(CAT_TRICK));
            activeSecret.children.append(SecretCard(MISDIRECTION));
            activeSecret.children.append(SecretCard(HIDDEN_CACHE));
            activeSecret.children.append(SecretCard(SNAKE_TRAP));
        break;

        case MAGE:
            activeSecret.children.append(SecretCard(FROZEN_CLONE));
            activeSecret.children.append(SecretCard(MIRROR_ENTITY));
            if(showWildSecrets) activeSecret.children.append(SecretCard(DDUPLICATE));
            activeSecret.children.append(SecretCard(ICE_BARRIER));
            activeSecret.children.append(SecretCard(EXPLOSIVE_RUNES));
            activeSecret.children.append(SecretCard(POTION_OF_POLIMORPH));
            if(showWildSecrets) activeSecret.children.append(SecretCard(EFFIGY));
            activeSecret.children.append(SecretCard(VAPORIZE));
            activeSecret.children.append(SecretCard(COUNTERSPELL));
            activeSecret.children.append(SecretCard(MANA_BIND));
            activeSecret.children.append(SecretCard(SPELLBENDER));
            activeSecret.children.append(SecretCard(ICE_BLOCK));
        break;

        case ROGUE:
            activeSecret.children.append(SecretCard(SUDDEN_BETRAYAL));
            activeSecret.children.append(SecretCard(CHEAT_DEATH));
            activeSecret.children.append(SecretCard(EVASION));
        break;

        default:
        break;
    }

    emit pDebug("Secret played. Hero: " + QString::number(hero));

    //Eliminar de las opciones, secretos que ya hemos reducido a 1 opcion
    foreach(ActiveSecret activeSecretOld, activeSecretList)
    {
        QString code = activeSecretOld.root.getCode();
        if(!code.isEmpty())
        {
            for(int i=0; i<activeSecret.children.count(); i++)
            {
                if(activeSecret.children[i].getCode() == code)
                {
                    emit pDebug("Option discarded on just played secret (already guessed on an active secret): " + code);
                    activeSecret.children.removeAt(i);
                    break;
                }
            }
        }
    }

    for(QList<SecretCard>::iterator it = activeSecret.children.begin(); it != activeSecret.children.end(); it++)
    {
        it->treeItem = new QTreeWidgetItem(activeSecret.root.treeItem);
        it->draw();
        emit checkCardImage(it->getCode());
    }

    activeSecretList.append(activeSecret);

    ui->secretsTreeWidget->setHidden(false);

    adjustSize();
}


void SecretsHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        if(it->root.getCode() == code)    it->root.draw();
        for(QList<SecretCard>::iterator it2 = it->children.begin(); it2 != it->children.end(); it2++)
        {
            if(it2->getCode() == code)    it2->draw();
        }
    }
}


void SecretsHandler::redrawClassCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        if(activeSecret.root.getCardClass()<9)   activeSecret.root.draw();
        foreach(SecretCard secretCard, activeSecret.children)
        {
            if(secretCard.getCardClass()<9)   secretCard.draw();
        }
    }
}


void SecretsHandler::redrawSpellWeaponCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        CardType cardType = activeSecret.root.getType();
        if(cardType == SPELL || cardType == WEAPON)   activeSecret.root.draw();
        foreach(SecretCard secretCard, activeSecret.children)
        {
            cardType = secretCard.getType();
            if(cardType == SPELL || cardType == WEAPON)   secretCard.draw();
        }
    }
}


void SecretsHandler::redrawAllCards()
{
    foreach(ActiveSecret activeSecret, activeSecretList)
    {
        activeSecret.root.draw();
        foreach(SecretCard secretCard, activeSecret.children)
        {
            secretCard.draw();
        }
    }
}


void SecretsHandler::resetSecretsInterface()
{
    ui->secretsTreeWidget->setHidden(true);
    ui->secretsTreeWidget->clear();
    activeSecretList.clear();
    secretTests.clear();
}


void SecretsHandler::secretRevealed(int id, QString code)
{
    for(int i=0; i<activeSecretList.count(); i++)
    {
        if(activeSecretList[i].id == id)
        {
            ui->secretsTreeWidget->takeTopLevelItem(i);
            delete activeSecretList[i].root.treeItem;
            activeSecretList.removeAt(i);
            break;
        }
    }

    for(int i=0; i<secretTests.count(); i++)
    {
        secretTests[i].secretRevealedLastSecond = true;
    }
    adjustSize();

    //No puede haber dos secretos iguales
    discardSecretOptionNow(code);

    emit pDebug("Secret revealed: " + code);


    //Reveal cards in Hand
    if(code == GETAWAY_KODO && !lastMinionDead.isEmpty())       emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(code == MANA_BIND && !lastSpellPlayed.isEmpty())    emit revealCreatedByCard(lastSpellPlayed, code, 1);
    else if(code == FROZEN_CLONE && !lastMinionPlayed.isEmpty())emit revealCreatedByCard(lastMinionPlayed, code, 2);
    else if(code == CHEAT_DEATH && !lastMinionDead.isEmpty())   emit revealCreatedByCard(lastMinionDead, code, 1);
    else if(code == DDUPLICATE && !lastMinionDead.isEmpty())    emit revealCreatedByCard(lastMinionDead, code, 2);
}


void SecretsHandler::discardSecretOptionDelay()
{
    if(secretTests.isEmpty())   return;

    SecretTest secretTest = secretTests.dequeue();
    if(secretTest.secretRevealedLastSecond)
    {
        emit pDebug("Option not discarded: " + secretTest.code + " (A secret revealed)");
        return;
    }

    discardSecretOptionNow(secretTest.code);
}


void SecretsHandler::discardSecretOptionNow(QString code)
{
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        for(int i=0; i<it->children.count(); i++)
        {
            if(it->children[i].getCode() == code)
            {
                emit pDebug("Option discarded: " + code);
                delete it->children[i].treeItem;
                it->children.removeAt(i);
                QTimer::singleShot(10, this, SLOT(adjustSize()));

                //Comprobar unica posibilidad
                checkLastSecretOption(*it);
                break;
            }
        }
    }
}


void SecretsHandler::discardSecretOption(QString code, int delay)
{
    if(activeSecretList.isEmpty()){}
    else if(activeSecretList.count() == 1)
    {
        discardSecretOptionNow(code);
    }
    else
    {
        SecretTest secretTest;
        secretTest.code = code;
        secretTest.secretRevealedLastSecond = false;
        secretTests.enqueue(secretTest);

        QTimer::singleShot(delay, this, SLOT(discardSecretOptionDelay()));
    }
}


void SecretsHandler::checkLastSecretOption(ActiveSecret &activeSecret)
{
    if(activeSecret.children.count() == 1)
    {
        activeSecret.root.setCode(activeSecret.children.first().getCode());
        activeSecret.root.draw();
        activeSecret.root.treeItem->removeChild(activeSecret.children.first().treeItem);
        activeSecret.children.clear();
        emit isolatedSecret(activeSecret.id, activeSecret.root.getCode());

        //No puede haber dos secretos iguales
        discardSecretOptionNow(activeSecret.root.getCode());
    }
}


void SecretsHandler::playerSpellPlayed(QString code)
{
    if(lastSpellPlayed.isEmpty())    lastSpellPlayed = code;

    discardSecretOptionNow(COUNTERSPELL);
    discardSecretOptionNow(MANA_BIND);

    discardSecretOptionNow(CAT_TRICK);
}


void SecretsHandler::playerSpellObjMinionPlayed()
{
    discardSecretOption(SPELLBENDER);//Ocultado por COUNTERSPELL
}


void SecretsHandler::playerSpellObjHeroPlayed()
{
    discardSecretOptionNow(EVASION);
}


void SecretsHandler::playerBattlecryObjHeroPlayed()
{
    discardSecretOptionNow(EVASION);
}


void SecretsHandler::playerHeroPower()
{
    discardSecretOptionNow(DART_TRAP);
}


void SecretsHandler::playerMinionPlayed(QString code, int playerMinions)
{
    Q_UNUSED(playerMinions);
    lastMinionPlayed = code;

    discardSecretOptionNow(FROZEN_CLONE);//No necesita objetivo
    discardSecretOption(MIRROR_ENTITY);//Ocultado por EXPLOSIVE_RUNES
    discardSecretOption(POTION_OF_POLIMORPH);//Ocultado por EXPLOSIVE_RUNES
    discardSecretOptionNow(EXPLOSIVE_RUNES);

    discardSecretOptionNow(SNIPE);
    discardSecretOptionNow(HIDDEN_CACHE);

    if(playerMinions>3)
    {
        discardSecretOptionNow(SACRED_TRIAL);
        discardSecretOption(REPENTANCE);//Ocultado por SACRED_TRIAL
    }
    else    discardSecretOptionNow(REPENTANCE);

}


void SecretsHandler::enemyMinionDead(QString code)
{
    if(lastMinionDead.isEmpty())    lastMinionDead = code;

    discardSecretOptionNow(DDUPLICATE);
    discardSecretOptionNow(EFFIGY);
    discardSecretOptionNow(REDEMPTION);
    discardSecretOptionNow(GETAWAY_KODO);

    discardSecretOptionNow(CHEAT_DEATH);
}


void SecretsHandler::avengeTested()
{
    discardSecretOptionNow(AVENGE);
}


void SecretsHandler::handOfSalvationTested()
{
    discardSecretOptionNow(HAND_OF_SALVATION);
}


void SecretsHandler::cSpiritTested()
{
    discardSecretOptionNow(COMPETITIVE_SPIRIT);
}


/*
 * http://hearthstone.gamepedia.com/Secret
 *
 * If a Secret removes the specific target for another Secret which was already triggered, the second Secret will not take effect,
 * since it now lacks a target. For example, if Freezing Trap removes the minion which would have been the target of Misdirection,
 * the Misdirection will not trigger, since it no longer has a target.
 *
 * Note that this rule only applies for Secrets which require specific targets; Secrets such as Explosive Trap and Snake Trap do not require targets,
 * and will always take effect once triggered, even if the original trigger minion has been removed from play.
 */
void SecretsHandler::playerAttack(bool isHeroFrom, bool isHeroTo, int playerMinions)
{
    if(isHeroFrom)
    {
        //Hero -> hero
        if(isHeroTo)
        {
            discardSecretOptionNow(ICE_BARRIER);

            discardSecretOptionNow(EXPLOSIVE_TRAP);//No necesita objetivo
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(MISDIRECTION);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOptionNow(WANDERING_MONSTER);//No necesita objetivo

            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);

            discardSecretOptionNow(EVASION);
        }
        //Hero -> minion
        else
        {
            discardSecretOptionNow(VENOMSTRIKE_TRAP);
            discardSecretOptionNow(SNAKE_TRAP);

            discardSecretOptionNow(NOBLE_SACRIFICE);
        }
    }
    else
    {
        //Minion -> hero
        if(isHeroTo)
        {
            discardSecretOptionNow(VAPORIZE);
            discardSecretOptionNow(ICE_BARRIER);

            discardSecretOptionNow(EXPLOSIVE_TRAP);//No necesita objetivo
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(FREEZING_TRAP);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOption(MISDIRECTION);//Ocultado por FREEZING_TRAP y EXPLOSIVE_TRAP
            discardSecretOptionNow(WANDERING_MONSTER);//No necesita objetivo

            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);

            if(playerMinions > 1)   discardSecretOptionNow(SUDDEN_BETRAYAL);
            discardSecretOption(EVASION);//Ocultado por SUDDEN_BETRAYAL
        }
        //Minion -> minion
        else
        {
            discardSecretOptionNow(FREEZING_TRAP);
            discardSecretOptionNow(VENOMSTRIKE_TRAP);
            discardSecretOptionNow(SNAKE_TRAP);

            discardSecretOptionNow(NOBLE_SACRIFICE);
        }
    }
}


void SecretsHandler::findSecretCardEntered(QTreeWidgetItem * item)
{
    QString code;
    int indexTopLevel = ui->secretsTreeWidget->indexOfTopLevelItem(item);
    int indexLowLevel = -1;
    if(indexTopLevel == -1)//Low level item
    {
        indexTopLevel = ui->secretsTreeWidget->indexOfTopLevelItem(item->parent());
        indexLowLevel = item->parent()->indexOfChild(item);
        code = activeSecretList[indexTopLevel].children[indexLowLevel].getCode();
    }
    else//Top level item
    {
        code = activeSecretList[indexTopLevel].root.getCode();
    }

    QRect rectCard = ui->secretsTreeWidget->visualItemRect(item);
    QPoint posCard = ui->secretsTreeWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int secretListTop = ui->secretsTreeWidget->mapToGlobal(QPoint(0,0)).y();
    int secretListBottom = ui->secretsTreeWidget->mapToGlobal(QPoint(0,ui->secretsTreeWidget->height())).y();
    emit cardEntered(code, globalRectCard, secretListTop, secretListBottom);
}


QStringList SecretsHandler::getSecretOptionCodes(int id)
{
    foreach(ActiveSecret secret, activeSecretList)
    {
        if(secret.id == id)
        {
            if(secret.children.isEmpty())   return QStringList(secret.root.getCode());

            QStringList codes;
            foreach(SecretCard secretCard, secret.children)
            {
                codes.append(secretCard.getCode());
            }
            return codes;
        }
    }
    return QStringList();
}


/* Secrets interactions. Kobolds & Catacombs update.
Hey guys!
It's the time for the test of new secrets. In order to update Arena Tracker I need to play with all the combinations of secrets to know in which order they trigger and which one overlap with any other.

Secrets, same as deathrattle, trigger in the same order you played them. If a secret X rely on a target and that target was eliminated by a previous secret Y, triggered by the same action, then the secret X won't trigger.

The result of this is a bunch of data that may be of interest for some of you, so here it is:


Sudden Betrayal and Evasion:

Sudden Betrayal will only trigger when you have 2+ minions. If it triggers, Evasion will never be revealed as your hero doesn't receive any damage.


Explosive Runes, Frozen Clone, Mirror Entity and Potion of Polimorph:

Frozen Clone will always trigger as it doesn't rely on the target to be alive.

Explosive Runes will always trigger as neither of the other 3 secrets will remove the target (Potion of Polimorph just morph it).

Mirror Entity and Potion of Polimorph won't trigger if Explosive Runes killed the minion before.

If Explosive Runes triggers but don't kill the minion, Mirror Entity will copy the damaged minion.

If Potion of Polimorph triggers first, Mirror Entity will copy a 1/1 sheep, Frozen Clone will create two copies of a 1/1 sheep and Explosive Runes will deal 1 damage to the sheep and the rest to the opponent hero.


Freezing Trap, Explosive Trap, Missdirection and Wandering Monster:

Explosive Trap and Wandering Monster will always trigger as they don't rely on the target to be alive.

Freezing Trap won't trigger if Explosive Trap killed the minion before.

Misdirection won't trigger if Explosive Trap or Freezing Trap killed the minion before.
*/






