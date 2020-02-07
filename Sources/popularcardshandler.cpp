#include "popularcardshandler.h"
#include <QtWidgets>

PopularCardsHandler::PopularCardsHandler(QObject *parent, Ui::Extended *ui, EnemyHandHandler *enemyHandHandler) : QObject(parent)
{
    this->ui = ui;
    this->cardsAnimating = false;
    this->enemyHandHandler = enemyHandHandler;
    this->patreonVersion = false;
    this->enemyMana = -1;
    this->enemyOverloadOwed = 0;
    this->inArena = false;

    completeUI();
}

PopularCardsHandler::~PopularCardsHandler()
{
    popularCardList.clear();
    ui->popularCardsListWidget->clear();
}


void PopularCardsHandler::completeUI()
{
    ui->popularCardsListWidget->setHidden(true);
    ui->popularCardsListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->popularCardsListWidget->setMouseTracking(true);
    ui->popularCardsListWidget->setFixedHeight(0);

    connect(ui->popularCardsListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findCardEntered(QListWidgetItem*)));
}


void PopularCardsHandler::adjustSize()
{
    if(cardsAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustSize()));
        return;
    }

    int rowHeight = ui->popularCardsListWidget->sizeHintForRow(0);
    int rows = popularCardList.count();
    int height = rows*rowHeight + 2*ui->popularCardsListWidget->frameWidth();
    int maxHeight = (ui->popularCardsListWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->popularCardsListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->popularCardsListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(SHOW_EASING_CURVE);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->popularCardsListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->popularCardsListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(SHOW_EASING_CURVE);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);

    this->cardsAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearCardsAnimating()));
}


void PopularCardsHandler::clearCardsAnimating()
{
    this->cardsAnimating = false;
    if(popularCardList.empty())     ui->popularCardsListWidget->setHidden(true);
}


void PopularCardsHandler::redrawDownloadedCardImage(QString code)
{
    for(PopularCard &popularCard: popularCardList)
    {
        if(popularCard.getCode() == code)
        {
            popularCard.draw();
        }
    }
}


void PopularCardsHandler::redrawClassCards()
{
    for(PopularCard &popularCard: popularCardList)
    {
        if(popularCard.getCardClass()<9)
        {
            popularCard.draw();
        }
    }
}


void PopularCardsHandler::redrawSpellWeaponCards()
{
    for(PopularCard &popularCard: popularCardList)
    {
        CardType cardType = popularCard.getType();
        if(cardType == SPELL || cardType == WEAPON)
        {
            popularCard.draw();
        }
    }
}


void PopularCardsHandler::redrawAllCards()
{
    for(PopularCard &popularCard: popularCardList)
    {
        popularCard.draw();
    }
}


void PopularCardsHandler::resetCardsInterface()
{
    clearAndHide();
    this->enemyMana = -1;
    this->enemyOverloadOwed = 0;
}


void PopularCardsHandler::clearAndHide()
{
    ui->popularCardsListWidget->setHidden(true);
    ui->popularCardsListWidget->clear();
    ui->popularCardsListWidget->setFixedHeight(0);
    popularCardList.clear();
}


void PopularCardsHandler::findCardEntered(QListWidgetItem * item)
{
    QString code = popularCardList[ui->popularCardsListWidget->row(item)].getCode();

    QRect rectCard = ui->popularCardsListWidget->visualItemRect(item);
    QPoint posCard = ui->popularCardsListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int cardListTop = ui->popularCardsListWidget->mapToGlobal(QPoint(0,0)).y();
    int cardListBottom = ui->popularCardsListWidget->mapToGlobal(QPoint(0,ui->popularCardsListWidget->height())).y();
    emit cardEntered(code, globalRectCard, cardListTop, cardListBottom);
}


void PopularCardsHandler::setCardsPickratesMap(QMap<QString, float> cardsPickratesMap[9])
{
    this->cardsPickratesMap = cardsPickratesMap;
}


void PopularCardsHandler::createCardsByPickrate(const QMap<QString, float> cardsPickratesMap[9], QStringList codeList)
{
    for(QString &code: codeList)
    {
        CardClass cardClass = Utility::getClassFromCode(code);
        int cost = std::min(Utility::getCardAttribute(code, "cost").toInt(), 10);

        if(cost>=2)
        {
            switch(cardClass)
            {
                case NEUTRAL:
                    for(int i=0; i<9; i++)
                    {
                        if(cardsPickratesMap[i][code]>=10)
                        {

                            cardsByPickrate[i][cost-2].append(code);
                        }
                    }
                break;
                default:
                    if(cardClass<9 && cardsPickratesMap[cardClass][code]>=10)
                    {
                        cardsByPickrate[cardClass][cost-2].append(code);
                    }
                break;
            }
        }
    }

    for(int i=0; i<9; i++)
    {
        for(int j=0; j<9; j++)
        {
            qSort(cardsByPickrate[i][j].begin(), cardsByPickrate[i][j].end(), [=](const QString &code1, const QString &code2)
            {
                return cardsPickratesMap[i][code1] > cardsPickratesMap[i][code2];
            });
        }
    }

    emit pDebug("cardsByPickRate map created.");
}


void PopularCardsHandler::newTurn(bool isPlayerTurn, int numTurn)
{
    if(!isPlayerTurn)   return;

    enemyMana = (numTurn + 1)/2;

    //El rival tendra uno mas de mana si tiene la moneda o es el primer jugador
    //A veces la moneda se puede crear en la mano enemiga despues de pasar al primer turno
    if(enemyHandHandler->isCoinInHand() || numTurn%2==0 || numTurn==1)    enemyMana++;

    //Si pasamos del limite de 10 mana no queremos mostrar popular cards mas
    if(enemyMana <= 10) enemyMana -= enemyOverloadOwed;

    showPopularCards();
}


void PopularCardsHandler::showPopularCards()
{
    if(!patreonVersion || popularCardsShown == 0 || !inArena ||
            enemyMana < 2 || enemyMana > 10 || enemyClass < 0 || enemyClass > 8)
    {
        clearAndHide();
        return;
    }

    ui->popularCardsListWidget->clear();
    popularCardList.clear();

    for(QString &code: cardsByPickrate[enemyClass][enemyMana-2])
    {
        QString manaText = QString::number((int)round(cardsPickratesMap[enemyClass][code]))+"%";
        PopularCard popularCard(code, manaText);
        popularCard.listItem = new QListWidgetItem(ui->popularCardsListWidget);
        popularCard.draw();
        popularCardList.append(popularCard);
        emit checkCardImage(code);

        if(popularCardsShown < 11 && popularCardList.count() == popularCardsShown)    break;
    }

    ui->popularCardsListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustSize()));
}


void PopularCardsHandler::setEnemyClass(QString hero)
{
    this->enemyClass = Utility::heroFromLogNumber(hero);
}


void PopularCardsHandler::setPopularCardsShown(int value)
{
    this->popularCardsShown = value;
    showPopularCards();
}


void PopularCardsHandler::setPremium(bool premium)
{
    this->patreonVersion = premium;
}


void PopularCardsHandler::enemyTagChange(QString tag, QString value)
{
    if(tag == "OVERLOAD_OWED")
    {
        this->enemyOverloadOwed = value.toInt();
    }
}


void PopularCardsHandler::enterArena()
{
    this->inArena = true;
}


void PopularCardsHandler::leaveArena()
{
    this->inArena = false;
}
