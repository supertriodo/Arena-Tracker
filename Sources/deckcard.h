#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>

#define GREEN QColor(200,250,200)
#define LIMEGREEN QColor(50,205,50)
#define RED QColor(255,0,0)
#define YELLOW QColor(255,255,0)
#define WHITE QColor(255,255,255)
#define BLACK QColor(0,0,0)
#define BLUE QColor(0,191,255)
#define VIOLET QColor(186,85,211)
#define ORANGE QColor(255,165,0)
#define TRANSPARENT QColor(0,0,0,0)
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
    QPixmap draw(QString code, uint total, bool drawRarity=false, QColor nameColor=BLACK, int cardHeight=35);

public:
    void draw(bool drawTotal=true, int cardHeight=35);
    void drawGreyed(bool drawTotal, int cardHeight=35);

    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
};

#endif // DECKCARD_H
