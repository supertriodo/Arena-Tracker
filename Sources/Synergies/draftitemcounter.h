#ifndef DRAFTITEMCOUNTER_H
#define DRAFTITEMCOUNTER_H

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
#include "../Widgets/hoverlabel.h"
#include "../utility.h"

class DraftItemCounter : public QObject
{
    Q_OBJECT
public:
    DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap, bool iconHover=true);
    DraftItemCounter(QObject *parent, QGridLayout *gridLayout, int gridRow, int gridCol, QPixmap pixmap, int iconWidth, bool iconHover=true);
    DraftItemCounter(QObject *parent);
    ~DraftItemCounter();

//Variables
private:
    HoverLabel *labelIcon;
    QLabel *labelCounter;
    int counter;
    QList<DeckCard> deckCardList, deckCardListSyn;

//Metodos
private:
    void init(QHBoxLayout *hLayout, bool iconHover);

public:
    void reset();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void increase(int numIncrease, int draftedCardsCount);
    void increase(const QString &code, bool count=true);
    bool isEmpty();
    bool insertCode(const QString code, QMap<QString, int> &synergies);
    void insertCards(QMap<QString, int> &synergies);
    void increaseSyn(const QString &code);
    void insertSynCards(QMap<QString, int> &synergies);
    int count();
    QList<DeckCard> getDeckCardList();
    void setTheme(QPixmap pixmap, int iconWidth=32, bool inDraftMechanicsWindow=false);
    void setIcon(QPixmap pixmap, int iconWidth=32);

signals:
    void iconEnter(QList<DeckCard> &deckCardList, QRect &labelRect);
    void iconLeave();

public slots:
private slots:
    void sendIconEnter();
};

#endif // DRAFTITEMCOUNTER_H
