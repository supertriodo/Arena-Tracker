#ifndef CARDGRAPHICSITEM_H
#define CARDGRAPHICSITEM_H

#include <QGraphicsItem>

#define CARD_LIFT 10


class CardGraphicsItem : public QGraphicsItem
{
//Constructor
public:
    CardGraphicsItem(int id, QString code, QString createdByCode, int turn, bool friendly);
    CardGraphicsItem(CardGraphicsItem *copy);

//Variables
private:
    QString code, createdByCode;
    int id, turn;
    bool played, discard, draw;
    int heightShow;

public:
    static const int WIDTH = 182;
    static const int HEIGHT = 254;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void setZonePos(bool friendly, int pos, int cardsZone, int viewWidth, int cardHeightShow);
    int getId();
    void setPlayed(bool played=true);
    void checkDownloadedCode(QString code);
    void setCode(QString code);
    void setDiscard();
    void setDraw();
};

#endif // CARDGRAPHICSITEM_H
