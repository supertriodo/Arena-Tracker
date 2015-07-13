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
    ui->deckListWidget->setIconSize(CARD_SIZE);
    ui->deckListWidget->setStyleSheet("background-color: transparent;");

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(enableDeckButtons()));
    connect(ui->deckButtonMin, SIGNAL(clicked()),
            this, SLOT(cardTotalMin()));
    connect(ui->deckButtonPlus, SIGNAL(clicked()),
            this, SLOT(cardTotalPlus()));
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

    qDebug() << "DeckHandler: Deck List cleared.";
}


QList<DeckCard> * DeckHandler::getDeckComplete()
{
    if(deckCardList[0].total==0)    return &deckCardList;
    else    return NULL;
}

void DeckHandler::newDeckCard(QString code, int total)
{
    if(code.isEmpty())  return;

    //Mazo completo
    if(deckCardList[0].total == 0)
    {
        qDebug() << "DeckHandler: WARNING: Insertando carta en un mazo completo. Desechamos:" <<
                    (*cardsJson)[code].value("name").toString();
        return;
    }

    //Ya existe en el mazo
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCardList[i].code == code)
        {
            qDebug() << "DeckHandler: Insertando carta existente en el mazo. Desechamos:" <<
                        (*cardsJson)[code].value("name").toString();
            return;
        }
    }

    DeckCard deckCard(code);
    deckCard.total = total;
    deckCard.listItem = new QListWidgetItem();
    deckCard.draw();
    insertDeckCard(deckCard);

    deckCardList[0].total-=total;
    deckCardList[0].draw();
    if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);

    emit checkCardImage(code);

    qDebug() << "DeckHandler: Completando mazo:" << total << (*cardsJson)[code].value("name").toString();
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
                remainingCards--;
                it->draw(false);

//                emit sendLog(tr("Deck: Card drawn: ") + (*cardsJson)[code].value("name").toString());
                showCount();
            }
            else if(it->remaining == 1)
            {
                it->remaining--;
                remainingCards--;
                if(it->total > 1)   it->draw();

                it->listItem->setIcon(QIcon(it->listItem->icon().pixmap(
                                        CARD_SIZE, QIcon::Disabled, QIcon::On)));

//                emit sendLog(tr("Deck: Card drawn: ") + (*cardsJson)[code].value("name").toString());
                showCount();
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
                remainingCards--;

                it->draw();
                it->listItem->setIcon(QIcon(it->listItem->icon().pixmap(
                                        CARD_SIZE, QIcon::Disabled, QIcon::On)));

                qDebug() << "DeckHandler: Nueva copia de carta robada" <<
                            (*cardsJson)[code].value("name").toString() <<
                            ", completando mazo.";
//                emit sendLog(tr("Deck: Discovered unknown card. Adding to deck: ") +
//                                  (*cardsJson)[code].value("name").toString());
                showCount();
            }
            else
            {
                qDebug() << "DeckHandler: WARNING: Nueva copia de carta robada" <<
                            (*cardsJson)[code].value("name").toString() <<
                            "pero el mazo esta completo.";
//                emit sendLog(tr("Deck: WARNING: Extra card drawn but deck is full. Is the deck right? ") +
//                              (*cardsJson)[code].value("name").toString());
            }
            return;
        }
    }

    //MALORNE
    if(code == MALORNE)  return;


    if(deckCardList[0].total>0)
    {
        qDebug() << "DeckHandler: Carta que no esta en el mazo robada" <<
                    (*cardsJson)[code].value("name").toString() <<
                    ", completando mazo.";
        newDeckCard(code);
        showPlayerCardDraw(code);
    }
    else
    {
        qDebug() << "DeckHandler: WARNING: Carta que no esta en el mazo robada" <<
                    (*cardsJson)[code].value("name").toString() <<
                    "pero el mazo esta completo.";
    }
}


void DeckHandler::showCount()
{
    if(inGame && ui->tabWidget->currentIndex()==tabDeck)
    {
        emit sendStatusBarMessage(tr("Cards in deck: ") + QString::number(remainingCards));
    }
}


void DeckHandler::redrawDownloadedCardImage(QString code)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->code == code)    it->draw();
    }
}


void DeckHandler::enableDeckButtons()
{
    int index = ui->deckListWidget->currentRow();

    if(index>0 && deckCardList[index].total > 1)
                                        ui->deckButtonMin->setEnabled(true);
    else                                ui->deckButtonMin->setEnabled(false);
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

    ui->tabDeck->setAttribute(Qt::WA_NoBackground);
    ui->tabDeck->repaint();
    ui->tabEnemy->setAttribute(Qt::WA_NoBackground);
    ui->tabEnemy->repaint();

    remainingCards = 30;

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
        }
        else    it->listItem->setHidden(true);
    }

    ui->deckListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->deckButtonMin->setHidden(false);
    ui->deckButtonPlus->setHidden(false);
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);

    ui->tabDeck->setAttribute(Qt::WA_NoBackground, false);
    ui->tabDeck->repaint();
    ui->tabEnemy->setAttribute(Qt::WA_NoBackground, false);
    ui->tabEnemy->repaint();

    emit sendStatusBarMessage(tr("Game over"), 3000);
    this->inGame = false;
}
