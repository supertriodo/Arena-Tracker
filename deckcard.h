#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>

#define GREEN QColor(200,250,200)
#define RED QColor(Qt::red)
#define YELLOW QColor(Qt::yellow)
#define WHITE QColor(Qt::white)
#define BLACK QColor(Qt::black)
#define CARD_SIZE QSize(218,35)

class DeckCard
{
public:
    DeckCard(QMap<QString, QJsonObject> *cardsJson);
    ~DeckCard();

//Variables
public:
    QListWidgetItem *listItem;
    QString code;
    int cost;
    uint total;
    uint remaining;

private:
    QMap<QString, QJsonObject> *cardsJson;

//Metodos
private:
    void draw(QListWidgetItem * item, QString code, uint total);

public:
    void draw(bool drawTotal);
    void draw();
};

#endif // DECKCARD_H
