#ifndef ENEMYRANKING_H
#define ENEMYRANKING_H

#include <QObject>
#include <QLabel>


class EnemyRankingItem
{
public:
    EnemyRankingItem(int rank, float rating, QString region, bool searchingTag = true)
    {
        this->rank = rank;
        this->rating = rating;
        this->region = region;
        this->searchingTag = searchingTag;
    }

    int rank;
    float rating;
    QString region;
    bool searchingTag = true;
};


class EnemyRanking : public QLabel
{
    Q_OBJECT

//Constructor
public:
    EnemyRanking(QWidget * parent);

//Variables
private:
    QString tag;
    QList<EnemyRankingItem> enemyRankingItems;

//Metodos
private:
    void drawPixmap(QPixmap &canvas, QRect &targetAll);
    void draw();
    void showEnemyRanking(int endHeight);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    void setEnemyRanking(const QString &tag, QList<EnemyRankingItem> enemyRankingItems);
    void updateRankingItem(const QString &tag, EnemyRankingItem enemyRankingItem);
    void hideEnemyRanking();
    QString getTag();
};

#endif // ENEMYRANKING_H

