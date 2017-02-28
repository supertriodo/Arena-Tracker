#include "secretshandler.h"
#include <QtWidgets>

SecretsHandler::SecretsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler) : QObject(parent)
{
    this->ui = ui;
    this->enemyHandHandler = enemyHandHandler;
    this->synchronized = false;
    this->secretsAnimating = false;
    this->lastMinionDead = "";

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
                                         "QTreeView::item{padding: -1px;}");
    ui->secretsTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->secretsTreeWidget->setIndentation(5);
    ui->secretsTreeWidget->setItemsExpandable(false);
    ui->secretsTreeWidget->setMouseTracking(true);

    connect(ui->secretsTreeWidget, SIGNAL(itemEntered(QTreeWidgetItem*,int)),
            this, SLOT(findSecretCardEntered(QTreeWidgetItem*)));
}


void SecretsHandler::setSynchronized()
{
    this->synchronized = true;
}


void SecretsHandler::resetLastMinionDead(QString code, QString subType)
{
    (void) code;
    //Duplica el primer esbirro que muera despues de una accion del usuario (!TRIGGER)
    if(subType != "TRIGGER")   this->lastMinionDead.clear();
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
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->secretsTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->secretsTreeWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(QEasingCurve::OutBounce);
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
        QString createdByCode = handCard->getCreatedByCode();
        if(createdByCode == KABAL_CHEMIST)
        {
            knownSecretPlayed(id, hero, POTION_OF_POLIMORPH);
        }
        //CreatedBy card
        else if(!createdByCode.isEmpty())
        {
            //Puede ser cualquier secreto
            unknownSecretPlayed(id, hero, tavernBrawl);
        }
        //Deck card
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

    //No puede haber dos secretos iguales
    discardSecretOptionNow(code);

    adjustSize();
}


void SecretsHandler::unknownSecretPlayed(int id, CardClass hero, LoadingScreenState loadingScreenState)
{
    ActiveSecret activeSecret;
    activeSecret.id = id;
    activeSecret.root.hero = hero;

    activeSecret.root.treeItem = new QTreeWidgetItem(ui->secretsTreeWidget);
    activeSecret.root.treeItem->setExpanded(true);
    activeSecret.root.draw();

    switch(hero)
    {
        case PALADIN:
            if(loadingScreenState != arena) activeSecret.children.append(SecretCard(AVENGE));
            activeSecret.children.append(SecretCard(NOBLE_SACRIFICE));
            activeSecret.children.append(SecretCard(REPENTANCE));
            activeSecret.children.append(SecretCard(REDEMPTION));
            activeSecret.children.append(SecretCard(SACRED_TRIAL));
            activeSecret.children.append(SecretCard(EYE_FOR_AN_EYE));
            activeSecret.children.append(SecretCard(GETAWAY_KODO));
            activeSecret.children.append(SecretCard(COMPETITIVE_SPIRIT));
        break;

        case HUNTER:
            activeSecret.children.append(SecretCard(FREEZING_TRAP));
            activeSecret.children.append(SecretCard(EXPLOSIVE_TRAP));
            activeSecret.children.append(SecretCard(BEAR_TRAP));
            if(loadingScreenState != arena) activeSecret.children.append(SecretCard(SNIPE));
            activeSecret.children.append(SecretCard(DART_TRAP));
            activeSecret.children.append(SecretCard(CAT_TRICK));
            activeSecret.children.append(SecretCard(MISDIRECTION));
            activeSecret.children.append(SecretCard(HIDDEN_CACHE));
            activeSecret.children.append(SecretCard(SNAKE_TRAP));
        break;

        case MAGE:
            activeSecret.children.append(SecretCard(MIRROR_ENTITY));
            if(loadingScreenState != arena) activeSecret.children.append(SecretCard(DDUPLICATE));
            activeSecret.children.append(SecretCard(ICE_BARRIER));
            activeSecret.children.append(SecretCard(POTION_OF_POLIMORPH));
            activeSecret.children.append(SecretCard(EFFIGY));
            activeSecret.children.append(SecretCard(VAPORIZE));
            activeSecret.children.append(SecretCard(COUNTERSPELL));
            activeSecret.children.append(SecretCard(SPELLBENDER));
            activeSecret.children.append(SecretCard(ICE_BLOCK));
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


    //Duplicates en Hand
    if(code == DDUPLICATE && !lastMinionDead.isEmpty())  emit duplicated(lastMinionDead);
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
    if(activeSecretList.isEmpty())  return;

    SecretTest secretTest;
    secretTest.code = code;
    secretTest.secretRevealedLastSecond = false;
    secretTests.enqueue(secretTest);

    QTimer::singleShot(delay, this, SLOT(discardSecretOptionDelay()));
}


void SecretsHandler::checkLastSecretOption(ActiveSecret &activeSecret)
{
    if(activeSecret.children.count() == 1)
    {
        activeSecret.root.setCode(activeSecret.children.first().getCode());
        activeSecret.root.draw();
        activeSecret.root.treeItem->removeChild(activeSecret.children.first().treeItem);
        activeSecret.children.clear();

        //No puede haber dos secretos iguales
        discardSecretOptionNow(activeSecret.root.getCode());
    }
}


void SecretsHandler::playerSpellPlayed()
{
    discardSecretOptionNow(COUNTERSPELL);
    discardSecretOptionNow(CAT_TRICK);
}


/*
 * COUNTERSPELL no crea la primera linea en el log al desvelarse, solo la segunda que aparece cuando la animacion se completa
 * Eso hace que el caso de comprobacion de SPELLBENDER tenga que ser de mas delay.
 * No queremos que SPELLBENDER se descarte de un segundo secreto cuando al lanzar un hechizo el primer secreto
 * desvela COUNTERSPELL
 */

void SecretsHandler::playerSpellObjPlayed()
{
    discardSecretOption(SPELLBENDER, 7000);//Ocultado por COUNTERSPELL
}


void SecretsHandler::playerHeroPower()
{
    discardSecretOptionNow(DART_TRAP);
}


void SecretsHandler::playerMinionPlayed(int playerMinions)
{
    discardSecretOptionNow(MIRROR_ENTITY);
    discardSecretOptionNow(POTION_OF_POLIMORPH);
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
}


void SecretsHandler::avengeTested()
{
    discardSecretOptionNow(AVENGE);
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
void SecretsHandler::playerAttack(bool isHeroFrom, bool isHeroTo)
{
    if(isHeroFrom)
    {
        //Hero -> hero
        if(isHeroTo)
        {
            discardSecretOptionNow(ICE_BARRIER);
            discardSecretOptionNow(EXPLOSIVE_TRAP);
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(MISDIRECTION);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);
        }
        //Hero -> minion
        else
        {
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
            discardSecretOptionNow(EXPLOSIVE_TRAP);
            discardSecretOptionNow(BEAR_TRAP);
            discardSecretOption(FREEZING_TRAP);//Ocultado por EXPLOSIVE_TRAP
            discardSecretOption(MISDIRECTION);//Ocultado por FREEZING_TRAP y EXPLOSIVE_TRAP
            discardSecretOption(EYE_FOR_AN_EYE);//Ocultado por NOBLE_SACRIFICE
            discardSecretOptionNow(NOBLE_SACRIFICE);
        }
        //Minion -> minion
        else
        {
            discardSecretOptionNow(FREEZING_TRAP);
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













