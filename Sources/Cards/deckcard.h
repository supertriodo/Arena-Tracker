#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>
#include "../constants.h"


#define CARD_SIZE QSize(218,35)


enum CardRarity {INVALID_RARITY, COMMON, RARE, EPIC, LEGENDARY};
enum CardType {INVALID_TYPE, HERO, MINION, SPELL, ENCHANTMENT, WEAPON, HERO_POWER};
enum CardRace {INVALID_RACE, BLOODELF, DRAENEI, DWARF, GNOME, GOBLIN, HUMAN, NIGHTELF, ORC, TAUREN, TROLL, UNDEAD,
               WORGEN, GOBLIN2, MURLOC, DEMON, SCOURGE, MECHANICAL, ELEMENTAL, OGRE, BEAST, TOTEM, NERUBIAN, PIRATE,
               DRAGON, BLANK, ALL, EGG};
//--------------------------------------------------------
//----NEW HERO CLASS
//--------------------------------------------------------
enum CardClass {DEMONHUNTER, DRUID, HUNTER, MAGE, PALADIN, PRIEST, ROGUE, SHAMAN, WARLOCK, WARRIOR, INVALID_CLASS, NEUTRAL};


class DeckCard
{
public:
    DeckCard(QString code, bool outsider=false);
    ~DeckCard();

//Variables
public:
    QListWidgetItem *listItem;
    int total;
    int remaining;
    bool special;
    int id;
    QList<int> outsiderIds;


protected:
    QString code, name;
    CardRarity rarity;
    CardType type;
    QList<CardClass> cardClass;
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
    QPixmap draw(int total, bool drawRarity, QColor nameColor=BLACK, bool resize=true, QString manaText="");
    QPixmap drawCustomCard(QString customCode, QString customText);
    QColor getRarityColor();
    QPixmap resizeCardHeight(QPixmap &canvas);
    void disablePixmap(QPixmap &canvas);

public:
    void draw();
    QString getCode();
    CardType getType();
    QString getName();
    CardRarity getRarity();
    QList<CardClass> getCardClass();
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
