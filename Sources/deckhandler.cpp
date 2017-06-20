#include "deckhandler.h"
#include "Utils/deckstringhandler.h"
#include "themehandler.h"
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

DeckHandler::DeckHandler(QObject *parent, Ui::Extended *ui, EnemyDeckHandler *enemyDeckHandler, PlanHandler *planHandler) : QObject(parent)
{
    this->ui = ui;
    this->inGame = false;
    this->inArena = false;
    this->transparency = Opaque;
    this->drawAnimating = false;
    this->rngAnimating = false;
    this->drawDisappear = 10;
    this->loadedDeckName = QString();
    this->loadDeckItemsMap.clear();
    this->mouseInApp = false;
    this->enemyDeckHandler = enemyDeckHandler;
    this->planHandler = planHandler;
    this->showManaLimits = false;
    this->lastCreatedByCode = "";

    completeUI();
    unlockDeckInterface();
}

DeckHandler::~DeckHandler()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
    drawCardList.clear();
    delete bombWindow;
}


void DeckHandler::completeUI()
{
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);
    ui->deckButtonRemove->setEnabled(false);
    hideDeckButtons();

    enableDeckButtonSave(false);
    ui->deckButtonDeleteDeck->setEnabled(false);

    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->drawListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->drawListWidget->setFixedHeight(0);
    ui->drawListWidget->setMouseTracking(true);

    ui->deckListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->deckListWidget->setMouseTracking(true);

    ui->rngListWidget->setHidden(true);
    ui->rngListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->rngListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->rngListWidget->setFixedHeight(0);
    ui->rngListWidget->setMouseTracking(true);

    createLoadDeckTreeWidget();

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(enableDeckButtons()));
    connect(ui->deckListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(cardTotalPlus(QListWidgetItem*)));
    connect(ui->deckListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDeckCardEntered(QListWidgetItem*)));
    connect(ui->deckListWidget, SIGNAL(xLeave()),
            this, SLOT(deselectRow()));
    connect(ui->drawListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDrawCardEntered(QListWidgetItem*)));
    connect(ui->deckButtonMin, SIGNAL(clicked()),
            this, SLOT(cardTotalMin()));
    connect(ui->deckButtonPlus, SIGNAL(clicked()),
            this, SLOT(cardTotalPlus()));
    connect(ui->deckButtonRemove, SIGNAL(clicked()),
            this, SLOT(cardRemove()));

    connect(ui->deckLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(enableDeckButtonSave()));
    connect(ui->deckLineEdit, SIGNAL(returnPressed()),
            this, SLOT(saveDeck()));
    connect(ui->deckButtonSave, SIGNAL(clicked()),
            this, SLOT(saveDeck()));
    connect(ui->deckButtonLoad, SIGNAL(clicked()),
            this, SLOT(toggleLoadDeckTreeWidget()));
    connect(ui->deckButtonDeleteDeck, SIGNAL(clicked()),
            this, SLOT(removeDeck()));

    addNewDeckMenu(ui->deckButtonNew);
    createBombWindow();
}


void DeckHandler::createBombWindow()
{
    bombWindow = new BombWindow((QWidget*)this->parent(), ui->planGraphicsView->scene());
    connect(ui->rngListWidget,SIGNAL(leave()),
            bombWindow, SLOT(hide()));
    connect(ui->rngListWidget,SIGNAL(leave()),
            planHandler, SLOT(resetDeadProbs()));
    connect(ui->rngListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(rngCardEntered(QListWidgetItem*)));
}


void DeckHandler::rngCardEntered(QListWidgetItem * item)
{
    RngCard rngCard = rngCardList[ui->rngListWidget->row(item)];
    QString code = rngCard.getCode();
    planHandler->showLastTurn();
    planHandler->resetDeadProbs();
    showBombWindow();
    planHandler->checkBomb(code);
}


void DeckHandler::showBombWindow()
{
    QSize sizeTab = ui->tabEnemy->geometry().size();
    QPoint topLeft = ui->tabEnemy->mapToGlobal(ui->tabEnemy->geometry().topLeft());
    QRect rectTab(topLeft, sizeTab);
    bombWindow->showAt(rectTab);
}


void DeckHandler::newRngCard(QString code, int id)
{
    RngCard rngCard(code);
    rngCard.id = id;
    rngCard.listItem = new QListWidgetItem();
    rngCardList.append(rngCard);
    ui->rngListWidget->addItem(rngCard.listItem);
    rngCard.draw();
    emit checkCardImage(code);
    if(showRngList)     ui->rngListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustRngSize()));
}


void DeckHandler::removeRngCard(int id, QString code)
{
    (void) code;

    int i=0;
    for (QList<RngCard>::iterator it = rngCardList.begin(); it != rngCardList.end(); it++, i++)
    {
        if(it->id == id)
        {
            delete it->listItem;
            rngCardList.removeAt(i);
            QTimer::singleShot(10, this, SLOT(adjustRngSize()));
            return;
        }
    }
}


void DeckHandler::adjustRngSize()
{
    if(rngAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustRngSize()));
        return;
    }

    int rowHeight = ui->rngListWidget->sizeHintForRow(0);
    int rows = rngCardList.count();
    int height = rows*rowHeight + 2*ui->rngListWidget->frameWidth();
    int maxHeight = (ui->rngListWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->rngListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->rngListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->rngListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->rngListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->rngAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearRngAnimating()));
}


void DeckHandler::clearRngAnimating()
{
    this->rngAnimating = false;
    if(rngCardList.empty())    ui->rngListWidget->setHidden(true);
}


void DeckHandler::setShowRngList(bool value)
{
    this->showRngList = value;
    if(!value)                      ui->rngListWidget->setHidden(true);
    else if(!rngCardList.empty())   ui->rngListWidget->setHidden(false);
}


void DeckHandler::addNewDeckMenu(QPushButton *button)
{
    QMenu *newDeckMenu = new QMenu(button);

    QAction *action = newDeckMenu->addAction("New empty deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newEmptyDeck()));

    action = newDeckMenu->addAction("New from clipboard");
    connect(action, SIGNAL(triggered()), this, SLOT(newImportDeckString()));

    action = newDeckMenu->addAction("Copy");
    connect(action, SIGNAL(triggered()), this, SLOT(exportDeckString()));

    action = newDeckMenu->addAction("Clone current deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newCopyCurrentDeck()));

    action = newDeckMenu->addAction("Clone enemy deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newCopyEnemyDeck()));

    button->setMenu(newDeckMenu);
}


void DeckHandler::createLoadDeckTreeWidget()
{
    QTreeWidget *treeWidget = ui->loadDeckTreeWidget;
    treeWidget->setColumnCount(1);
    treeWidget->setIconSize(QSize(32,32));
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setHidden(true);
    treeWidget->setFixedHeight(0);

    for(int i=0; i<9; i++)
    {
        loadDeckClasses[i] = new QTreeWidgetItem(treeWidget);
        loadDeckClasses[i]->setHidden(true);
        loadDeckClasses[i]->setExpanded(true);
        loadDeckClasses[i]->setText(0, Utility::getHeroName(i));
        loadDeckClasses[i]->setForeground(0, QBrush(QColor(Utility::getHeroColor(i))));
    }

    loadDeckClasses[9] = new QTreeWidgetItem(treeWidget);
    loadDeckClasses[9]->setHidden(true);
    loadDeckClasses[9]->setExpanded(true);
    loadDeckClasses[9]->setText(0, "Multi class");
    loadDeckClasses[9]->setIcon(0, QIcon(":/Images/secretHunter.png"));
    loadDeckClasses[9]->setForeground(0, QBrush(QColor(Utility::getHeroColor(9))));

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(unselectClassItems()));
    connect(treeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(hideIfDeckSelected()));
}


bool DeckHandler::isItemClass(QTreeWidgetItem *item)
{
    for(int i=0; i<10; i++) if(item == loadDeckClasses[i])  return true;
    return false;
}


void DeckHandler::unselectClassItems()
{
    if(ui->loadDeckTreeWidget->selectedItems().isEmpty())   return;
    QTreeWidgetItem *item = ui->loadDeckTreeWidget->selectedItems().first();
    if(isItemClass(item))   ui->loadDeckTreeWidget->clearSelection();
}


void DeckHandler::hideIfDeckSelected()
{
    if(ui->loadDeckTreeWidget->selectedItems().isEmpty())   return;
    QTreeWidgetItem *item = ui->loadDeckTreeWidget->selectedItems().first();
    if(!isItemClass(item))  hideDeckTreeWidget();
}


void DeckHandler::loadSelectedDeck()
{
    if(ui->loadDeckTreeWidget->selectedItems().isEmpty())   return;
    QTreeWidgetItem *item = ui->loadDeckTreeWidget->selectedItems().first();
    if(item!=NULL && !isItemClass(item))  loadDeck(item->text(0));
}


void DeckHandler::toggleLoadDeckTreeWidget()
{
    if(ui->loadDeckTreeWidget->isHidden())  showDeckTreeWidget();
    else                                    hideDeckTreeWidget();
}


void DeckHandler::showDeckTreeWidget()
{
    ui->deckButtonLoad->setEnabled(false);
    ui->loadDeckTreeWidget->setHidden(false);
    ui->loadDeckTreeWidget->clearSelection();
    int totalHeight = ui->deckListWidget->height();

    //Show DeckTreeWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckTreeWidget()));

    animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckTreeWidget()));

    //Hide DeckListWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation2, SIGNAL(finished()),
            this, SLOT(finishHideDeckListWidget()));
}


void DeckHandler::finishShowDeckTreeWidget()
{
    ui->loadDeckTreeWidget->setMinimumHeight(0);
    ui->loadDeckTreeWidget->setMaximumHeight(16777215);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::finishHideDeckListWidget()
{
    ui->deckListWidget->setHidden(true);
    ui->deckListWidget->setFixedHeight(0);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::hideDeckTreeWidget()
{
    loadSelectedDeck();
    ui->deckButtonLoad->setEnabled(false);
    ui->deckListWidget->setHidden(false);
    int totalHeight = ui->loadDeckTreeWidget->height();

    //Show DeckListWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->deckListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckListWidget()));

    animation = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(HIDE_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckListWidget()));

    //Hide DeckTreeWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(HIDE_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation2, SIGNAL(finished()),
            this, SLOT(finishHideDeckTreeWidget()));
}


void DeckHandler::finishHideDeckTreeWidget()
{
    ui->loadDeckTreeWidget->setHidden(true);
    ui->loadDeckTreeWidget->setFixedHeight(0);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::finishShowDeckListWidget()
{
    ui->deckListWidget->setMinimumHeight(0);
    ui->deckListWidget->setMaximumHeight(16777215);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::enableDeckButtonSave(bool enable)
{
//    if(enable)  emit pDebug("Save button enabled");
//    else        emit pDebug("Save button disabled");
    if(inArena) enable = false;
    ui->deckButtonSave->setEnabled(enable);
}


void DeckHandler::adjustDrawSize()
{
    if(drawAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustDrawSize()));
        return;
    }

    int rowHeight = ui->drawListWidget->sizeHintForRow(0);
    int rows = drawCardList.count();
    int height = rows*rowHeight + 2*ui->drawListWidget->frameWidth();
    int maxHeight = (ui->drawListWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->drawListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->drawListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->drawListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->drawListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->drawAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearDrawAnimating()));
}


void DeckHandler::clearDrawAnimating()
{
    this->drawAnimating = false;
    if(drawCardList.empty())    ui->drawListWidget->setHidden(true);
}


void DeckHandler::reset()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
    clearDrawList(true);

    DeckCard deckCard("");
    deckCard.total = 30;
    deckCard.remaining = deckCard.total;
    deckCard.listItem = new QListWidgetItem();
    deckCard.draw();
    insertDeckCard(deckCard); //Lanza emit deckSizeChanged();

    enableDeckButtons();

    emit pDebug("Deck list cleared.");
}


QList<DeckCard> * DeckHandler::getDeckComplete()
{
    if(deckCardList[0].total==0)    return &deckCardList;
    else    return NULL;
}


QList<DeckCard> DeckHandler::getDeckCardList()
{
    return deckCardList;
}


int DeckHandler::getNumCardRows()
{
    int numCards = deckCardList.count();
    if(deckCardList[0].listItem->isHidden())    numCards--;
    return numCards;
}


void DeckHandler::newDeckCardAsset(QString code)
{
    newDeckCard(code, 1, true);
}


void DeckHandler::newDeckCardDraft(QString code)
{
    newDeckCard(code, 1, true);
}


void DeckHandler::newDeckCardWeb(QString code, int total)
{
    newDeckCard(code, total, false);
}


void DeckHandler::newDeckCardOutsider(QString code, int id)
{
    newDeckCard(code, 1, false, true, id);
}


void DeckHandler::newDeckCard(QString code, int total, bool add, bool outsider, int id)
{
    if(outsider)
    {
        if(code.isEmpty() && this->lastCreatedByCode.isEmpty())     return;
    }
    else
    {
        if(code.isEmpty())  return;
    }

    //Mazo completo
    if(!outsider && (deckCardList[0].total < (uint)total))
    {
        emit pDebug("Deck is full: Not adding: (" + QString::number(total) + ") " +
                    Utility::getCardAttribute(code, "name").toString(), Warning);
        return;
    }

    //Ya existe en el mazo
    bool found = false;
    if(!outsider)
    {
        for(int i=0; i<deckCardList.length(); i++)
        {
            if(deckCardList[i].getCode() == code)
            {
                if(!add)
                {
                    emit pDebug(Utility::getCardAttribute(code, "name").toString() + " already in deck.");
                    return;
                }

                found = true;
                deckCardList[i].total+=total;
                deckCardList[i].remaining+=total;
                deckCardList[i].draw();
                break;
            }
        }
    }

    if(!found)
    {
        DeckCard deckCard(code, outsider);
        deckCard.total = total;
        deckCard.remaining = total;
        deckCard.listItem = new QListWidgetItem();

        //Outsider
        if(outsider)
        {
            deckCard.id = id;

            if(code.isEmpty())
            {
                deckCard.special = true;
                deckCard.setCreatedByCode(this->lastCreatedByCode);
                emit checkCardImage(this->lastCreatedByCode);
            }
        }
        insertDeckCard(deckCard);
        deckCard.draw();
        emit checkCardImage(code);
        updateManaLimits();
    }

    if(!outsider)
    {
        deckCardList[0].total-=total;
        deckCardList[0].remaining = deckCardList[0].total;
        deckCardList[0].draw();
        if(deckCardList[0].total == 0)  hideUnknown();
    }

    if(!this->inArena && !outsider)   enableDeckButtonSave();

    emit pDebug("Add to deck" + (outsider?QString(" OUTSIDER"):QString("")) + ": (" + QString::number(total) + ")" +
                Utility::getCardAttribute(code, "name").toString());
}


void DeckHandler::insertDeckCard(DeckCard &deckCard)
{
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCard.getCost() < deckCardList[i].getCost())
        {
            deckCardList.insert(i, deckCard);
            ui->deckListWidget->insertItem(i, deckCard.listItem);
            emit deckSizeChanged();
            return;
        }
        else if(deckCard.getCost() == deckCardList[i].getCost())
        {
            /*if(deckCard.getType() != deckCardList[i].getType())//Weapon-->Spells-->Minions
            {
                if(deckCard.getType() == WEAPON || deckCardList[i].getType() == MINION)
                {
                    deckCardList.insert(i, deckCard);
                    ui->deckListWidget->insertItem(i, deckCard.listItem);
                    emit deckSizeChanged();
                    return;
                }
            }
            else */if(deckCard.getName().toLower() < deckCardList[i].getName().toLower())
            {
                deckCardList.insert(i, deckCard);
                ui->deckListWidget->insertItem(i, deckCard.listItem);
                emit deckSizeChanged();
                return;
            }
        }
    }
    deckCardList.append(deckCard);
    ui->deckListWidget->addItem(deckCard.listItem);
    emit deckSizeChanged();
}


void DeckHandler::updateManaLimits()
{
    if(!showManaLimits)  return;

    for(int i=1, mana=-1; i<deckCardList.length(); i++)
    {
        int manaCard = deckCardList[i].getCost();
        deckCardList[i].resetManaLimits();

        if(mana != manaCard)
        {
            if(i > 1 && mana < 6)
            {
                deckCardList[i-1].setManaLimit(false);
                deckCardList[i].setManaLimit(true);
            }
            mana = manaCard;
        }
    }
}


void DeckHandler::setShowManaLimits(bool value)
{
    this->showManaLimits = value;
    if(showManaLimits)  updateManaLimits();
    else                for(int i=1; i<deckCardList.length(); i++)     deckCardList[i].resetManaLimits();
}


void DeckHandler::hideUnknown(bool hidden)
{
    deckCardList[0].listItem->setHidden(hidden);
    emit deckSizeChanged();
}


void DeckHandler::newDrawCard(QString code, bool mulligan)
{
    DrawCard drawCard(code);
    drawCard.listItem = new QListWidgetItem();
    drawCardList.append(drawCard);
    ui->drawListWidget->addItem(drawCard.listItem);
    drawCard.draw();
    emit checkCardImage(code);
    ui->drawListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));

    if(!mulligan && this->drawDisappear>0)
    {
        QTimer::singleShot(this->drawDisappear*1000, this, SLOT(removeOldestDrawCard()));
    }
}


void DeckHandler::removeOldestDrawCard()
{
    if(drawCardList.empty())    return;
    DrawCard drawCard = drawCardList.takeFirst();
    ui->drawListWidget->removeItemWidget(drawCard.listItem);
    delete drawCard.listItem;
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));
}


void DeckHandler::playerCardDraw(QString code, int id)
{
    drawFromDeck(code, id);
}


void DeckHandler::playerCardToHand(int id, QString code, int turn)
{
    if(this->drawDisappear>=0)          newDrawCard(code, turn==0);
    if(planHandler->isCardBomb(code))   newRngCard(code, id);
}


void DeckHandler::drawFromDeck(QString code, int id)
{
    //Avoid special cards
    if(code == THE_COIN)    return;

    //Check outsiders (por id), todos los outsiders tienen id
    for(int i=1; i<deckCardList.length(); i++)
    {
        DeckCard *card = &deckCardList[i];
        if(card->isOutsider() && card->id == id)
        {
            if(card->remaining > 1)
            {
                card->remaining--;
                card->draw();
            }
            else
            {
                removeFromDeck(i);
                i--;
            }
            return;
        }
    }

    //Check normal deck
    for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining > 0)
            {
                it->remaining--;
                it->draw();
            }
            //it->remaining == 0
            //Reajustamos el mazo si tiene unknown cards
            else if(deckCardList[0].total > 0 && !it->isOutsider())
            {
                deckCardList[0].total--;
                deckCardList[0].remaining = deckCardList[0].total;
                if(deckCardList[0].total == 0)  hideUnknown();
                else                            deckCardList[0].draw();

                it->total++;
                it->draw();

                emit pDebug("New card: " + it->getName() + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total));
            }
            else
            {
                emit pDebug("New card but deck is full. " + it->getName() + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total), Warning);
            }

            //Id -- Nos permite saber el code de las starting cards para devolverlas al deck durante el mulligan.
            cardId2Code[id] = code;

            return;
        }
    }

    if(deckCardList[0].total>0)
    {
        emit pDebug("New card: " +
                          Utility::getCardAttribute(code, "name").toString() + ". 0/1");
        newDeckCard(code);
        drawFromDeck(code, id);
    }
    else
    {
        emit pDebug("New card but deck is full. " +
                      Utility::getCardAttribute(code, "name").toString(), Warning);
    }
}


void DeckHandler::returnToDeck(QString code, int id)
{
    //Nos permite saber el code de las starting cards para devolverlas al deck durante el mulligan.
    if(cardId2Code.contains(id))
    {
        code = cardId2Code[id];
    }

    if(!code.isEmpty())
    {
        for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
        {
            if(it->getCode() == code)
            {
                it->remaining++;
                it->draw();
                emit pDebug("Return to deck: " + code + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total));
                return;
            }
        }
    }

    newDeckCardOutsider(code, id);
}


void DeckHandler::setLastCreatedByCode(QString code, QString blockType)
{
    if(blockType == "JOUST")                this->lastCreatedByCode = "";
    else if(isLastCreatedByCodeValid(code)) this->lastCreatedByCode = code;
    else                                    emit pDebug("CreatedBy code is in the forbidden list: " + code, Warning);
}


void DeckHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code || it->getCreatedByCode() == code)
        {
            it->draw();
        }
    }

    for(QList<DrawCard>::iterator it = drawCardList.begin(); it != drawCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->draw();
        }
    }

    for(QList<RngCard>::iterator it = rngCardList.begin(); it != rngCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->draw();
        }
    }
}


void DeckHandler::redrawClassCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        if(deckCard.getCardClass()<9)
        {
            deckCard.draw();
        }
    }

    foreach(DrawCard drawCard, drawCardList)
    {
        if(drawCard.getCardClass()<9)
        {
            drawCard.draw();
        }
    }

    foreach(RngCard rngCard, rngCardList)
    {
        if(rngCard.getCardClass()<9)
        {
            rngCard.draw();
        }
    }
}


void DeckHandler::redrawSpellWeaponCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        CardType cardType = deckCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            deckCard.draw();
        }
    }

    foreach(DrawCard drawCard, drawCardList)
    {
        CardType cardType = drawCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            drawCard.draw();
        }
    }

    foreach(RngCard rngCard, rngCardList)
    {
        CardType cardType = rngCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            rngCard.draw();
        }
    }
}


void DeckHandler::redrawAllCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        deckCard.draw();
    }

    foreach(DrawCard drawCard, drawCardList)
    {
        drawCard.draw();
    }

    foreach(RngCard rngCard, rngCardList)
    {
        rngCard.draw();
    }
}


void DeckHandler::updateIconSize(int cardHeight)
{
    int iconHeight = cardHeight*52/35;
    if(iconHeight<52)   iconHeight=52;
    ui->deckButtonNew->setIconSize(QSize(iconHeight, iconHeight));
    ui->deckButtonLoad->setIconSize(QSize(iconHeight, iconHeight));
    ui->deckButtonSave->setIconSize(QSize(iconHeight, iconHeight));
    ui->deckButtonDeleteDeck->setIconSize(QSize(iconHeight, iconHeight));

    //Evita que new deck coja mas espacio
    ui->deckButtonLoad->setMinimumWidth(iconHeight);
    ui->deckButtonSave->setMinimumWidth(iconHeight);
    ui->deckButtonDeleteDeck->setMinimumWidth(iconHeight);
}


void DeckHandler::enableDeckButtons()
{
    int index = ui->deckListWidget->currentRow();

    if(index>0 && deckCardList[index].total > 1)
                                        ui->deckButtonMin->setEnabled(true);
    else                                ui->deckButtonMin->setEnabled(false);
    if(index>0 && deckCardList[index].total == 1)
                                        ui->deckButtonRemove->setEnabled(true);
    else                                ui->deckButtonRemove->setEnabled(false);
    if(index>0 && deckCardList.first().total > 0 &&
            (inArena |
                ((deckCardList[index].total == 1) && (deckCardList[index].getRarity() != LEGENDARY))
            )
        )
                                        ui->deckButtonPlus->setEnabled(true);
    else                                ui->deckButtonPlus->setEnabled(false);


    if(index>0)
    {
        showDeckButtons();
    }
    else
    {
        hideDeckButtons();
    }
}


void DeckHandler::showDeckButtons()
{
    if(ui->deckButtonMin->isHidden())
    {
        ui->tabDeckLayout->removeItem(ui->horizontalLayoutDeckButtons);
        ui->tabDeckLayout->insertItem(1, ui->horizontalLayoutDeckButtons);
        ui->deckButtonMin->setHidden(false);
        ui->deckButtonPlus->setHidden(false);
        ui->deckButtonRemove->setHidden(false);
    }
}


void DeckHandler::hideDeckButtons()
{
    ui->deckButtonMin->setHidden(true);
    ui->deckButtonPlus->setHidden(true);
    ui->deckButtonRemove->setHidden(true);
    ui->tabDeckLayout->removeItem(ui->horizontalLayoutDeckButtons);
}


void DeckHandler::deselectRow()
{
    ui->deckListWidget->setCurrentRow(-1);
}


void DeckHandler::cardTotalMin()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total--;
    deckCardList[index].remaining = deckCardList[index].total;
    deckCardList[0].total++;
    deckCardList[0].remaining = deckCardList[0].total;

    deckCardList[index].draw();
    if(deckCardList[0].total==1)    hideUnknown(false);
    deckCardList[0].draw();
    enableDeckButtons();

    enableDeckButtonSave();
}


void DeckHandler::cardTotalPlus()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total++;
    deckCardList[index].remaining = deckCardList[index].total;
    deckCardList[0].total--;
    deckCardList[0].remaining = deckCardList[0].total;

    deckCardList[index].draw();
    if(deckCardList[0].total==0)    hideUnknown();
    else                            deckCardList[0].draw();
    enableDeckButtons();

    enableDeckButtonSave();
}


void DeckHandler::cardRemove()
{
    int index = ui->deckListWidget->currentRow();
    if(deckCardList[index].total!=1 || index==0)
    {
        enableDeckButtons();
        return;
    }

    removeFromDeck(index);

    deckCardList[0].total++;
    deckCardList[0].remaining = deckCardList[0].total;
    if(deckCardList[0].total==1)    hideUnknown(false);
    deckCardList[0].draw();
    enableDeckButtons();

    enableDeckButtonSave();
    emit deckSizeChanged();
}


void DeckHandler::cardTotalPlus(QListWidgetItem *item)
{
    item->setSelected(true);
    int index = ui->deckListWidget->row(item);
    if(index > 0 &&
        deckCardList.first().total > 0 &&
        (inArena |
            ((deckCardList[index].total == 1) && (deckCardList[index].getRarity() != LEGENDARY))
        ))
    {
        cardTotalPlus();
    }
}


void DeckHandler::removeFromDeck(int index)
{
    ui->deckListWidget->removeItemWidget(deckCardList[index].listItem);
    delete deckCardList[index].listItem;
    deckCardList.removeAt(index);
    updateManaLimits();
}


void DeckHandler::lockDeckInterface()
{
    emit pDebug("Lock deck interface.");

    this->inGame = true;

    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        it->remaining = it->total;
    }

    ui->deckListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->deckListWidget->selectionModel()->reset();
    ui->deckListWidget->clearFocus();
    ui->deckListWidget->setFocusPolicy(Qt::NoFocus);
    hideDeckButtons();
    hideManageDecksButtons();

    updateTransparency();
    clearDrawList(true);
    clearRngList();
    this->lastCreatedByCode = "";
}


void DeckHandler::unlockDeckInterface()
{
    emit pDebug("Unlock deck interface.");

    this->inGame = false;

    for(int i=0; i<deckCardList.length(); i++)
    {
        DeckCard *card = &deckCardList[i];
        if(card->isOutsider())
        {
            removeFromDeck(i);
            i--;
        }
        else
        {
            card->id = 0;

            if(card->total > 0)
            {
                card->remaining = card->total;
                card->draw();
                card->listItem->setHidden(false);
            }
            else    card->listItem->setHidden(true);
        }
    }

    cardId2Code.clear();

    ui->deckListWidget->setFocusPolicy(Qt::ClickFocus);
    ui->deckListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    showManageDecksButtons();

    updateTransparency();
    clearDrawList(true);
    clearRngList();
}


void DeckHandler::updateTransparency()
{
    if(!mouseInApp && (transparency==Transparent || (inGame && transparency==AutoTransparent)))
    {
        ui->tabDeck->setAttribute(Qt::WA_NoBackground);
        ui->tabDeck->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabDeck)
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabDeck->setAttribute(Qt::WA_NoBackground, false);
        ui->tabDeck->repaint();

        if(transparency==AutoTransparent && ui->tabWidget->currentWidget()==ui->tabDeck)
        {
            emit needMainWindowFade(false);
        }
    }
}


void DeckHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void DeckHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}


void DeckHandler::setTheme()
{
    ui->deckButtonDeleteDeck->setIcon(QIcon(ThemeHandler::buttonRemoveDeckFile()));
    ui->deckButtonLoad->setIcon(QIcon(ThemeHandler::buttonLoadDeckFile()));
    ui->deckButtonNew->setIcon(QIcon(ThemeHandler::buttonNewDeckFile()));
    ui->deckButtonSave->setIcon(QIcon(ThemeHandler::buttonSaveDeckFile()));

    ui->deckButtonMin->setIcon(QIcon(ThemeHandler::buttonMinFile()));
    ui->deckButtonPlus->setIcon(QIcon(ThemeHandler::buttonPlusFile()));
    ui->deckButtonRemove->setIcon(QIcon(ThemeHandler::buttonRemoveFile()));


    ui->loadDeckTreeWidget->setTheme(true);
    ui->deckListWidget->setTheme();
    bombWindow->setTheme();

    for(int i=0; i<9; i++)
    {
        loadDeckClasses[i]->setIcon(0, QIcon(ThemeHandler::heroFile(Utility::getHeroLogNumber(i))));
    }
}


void DeckHandler::clearRngList()
{
    ui->rngListWidget->clear();
    ui->rngListWidget->setHidden(true);
    ui->rngListWidget->setFixedHeight(0);
    rngCardList.clear();
}


void DeckHandler::clearDrawList(bool forceClear)
{
    if(!forceClear && this->drawDisappear>0)     return;

    ui->drawListWidget->clear();
    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setFixedHeight(0);
    drawCardList.clear();

    emit pDebug("Clear Draw List.");
}


void DeckHandler::setDrawDisappear(int value)
{
    this->drawDisappear = value;
    clearDrawList(true);
}


void DeckHandler::findDeckCardEntered(QListWidgetItem * item)
{
    DeckCard deckCard = deckCardList[ui->deckListWidget->row(item)];
    QString code = deckCard.getCode();
    if(code.isEmpty())  code = deckCard.getCreatedByCode();

    QRect rectCard = ui->deckListWidget->visualItemRect(item);
    QPoint posCard = ui->deckListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int deckListTop = ui->deckListWidget->mapToGlobal(QPoint(0,0)).y();
    int deckListBottom = ui->deckListWidget->mapToGlobal(QPoint(0,ui->deckListWidget->height())).y();
    emit cardEntered(code, globalRectCard, deckListTop, deckListBottom);
}


void DeckHandler::findDrawCardEntered(QListWidgetItem * item)
{
    QString code = drawCardList[ui->drawListWidget->row(item)].getCode();

    QRect rectCard = ui->drawListWidget->visualItemRect(item);
    QPoint posCard = ui->drawListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int drawListTop = -1;
    int drawListBottom = ui->tabEnemy->mapToGlobal(QPoint(0,ui->tabEnemy->height())).y();
    emit cardEntered(code, globalRectCard, drawListTop, drawListBottom);
}


/*
 * ObjectDecks
 * {
 * deck1 -> ObjectDeck
 * deck2 -> ObjectDeck
 * }
 *
 * ObjectDeck
 * {
 * id1 -> nCards
 * id2 -> nCards
 * hero -> heroLog
 * }
 */
void DeckHandler::loadDecks()
{
    //Iniciamos deckCardList con 30 cartas desconocidas
    enableDeckButtonSave(false);
    newEmptyDeck();

    //Load decks from file
    QFile jsonFile(Utility::dataPath() + "/ArenaTrackerDecks.json");
    if(!jsonFile.exists())
    {
        emit pDebug("Json decks file doesn't exists.");
        emit pLog("Deck: Loaded 0 decks.");
        return;
    }

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Failed to load Arena Tracker decks json from disk.", Error);
        emit pLog(tr("File: ERROR: Loading Arena Tracker decks json from disk."));
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    decksJson = jsonDoc.object();

    emit pDebug("Loaded " + QString::number(decksJson.count()) + " decks from json file.");
    emit pLog("Deck: Loaded " + QString::number(decksJson.count()) + " decks.");

    //Load decks to loadDeckTreeWidget
    int deckLoaded = 0;
    foreach(QString deckName, decksJson.keys())
    {
        addDeckToLoadTree(deckName);
        deckLoaded++;
    }

    if(deckLoaded > 50)     ui->loadDeckTreeWidget->collapseAll();
}


void DeckHandler::addDeckToLoadTree(QString deckName)
{
    if(!decksJson.contains(deckName))
    {
        emit pDebug("Deck " + deckName + " not found. Adding to loadDeckTreeWidget.", Error);
        return;
    }

    int indexClassArray[9] = {8,6,5,3,1,0,7,2,4};
    QString heroLog = decksJson[deckName].toObject()["hero"].toString();
    int numberClass = heroLog.toInt()-1;
    int indexClass;
    if(numberClass<0 || numberClass>8)      indexClass = 9;
    else                                    indexClass = indexClassArray[numberClass];
    QTreeWidgetItem *deckClass = loadDeckClasses[indexClass];
    if(deckClass->isHidden())   deckClass->setHidden(false);

    //Create item
    QTreeWidgetItem *item = new QTreeWidgetItem(deckClass);
    item->setText(0, deckName);
    item->setToolTip(0, deckName);
    item->setForeground(0, QBrush(QColor(Utility::getHeroColor(indexClass))));
    loadDeckItemsMap[deckName] = item;
    deckClass->sortChildren(0, Qt::AscendingOrder);
}


void DeckHandler::removeDeckFromLoadTree(QString deckName)
{
    QTreeWidgetItem *item = loadDeckItemsMap[deckName];
    if(item != NULL)
    {
        QTreeWidgetItem *itemClass = item->parent();
        delete item;
        if(itemClass != NULL && (itemClass->childCount()==0))   itemClass->setHidden(true);
    }

    loadDeckItemsMap.remove(deckName);
}


void DeckHandler::loadDeck(QString deckName)
{
    if(ui->deckButtonSave->isEnabled() && !askSaveDeck())   return;

    if(!decksJson.contains(deckName))
    {
        emit pDebug("Deck " + deckName + " not found. Trying to load.", Error);
        return;
    }

    reset();
    QJsonObject jsonObjectDeck = decksJson[deckName].toObject();

    //Desactiva showManaLimits durante la carga
    bool oldShowManaLimits = showManaLimits;
    showManaLimits = false;
    foreach(QString key, jsonObjectDeck.keys())
    {
        if(key != "hero")   newDeckCard(key, jsonObjectDeck[key].toInt());
    }
    showManaLimits = oldShowManaLimits;
    updateManaLimits();

    loadedDeckName = deckName;
    ui->deckLineEdit->setText(deckName);
    enableDeckButtonSave(false);
    ui->deckButtonDeleteDeck->setEnabled(true);

    emit pDebug("Deck " + deckName + " loaded.");
    emit pLog("Deck: " + deckName + " loaded.");
}


void DeckHandler::saveDeck()
{
    if(!ui->deckButtonSave->isEnabled())    return;

    //Create json deck
    QJsonObject jsonObjectDeck;
    QString hero = "";

    foreach(DeckCard deckCard, deckCardList)
    {
        QString code = deckCard.getCode();
        int total = deckCard.total;
        if(!code.isEmpty() && total > 0)
        {
            jsonObjectDeck.insert(deckCard.getCode(), (int)deckCard.total);
            if(hero.isEmpty())
            {
                hero = Utility::heroToLogNumber(deckCard.getCardClass());
            }
        }
    }
    jsonObjectDeck.insert("hero", hero);

    //Remove existing json deck
    if(!loadedDeckName.isNull())
    {
        decksJson.remove(loadedDeckName);
        emit pDebug("Removed " + loadedDeckName + " from decksJson.");
    }

    //Verificar nombre unico
    QString deckName = ui->deckLineEdit->text();
    if(decksJson.contains(deckName))
    {
        int num = 2;
        while(decksJson.contains(deckName + " " + QString::number(num)))    num++;
        deckName = deckName + " " + QString::number(num);
        ui->deckLineEdit->setText(deckName);
    }

    //Add json deck
    decksJson.insert(deckName, jsonObjectDeck);

    //Update load deck tree
    if(loadedDeckName.isNull())
    {
        addDeckToLoadTree(deckName);
    }
    else if(loadedDeckName != deckName)
    {
        removeDeckFromLoadTree(loadedDeckName);
        addDeckToLoadTree(deckName);
    }

    loadedDeckName = deckName;
    enableDeckButtonSave(false);
    ui->deckButtonDeleteDeck->setEnabled(true);

    emit pDebug("Added " + deckName + " to decksJson.");
    emit pLog("Deck: " + deckName + " saved.");

    //Save to disk
    saveDecksJsonFile();
}


void DeckHandler::saveDecksJsonFile()
{
    //Build json data from decksJson
    QJsonDocument jsonDoc;
    jsonDoc.setObject(decksJson);


    //Save to disk
    QFile jsonFile(Utility::dataPath() + "/ArenaTrackerDecks.json");
    if(jsonFile.exists())   jsonFile.remove();

    if(!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pDebug("Failed to create Arena Tracker decks json on disk.", Error);
        emit pLog(tr("File: ERROR: Creating Arena Tracker decks json on disk."));
        return;
    }
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    emit pDebug("Decks json file updated.");
}


QString DeckHandler::getNewDeckName()
{
    QString newDeckName = "New deck";
    if(!decksJson.contains(newDeckName))    return newDeckName;

    int num = 2;
    while(decksJson.contains(newDeckName + " " + QString::number(num)))    num++;
    return newDeckName + " " + QString::number(num);
}


bool DeckHandler::askSaveDeck()
{
    QString deckName = ui->deckLineEdit->text();

    int ret = QMessageBox::warning(ui->tabDeck, "Save " + deckName + "?",
            deckName + " has unsaved changes.",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

    if(ret == QMessageBox::Save)
    {
        saveDeck();
        return true;
    }
    else if(ret == QMessageBox::Discard)    return true;
    else                                    return false;
}


void DeckHandler::newEmptyDeck()
{
    newDeck(true);
}


void DeckHandler::newCopyCurrentDeck()
{
    newDeck(false);
}


void DeckHandler::newImportDeckString()
{
    if(newDeck(true))   importDeckString();
}


void DeckHandler::newCopyEnemyDeck()
{
    if(newDeck(true))   importEnemyDeck();
}


bool DeckHandler::newDeck(bool reset)
{
    if(ui->deckButtonSave->isEnabled() && !askSaveDeck())   return false;

    if(reset)   this->reset();
    loadedDeckName = QString();
    enableDeckButtonSave(!reset);
    ui->deckButtonDeleteDeck->setEnabled(false);
    ui->deckLineEdit->setText(getNewDeckName());

    emit pDebug("New deck.");
    return true;
}


void DeckHandler::importDeckString()
{
    QString deckName;
    QString text = QApplication::clipboard()->text();
    QList<CodeAndCount> deckList = DeckStringHandler::readDeckString(text, deckName);

    if(deckList.isEmpty())
    {
        emit showMessageProgressBar("Invalid HS deck");
        emit pDebug("Invalid HS deck");
        return;
    }

    for(const CodeAndCount &codeAndCount: deckList)
    {
        QString code = codeAndCount.code;
        int count = codeAndCount.count;
        newDeckCard(code, count);
    }

    if(!deckName.isEmpty())     ui->deckLineEdit->setText(deckName);
    emit showMessageProgressBar("HS deck created");
    emit pDebug("HS deck created");
}


void DeckHandler::exportDeckString()
{
    QList<CodeAndCount> deckList;
    for(DeckCard &deckCard: deckCardList)
    {
        QString code = deckCard.getCode();
        int count = deckCard.total;
        if(!code.isEmpty())     deckList.append(CodeAndCount(code, count));
    }

    QString deckName = ui->deckLineEdit->text();
    QString text = DeckStringHandler::writeDeckString(deckList, deckName);

    if(text.isEmpty())
    {
        emit showMessageProgressBar("Invalid HS deck");
        emit pDebug("Invalid HS deck");
        return;
    }

    QApplication::clipboard()->setText(text);
    emit showMessageProgressBar("HS deck copied");
    emit pDebug("HS deck copied");
}


void DeckHandler::importEnemyDeck()
{
    QList<DeckCard> enemyDeckCardList = enemyDeckHandler->getDeckCardList();

    foreach(DeckCard deckCard, enemyDeckCardList)
    {
        if(!deckCard.getCode().isEmpty() && !deckCard.isOutsider())  newDeckCard(deckCard.getCode(), deckCard.total);
    }
}


void DeckHandler::removeDeck()
{
    if(loadedDeckName.isNull())
    {
        emit pDebug("Clicked remove deck with no loadedDeckName. Remove button should be disabled.", Warning);
        return;
    }

    //Ask user
    int ret = QMessageBox::warning(ui->tabDeck, "Remove " + loadedDeckName + "?",
            "Do you want to remove " + loadedDeckName + " from your decks.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if(ret == QMessageBox::No)  return;

    //Remove existing json deck
    decksJson.remove(loadedDeckName);
    removeDeckFromLoadTree(loadedDeckName);

    loadedDeckName = QString();
    enableDeckButtonSave(false);
    ui->deckButtonDeleteDeck->setEnabled(false);
    emit pDebug("Removed " + loadedDeckName + " from decksJson.");

    //Save to disk
    saveDecksJsonFile();

    //New
    newEmptyDeck();
}


void DeckHandler::enterArena()
{
    emit pDebug("Enter arena");

    this->inArena = true;
    enableDeckButtonSave(false);
    hideManageDecksButtons();
}


void DeckHandler::leaveArena()
{
    if(!inArena)    return;

    emit pDebug("Leave arena");

    this->inArena = false;
    enableDeckButtonSave(false);
    showManageDecksButtons();

    //Recuperamos deck
    if(!loadedDeckName.isNull() && decksJson.contains(loadedDeckName))
    {
        loadDeck(loadedDeckName);
    }
    else
    {
        newEmptyDeck();
    }
}


void DeckHandler::showManageDecksButtons()
{
    if(ui->deckButtonNew->isHidden() && !inArena && !inGame)
    {
        ui->tabDeckLayout->removeItem(ui->verticalLayoutManageDecks);
        ui->tabDeckLayout->addItem(ui->verticalLayoutManageDecks);
        ui->deckLineEdit->setHidden(false);
        ui->deckButtonNew->setHidden(false);
        ui->deckButtonLoad->setHidden(false);
        ui->deckButtonSave->setHidden(false);
        ui->deckButtonDeleteDeck->setHidden(false);
    }
}


void DeckHandler::hideManageDecksButtons()
{
    if(!ui->loadDeckTreeWidget->isHidden())
    {
        ui->loadDeckTreeWidget->clearSelection();
        hideDeckTreeWidget();
    }

    ui->deckLineEdit->setHidden(true);
    ui->deckButtonNew->setHidden(true);
    ui->deckButtonLoad->setHidden(true);
    ui->deckButtonSave->setHidden(true);
    ui->deckButtonDeleteDeck->setHidden(true);
    ui->tabDeckLayout->removeItem(ui->verticalLayoutManageDecks);
}


void DeckHandler::completeArenaDeck(QString draftLog)
{
    if(deckCardList[0].total == 0)
    {
        emit pDebug("Completing Arena Deck: Deck complete from log.");
        return;
    }

    QList<QString> cardsInDeck, cardsToAdd;

    //Create cardsInDeck list
    foreach(DeckCard deckCard, deckCardList)
    {
        QString code = deckCard.getCode();
        if(!code.isEmpty())
        {
            for(uint i=0; i<deckCard.total; i++) cardsInDeck.append(code);
        }
    }

    //Create cardsToAdd list
    QFile logFile(Utility::gameslogPath() + "/" + draftLog);
    if(!logFile.open(QIODevice::ReadOnly))
    {
        emit pDebug("Cannot open draft log " + Utility::gameslogPath() + "/" + draftLog, Error);
        return;
    }

    char line[2048];
    while(logFile.readLine(line, sizeof(line)) > 0)
    {
        QString code = getCodeFromDraftLogLine(line);
        if(!code.isEmpty())
        {
            if(cardsInDeck.contains(code))
            {
                cardsInDeck.removeOne(code);
            }
            else
            {
                cardsToAdd.append(code);
            }
        }
    }
    logFile.close();

    //Check lists make sense
    if(deckCardList[0].total != (uint)cardsToAdd.count())
    {
        emit pDebug("Completing Arena Deck: Cards to add != unknown cards.");
        return;
    }

    //Complete deck
    foreach(QString code, cardsToAdd)
    {
        newDeckCardDraft(code);
    }

    emit pDebug("Completing Arena Deck: " + QString::number(cardsToAdd.count()) + " cards added.");
}


QString DeckHandler::getCodeFromDraftLogLine(QString line)
{
    QRegularExpressionMatch match;
    //Pick card: CS1_112
    if(line.contains(QRegularExpression("Pick card: (\\w+)"), &match))
    {
        return match.captured(1);
    }
    else
    {
        return "";
    }
}


//Card exceptions
bool DeckHandler::isLastCreatedByCodeValid(QString code)
{
    if(code == DARKSHIRE_COUNCILMAN)    return false;
    return true;
}







