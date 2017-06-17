#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>
#include "../constants.h"


#define CARD_SIZE QSize(218,35)


enum CardRarity {INVALID_RARITY, COMMON, RARE, EPIC, LEGENDARY};
enum CardType {INVALID_TYPE, HERO, MINION, SPELL, ENCHANTMENT, WEAPON, HERO_POWER};
enum CardClass {DRUID, HUNTER, MAGE, PALADIN, PRIEST, ROGUE, SHAMAN, WARLOCK, WARRIOR, DREAM, INVALID_CLASS, NEUTRAL};
enum CardRace {INVALID_RACE, BLOODELF, DRAENEI, DWARF, GNOME, GOBLIN, HUMAN, NIGHTELF, ORC, TAUREN, TROLL, UNDEAD,
               WORGEN, GOBLIN2, MURLOC, DEMON, SCOURGE, MECHANICAL, ELEMENTAL, OGRE, BEAST, TOTEM, NERUBIAN, PIRATE, DRAGON};


class DeckCard
{
public:
    DeckCard(QString code, bool outsider=false);
    ~DeckCard();

//Variables
public:
    QListWidgetItem *listItem;
    uint total;
    uint remaining;
    bool special;
    int id;


protected:
    QString code, name;
    CardRarity rarity;
    CardType type;
    CardClass cardClass;
    CardRace cardRace;
    int cost;
    QString createdByCode;

    static bool drawClassColor, drawSpellWeaponColor;
    static int cardHeight;

private:
    bool topManaLimit, bottomManaLimit;
    bool outsider;

//Metodos
protected:
    QPixmap draw(uint total, bool drawRarity, QColor nameColor=BLACK, bool resize=true);
    QPixmap drawCustomCard(QString customCode, QString customText);
    QColor getRarityColor();
    CardRarity getRarityFromString(QString value);
    CardType getTypeFromString(QString value);
    CardClass getClassFromString(QString value);
    CardRace getRaceFromString(QString value);
    QPixmap resizeCardHeight(QPixmap &canvas);

public:
    void draw();
    QString getCode();
    CardType getType();
    QString getName();
    CardRarity getRarity();
    CardClass getCardClass();
    QString getHeroLogNumber();
    CardRace getRace();
    int getCost();
    void setCode(QString code);
    void setManaLimit(bool top);
    void resetManaLimits();
    bool isOutsider();
    void setCreatedByCode(QString code);
    QString getCreatedByCode();

    static void setDrawClassColor(bool value);
    static void setDrawSpellWeaponColor(bool value);
    static void setCardHeight(int value);
    static int getCardHeight();
    static int getCardWidth();
};

#endif // DECKCARD_H
