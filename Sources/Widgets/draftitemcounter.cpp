#include "draftitemcounter.h"
#include <QtWidgets>

DraftItemCounter::DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap) : QObject(parent)
{
    labelIcon = new QLabel();
    labelCounter = new QLabel();
    labelIcon->setPixmap(pixmap);
    hLayout->addWidget(labelIcon);
    hLayout->addWidget(labelCounter);

    reset();
}


DraftItemCounter::DraftItemCounter(QObject *parent) : QObject(parent)
{
    labelIcon = NULL;
    labelCounter = NULL;
    reset();
}


DraftItemCounter::~DraftItemCounter()
{
    if(labelIcon != NULL)       delete labelIcon;
    if(labelCounter != NULL)    delete labelCounter;
}


void DraftItemCounter::reset()
{
    this->counter = 0;
    this->deckCardList.clear();
    this->deckCardListSyn.clear();

    if(labelIcon != NULL && labelCounter != NULL)
    {
        labelCounter->setText("0");
        labelIcon->setHidden(true);
        labelCounter->setHidden(true);
    }
}


void DraftItemCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    if(labelIcon != NULL && labelCounter != NULL)
    {
        if(!mouseInApp && transparency == Transparent)
        {
            labelIcon->setStyleSheet("QLabel {background-color: transparent; color: white;}");
            labelCounter->setStyleSheet("QLabel {background-color: transparent; color: white;}");
        }
        else
        {
            labelIcon->setStyleSheet("");
            labelCounter->setStyleSheet("");
        }
    }
}


void DraftItemCounter::increase()
{
    if(labelIcon != NULL && labelCounter != NULL)
    {
        this->counter++;
        labelCounter->setText(QString::number(counter));
        if(counter == 1)
        {
            labelIcon->setHidden(false);
            labelCounter->setHidden(false);
        }
    }
}
void DraftItemCounter::increase(const QString &code, bool count)
{
    bool duplicatedCard = false;
    for(DeckCard &deckCard: deckCardList)
    {
        if(deckCard.getCode() == code)
        {
            deckCard.total++;
            deckCard.remaining = deckCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        deckCardList.append(DeckCard(code));
    }

    if(labelIcon != NULL && labelCounter != NULL && count)
    {
        this->counter++;
        labelCounter->setText(QString::number(counter));
        if(counter == 1)
        {
            labelIcon->setHidden(false);
            labelCounter->setHidden(false);
        }
    }
}


void DraftItemCounter::increaseSyn(const QString &code)
{
    bool duplicatedCard = false;
    for(DeckCard &deckCard: deckCardListSyn)
    {
        if(deckCard.getCode() == code)
        {
            deckCard.total++;
            deckCard.remaining = deckCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        deckCardListSyn.append(DeckCard(code));
    }
}


void DraftItemCounter::insertCards(QMap<QString,int> &synergies)
{
    for(DeckCard &deckCard: deckCardList)
    {
        QString code = deckCard.getCode();
        if(!synergies.contains(code))
        {
            synergies[code] = deckCard.total;
        }
    }
}


void DraftItemCounter::insertSynCards(QMap<QString,int> &synergies)
{
    for(DeckCard &deckCard: deckCardListSyn)
    {
        QString code = deckCard.getCode();
        if(!synergies.contains(code))
        {
            synergies[code] = deckCard.total;
        }
    }
}


bool DraftItemCounter::isEmpty()
{
    return counter == 0;
}


int DraftItemCounter::count()
{
    return counter;
}
