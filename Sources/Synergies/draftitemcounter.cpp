#include "draftitemcounter.h"
#include "../themehandler.h"
#include <QtWidgets>

DraftItemCounter::DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap, bool iconHover) : QObject(parent)
{
    //Constructor MainWindow
    init(hLayout, iconHover);
    setTheme(pixmap);
}


DraftItemCounter::DraftItemCounter(QObject *parent, QGridLayout *gridLayout, int gridRow, int gridCol,
                                   QPixmap pixmap, int iconWidth, bool iconHover) : QObject(parent)
{
    //Constructor DraftMechanicsWindow
    QHBoxLayout *hLayout = new QHBoxLayout();
    init(hLayout, iconHover);
    setTheme(pixmap, iconWidth, true);

    gridLayout->addLayout(hLayout, gridRow, gridCol);
}


DraftItemCounter::DraftItemCounter(QObject *parent) : QObject(parent)
{
    //Synergy keys sin icono, solo datos
    labelIcon = nullptr;
    labelCounter = nullptr;
    reset();
}


DraftItemCounter::~DraftItemCounter()
{
    if(labelIcon != nullptr)       delete labelIcon;
    if(labelCounter != nullptr)    delete labelCounter;
}


void DraftItemCounter::init(QHBoxLayout *hLayout, bool iconHover)
{
    labelIcon = new HoverLabel();
    labelCounter = new QLabel();
    hLayout->addWidget(labelIcon);
    hLayout->addWidget(labelCounter);

    if(iconHover)
    {
        connect(labelIcon, SIGNAL(enter()),
                this, SLOT(sendIconEnter()));
        connect(labelIcon, SIGNAL(leave()),
                this, SIGNAL(iconLeave()));
    }

    reset();
}


void DraftItemCounter::setIcon(QPixmap &pixmap, int iconWidth)
{
    labelIcon->setPixmap(pixmap.scaledToWidth(iconWidth, Qt::SmoothTransformation));
}


void DraftItemCounter::setTheme(QPixmap pixmap, int iconWidth, bool inDraftMechanicsWindow)
{
    if(labelIcon != nullptr)
    {
        QFont font(ThemeHandler::bigFont());
        font.setPixelSize(static_cast<int>(iconWidth*0.6));
        labelCounter->setFont(font);
        if(inDraftMechanicsWindow)  labelCounter->setStyleSheet(".QLabel { color: " + ThemeHandler::fgDraftMechanicsColor() + ";}");

        setIcon(pixmap, iconWidth);
    }
}


void DraftItemCounter::reset()
{
    this->counter = 0;
    this->deckCardList.clear();
    this->codeMap.clear();
    this->codeSynMap.clear();

    if(labelIcon != nullptr)
    {
        labelCounter->setText("0");
        labelIcon->setDisabled(true);
    }
}


void DraftItemCounter::hide()
{
    if(labelIcon != nullptr)
    {
        labelIcon->setHidden(true);
        labelCounter->setHidden(true);
    }
}


void DraftItemCounter::show()
{
    if(labelIcon != nullptr)
    {
        labelIcon->setHidden(false);
        labelCounter->setHidden(false);
    }
}


void DraftItemCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    if(labelIcon != nullptr)
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


//Mana counter increase
void DraftItemCounter::increase(int numIncrease, int draftedCardsCount)
{
    if(labelIcon != nullptr)
    {
        if(counter == 0)
        {
            labelIcon->setDisabled(false);
        }
        this->counter += numIncrease;
        labelCounter->setText(QString::number((counter*10/std::max(1,draftedCardsCount))/10.0));
    }
}
void DraftItemCounter::increase(const QString &code)
{
    if(codeMap.contains(code))  codeMap[code]++;
    else                        codeMap[code] = 1;

    if(labelIcon != nullptr)
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

        this->counter++;
        labelCounter->setText(QString::number(counter));
        if(counter == 1)
        {
            labelIcon->setDisabled(false);
        }
    }
}


void DraftItemCounter::increaseSyn(const QString &code)
{
    if(codeSynMap.contains(code))   codeSynMap[code]++;
    else                            codeSynMap[code] = 1;
}


//Se usa para obtener las sinergias de los direct links
bool DraftItemCounter::insertCode(const QString code, QMap<QString,int> &synergies)
{
    if(codeMap.contains(code))
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeMap[code];
        }
        return true;
    }
    return false;
}


void DraftItemCounter::insertCards(QMap<QString,int> &synergies)
{
    for(QString code: codeMap.keys())
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeMap[code];
        }
    }
}


void DraftItemCounter::insertSynCards(QMap<QString,int> &synergies)
{
    for(QString code: codeSynMap.keys())
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeSynMap[code];
        }
    }
}


int DraftItemCounter::count()
{
    return counter;
}


QMap<QString, int> &DraftItemCounter::getCodeMap()
{
    return codeMap;
}


void DraftItemCounter::sendIconEnter()
{
    QPoint topLeft = labelIcon->mapToGlobal(QPoint(0,0));
    QPoint bottomRight = labelIcon->mapToGlobal(QPoint(labelIcon->width(),labelIcon->height()));
    QRect labelRect = QRect(topLeft, bottomRight);

    QMap<int,DeckCard> deckCardMap;
    for(DeckCard &deckCard: deckCardList)   deckCardMap.insertMulti(deckCard.getCost(), deckCard);
    QList<DeckCard> deckCardOrderedList = deckCardMap.values();

    emit iconEnter(deckCardOrderedList, labelRect);
}













