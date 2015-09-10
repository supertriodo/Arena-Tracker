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
#define TRANSPARENT QColor(Qt::transparent)
#define CARD_SIZE QSize(218,35)

class DeckCard
{
public:
    DeckCard(QString code);
    ~DeckCard();

//Variables
public:
    QListWidgetItem *listItem;
    QString code;
    int cost;
    uint total;
    uint remaining;

private:
    static QMap<QString, QJsonObject> *cardsJson;

//Metodos
private:
    QColor getRarityColor(QString code);

protected:
    QPixmap draw(QString code, uint total, bool drawRarity=false);

public:
    void draw(bool drawTotal);
    void draw();

    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
};

#endif // DECKCARD_H
