#include "draftitemcounter.h"
#include "../themehandler.h"
#include <QtWidgets>


DraftItemCounter::DraftItemCounter(QObject *parent, QString synergyTag, QString synergyTagExtra,
                                   QGridLayout *gridLayout, int gridRow, int gridCol,
                                   QPixmap pixmap, int iconWidth, bool iconHover, bool inDraftMechanicsWindow) : QObject(parent)
{
    //Constructor DraftMechanicsWindow/MainWindow
    this->synergyTag = synergyTag;
    this->synergyTagExtra = synergyTagExtra;
    this->horLayout = new QHBoxLayout();
    init(horLayout, iconHover);
    setTheme(pixmap, iconWidth, inDraftMechanicsWindow);

    gridLayout->addLayout(horLayout, gridRow, gridCol);
}


DraftItemCounter::DraftItemCounter(QObject *parent, QString synergyTag) : QObject(parent)
{
    //Synergy keys sin icono, solo datos
    labelIcon = nullptr;
    labelCounter = nullptr;
    this->synergyTag = synergyTag;
    this->synergyTagExtra = "";
    this->horLayout = nullptr;
    reset();
}


DraftItemCounter::~DraftItemCounter()
{
    if(labelIcon != nullptr)        delete labelIcon;
    if(labelCounter != nullptr)     delete labelCounter;
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


void DraftItemCounter::moveLayout(QGridLayout *gridLayout, int gridRow, int gridCol)
{
    gridLayout->removeItem(horLayout);
    gridLayout->addLayout(horLayout, gridRow, gridCol);
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
    this->synergyCardList.clear();
    this->extraCardList.clear();
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
//Cartas disable en listas de label (como coste 2 pero no drop2)
void DraftItemCounter::increaseExtra(const QString &code, const QString &synergyTagExc)
{
    if(labelIcon != nullptr)
    {
        bool duplicatedCard = false;
        for(SynergyCard &synergyCard: extraCardList)
        {
            if(synergyCard.getCode() == code)
            {
                synergyCard.total++;
                duplicatedCard = true;
                break;
            }
        }

        if(!duplicatedCard)
        {
            SynergyCard synergyCard(code);
            synergyCard.remaining = 0;
            synergyCard.setSynergyTag(synergyTagExc);
            extraCardList.append(synergyCard);
        }
    }
}
void DraftItemCounter::increase(const QString &code)
{
    if(codeMap.contains(code))  codeMap[code]++;
    else                        codeMap[code] = 1;

    if(labelIcon != nullptr)
    {
        bool duplicatedCard = false;
        for(SynergyCard &synergyCard: synergyCardList)
        {
            if(synergyCard.getCode() == code)
            {
                synergyCard.total++;
                synergyCard.remaining = synergyCard.total;
                duplicatedCard = true;
                break;
            }
        }

        if(!duplicatedCard)
        {
            synergyCardList.append(SynergyCard(code));
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


//Se usa en heroPowerGen(V_HERO_POWER), sinergia gen-gen, para evitar sinergias de una carta con ella misma
void DraftItemCounter::insertCards(QMap<QString, QMap<QString, int>> &synergyTagMap, QString avoidCode)
{
    QMap<QString,int> synergies;
    if(synergyTagMap.contains(synergyTag))  synergies = synergyTagMap[synergyTag];

    const QList<QString> codeList = codeMap.keys();
    for(const QString &code: codeList)
    {
        if(!synergies.contains(code) && code!=avoidCode)
        {
            synergies[code] = codeMap[code];
        }
    }

    if(!synergies.isEmpty())    synergyTagMap[synergyTag] = synergies;
}


void DraftItemCounter::insertSynCards(QMap<QString, QMap<QString, int>> &synergyTagMap)
{
    QMap<QString,int> synergies;
    if(synergyTagMap.contains(synergyTag))  synergies = synergyTagMap[synergyTag];

    const QList<QString> codeList = codeSynMap.keys();
    for(const QString &code: codeList)
    {
        if(!synergies.contains(code))
        {
            synergies[code] = codeSynMap[code];
        }
    }

    if(!synergies.isEmpty())    synergyTagMap[synergyTag] = synergies;
}


int DraftItemCounter::count()
{
    return counter;
}


QMap<QString, int> &DraftItemCounter::getCodeMap()
{
    return codeMap;
}


QMap<QString, QString> DraftItemCounter::getCodeTagMap()
{
    QMap<QString, QString> codeTagMap;
    for(SynergyCard &synergyCard: synergyCardList)
    {
        QString code = synergyCard.getCode();
        for(int i=0; i<synergyCard.total; i++)  codeTagMap.insertMulti(code, "");
    }
    for(SynergyCard &synergyCard: extraCardList)
    {
        QString code = synergyCard.getCode();
        QString tag = synergyCard.getSynergyTag();
        if(tag.isEmpty())  tag = ".";
        for(int i=0; i<synergyCard.total; i++)  codeTagMap.insertMulti(code, tag);
    }
    return codeTagMap;
}


void DraftItemCounter::sendIconEnter()
{
    //Positioning
    QPoint topLeft = labelIcon->mapToGlobal(QPoint(0,0));
    QPoint bottomRight = labelIcon->mapToGlobal(QPoint(labelIcon->width(),labelIcon->height()));
    QRect labelRect = QRect(topLeft, bottomRight);

    //synergyCardList
    QMap<int,SynergyCard> synergyCardMap;
    for(SynergyCard &synergyCard: synergyCardList)  synergyCardMap.insertMulti(synergyCard.getCost(), synergyCard);
    QList<SynergyCard> synergyCardOrderedList = synergyCardMap.values();
    if(!synergyCardOrderedList.isEmpty())   synergyCardOrderedList.first().setSynergyTag(synergyTag);

    //extraCardList
    QMap<int,SynergyCard> extraCardMap, extraCardMapWithTag;
    for(SynergyCard &synergyCard: extraCardList)
    {
        if(synergyCard.getSynergyTag().isEmpty())   extraCardMap.insertMulti(synergyCard.getCost(), synergyCard);
        else                                        extraCardMapWithTag.insertMulti(synergyCard.getCost(), synergyCard);
    }
    QList<SynergyCard> extraCardOrderedList = extraCardMap.values();
    if(!extraCardOrderedList.isEmpty()) extraCardOrderedList.first().setSynergyTag(synergyTagExtra);
    synergyCardOrderedList.append(extraCardOrderedList);
    synergyCardOrderedList.append(extraCardMapWithTag.values());

    emit iconEnter(synergyCardOrderedList, labelRect);
}













