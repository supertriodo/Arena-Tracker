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
    QLabel *labelCounter;
    int counter;
    QList<DeckCard> deckCardList;
    QMap<QString, int> codeMap, codeSynMap;

protected:
    HoverLabel *labelIcon;

//Metodos
private:
    void init(QHBoxLayout *hLayout, bool iconHover);
    void setIcon(QPixmap &pixmap, int iconWidth=32);

public:
    void reset();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void increase(int numIncrease, int draftedCardsCount);
    void increase(const QString &code);
    bool insertCode(const QString code, QMap<QString, int> &synergies);
    void insertCards(QMap<QString, int> &synergies);
    void increaseSyn(const QString &code);
    void insertSynCards(QMap<QString, int> &synergies);
    int count();
    QMap<QString, int> &getCodeMap();
    void setTheme(QPixmap pixmap, int iconWidth=32, bool inDraftMechanicsWindow=false);
    void hide();
    void show();

signals:
    void iconEnter(QList<DeckCard> &deckCardList, QRect &labelRect);
    void iconLeave();

public slots:
private slots:
    void sendIconEnter();
};

#endif // DRAFTITEMCOUNTER_H
