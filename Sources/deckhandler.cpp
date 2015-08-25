#include "deckhandler.h"
#include <QtWidgets>

DeckHandler::DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::MainWindow *ui) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->inGame = false;

    //Iniciamos deckCardList con 30 cartas desconocidas
    reset();

    completeUI();
}

DeckHandler::~DeckHandler()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
}


void DeckHandler::completeUI()
{
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);
    ui->deckButtonRemove->setEnabled(false);
    ui->deckListWidget->setIconSize(CARD_SIZE);

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(enableDeckButtons()));
    connect(ui->deckButtonMin, SIGNAL(clicked()),
            this, SLOT(cardTotalMin()));
    connect(ui->deckButtonPlus, SIGNAL(clicked()),
            this, SLOT(cardTotalPlus()));
    connect(ui->deckButtonRemove, SIGNAL(clicked()),
            this, SLOT(cardRemove()));
}


void DeckHandler::reset()
{
    ui->deckListWidget->clear();
    deckCardList.clear();

    DeckCard deckCard("");
    deckCard.total = 30;
    deckCard.listItem = new QListWidgetItem();
    deckCard.draw();
    insertDeckCard(deckCard);

    enableDeckButtons();

    emit pDebug("Deck list cleared.");
}


QList<DeckCard> * DeckHandler::getDeckComplete()
{
    if(deckCardList[0].total==0)    return &deckCardList;
    else    return NULL;
}


void DeckHandler::newDeckCardAsset(QString code)
{
    newDeckCard(code, 1, true);
}


void DeckHandler::newDeckCard(QString code, int total, bool noAdd)
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
        if(deckCardList[i].code == code)
        {
            if(noAdd)
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
        deckCard.total = total;
        deckCard.remaining = total;
        deckCard.listItem = new QListWidgetItem();
        insertDeckCard(deckCard);
        deckCard.draw();
        emit checkCardImage(code);
    }

    deckCardList[0].total-=total;
    deckCardList[0].draw();
    if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);

    emit pDebug("Add to deck: (" + QString::number(total) + ") " +
                (*cardsJson)[code].value("name").toString());
}


void DeckHandler::insertDeckCard(DeckCard &deckCard)
{
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCard.cost<deckCardList[i].cost)
        {
            deckCardList.insert(i, deckCard);
            ui->deckListWidget->insertItem(i, deckCard.listItem);
            return;
        }
    }
    deckCardList.append(deckCard);
    ui->deckListWidget->addItem(deckCard.listItem);
}


void DeckHandler::showPlayerCardDraw(QString code)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->code == code)
        {
            if(it->remaining>1)
            {
                it->remaining--;
                it->draw(false);
            }
            else if(it->remaining == 1)
            {
                it->remaining--;
                if(it->total > 1)   it->draw();

                it->listItem->setIcon(QIcon(it->listItem->icon().pixmap(
                                        CARD_SIZE, QIcon::Disabled, QIcon::On)));
            }
            //it->remaining == 0
            //MALORNE
            else if(code == MALORNE)  return;
            //Reajustamos el mazo si tiene unknown cards
            else if(deckCardList[0].total>0)
            {
                deckCardList[0].total--;
                if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);
                else                            deckCardList[0].draw();
                it->total++;

                it->draw();
                it->listItem->setIcon(QIcon(it->listItem->icon().pixmap(
                                        CARD_SIZE, QIcon::Disabled, QIcon::On)));

                emit pDebug("New card: " +
                                  (*cardsJson)[code].value("name").toString());
                emit pLog(tr("Deck: New card: ") +
                                  (*cardsJson)[code].value("name").toString());
            }
            else
            {
                emit pDebug("New card but deck is full. " +
                              (*cardsJson)[code].value("name").toString(), Warning);
                emit pLog(tr("Deck: WARNING: New card but deck is full. Is the deck right? ") +
                              (*cardsJson)[code].value("name").toString());
            }
            return;
        }
    }

    //MALORNE
    if(code == MALORNE)  return;


    if(deckCardList[0].total>0)
    {
        emit pDebug("New card: " +
                          (*cardsJson)[code].value("name").toString());
        emit pLog(tr("Deck: New card: ") +
                          (*cardsJson)[code].value("name").toString());
        newDeckCard(code);
        showPlayerCardDraw(code);
    }
    else
    {
        emit pDebug("New card but deck is full. " +
                      (*cardsJson)[code].value("name").toString(), Warning);
        emit pLog(tr("Deck: WARNING: New card but deck is full. Is the deck right? ") +
                      (*cardsJson)[code].value("name").toString());
    }
}


void DeckHandler::redrawDownloadedCardImage(QString code)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->code == code)
        {
            if(it->remaining > 0)
            {
                it->draw(false);
            }
            else
            {
                it->draw();
                it->listItem->setIcon(QIcon(it->listItem->icon().pixmap(
                                    CARD_SIZE, QIcon::Disabled, QIcon::On)));
            }
        }
    }
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
    if(index>0 && deckCardList.first().total > 0)
                                        ui->deckButtonPlus->setEnabled(true);
    else                                ui->deckButtonPlus->setEnabled(false);
}


void DeckHandler::cardTotalMin()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total--;
    deckCardList[0].total++;

    deckCardList[index].draw();
    if(deckCardList[0].total==1)    deckCardList[0].listItem->setHidden(false);
    deckCardList[0].draw();
    enableDeckButtons();
}


void DeckHandler::cardTotalPlus()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total++;
    deckCardList[0].total--;

    deckCardList[index].draw();
    if(deckCardList[0].total==0)    deckCardList[0].listItem->setHidden(true);
    else                            deckCardList[0].draw();
    enableDeckButtons();
}


void DeckHandler::cardRemove()
{
    int index = ui->deckListWidget->currentRow();
    if(deckCardList[index].total!=1 || index==0)
    {
        enableDeckButtons();
        return;
    }

    int ret = QMessageBox::warning(ui->centralWidget, tr("Sure?"), tr("Remove (") +
            (*cardsJson)[deckCardList[index].code].value("name").toString() +   tr(") from your deck?"),
            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
    if(ret == QMessageBox::Cancel)  return;

    ui->deckListWidget->removeItemWidget(deckCardList[index].listItem);
    delete deckCardList[index].listItem;
    deckCardList.removeAt(index);

    deckCardList[0].total++;
    if(deckCardList[0].total==1)    deckCardList[0].listItem->setHidden(false);
    deckCardList[0].draw();
    enableDeckButtons();
}


void DeckHandler::lockDeckInterface()
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        it->remaining = it->total;
    }

    ui->deckListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->deckListWidget->selectionModel()->reset();
    ui->deckButtonMin->setHidden(true);
    ui->deckButtonPlus->setHidden(true);
    ui->deckButtonRemove->setHidden(true);

    ui->deckListWidget->setStyleSheet("background-color: transparent;");
    ui->tabDeck->setAttribute(Qt::WA_NoBackground);
    ui->tabDeck->repaint();
    ui->tabEnemy->setAttribute(Qt::WA_NoBackground);
    ui->tabEnemy->repaint();

    this->inGame = true;
}


void DeckHandler::unlockDeckInterface()
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->total>0)
        {
            it->draw();
            it->listItem->setHidden(false);
            it->remaining = it->total;
        }
        else    it->listItem->setHidden(true);
    }

    ui->deckListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->deckButtonMin->setHidden(false);
    ui->deckButtonPlus->setHidden(false);
    ui->deckButtonRemove->setHidden(false);
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);
    ui->deckButtonRemove->setEnabled(false);

        ui->deckListWidget->setStyleSheet("");
    ui->tabDeck->setAttribute(Qt::WA_NoBackground, false);
    ui->tabDeck->repaint();
    ui->tabEnemy->setAttribute(Qt::WA_NoBackground, false);
    ui->tabEnemy->repaint();

    this->inGame = false;
}
