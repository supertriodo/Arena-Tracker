#ifndef CARDGRAPHICSITEM_H
#define CARDGRAPHICSITEM_H

#include <QGraphicsItem>


class CardGraphicsItem : public QGraphicsItem
{
//Constructor
public:
    CardGraphicsItem(QString code, int id);
    CardGraphicsItem(CardGraphicsItem *copy);

//Variables
private:
    QString code;
    int id;
    bool played;

public:
    static const int WIDTH = 182;
    static const int HEIGHT = 254;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void setZonePos(bool friendly, int pos, int cardsZone, int viewWidth);
    int getId();
    void setPlayed();
    void checkDownloadedCode(QString code);
};

#endif // CARDGRAPHICSITEM_H
