#ifndef CARDGRAPHICSITEM_H
#define CARDGRAPHICSITEM_H

#include <QGraphicsItem>
#include "graphicsitemsender.h"
#include "miniongraphicsitem.h"

#define CARD_LIFT 10


class CardGraphicsItem : public QGraphicsItem
{
friend class MinionGraphicsItem;

//Constructor
public:
    CardGraphicsItem(int id, QString code, QString createdByCode, int turn, bool friendly, GraphicsItemSender *graphicsItemSender, QStringList mechanics);
    CardGraphicsItem(CardGraphicsItem *copy, bool showMechanics);

//Variables
private:
    QString code, createdByCode;
    QStringList mechanics;
    int id, turn;
    bool played, discard, draw;
    int origAttack, origHealth;
    int attack, health;
    int origCost, cost;
    int buffAttack, buffHealth;
    int heightShow;
    bool friendly;
    bool showTransparent;
    GraphicsItemSender *graphicsItemSender;

public:
    static const int WIDTH = 182;
    static const int HEIGHT = 254;

//Metodos
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void setZonePos(bool friendly, int pos, int cardsZone, int viewWidth, int cardHeightShow);
    int getId();
    void setPlayed(bool played=true);
    void checkDownloadedCode(QString code);
    void setDiscard();
    void setDraw(bool drawn=true);
    bool isDiscard();
    bool isPlayed();
    bool isDraw();
    void reduceCost(int cost);
    void togglePlayed();
    void processTagChange(QString tag, QString value);
    void addBuff(int addAttack, int addHealth);
    QString getCode();
    QString getCreatedByCode();
    void showManaPlayable(int mana);
    int getManaSpent(bool includeRefresh);
    void setShowTransparent(bool value=true);
    bool isTransparent();
    void changeCode(QString newCode);
};

#endif // CARDGRAPHICSITEM_H
