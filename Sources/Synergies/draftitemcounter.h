#ifndef DRAFTITEMCOUNTER_H
#define DRAFTITEMCOUNTER_H

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
#include "../Widgets/hoverlabel.h"
#include "../utility.h"
#include "../Cards/synergycard.h"

class DraftItemCounter : public QObject
{
    Q_OBJECT
public:
    DraftItemCounter();
    DraftItemCounter(QObject *parent, QString synergyTag, QString synergyTagExtra,
                     QGridLayout *gridLayout, int gridRow, int gridCol, QPixmap pixmap,
                     int iconWidth, bool iconHover=true, bool inDraftMechanicsWindow=true);
    DraftItemCounter(QObject *parent, QString synergyTag);
    ~DraftItemCounter();

//Variables
private:
    QLabel *labelCounter;
    int counter;
    QHBoxLayout *horLayout;
    QList<SynergyCard> synergyCardList, extraCardList;
    QMap<QString, int> codeMap, codeSynMap;
    QString synergyTag, synergyTagExtra;

protected:
    HoverLabel *labelIcon;

//Metodos
private:
    void init(QHBoxLayout *hLayout, bool iconHover);
    void setIcon(QPixmap &pixmap, int iconWidth=32);

public:
    void reset();
    QString getSynergyTag();
    void setTransparency(Transparency transparency, bool mouseInApp);
    void increaseExtra(const QString &code, const QString &synergyTagExc="");
    void increase(int numIncrease, int draftedCardsCount);
    void increase(const QString &code);
    bool insertCode(const QString code, QMap<QString, int> &synergies);
    void insertCards(QMap<QString, QMap<QString, int> > &synergyTagMap, QString avoidCode="");
    void increaseSyn(const QString &code);
    void insertSynCards(QMap<QString, QMap<QString, int> > &synergyTagMap);
    int count();
    QMap<QString, int> &getCodeMap();
    QMap<QString, int> &getCodeSynMap();
    QMap<QString, QString> getCodeTagMap();
    void setTheme(QPixmap pixmap, int iconWidth, bool inDraftMechanicsWindow);
    void hide();
    void show();
    void moveLayout(QGridLayout *gridLayout, int gridRow, int gridCol);

signals:
    void iconEnter(QList<SynergyCard> &synergyCardList, QRect &labelRect);
    void iconLeave();

private slots:
    void sendIconEnter();
};

#endif // DRAFTITEMCOUNTER_H
