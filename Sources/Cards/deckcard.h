#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>
#include "../constants.h"


#define CARD_SIZE QSize(218,35)
#define DISABLE_OPACITY 150


//Usamos sus numeros para comparacion con rarity template en DraftHandler::reviewBestCards()
enum CardRarity {COMMON=0, RARE=1, EPIC=2, LEGENDARY=3, FREE, INVALID_RARITY};
//Nuevos CardType revisar SynergyHandler::updateCardTypeCounters
enum CardType {INVALID_TYPE, HERO, MINION, SPELL, WEAPON, HERO_POWER, LOCATION};
enum CardRace {INVALID_RACE, BLOODELF, DRAENEI, DWARF, GNOME, GOBLIN, HUMAN, NIGHTELF, ORC, TAUREN, TROLL, UNDEAD,
               WORGEN, GOBLIN2, MURLOC, DEMON, SCOURGE, MECHANICAL, ELEMENTAL, OGRE, BEAST, TOTEM, NERUBIAN, PIRATE,
               DRAGON, BLANK, ALL, EGG, QUILBOAR, CENTAUR, FURBOLG, HIGHELF, TREANT, OWLKIN, HALFORC, LOCK, NAGA, OLDGOD,
               PANDAREN, GRONN};
enum CardSchool {INVALID_SCHOOL, NONE, ARCANE, FEL, FIRE, FROST, HOLY, SHADOW, NATURE, PHYSICAL_COMBAT};
//--------------------------------------------------------
//----NEW HERO CLASS
//--------------------------------------------------------
enum CardClass {DEATHKNIGHT, DEMONHUNTER, DRUID, HUNTER, MAGE, PALADIN, PRIEST, ROGUE, SHAMAN, WARLOCK, WARRIOR,
                NUM_HEROS, INVALID_CLASS, NEUTRAL};


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
    QList<CardRace> cardRace;
    CardSchool cardSchool;
    int cost;
    QString createdByCode;

    static bool drawClassColor, drawSpellWeaponColor;
    static int cardHeight;

private:
    bool topManaLimit, bottomManaLimit;
    bool outsider;

//Metodos
protected:
    QPixmap draw(int total, bool drawRarity, QColor nameColor=BLACK, QString manaText="", int cardWidth=0, QStringList mechanics={});
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
    QList<CardRace> getRace();
    CardSchool getSchool();
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
