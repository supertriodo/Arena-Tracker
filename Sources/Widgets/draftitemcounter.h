#ifndef DRAFTITEMCOUNTER_H
#define DRAFTITEMCOUNTER_H

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
#include "../utility.h"

class DraftItemCounter : public QObject
{
    Q_OBJECT
public:
    DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap);

//Variables
private:
    QLabel labelIcon, labelCounter;
    int counter;
    QList<DeckCard> deckCardList, deckCardListSyn;

//Metodos
public:
    void reset();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void increase(const QString &code);
    bool isHidden();
    void insertCards(QMap<QString, int> &synergies);
    void increaseSyn(const QString &code);
    void insertSynCards(QMap<QString, int> &synergies);

signals:

public slots:
};

#endif // DRAFTITEMCOUNTER_H
