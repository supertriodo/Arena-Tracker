#ifndef HEROPOWERGRAPHICSITEM_H
#define HEROPOWERGRAPHICSITEM_H

#include <QGraphicsItem>
#include "graphicsitemsender.h"

class HeroPowerGraphicsItem : public QGraphicsItem
{
public:
//Constructor
    HeroPowerGraphicsItem(QString code, int id, bool friendly, bool playerTurn, GraphicsItemSender *graphicsItemSender);
    HeroPowerGraphicsItem(HeroPowerGraphicsItem *copy);

//Variables
private:
    QString code;
    int id, cost;
    bool exausted;
    bool friendly, playerTurn;
    bool showTransparent;
    GraphicsItemSender *graphicsItemSender;

public:
    static const int WIDTH = 140;
    static const int HEIGHT = 134;

//Metodos
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void checkDownloadedCode(QString code);
    void changeHeroPower(QString code, int id);
    void processTagChange(QString tag, QString value);
    int getId();
    void setPlayerTurn(bool playerTurn);
    void toggleExausted();
    bool isExausted();
    QString getCode();
    int getCost();
    void showManaPlayable(int mana);
    void setShowTransparent(bool value=true);
    bool isTransparent();
};

#endif // HEROPOWERGRAPHICSITEM_H
