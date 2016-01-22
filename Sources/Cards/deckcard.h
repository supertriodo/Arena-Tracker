#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>

#define GREEN QColor(50,175,50)
#define RED QColor(255,0,0)
#define YELLOW QColor(255,255,0)
#define WHITE QColor(255,255,255)
#define BLACK QColor(0,0,0)
#define BLUE QColor(0,191,255)
#define VIOLET QColor(186,85,211)
#define ORANGE QColor(255,165,0)
#define TRANSPARENT QColor(0,0,0,0)
#define CARD_SIZE QSize(218,35)


enum CardRarity {INVALID_RARITY, COMMON, RARE, EPIC, LEGENDARY};
enum CardType {INVALID_TYPE, HERO, MINION, SPELL, ENCHANTMENT, WEAPON, HERO_POWER};
enum CardClass {DRUID, HUNTER, MAGE, PALADIN, PRIEST, ROGUE, SHAMAN, WARLOCK, WARRIOR, DREAM, INVALID_CLASS, NEUTRAL};


class DeckCard
{
public:
    DeckCard(QString code);
    ~DeckCard();

//Variables
public:
    QListWidgetItem *listItem;
    uint total;
    uint remaining;

protected:
    QString code, name;
    CardRarity rarity;
    CardType type;
    CardClass cardClass;
    int cost;

    static bool drawClassColor, drawSpellWeaponColor;
    static int cardHeight;
    static QMap<QString, QJsonObject> *cardsJson;

//Metodos
protected:
    QPixmap draw(uint total, bool drawRarity=false, QColor nameColor=BLACK, bool resize=true);
    QColor getRarityColor();
    CardRarity getRarityFromString(QString value);
    CardType getTypeFromString(QString value);
    CardClass getClassFromString(QString value);
    QString tooltip();
    QPixmap resizeCardHeight(QPixmap &canvas);

public:
    void draw(bool drawTotal=true);
    void drawGreyed(bool drawTotal);
    QString getCode();
    CardType getType();
    QString getName();
    CardRarity getRarity();
    CardClass getCardClass();
    int getCost();
    void setCode(QString code);

    static void setCardsJson(QMap<QString, QJsonObject> *cardsJson);
    static void setDrawClassColor(bool value);
    static void setDrawSpellWeaponColor(bool value);
    static void setCardHeight(int value);
};

#endif // DECKCARD_H
