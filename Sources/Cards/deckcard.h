#ifndef DECKCARD_H
#define DECKCARD_H

#include <QListWidgetItem>
#include <QString>
#include <QMap>
#include "../constants.h"
#include "../utility.h"


#define CARD_SIZE QSize(218,35)
#define DISABLE_OPACITY 150


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
    //Usados para pintar los score
    bool showScores, showHA, showHSR, showFire;
    bool badScoreHA, badScoreHSR, badScoreFire;
    bool redraftingReview;
    int scoreHA;
    float scoreHSR, scoreFire;
    int includedDecks, samplesFire;
    int classOrder;

//Metodos
private:
    void drawBadScore(QPainter &painter, QPixmap &canvas, ScoreSource scoreSource, int num);

protected:
    QPixmap draw(int total, bool drawRarity, QColor nameColor=BLACK, QString manaText="", int cardWidth=0, QStringList mechanics={});
    QPixmap drawCustomCard(QString customCode, QString customText);
    QColor getRarityColor();
    QPixmap resizeCardHeight(QPixmap &canvas);
    void disablePixmap(QPixmap &canvas);

public:
    void draw();
    bool isCode(const QString &code);
    QString getCode();
    CardType getType();
    QString getName();
    CardRarity getRarity();
    QList<CardClass> getCardClass();
    QList<CardRace> getRace();
    CardSchool getSchool();
    int getCost() const;
    void setCode(QString code);
    void setManaLimit(bool top);
    void resetManaLimits();
    bool isOutsider();
    void setCreatedByCode(QString code);
    QString getCreatedByCode();
    void setEachShowScores(bool showHA, bool showHSR, bool showFire, bool redraw);
    void hideScores();
    void setShowScores(bool showScores);
    void setScores(int haTier, float hsrWR, float fireWR, int classOrder, int includedDecks, int samplesFire);
    void setBadScoreHA(bool badScore=true);
    void setBadScoreHSR(bool badScore=true);
    void setBadScoreFire(bool badScore=true);
    void setRedraftingReview(bool show=true);
    bool operator<(const DeckCard &other) const;
    bool operator==(const DeckCard& other) const;
    bool operator!=(const DeckCard& other) const;
    bool operator>(const DeckCard& other) const;
    bool operator<=(const DeckCard& other) const;
    bool operator>=(const DeckCard& other) const;

    static void setDrawClassColor(bool value);
    static void setDrawSpellWeaponColor(bool value);
    static void setCardHeight(int value);
    static int getCardHeight();
    static int getCardWidth();
};

#endif // DECKCARD_H
