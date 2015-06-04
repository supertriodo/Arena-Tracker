#include "secretshandler.h"

SecretsHandler::SecretsHandler(QObject *parent, Ui::MainWindow *ui) : QObject(parent)
{
    this->ui = ui;

    completeUI();
}

SecretsHandler::~SecretsHandler()
{

}


void SecretsHandler::completeUI()
{
    ui->secretsTreeWidget->setHidden(true);

    ui->secretsTreeWidget->setColumnCount(1);
    ui->secretsTreeWidget->header()->close();
    ui->secretsTreeWidget->setIconSize(CARD_SIZE);
    ui->secretsTreeWidget->setStyleSheet("background-color: transparent;");
}


void SecretsHandler::secretPlayed(int id, SecretHero hero)
{
    ui->secretsTreeWidget->setHidden(false);

    ActiveSecret activeSecret;
    activeSecret.id = id;
    activeSecret.root.hero = hero;

    activeSecret.root.treeItem = new QTreeWidgetItem(ui->secretsTreeWidget);
    activeSecret.root.treeItem->setExpanded(true);
    activeSecret.root.draw();

    switch(hero)
    {
        case paladin:
            activeSecret.children.append(SecretCard(AVENGE));
            activeSecret.children.append(SecretCard(NOBLE_SACRIFICE));
            activeSecret.children.append(SecretCard(REPENTANCE));
            activeSecret.children.append(SecretCard(REDEMPTION));
            activeSecret.children.append(SecretCard(EYE_FOR_AN_EYE));
        break;

        case hunter:
            activeSecret.children.append(SecretCard(FREEZING_TRAP));
            activeSecret.children.append(SecretCard(EXPLOSIVE_TRAP));
            activeSecret.children.append(SecretCard(SNIPE));
            activeSecret.children.append(SecretCard(MISDIRECTION));
            activeSecret.children.append(SecretCard(SNAKE_TRAP));
        break;

        case mage:
            activeSecret.children.append(SecretCard(MIRROR_ENTITY));
            activeSecret.children.append(SecretCard(DUPLICATE));
            activeSecret.children.append(SecretCard(ICE_BARRIER));
            activeSecret.children.append(SecretCard(VAPORIZE));
            activeSecret.children.append(SecretCard(COUNTERSPELL));
            activeSecret.children.append(SecretCard(SPELLBENDER));
            activeSecret.children.append(SecretCard(ICE_BLOCK));
        break;

        case unknown:
        break;
    }

    for(QList<SecretCard>::iterator it = activeSecret.children.begin(); it != activeSecret.children.end(); it++)
    {
        it->treeItem = new QTreeWidgetItem(activeSecret.root.treeItem);
        it->draw();
        emit checkCardImage(it->code);
    }

    activeSecretList.append(activeSecret);
}


void SecretsHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        for(QList<SecretCard>::iterator it2 = it->children.begin(); it2 != it->children.end(); it2++)
        {
            if(it2->code == code)    it2->draw();
        }
    }
}


void SecretsHandler::resetSecretsInterface()
{
    ui->secretsTreeWidget->setHidden(true);
    ui->secretsTreeWidget->clear();
    activeSecretList.clear();
}


void SecretsHandler::secretRevealed(int id)
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

    if(activeSecretList.count() == 0)
    {
        ui->secretsTreeWidget->setHidden(true);
    }
}


void SecretsHandler::discardSecretOption(QString code)
{
    for(QList<ActiveSecret>::iterator it = activeSecretList.begin(); it != activeSecretList.end(); it++)
    {
        for(int i=0; i<it->children.count(); i++)
        {
            if(it->children[i].code == code)
            {
                it->root.treeItem->removeChild(it->children[i].treeItem);
                it->children.removeAt(i);

                //Comprobar unica posibilidad
                checkLastSecretOption(*it);
                break;
            }
        }
    }
}


void SecretsHandler::checkLastSecretOption(ActiveSecret activeSecret)
{
    if(activeSecret.children.count() == 1)
    {
        activeSecret.root.code = activeSecret.children.first().code;
        activeSecret.root.draw();
        activeSecret.root.treeItem->removeChild(activeSecret.children.first().treeItem);
        activeSecret.children.clear();

        //No puede haber dos secretos iguales
        discardSecretOption(activeSecret.root.code);
    }
}


void SecretsHandler::playerSpellPlayed()
{
    discardSecretOption(COUNTERSPELL);
}


void SecretsHandler::playerSpellObjPlayed()
{
    discardSecretOption(SPELLBENDER);
}


void SecretsHandler::playerMinionPlayed()
{
    discardSecretOption(MIRROR_ENTITY);
    discardSecretOption(SNIPE);
    discardSecretOption(REPENTANCE);
}


void SecretsHandler::enemyMinionDead()
{
    discardSecretOption(DUPLICATE);
    discardSecretOption(REDEMPTION);
}


void SecretsHandler::avengeTested()
{
    discardSecretOption(AVENGE);
}


void SecretsHandler::playerAttack(bool isHeroFrom, bool isHeroTo)
{
    if(isHeroFrom)
    {
        //Hero -> hero
        if(isHeroTo)
        {
            discardSecretOption(ICE_BARRIER);
            discardSecretOption(EXPLOSIVE_TRAP);
            discardSecretOption(MISDIRECTION);
            discardSecretOption(EYE_FOR_AN_EYE);
            discardSecretOption(NOBLE_SACRIFICE);
        }
        //Hero -> minion
        else
        {
            discardSecretOption(SNAKE_TRAP);
            discardSecretOption(NOBLE_SACRIFICE);
        }
    }
    else
    {
        //Minion -> hero
        if(isHeroTo)
        {
            discardSecretOption(VAPORIZE);
            discardSecretOption(ICE_BARRIER);
            discardSecretOption(EXPLOSIVE_TRAP);
            discardSecretOption(FREEZING_TRAP);
            discardSecretOption(MISDIRECTION);
            discardSecretOption(EYE_FOR_AN_EYE);
            discardSecretOption(NOBLE_SACRIFICE);
        }
        //Minion -> minion
        else
        {
            discardSecretOption(FREEZING_TRAP);
            discardSecretOption(SNAKE_TRAP);
            discardSecretOption(NOBLE_SACRIFICE);
        }
    }
}

















