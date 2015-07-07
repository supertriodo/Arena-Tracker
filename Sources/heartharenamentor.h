#ifndef HEARTHARENAMENTOR_H
#define HEARTHARENAMENTOR_H

#include <QObject>
#include <QNetworkAccessManager>

class HearthArenaMentor : public QObject
{
    Q_OBJECT
public:
    HearthArenaMentor(QObject *parent);
    ~HearthArenaMentor();


//Variables
private:
    QNetworkAccessManager *networkManager;


//Metodos
private:
    QString heroToWebNumber(const QString &hero);

public:
    void askCardsRating(QString &hero, QList<int> &draftedCards, int cards[3]);

signals:
    void newTip(QString tip, double rating1, double rating2, double rating3,
                QString synergy1, QString synergy2, QString synergy3);
    void sendLog(QString line);

public slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // HEARTHARENAMENTOR_H
