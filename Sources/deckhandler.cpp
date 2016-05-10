#include "deckhandler.h"
#include <QtWidgets>

DeckHandler::DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson,
                         Ui::Extended *ui, EnemyDeckHandler *enemyDeckHandler) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->inGame = false;
    this->inArena = false;
    this->transparency = Opaque;
    this->drawAnimating = false;
    this->drawDisappear = 10;
    this->loadedDeckName = QString();
    this->loadDeckItemsMap.clear();
    this->mouseInApp = false;
    this->enemyDeckHandler = enemyDeckHandler;
    this->synchronized = false;

    completeUI();
}

DeckHandler::~DeckHandler()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
    drawCardList.clear();
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
    ui->drawListWidget->setMouseTracking(true);
    ui->deckListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->deckListWidget->setMouseTracking(true);

    createLoadDeckTreeWidget();

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(enableDeckButtons()));
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
}


void DeckHandler::addNewDeckMenu(QPushButton *button)
{
    QMenu *newDeckMenu = new QMenu(button);

    QAction *action = newDeckMenu->addAction("New empty deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newEmptyDeck()));

    action = newDeckMenu->addAction("Clone current deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newCopyCurrentDeck()));

    action = newDeckMenu->addAction("Clone enemy deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newCopyEnemyDeck()));

    action = newDeckMenu->addAction("Import HearthHead deck");
    connect(action, SIGNAL(triggered()), this, SLOT(newImportHearthHead()));

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
        loadDeckClasses[i]->setIcon(0, QIcon(":/Images/hero" + Utility::getHeroLogNumber(i) + ".png"));
        loadDeckClasses[i]->setForeground(0, QBrush(QColor(Utility::getHeroColor(i))));
    }

    loadDeckClasses[9] = new QTreeWidgetItem(treeWidget);
    loadDeckClasses[9]->setHidden(true);
    loadDeckClasses[9]->setExpanded(true);
    loadDeckClasses[9]->setText(0, "Multi class");
    loadDeckClasses[9]->setIcon(0, QIcon(":/Images/secretHunter.png"));
    loadDeckClasses[9]->setForeground(0, QBrush(QColor(Utility::getHeroColor(9))));

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(unselectClassItems()));
    connect(treeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(hideDeckTreeWidget()));
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
    QEasingCurve easingCurve = QEasingCurve::OutCubic;

    //Show DeckTreeWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckTreeWidget()));

    animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(easingCurve);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckTreeWidget()));

    //Hide DeckListWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(easingCurve);
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
    QEasingCurve easingCurve = QEasingCurve::InCubic;

    //Show DeckListWidget
    QPropertyAnimation *animation = new QPropertyAnimation(ui->deckListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckListWidget()));

    animation = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight()+2);
    animation->setEndValue(totalHeight+2);
    animation->setEasingCurve(easingCurve);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckListWidget()));

    //Hide DeckTreeWidget
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(easingCurve);
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


void DeckHandler::setSynchronized()
{
    this->synchronized = true;
    if(this->inGame)    lockDeckInterface();
    else                unlockDeckInterface();
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
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->drawListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->drawListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(QEasingCurve::OutBounce);
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
    deckCard.draw(true);
    insertDeckCard(deckCard); //Lanza emit deckSizeChanged();

    enableDeckButtons();

    emit pDebug("Deck list cleared.");
}


QList<DeckCard> * DeckHandler::getDeckComplete()
{
    if(deckCardList[0].total==0)    return &deckCardList;
    else    return NULL;
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


void DeckHandler::newDeckCard(QString code, int total, bool add)
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
            deckCardList[i].draw(true);
            break;
        }
    }

    if(!found)
    {
        DeckCard deckCard(code);
        deckCard.total = total;
        deckCard.remaining = total;
        deckCard.listItem = new QListWidgetItem();
        insertDeckCard(deckCard);
        deckCard.draw(true);
        emit checkCardImage(code);
    }

    deckCardList[0].total-=total;
    deckCardList[0].remaining = deckCardList[0].total;
    deckCardList[0].draw(true);
    if(deckCardList[0].total == 0)  hideUnknown();

    if(!this->inArena)   enableDeckButtonSave();

    emit pDebug("Add to deck: (" + QString::number(total) + ")" +
                (*cardsJson)[code].value("name").toString());
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
            if(deckCard.getType() != deckCardList[i].getType())
            {
                if(deckCard.getType() == WEAPON || deckCardList[i].getType() == MINION)
                {
                    deckCardList.insert(i, deckCard);
                    ui->deckListWidget->insertItem(i, deckCard.listItem);
                    emit deckSizeChanged();
                    return;
                }
            }
            else if(deckCard.getName().toLower() < deckCardList[i].getName().toLower())
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


void DeckHandler::hideUnknown(bool hidden)
{
    deckCardList[0].listItem->setHidden(hidden);
    emit deckSizeChanged();
}


void DeckHandler::newDrawCard(QString code)
{
    DrawCard drawCard(code);
    drawCard.listItem = new QListWidgetItem();
    drawCardList.append(drawCard);
    ui->drawListWidget->addItem(drawCard.listItem);
    drawCard.draw();
    emit checkCardImage(code);
    ui->drawListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));

    if(this->drawDisappear>0)   QTimer::singleShot(this->drawDisappear*1000,
                                                    this, SLOT(removeOldestDrawCard()));
}


void DeckHandler::removeOldestDrawCard()
{
    if(drawCardList.empty())    return;
    DrawCard drawCard = drawCardList.takeFirst();
    ui->drawListWidget->removeItemWidget(drawCard.listItem);
    delete drawCard.listItem;
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));
}


void DeckHandler::showPlayerCardDraw(QString code)
{
    if(this->drawDisappear>=0)    newDrawCard(code);
    drawFromDeck(code);
}


void DeckHandler::drawFromDeck(QString code)
{
    //Avoid special cards
    if(code == THE_COIN)    return;

    for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining>1)
            {
                it->remaining--;
                it->draw(false);
            }
            else if(it->remaining == 1)
            {
                it->remaining--;
                it->drawGreyed(true);
            }
            //it->remaining == 0
            //Reajustamos el mazo si tiene unknown cards
            else if(deckCardList[0].total>0)
            {
                deckCardList[0].total--;
                deckCardList[0].remaining = deckCardList[0].total;
                if(deckCardList[0].total == 0)  hideUnknown();
                else                            deckCardList[0].draw(true);
                it->total++;

                it->drawGreyed(true);

                emit pDebug("New card: " + it->getName() + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total));
            }
            else
            {
                emit pDebug("New card but deck is full. " + it->getName() + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total), Warning);
            }
            return;
        }
    }

    if(deckCardList[0].total>0)
    {
        emit pDebug("New card: " +
                          (*cardsJson)[code].value("name").toString() + ". 0/1");
        newDeckCard(code);
        drawFromDeck(code);
    }
    else
    {
        emit pDebug("New card but deck is full. " +
                      (*cardsJson)[code].value("name").toString(), Warning);
    }
}


void DeckHandler::returnToDeck(QString code)
{
    for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining < it->total)
            {
                it->remaining++;
                it->draw(false);
                emit pDebug("Return to deck: " + code + ". " +
                            QString::number(it->remaining) + "/" + QString::number(it->total));
            }
            else
            {
                emit pDebug("Not return to deck: " + code + ". Remaining=Total " +
                            QString::number(it->remaining) + "/" + QString::number(it->total), Warning);
            }
            return;
        }
    }

    emit pDebug("Not return to deck: " + code + ". Code not found", Warning);
}


void DeckHandler::redrawDownloadedCardImage(QString code)
{
    for(QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining > 0)
            {
                it->draw(false);
            }
            else
            {
                it->drawGreyed(true);
            }
        }
    }

    for(QList<DrawCard>::iterator it = drawCardList.begin(); it != drawCardList.end(); it++)
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
            if(deckCard.remaining > 0)
            {
                deckCard.draw(false);
            }
            else
            {
                deckCard.drawGreyed(true);
            }
        }
    }

    foreach(DrawCard drawCard, drawCardList)
    {
        if(drawCard.getCardClass()<9)
        {
            drawCard.draw();
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
            if(deckCard.remaining > 0)
            {
                deckCard.draw(false);
            }
            else
            {
                deckCard.drawGreyed(true);
            }
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
}


void DeckHandler::redrawAllCards()
{
    foreach(DeckCard deckCard, deckCardList)
    {
        if(deckCard.remaining > 0)
        {
            deckCard.draw(false);
        }
        else
        {
            deckCard.drawGreyed(true);
        }
    }

    foreach(DrawCard drawCard, drawCardList)
    {
        drawCard.draw();
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

    deckCardList[index].draw(true);
    if(deckCardList[0].total==1)    hideUnknown(false);
    deckCardList[0].draw(true);
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

    deckCardList[index].draw(true);
    if(deckCardList[0].total==0)    hideUnknown();
    else                            deckCardList[0].draw(true);
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

//    int ret = QMessageBox::warning(ui->centralWidget, tr("Sure?"), tr("Remove (") +
//            deckCardList[index].getName() +   tr(") from your deck?"),
//            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
//    if(ret == QMessageBox::Cancel)  return;

    ui->deckListWidget->removeItemWidget(deckCardList[index].listItem);
    delete deckCardList[index].listItem;
    deckCardList.removeAt(index);

    deckCardList[0].total++;
    deckCardList[0].remaining = deckCardList[0].total;
    if(deckCardList[0].total==1)    hideUnknown(false);
    deckCardList[0].draw(true);
    enableDeckButtons();

    enableDeckButtonSave();
    emit deckSizeChanged();
}


void DeckHandler::lockDeckInterface()
{
    this->inGame = true;

    if(!synchronized)   return;

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

    emit pDebug("Lock deck interface.");
}


void DeckHandler::unlockDeckInterface()
{
    this->inGame = false;

    if(!synchronized)   return;

    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->total>0)
        {
            it->draw(true);
            it->listItem->setHidden(false);
            it->remaining = it->total;
        }
        else    it->listItem->setHidden(true);
    }

    ui->deckListWidget->setFocusPolicy(Qt::ClickFocus);
    ui->deckListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    showManageDecksButtons();

    updateTransparency();
    clearDrawList(true);

    emit pDebug("Unlock deck interface.");
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


void DeckHandler::setTheme(Theme value)
{
    if(value == ThemeBlack)
    {
        ui->loadDeckTreeWidget->setStyleSheet("QTreeView{background-color: black; outline: 0;}");
    }
    else
    {
        ui->loadDeckTreeWidget->setStyleSheet("QTreeView{background-color: #F0F0F0; outline: 0;}");
    }
}


void DeckHandler::clearDrawList(bool forceClear)
{
    if(!forceClear && this->drawDisappear>0)     return;

    ui->drawListWidget->clear();
    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setMinimumHeight(0);
    ui->drawListWidget->setMaximumHeight(0);
    drawCardList.clear();
}


void DeckHandler::setDrawDisappear(int value)
{
    this->drawDisappear = value;
    clearDrawList(true);
}


void DeckHandler::findDeckCardEntered(QListWidgetItem * item)
{
    QString code = deckCardList[ui->deckListWidget->row(item)].getCode();

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
    int drawListBottom = ui->drawListWidget->mapToGlobal(QPoint(0,ui->drawListWidget->height())).y();
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
    foreach(QString deckName, decksJson.keys())
    {
        addDeckToLoadTree(deckName);
    }
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

    foreach(QString key, jsonObjectDeck.keys())
    {
        if(key != "hero")   newDeckCard(key, jsonObjectDeck[key].toInt());
    }

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


void DeckHandler::newImportHearthHead()
{
    if(newDeck(true) && showHearthHeadHowTo())  importHearthHead();
}


void DeckHandler::newCopyEnemyDeck()
{
    if(newDeck(true))   importEnemyDeck();
}


bool DeckHandler::showHearthHeadHowTo()
{
    QString text =  "This option allows you to import a deck from HearthHead:<br/><br/>"
            "1) Go to <a href='http://www.hearthhead.com/'>HearthHead.com</a> and choose the deck you want.<br/><br/>"
            "2) Click Export -> Cockatrice and copy the text shown.<br/>"
                    "*   There's no need to paste it anywhere.<br/><br/>"
            "3) Ok this dialog and Arena Tracker will create the deck.";

    QMessageBox msgBox(0);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(text);
    msgBox.setWindowTitle("Import HearthHead deck");
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.exec();

    if(msgBox.result() == QMessageBox::Ok)  return true;
    else                                    return false;
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


void DeckHandler::importHearthHead()
{
    QString data = QApplication::clipboard()->text();
    QStringList dataLines = data.split('\n');

    QRegularExpressionMatch match;
    this->reset();

    foreach(QString line, dataLines)
    {
        QString name;
        QString code;
        int numCards = 0;

        //2 Arcane Missiles
        if(line.contains(QRegularExpression("^(\\d+) +([^ ].*)$"), &match))
        {
            numCards = match.captured(1).toInt();
            name = match.captured(2);

            code = Utility::cardLocalCodeFromName(name);
            if(code.isEmpty())  code = Utility::cardEnCodeFromName(name);
        }

        emit pDebug("Import HearthHead: " + line);

        if(code.isNull())
        {
            emit pDebug("Import HearthHead: Bad Format.", Warning);
        }
        else if(code.isEmpty())
        {
            emit pDebug("Import HearthHead: Name: " + name + " not found in Jsons.", Warning);
        }
        else if(numCards<1 || numCards>2)
        {
            emit pDebug("Import HearthHead: Num cards: " + QString::number(numCards) + " is not correct.", Warning);
        }
        else
        {
            newDeckCard(code, numCards);
        }
    }
}


void DeckHandler::importEnemyDeck()
{
    QList<DeckCard> enemyDeckCardList = enemyDeckHandler->getDeckCardList();

    foreach(DeckCard deckCard, enemyDeckCardList)
    {
        if(!deckCard.getCode().isEmpty())   newDeckCard(deckCard.getCode(), deckCard.total);
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


bool DeckHandler::deckBuilderPY()
{
    //Detecta zonas en pantalla
    std::vector<Point2f> screenPoints;
    std::vector<Point2f> templatePoints(4);
    templatePoints[0] = cvPoint(187,335);
    templatePoints[1] = cvPoint(432,335);
    templatePoints[2] = cvPoint(760,995);
    templatePoints[3] = cvPoint(1265,75);


    QList<QScreen *> screens = QGuiApplication::screens();
    for(int screenIndex=0; screenIndex<screens.count(); screenIndex++)
    {
        QScreen *screen = screens[screenIndex];
        if (!screen)    continue;
        QRect rect = screen->geometry();
        if(rect.x() != 0 || rect.y() != 0)  continue;

        //Main Screen
        screenPoints = Utility::findTemplateOnScreen("collectionTemplate.png", screen, templatePoints);
        break;
    }


    if(screenPoints.empty())
    {
        emit pDebug("DeckBuilder: Collection template not found on main screen.");
        emit pLog("Deck Builder: Hearthstone not found on main screen.");
        return false;
    }


    //Lanza script
    QProcess p;
    QStringList params;
    bool goldenCards = ui->configCheckGoldenCards->isChecked();

    params << QDir::toNativeSeparators(Utility::extraPath() + "/deckBuilder.py");
    foreach(Point2f point, screenPoints)
    {
        params << QString::number((int)point.x) << QString::number((int)point.y);
    }

    params << ui->deckLineEdit->text();//Deck name
    params << QString::number(0.1);//Time pause
    params << (goldenCards?QString("1"):QString("0"));//Golden cards

    foreach(DeckCard deckCard, deckCardList.mid(1))
    {
        params << Utility::removeAccents(deckCard.getName()) << QString::number(deckCard.total);
    }

    emit pDebug("Start script:\n" + params.join(" - "));

#ifdef Q_OS_WIN
    p.start("python", params);
#else
    p.start("python3", params);
#endif
    p.waitForFinished(-1);

    emit pDebug("End script:\n" + p.readAll());
    return true;
}


void DeckHandler::askCreateDeckPY()
{
    QString text = "This will create in Hearthstone your deck,\ntaking control of mouse and keyboard.";

    QMessageBox msgBox((QWidget*)this->parent());
    msgBox.setWindowTitle("Create Deck in Hearthstone");
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Information);

    QPushButton *button1 = msgBox.addButton("Start", QMessageBox::AcceptRole);
                           msgBox.addButton("Cancel", QMessageBox::RejectRole);
    QPushButton *button3 = msgBox.addButton("How To Use/Install", QMessageBox::HelpRole);

    msgBox.exec();

    if(msgBox.clickedButton() == button1)           deckBuilderPY();
    else if(msgBox.clickedButton() == button3)      showInstallPY();
}


void DeckHandler::showInstallPY()
{
    QString instructions1 =
        "This option allows you to create in Hearthstone your current deck, taking control of mouse and keyboard.<br/><br/>"
        "1) The first time you use it, you will need to install python v3 in your system.<br/><br/>";
    QString instructions3 =
        "2) If you have more than one screen, make sure Hearthstone is located in your primary screen.<br/><br/>"
        "3) Make sure you have manually created an empty deck of the correct class in Hearthstone "
                "and stay in the screen where you select the cards for your deck.<br/><br/>"
        "4) During the process Arena Tracker will take control of your mouse and keyboard to create the deck so don't use them. "
                "If you need to stop the script move quickly the mouse to the top-left corner of the screen.";

#ifdef Q_OS_LINUX
    QString instructions2 =
            "*   Open a terminal (Ubuntu/Linux Mint):<br/>"
            "sudo apt-get install python3-pip<br/>"
            "sudo pip3 install python3-xlib<br/>"
            "sudo apt-get install python3-tk<br/>"
            "sudo apt-get install python3-dev<br/>"
            "sudo pip3 install image<br/>"
            "sudo pip3 install pyautogui<br/><br/>";
#endif
#ifdef Q_OS_WIN
    QString instructions2 =
            "1a) Install <a href='https://www.python.org/downloads/'>Python v3</a>.<br/>"
            "*   Check: Add Python to PATH during the installation.<br/><br/>"
            "1b) Open a terminal (cmd.exe) to install PyAutoGUI.<br/>"
            "*   pip install image<br/>"
            "*   pip install pyautogui<br/><br/>";
#endif
#ifdef Q_OS_MAC
    QString instructions2 =
            "1a) Install <a href='https://www.python.org/downloads/'>Python v3</a>.<br/><br/>"
            "1b) Open a terminal to install PyAutoGUI.<br/>"
            "*   sudo pip3 install pyobjc-core<br/>"
            "*   sudo pip3 install pyobjc<br/>"
            "*   sudo pip3 install image<br/>"
            "*   sudo pip3 install pyautogui<br/><br/>";
#endif

    QMessageBox msgBox(0);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(instructions1 + instructions2 + instructions3);
    msgBox.setWindowTitle("Create Deck in Hearthstone (How to Use/Install)");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
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










