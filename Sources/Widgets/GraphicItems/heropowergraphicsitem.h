#ifndef HEROPOWERGRAPHICSITEM_H
#define HEROPOWERGRAPHICSITEM_H

#include <QGraphicsItem>

class HeroPowerGraphicsItem : public QGraphicsItem
{
public:
//Constructor
    HeroPowerGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    HeroPowerGraphicsItem(HeroPowerGraphicsItem *copy);

//Variables
private:
    QString code;
    int id;
    bool exausted;
    bool friendly, playerTurn;

public:
    static const int WIDTH = 140;
    static const int HEIGHT = 134;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    void checkDownloadedCode(QString code);
    void changeHeroPower(QString code, int id);
    void processTagChange(QString tag, QString value);
    int getId();
    void setPlayerTurn(bool playerTurn);
};

#endif // HEROPOWERGRAPHICSITEM_H
